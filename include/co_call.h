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

#ifndef _CO_CALL_H__
#define _CO_CALL_H__

#include "core_object.h"
#include <tel_call.h>
#include <tel_return.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef TelCallStatus CallObject;

typedef struct {
	TelReturn (*dial)(CoreObject *co, const TelCallDial *dial_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*answer)(CoreObject *co, TelCallAnswerType ans_type, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*end)(CoreObject *co, const TelCallEnd *end_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*send_dtmf)(CoreObject *co, const char *dtmf_str, TcoreObjectResponseCallback cb,  void *cb_data);
	TelReturn (*hold)(CoreObject *co, TcoreObjectResponseCallback cb ,void *cb_data);
	TelReturn (*active)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*swap)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*join)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*split)(CoreObject *co, unsigned int call_id, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*transfer)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*deflect)(CoreObject *co, const char *deflect_to, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_active_line)(CoreObject *co, TelCallActiveLine active_line, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_active_line)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_volume_info)(CoreObject *co, const TelCallVolumeInfo *volume_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_volume_info)(CoreObject *co, TelCallSoundDevice sound_device, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_sound_path)(CoreObject *co, const TelCallSoundPathInfo *sound_path_info, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_mute)(CoreObject *co, gboolean mute, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_mute_status)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_sound_recording)(CoreObject *co, TelCallSoundRecording sound_rec, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_sound_equalization)(CoreObject *co, const TelCallSoundEqualization *sound_eq, TcoreObjectResponseCallback cb, void *cb_data);
} TcoreCallOps;


/* Call Core API */
CoreObject *tcore_call_new(TcorePlugin *p, TcoreCallOps *ops, TcoreHal *hal);
void tcore_call_free(CoreObject *co);

gboolean tcore_call_set_ops(CoreObject *co, TcoreCallOps *ops);
void tcore_call_override_ops(CoreObject *co, TcoreCallOps *call_ops);

/* Call Object API */
CallObject *tcore_call_object_new(CoreObject *co, unsigned int call_id);
void tcore_call_object_free(CoreObject *co, CallObject *call_obj);
CallObject *tcore_call_object_current_on_mt_processing(CoreObject *co);
CallObject *tcore_call_object_current_on_mo_processing(CoreObject *co);
CallObject *tcore_call_object_find_by_id(CoreObject *co, unsigned int call_id);
CallObject *tcore_call_object_find_by_number(CoreObject *co, const char *num);
GSList *tcore_call_object_find_by_status(CoreObject *co, TelCallState cs);

gboolean tcore_call_object_get_id(CallObject *call_obj, unsigned int *call_id);

gboolean tcore_call_object_set_type(CallObject *call_obj, TelCallType ct);
gboolean tcore_call_object_get_call_type(CallObject *call_obj, TelCallType *call_type);


gboolean tcore_call_object_set_direction(CallObject *call_obj, gboolean mo);
gboolean tcore_call_object_get_direction(CallObject *call_obj, gboolean *mo_call);

gboolean tcore_call_object_set_state(CallObject *call_obj, TelCallState call_state);
gboolean tcore_call_object_get_state(CallObject *call_obj, TelCallState *call_state);

gboolean tcore_call_object_set_cli_info(CallObject *call_obj, TelCallCliValidity cli_validity, char *num);
gboolean tcore_call_object_get_cli_validity(CallObject *call_obj, TelCallCliValidity *cli_validity);
gsize tcore_call_object_get_number(CallObject *call_obj, char *num);

gboolean tcore_call_object_set_cni_info(CallObject *call_obj, TelCallCniValidity cni_validity, char *name);
gboolean tcore_call_object_get_cni_validity(CallObject *call_obj, TelCallCniValidity *cni_validity);
gsize tcore_call_object_get_name(CallObject *call_obj, char *name);

gboolean tcore_call_object_set_multiparty_state(CallObject *call_obj, gboolean state);
gboolean tcore_call_object_get_multiparty_state(CallObject *call_obj, gboolean *mpty);

gboolean tcore_call_object_set_mt_forward(CallObject *call_obj, gboolean forward);
gboolean tcore_call_object_get_mt_forward(CallObject *call_obj, gboolean *forward);

gboolean tcore_call_object_set_active_line(CallObject *call_obj, TelCallActiveLine line);
gboolean tcore_call_object_get_active_line(CallObject *call_obj, TelCallActiveLine *active_line);
gboolean tcore_call_object_get_count(CoreObject *co, unsigned int *count);

#ifdef __cplusplus
}
#endif

#endif
