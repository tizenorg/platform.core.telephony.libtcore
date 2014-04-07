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

#ifndef __CO_SAP_H__
#define __CO_SAP_H__

#include "core_object.h"
#include <tel_sap.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TelReturn (*req_connect)(CoreObject *co, unsigned int max_msg_size, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*req_disconnect)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_atr)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*req_transfer_apdu)(CoreObject *co, const TelSapApdu *apdu_data, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*req_transport_protocol)(CoreObject *co, TelSimSapProtocol protocol, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*req_power_operation)(CoreObject *co, TelSapPowerMode power_mode, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_cardreader_status)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
} TcoreSapOps;

CoreObject *tcore_sap_new(TcorePlugin *plugin, TcoreSapOps *sap_ops, TcoreHal *hal);
void tcore_sap_free(CoreObject *co);

gboolean tcore_sap_set_ops(CoreObject *co, TcoreSapOps *ops);
void tcore_sap_override_ops(CoreObject *co, TcoreSapOps *sap_ops);

#ifdef __cplusplus
}
#endif

#endif /* __CO_SAP_H__ */
