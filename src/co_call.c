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


#define _check_null( name, value, err ) { \
	if ( !value ) { \
		dbg("[error] %s : NULL", name ); \
		return err; \
	} \
}

#define CALL_OPS(ops, name, co, ur) \
	((NULL == ops->name) ? TCORE_RETURN_ENOSYS : ops->name(co, ur))

struct call_cli_info {
	enum tcore_call_cli_mode mode;
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
};

struct private_object_data {
	GSList* cobjs;
	struct tcore_call_operations *ops;
	struct tcore_call_control_operations *cops;
	struct tcore_call_information_operations *iops;
};

static void _clone_hook(CoreObject *src, CoreObject *dest)
{
	struct private_object_data *src_po = NULL;
	struct private_object_data *dest_po = NULL;

	if (!src || !dest)
		return;

	dest_po = calloc(sizeof(struct private_object_data), 1);
	if (!dest_po) {
		tcore_object_link_object(dest, NULL);
		return;
	}

	src_po = tcore_object_ref_object(src);
	dest_po->ops = src_po->ops;

	tcore_object_link_object(dest, dest_po);
}

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
		return CALL_OPS(po->ops, dial, o, ur);

	case TREQ_CALL_ANSWER:
		return CALL_OPS(po->ops, answer, o, ur);

	case TREQ_CALL_END:
		return CALL_OPS(po->ops, end, o, ur);

	case TREQ_CALL_HOLD:
		return CALL_OPS(po->ops, hold, o, ur);

	case TREQ_CALL_ACTIVE:
		return CALL_OPS(po->ops, active, o, ur);

	case TREQ_CALL_SWAP:
		return CALL_OPS(po->ops, swap, o, ur);

	case TREQ_CALL_JOIN:
		return CALL_OPS(po->ops, join, o, ur);

	case TREQ_CALL_SPLIT:
		return CALL_OPS(po->ops, split, o, ur);

	case TREQ_CALL_DEFLECT:
		return CALL_OPS(po->ops, deflect, o, ur);

	case TREQ_CALL_TRANSFER:
		return CALL_OPS(po->ops, transfer, o, ur);

	case TREQ_CALL_SEND_DTMF:
		return CALL_OPS(po->ops, send_dtmf, o, ur);

	case TREQ_CALL_SET_SOUND_PATH:
		return CALL_OPS(po->ops, set_sound_path, o, ur);

	case TREQ_CALL_GET_SOUND_VOLUME_LEVEL:
		return CALL_OPS(po->ops, get_sound_volume_level, o, ur);

	case TREQ_CALL_SET_SOUND_VOLUME_LEVEL:
		return CALL_OPS(po->ops, set_sound_volume_level, o, ur);

	case TREQ_CALL_MUTE:
		return CALL_OPS(po->ops, mute, o, ur);

	case TREQ_CALL_UNMUTE:
		return CALL_OPS(po->ops, unmute, o, ur);

	case TREQ_CALL_GET_MUTE_STATUS:
		return CALL_OPS(po->ops, get_mute_status, o, ur);

	case TREQ_CALL_SET_SOUND_RECORDING:
		return CALL_OPS(po->ops, set_sound_recording, o, ur);

	case TREQ_CALL_SET_SOUND_EQUALIZATION:
		return CALL_OPS(po->ops, set_sound_equalization, o, ur);

	case TREQ_CALL_SET_SOUND_NOISE_REDUCTION:
		return CALL_OPS(po->ops, set_sound_noise_reduction, o, ur);

	case TREQ_CALL_SET_SOUND_CLOCK_STATUS:
		return CALL_OPS(po->ops, set_sound_clock_status, o, ur);

	default:
		break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;
	GSList *list;

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	if (po->cobjs) {
		for (list = po->cobjs; list != NULL; list = list->next) {
			if (list->data)
				free(list->data);

			list->data = NULL;
		}

		g_slist_free(po->cobjs);
		po->cobjs = NULL;
	}

	free(po);
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
			return 0;
		}
	}
	else {
		int i = 0;

		for (i = 1; i < 6; i++) {
			// 6 is MAX call count
			if (!_find_object(po->cobjs, (void*) &i, (void*) _compare_by_id)) {
				co->id = i;
				break;
			}
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

	if (!co) {
		dbg("co is not free");
		g_free(co);
	}
	else
		dbg("co is also free");

	return TRUE;
}

struct call_object *tcore_call_object_current_on_mt_processing(CoreObject *o)
{
	struct private_object_data *po = NULL;
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

	return (struct call_object*) l->data;
}

struct call_object *tcore_call_object_current_on_mo_processing(CoreObject *o)
{
	struct private_object_data *po = NULL;
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

	return (struct call_object*) l->data;
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

gboolean tcore_call_object_set_cli_info(struct call_object *co, enum tcore_call_cli_mode mode, char *num)
{
	char *pos = 0;

	_check_null( "co", co, FALSE);

	if (!num) {
		co->cli.mode = mode;
		return TRUE;
	}

	dbg("num  : %s", num);
	dbg("mode : 0x%x", mode);

	pos = num;

	if (!mode) {
		co->cli.mode = _check_cli_mode_by_number(num);

		if (co->cli.mode)
			pos = (num + 4);
	}
	else {
		co->cli.mode = mode;
	}

	strncpy(co->cli.number, pos, (strlen(pos) + 1));
	co->cli.number_len = strlen(co->cli.number);

	dbg("co->cli.mode   : 0x%x", co->cli.mode);
	dbg("co->cli.number : %s", co->cli.number);
	dbg("co->cli.number_len : %d", co->cli.number_len);

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

gboolean tcore_call_object_set_cna_info(struct call_object *co, enum tcore_call_cna_mode mode, char *name, int dcs)
{
	_check_null( "co", co, FALSE);
	_check_null( "name", name, FALSE);

	co->cna.mode = mode;
	strncpy(co->cna.name, name, MAX_CALL_NAME_LEN);
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

	po = calloc(sizeof(struct private_object_data), 1);
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_CALL);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_clone_hook(o, _clone_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_call_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	free(po);
	tcore_object_free(o);
}
