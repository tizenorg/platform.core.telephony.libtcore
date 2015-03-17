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
#include "plugin.h"
#include "queue.h"
#include "user_request.h"
#include "co_call.h"


#define MAX_CALL_OBJECTS		6

#define _check_null( name, value, err ) { \
	if ( !value ) { \
		dbg("[error] %s : NULL", name ); \
		return err; \
	} \
}

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
	unsigned int id;
	enum tcore_call_direction direction;
	enum tcore_call_status status;
	gboolean mpty;
	struct call_cli_info cli;
	struct call_cna_info cna;
	unsigned int cug_id;
	unsigned int active_line;
	struct call_time { // second
		long start;
		long end;
	} time;
	gboolean is_volte_call;
	int session_id; // VoLTE only
	int conf_call_session_id; // Conference call session-id (VoLTE only)
	gboolean early_media; // VoLTE only
};

struct private_object_data {
	GSList* cobjs;
	struct tcore_call_operations *ops;
	struct tcore_call_control_operations *cops;
	struct tcore_call_information_operations *iops;
};

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->ops", po->ops, TCORE_RETURN_FAILURE);
	_check_null( "ur", ur, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_CALL_DIAL:
			_check_null( "po->ops->dial", po->ops->dial, TCORE_RETURN_FAILURE);
			return po->ops->dial(o, ur);

		case TREQ_CALL_ANSWER:
			_check_null( "po->ops->answer", po->ops->answer, TCORE_RETURN_FAILURE);
			return po->ops->answer(o, ur);

		case TREQ_CALL_END:
			_check_null( "po->ops->end", po->ops->end, TCORE_RETURN_FAILURE);
			return po->ops->end(o, ur);

		case TREQ_CALL_HOLD:
			_check_null( "po->ops->hold", po->ops->hold, TCORE_RETURN_FAILURE);
			return po->ops->hold(o, ur);

		case TREQ_CALL_ACTIVE:
			_check_null( "po->ops->active", po->ops->active, TCORE_RETURN_FAILURE);
			return po->ops->active(o, ur);

		case TREQ_CALL_SWAP:
			_check_null( "po->ops->swap", po->ops->swap, TCORE_RETURN_FAILURE);
			return po->ops->swap(o, ur);

		case TREQ_CALL_JOIN:
			_check_null( "po->ops->join", po->ops->join, TCORE_RETURN_FAILURE);
			return po->ops->join(o, ur);

		case TREQ_CALL_SPLIT:
			_check_null( "po->ops->split", po->ops->split, TCORE_RETURN_FAILURE);
			return po->ops->split(o, ur);

		case TREQ_CALL_DEFLECT:
			_check_null( "po->ops->deflect", po->ops->deflect, TCORE_RETURN_FAILURE);
			return po->ops->deflect(o, ur);

		case TREQ_CALL_TRANSFER:
			_check_null( "po->ops->transfer", po->ops->transfer, TCORE_RETURN_FAILURE);
			return po->ops->transfer(o, ur);

		case TREQ_CALL_START_CONT_DTMF:
			_check_null( "po->ops->start_cont_dtmf", po->ops->start_cont_dtmf, TCORE_RETURN_FAILURE);
			return po->ops->start_cont_dtmf(o, ur);

		case TREQ_CALL_STOP_CONT_DTMF:
			_check_null( "po->ops->stop_cont_dtmf", po->ops->stop_cont_dtmf, TCORE_RETURN_FAILURE);
			return po->ops->stop_cont_dtmf(o, ur);

		case TREQ_CALL_SEND_BURST_DTMF:
			_check_null( "po->ops->send_burst_dtmf", po->ops->send_burst_dtmf, TCORE_RETURN_FAILURE);
			return po->ops->send_burst_dtmf(o, ur);

		case TREQ_CALL_GET_PRIVACY_MODE:
			_check_null( "po->ops->get_privacy_mode", po->ops->get_privacy_mode, TCORE_RETURN_FAILURE);
			return po->ops->get_privacy_mode(o, ur);

		case TREQ_CALL_SET_PRIVACY_MODE:
			_check_null( "po->ops->set_privacy_mode", po->ops->set_privacy_mode, TCORE_RETURN_FAILURE);
			return po->ops->set_privacy_mode(o, ur);

		case TREQ_CALL_SET_SOUND_PATH:
			_check_null( "po->ops->set_sound_path", po->ops->set_sound_path, TCORE_RETURN_FAILURE);
			return po->ops->set_sound_path(o, ur);

		case TREQ_CALL_GET_SOUND_VOLUME_LEVEL:
			_check_null( "po->ops->get_sound_volume_level", po->ops->get_sound_volume_level, TCORE_RETURN_FAILURE);
			return po->ops->get_sound_volume_level(o, ur);

		case TREQ_CALL_SET_SOUND_VOLUME_LEVEL:
			_check_null( "po->ops->set_sound_volume_level", po->ops->set_sound_volume_level, TCORE_RETURN_FAILURE);
			return po->ops->set_sound_volume_level(o, ur);

		case TREQ_CALL_SET_SOUND_MUTE_STATUS:
			_check_null( "po->ops->set_sound_mute_status", po->ops->set_sound_mute_status, TCORE_RETURN_FAILURE);
			return po->ops->set_sound_mute_status(o, ur);

		case TREQ_CALL_GET_SOUND_MUTE_STATUS:
			_check_null( "po->ops->get_sound_mute_status", po->ops->get_sound_mute_status, TCORE_RETURN_FAILURE);
			return po->ops->get_sound_mute_status(o, ur);

		case TREQ_CALL_SET_SOUND_RECORDING:
			_check_null( "po->ops->set_sound_recording", po->ops->set_sound_recording, TCORE_RETURN_FAILURE);
			return po->ops->set_sound_recording(o, ur);

		case TREQ_CALL_SET_SOUND_EQUALIZATION:
			_check_null( "po->ops->set_sound_equalization", po->ops->set_sound_equalization, TCORE_RETURN_FAILURE);
			return po->ops->set_sound_equalization(o, ur);

		case TREQ_CALL_SET_SOUND_NOISE_REDUCTION:
			_check_null( "po->ops->set_sound_noise_reduction", po->ops->set_sound_noise_reduction, TCORE_RETURN_FAILURE);
			return po->ops->set_sound_noise_reduction(o, ur);

		case TREQ_CALL_SET_SOUND_CLOCK_STATUS:
			_check_null( "po->ops->set_sound_clock_status", po->ops->set_sound_clock_status, TCORE_RETURN_FAILURE);
			return po->ops->set_sound_clock_status(o, ur);

		case TREQ_CALL_SET_PREFERRED_VOICE_SUBSCRIPTION:
			_check_null( "po->ops->set_preferred_voice_subscription", po->ops->set_preferred_voice_subscription, TCORE_RETURN_FAILURE);
			return po->ops->set_preferred_voice_subscription(o, ur);

		case TREQ_CALL_GET_PREFERRED_VOICE_SUBSCRIPTION:
			_check_null( "po->ops->get_preferred_voice_subscription", po->ops->get_preferred_voice_subscription, TCORE_RETURN_FAILURE);
			return po->ops->get_preferred_voice_subscription(o, ur);

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

typedef gboolean(*func)(struct call_object* co, void *data);
static struct call_object *_find_object(GSList *objs, void* data, func compare)
{
	struct call_object *co = 0;
	GSList *l = 0;

	_check_null( "objs", objs, 0);
	_check_null( "compare", compare, 0);

	l = objs;
	while (l) {
		co = (struct call_object*) l->data;

		if (compare(co, data))
			return co;

		l = g_slist_next( l );
	}

	return NULL;
}

static GSList* _find_object_all(GSList *objs, void* data, func compare)
{
	struct call_object *co = 0;
	GSList *l = 0;
	GSList *ret = 0;

	_check_null( "objs", objs, 0);
	_check_null( "compare", compare, 0);

	l = objs;
	while (l) {
		co = (struct call_object*) l->data;

		if (compare(co, data))
			ret = g_slist_append(ret, co);

		l = g_slist_next( l );
	}

	return ret;
}

static gboolean _compare_by_id(struct call_object* co, void *data)
{
	unsigned int *id = (unsigned int*) data;

	_check_null( "co", co, FALSE);
	_check_null( "data", data, FALSE);

	if (co->id == *id)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_session_id(struct call_object *co, void *data)
{
	int *session_id = data;

	_check_null("co", co, FALSE);
	_check_null("data", data, FALSE);

	if (co->session_id == *session_id)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_status(struct call_object* co, void *data)
{
	enum tcore_call_status *ct = (enum tcore_call_status*) data;

	_check_null( "co", co, FALSE);
	_check_null( "data", data, FALSE);

	if (co->status == *ct)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_number(struct call_object* co, void *data)
{
	char *number = (char*) data;

	_check_null( "co", co, FALSE);
	_check_null( "data", data, FALSE);

	if (!strcmp(co->cli.number, number))
		return TRUE;

	return FALSE;
}

static enum tcore_call_cli_mode _check_cli_mode_by_number(char *num)
{
	_check_null( "num", num, TCORE_CALL_CLI_MODE_DEFAULT);

	if (!strncmp(num, "*31#", 4))
		return TCORE_CALL_CLI_MODE_PRESENT;

	if (!strncmp(num, "#31#", 4))
		return TCORE_CALL_CLI_MODE_RESTRICT;

	return TCORE_CALL_CLI_MODE_DEFAULT;
}


// Call Object API

struct call_object *tcore_call_object_new(CoreObject *o, int id)
{
	struct private_object_data *po = 0;
	struct call_object *co = 0;

	po = tcore_object_ref_object(o);

	_check_null( "po", po, 0);

	co = g_new0( struct call_object, 1 );

	if (id > 0) {
		if (!_find_object(po->cobjs, (void*) &id, (void*) _compare_by_id))
			co->id = id;
		else {
			dbg("[ error ] call object exist already. [ %d ]", id);
			g_free(co);
			return NULL;
		}
	}
	else {
		int i = 0;

		for (i = 1; i <= MAX_CALL_OBJECTS; i++) {	/* 6 is MAX call count */
			if (!_find_object(po->cobjs, (void*) &i, (void*) _compare_by_id)) {
				co->id = i;
				break;
			}
		}

		 /* Free the allocated Core Object if ID is not allocated */
		if (i > MAX_CALL_OBJECTS) {
			err("[ error ] failed to assign call id");
			g_free(co);
			return NULL;
		}
	}

	po->cobjs = g_slist_append(po->cobjs, co);

	dbg("new call object id : [%d]", co->id);

	return co;
}

gboolean tcore_call_object_free(CoreObject *o, struct call_object *co)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, FALSE);
	_check_null( "po->cobjs", po->cobjs, FALSE);
	_check_null( "co", co, FALSE);

	po->cobjs = g_slist_remove(po->cobjs, co);
	g_free(co);

	return TRUE;
}

int tcore_call_object_total_length( CoreObject *o )
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, FALSE);
	_check_null( "po->cobjs", po->cobjs, FALSE);

	return (int)g_slist_length(po->cobjs);
}

struct call_object *tcore_call_object_current_on_mt_processing(CoreObject *o)
{
	struct private_object_data *po = NULL;
	struct call_object *call_obj = NULL;
	GSList *l = 0;

	enum tcore_call_status cs = CALL_STATUS_INCOMING;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, 0);
	_check_null( "po->cobjs", po->cobjs, 0);

	l = _find_object_all(po->cobjs, (void*) &cs, (void*) _compare_by_status);
	if (!l) {
		cs = CALL_STATUS_WAITING;
		l = _find_object_all(po->cobjs, (void*) &cs, (void*) _compare_by_status);
		if (!l)
			return 0;
	}

	call_obj = (struct call_object*) l ->data;
	g_slist_free(l);
	return call_obj;
}

struct call_object *tcore_call_object_current_on_mo_processing(CoreObject *o)
{
	struct private_object_data *po = NULL;
	struct call_object *call_obj = NULL;
	GSList *l = 0;

	enum tcore_call_status cs = CALL_STATUS_DIALING;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, 0);
	_check_null( "po->cobjs", po->cobjs, 0);

	l = _find_object_all(po->cobjs, (void*) &cs, (void*) _compare_by_status);
	if (!l) {
		cs = CALL_STATUS_ALERT;
		l = _find_object_all(po->cobjs, (void*) &cs, (void*) _compare_by_status);
		if (!l)
			return 0;
	}

	call_obj = (struct call_object*) l ->data;
	g_slist_free(l);
	return call_obj;
}

struct call_object *tcore_call_object_find_by_id(CoreObject *o, int id)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);

	_check_null( "po", po, 0);
	_check_null( "po->cobjs", po->cobjs, 0);

	return _find_object(po->cobjs, (void*) &id, (void*) _compare_by_id);
}

struct call_object *tcore_call_object_find_by_number(CoreObject *o, char *num)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);

	_check_null( "po", po, 0);
	_check_null( "po->cobjs", po->cobjs, 0);
	_check_null( "num", num, 0);

	return _find_object(po->cobjs, (void*) num, (void*) _compare_by_number);
}

GSList* tcore_call_object_find_by_status(CoreObject *o, enum tcore_call_status cs)
{
	struct private_object_data *po = NULL;

	po = tcore_object_ref_object(o);

	_check_null( "po", po, 0);
	_check_null( "po->cobjs", po->cobjs, 0);

	return _find_object_all(po->cobjs, (void*) &cs, (void*) _compare_by_status);
}

int tcore_call_object_get_id(struct call_object *co)
{
	_check_null( "co", co, -1);

	return co->id;
}

gboolean tcore_call_object_set_type(struct call_object *co, enum tcore_call_type ct)
{
	_check_null( "co", co, FALSE);

	co->type = ct;
	return TRUE;
}

enum tcore_call_type tcore_call_object_get_type(struct call_object *co)
{
	_check_null( "co", co, -1);

	return co->type;
}

gboolean tcore_call_object_set_direction(struct call_object *co, enum tcore_call_direction cd)
{
	_check_null( "co", co, FALSE);

	co->direction = cd;
	return TRUE;
}

enum tcore_call_direction tcore_call_object_get_direction(struct call_object *co)
{
	_check_null( "co", co, -1);

	return co->direction;
}

gboolean tcore_call_object_set_status(struct call_object *co, enum tcore_call_status cs)
{
	_check_null( "co", co, FALSE);

	co->status = cs;
	return TRUE;
}

enum tcore_call_status tcore_call_object_get_status(struct call_object *co)
{
	_check_null( "co", co, -1);

	return co->status;
}

gboolean tcore_call_object_set_cli_info(struct call_object *co,
						enum tcore_call_cli_mode mode, enum tcore_call_no_cli_cause cause,
						char *num, int num_len)
{
	char *pos = 0;

	_check_null( "co", co, FALSE);

	dbg("num  : %s", num);
	dbg("mode : 0x%x", mode);

	if (!num) {
		co->cli.mode = mode;
		if (mode == TCORE_CALL_CLI_MODE_RESTRICT)
			co->cli.no_cli_cause = cause;
		else
			co->cli.no_cli_cause = TCORE_CALL_NO_CLI_CAUSE_NONE;

		co->cli.number_len = num_len ;
		co->cli.number [0] = '\0';

		return TRUE;
	}

	pos = num;

	if (mode == TCORE_CALL_CLI_MODE_DEFAULT) {
		co->cli.mode = _check_cli_mode_by_number(num);

		if (co->cli.mode != TCORE_CALL_CLI_MODE_DEFAULT)
			pos = (num + 4);
	}
	else {
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

int tcore_call_object_get_number(struct call_object *co, char *num)
{
	_check_null( "co", co, -1);
	_check_null( "num", num, -1);

	strncpy(num, co->cli.number, MAX_CALL_NUMBER_LEN);
	return co->cli.number_len;
}

enum tcore_call_cli_mode tcore_call_object_get_cli_mode(struct call_object *co)
{
	_check_null( "co", co, -1);
	return co->cli.mode;
}

enum tcore_call_no_cli_cause tcore_call_object_get_no_cli_cause(struct call_object *co)
{
	_check_null( "co", co, -1);
	return co->cli.no_cli_cause;
}


gboolean tcore_call_object_set_cna_info(struct call_object *co, enum tcore_call_cna_mode mode, char *name, int dcs)
{
	int len;
	_check_null( "co", co, FALSE);
	_check_null( "name", name, FALSE);

	len = strlen(name);
	if (len >= MAX_CALL_NAME_LEN)
		return FALSE;

	strncpy(co->cna.name, name, len);
	co->cna.name[len] = '\0';

	co->cna.mode = mode;

	return TRUE;
}

int tcore_call_object_get_name(struct call_object *co, char *name)
{
	_check_null( "co", co, -1);
	_check_null( "name", name, -1);

	strncpy(name, co->cna.name, MAX_CALL_NAME_LEN);
	return co->cna.name_len;
}

enum tcore_call_cna_mode tcore_call_object_get_cna_mode(struct call_object *co)
{
	_check_null( "co", co, -1);

	return co->cna.mode;
}

gboolean tcore_call_object_set_multiparty_state(struct call_object *co, gboolean is)
{
	_check_null( "co", co, FALSE);

	co->mpty = is;
	return TRUE;
}

gboolean tcore_call_object_get_multiparty_state(struct call_object *co)
{
	_check_null( "co", co, FALSE);

	return co->mpty;
}

gboolean tcore_call_object_set_active_line(struct call_object *co, unsigned int line)
{
	_check_null( "co", co, FALSE);

	co->active_line = line;
	return TRUE;
}

int tcore_call_object_get_active_line(struct call_object *co)
{
	_check_null( "co", co, -1);

	return co->active_line;
}

gboolean tcore_call_object_set_is_volte_call(struct call_object *co, gboolean flag)
{
	_check_null( "co", co, FALSE);

	co->is_volte_call = flag;
	return TRUE;
}

gboolean tcore_call_object_get_is_volte_call(struct call_object *co)
{
	_check_null( "co", co, FALSE);

	return co->is_volte_call;
}

gboolean tcore_call_object_set_session_id(struct call_object *co, int session_id)
{
	_check_null( "co", co, FALSE);

	co->session_id = session_id;
	return TRUE;
}

int tcore_call_object_get_session_id(struct call_object *co)
{
	_check_null( "co", co, FALSE);

	return co->session_id;
}

gboolean tcore_call_object_set_conf_call_session_id(struct call_object *co, int session_id)
{
	_check_null( "co", co, FALSE);

	co->conf_call_session_id = session_id;
	return TRUE;
}

gboolean tcore_call_object_get_conf_call_session_id(struct call_object *co)
{
	_check_null( "co", co, FALSE);

	return co->conf_call_session_id;
}

gboolean tcore_call_object_set_early_media(struct call_object *co, gboolean flag)
{
	_check_null( "co", co, FALSE);

	co->early_media = flag;
	return TRUE;
}

gboolean tcore_call_object_get_early_media(struct call_object *co)
{
	_check_null( "co", co, FALSE);

	return co->early_media;
}

struct call_object *tcore_call_object_find_by_session_id(CoreObject *o, int session_id)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, 0);
	_check_null( "po->cobjs", po->cobjs, 0);

	return _find_object(po->cobjs, &session_id, _compare_by_session_id);
}

gboolean tcore_call_object_check_cs_call_existence(CoreObject *o)
{
	struct private_object_data *po = NULL;
	struct call_object *call_obj = NULL;
	GSList *call_list = NULL;

	po = tcore_object_ref_object(o);
	_check_null("po", po, FALSE);

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
	struct call_object *call_obj = NULL;
	GSList *call_list = NULL;
	GSList *session_ids = NULL;

	po = tcore_object_ref_object(o);
	_check_null("po", po, FALSE);

	call_list = po->cobjs;
	while (call_list) {
		call_obj = call_list->data;
		if (call_obj->is_volte_call == TRUE)
			session_ids = g_slist_append(session_ids, (gpointer)call_obj->session_id);

		call_list = g_slist_next(call_list);
	}

	return session_ids;
}

TReturn tcore_call_control_answer_hold_and_accept(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->answer_hold_and_accept(o, ur, cb, user_data);
}

TReturn tcore_call_control_answer_replace(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->answer_replace(o, ur, cb, user_data);
}

TReturn tcore_call_control_answer_reject(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->answer_reject(o, ur, cb, user_data);
}

TReturn tcore_call_control_end_specific(CoreObject* o, UserRequest* ur, const int id, ConfirmCallback cb,
		void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->end_specific(o, ur, id, cb, user_data);
}

TReturn tcore_call_control_end_all_active(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->end_all_active(o, ur, cb, user_data);
}

TReturn tcore_call_control_end_all_held(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->end_all_held(o, ur, cb, user_data);
}

TReturn tcore_call_control_active(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->active(o, ur, cb, user_data);
}

TReturn tcore_call_control_hold(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->hold(o, ur, cb, user_data);
}

TReturn tcore_call_control_swap(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->swap(o, ur, cb, user_data);
}

TReturn tcore_call_control_join(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->join(o, ur, cb, user_data);
}

TReturn tcore_call_control_split(CoreObject* o, UserRequest* ur, const int id, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->split(o, ur, id, cb, user_data);
}

TReturn tcore_call_control_transfer(CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->transfer(o, ur, cb, user_data);
}

TReturn tcore_call_control_deflect(CoreObject* o, UserRequest* ur, const char* number, ConfirmCallback cb,
		void* user_data)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->deflect(o, ur, number, cb, user_data);
}

void tcore_call_control_set_operations(CoreObject *o, struct tcore_call_control_operations *ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	po->cops = ops;
}

void tcore_call_information_mo_col(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po, );
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_col(o, number);
}

void tcore_call_information_mo_waiting(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po, );
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_waiting(o);
}

void tcore_call_information_mo_cug(CoreObject *o, int cug_index)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po, );
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_cug(o, cug_index);
}

void tcore_call_information_mo_forwarded(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_forwarded(o);
}

void tcore_call_information_mo_barred_incoming(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_barred_incoming(o);
}

void tcore_call_information_mo_barred_outgoing(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_barred_outgoing(o);
}

void tcore_call_information_mo_deflected(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_deflected(o);
}

void tcore_call_information_mo_clir_suppression_reject(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_clir_suppression_reject(o);
}

void tcore_call_information_mo_cfu(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_cfu(o);
}

void tcore_call_information_mo_cfc(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mo_call_cfc(o);
}

void tcore_call_information_mt_cli(CoreObject *o, enum tcore_call_cli_mode mode, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po, );
	_check_null( "po->iops", po->iops,);

	po->iops->mt_call_cli(o, mode, number);
}

void tcore_call_information_mt_cna(CoreObject *o, enum tcore_call_cna_mode mode, char* name, int dcs)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mt_call_cna(o, mode, name, dcs);
}

void tcore_call_information_mt_forwarded_call(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mt_call_forwarded_call(o, number);
}

void tcore_call_information_mt_cug_call(CoreObject *o, int cug_index, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mt_call_cug_call(o, cug_index, number);
}

void tcore_call_information_mt_deflected_call(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mt_call_deflected_call(o, number);
}

void tcore_call_information_mt_transfered(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->mt_call_transfered(o, number);
}

void tcore_call_information_held(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->call_held(o, number);
}

void tcore_call_information_active(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->call_active(o, number);
}

void tcore_call_information_joined(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->call_joined(o, number);
}

void tcore_call_information_released_on_hold(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->call_released_on_hold(o, number);
}

void tcore_call_information_transfer_alert(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->call_transfer_alert(o, number);
}

void tcore_call_information_transfered(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->call_transfered(o, number);
}

void tcore_call_information_cf_check_ss_message(CoreObject *o, char* number)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);

	_check_null( "po", po,);
	_check_null( "po->iops", po->iops,);

	po->iops->call_cf_check_message(o, number);
}

void tcore_call_information_set_operations(CoreObject *o, struct tcore_call_information_operations *ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	po->iops = ops;
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

	po = g_try_new0(struct private_object_data, 1);
	if (po == NULL) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

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

void tcore_call_set_ops(CoreObject *o, struct tcore_call_operations *ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		return;
	}

	po->ops = ops;
}
