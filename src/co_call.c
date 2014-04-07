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

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "co_call.h"

/* Max 6 Call Objects are supported */
#define MAX_CALL_OBJECTS		6

typedef struct {
	GSList *cobjs;
	TcoreCallOps *ops;
} PrivateObject;

typedef gboolean(*TcoreCompareCallObject)(CallObject *call_obj, void *data);

static void _po_clone_hook(CoreObject *src, CoreObject *dest)
{
	PrivateObject *src_po;
	PrivateObject *dest_po;

	tcore_check_return_assert(src != NULL);
	tcore_check_return_assert(dest != NULL);

	src_po = tcore_object_ref_object(src);
	if (NULL == src_po) {
		err("invalid source private object");
		tcore_object_link_object(dest, NULL);
		return;
	}

	tcore_check_return_assert(src_po->ops != NULL);
	dest_po = tcore_malloc0(sizeof(PrivateObject));
	dest_po->ops = tcore_memdup((gconstpointer)src_po->ops, sizeof(TcoreCallOps));
	tcore_object_link_object(dest, dest_po);
}

static TelReturn _dispatcher(CoreObject *co, TcoreCommand command,
		const void *request, TcoreObjectResponseCallback cb,
		const void *user_data)
{
	PrivateObject *po;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL,  TEL_RETURN_INVALID_PARAMETER);
	tcore_check_return_value_assert(po->ops != NULL, TEL_RETURN_INVALID_PARAMETER);

	dbg("command [0x%x]", command);
	switch (command) {
	case TCORE_COMMAND_CALL_DIAL:
		if(po->ops->dial)
			return po->ops->dial(co,
			(const TelCallDial *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_ANSWER:
		if(po->ops->answer)
			return po->ops->answer(co,
			*(TelCallAnswerType *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_END:
		if(po->ops->end)
			return po->ops->end(co,
			(const TelCallEnd *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_SEND_DTMF:
		if(po->ops->send_dtmf)
			return po->ops->send_dtmf(co,
			(const char *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_HOLD:
		if(po->ops->hold)
			return po->ops->hold(co,
			cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_ACTIVE:
		if(po->ops->active)
			return po->ops->active(co,
			cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_SWAP:
		if(po->ops->swap)
			return po->ops->swap(co,
			cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_JOIN:
		if(po->ops->join)
			return po->ops->join(co,
			cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_SPLIT:
		if(po->ops->split)
			return po->ops->split(co,
			*(unsigned int *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_TRANSFER:
		if(po->ops->transfer)
			return po->ops->transfer(co,
			cb, (void *) user_data);
		break;
	case TCORE_COMMAND_CALL_DEFLECT:
		if(po->ops->deflect)
			return po->ops->deflect(co,
			(const char *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_SET_ACTIVE_LINE:
		if(po->ops->set_active_line)
			return po->ops->set_active_line(co,
			*(TelCallActiveLine *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_GET_ACTIVE_LINE:
		if(po->ops->get_active_line)
			return po->ops->get_active_line(co,
			cb, (void *) user_data);
		break;
	case TCORE_COMMAND_CALL_SET_VOLUME_INFO:
		if(po->ops->set_volume_info)
			return po->ops->set_volume_info(co,
			(const TelCallVolumeInfo *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_GET_VOLUME_INFO:
		if(po->ops->get_volume_info)
			return po->ops->get_volume_info(co,
			*(TelCallSoundDevice *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_SET_SOUND_PATH:
		if(po->ops->set_sound_path)
			return po->ops->set_sound_path(co,
			(const TelCallSoundPathInfo *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_SET_MUTE:
		if(po->ops->set_mute)
			return po->ops->set_mute(co,
			*(gboolean *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_GET_MUTE_STATUS:
		if(po->ops->get_mute_status)
			return po->ops->get_mute_status(co,
			cb, (void *) user_data);
		break;
	case TCORE_COMMAND_CALL_SET_SOUND_RECORDING:
		if(po->ops->set_sound_recording)
			return po->ops->set_sound_recording(co,
			*(TelCallSoundRecording *)request, cb, (void *)user_data);
		break;
	case TCORE_COMMAND_CALL_SET_SOUND_EQUALIZATION:
		if(po->ops->set_sound_equalization)
			return po->ops->set_sound_equalization(co,
			(const TelCallSoundEqualization *)request, cb, (void *)user_data);
		break;
	default:
		err("Invalid Command");
		return TEL_RETURN_INVALID_PARAMETER;
	}
	err("Operation NOT Permitted");
	return TEL_RETURN_OPERATION_NOT_SUPPORTED;
}

static void _po_free_hook(CoreObject *co)
{
	PrivateObject *po;

	po = tcore_object_ref_object(co);
	tcore_check_return(po != NULL);

	g_slist_free_full(po->cobjs, g_free);
	tcore_free(po->ops);
	tcore_free(po);
	tcore_object_link_object(co, NULL);
}

static CallObject *_find_object(GSList *objs, void  *data, TcoreCompareCallObject compare)
{

	GSList *list = objs;

	tcore_check_return_value(list != NULL, NULL);
	tcore_check_return_value_assert(compare != NULL, NULL);

	while (list) {
		CallObject *call_obj = (CallObject *) list->data;
		if (compare(call_obj, data))
			return call_obj;

		list = g_slist_next(list);
	}
	return NULL;
}

static GSList *_find_object_all(GSList *objs, void  *data, TcoreCompareCallObject compare)
{

	GSList *list = objs;
	GSList *obj_list = NULL;

	tcore_check_return_value(list != NULL, NULL);
	tcore_check_return_value_assert(compare != NULL, NULL);

	while (list) {
		CallObject *call_obj = (CallObject *) list->data;

		if (compare(call_obj, data))
			obj_list = g_slist_append(obj_list, call_obj);

		list = g_slist_next(list);
	}

	return obj_list;
}

static gboolean _compare_by_id(CallObject *call_obj, void *data)
{

	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(data != NULL, FALSE);

	if (call_obj->call_id == *(unsigned int*) data)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_status(CallObject *call_obj, void *data)
{

	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(data != NULL, FALSE);

	if (call_obj->call_state == *(TelCallState *) data)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_number(CallObject *call_obj, void *data)
{

	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(data != NULL, FALSE);

	if (0 == g_strcmp0(call_obj->number, (char *)data))
		return TRUE;

	return FALSE;
}

static gboolean _check_cli_mode_by_number(
	char num[TEL_CALL_CALLING_NUMBER_LEN_MAX + 1],
	TelCallCliValidity *cli_validity)
{
	tcore_check_return_value_assert(cli_validity != NULL, FALSE);

	if (0 == strncmp(num, "*31#", 4))
		 *cli_validity = TEL_CALL_CLI_VALIDITY_VALID;
	else if (0 == strncmp(num, "#31#", 4))
		*cli_validity = TEL_CALL_CLI_VALIDITY_WITHHELD;
	else
		*cli_validity = TEL_CALL_CLI_VALIDITY_NOT_AVAILABLE;

	return TRUE;
}

/* Call Object API */
CallObject *tcore_call_object_new(CoreObject *co, unsigned int call_id)
{
	PrivateObject *po;
	CallObject *call_obj;
	unsigned int my_call_id;

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);

	if (call_id > 0) {
		if (NULL == _find_object(po->cobjs, (void *)&call_id,
			(TcoreCompareCallObject)_compare_by_id)) {
			my_call_id = call_id;
		} else {
			warn(" call object exist already. [ %d ]", call_id);
			return NULL;
		}
	} else {
		unsigned int i;
		for (i = 1; i <= MAX_CALL_OBJECTS; i++) {	/* Presently 6 is MAX call count */
			if (NULL == _find_object(po->cobjs, (void *)&i,
				(TcoreCompareCallObject) _compare_by_id)) {
				my_call_id = i;
				break;
			}
		}
		/* Free the allocated Core Object if ID is not allocated */
		if(i > MAX_CALL_OBJECTS) {
			err("failed to assign call id");
			return NULL;
		}
	}

	call_obj = tcore_malloc0(sizeof(CallObject));
	call_obj->call_id = my_call_id;
	/* Appending the new Call object to Call Objects list */
	po->cobjs = g_slist_append(po->cobjs, call_obj);
	dbg("new call object id : [%d]", call_obj->call_id);

	return call_obj;
}

void tcore_call_object_free(CoreObject *co, CallObject *call_obj)
{
	PrivateObject *po;

	po = tcore_object_ref_object(co);
	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->cobjs != NULL);
	tcore_check_return_assert(call_obj != NULL);

	po->cobjs = g_slist_remove(po->cobjs, call_obj);
	dbg("Freeing call core object");
	tcore_free(call_obj);
}

gboolean tcore_call_object_get_count(CoreObject *co, unsigned int *count)
{
	PrivateObject *po = NULL;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_CALL, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);
	tcore_check_return_value_assert(count != NULL, FALSE);

	*count = g_slist_length(po->cobjs);

	return TRUE;
}

CallObject *tcore_call_object_current_on_mt_processing(CoreObject *co)
{
	PrivateObject *po;
	CallObject *call_obj = NULL;
	TelCallState call_state = TEL_CALL_STATE_INCOMING;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_CALL, NULL);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);
	tcore_check_return_value_assert(po->cobjs != NULL, NULL);

	call_obj = _find_object(po->cobjs, (void *)&call_state,
		(TcoreCompareCallObject)_compare_by_status);
	if (NULL == call_obj) {
		call_state = TEL_CALL_STATE_WAITING;
		call_obj = _find_object(po->cobjs, (void *) &call_state,
				(TcoreCompareCallObject)_compare_by_status);
		if (NULL == call_obj) {
			err("no mt call object available");
			return NULL;
		}
	}
	return call_obj;
}

CallObject *tcore_call_object_current_on_mo_processing(CoreObject *co)
{
	PrivateObject *po;
	CallObject *call_obj = NULL;
	TelCallState call_state = TEL_CALL_STATE_DIALING;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_CALL, NULL);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);
	tcore_check_return_value_assert(po->cobjs != NULL, NULL);

	call_obj = _find_object(po->cobjs, (void *)&call_state,
		(TcoreCompareCallObject) _compare_by_status);
	if (NULL == call_obj) {
		call_state = TEL_CALL_STATE_ALERT;
		call_obj = _find_object(po->cobjs, (void *)&call_state,
			(TcoreCompareCallObject) _compare_by_status);
		if (NULL == call_obj) {
			err("no mo call object available");
			return NULL;
		}
	}
	return call_obj;
}

CallObject *tcore_call_object_find_by_id(CoreObject *co, unsigned int call_id)
{
	PrivateObject *po;
	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_CALL, NULL);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);

	return _find_object(po->cobjs, (void *)&call_id,
		(TcoreCompareCallObject) _compare_by_id);
}

CallObject *tcore_call_object_find_by_number(CoreObject *co,
	const char num[TEL_CALL_CALLING_NUMBER_LEN_MAX + 1])
{
	PrivateObject *po;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_CALL, NULL);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);

	return _find_object(po->cobjs, (void *)num,
		(TcoreCompareCallObject)_compare_by_number);
}

GSList *tcore_call_object_find_by_status(CoreObject *co, TelCallState call_state)
{
	PrivateObject *po;

	CORE_OBJECT_CHECK_RETURN(co, CORE_OBJECT_TYPE_CALL, NULL);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, NULL);

	return _find_object_all(po->cobjs, (void *) &call_state,
			(void *) _compare_by_status);
}

gboolean tcore_call_object_get_id(CallObject *call_obj, unsigned int *call_id)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(call_id != NULL, FALSE);

	*call_id = call_obj->call_id;
	return TRUE;
}

gboolean tcore_call_object_set_type(CallObject *call_obj, TelCallType call_type)
{
	tcore_check_return_value(call_obj != NULL, FALSE);

	call_obj->call_type = call_type;
	return TRUE;
}

gboolean tcore_call_object_get_call_type(CallObject *call_obj, TelCallType *call_type)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(call_type != NULL, FALSE);

	*call_type = call_obj->call_type;
	return TRUE;
}

gboolean tcore_call_object_set_direction(CallObject *call_obj, gboolean mo_call)
{
	tcore_check_return_value(call_obj != NULL, FALSE);

	call_obj->mo_call = mo_call;
	return TRUE;
}

gboolean tcore_call_object_get_direction(CallObject *call_obj, gboolean *mo_call)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(mo_call != NULL, FALSE);

	*mo_call = call_obj->mo_call;
	return TRUE;
}

gboolean tcore_call_object_set_state(CallObject *call_obj, TelCallState call_state)
{
	tcore_check_return_value(call_obj != NULL, FALSE);

	call_obj->call_state = call_state;
	return TRUE;
}

gboolean tcore_call_object_get_state(CallObject *call_obj, TelCallState *call_state)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(call_state != NULL, FALSE);

	*call_state = call_obj->call_state;
	return TRUE;
}

gboolean tcore_call_object_set_cli_info(CallObject *call_obj,
		TelCallCliValidity cli_validity, char *num)
{
	char *pos;
	tcore_check_return_value(call_obj != NULL, FALSE);

	if (NULL == num) {
		call_obj->cli_validity = cli_validity;
		return TRUE;
	}
	dbg("num  : %s, cli_validity:%d", num, cli_validity);
	pos = num;
	if (TEL_CALL_CLI_VALIDITY_NOT_AVAILABLE == cli_validity) {

		_check_cli_mode_by_number(num, &(call_obj->cli_validity));

		if (call_obj->cli_validity != TEL_CALL_CLI_VALIDITY_NOT_AVAILABLE)
			pos = (num + 4);
	} else {
		call_obj->cli_validity  = cli_validity;
	}

	g_strlcpy(call_obj->number, pos,  TEL_CALL_CALLING_NAME_LEN_MAX + 1);
	dbg("co->cli_validity: %d ,co->number : %s",
		call_obj->cli_validity, call_obj->number);
	return TRUE;
}

gsize tcore_call_object_get_number(CallObject *call_obj,
	char num[TEL_CALL_CALLING_NUMBER_LEN_MAX + 1])
{
	tcore_check_return_value(call_obj != NULL, 0);

	return g_strlcpy(num, call_obj->number,
		TEL_CALL_CALLING_NUMBER_LEN_MAX + 1);
}

gboolean tcore_call_object_get_cli_validity(CallObject *call_obj,
	TelCallCliValidity *cli_validity)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	*cli_validity = call_obj->cli_validity;
	return TRUE;
}

gboolean tcore_call_object_set_cni_info(CallObject *call_obj,
	TelCallCniValidity cni_validity,
	char name[TEL_CALL_CALLING_NAME_LEN_MAX + 1])
{
	tcore_check_return_value(call_obj != NULL, FALSE);

	g_strlcpy(call_obj->name, name,  TEL_CALL_CALLING_NAME_LEN_MAX + 1);
	call_obj->cni_validity = cni_validity;
	return TRUE;
}

gsize tcore_call_object_get_name(CallObject *call_obj,
	char name[TEL_CALL_CALLING_NAME_LEN_MAX + 1])
{
	tcore_check_return_value(call_obj != NULL, 0);

	return g_strlcpy(name, call_obj->name,
		TEL_CALL_CALLING_NAME_LEN_MAX + 1);
}

gboolean tcore_call_object_get_cni_validity(CallObject *call_obj,
	TelCallCniValidity *cni_validity)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(cni_validity != NULL, FALSE);

	*cni_validity = call_obj->cni_validity;
	return TRUE;
}

gboolean tcore_call_object_set_multiparty_state (CallObject *call_obj,
	gboolean mpty_state)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	call_obj->mpty = mpty_state;
	return TRUE;
}

gboolean tcore_call_object_get_multiparty_state(CallObject *call_obj,
	gboolean *mpty_state)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(mpty_state != NULL, FALSE);

	*mpty_state = call_obj->mpty;
	return TRUE;
}

gboolean tcore_call_object_set_mt_forward (CallObject *call_obj,
	gboolean forward)
{
	tcore_check_return_value(call_obj != NULL, FALSE);

	call_obj->forward = forward;
	return TRUE;
}

gboolean tcore_call_object_get_mt_forward(CallObject *call_obj,
	gboolean *forward)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(forward != NULL, FALSE);

	*forward = call_obj->forward;
	return TRUE;
}

gboolean tcore_call_object_set_active_line(CallObject *call_obj,
	TelCallActiveLine active_line)
{
	tcore_check_return_value(call_obj != NULL, FALSE);

	call_obj->active_line = active_line;
	return TRUE;
}

gboolean tcore_call_object_get_active_line(CallObject *call_obj,
	TelCallActiveLine *active_line)
{
	tcore_check_return_value(call_obj != NULL, FALSE);
	tcore_check_return_value_assert(active_line != NULL, FALSE);

	*active_line = call_obj->active_line;
	return TRUE;
}

void tcore_call_override_ops(CoreObject *o, TcoreCallOps *call_ops)
{
	PrivateObject *po;
	po = tcore_object_ref_object(o);

	tcore_check_return_assert(po != NULL);
	tcore_check_return_assert(po->ops != NULL);
	tcore_check_return_assert(call_ops != NULL);

	if (call_ops->dial)
		po->ops->dial = call_ops->dial;

	if (call_ops->answer)
		po->ops->answer = call_ops->answer;

	if (call_ops->end)
		po->ops->end = call_ops->end;

	if (call_ops->send_dtmf)
		po->ops->send_dtmf = call_ops->send_dtmf;

	if (call_ops->hold)
		po->ops->hold = call_ops->hold;

	if (call_ops->active)
		po->ops->active = call_ops->active;

	if (call_ops->swap)
		po->ops->swap = call_ops->swap;

	if (call_ops->join)
		po->ops->join = call_ops->join;

	if (call_ops->split)
		po->ops->split = call_ops->split;

	if (call_ops->transfer)
		po->ops->transfer = call_ops->transfer;

	if (call_ops->deflect)
		po->ops->deflect = call_ops->deflect;

	if (call_ops->set_active_line)
		po->ops->set_active_line = call_ops->set_active_line;

	if (call_ops->get_active_line)
		po->ops->get_active_line = call_ops->get_active_line;

	if (call_ops->set_volume_info)
		po->ops->set_volume_info = call_ops->set_volume_info;

	if (call_ops->get_volume_info)
		po->ops->get_volume_info = call_ops->get_volume_info;

	if (call_ops->set_sound_path)
		po->ops->set_sound_path = call_ops->set_sound_path;

	if (call_ops->set_mute)
		po->ops->set_mute = call_ops->set_mute;

	if (call_ops->get_mute_status)
		po->ops->get_mute_status = call_ops->get_mute_status;

	if (call_ops->set_sound_recording)
		po->ops->set_sound_recording = call_ops->set_sound_recording;

	if (call_ops->set_sound_equalization)
		po->ops->set_sound_equalization = call_ops->set_sound_equalization;
}

gboolean tcore_call_set_ops(CoreObject *co, TcoreCallOps *ops)
{
	PrivateObject *po;
	tcore_check_return_value(co != NULL, FALSE);

	po = tcore_object_ref_object(co);
	tcore_check_return_value_assert(po != NULL, FALSE);

	if (po->ops != NULL) {
		tcore_free(po->ops);
		po->ops = NULL;
	}

	if (ops != NULL)
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreCallOps));

	return TRUE;
}

CoreObject *tcore_call_new(TcorePlugin *p, TcoreCallOps *ops, TcoreHal *hal)
{
	CoreObject *co;
	PrivateObject *po;
	tcore_check_return_value_assert(p != NULL, NULL);

	co = tcore_object_new(p, hal);
	tcore_check_return_value_assert(co != NULL, NULL);

	po = tcore_malloc0(sizeof(PrivateObject));

	if (ops != NULL)
		po->ops = tcore_memdup((gconstpointer)ops, sizeof(TcoreCallOps));

	tcore_object_set_type(co, CORE_OBJECT_TYPE_CALL);
	tcore_object_link_object(co, po);
	tcore_object_set_free_hook(co, _po_free_hook);
	tcore_object_set_clone_hook(co, _po_clone_hook);
	tcore_object_set_dispatcher(co, _dispatcher);

	return co;
}

void tcore_call_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);
	tcore_object_free(o);
}
