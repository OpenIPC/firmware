/*
 * libat.so for the SigmaStar infinity3 (MSC313E) SDK blobs.
 *
 * The vendor libat.so is Android's property client and liblog, served by the
 * vendor's Android-style init (minit). The blobs only need its ABI: libmi and
 * libOMX_BELA dlopen("libat.so") for property_get/property_set, libCam* link
 * it for those and __android_log_print. Here properties are plain settings:
 * read once per process from /etc/sigmastar.conf (key=value lines, '#'
 * comments), and property_set only changes the calling process's copy, which
 * is all the blobs use it for -- the whole pipeline runs in one process.
 */
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#ifndef CONFIG_FILE
#define CONFIG_FILE "/etc/sigmastar.conf"
#endif
#define PROP_NAME_MAX 32
#define PROP_VALUE_MAX 92

struct prop {
	char name[PROP_NAME_MAX];
	char value[PROP_VALUE_MAX];
};

static struct prop *props;
static int count, capacity;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t once = PTHREAD_ONCE_INIT;

static struct prop *find(const char *name)
{
	int i;

	for (i = 0; i < count; i++)
		if (!strcmp(props[i].name, name))
			return &props[i];
	return NULL;
}

/* Called with lock held, or from load() before anyone else can look. */
static int store(const char *name, const char *value)
{
	struct prop *p;

	if (strlen(name) >= PROP_NAME_MAX || strlen(value) >= PROP_VALUE_MAX)
		return -1;
	p = find(name);
	if (!p) {
		if (count == capacity) {
			p = realloc(props, (capacity + 32) * sizeof(*p));
			if (!p)
				return -1;
			props = p;
			capacity += 32;
		}
		p = &props[count++];
		strcpy(p->name, name);
	}
	strcpy(p->value, value);
	return 0;
}

static char *trim(char *s)
{
	char *end;

	while (*s == ' ' || *s == '\t')
		s++;
	end = s + strlen(s);
	while (end > s && strchr(" \t\r\n", end[-1]))
		*--end = '\0';
	return s;
}

static void load(void)
{
	char line[256], *eq;
	FILE *f = fopen(CONFIG_FILE, "r");

	if (!f)
		return;
	while (fgets(line, sizeof(line), f)) {
		eq = strchr(line, '=');
		if (line[0] == '#' || !eq)
			continue;
		*eq = '\0';
		store(trim(line), trim(eq + 1));
	}
	fclose(f);
}

/*
 * Like bionic, copies only the value and its NUL: callers size the buffer
 * for what they expect, not for PROP_VALUE_MAX.
 */
int property_get(const char *key, char *value, const char *default_value)
{
	struct prop *p;
	int len = 0;

	pthread_once(&once, load);
	pthread_mutex_lock(&lock);
	p = find(key);
	if (p)
		default_value = p->value;
	if (default_value) {
		len = strlen(default_value);
		memcpy(value, default_value, len + 1);
	} else {
		value[0] = '\0';
	}
	pthread_mutex_unlock(&lock);
	return len;
}

int property_set(const char *key, const char *value)
{
	int ret;

	if (!key)
		return -1;
	pthread_once(&once, load);
	pthread_mutex_lock(&lock);
	ret = store(key, value ? value : "");
	pthread_mutex_unlock(&lock);
	return ret;
}

/*
 * liblog. The vendor one writes to the Android logger ring; here the ring is
 * syslogd's (logread). Errors and fatals by default: the ISP logs some errors
 * per frame while it settles, and the ring takes them without touching the
 * console or flash. AT_LOG_LEVEL moves the threshold (2 verbose, 3 debug,
 * 4 info, 5 warn, 6 error, 7 fatal, 8 nothing).
 */
static int log_level(void)
{
	static int level;
	const char *env;

	if (!level) {
		env = getenv("AT_LOG_LEVEL");
		level = env ? atoi(env) : 6;
	}
	return level;
}

int __android_log_write(int prio, const char *tag, const char *msg)
{
	static const int to_syslog[] = {
		LOG_DEBUG, LOG_DEBUG, LOG_DEBUG, LOG_DEBUG,	/* ?, ?, V, D */
		LOG_INFO, LOG_WARNING, LOG_ERR, LOG_CRIT,	/* I, W, E, F */
	};

	if (prio < log_level())
		return 0;
	syslog(prio >= 0 && prio < 8 ? to_syslog[prio] : LOG_ERR, "%s: %s",
	       tag ? tag : "", msg);
	return 1;
}

int __android_log_vprint(int prio, const char *tag, const char *fmt, va_list ap)
{
	char buf[1024];

	if (prio < log_level())
		return 0;
	vsnprintf(buf, sizeof(buf), fmt, ap);
	return __android_log_write(prio, tag, buf);
}

int __android_log_print(int prio, const char *tag, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = __android_log_vprint(prio, tag, fmt, ap);
	va_end(ap);
	return ret;
}
