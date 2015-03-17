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

#define info(fmt,args...)  { if(tcore_debug) RLOG(LOG_INFO, TCORE_LOG_TAG, fmt "\n", ##args); }
#define msg(fmt,args...)  { if(tcore_debug) RLOG(LOG_DEBUG, TCORE_LOG_TAG, fmt "\n", ##args); }
#define dbg(fmt,args...)  { if(tcore_debug) RLOG(LOG_DEBUG, TCORE_LOG_TAG, fmt "\n", ##args); }
#define warn(fmt,args...)  { RLOG(LOG_WARN, TCORE_LOG_TAG, fmt "\n", ##args); }
#define err(fmt,args...)  { RLOG(LOG_ERROR, TCORE_LOG_TAG, fmt "\n", ##args); }
#define fatal(fmt,args...)  { RLOG(LOG_FATAL, TCORE_LOG_TAG, fmt "\n", ##args); }

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

#define info(fmt,args...)  { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_INFO, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define msg(fmt,args...)  { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_INFO, TCORE_LOG_TAG, fmt "\n", ##args); }
#define dbg(fmt,args...)  { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_DEBUG, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define warn(fmt,args...)  { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_WARN, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define err(fmt,args...)  { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_ERROR, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define fatal(fmt,args...)  { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_FATAL, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }

#define info_ex(tag,fmt,args...)  { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_INFO, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define msg_ex(tag,fmt,args...)  { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_INFO, tag, fmt "\n", ##args); }
#define dbg_ex(tag,fmt,args...)  { if(tcore_debug) tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_DEBUG, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define warn_ex(tag,fmt,args...)  { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_WARN, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define err_ex(tag,fmt,args...)  { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_ERROR, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define fatal_ex(tag,fmt,args...)  { tcore_log(TCORE_LOG_TYPE_RADIO, TCORE_LOG_FATAL, tag, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }

#define TIME_CHECK(fmt,args...) { tcore_log(TCORE_LOG_TYPE_TIME_CHECK, TCORE_LOG_INFO, "TIME_CHECK", fmt "\n", ##args); }

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

#define info(fmt,args...) { if(tcore_debug) TCORE_LOG_FUNC(TCORE_LOG_FILE, fmt "\n", ##args); fflush(TCORE_LOG_FILE);}
#define msg(fmt,args...) { if(tcore_debug) TCORE_LOG_FUNC(TCORE_LOG_FILE, fmt "\n", ##args); fflush(TCORE_LOG_FILE);}
#define dbg(fmt,args...) { if(tcore_debug) TCORE_LOG_FUNC(TCORE_LOG_FILE, ANSI_COLOR_LIGHTGRAY "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);}
#define warn(fmt,args...) TCORE_LOG_FUNC(TCORE_LOG_FILE, ANSI_COLOR_YELLOW "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);
#define err(fmt,args...)  TCORE_LOG_FUNC(TCORE_LOG_FILE, ANSI_COLOR_LIGHTRED "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);
#define fatal(fmt,args...)  TCORE_LOG_FUNC(TCORE_LOG_FILE, ANSI_COLOR_LIGHTRED "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);

#endif

__END_DECLS

#endif
