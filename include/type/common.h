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

#ifndef __TYPE_COMMON_H__
#define __TYPE_COMMON_H__

#include <errno.h>

#define TCORE_REQUEST        0x10000000
#define TCORE_RESPONSE       0x20000000
#define TCORE_NOTIFICATION   0x30000000
#define TCORE_RETURN         0x40000000
#define TCORE_3GPP_RETURN    0x50000000

#define TCORE_TYPE_DEFAULT   0x00100000
#define TCORE_TYPE_SERVER    0x00200000
#define TCORE_TYPE_MODEM     0x00300000
#define TCORE_TYPE_PENDING   0x00400000
#define TCORE_TYPE_HOOK      0x00500000
#define TCORE_TYPE_CALL      0x01000000
#define TCORE_TYPE_NETWORK   0x01100000
#define TCORE_TYPE_PS        0x01200000
#define TCORE_TYPE_PDP       0x01300000
#define TCORE_TYPE_SIM       0x01400000
#define TCORE_TYPE_SAT       0x01500000
#define TCORE_TYPE_SAP       0x01600000
#define TCORE_TYPE_SMS       0x01700000
#define TCORE_TYPE_PHONEBOOK 0x01800000
#define TCORE_TYPE_SS        0x01900000
#define TCORE_TYPE_SOUND     0x01A00000
#define TCORE_TYPE_GPS       0x01B00000
#define TCORE_TYPE_CUSTOM    0x0F000000

#define TCORE_PLUGIN_DEFAULT "default"


__BEGIN_DECLS

enum tcore_return {
	TCORE_RETURN_SUCCESS = 0,
	TCORE_RETURN_FAILURE = -1,
	TCORE_RETURN_OPERATION_ABORTED = 1, /**< Operation Aborted */
	TCORE_RETURN_ENOENT = ENOENT, /* No such file or directory */
	TCORE_RETURN_EPERM = EPERM, /* Operation not permitted */
	TCORE_RETURN_ENOMEM = ENOMEM, /* Out of memory */
	TCORE_RETURN_EAGAIN = EAGAIN, /* Try again */
	TCORE_RETURN_EINVAL = EINVAL, /* Invalid argument */
	TCORE_RETURN_ENOSYS = ENOSYS, /* Function not implemented */
	TCORE_RETURN_EALREADY = EALREADY, /* Operation already in progress */
	TCORE_RETURN_ETIMEDOUT = ETIMEDOUT, /* Connection timed out */
	TCORE_RETURN_EMSGSIZE = EMSGSIZE, /* Message too long */
	TCORE_RETURN_ENODATA = ENODATA, /* No data available */

	TCORE_RETURN_SERVER_WRONG_PLUGIN = TCORE_RETURN | TCORE_TYPE_SERVER,

	TCORE_RETURN_PENDING_WRONG_ID = TCORE_RETURN | TCORE_TYPE_PENDING,

	TCORE_RETURN_PS_NETWORK_NOT_READY = TCORE_RETURN | TCORE_TYPE_PS,
	TCORE_RETURN_PS_CID_ERROR,
	TCORE_RETURN_PS_ACTIVATING,
	TCORE_RETURN_PS_ALREADY_ACTIVATED,
	TCORE_RETURN_PS_DEACTIVATING,

	TCORE_RETURN_HOOK_STOP = TCORE_RETURN | TCORE_TYPE_HOOK,

	TCORE_RETURN_SIM = TCORE_RETURN | TCORE_TYPE_SIM,
	TCORE_RETURN_SIM_DISABLED_IN_SST, /* feature in sst is disabled */

	/* NETTEXT */
	TCORE_RETURN_SMS = TCORE_RETURN | TCORE_TYPE_SMS,
	TCORE_RETURN_SMS_DEVICE_NOT_READY, /**<Nettext device not ready */
	TCORE_RETURN_SMS_SCADDR_NOT_SET, /**<Nettext SCA address not set */
	TCORE_RETURN_SMS_INVALID_DATA_LEN, /**<Nettext Invalid data length */
	TCORE_RETURN_SMS_SCADDRESS_NOT_SET, /**<Nettext SCA address not set*/

	TCORE_RETURN_3GPP_ERROR = TCORE_3GPP_RETURN, /**< TCORE_3GPP_RETURN + (0x0000 ~ 0x7FFF) */
	TCORE_RETURN_3GPP_PLMN_NOT_ALLOWED,    /* 3GPP error cause 11*/
	TCORE_RETURN_3GPP_ROAMING_NOT_ALLOWED, /* 3GPP error cause 13 */

	TCORE_RETURN_UNKNOWN = TCORE_RETURN | 0x0FFFFFFF
};

typedef enum tcore_return TReturn;

__END_DECLS

#endif
