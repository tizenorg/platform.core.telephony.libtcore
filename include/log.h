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

#ifdef FEATURE_DLOG_DEBUG

#include <dlog.h>

#ifndef TCORE_LOG_TAG
#define TCORE_LOG_TAG "UNKNOWN"
#endif

#define msg(fmt,args...)  { RLOG(LOG_INFO, TCORE_LOG_TAG, fmt "\n", ##args); }
#define dbg(fmt,args...)  { RLOG(LOG_DEBUG, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define warn(fmt,args...)  { RLOG(LOG_WARN, TCORE_LOG_TAG, "<%s:%d> " fmt "\n", __func__, __LINE__, ##args); }
#define err(fmt,args...)  { RLOG(LOG_FATAL, TCORE_LOG_TAG, "<%s:%D> " fmt "\n", __func__, __LINE__, ##args); }

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

#define msg(fmt,args...)  fprintf(TCORE_LOG_FILE, fmt "\n", ##args); fflush(TCORE_LOG_FILE);
#define dbg(fmt,args...)  fprintf(TCORE_LOG_FILE, ANSI_COLOR_LIGHTGRAY "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);
#define warn(fmt,args...) fprintf(TCORE_LOG_FILE, ANSI_COLOR_YELLOW "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);
#define err(fmt,args...)  fprintf(TCORE_LOG_FILE, ANSI_COLOR_LIGHTRED "<%s:%s> " ANSI_COLOR_NORMAL fmt "\n", __FILE__, __FUNCTION__, ##args); fflush(TCORE_LOG_FILE);

#endif

__END_DECLS

#endif
