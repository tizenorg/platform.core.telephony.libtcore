#define FEATURE_TLOG_DEBUG

#include <stdio.h>
#include <glib.h>

#include <tcore.h>
#include <log.h>

void tcore_log(enum tcore_log_type type, enum tcore_log_priority priority, const char *tag, const char *fmt, ...)
{
	va_list ap;
	char buf[1024];

	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);

	printf("%s: %s", tag, buf);
	fflush(stdout);
}
