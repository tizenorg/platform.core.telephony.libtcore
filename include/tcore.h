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

#ifndef __TCORE_H__
#define __TCORE_H__

#include <sys/types.h>
#include <stdint.h>

#include <log.h>

#include <type/common.h>

#include <type/request.h>
#include <type/response.h>
#include <type/notification.h>

#include <type/ps.h>
#include <type/network.h>
#include <type/modem.h>
#include <type/sim.h>
#include <type/sat.h>
#include <type/sms.h>
#include <type/call.h>
#include <type/ss.h>
#include <type/sap.h>
#include <type/phonebook.h>
#include <type/gps.h>

typedef struct tcore_object_type CoreObject;
typedef struct tcore_hal_type TcoreHal;
typedef struct tcore_plugin_type TcorePlugin;
typedef struct tcore_modem_type TcoreModem;
typedef struct tcore_queue_type TcoreQueue;
typedef struct tcore_pending_type TcorePending;
typedef struct tcore_communicator_type Communicator;
typedef struct tcore_user_request_type UserRequest;
typedef struct tcore_server_type Server;
typedef struct tcore_storage_type Storage;
typedef struct tcore_at_type TcoreAT;
typedef struct tcore_udev_type TcoreUdev;

enum tcore_hook_return {
	TCORE_HOOK_RETURN_STOP_PROPAGATION = FALSE,
	TCORE_HOOK_RETURN_CONTINUE = TRUE
};

#endif
