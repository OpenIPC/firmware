/*
 *
 * Copyright (c) OpenIPC  https://openipc.org  MIT License
 *
 * ntfy.c — lightweight ntfy.sh client for embedded Linux
 *
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>

#include <curl/curl.h>

/* ─── Constants ───────────────────────────────────────────────────── */

#define NTFY_VERSION     "1.0.0"
#define NTFY_DEFAULT_URL "https://ntfy.sh"
#define NTFY_MAX_URL     1024
#define NTFY_MAX_HDR     512
#define NTFY_LINE_BUF    8192
#define NTFY_JSON_VAL    4096
#define NTFY_STDIN_MAX   65536

/* ─── Logging ─────────────────────────────────────────────────────── */

#define log_info(fmt, ...) \
	do { printf(fmt "\n", ##__VA_ARGS__); \
	     syslog(LOG_INFO, fmt, ##__VA_ARGS__); } while (0)
#define log_err(fmt, ...) \
	do { fprintf(stderr, "ntfy: " fmt "\n", ##__VA_ARGS__); \
	     syslog(LOG_ERR, fmt, ##__VA_ARGS__); } while (0)

/* ─── Global shutdown flag ────────────────────────────────────────── */

static volatile sig_atomic_t g_running = 1;

static void sig_handler(int sig)
{
	(void)sig;
	g_running = 0;
}

/* ─── Minimal JSON helpers ────────────────────────────────────────── */

/*
 * Extract a JSON string value by key.  Handles escaped characters
 * inside values.  Returns 0 on success, -1 if key not found.
 *
 * Works only on flat JSON objects (no nesting) — sufficient for the
 * ntfy wire format.
 */
static int json_str(const char *json, const char *key,
		    char *out, size_t out_sz)
{
	char pat[128];
	const char *p;

	snprintf(pat, sizeof(pat), "\"%s\":\"", key);
	p = strstr(json, pat);
	if (!p) {
		snprintf(pat, sizeof(pat), "\"%s\": \"", key);
		p = strstr(json, pat);
		if (!p)
			return -1;
	}
	p += strlen(pat);

	size_t i = 0;
	while (*p && i < out_sz - 1) {
		if (*p == '\\' && *(p + 1)) {
			p++;
			switch (*p) {
			case '"':  out[i++] = '"';  break;
			case '\\': out[i++] = '\\'; break;
			case 'n':  out[i++] = '\n'; break;
			case 't':  out[i++] = '\t'; break;
			case '/':  out[i++] = '/';  break;
			default:   out[i++] = *p;   break;
			}
		} else if (*p == '"') {
			break;
		} else {
			out[i++] = *p;
		}
		p++;
	}
	out[i] = '\0';
	return 0;
}

/* Extract a JSON integer value by key.  Returns 0 on success. */
static int json_long(const char *json, const char *key, long *out)
{
	char pat[128];

	snprintf(pat, sizeof(pat), "\"%s\":", key);
	const char *p = strstr(json, pat);
	if (!p)
		return -1;
	p += strlen(pat);

	while (*p == ' ' || *p == '\t')
		p++;

	char *end;
	long val = strtol(p, &end, 10);
	if (end == p)
		return -1;

	*out = val;
	return 0;
}

/*
 * Extract a JSON array of strings as a comma-separated flat string.
 * e.g.  "tags":["warning","skull"]  →  "warning,skull"
 */
static int json_tags(const char *json, char *out, size_t out_sz)
{
	const char *p = strstr(json, "\"tags\":[");
	if (!p) {
		p = strstr(json, "\"tags\": [");
		if (!p)
			return -1;
	}

	p = strchr(p, '[');
	if (!p)
		return -1;
	p++;

	const char *end = strchr(p, ']');
	if (!end)
		return -1;

	size_t j = 0;
	int in_str = 0;

	for (; p < end && j < out_sz - 1; p++) {
		if (*p == '"') {
			in_str = !in_str;
			continue;
		}
		if (in_str) {
			out[j++] = *p;
		} else if (*p == ',') {
			out[j++] = ',';
		}
	}

	out[j] = '\0';
	return 0;
}

/* ─── Stream context for subscribe ────────────────────────────────── */

typedef struct {
	char   buf[NTFY_LINE_BUF];
	size_t len;
	int    quiet;
} ntfy_stream_t;

/* Format and print a single ntfy JSON message line. */
static void process_line(const char *line, int quiet)
{
	char event[64] = {0};

	json_str(line, "event", event, sizeof(event));

	if (strcmp(event, "message") != 0)
		return;

	char msg[NTFY_JSON_VAL] = {0};

	json_str(line, "message", msg, sizeof(msg));

	if (quiet) {
		printf("%s\n", msg);
		fflush(stdout);
		return;
	}

	char title[256] = {0};
	char topic[256] = {0};
	char tags[512]  = {0};
	long ts   = 0;
	long prio = 0;

	json_str(line, "title", title, sizeof(title));
	json_str(line, "topic", topic, sizeof(topic));
	json_long(line, "time", &ts);
	json_long(line, "priority", &prio);
	json_tags(line, tags, sizeof(tags));

	/* Format timestamp */
	char timebuf[20] = "?";
	if (ts > 0) {
		time_t t = (time_t)ts;
		struct tm *tm = localtime(&t);
		if (tm)
			strftime(timebuf, sizeof(timebuf),
				 "%Y-%m-%d %H:%M:%S", tm);
	}

	const char *prio_str = "";
	if      (prio == 5) prio_str = " [URGENT]";
	else if (prio == 4) prio_str = " [HIGH]";
	else if (prio == 2) prio_str = " [LOW]";
	else if (prio == 1) prio_str = " [MIN]";

	if (title[0])
		printf("[%s] %s%s %s: %s\n",
		       timebuf, topic, prio_str, title, msg);
	else
		printf("[%s] %s%s %s\n",
		       timebuf, topic, prio_str, msg);

	if (tags[0])
		printf("  tags: %s\n", tags);

	fflush(stdout);
}

/* libcurl write callback: buffers data, processes complete lines. */
static size_t stream_cb(char *data, size_t size, size_t nmemb,
			void *userdata)
{
	ntfy_stream_t *ctx = userdata;
	size_t total = size * nmemb;

	for (size_t i = 0; i < total; i++) {
		if (data[i] == '\n') {
			ctx->buf[ctx->len] = '\0';
			if (ctx->len > 0)
				process_line(ctx->buf, ctx->quiet);
			ctx->len = 0;
		} else if (ctx->len < NTFY_LINE_BUF - 1) {
			ctx->buf[ctx->len++] = data[i];
		}
	}

	if (!g_running)
		return 0;

	return total;
}

/* libcurl progress callback: abort transfer on shutdown signal. */
static int progress_cb(void *p, curl_off_t dt, curl_off_t dn,
		       curl_off_t ut, curl_off_t un)
{
	(void)p; (void)dt; (void)dn; (void)ut; (void)un;
	return g_running ? 0 : 1;
}

/* ─── Publish response buffer ─────────────────────────────────────── */

typedef struct {
	char   buf[4096];
	size_t len;
} ntfy_resp_t;

static size_t resp_cb(char *data, size_t size, size_t nmemb,
		      void *userdata)
{
	ntfy_resp_t *ctx = userdata;
	size_t total = size * nmemb;
	size_t space = sizeof(ctx->buf) - ctx->len - 1;
	size_t copy  = total < space ? total : space;

	memcpy(ctx->buf + ctx->len, data, copy);
	ctx->len += copy;
	ctx->buf[ctx->len] = '\0';
	return total;
}

/* ─── Helper: join argv words into a single string ────────────────── */

static char *join_args(char **argv, int start, int end)
{
	size_t total = 0;
	for (int i = start; i < end; i++)
		total += strlen(argv[i]) + 1;

	char *buf = malloc(total + 1);
	if (!buf)
		return NULL;

	size_t pos = 0;
	for (int i = start; i < end; i++) {
		if (i > start)
			buf[pos++] = ' ';
		size_t len = strlen(argv[i]);
		memcpy(buf + pos, argv[i], len);
		pos += len;
	}
	buf[pos] = '\0';
	return buf;
}

/* ─── Helper: read message body from stdin ────────────────────────── */

static char *read_stdin(void)
{
	char  *buf = NULL;
	size_t cap = 0;
	size_t len = 0;
	char   tmp[1024];
	size_t n;

	while ((n = fread(tmp, 1, sizeof(tmp), stdin)) > 0) {
		if (len + n + 1 > cap) {
			cap = (len + n + 1) * 2;
			if (cap > NTFY_STDIN_MAX)
				cap = NTFY_STDIN_MAX;
			char *nb = realloc(buf, cap);
			if (!nb) {
				free(buf);
				return NULL;
			}
			buf = nb;
		}
		memcpy(buf + len, tmp, n);
		len += n;
		if (len >= NTFY_STDIN_MAX)
			break;
	}

	if (buf)
		buf[len] = '\0';
	return buf;
}

/* ─── Helper: setup common curl auth headers ──────────────────────── */

static struct curl_slist *auth_headers(struct curl_slist *headers,
				       CURL *curl,
				       const char *token,
				       const char *userpass)
{
	if (token && token[0]) {
		char hdr[NTFY_MAX_HDR];
		snprintf(hdr, sizeof(hdr), "Authorization: Bearer %s", token);
		headers = curl_slist_append(headers, hdr);
	}
	if (userpass && userpass[0])
		curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);

	return headers;
}

/* ─── Command: publish ────────────────────────────────────────────── */

static int cmd_publish(const char *server, const char *topic,
		       const char *message, const char *title,
		       const char *priority, const char *tags,
		       const char *token, const char *userpass)
{
	char url[NTFY_MAX_URL];
	snprintf(url, sizeof(url), "%s/%s", server, topic);

	CURL *curl = curl_easy_init();
	if (!curl) {
		log_err("curl_easy_init failed");
		return 1;
	}

	struct curl_slist *hdrs = NULL;
	char hdr[NTFY_MAX_HDR];

	if (title && title[0]) {
		snprintf(hdr, sizeof(hdr), "Title: %s", title);
		hdrs = curl_slist_append(hdrs, hdr);
	}
	if (priority && priority[0]) {
		snprintf(hdr, sizeof(hdr), "Priority: %s", priority);
		hdrs = curl_slist_append(hdrs, hdr);
	}
	if (tags && tags[0]) {
		snprintf(hdr, sizeof(hdr), "Tags: %s", tags);
		hdrs = curl_slist_append(hdrs, hdr);
	}

	hdrs = auth_headers(hdrs, curl, token, userpass);

	ntfy_resp_t resp;
	memset(&resp, 0, sizeof(resp));

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, resp_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "ntfy-c/" NTFY_VERSION);

	CURLcode res = curl_easy_perform(curl);
	int ret = 0;

	if (res != CURLE_OK) {
		log_err("publish failed: %s", curl_easy_strerror(res));
		ret = 1;
	} else {
		long code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
		if (code >= 200 && code < 300) {
			char id[64] = {0};
			json_str(resp.buf, "id", id, sizeof(id));
			log_info("published to %s (id: %s)",
				 topic, id[0] ? id : "?");
		} else {
			log_err("server returned HTTP %ld", code);
			if (resp.len > 0)
				fprintf(stderr, "%s\n", resp.buf);
			ret = 1;
		}
	}

	curl_slist_free_all(hdrs);
	curl_easy_cleanup(curl);
	return ret;
}

/* ─── Command: subscribe / poll ───────────────────────────────────── */

static int cmd_subscribe(const char *server, const char *topic,
			 int poll_mode, const char *since,
			 const char *token, const char *userpass,
			 int quiet)
{
	char url[NTFY_MAX_URL];
	int  off;

	off = snprintf(url, sizeof(url), "%s/%s/json", server, topic);

	const char *sep = "?";

	if (poll_mode) {
		off += snprintf(url + off, sizeof(url) - off,
				"%spoll=1", sep);
		sep = "&";
	}
	if (since && since[0]) {
		snprintf(url + off, sizeof(url) - off,
			 "%ssince=%s", sep, since);
	}

	CURL *curl = curl_easy_init();
	if (!curl) {
		log_err("curl_easy_init failed");
		return 1;
	}

	struct curl_slist *hdrs = NULL;
	hdrs = auth_headers(hdrs, curl, token, userpass);

	ntfy_stream_t ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.quiet = quiet;

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stream_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ctx);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "ntfy-c/" NTFY_VERSION);

	if (poll_mode) {
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
	} else {
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_cb);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	}

	if (!quiet)
		fprintf(stderr, "ntfy: %s %s/%s\n",
			poll_mode ? "polling" : "subscribing to",
			server, topic);

	CURLcode res = curl_easy_perform(curl);
	int ret = 0;

	if (res != CURLE_OK && res != CURLE_ABORTED_BY_CALLBACK) {
		log_err("subscribe failed: %s", curl_easy_strerror(res));
		ret = 1;
	} else {
		long code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
		if (code >= 400) {
			log_err("server returned HTTP %ld", code);
			ret = 1;
		}
	}

	/* Flush any remaining partial line */
	if (ctx.len > 0) {
		ctx.buf[ctx.len] = '\0';
		process_line(ctx.buf, ctx.quiet);
	}

	curl_slist_free_all(hdrs);
	curl_easy_cleanup(curl);
	return ret;
}

/* ─── Usage ───────────────────────────────────────────────────────── */

static void usage(const char *prog)
{
	fprintf(stderr,
		"ntfy " NTFY_VERSION
		" — lightweight ntfy client for embedded Linux\n"
		"Part of the OpenIPC ecosystem — https://openipc.org\n\n"
		"Usage:\n"
		"  %s pub  [opts] <topic> <message...>  "
		"Publish a message\n"
		"  %s sub  [opts] <topic>               "
		"Subscribe (streaming)\n"
		"  %s poll [opts] <topic>               "
		"Poll cached messages\n\n"
		"Options:\n"
		"  -s <url>        Server URL"
		" (default: " NTFY_DEFAULT_URL ")\n"
		"  -t <title>      Message title (pub)\n"
		"  -p <priority>   "
		"Priority: min,low,default,high,urgent (pub)\n"
		"  -g <tags>       Comma-separated tags (pub)\n"
		"  -T <token>      Bearer auth token\n"
		"  -u <user:pass>  Basic auth credentials\n"
		"  -S <since>      "
		"Fetch since: duration/timestamp/id/all (sub/poll)\n"
		"  -q              Quiet — message body only\n"
		"  -h              Show this help\n\n"
		"Examples:\n"
		"  %s pub alerts \"Disk full\"\n"
		"  %s pub -t Warning -p high alerts \"CPU 98%%\"\n"
		"  %s sub alerts\n"
		"  %s poll -S 30m alerts\n"
		"  echo \"Backup done\" | %s pub mytopic\n",
		prog, prog, prog, prog, prog, prog, prog, prog);
}

/* ─── Main ────────────────────────────────────────────────────────── */

int main(int argc, char *argv[])
{
	const char *prog = argv[0];

	if (argc < 2) {
		usage(prog);
		return 1;
	}

	/* Sub-command */
	const char *cmd = argv[1];

	if (strcmp(cmd, "-h") == 0 || strcmp(cmd, "--help") == 0) {
		usage(prog);
		return 0;
	}

	int mode;     /* 0 = pub, 1 = sub, 2 = poll */

	if      (strcmp(cmd, "pub")  == 0 || strcmp(cmd, "publish")   == 0)
		mode = 0;
	else if (strcmp(cmd, "sub")  == 0 || strcmp(cmd, "subscribe") == 0)
		mode = 1;
	else if (strcmp(cmd, "poll") == 0)
		mode = 2;
	else {
		fprintf(stderr, "ntfy: unknown command '%s'\n", cmd);
		usage(prog);
		return 1;
	}

	/*
	 * Shift argv so getopt sees the sub-command args.
	 * argv[1] = argv[0] preserves the program name for error messages.
	 */
	argv[1] = argv[0];
	argc--;
	argv++;

	const char *server   = NTFY_DEFAULT_URL;
	const char *title    = NULL;
	const char *priority = NULL;
	const char *tags     = NULL;
	const char *token    = NULL;
	const char *userpass = NULL;
	const char *since    = NULL;
	int         quiet    = 0;

	optind = 1;
	int opt;

	while ((opt = getopt(argc, argv, "s:t:p:g:T:u:S:qh")) != -1) {
		switch (opt) {
		case 's': server   = optarg; break;
		case 't': title    = optarg; break;
		case 'p': priority = optarg; break;
		case 'g': tags     = optarg; break;
		case 'T': token    = optarg; break;
		case 'u': userpass = optarg; break;
		case 'S': since    = optarg; break;
		case 'q': quiet    = 1;      break;
		case 'h': usage(prog); return 0;
		default:  usage(prog); return 1;
		}
	}

	int remaining = argc - optind;

	openlog("ntfy", LOG_PID | LOG_CONS, LOG_DAEMON);

	/* Signal handlers for clean shutdown */
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_handler;
	sigaction(SIGINT,  &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	curl_global_init(CURL_GLOBAL_DEFAULT);

	int ret = 1;

	if (mode == 0) {
		/* ── Publish ── */
		if (remaining < 1) {
			fprintf(stderr, "ntfy: pub requires <topic>\n");
			goto cleanup;
		}

		const char *topic   = argv[optind];
		const char *message = NULL;
		char *heap_msg      = NULL;

		if (remaining >= 2) {
			heap_msg = join_args(argv, optind + 1, argc);
			message  = heap_msg;
		} else if (!isatty(STDIN_FILENO)) {
			heap_msg = read_stdin();
			message  = heap_msg;
		}

		if (!message || !message[0]) {
			fprintf(stderr,
				"ntfy: pub requires a message "
				"(argument or stdin)\n");
			free(heap_msg);
			goto cleanup;
		}

		ret = cmd_publish(server, topic, message,
				  title, priority, tags,
				  token, userpass);
		free(heap_msg);
	} else {
		/* ── Subscribe / Poll ── */
		if (remaining < 1) {
			fprintf(stderr, "ntfy: %s requires <topic>\n",
				mode == 1 ? "sub" : "poll");
			goto cleanup;
		}

		ret = cmd_subscribe(server, argv[optind],
				    mode == 2, since,
				    token, userpass, quiet);
	}

cleanup:
	curl_global_cleanup();
	closelog();
	return ret;
}
