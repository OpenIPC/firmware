/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
#include <stdarg.h>
#include <stdio.h>
#include "t23_ipa.h"

static bool log_to_syslog;

void ipa_log_open(bool use_syslog)
{
	if (use_syslog && !log_to_syslog) {
		openlog("t23-ipa", LOG_PID, LOG_DAEMON);
		log_to_syslog = true;
	}
}

void ipa_log_close(void)
{
	if (log_to_syslog) {
		closelog();
		log_to_syslog = false;
	}
}

void ipa_log(int priority, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if (log_to_syslog) {
		vsyslog(priority, fmt, ap);
	} else {
		FILE *out = priority <= LOG_WARNING ? stderr : stdout;

		fputs("ipa: ", out);
		vfprintf(out, fmt, ap);
	}
	va_end(ap);
}
