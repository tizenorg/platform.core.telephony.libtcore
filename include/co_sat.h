/*
 * libtcore
 *
 * Copyright (c) 2013 Samsung Electronics Co., Ltd. All rights reserved.
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

#ifndef __CO_SAT_H__
#define __CO_SAT_H__

#include <core_object.h>
#include "type/sat_internal.h"
#include <tel_return.h>

#ifdef __cplusplus
	extern "C" {
#endif

/* OEM operations */
typedef struct {
	TelReturn (*send_envelope)(CoreObject *co, const TelSatRequestEnvelopCmdData *envelop_data, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*send_terminal_response)(CoreObject *co, const TelSatRequestTerminalResponseData *terminal_rsp_data, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*send_user_confirmation)(CoreObject *co, const TelSatRequestUserConfirmationData *user_conf_data, TcoreObjectResponseCallback cb, void *cb_data);
}TcoreSatOps;

CoreObject *tcore_sat_new(TcorePlugin *p, TcoreSatOps *sat_ops, TcoreHal *hal);
void tcore_sat_free(CoreObject *co);
void tcore_sat_override_ops(CoreObject *co, TcoreSatOps *sat_ops);
gboolean tcore_sat_set_ops(CoreObject *co, TcoreSatOps *ops);

gboolean tcore_sat_decode_proactive_command(unsigned char* tlv_origin, unsigned int tlv_length,
		TelSatDecodedProactiveData* decoded_tlv, int* decode_err_code);
int tcore_sat_decode_call_control_result(unsigned char* tlv_origin, unsigned int tlv_length, TelSatNotiCallControlResultInd* call_ctrl_result_tlv);
gboolean tcore_sat_encode_envelop_cmd(const TelSatRequestEnvelopCmdData *src_envelop, char *dst_envelop, int* envelope_length);
gboolean tcore_sat_encode_terminal_response(const TelSatRequestTerminalResponseData *src_tr, char *dst_tr, int* tr_length);

#ifdef __cplusplus
}
#endif

#endif 	/* __CO_SAT_H__ */
