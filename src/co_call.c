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
#include "server.h"
#include "queue.h"
#include "user_request.h"
#include "co_call.h"

/* Max 6 Call Objects are supported */
#define MAX_CALL_OBJECTS			6

#define _check_null(name, value, err) { \
	if (NULL == value) { \
		dbg("[error] %s : NULL", name); \
		return err; \
	} \
}

typedef gboolean(*func)(struct call_object *co, void *data);

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
	GSList *cobjs;
	struct tcore_call_operations *ops;
	struct tcore_call_control_operations *cops;

	/* To be removed later */
	struct tcore_call_information_operations *iops;
};

static void _clone_hook(CoreObject *src, CoreObject *dest)
{
	struct private_object_data *src_po;
	struct private_object_data *dest_po;

	if ((NULL == src) || (NULL == dest))
		return;

	dest_po = g_try_new0(struct private_object_data, 1);
	if (NULL == dest_po) {
		tcore_object_link_object(dest, NULL);
		return;
	}

	src_po = tcore_object_ref_object(src);
	if (NULL == src_po) {
		tcore_object_link_object(dest, NULL);
		g_free(dest_po);
		return;
	}

	dest_po->ops = src_po->ops;
	dest_po->cops = src_po->cops;

	tcore_object_link_object(dest, dest_po);
}

static void _clone_call_operations(struct private_object_data *po,
								struct tcore_call_operations *call_ops)
{
	if (call_ops->dial)
		po->ops->dial = call_ops->dial;

	if (call_ops->answer)
		po->ops->answer = call_ops->answer;

	if (call_ops->end)
		po->ops->end = call_ops->end;

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

	if (call_ops->deflect)
		po->ops->deflect = call_ops->deflect;

	if (call_ops->transfer)
		po->ops->transfer = call_ops->transfer;

	if (call_ops->send_dtmf)
		po->ops->send_dtmf = call_ops->send_dtmf;

	if (call_ops->set_sound_path)
		po->ops->set_sound_path = call_ops->set_sound_path;

	if (call_ops->set_sound_volume_level)
		po->ops->set_sound_volume_level =
							call_ops->set_sound_volume_level;

	if (call_ops->get_sound_volume_level)
		po->ops->get_sound_volume_level =
							call_ops->get_sound_volume_level;

	if (call_ops->mute)
		po->ops->mute = call_ops->mute;

	if (call_ops->unmute)
		po->ops->unmute = call_ops->unmute;

	if (call_ops->get_mute_status)
		po->ops->get_mute_status = call_ops->get_mute_status;

	if (call_ops->set_sound_recording)
		po->ops->set_sound_recording = call_ops->set_sound_recording;

	if (call_ops->set_sound_equalization)
		po->ops->set_sound_equalization =
							call_ops->set_sound_equalization;

	if (call_ops->set_sound_noise_reduction)
		po->ops->set_sound_noise_reduction =
							call_ops->set_sound_noise_reduction;

	if (call_ops->set_active_line)
		po->ops->set_active_line = call_ops->set_active_line;

	if (call_ops->get_active_line)
		po->ops->get_active_line = call_ops->get_active_line;

	if (call_ops->activate_ccbs)
		po->ops->activate_ccbs = call_ops->activate_ccbs;
}

static void _clone_call_control_operations(struct private_object_data *po,
							struct tcore_call_control_operations *control_ops)
{
	if (control_ops->answer_hold_and_accept)
		po->cops->answer_hold_and_accept = control_ops->answer_hold_and_accept;

	if (control_ops->answer_replace)
		po->cops->answer_replace = control_ops->answer_replace;

	if (control_ops->answer_reject)
		po->cops->answer_reject = control_ops->answer_reject;

	if (control_ops->end_specific)
		po->cops->end_specific = control_ops->end_specific;

	if (control_ops->end_all_active)
		po->cops->end_all_active = control_ops->end_all_active;

	if (control_ops->end_all_held)
		po->cops->end_all_held = control_ops->end_all_held;

	if (control_ops->active)
		po->cops->active = control_ops->active;

	if (control_ops->hold)
		po->cops->hold = control_ops->hold;

	if (control_ops->swap)
		po->cops->swap = control_ops->swap;

	if (control_ops->join)
		po->cops->join = control_ops->join;

	if (control_ops->split)
		po->cops->split = control_ops->split;

	if (control_ops->transfer)
		po->cops->transfer = control_ops->transfer;

	if (control_ops->deflect)
		po->cops->deflect = control_ops->deflect;

	return;
}

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;

	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null("po", po, TCORE_RETURN_FAILURE);
	_check_null("po->ops", po->ops, TCORE_RETURN_FAILURE);
	_check_null("ur", ur, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_CALL_DIAL:
		_check_null("po->ops->dial", po->ops->dial,
						TCORE_RETURN_FAILURE);
		return po->ops->dial(o, ur);

	case TREQ_CALL_ANSWER:
		_check_null("po->ops->answer", po->ops->answer,
						TCORE_RETURN_FAILURE);
		return po->ops->answer(o, ur);

	case TREQ_CALL_END:
		_check_null("po->ops->end", po->ops->end,
						TCORE_RETURN_FAILURE);
		return po->ops->end(o, ur);

	case TREQ_CALL_HOLD:
		_check_null("po->ops->hold", po->ops->hold,
						TCORE_RETURN_FAILURE);
		return po->ops->hold(o, ur);

	case TREQ_CALL_ACTIVE:
		_check_null("po->ops->active", po->ops->active,
						TCORE_RETURN_FAILURE);
		return po->ops->active(o, ur);

	case TREQ_CALL_SWAP:
		_check_null("po->ops->swap", po->ops->swap,
						TCORE_RETURN_FAILURE);
		return po->ops->swap(o, ur);

	case TREQ_CALL_JOIN:
		_check_null("po->ops->join", po->ops->join,
						TCORE_RETURN_FAILURE);
		return po->ops->join(o, ur);

	case TREQ_CALL_SPLIT:
		_check_null("po->ops->split", po->ops->split,
						TCORE_RETURN_FAILURE);
		return po->ops->split(o, ur);

	case TREQ_CALL_DEFLECT:
		_check_null("po->ops->deflect", po->ops->deflect,
						TCORE_RETURN_FAILURE);
		return po->ops->deflect(o, ur);

	case TREQ_CALL_TRANSFER:
		_check_null("po->ops->transfer", po->ops->transfer,
						TCORE_RETURN_FAILURE);
		return po->ops->transfer(o, ur);

	case TREQ_CALL_SEND_DTMF:
		_check_null("po->ops->send_dtmf", po->ops->send_dtmf,
						TCORE_RETURN_FAILURE);
		return po->ops->send_dtmf(o, ur);

	case TREQ_CALL_SET_SOUND_PATH:
		_check_null("po->ops->set_sound_path", po->ops->set_sound_path,
						TCORE_RETURN_FAILURE);
		return po->ops->set_sound_path(o, ur);

	case TREQ_CALL_GET_SOUND_VOLUME_LEVEL:
		_check_null("po->ops->get_sound_volume_level",
						po->ops->get_sound_volume_level,
						TCORE_RETURN_FAILURE);
		return po->ops->get_sound_volume_level(o, ur);

	case TREQ_CALL_SET_SOUND_VOLUME_LEVEL:
		_check_null("po->ops->set_sound_volume_level",
						po->ops->set_sound_volume_level,
						TCORE_RETURN_FAILURE);
		return po->ops->set_sound_volume_level(o, ur);

	case TREQ_CALL_MUTE:
		_check_null("po->ops->mute", po->ops->mute,
						TCORE_RETURN_FAILURE);
		return po->ops->mute(o, ur);

	case TREQ_CALL_UNMUTE:
		_check_null("po->ops->unmute", po->ops->unmute,
						TCORE_RETURN_FAILURE);
		return po->ops->unmute(o, ur);

	case TREQ_CALL_GET_MUTE_STATUS:
		_check_null("po->ops->get_mute_status",
						po->ops->get_mute_status,
						TCORE_RETURN_FAILURE);
		return po->ops->get_mute_status(o, ur);

	case TREQ_CALL_SET_SOUND_RECORDING:
		_check_null("po->ops->set_sound_recording",
						po->ops->set_sound_recording,
						TCORE_RETURN_FAILURE);
		return po->ops->set_sound_recording(o, ur);

	case TREQ_CALL_SET_SOUND_EQUALIZATION:
		_check_null("po->ops->set_sound_equalization",
						po->ops->set_sound_equalization,
						TCORE_RETURN_FAILURE);
		return po->ops->set_sound_equalization(o, ur);

	case TREQ_CALL_SET_SOUND_NOISE_REDUCTION:
		_check_null("po->ops->set_sound_noise_reduction",
						po->ops->set_sound_noise_reduction,
						TCORE_RETURN_FAILURE);
		return po->ops->set_sound_noise_reduction(o, ur);

	default:
		break;
	}

	return TCORE_RETURN_SUCCESS;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po;

	po = tcore_object_ref_object(o);
	if (NULL == po)
		return;

	g_slist_free_full(po->cobjs, g_free);
	g_free(po);

	tcore_object_link_object(o, NULL);
}

static struct call_object *_find_object(GSList *objs, void  *data, func compare)
{
	struct call_object *co = 0;
	GSList *l = 0;

	_check_null("objs", objs, 0);
	_check_null("compare", compare, 0);

	l = objs;
	while (l) {
		co = (struct call_object *) l->data;

		if (compare(co, data))
			return co;

		l = g_slist_next(l);
	}

	return NULL;
}

static GSList *_find_object_all(GSList *objs, void  *data, func compare)
{
	struct call_object *co;
	GSList *l;
	GSList *ret = NULL;

	_check_null("objs", objs, 0);
	_check_null("compare", compare, 0);

	l = objs;
	while (l) {
		co = (struct call_object*) l->data;

		if (compare(co, data))
			ret = g_slist_append(ret, co);

		l = g_slist_next(l);
	}

	return ret;
}

static gboolean _compare_by_id(struct call_object *co, void *data)
{
	unsigned int *id = (unsigned int*) data;

	_check_null("co", co, FALSE);
	_check_null("data", data, FALSE);

	if (co->id == *id)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_status(struct call_object *co, void *data)
{
	enum tcore_call_status *ct = (enum tcore_call_status*) data;

	_check_null("co", co, FALSE);
	_check_null("data", data, FALSE);

	if (co->status == *ct)
		return TRUE;

	return FALSE;
}

static gboolean _compare_by_number(struct call_object *co, void *data)
{
	char *number = (char*) data;

	_check_null("co", co, FALSE);
	_check_null("data", data, FALSE);

	if (0 == strcmp(co->cli.number, number))
		return TRUE;

	return FALSE;
}

static enum tcore_call_cli_mode _check_cli_mode_by_number(char *num)
{
	_check_null("num", num, TCORE_CALL_CLI_MODE_DEFAULT);

	if (0 == strncmp(num, "*31#", 4))
		return TCORE_CALL_CLI_MODE_PRESENT;

	if (0 == strncmp(num, "#31#", 4))
		return TCORE_CALL_CLI_MODE_RESTRICT;

	return TCORE_CALL_CLI_MODE_DEFAULT;
}

static void _call_information_mo_by_object(CoreObject *o,
							enum tcore_notification_command command)
{
	TcorePlugin *plugin;
	CallObject *co;
	int id;
	dbg("Entry");

	/* Referring Parent plugin from Core Object */
	plugin = tcore_object_ref_plugin(o);
	if (NULL == plugin) {
		err("Failed to find Plug-in!");
		return;
	}

	/* Extracting the Call Core object from current MO Call objects */
	co = tcore_call_object_current_on_mo_processing(o);
	if (NULL == co) {
		err("Failed to find Call Core object!");
		return;
	}

	/* Extracting the Call ID from Call Object */
	id = tcore_call_object_get_id(co);

	/* Send notification to TAPI */
	tcore_server_send_notification(tcore_plugin_ref_server(plugin),
									o,
									command,
									sizeof(unsigned int),
									(void *)&id);

	dbg("Exit");
}

static void _call_information_mo_by_number(CoreObject *o,
					char *number, enum tcore_notification_command command)
{
	TcorePlugin *plugin;
	CallObject *co;
	int id;
	dbg("Entry");

	/* Referring Parent plugin from Core Object */
	plugin = tcore_object_ref_plugin(o);
	if (NULL == plugin) {
		err("Failed to find Plug-in!");
		return;
	}

	/* Extracting the Call Core object by 'number' */
	co = tcore_call_object_find_by_number(o, number);
	if (NULL == co) {
		err("Failed to find Call Core object!");
		return;
	}

	/* Extracting the Call ID from Call Object */
	id = tcore_call_object_get_id(co);

	/* Send notification to TAPI */
	tcore_server_send_notification(tcore_plugin_ref_server(plugin),
									o,
									command,
									sizeof(unsigned int),
									(void *)&id);

	dbg("Exit");
}

static void _call_info_mt_cli(CoreObject *o, enum tcore_call_cli_mode mode,
							char *number)
{
	CallObject *co;
	dbg("Entry");

	/* Extracting the Call Core object from current MT Call objects */
	co = tcore_call_object_current_on_mt_processing(o);
	if (NULL == co) {
		err("Failed to find Call Core object!");
		return;
	}

	/* Set CLI information */
	tcore_call_object_set_cli_info(co, mode, number);

	dbg("Exit");
}

static void _call_info_mt_cna(CoreObject *o, enum tcore_call_cna_mode mode,
							char *name, int dcs)
{
	CallObject *co;
	dbg("Entry");

	/* Extracting the Call Core object from current MT Call objects */
	co = tcore_call_object_current_on_mt_processing(o);
	if (NULL == co) {
		err("Failed to find Call Core object!");
		return;
	}

	/* Set CNA information */
	tcore_call_object_set_cna_info(co, mode, name, dcs);

	dbg("Exit");
}

/* Call Object API */
struct call_object *tcore_call_object_new(CoreObject *o, int id)
{
	struct private_object_data *po;
	struct call_object *co;

	po = tcore_object_ref_object(o);
	_check_null("po", po, NULL);

	co = g_try_new0(struct call_object, 1);
	if (NULL == co) {
		err("Failed to allocate memory for Call Core object!");
		return NULL;
	}

	if (id > 0) {
		if (NULL == _find_object(po->cobjs, (void *) &id, (void *) _compare_by_id))
			co->id = id;
		else {
			dbg("[ error ] call object exist already. [ %d ]", id);
			g_free(co);
			return NULL;
		}
	} else {
		int i = 0;

		for (i = 1; i <= MAX_CALL_OBJECTS; i++) {	/* Presently 6 is MAX call count */
			if (NULL == _find_object(po->cobjs, (void *) &i,
								(void *) _compare_by_id)) {
				co->id = i;
				break;
			}
		}

		/* Free the allocated Core Object if ID is not allocated */
		if(i > MAX_CALL_OBJECTS) {
			err("[ error ] failed to assign call id");
			g_free(co);
			return NULL;
		}
	}

	/* Appending the new Call object to Call Objects list */
	po->cobjs = g_slist_append(po->cobjs, co);
	dbg("new call object id : [%d]", co->id);

	return co;
}

gboolean tcore_call_object_free(CoreObject *o, struct call_object *co)
{
	struct private_object_data *po;

	po = tcore_object_ref_object(o);
	_check_null("po", po, FALSE);
	_check_null("po->cobjs", po->cobjs, FALSE);

	_check_null("co", co, FALSE);

	po->cobjs = g_slist_remove(po->cobjs, co);

	dbg("Freeing call core object");
	g_free(co);

	return TRUE;
}

struct call_object *tcore_call_object_current_on_mt_processing(CoreObject *o)
{
	struct private_object_data *po;
	GSList *l;

	enum tcore_call_status cs = CALL_STATUS_INCOMING;

	po = tcore_object_ref_object(o);
	_check_null("po", po, 0);
	_check_null("po->cobjs", po->cobjs, 0);

	l = _find_object_all(po->cobjs, (void *) &cs, (void *) _compare_by_status);
	if (NULL == l) {
		cs = CALL_STATUS_WAITING;
		l = _find_object_all(po->cobjs, (void *) &cs, (void *)
							_compare_by_status);
		if (NULL == l)
			return 0;
	}

	return (struct call_object*) l->data;
}

struct call_object *tcore_call_object_current_on_mo_processing(CoreObject *o)
{
	struct private_object_data *po;
	GSList *l;

	enum tcore_call_status cs = CALL_STATUS_DIALING;

	po = tcore_object_ref_object(o);
	_check_null("po", po, 0);
	_check_null("po->cobjs", po->cobjs, 0);

	l = _find_object_all(po->cobjs, (void *) &cs, (void *) _compare_by_status);
	if (NULL == l) {
		cs = CALL_STATUS_ALERT;
		l = _find_object_all(po->cobjs, (void *) &cs, (void *) _compare_by_status);
		if (NULL == l)
			return 0;
	}

	return (struct call_object*) l->data;
}

struct call_object *tcore_call_object_find_by_id(CoreObject *o, int id)
{
	struct private_object_data *po;

	po = tcore_object_ref_object(o);
	_check_null("po", po, 0);
	_check_null("po->cobjs", po->cobjs, 0);

	return _find_object(po->cobjs, (void *) &id, (void *) _compare_by_id);
}

struct call_object *tcore_call_object_find_by_number(CoreObject *o, char *num)
{
	struct private_object_data *po;

	po = tcore_object_ref_object(o);
	_check_null("po", po, 0);
	_check_null("po->cobjs", po->cobjs, 0);

	_check_null("num", num, 0);

	return _find_object(po->cobjs, (void *) num, (void *) _compare_by_number);
}

GSList *tcore_call_object_find_by_status(CoreObject *o,
										enum tcore_call_status cs)
{
	struct private_object_data *po;

	po = tcore_object_ref_object(o);
	_check_null("po", po, 0);
	_check_null("po->cobjs", po->cobjs, 0);

	return _find_object_all(po->cobjs, (void *) &cs,
							(void *) _compare_by_status);
}

int tcore_call_object_get_id(struct call_object *co)
{
	_check_null("co", co, -1);

	return co->id;
}

gboolean tcore_call_object_set_type(struct call_object *co,
											enum tcore_call_type ct)
{
	_check_null("co", co, FALSE);

	co->type = ct;
	return TRUE;
}

enum tcore_call_type tcore_call_object_get_type(struct call_object *co)
{
	_check_null("co", co, -1);

	return co->type;
}

gboolean tcore_call_object_set_direction(struct call_object *co,
											enum tcore_call_direction cd)
{
	_check_null("co", co, FALSE);

	co->direction = cd;
	return TRUE;
}

enum tcore_call_direction tcore_call_object_get_direction(struct call_object *co)
{
	_check_null("co", co, -1);

	return co->direction;
}

gboolean tcore_call_object_set_status(struct call_object *co,
											enum tcore_call_status cs)
{
	_check_null("co", co, FALSE);

	co->status = cs;
	return TRUE;
}

enum tcore_call_status tcore_call_object_get_status(struct call_object *co)
{
	_check_null("co", co, -1);

	return co->status;
}

gboolean tcore_call_object_set_cli_info(struct call_object *co,
									enum tcore_call_cli_mode mode, char *num)
{
	char *pos;

	_check_null("co", co, FALSE);

	if (NULL == num) {
		co->cli.mode = mode;
		return TRUE;
	}

	dbg("num  : %s", num);
	dbg("mode : 0x%x", mode);

	pos = num;

	if (TCORE_CALL_CLI_MODE_DEFAULT == mode) {
		co->cli.mode = _check_cli_mode_by_number(num);

		if (co->cli.mode)
			pos = (num + 4);
	} else {
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
	_check_null("co", co, -1);
	_check_null("num", num, -1);

	strncpy(num, co->cli.number, MAX_CALL_NUMBER_LEN);
	return co->cli.number_len;
}

enum tcore_call_cli_mode tcore_call_object_get_cli_mode(struct call_object *co)
{
	_check_null("co", co, -1);
	return co->cli.mode;
}

gboolean tcore_call_object_set_cna_info(struct call_object *co,
		enum tcore_call_cna_mode mode, char *name, int dcs)
{
	int len;

	_check_null("co", co, FALSE);
	_check_null("name", name, FALSE);

	len = strlen(name);
	if (len >= MAX_CALL_NAME_LEN) {
		dbg("Call name is too long");
		return FALSE;
	}

	strncpy(co->cna.name, name, len);
	co->cna.name[len] = '\0';

	co->cna.mode = mode;

	return TRUE;
}

int tcore_call_object_get_name(struct call_object *co, char *name)
{
	_check_null("co", co, -1);
	_check_null("name", name, -1);

	strncpy(name, co->cna.name, MAX_CALL_NAME_LEN);
	return co->cna.name_len;
}

enum tcore_call_cna_mode tcore_call_object_get_cna_mode(struct call_object *co)
{
	_check_null("co", co, -1);

	return co->cna.mode;
}

gboolean tcore_call_object_set_multiparty_state(struct call_object *co, gboolean is)
{
	_check_null("co", co, FALSE);

	co->mpty = is;
	return TRUE;
}

gboolean tcore_call_object_get_multiparty_state(struct call_object *co)
{
	_check_null("co", co, FALSE);

	return co->mpty;
}

gboolean tcore_call_object_set_active_line(struct call_object *co, unsigned int line)
{
	_check_null("co", co, FALSE);

	co->active_line = line;
	return TRUE;
}

int tcore_call_object_get_active_line(struct call_object *co)
{
	_check_null("co", co, -1);

	return co->active_line;
}

TReturn tcore_call_control_answer_hold_and_accept(CoreObject *o,
							UserRequest *ur, ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->answer_hold_and_accept(o, ur, cb, user_data);
}

TReturn tcore_call_control_answer_replace(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->answer_replace(o, ur, cb, user_data);
}

TReturn tcore_call_control_answer_reject(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->answer_reject(o, ur, cb, user_data);
}

TReturn tcore_call_control_end_specific(CoreObject *o, UserRequest *ur,
										const int id, ConfirmCallback cb,
		void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->end_specific(o, ur, id, cb, user_data);
}

TReturn tcore_call_control_end_all_active(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->end_all_active(o, ur, cb, user_data);
}

TReturn tcore_call_control_end_all_held(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->end_all_held(o, ur, cb, user_data);
}

TReturn tcore_call_control_active(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->active(o, ur, cb, user_data);
}

TReturn tcore_call_control_hold(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->hold(o, ur, cb, user_data);
}

TReturn tcore_call_control_swap(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->swap(o, ur, cb, user_data);
}

TReturn tcore_call_control_join(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->join(o, ur, cb, user_data);
}

TReturn tcore_call_control_split(CoreObject *o, UserRequest *ur, const int id,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->split(o, ur, id, cb, user_data);
}

TReturn tcore_call_control_transfer(CoreObject *o, UserRequest *ur,
										ConfirmCallback cb, void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->transfer(o, ur, cb, user_data);
}

TReturn tcore_call_control_deflect(CoreObject *o, UserRequest *ur,
									const char *number, ConfirmCallback cb,
									void  *user_data)
{
	struct private_object_data *po;
	po = tcore_object_ref_object(o);

	_check_null( "po", po, TCORE_RETURN_FAILURE);
	_check_null( "po->cops", po->cops, TCORE_RETURN_FAILURE);

	return po->cops->deflect(o, ur, number, cb, user_data);
}

void tcore_call_control_set_operations(CoreObject *o,
				struct tcore_call_control_operations *ops)
{
	struct private_object_data *po;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);
	if (NULL == po)
		return;

	po->cops = ops;
}

void tcore_call_information_mo_col(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);
}

void tcore_call_information_mo_waiting(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_object(o, TNOTI_CALL_INFO_WAITING);
}

void tcore_call_information_mo_cug(CoreObject *o, int cug_index)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);
}

void tcore_call_information_mo_forwarded(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_object(o, TNOTI_CALL_INFO_FORWARDED);
}

void tcore_call_information_mo_barred_incoming(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_object(o, TNOTI_CALL_INFO_BARRED_INCOMING);
}

void tcore_call_information_mo_barred_outgoing(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_object(o, TNOTI_CALL_INFO_BARRED_OUTGOING);
}

void tcore_call_information_mo_deflected(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_object(o, TNOTI_CALL_INFO_DEFLECTED);
}

void tcore_call_information_mo_clir_suppression_reject(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_object(o, TNOTI_CALL_INFO_CLIR_SUPPRESSION_REJECT);
}

void tcore_call_information_mo_cfu(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_object(o, TNOTI_CALL_INFO_FORWARD_UNCONDITIONAL);
}

void tcore_call_information_mo_cfc(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_object(o, TNOTI_CALL_INFO_FORWARD_CONDITIONAL);
}

void tcore_call_information_mt_cli(CoreObject *o,
								enum tcore_call_cli_mode mode, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_info_mt_cli(o, mode, number);
}

void tcore_call_information_mt_cna(CoreObject *o,
							enum tcore_call_cna_mode mode, char *name, int dcs)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	_call_info_mt_cna(o, mode, name, dcs);
}

void tcore_call_information_mt_forwarded_call(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_FORWARDED_CALL);
}

void tcore_call_information_mt_cug_call(CoreObject *o, int cug_index, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);
}

void tcore_call_information_mt_deflected_call(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_DEFLECTED_CALL);
}

void tcore_call_information_mt_transfered(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_TRANSFERED_CALL);
}

void tcore_call_information_held(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_HELD);
}

void tcore_call_information_active(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_ACTIVE);
}

void tcore_call_information_joined(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_JOINED);
}

void tcore_call_information_released_on_hold(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_RELEASED_ON_HOLD);
}

void tcore_call_information_transfer_alert(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_TRANSFERED_CALL);
}

void tcore_call_information_transfered(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_TRANSFER_ALERT);
}

void tcore_call_information_cf_check_ss_message(CoreObject *o, char *number)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	return _call_information_mo_by_number(o, number, TNOTI_CALL_INFO_CF_CHECK_MESSAGE);
}

/* To be removed later */
void tcore_call_information_set_operations(CoreObject *o,
								struct tcore_call_information_operations *ops)
{
	struct private_object_data *po;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);
	if (NULL == po)
		return;

	po->iops = ops;
}

void tcore_call_override_ops(CoreObject *o,
							struct tcore_call_operations *call_ops,
							struct tcore_call_control_operations *control_ops)
{
	struct private_object_data *po;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (NULL == po)
		return;

	if (call_ops)
		_clone_call_operations(po, call_ops);

	if (control_ops)
		_clone_call_control_operations(po, control_ops);
}

CoreObject *tcore_call_new(TcorePlugin *p,
		struct tcore_call_operations *ops, TcoreHal *hal)
{
	CoreObject *o;
	struct private_object_data *po;

	if (NULL == p)
		return NULL;

	o = tcore_object_new(p, hal);
	if (!o)
		return NULL;

	po = g_try_new0(struct private_object_data, 1);
	if (NULL == po) {
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
	struct private_object_data *po;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_CALL);

	po = tcore_object_ref_object(o);
	if (NULL == po)
		return;

	free(po);
	tcore_object_free(o);
}
