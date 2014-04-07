/*
 * libtcore
 *
 * Copyright (c) 2013 Samsung Electronics Co. Ltd. All rights reserved.
 * Copyright (c) 2013 Intel Corporation. All rights reserved.
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

#ifndef __TYPE_COMMON_H__
#define __TYPE_COMMON_H__

#include <errno.h>

#define TCORE_COMMAND			0x10000000
#define TCORE_NOTIFICATION		0x20000000

/* Command Masks */
/* TODO - Remove unwanted command masks */
#define TCORE_TYPE_DEFAULT		0x00100000
#define TCORE_TYPE_SERVER		0x00200000
#define TCORE_TYPE_PENDING		0x00300000
#define TCORE_TYPE_HOOK		0x00400000
#define TCORE_TYPE_MODEM		0x00500000
#define TCORE_TYPE_SIM			0x00600000
#define TCORE_TYPE_NETWORK	0x00700000
#define TCORE_TYPE_CALL		0x00800000
#define TCORE_TYPE_SS			0x00900000
#define TCORE_TYPE_PHONEBOOK	0x00A00000
#define TCORE_TYPE_SMS			0x00B00000
#define TCORE_TYPE_PS			0x00C00000
#define TCORE_TYPE_PDP			0x00D00000
#define TCORE_TYPE_SAT			0x00E00000
#define TCORE_TYPE_SAP			0x00F00000
#define TCORE_TYPE_SOUND		0x01000000
#define TCORE_TYPE_GPS			0x01100000
#define TCORE_TYPE_CUSTOM		0x0F000000

#define TCORE_PLUGIN_DEFAULT	"default"

#endif	/* __TYPE_COMMON_H__ */
