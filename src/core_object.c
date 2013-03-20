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
#include "server.h"
#include "plugin.h"
#include "core_object.h"
#include "hal.h"
#include "at.h"

#include "co_call.h"
#include "co_modem.h"
#include "co_ps.h"
#include "co_network.h"
#include "co_ss.h"
#include "co_sim.h"
#include "co_sat.h"
#include "co_sap.h"
#include "co_sms.h"
#include "co_phonebook.h"
#include "co_gps.h"

#define INIT_CORE_OBJECT(initializer, plugin, type, hal)	do { \
	if (initializer != NULL) { \
		co = tcore_object_clone_template_object(plugin, type); \
		if (co == NULL) { \
			break; \
		} \
		\
		tcore_object_set_hal(co, hal); \
		\
		ret = initializer(plugin, co); \
	} \
} while(0);

#define DEINIT_CORE_OBJECT(deinitializer, plugin, type)		do { \
	if (deinitializer != NULL) { \
		/* Assuming only one Core Object is present of a specific TYPE */ \
		co = tcore_plugin_ref_core_object(plugin, type); \
		if (co == NULL) { \
			break; \
		} \
		\
		deinitializer(plugin, co); \
		\
		tcore_object_free(co); \
	} \
} while(0);


struct callback_type {
	CoreObject *co;
	char *event;
	tcore_object_callback callback;
	void *user_data;
};

struct tcore_object_type {
	unsigned int type;

	TcorePlugin *parent_plugin;

	void *object;
	void *user_data;

	tcore_object_free_hook free_hook;
	tcore_object_clone_hook clone_hook;
	tcore_object_dispatcher dispatcher;
	GSList *callbacks;

	TcoreHal *hal;
};

/* Mapping Table */
struct tcore_object_mapping_tbl {
	TcoreHal *hal;
	GSList *object_type;
};

static void _util_print_mapping_tbl_entry(object_mapping_table_t *tbl_entry)
{
	GSList *co_list;

	msg("------> Table Entry - HAL: [0x%x]", tbl_entry->hal);

	co_list = tbl_entry->object_type;
	if (co_list == NULL) {
		err("No Core Objects defined for this Mapping Table Entry");
		return;
	}

	/* Search all the Table entries with matching 'type' */
	for ( ; co_list ; co_list = co_list->next) {
		if (co_list->data == NULL)
			continue;

		msg("		Core Object type: [0x%x]", co_list->data);
	}
}

static void _free_tbl_entry(gpointer data)
{
	object_mapping_table_t *tbl_entry;

	if (data == NULL)
		return;

	tbl_entry = data;

	dbg("Removing Mapping Table Entry - HAL: [0x%x]", tbl_entry->hal);
	_util_print_mapping_tbl_entry(tbl_entry);

	/* Free Core Object types list */
	g_slist_free(tbl_entry->object_type);

	/* Free Table entry */
	g_free(tbl_entry);
}

static CoreObject *_object_new(TcorePlugin *plugin, unsigned int type)
{
	CoreObject *co;

	co = g_try_new0(struct tcore_object_type, 1);
	if (co == NULL)
		return NULL;

	co->parent_plugin = plugin;

	co->type = type;

	return co;
}

static gboolean _on_at_event(TcoreAT *at, const GSList *lines, void *user_data)
{
	gboolean ret;

	struct callback_type *cb = user_data;

	ret = cb->callback(cb->co, lines, cb->user_data);
	if (ret == FALSE) {

	}

	return ret;
}

static void _remove_at_callback(TcoreAT *at, struct callback_type *cb)
{
	tcore_at_remove_notification_full(at, cb->event, _on_at_event, cb);
}

static object_mapping_table_t *_object_search_mapping_tbl_entry(GSList *mapping_tbl_list,
														TcoreHal *hal)
{
	GSList *list;
	object_mapping_table_t *tbl_entry = NULL;

	for (list = mapping_tbl_list; list ; list = list->next) {
		tbl_entry = list->data;
		if (tbl_entry == NULL)
			continue;

		/* Search for Table entry with matching 'hal' */
		if (tbl_entry->hal == hal) {
			return tbl_entry;
		}
	}

	return NULL;
}

static object_mapping_table_t *_object_search_mapping_tbl_entry_by_type(
								GSList *mapping_tbl_list, unsigned int type)
{
	GSList *list;
	GSList *co_list;
	object_mapping_table_t *tbl_entry = NULL;

	for (list = mapping_tbl_list; list ; list = list->next) {
		tbl_entry = list->data;
		if (tbl_entry == NULL)
			continue;

		/* Search all the Table entries with matching 'type' */
		for (co_list = tbl_entry->object_type ; co_list ; co_list = co_list->next) {
			if (co_list->data == NULL)
				continue;

			if (type == (unsigned int)co_list->data) {
				return tbl_entry;
			}
		}
	}

	return tbl_entry;
}


static gboolean _init_core_object_by_type(unsigned int type,
	TcorePlugin *plugin, TcoreHal *hal, struct object_initializer *initializer_list)
{
	CoreObject *co;
	gboolean ret = FALSE;

	switch (type) {
	case CORE_OBJECT_TYPE_MODEM:
		INIT_CORE_OBJECT(initializer_list->modem_init,
						plugin, type, hal);

		/* Send Notification for MODEM ADDED */
		if (ret == TRUE)
			tcore_server_send_notification(tcore_plugin_ref_server(plugin),
							co, TNOTI_MODEM_ADDED, 0, NULL);
	break;

	case CORE_OBJECT_TYPE_CALL:
		INIT_CORE_OBJECT(initializer_list->call_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_SS:
		INIT_CORE_OBJECT(initializer_list->ss_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_NETWORK:
		INIT_CORE_OBJECT(initializer_list->network_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_PS:
		INIT_CORE_OBJECT(initializer_list->ps_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_SIM:
		INIT_CORE_OBJECT(initializer_list->sim_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_SAT:
		INIT_CORE_OBJECT(initializer_list->sat_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_SAP:
		INIT_CORE_OBJECT(initializer_list->sap_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_SMS:
		INIT_CORE_OBJECT(initializer_list->sms_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_PHONEBOOK:
		INIT_CORE_OBJECT(initializer_list->phonebook_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_GPS:
		INIT_CORE_OBJECT(initializer_list->gps_init,
						plugin, type, hal);
	break;

	case CORE_OBJECT_TYPE_SOUND:		/* Fall through */
	case CORE_OBJECT_TYPE_CUSTOM:		/* Fall through */
	default:
		dbg("Unsupport/Invalid Core Object Type [0x%x]", type);
	}

	return ret;
}

static void _deinit_core_object_by_type(unsigned int type,
			TcorePlugin *plugin, struct object_deinitializer *deinitializer_list)
{
	CoreObject *co;

	switch (type) {
	case CORE_OBJECT_TYPE_MODEM:
		DEINIT_CORE_OBJECT(deinitializer_list->modem_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_CALL:
		DEINIT_CORE_OBJECT(deinitializer_list->call_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_SS:
		DEINIT_CORE_OBJECT(deinitializer_list->ss_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_NETWORK:
		DEINIT_CORE_OBJECT(deinitializer_list->network_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_PS:
		DEINIT_CORE_OBJECT(deinitializer_list->ps_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_SIM:
		DEINIT_CORE_OBJECT(deinitializer_list->sim_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_SAT:
		DEINIT_CORE_OBJECT(deinitializer_list->sat_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_SAP:
		DEINIT_CORE_OBJECT(deinitializer_list->sap_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_SMS:
		DEINIT_CORE_OBJECT(deinitializer_list->sms_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_PHONEBOOK:
		DEINIT_CORE_OBJECT(deinitializer_list->phonebook_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_GPS:
		DEINIT_CORE_OBJECT(deinitializer_list->gps_deinit,
							plugin, type);
	break;

	case CORE_OBJECT_TYPE_SOUND:		/* Fall through */
	case CORE_OBJECT_TYPE_CUSTOM:		/* Fall through */
	default:
		dbg("Unsupport/Invalid Core Object Type [0x%x]", type);
	}
}

CoreObject *tcore_object_new(TcorePlugin *plugin, TcoreHal *hal)
{
	CoreObject *co;

	co = _object_new(plugin, CORE_OBJECT_TYPE_DEFAULT);
	if (co == NULL)
		return NULL;

	tcore_object_set_hal(co, hal);

	if (plugin)
		tcore_plugin_add_core_object(plugin, co);

	return co;
}

void tcore_object_free(CoreObject *co)
{
	GSList *l = NULL;
	struct callback_type *cb = NULL;

	if (co == NULL)
		return;

	dbg("Type: [0x%x]", co->type);

	if (co->free_hook)
		co->free_hook(co);

	if (co->callbacks) {
		for (l = co->callbacks; l; l = l->next) {
			cb = l->data;
			if (cb == NULL)
				continue;

			/* Freeing 'event' and 'cb' node */
			g_free(cb->event);
			g_free(cb);
		}

		g_slist_free(co->callbacks);
		co->callbacks = NULL;
	}

	g_free(co);
}

CoreObject *tcore_object_clone(CoreObject *src, TcorePlugin *new_parent)
{
	CoreObject *dest;
	TcorePlugin *p;
	GSList *l;
	struct callback_type *cb = NULL;

	if (src == NULL)
		return NULL;

	if (new_parent)
		p = new_parent;
	else
		p = src->parent_plugin;

	dest = _object_new(p, src->type);
	if (dest == NULL)
		return NULL;

	dest->object = src->object;
	dest->user_data = src->user_data;
	dest->free_hook = src->free_hook;
	dest->clone_hook = src->clone_hook;
	dest->dispatcher = src->dispatcher;
	dest->hal = src->hal;

	for (l = src->callbacks; l; l = l->next) {
		cb = l->data;
		if (cb == NULL)
			continue;

		tcore_object_add_callback(dest, cb->event, cb->callback, cb->user_data);
	}

	tcore_plugin_add_core_object(p, dest);

	if (src->clone_hook)
		src->clone_hook(src, dest);

	return dest;
}

CoreObject *tcore_object_clone_template_object(TcorePlugin *p,
				unsigned int co_type)
{
	CoreObject *template_co;

	template_co = tcore_server_find_template_object(tcore_plugin_ref_server(p), co_type);
	if(template_co == NULL)
		return NULL;

	return tcore_object_clone(template_co, p);

}

TReturn tcore_object_set_free_hook(CoreObject *co,
		tcore_object_free_hook free_hook)
{
	if (co == NULL)
		return TCORE_RETURN_EINVAL;

	co->free_hook = free_hook;

	return TCORE_RETURN_SUCCESS;
}


TReturn tcore_object_set_clone_hook(CoreObject *co,
		tcore_object_clone_hook clone_hook)
{
	if (co == NULL)
		return TCORE_RETURN_EINVAL;

	co->clone_hook = clone_hook;

	return TCORE_RETURN_SUCCESS;
}

TcorePlugin *tcore_object_ref_plugin(CoreObject *co)
{
	if (co == NULL)
		return NULL;

	return co->parent_plugin;
}

TReturn tcore_object_link_object(CoreObject *co, void *object)
{
	if (co == NULL)
		return TCORE_RETURN_EINVAL;

	co->object = object;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_object_ref_object(CoreObject *co)
{
	if (co == NULL)
		return NULL;

	return co->object;
}

TReturn tcore_object_set_type(CoreObject *co, unsigned int type)
{
	if (co == NULL)
		return TCORE_RETURN_EINVAL;

	co->type = type;

	dbg("Set core_object to type: [0x%x])", type);

	return TCORE_RETURN_SUCCESS;
}

unsigned int tcore_object_get_type(CoreObject *co)
{
	if (co == NULL)
		return 0;

	return co->type;
}

TReturn tcore_object_set_hal(CoreObject *co, TcoreHal *hal)
{
	TcoreAT *at;
	struct callback_type *cb = NULL;
	GSList *l = NULL;
	dbg("Core Object Type: [0x%x] HAL: [0x%x]", co->type, hal);

	if (co == NULL)
		return TCORE_RETURN_EINVAL;

	if (co->hal) {
		/* Remove unsolicited callbacks */
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT) {
			at = tcore_hal_get_at(co->hal);
			for (l = co->callbacks; l != NULL; l = l->next) {
				cb = l->data;
				if (cb == NULL)
					continue;

				tcore_at_remove_notification_full(
					at, cb->event, _on_at_event, cb);
			}
		}
	}

	co->hal = hal;
	if (hal == NULL)
		return TCORE_RETURN_SUCCESS;

	/* Register unsolicited callbacks */
	if (tcore_hal_get_mode(hal) == TCORE_HAL_MODE_AT) {
		at = tcore_hal_get_at(hal);
		for (l = co->callbacks; l != NULL; l = l->next) {
			cb = l->data;
			if (cb == NULL)
				continue;

			if (cb->event[0] == 27)
				tcore_at_add_notification(at, cb->event + 1,
						TRUE, _on_at_event, cb);
			else
				tcore_at_add_notification(at, cb->event,
						FALSE, _on_at_event, cb);
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TcoreHal *tcore_object_get_hal(CoreObject *co)
{
	if (co == NULL)
		return NULL;

	return co->hal;
}

TReturn tcore_object_link_user_data(CoreObject *co,
		void *user_data)
{
	if (co == NULL)
		return TCORE_RETURN_EINVAL;

	co->user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_object_ref_user_data(CoreObject *co)
{
	if (co == NULL)
		return NULL;

	return co->user_data;
}

TReturn tcore_object_dispatch_request(CoreObject *co,
		UserRequest *ur)
{
	if ((co == NULL) || (ur == NULL))
		return TCORE_RETURN_EINVAL;

	if (co->dispatcher == NULL) {
		err("Dispatcher is NULL");
		return TCORE_RETURN_ENOSYS;
	}

	return co->dispatcher(co, ur);
}

TReturn tcore_object_set_dispatcher(CoreObject *co,
		tcore_object_dispatcher func)
{
	if ((co == NULL) || (func == NULL))
		return TCORE_RETURN_EINVAL;

	co->dispatcher = func;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_override_callback(CoreObject *co,
		const char *event, tcore_object_callback callback, void *user_data)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;
	TcoreAT *at = NULL;

	if ((co == NULL) || (event == NULL) || (callback == NULL))
		return TCORE_RETURN_EINVAL;

	if (strlen(event) < 1)
		return TCORE_RETURN_EINVAL;

	if (co->hal) {
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT)
			at = tcore_hal_get_at(co->hal);
	}

	for (l = co->callbacks; l; l = l->next) {
		cb = l->data;
		if (cb == NULL)
			continue;

		if (g_strcmp0(cb->event, event) != 0)
			continue;

		if (at)
			_remove_at_callback(at, cb);

		g_free(cb->event);
		co->callbacks = g_slist_remove(co->callbacks, cb);
		g_free(cb);
	}

	return tcore_object_add_callback(co, event, callback, user_data);
}

TReturn tcore_object_add_callback(CoreObject *co,
		const char *event, tcore_object_callback callback, void *user_data)
{
	struct callback_type *cb = NULL;
	TcoreAT *at = NULL;

	if ((co == NULL) || (event == NULL) || (callback == NULL))
		return TCORE_RETURN_EINVAL;

	if (strlen(event) < 1)
		return TCORE_RETURN_EINVAL;

	cb = g_try_new0(struct callback_type, 1);
	if (cb == NULL)
		return TCORE_RETURN_ENOMEM;

	cb->co = co;
	cb->event = g_strdup(event);
	cb->callback = callback;
	cb->user_data = user_data;

	co->callbacks = g_slist_append(co->callbacks, cb);

	if (co->hal) {
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT) {
			at = tcore_hal_get_at(co->hal);
			if (event[0] == 27)
				tcore_at_add_notification(at, cb->event + 1, TRUE, _on_at_event, cb);
			else
				tcore_at_add_notification(at, cb->event, FALSE, _on_at_event, cb);

		}
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_del_callback(CoreObject *co,
		const char *event, tcore_object_callback callback)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;
	TcoreAT *at = NULL;

	if ((co == NULL) || (event == NULL) || (callback == NULL) || (co->callbacks == NULL))
		return TCORE_RETURN_EINVAL;

	if (strlen(event) < 1)
		return TCORE_RETURN_EINVAL;

	if (co->hal) {
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT)
			at = tcore_hal_get_at(co->hal);
	}

	for (l = co->callbacks; l; l = l->next) {
		cb = l->data;
		if (cb == NULL)
			continue;

		if (cb->callback != callback)
			continue;

		if (g_strcmp0(cb->event, event) != 0)
			continue;

		if (at)
			_remove_at_callback(at, cb);

		g_free(cb->event);
		co->callbacks = g_slist_remove(co->callbacks, cb);
		g_free(cb);
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_emit_callback(CoreObject *co,
		const char *event, const void *event_info)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;
	TReturn ret;

	if ((co == NULL) || (event == NULL))
		return TCORE_RETURN_EINVAL;

	l = co->callbacks;
	while (l) {
		cb = l->data;
		if (cb == NULL) {
			l = l->next;
			continue;
		}

		if (g_strcmp0(cb->event, event) != 0) {
			l = l->next;
			continue;
		}

		if (cb->callback) {
			ret = cb->callback(co, event_info, cb->user_data);
			if (ret == FALSE) {
				l = l->next;
				co->callbacks = g_slist_remove(co->callbacks, cb);
				continue;
			}
		}

		l = l->next;
	}

	return TCORE_RETURN_SUCCESS;
}

void *tcore_object_add_mapping_tbl_entry(void *mapping_tbl,
						unsigned int object_type, TcoreHal *hal)
{
	GSList *mapping_tbl_list = mapping_tbl;
	object_mapping_table_t *tbl_entry;

	if (hal == NULL) {
		err("Mapping Table: [0x%x] HAL: [0x%x]", mapping_tbl, hal);
		return mapping_tbl;
	}

	/*
	 * Search 'hal' across all the Table entries of the Mapping Table
	 *
	 * Table entry MAY NOT be found as -
	 *	1. Mapping Table is NOT yet created
	 *	2. Table entry for corresponding 'hal' is NOT present
	 *
	 * In each of the above cases, the new Mapping Table entry should be added
	 * to the Mapping Table.
	 */
	tbl_entry = _object_search_mapping_tbl_entry(mapping_tbl_list, hal);
	if (tbl_entry == NULL) {
		dbg("Creating New Table entry for HAL: [0x%x]", hal);
		/*
		 * If there is NO previously added Table entry for the 'hal' then
		 * new Table entry is created
		 */

		tbl_entry = g_try_new0(object_mapping_table_t, 1);
		if (tbl_entry != NULL)
			tbl_entry->hal = hal;
		else {
			err("Failed to allocate memory");
			return mapping_tbl_list;
		}

		/* Add the Mapping Table entry to Mapping Table */
		mapping_tbl_list = g_slist_append(mapping_tbl_list, tbl_entry);
	}

	/*
	 * Appending the Core Object type to the list of Core Objects types
	 */
	tbl_entry->object_type = g_slist_append(tbl_entry->object_type, (gpointer)object_type);
	dbg("Added Mapping Table entry - HAL: [0x%x] Object type: [0x%x]", hal, object_type);

	return mapping_tbl_list;
}

void tcore_object_remove_mapping_tbl(void *mapping_tbl)
{
	GSList *mapping_tbl_list = mapping_tbl;

	if (mapping_tbl_list == NULL) {
		err("Mapping Table is NULL");
		return;
	}

	/* Freeing Mapping Table */
	g_slist_free_full(mapping_tbl_list, _free_tbl_entry);
}

void *tcore_object_remove_mapping_tbl_entry(void *mapping_tbl, TcoreHal *hal)
{
	GSList *mapping_tbl_list = mapping_tbl;
	object_mapping_table_t *tbl_entry;

	if (mapping_tbl_list == NULL) {
		err("Mapping Table is NULL");
		return mapping_tbl;
	}

	tbl_entry = _object_search_mapping_tbl_entry(mapping_tbl_list, hal);
	if (tbl_entry == NULL) {
		err("Table entry is NOT available");
		return mapping_tbl_list;
	}

	dbg("Removing Mapping Table Entry - HAL: [0x%x]", hal);
	_util_print_mapping_tbl_entry(tbl_entry);

	/* Free Core Object types list */
	g_slist_free(tbl_entry->object_type);

	/* Remove mapping Table entry */
	mapping_tbl_list = g_slist_remove(mapping_tbl_list, tbl_entry);

	/* Free Table entry */
	g_free(tbl_entry);

	return mapping_tbl_list;
}

void tcore_object_remove_mapping_tbl_entry_by_type(void *mapping_tbl,
							unsigned int co_type)
{
	GSList *mapping_tbl_list;
	object_mapping_table_t *tbl_entry;

	if (mapping_tbl == NULL) {
		err("Mapping Table is NULL");
		return;
	}

	mapping_tbl_list = mapping_tbl;

	tbl_entry =
		_object_search_mapping_tbl_entry_by_type(mapping_tbl_list, co_type);
	if (tbl_entry == NULL) {
		err("Table entry is NOT available");
		return;
	}

	/* Remove the Core Object type from the list */
	tbl_entry->object_type = g_slist_remove(tbl_entry->object_type, (gconstpointer)co_type);
}

void tcore_object_print_mapping_tbl(void *mapping_tbl)
{
	GSList *mapping_tbl_list;
	object_mapping_table_t *tbl_entry = NULL;


	if (mapping_tbl == NULL) {
		err("Mapping Table is NULL");
		return;
	}

	mapping_tbl_list = mapping_tbl;
	if (mapping_tbl_list == NULL) {
		err("No Mapping Table present");
		return;
	}

	for ( ; mapping_tbl_list ; mapping_tbl_list = mapping_tbl_list->next) {
		tbl_entry = mapping_tbl_list->data;
		if (tbl_entry == NULL)
			continue;

		_util_print_mapping_tbl_entry(tbl_entry);
	}
}

/* Initialize Core Objects */
TReturn tcore_object_init_objects(TcorePlugin *plugin,
						struct object_initializer *initializer_list)
{
	GSList *mapping_tbl_list;
	gboolean ret = FALSE;

	/* Refer mapping_tbl from Server's Modem list */
	mapping_tbl_list = tcore_server_get_cp_mapping_tbl(plugin);

	/*
	 * Mapping Table is a MUST
	 * If Mapping Table is NOT NULL, then initialization would be guided by the
	 * Mapping Table entries,
	 * else, it is treated as a Failure.
	 */
	if (mapping_tbl_list != NULL) {
		object_mapping_table_t *tbl_entry;
		GSList *object_type_list;
		unsigned int type;

		/* Initialize each Core Object based on the Mapping Table entries */
		for ( ; mapping_tbl_list ; mapping_tbl_list = mapping_tbl_list->next) {
			tbl_entry = mapping_tbl_list->data;
			if (tbl_entry == NULL)
				continue;

			/* To handle NULL 'init' function case */
			ret = FALSE;

			object_type_list = tbl_entry->object_type;

			for ( ; object_type_list ; object_type_list = object_type_list->next) {
				type = (unsigned int)object_type_list->data;
				dbg("Core Object type: [0x%x]", type);

				ret = _init_core_object_by_type(type, plugin, tbl_entry->hal, initializer_list);
				if (ret == FALSE) {
					err("Failed to initialize Core Object Type [0x%x]", type);

					/*
					 * Will stop initializing other Modules in case of Error,
					 * but we need to check if we need to continue OR not as we cannot
					 * define which Modules are Mandatory and which are NOT.
					 */
					break;
				}
			}
		}
	} else
		err("Mapping Table is NOT present");

	if (ret == FALSE) {
		err("Failed to initialize Core Objects");
		return TCORE_RETURN_FAILURE;
	}

	dbg("Successfully initialized Core Objects");
	return TCORE_RETURN_SUCCESS;
}

/* De-initialize Core Objects */
void tcore_object_deinit_objects(TcorePlugin *plugin,
						struct object_deinitializer *deinitializer_list)
{
	GSList *mapping_tbl_list;

	/* Refer mapping_tbl from Server's Modem list */
	mapping_tbl_list = tcore_server_get_cp_mapping_tbl(plugin);

	/*
	 * Mapping Table is a MUST
	 * If Mapping Table is NOT NULL, then de-initialization would be guided by the
	 * Mapping Table entries,
	 * else,
	 * just return with an Error log.
	 */
	if (mapping_tbl_list != NULL) {
		object_mapping_table_t *tbl_entry;
		GSList *object_type_list;
		unsigned int type;

		/* De-initialize each Core Object based on the Mapping Table entries */
		for ( ; mapping_tbl_list ; mapping_tbl_list = mapping_tbl_list->next) {
			tbl_entry = mapping_tbl_list->data;
			if (tbl_entry == NULL)
				continue;

			object_type_list = tbl_entry->object_type;

			for ( ; object_type_list ; object_type_list = object_type_list->next) {
				type = (unsigned int)object_type_list->data;
				dbg("Core Object type: [0x%x]", type);

				_deinit_core_object_by_type(type, plugin, deinitializer_list);
			}
		}
		dbg("Successfully de-initialized Core Objects");
	} else
		err("Mapping Table is NOT present");
}
