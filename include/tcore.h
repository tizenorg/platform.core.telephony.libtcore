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

#ifndef __TCORE_H__
#define __TCORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stdint.h>

#include <log.h>
#include <string.h>

#include "type/common.h"
#include "type/command.h"
#include "type/notification.h"

#include <tel_return.h>
#include <tel_network.h>
#include <tel_modem.h>
#include <tel_sim.h>
//#include <tel_sat.h>
#include <tel_sms.h>
#include <tel_call.h>
#include <tel_ss.h>
#include <tel_sap.h>
#include <tel_phonebook.h>
#include <tel_gps.h>

#include <util.h>

typedef struct tcore_object_type CoreObject;
typedef struct tcore_hal_type TcoreHal;
typedef struct tcore_plugin_type TcorePlugin;
typedef struct tcore_modem_type TcoreModem;
typedef struct tcore_queue_type TcoreQueue;
typedef struct tcore_pending_type TcorePending;
typedef struct tcore_communicator_type Communicator;
typedef struct tcore_server_type Server;
typedef struct tcore_storage_type TcoreStorage;
typedef struct tcore_at_type TcoreAT;
typedef struct tcore_udev_type TcoreUdev;

typedef enum {
	TCORE_HOOK_RETURN_STOP_PROPAGATION = FALSE,
	TCORE_HOOK_RETURN_CONTINUE = TRUE
} TcoreHookReturn;

/*
 *Function pointers
 */
/* Response callbacks */
typedef void (*TcorePluginResponseCallback)(TcorePlugin *plugin,
	gint result, const void *response, void *user_data);

typedef void (*TcoreObjectResponseCallback)(CoreObject *co,
	gint result, const void *response, void *user_data);

/* Dispatcher */
typedef TelReturn (*TcoreObjectDispatcher)(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *user_data);

/* Hooks */
typedef TcoreHookReturn (*TcoreRequestHook)(CoreObject *co,
	TcoreCommand command, const void *request, const void *user_data,
	TcoreObjectResponseCallback cb, const void *cb_data);
typedef TcoreHookReturn (*TcoreResponseHook)(CoreObject *co,
	gint result, TcoreCommand command, const void *response, const void *user_data);
typedef TcoreHookReturn (*TcoreNotificationHook)(TcorePlugin *source,
	TcoreNotification command, guint data_len, void *data, void *user_data);

#ifdef __cplusplus
}
#endif

#endif	/* __TCORE_H__ */
