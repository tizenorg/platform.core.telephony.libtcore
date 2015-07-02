/*
 * libtcore
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Ja-young Gu <jygu@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TCORE_LOG_H__
#define __TCORE_LOG_H__

__BEGIN_DECLS

#include <glib.h>
extern gboolean tcore_debug;

#ifdef FEATURE_DLOG_DEBUG

#include <dlog.h>

#ifndef TCORE_LOG_TAG
#define TCORE_LOG_TAG "UNKNOWN"
#endif

#define info(fmt,args...)  do { if(tcore_debug) RLOG(LOG_INFO, TCORE_LOG_TAG, fmt "\n", ##args); } while (0)
#define msg(fmt,args...)  do { if(tcore_debug) RLOG(LOG_DEBUG, TCORE_LOG_TAG, fmt "\n", ##args); } while (0)
#define dbg(fmt,args...)  do { if(tcore_debug) RLOG(LOG_DEBUG, TCORE_LOG_TAG, fmt "\n", ##args); } while (0)
#define warn(fmt,args...)  do { RLOG(LOG_WARN, TCORE_LOG_TAG, fmt "\n", ##args); } while (0)
#define err(fmt,args...)  do { RLOG(LOG_ERROR, TCORE_LOG_TAG, fmt "\n", ##args); } while (0)
#define fatal(fmt,args...)  do { RLOG(LOG_FATAL, TCORE_LOG_TAG, fmt "\n", ##args); } while (0)

#elif defined(FEATURE_TLOG_DEBUG)

#ifndef TCORE_LOG_TAG
#define TCORE_LOG_TAG "UNKNOWN"
#endif

enum tcore_log_type {
	TCORE_LOG_TYPE_MAIN = 0,
	TCORE_LOG_TYPE_RADIO,
	TCORE_LOG_TYPE_SYSTEM,
	TCORE_LOG_TYPE_TIME_CHECK
};

enum tcore_log_priority {
	TCORE_LOG_UNKNOWN = 0,
	TCORE_LOG_DEFAULT,
	TCORE_LOG_VERBOSE,
	TCORE_LOG_DEBUG,
	TCORE_LOG_INFO,
	TCORE_LOG_WARN,
	TCORE_LOG_ERROR,
	TCORE_LOG_FATAL,
	TCORE_LOG_SILENT
};

/*
 * Virtual log function.
 * Daemon should implement the actual content. (printrf/file writing/...)
 */
void tcore_log(enum tcore_log_type type, enum tcore_log_priority priority, const char *tag, const char *fmt, ...);

#define info(fmt,args...)  do { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_INFO, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)
#define msg(fmt,args...)  do { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_INFO, TCORE_LOG_TAG, fmt "\n", ##args); } while (0)
#define dbg(fmt,args...)  do { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_DEBUG, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)
#define warn(fmt,args...)  do { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_WARN, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)
#define err(fmt,args...)  do { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_ERROR, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)
#define fatal(fmt,args...)  do { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_FATAL, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)

#define info_ex(tag,fmt,args...)  do { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_INFO, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)
#define msg_ex(tag,fmt,args...)  do { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_INFO, tag, fmt "\n", ##args); } while (0)
#define dbg_ex(tag,fmt,args...)  do { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_DEBUG, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)
#define warn_ex(tag,fmt,args...)  do { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_WARN, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)
#define err_ex(tag,fmt,args...)  do { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_ERROR, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)
#define fatal_ex(tag,fmt,args...)  do { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_FATAL, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); } while (0)

#define TIME_CHECK(fmt,args...) do { tcore_log(TCORE_LOG_TYPE_TIME_CHECK, TCORE_LOG_INFO, "TIME_CHECK", fmt "\n", ##args); } while (0)

#else

#define ANSI_COLOR_NORMAL "\e[0m"

#define ANSI_COLOR_BLACK "\e[0;30m"
#define ANSI_COLOR_RED "\e[0;31m"
#define ANSI_COLOR_GREEN "\e[0;32m"
#define ANSI_COLOR_BROWN "\e[0;33m"
#define ANSI_COLOR_BLUE "\e[0;34m"
#define ANSI_COLOR_MAGENTA "\e[0;35m"
#define ANSI_COLOR_CYAN "\e[0;36m"
#define ANSI_COLOR_LIGHTGRAY "\e[0;37m"

#define ANSI_COLOR_DARKGRAY "\e[1;30m"
#define ANSI_COLOR_LIGHTRED "\e[1;31m"
#define ANSI_COLOR_LIGHTGREEN "\e[1;32m"
#define ANSI_COLOR_YELLOW "\e[1;33m"
#define ANSI_COLOR_LIGHTBLUE "\e[1;34m"
#define ANSI_COLOR_LIGHTMAGENTA "\e[1;35m"
#define ANSI_COLOR_LIGHTCYAN "\e[1;36m"
#define ANSI_COLOR_WHITE "\e[1;37m"

#ifndef TCORE_LOG_FILE
#define TCORE_LOG_FILE stdout
#endif

#ifndef TCORE_LOG_FUNC
#define TCORE_LOG_FUNC fprintf
#endif

#define info(fmt,args...) do { if(tcore_debug) TCORE_LOG_FUNC(TCORE_LOG_FILE, fmt "\n", ##args); fflush(TCORE_LOG_FILE);} while (0)
#define msg(fmt,args...) do { if(tcore_debug) TCORE_LOG_FUNC(TCORE_LOG_FILE, fmt "\n", ##args); fflush(TCORE_LOG_FILE);} while (0)
#define dbg(fmt,args...) do { if(tcore_debug) TCORE_LOG_FUNC(TCORE_LOG_FILE, ANSI_COLOR_LIGHTGRAY "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);} while (0)
#define warn(fmt,args...) do {TCORE_LOG_FUNC(TCORE_LOG_FILE, ANSI_COLOR_YELLOW "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);} while (0)
#define err(fmt,args...)  do {TCORE_LOG_FUNC(TCORE_LOG_FILE, ANSI_COLOR_LIGHTRED "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);} while (0)
#define fatal(fmt,args...)  do {TCORE_LOG_FUNC(TCORE_LOG_FILE, ANSI_COLOR_LIGHTRED "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);} while (0)

#endif

__END_DECLS

#endif
