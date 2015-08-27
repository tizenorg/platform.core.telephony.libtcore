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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "internal/tcore_types.h"
#include "plugin.h"
#include "queue.h"
#include "user_request.h"
#include "co_call.h"


#define MAX_CALL_OBJECTS		6

struct call_cli_info {
	enum tcore_call_cli_mode mode;
	enum tcore_call_no_cli_cause no_cli_cause;
	char number[MAX_CALL_NUMBER_LEN];
	int number_len;
};

struct call_cna_info {
	enum tcore_call_cna_mode mode;
	int dcs;
	char name[MAX_CALL_NAME_LEN];
	int name_len;
};

struct call_object {
	enum tcore_call_type type;

	unsigned int handle;
	unsigned int id;

	enum tcore_call_direction direction;
	enum tcore_call_status status;
	gboolean mpty;
	struct call_cli_info cli;
	struct call_cna_info cna;
	unsigned int cug_id;
	unsigned int active_line;
	struct call_time {
		long start; /* In seconds */
		long end; /* In seconds */
	} time;

	gboolean is_volte_call;
	int session_id; /* VoLTE only */
	int conf_call_session_id; /* Conference call session-id (VoLTE only) */
	gboolean early_media; /* VoLTE only */

	gboolean is_release_pending;
	enum tcore_call_silent_redial_reason redial_reason;
};

struct private_object_data {
	GSList *cobjs;
	struct tcore_call_operations *ops[TCORE_OPS_TYPE_MAX];
	struct tcore_call_control_operations *cops;
	struct tcore_call_information_operations *iops;
};

static unsigned int __assign_handle(CoreObject *o)
{
	unsigned int handle_candidate = MIN_HANDLE;
	CallObject *co = NULL;

	/* Find unused 'handle' - starting from 1 */
	while (handle_candidate < MAX_HANDLE) {
		co = tcore_call_object_find_by_handle(o, handle_candidate);
		if (NULL == co) {
			/* Unused handle found */
			return handle_candidate;
		} else {
			/* 'handle' already used, try next value */
			handle_candidate++;
			co = NULL;
		}
	}

	err("available handle not found, serious");
	return INVALID_HANDLE;
}

static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;

	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_call_operations *ops = NULL;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_CALL, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_CALL_DIAL:
		tcore_check_null_ret_err("ops->dial",
			ops->dial, TCORE_RETURN_ENOSYS);

		return ops->dial(o, ur);

	case TREQ_CALL_ANSWER:
		tcore_check_null_ret_err("ops->answer",
			ops->answer, TCORE_RETURN_ENOSYS);

		return ops->answer(o, ur);

	case TREQ_CALL_END:
		tcore_check_null_ret_err("ops->end",
			ops->end, TCORE_RETURN_ENOSYS);

		return ops->end(o, ur);

	case TREQ_CALL_HOLD:
		tcore_check_null_ret_err("ops->hold",
			ops->hold, TCORE_RETURN_ENOSYS);

		return ops->hold(o, ur);

	case TREQ_CALL_ACTIVE:
		tcore_check_null_ret_err("ops->active",
			ops->active, TCORE_RETURN_ENOSYS);

		return ops->active(o, ur);

	case TREQ_CALL_SWAP:
		tcore_check_null_ret_err("ops->swap",
			ops->swap, TCORE_RETURN_ENOSYS);

		return ops->swap(o, ur);

	case TREQ_CALL_JOIN:
		tcore_check_null_ret_err("ops->join",
			ops->join, TCORE_RETURN_ENOSYS);

		return ops->join(o, ur);

	case TREQ_CALL_SPLIT:
		tcore_check_null_ret_err("ops->split",
			ops->split, TCORE_RETURN_ENOSYS);

		return ops->split(o, ur);

	case TREQ_CALL_DEFLECT:
		tcore_check_null_ret_err("ops->deflect",
			ops->deflect, TCORE_RETURN_ENOSYS);

		return ops->deflect(o, ur);

	case TREQ_CALL_TRANSFER:
		tcore_check_null_ret_err("ops->transfer",
			ops->transfer, TCORE_RETURN_ENOSYS);

		return ops->transfer(o, ur);

	case TREQ_CALL_START_CONT_DTMF:
		tcore_check_null_ret_err("ops->start_cont_dtmf",
			ops->start_cont_dtmf, TCORE_RETURN_ENOSYS);

		return ops->start_cont_dtmf(o, ur);

	case TREQ_CALL_STOP_CONT_DTMF:
		tcore_check_null_ret_err("ops->stop_cont_dtmf",
			ops->stop_cont_dtmf, TCORE_RETURN_ENOSYS);

		return ops->stop_cont_dtmf(o, ur);

	case TREQ_CALL_SEND_BURST_DTMF:
		tcore_check_null_ret_err("ops->send_burst_dtmf",
			ops->send_burst_dtmf, TCORE_RETURN_ENOSYS);

		return ops->send_burst_dtmf(o, ur);

	case TREQ_CALL_GET_PRIVACY_MODE:
		tcore_check_null_ret_err("ops->get_privacy_mode",
			ops->get_privacy_mode, TCORE_RETURN_ENOSYS);

		return ops->get_privacy_mode(o, ur);

	case TREQ_CALL_SET_PRIVACY_MODE:
		tcore_check_null_ret_err("ops->set_privacy_mode",
			ops->set_privacy_mode, TCORE_RETURN_ENOSYS);

		return ops->set_privacy_mode(o, ur);

	case TREQ_CALL_SET_SOUND_PATH:
		tcore_check_null_ret_err("ops->set_sound_path",
			ops->set_sound_path, TCORE_RETURN_ENOSYS);

		return ops->set_sound_path(o, ur);

	case TREQ_CALL_GET_SOUND_VOLUME_LEVEL:
		tcore_check_null_ret_err("ops->get_sound_volume_level",
			ops->get_sound_volume_level, TCORE_RETURN_ENOSYS);

		return ops->get_sound_volume_level(o, ur);

	case TREQ_CALL_SET_SOUND_VOLUME_LEVEL:
		tcore_check_null_ret_err("ops->set_sound_volume_level",
			ops->set_sound_volume_level, TCORE_RETURN_ENOSYS);

		return ops->set_sound_volume_level(o, ur);

	case TREQ_CALL_SET_SOUND_MUTE_STATUS:
		tcore_check_null_ret_err("ops->set_sound_mute_status",
			ops->set_sound_mute_status, TCORE_RETURN_ENOSYS);

		return ops->set_sound_mute_status(o, ur);

	case TREQ_CALL_GET_SOUND_MUTE_STATUS:
		tcore_check_null_ret_err("ops->get_sound_mute_status",
			ops->get_sound_mute_status, TCORE_RETURN_ENOSYS);

		return ops->get_sound_mute_status(o, ur);

	case TREQ_CALL_SET_SOUND_RECORDING:
		tcore_check_null_ret_err("ops->set_sound_recording",
			ops->set_sound_recording, TCORE_RETURN_ENOSYS);

		return ops->set_sound_recording(o, ur);

	case TREQ_CALL_SET_SOUND_EQUALIZATION:
		tcore_check_null_ret_err("ops->set_sound_equalization",
			ops->set_sound_equalization, TCORE_RETURN_ENOSYS);

		return ops->set_sound_equalization(o, ur);

	case TREQ_CALL_SET_SOUND_NOISE_REDUCTION:
		tcore_check_null_ret_err("ops->set_sound_noise_reduction",
			ops->set_sound_noise_reduction, TCORE_RETURN_ENOSYS);

		return ops->set_sound_noise_reduction(o, ur);

	case TREQ_CALL_SET_SOUND_CLOCK_STATUS:
		tcore_check_null_ret_err("ops->set_sound_clock_status",
			ops->set_sound_clock_status, TCORE_RETURN_ENOSYS);

		return ops->set_sound_clock_status(o, ur);

	case TREQ_CALL_SET_PREFERRED_VOICE_SUBSCRIPTION:
		tcore_check_null_ret_err("ops->set_preferred_voice_subscription",
			ops->set_preferred_voice_subscription, TCORE_RETURN_ENOSYS);

		return ops->set_preferred_voice_subscription(o, ur);

	case TREQ_CALL_GET_PREFERRED_VOICE_SUBSCRIPTION:
		tcore_check_null_ret_err("ops->get_preferred_voice_subscription",
			ops->get_preferred_voice_subscription, TCORE_RETURN_ENOSYS);

		return ops->get_preferred_voice_subscription(o, ur);

	case TREQ_CALL_MODIFY:
		tcore_check_null_ret_err("ops->modify",
			ops->modify, TCORE_RETURN_ENOSYS);

		return ops->modify(o, ur);

	case TREQ_CALL_CONFIRM_MODIFY:
		tcore_check_null_ret_err("ops->confirm_modify",
			ops->confirm_modify, TCORE_RETURN_ENOSYS);

		return ops->confirm_modify(o, ur);

	default:
		break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	g_slist_free_full(po->cobjs, g_free);
	g_free(po);

	tcore_object_link_object(o, NULL);
}

typedef gboolean(*func)(CallObject *co, void *data);
static CallObject *_find_object(GSList *objs, void *data, func compare)
{
	CallObject *co = NULL;
	GSList *l = NULL;

	tcore_check_null_ret_err("objs", objs, NULL);
	tcore_check_null_ret_err("compare", compare, NULL);

	l = objs;
	while (l) {
		co = (CallObject *) l->data;

		if (compare(co, data))
			return co;

		l = g_slist_next(l);
	}

	return NULL;
}

static GSList *_find_object_all(GSList *objs, void *data, func compare)
{
	CallObject *co = NULL;
	GSList *l = NULL;
	GSList *ret = NULL;

	tcore_check_null_ret_err("objs", objs, NULL);
	tcore_check_null_ret_err("compare", compare, NULL);

	l = objs;
	while (l) {
		co = (CallObject *) l->data;

		if (compare(co, data))
			ret = g_slist_append(ret, co);

		l = g_slist_next(l);
	}

	return ret;
}

static gboolean _compare_by_id(CallObject *co, void *data)
{
	unsigned int *id = (unsigned int *) data;

	tcore_check_null_ret_err("co", co, FALSE);
	tcore_check_null_ret_err("data", data, FALSE);

	if (co->id == *id)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_handle(CallObject *co, void *data)
{
	unsigned int *handle = (unsigned int *) data;

	tcore_check_null_ret_err("co", co, FALSE);
	tcore_check_null_ret_err("data", data, FALSE);

	if (co->handle == *handle)
		return TRUE;

	return FALSE;
}


static gboolean _compare_by_session_id(CallObject *co, void *data)
{
	int *session_id = data;

	tcore_check_null_ret_err("co", co, FALSE);
	tcore_check_null_ret_err("data", data, FALSE);

	if (co->session_id == *session_id)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_status(CallObject *co, void *data)
{
	enum tcore_call_status *ct = (enum tcore_call_status *) data;

	tcore_check_null_ret_err("co", co, FALSE);
	tcore_check_null_ret_err("data", data, FALSE);

	if (co->status == *ct)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_number(CallObject *co, void *data)
{
	char *number = (char *) data;

	tcore_check_null_ret_err("co", co, FALSE);
	tcore_check_null_ret_err("data", data, FALSE);

	if (!strcmp(co->cli.number, number))
		return TRUE;

	return FALSE;
}

static enum tcore_call_cli_mode _check_cli_mode_by_number(char *num)
{
	tcore_check_null_ret_err("num", num, TCORE_CALL_CLI_MODE_DEFAULT);

	if (!strncmp(num, "*31#", 4))
		return TCORE_CALL_CLI_MODE_PRESENT;

	if (!strncmp(num, "#31#", 4))
		return TCORE_CALL_CLI_MODE_RESTRICT;

	return TCORE_CALL_CLI_MODE_DEFAULT;
}


/* Call Object APIs */
CallObject *tcore_call_object_new(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CallObject *co = NULL;

	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, NULL);

	co = g_try_malloc0(sizeof(struct call_object));
	if (co == NULL)
		return NULL;

	co->handle  = __assign_handle(o);

	if (INVALID_HANDLE == co->handle) {
		err("valid handle not available. call object creation failed");
		g_free(co);
		return NULL;
	}
	co->id = -1;

	po->cobjs = g_slist_append(po->cobjs, co);

	dbg("new call object handle : [%d]", co->handle);

	return co;
}

gboolean tcore_call_object_free(CoreObject *o, CallObject *co)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, FALSE);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, FALSE);
	tcore_check_null_ret_err("co", co, FALSE);

	po->cobjs = g_slist_remove(po->cobjs, co);
	g_free(co);

	return TRUE;
}

int tcore_call_object_total_length(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, 0);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, 0);

	return (int)g_slist_length(po->cobjs);
}

CallObject *tcore_call_object_current_on_mt_processing(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CallObject *call_obj = NULL;
	GSList *l = NULL;

	enum tcore_call_status cs = TCORE_CALL_STATUS_INCOMING;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, NULL);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, NULL);

	l = _find_object_all(po->cobjs, (void *)&cs, (void *)_compare_by_status);
	if (!l) {
		cs = TCORE_CALL_STATUS_WAITING;
		l = _find_object_all(po->cobjs, (void *)&cs, (void *)_compare_by_status);
		if (!l)
			return 0;
	}

	call_obj = (CallObject *)(l->data);
	g_slist_free(l);

	return call_obj;
}

CallObject *tcore_call_object_current_on_mo_processing(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CallObject *call_obj = NULL;
	GSList *l = NULL;

	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, NULL);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, NULL);

	l = tcore_call_object_find_by_status(o, TCORE_CALL_STATUS_DIALING);
	if (!l) {
		l = tcore_call_object_find_by_status(o, TCORE_CALL_STATUS_ALERT);
		if (!l) {
			l = tcore_call_object_find_by_status(o, TCORE_CALL_STATUS_SETUP);
			if (!l) {
				l = tcore_call_object_find_by_status(o, TCORE_CALL_STATUS_SETUP_PENDING);
				if (!l)
					return NULL;
			}
		}
	}

	call_obj = (CallObject *)(l->data);
	g_slist_free(l);

	return call_obj;
}

int tcore_call_get_id_by_handle(CoreObject *o, int handle)
{
	CallObject *co = NULL;

	co = tcore_call_object_find_by_handle(o, handle);
	tcore_check_null_ret_err("co", co, -1);

	return tcore_call_object_get_id(co);
}

int tcore_call_get_handle_by_id(CoreObject *o, int call_id)
{
	CallObject *co = NULL;

	co = tcore_call_object_find_by_id(o, call_id);
	tcore_check_null_ret_err("co", co, -1);

	return tcore_call_object_get_handle(co);
}

CallObject *tcore_call_object_find_by_id(CoreObject *o, int id)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, NULL);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, NULL);

	return _find_object(po->cobjs, (void *) &id, (void *) _compare_by_id);
}

CallObject *tcore_call_object_find_by_handle(CoreObject *o, int handle)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, NULL);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, NULL);

	return _find_object(po->cobjs, (void *) &handle, (void *) _compare_by_handle);

}


CallObject *tcore_call_object_find_by_number(CoreObject *o, char *num)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, NULL);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, NULL);
	tcore_check_null_ret_err("num", num, NULL);

	return _find_object(po->cobjs, (void *) num, (void *) _compare_by_number);
}

GSList *tcore_call_object_find_by_status(CoreObject *o, enum tcore_call_status cs)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, NULL);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, NULL);

	return _find_object_all(po->cobjs, (void *) &cs, (void *) _compare_by_status);
}

int tcore_call_object_get_id(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->id;
}

gboolean tcore_call_object_set_id(CallObject *co , int call_id)
{
	tcore_check_null_ret_err("co", co, FALSE);
	co->id = call_id;

	return TRUE;
}

int tcore_call_object_get_handle(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->handle;
}


gboolean tcore_call_object_set_type(CallObject *co, enum tcore_call_type ct)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->type = ct;
	return TRUE;
}

enum tcore_call_type tcore_call_object_get_type(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->type;
}

gboolean tcore_call_object_set_direction(CallObject *co, enum tcore_call_direction cd)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->direction = cd;
	return TRUE;
}

enum tcore_call_direction tcore_call_object_get_direction(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->direction;
}

gboolean tcore_call_object_set_status(CallObject *co, enum tcore_call_status cs)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->status = cs;
	return TRUE;
}

enum tcore_call_status tcore_call_object_get_status(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->status;
}

gboolean tcore_call_object_set_cli_info(CallObject *co,
	enum tcore_call_cli_mode mode, enum tcore_call_no_cli_cause cause,
	char *num, int num_len)
{
	char *pos = NULL;

	tcore_check_null_ret_err("co", co, FALSE);

	dbg("num  : %s", num);
	dbg("mode : 0x%x", mode);

	if (!num) {
		co->cli.mode = mode;
		if (mode == TCORE_CALL_CLI_MODE_RESTRICT)
			co->cli.no_cli_cause = cause;
		else
			co->cli.no_cli_cause = TCORE_CALL_NO_CLI_CAUSE_NONE;

		co->cli.number_len = num_len ;
		co->cli.number[0] = '\0';

		return TRUE;
	}

	pos = num;

	if (mode == TCORE_CALL_CLI_MODE_DEFAULT) {
		co->cli.mode = _check_cli_mode_by_number(num);

		if (co->cli.mode != TCORE_CALL_CLI_MODE_DEFAULT)
			pos = (num + 4);
	} else {
		co->cli.mode = mode;
		if (mode == TCORE_CALL_CLI_MODE_RESTRICT)
			co->cli.no_cli_cause = cause;
		else
			co->cli.no_cli_cause = TCORE_CALL_NO_CLI_CAUSE_NONE;
	}

	strncpy(co->cli.number, pos, ((num_len - (pos - num)) + 1));
	co->cli.number_len = strlen(co->cli.number);

	dbg("co->cli.mode: [0x%x]", co->cli.mode);
	dbg("co->cli.no_cli_cause: [0x%x]", co->cli.no_cli_cause);
	dbg("co->cli.number: [%s]", co->cli.number);
	dbg("co->cli.number_len: [%d]", co->cli.number_len);

	return TRUE;
}

int tcore_call_object_get_number(CallObject *co, char *num)
{
	tcore_check_null_ret_err( "co", co, -1);
	tcore_check_null_ret_err( "num", num, -1);

	strncpy(num, co->cli.number, MAX_CALL_NUMBER_LEN);
	return co->cli.number_len;
}

enum tcore_call_cli_mode tcore_call_object_get_cli_mode(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->cli.mode;
}

enum tcore_call_no_cli_cause tcore_call_object_get_no_cli_cause(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->cli.no_cli_cause;
}


gboolean tcore_call_object_set_cna_info(CallObject *co, enum tcore_call_cna_mode mode, char *name, int dcs)
{
	int len;
	tcore_check_null_ret_err("co", co, FALSE);
	tcore_check_null_ret_err("name", name, FALSE);

	len = strlen(name);
	if (len >= MAX_CALL_NAME_LEN)
		return FALSE;

	strncpy(co->cna.name, name, len);
	co->cna.name[len] = '\0';

	co->cna.mode = mode;

	return TRUE;
}

int tcore_call_object_get_name(CallObject *co, char *name)
{
	tcore_check_null_ret_err( "co", co, -1);
	tcore_check_null_ret_err( "name", name, -1);

	strncpy(name, co->cna.name, MAX_CALL_NAME_LEN);
	return co->cna.name_len;
}

enum tcore_call_cna_mode tcore_call_object_get_cna_mode(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->cna.mode;
}

gboolean tcore_call_object_set_multiparty_state(CallObject *co, gboolean is)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->mpty = is;

	return TRUE;
}

gboolean tcore_call_object_get_multiparty_state(CallObject *co)
{
	tcore_check_null_ret_err("co", co, FALSE);

	return co->mpty;
}

gboolean tcore_call_object_set_active_line(CallObject *co, unsigned int line)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->active_line = line;

	return TRUE;
}

int tcore_call_object_get_active_line(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->active_line;
}

gboolean tcore_call_object_set_is_volte_call(CallObject *co, gboolean flag)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->is_volte_call = flag;

	return TRUE;
}

gboolean tcore_call_object_get_is_volte_call(CallObject *co)
{
	tcore_check_null_ret_err("co", co, FALSE);

	return co->is_volte_call;
}

gboolean tcore_call_object_set_session_id(CallObject *co, int session_id)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->session_id = session_id;

	return TRUE;
}

int tcore_call_object_get_session_id(CallObject *co)
{
	tcore_check_null_ret_err("co", co, FALSE);

	return co->session_id;
}

gboolean tcore_call_object_set_conf_call_session_id(CallObject *co, int session_id)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->conf_call_session_id = session_id;

	return TRUE;
}

gboolean tcore_call_object_get_conf_call_session_id(CallObject *co)
{
	tcore_check_null_ret_err("co", co, FALSE);

	return co->conf_call_session_id;
}

gboolean tcore_call_object_set_early_media(CallObject *co, gboolean flag)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->early_media = flag;

	return TRUE;
}

gboolean tcore_call_object_get_early_media(CallObject *co)
{
	tcore_check_null_ret_err("co", co, FALSE);

	return co->early_media;
}

CallObject *tcore_call_object_find_by_session_id(CoreObject *o, int session_id)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, NULL);
	tcore_check_null_ret_err("po->cobjs", po->cobjs, NULL);

	return _find_object(po->cobjs, &session_id, _compare_by_session_id);
}

gboolean tcore_call_object_check_cs_call_existence(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CallObject *call_obj = NULL;
	GSList *call_list = NULL;

	po = tcore_object_ref_object(o);
	tcore_check_null_ret_err("po", po, FALSE);

	call_list = po->cobjs;
	while (call_list) {
		call_obj = call_list->data;
		if (call_obj->is_volte_call == FALSE)
			return TRUE;

		call_list = g_slist_next(call_list);
	}

	return FALSE;
}

GSList *tcore_call_object_get_all_session_ids(CoreObject *o)
{
	struct private_object_data *po = NULL;
	CallObject *call_obj = NULL;
	GSList *call_list = NULL;
	GSList *session_ids = NULL;

	po = tcore_object_ref_object(o);
	tcore_check_null_ret_err("po", po, NULL);

	call_list = po->cobjs;
	while (call_list) {
		call_obj = call_list->data;
		if (call_obj->is_volte_call == TRUE)
			session_ids = g_slist_append(session_ids, GINT_TO_POINTER(call_obj->session_id));

		call_list = g_slist_next(call_list);
	}

	return session_ids;
}

gboolean tcore_call_object_set_is_release_pending(CallObject *co, gboolean flag)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->is_release_pending = flag;

	return TRUE;

}

gboolean tcore_call_object_get_is_release_pending(CallObject *co)
{
	tcore_check_null_ret_err("co", co, FALSE);

	return co->is_release_pending;
}

gboolean tcore_call_object_set_silent_redial_reason(CallObject *co, enum tcore_call_silent_redial_reason reason)
{
	tcore_check_null_ret_err("co", co, FALSE);

	co->redial_reason = reason;

	return TRUE;

}
enum tcore_call_silent_redial_reason tcore_call_object_get_silent_redial_reason(CallObject *co)
{
	tcore_check_null_ret_err("co", co, -1);

	return co->redial_reason;
}

TReturn tcore_call_control_answer_hold_and_accept(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->answer_hold_and_accept", po->cops->answer_hold_and_accept, TCORE_RETURN_ENOSYS);

	return po->cops->answer_hold_and_accept(o, ur, cb, user_data);
}

TReturn tcore_call_control_answer_replace(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->answer_replace", po->cops->answer_replace, TCORE_RETURN_ENOSYS);

	return po->cops->answer_replace(o, ur, cb, user_data);
}

TReturn tcore_call_control_answer_reject(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->answer_reject", po->cops->answer_reject, TCORE_RETURN_ENOSYS);

	return po->cops->answer_reject(o, ur, cb, user_data);
}

TReturn tcore_call_control_end_specific(CoreObject *o, UserRequest* ur, const int id, ConfirmCallback cb,
		void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->end_specific", po->cops->end_specific, TCORE_RETURN_ENOSYS);

	return po->cops->end_specific(o, ur, id, cb, user_data);
}

TReturn tcore_call_control_end_all_active(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->end_all_active", po->cops->end_all_active, TCORE_RETURN_ENOSYS);

	return po->cops->end_all_active(o, ur, cb, user_data);
}

TReturn tcore_call_control_end_all_held(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->end_all_held", po->cops->end_all_held, TCORE_RETURN_ENOSYS);

	return po->cops->end_all_held(o, ur, cb, user_data);
}

TReturn tcore_call_control_active(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->active", po->cops->active, TCORE_RETURN_ENOSYS);

	return po->cops->active(o, ur, cb, user_data);
}

TReturn tcore_call_control_hold(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->hold", po->cops->hold, TCORE_RETURN_ENOSYS);

	return po->cops->hold(o, ur, cb, user_data);
}

TReturn tcore_call_control_swap(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->swap", po->cops->swap, TCORE_RETURN_ENOSYS);

	return po->cops->swap(o, ur, cb, user_data);
}

TReturn tcore_call_control_join(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->join", po->cops->join, TCORE_RETURN_ENOSYS);

	return po->cops->join(o, ur, cb, user_data);
}

TReturn tcore_call_control_split(CoreObject *o, UserRequest* ur, const int id, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->split", po->cops->split, TCORE_RETURN_ENOSYS);

	return po->cops->split(o, ur, id, cb, user_data);
}

TReturn tcore_call_control_transfer(CoreObject *o, UserRequest* ur, ConfirmCallback cb, void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->transfer", po->cops->transfer, TCORE_RETURN_ENOSYS);

	return po->cops->transfer(o, ur, cb, user_data);
}

TReturn tcore_call_control_deflect(CoreObject *o, UserRequest* ur, const char *number, ConfirmCallback cb,
		void *user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops", po->cops, TCORE_RETURN_FAILURE);
	tcore_check_null_ret_err("po->cops->deflect", po->cops->deflect, TCORE_RETURN_ENOSYS);

	return po->cops->deflect(o, ur, number, cb, user_data);
}

void tcore_call_control_set_operations(CoreObject *o, struct tcore_call_control_operations *cops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->cops = cops;
}

void tcore_call_information_mo_col(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_col", po->iops->mo_call_col);

	po->iops->mo_call_col(o, number);
}

void tcore_call_information_mo_waiting(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_waiting", po->iops->mo_call_waiting);

	po->iops->mo_call_waiting(o);
}

void tcore_call_information_mo_cug(CoreObject *o, int cug_index)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_cug", po->iops->mo_call_cug);

	po->iops->mo_call_cug(o, cug_index);
}

void tcore_call_information_mo_forwarded(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_forwarded", po->iops->mo_call_forwarded);

	po->iops->mo_call_forwarded(o);
}

void tcore_call_information_mo_barred_incoming(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_barred_incoming", po->iops->mo_call_barred_incoming);

	po->iops->mo_call_barred_incoming(o);
}

void tcore_call_information_mo_barred_outgoing(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_barred_outgoing", po->iops->mo_call_barred_outgoing);

	po->iops->mo_call_barred_outgoing(o);
}

void tcore_call_information_mo_deflected(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_deflected", po->iops->mo_call_deflected);

	po->iops->mo_call_deflected(o);
}

void tcore_call_information_mo_clir_suppression_reject(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_clir_suppression_reject", po->iops->mo_call_clir_suppression_reject);

	po->iops->mo_call_clir_suppression_reject(o);
}

void tcore_call_information_mo_cfu(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_cfu", po->iops->mo_call_cfu);

	po->iops->mo_call_cfu(o);
}

void tcore_call_information_mo_cfc(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mo_call_cfc", po->iops->mo_call_cfc);

	po->iops->mo_call_cfc(o);
}

void tcore_call_information_mt_cli(CoreObject *o, enum tcore_call_cli_mode mode, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mt_call_cli", po->iops->mt_call_cli);

	po->iops->mt_call_cli(o, mode, number);
}

void tcore_call_information_mt_cna(CoreObject *o, enum tcore_call_cna_mode mode, char *name, int dcs)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mt_call_cna", po->iops->mt_call_cna);

	po->iops->mt_call_cna(o, mode, name, dcs);
}

void tcore_call_information_mt_forwarded_call(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mt_call_forwarded_call", po->iops->mt_call_forwarded_call);

	po->iops->mt_call_forwarded_call(o, number);
}

void tcore_call_information_mt_cug_call(CoreObject *o, int cug_index, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mt_call_cug_call", po->iops->mt_call_cug_call);

	po->iops->mt_call_cug_call(o, cug_index, number);
}

void tcore_call_information_mt_deflected_call(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mt_call_deflected_call", po->iops->mt_call_deflected_call);

	po->iops->mt_call_deflected_call(o, number);
}

void tcore_call_information_mt_transfered(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->mt_call_transfered", po->iops->mt_call_transfered);

	po->iops->mt_call_transfered(o, number);
}

void tcore_call_information_held(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->call_held", po->iops->call_held);

	po->iops->call_held(o, number);
}

void tcore_call_information_active(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->call_active", po->iops->call_active);

	po->iops->call_active(o, number);
}

void tcore_call_information_joined(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->call_joined", po->iops->call_joined);

	po->iops->call_joined(o, number);
}

void tcore_call_information_released_on_hold(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->call_released_on_hold", po->iops->call_released_on_hold);

	po->iops->call_released_on_hold(o, number);
}

void tcore_call_information_transfer_alert(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->call_transfer_alert", po->iops->call_transfer_alert);

	po->iops->call_transfer_alert(o, number);
}

void tcore_call_information_transfered(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->call_transfered", po->iops->call_transfered);

	po->iops->call_transfered(o, number);
}

void tcore_call_information_cf_check_ss_message(CoreObject *o, char *number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	tcore_check_null_ret("po", po);
	tcore_check_null_ret("po->iops", po->iops);
	tcore_check_null_ret("po->iops->call_cf_check_message", po->iops->call_cf_check_message);

	po->iops->call_cf_check_message(o, number);
}

void tcore_call_information_set_operations(CoreObject *o, struct tcore_call_information_operations *iops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	po->iops = iops;
}

CoreObject *tcore_call_new(TcorePlugin *p, const char *name, struct tcore_call_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name, hal);
	if (!o)
		return NULL;

	po = g_try_malloc0(sizeof(struct private_object_data));
	if (po == NULL) {
		tcore_object_free(o);
		return NULL;
	}

	/* set ops to default type when core object is created. */
	po->ops[TCORE_OPS_TYPE_CP] = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_CALL);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_call_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);
	tcore_object_free(o);
}

void tcore_call_set_ops(CoreObject *o, struct tcore_call_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	/* set ops according to ops_type */
	po->ops[ops_type] = ops;
}
