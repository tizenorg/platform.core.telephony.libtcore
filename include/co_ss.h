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

#ifndef __CO_SS_H__
#define __CO_SS_H__

#include "core_object.h"
#include <tel_ss.h>
#include <tel_return.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ussd_session UssdSession;

typedef struct {
	TelReturn (*set_barring)(CoreObject *co, const TelSsBarringInfo *barring_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_barring_status)(CoreObject *co, const TelSsBarringGetInfo *get_barring_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*change_barring_password)(CoreObject *co, const TelSsBarringPwdInfo *barring_pwd_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_forwarding)(CoreObject *co, const TelSsForwardInfo *forwarding_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_forwarding_status)(CoreObject *co, const TelSsForwardGetInfo *get_forwarding_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_waiting)(CoreObject *co, const TelSsWaitingInfo *waiting_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_waiting_status)(CoreObject *co, TelSsClass ss_class, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_cli)(CoreObject *co, const TelSsCliInfo *cli_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_cli_status)(CoreObject *co, TelSsCliType cli_type, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*send_ussd_request)(CoreObject *co, const TelSsUssdInfo *ussd_request, TcoreObjectResponseCallback cb, void *cb_data);
} TcoreSsOps;


CoreObject *tcore_ss_new(TcorePlugin *p, TcoreSsOps *ops, TcoreHal *hal);
void tcore_ss_free(CoreObject *co);

gboolean tcore_ss_set_ops(CoreObject *co, TcoreSsOps *ops);
void tcore_ss_override_ops(CoreObject *co, TcoreSsOps *ss_ops);

UssdSession *tcore_ss_ussd_create_session(CoreObject *co, TelSsUssdType type, void *data, guint data_len);
void tcore_ss_ussd_destroy_session(UssdSession *ussd_s);
UssdSession *tcore_ss_ussd_get_session(CoreObject *co);
gboolean tcore_ss_ussd_get_session_type(UssdSession *ussd_s, TelSsUssdType *ussd_type);
gboolean tcore_ss_ussd_set_session_type(UssdSession *ussd_s, TelSsUssdType type);
gboolean tcore_ss_ussd_get_session_data(UssdSession *ussd_s, void **data, guint *data_len);
gboolean tcore_ss_ussd_set_session_data(UssdSession *ussd_s, void *data, guint data_len);

#ifdef __cplusplus
}
#endif


#endif
