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

#ifndef __CO_SMS_H__
#define __CO_SMS_H__

#define tcore_sms_convert_to_semioctet_length(length)	((length + 1) / 2)

#include "core_object.h"
#include <tel_sms.h>
#include <tel_return.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tcore_sms_operations {
	TelReturn (*send_sms)(CoreObject *co, const TelSmsSendInfo *send_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*read_in_sim)(CoreObject *co, unsigned int index, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*write_in_sim)(CoreObject *co, const TelSmsSimDataInfo *wdata, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*delete_in_sim)(CoreObject *co, unsigned int index, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_count)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_cb_config)(CoreObject *co, const TelSmsCbConfigInfo *cb_conf, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_cb_config)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_parameters)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_parameters)(CoreObject *co, const TelSmsParamsInfo *params, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*send_deliver_report)(CoreObject *co, const TelSmsDeliverReportInfo *dr_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_sca)(CoreObject *co, const TelSmsSca *sca, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_sca)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_memory_status)(CoreObject *co, gboolean available, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_message_status)(CoreObject *co, const TelSmsStatusInfo *status_info, TcoreObjectResponseCallback cb, void *cb_data);
} TcoreSmsOps;

gboolean tcore_sms_get_ready_status(CoreObject *co, gboolean *status);
gboolean tcore_sms_set_ready_status(CoreObject *co, gboolean status);

CoreObject *tcore_sms_new(TcorePlugin *p, TcoreSmsOps *sms_ops, TcoreHal *hal);
void tcore_sms_free(CoreObject *co);

gboolean tcore_sms_set_ops(CoreObject *co, TcoreSmsOps *ops);
void tcore_sms_override_ops(CoreObject *co, TcoreSmsOps *sms_ops);

#ifdef __cplusplus
}
#endif

#endif /*__CO_SMS_H__*/
