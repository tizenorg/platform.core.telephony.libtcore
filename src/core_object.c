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

struct callback_type {
	CoreObject *co;
	char *event;
	CoreObjectCallback callback;
	void *user_data;
};

struct tcore_object_type {
	unsigned int type;
	char *name;

	TcorePlugin *parent_plugin;

	void *object;
	void *user_data;

	CoreObjectFreeHook free_hook;
	CoreObjectCloneHook clone_hook;
	CoreObjectDispatcher dispatcher;
	GSList *callbacks;
	GHashTable *property;

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

	co = calloc(1, sizeof(struct tcore_object_type));
	if (!co)
		return NULL;

	co->parent_plugin = plugin;

	co->type = type;
	co->property = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

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

			if (type == (unsigned long)co_list->data) {
				return tbl_entry;
			}
		}
	}

	return tbl_entry;
}

static CoreObject *_create_core_object_by_type(guint type,
	TcorePlugin *plugin, TcoreHal *hal)
{
	CoreObject *co = NULL;

	switch (type) {
	case CORE_OBJECT_TYPE_MODEM:
		/* Create Core Object */
		co = tcore_modem_new(plugin, "modem", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_CALL:
		/* Create Core Object */
		co = tcore_call_new(plugin, "call", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SS:
		/* Create Core Object */
		co = tcore_ss_new(plugin, "ss", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_NETWORK:
		/* Create Core Object */
		co = tcore_network_new(plugin, "network", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_PS:
		/* Create Core Object */
		co = tcore_ps_new(plugin, "ps", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SIM:
		/* Create Core Object */
		co = tcore_sim_new(plugin, "sim", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SAT:
		/* Create Core Object */
		co = tcore_sat_new(plugin, "sat", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SAP:
		/* Create Core Object */
		co = tcore_sap_new(plugin, "sap", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SMS:
		/* Create Core Object */
		co = tcore_sms_new(plugin, "sms", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_PHONEBOOK:
		/* Create Core Object */
		co = tcore_phonebook_new(plugin, "phonebook", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_GPS:
		/* Create Core Object */
		co = tcore_gps_new(plugin, "gps", NULL, hal);
	break;

	case CORE_OBJECT_TYPE_CUSTOM:		/* Fall through */
	default:
		err("Unsupport/Invalid Core Object Type [0x%x]", type);
	}

	return co;
}


static gboolean _init_core_object_by_type(unsigned int type,
	TcorePlugin *plugin, struct object_initializer *initializer_list)
{
	gboolean ret = FALSE;
	CoreObject *co = tcore_plugin_ref_core_object(plugin, type);
	if (co == NULL) {
		err("No Core Object of type: [0x%x]", type);
		return FALSE;
	}

	switch (type) {
	case CORE_OBJECT_TYPE_MODEM: {
		/* Invoke initializer */
		if (initializer_list->modem_init)
			ret = initializer_list->modem_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_CALL: {
		/* Invoke initializer */
		if (initializer_list->call_init)
			ret = initializer_list->call_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_SS: {
		/* Invoke initializer */
		if (initializer_list->ss_init)
			ret = initializer_list->ss_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_NETWORK: {
		/* Invoke initializer */
		if (initializer_list->network_init)
			ret = initializer_list->network_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_PS: {
		/* Invoke initializer */
		if (initializer_list->ps_init)
			ret = initializer_list->ps_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_SIM: {
		/* Invoke initializer */
		if (initializer_list->sim_init)
			ret = initializer_list->sim_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_SAT: {
		/* Invoke initializer */
		if (initializer_list->sat_init)
			ret = initializer_list->sat_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_SAP: {
		/* Invoke initializer */
		if (initializer_list->sap_init)
			ret = initializer_list->sap_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_SMS:{
		/* Invoke initializer */
		if (initializer_list->sms_init)
			ret = initializer_list->sms_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_PHONEBOOK: {
		/* Invoke initializer */
		if (initializer_list->phonebook_init)
			ret = initializer_list->phonebook_init(plugin, co);
	} break;

	case CORE_OBJECT_TYPE_GPS:{
		/* Invoke initializer */
		if (initializer_list->gps_init)
			ret = initializer_list->gps_init(plugin, co);
	} break;

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

	co = tcore_plugin_ref_core_object(plugin, type);
	if (co == NULL) {
		err("No Core Object of type: [0x%x]", type);
		return;
	}

	switch (type) {
	case CORE_OBJECT_TYPE_MODEM: {
		if (deinitializer_list->modem_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->modem_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_CALL: {
		if (deinitializer_list->call_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->call_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_SS: {
		if (deinitializer_list->ss_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->ss_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_NETWORK: {
		if (deinitializer_list->network_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->network_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_PS: {
		if (deinitializer_list->ps_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->ps_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_SIM: {
		if (deinitializer_list->sim_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->sim_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_SAT: {
		if (deinitializer_list->sat_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->sat_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_SAP: {
		if (deinitializer_list->sap_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->sap_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_SMS:{
		if (deinitializer_list->sms_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->sms_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_PHONEBOOK: {
		if (deinitializer_list->phonebook_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->phonebook_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_GPS:{
		if (deinitializer_list->gps_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->gps_deinit(plugin, co);
		}
	} break;

	case CORE_OBJECT_TYPE_CUSTOM:		/* Fall through */
	default:
		dbg("Unsupport/Invalid Core Object Type [0x%x]", type);
		return;
	}

	/* Free Core Object */
	tcore_object_free(co);
}


CoreObject *tcore_object_new(TcorePlugin *plugin,
		const char *name, TcoreHal *hal)
{
	CoreObject *co;

	co = _object_new(plugin, CORE_OBJECT_TYPE_DEFAULT);
	if (!co)
		return NULL;

	tcore_object_set_hal(co, hal);
	tcore_object_set_name(co, name);

	if (plugin)
		tcore_plugin_add_core_object(plugin, co);

	return co;
}

void tcore_object_free(CoreObject *co)
{
	GSList *l = NULL;
	struct callback_type *cb = NULL;

	if (!co)
		return;

	dbg("co_name=%s", co->name);

	if (co->free_hook)
		co->free_hook(co);
	else {
		if (co->object)
			warn("free_hook is null, private-object couldn't be freed!!");
	}

	if (co->property) {
		g_hash_table_destroy(co->property);
	}

	if (co->callbacks) {
		for (l = co->callbacks; l; l = l->next) {
			cb = l->data;
			if (!cb)
				continue;

			if (cb->event)
				free(cb->event);

			free(cb);
		}

		g_slist_free(co->callbacks);
		co->callbacks = NULL;
	}

	if (co->parent_plugin)
		tcore_plugin_remove_core_object(co->parent_plugin, co);

	if (co->name)
		free(co->name);

	memset(co, 0, sizeof(CoreObject));
	free(co);
}

CoreObject *tcore_object_clone(CoreObject *src, TcorePlugin *new_parent)
{
	CoreObject *dest;
	TcorePlugin *p;

	GSList *l = NULL;
	struct callback_type *cb = NULL;

	if (!src)
		return NULL;

	if (new_parent)
		p = new_parent;
	else
		p = src->parent_plugin;

	dest = _object_new(p, src->type);
	if (!dest)
		return NULL;

	dest->object = src->object;
	dest->user_data = src->user_data;
	dest->free_hook = src->free_hook;
	dest->clone_hook = src->clone_hook;
	dest->dispatcher = src->dispatcher;
	dest->hal = src->hal;

	for (l = src->callbacks; l; l = l->next) {
		cb = l->data;
		if (!cb)
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

const char *tcore_object_ref_name(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->name;
}

TReturn tcore_object_set_free_hook(CoreObject *co,
		CoreObjectFreeHook free_hook)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->free_hook = free_hook;

	return TCORE_RETURN_SUCCESS;
}


TReturn tcore_object_set_clone_hook(CoreObject *co,
		CoreObjectCloneHook clone_hook)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->clone_hook = clone_hook;

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_set_name(CoreObject *co, const char *name)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	if (co->name) {
		free(co->name);
		co->name = NULL;
	}

	if (name)
		co->name = strdup(name);

	return TCORE_RETURN_SUCCESS;
}

TcorePlugin *tcore_object_ref_plugin(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->parent_plugin;
}

TReturn tcore_object_link_object(CoreObject *co, void *object)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->object = object;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_object_ref_object(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->object;
}

TReturn tcore_object_set_type(CoreObject *co, unsigned int type)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->type = type;

	return TCORE_RETURN_SUCCESS;
}

unsigned int tcore_object_get_type(CoreObject *co)
{
	if (!co)
		return 0;

	return co->type;
}

TReturn tcore_object_set_hal(CoreObject *co, TcoreHal *hal)
{
	TcoreAT *at;
	struct callback_type *cb = NULL;
	GSList *l = NULL;

	if (!co)
		return TCORE_RETURN_EINVAL;

	if (co->hal) {
		// remove unsolicited callbacks
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT) {
			at = tcore_hal_get_at(co->hal);
			for (l = co->callbacks; l; l = l->next) {
				cb = l->data;
				if (!cb)
					continue;

				tcore_at_remove_notification_full(at, cb->event, _on_at_event, cb);
			}
		}
	}

	co->hal = hal;
	if (!hal)
		return TCORE_RETURN_SUCCESS;

	// register unsolicited callbacks
	if (tcore_hal_get_mode(hal) == TCORE_HAL_MODE_AT) {
		at = tcore_hal_get_at(hal);
		for (l = co->callbacks; l; l = l->next) {
			cb = l->data;
			if (!cb)
				continue;

			if (cb->event[0] == 27)
				tcore_at_add_notification(at, cb->event + 1, TRUE, _on_at_event, cb);
			else
				tcore_at_add_notification(at, cb->event, FALSE, _on_at_event, cb);
		}
	}

	return TCORE_RETURN_SUCCESS;
}

TcoreHal *tcore_object_get_hal(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->hal;
}

TReturn tcore_object_link_user_data(CoreObject *co,
		void *user_data)
{
	if (!co)
		return TCORE_RETURN_EINVAL;

	co->user_data = user_data;

	return TCORE_RETURN_SUCCESS;
}

void *tcore_object_ref_user_data(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->user_data;
}

TReturn tcore_object_dispatch_request(CoreObject *co,
		UserRequest *ur)
{
	if (!co || !ur)
		return TCORE_RETURN_EINVAL;

	if (!co->dispatcher)
		return TCORE_RETURN_ENOSYS;

	return co->dispatcher(co, ur);
}

TReturn tcore_object_set_dispatcher(CoreObject *co,
		CoreObjectDispatcher func)
{
	if (!co || !func)
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
		const char *event,
		CoreObjectCallback callback, void *user_data)
{
	struct callback_type *cb = NULL;
	TcoreAT *at = NULL;

	if (!co || !event || !callback)
		return TCORE_RETURN_EINVAL;

	if (strlen(event) < 1)
		return TCORE_RETURN_EINVAL;

	cb = calloc(1, sizeof(struct callback_type));
	if (!cb)
		return TCORE_RETURN_ENOMEM;

	cb->co = co;
	cb->event = strdup(event);
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
		const char *event, CoreObjectCallback callback)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;
	TcoreAT *at = NULL;

	if (!co || !event || !callback || !co->callbacks)
		return TCORE_RETURN_EINVAL;

	if (strlen(event) == 0)
		return TCORE_RETURN_EINVAL;

	if (co->hal) {
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT)
			at = tcore_hal_get_at(co->hal);
	}

	l = co->callbacks;
	while (l) {
		cb = l->data;
		if (!cb) {
			l = l->next;
			continue;
		}

		if (cb->callback != callback) {
			l = l->next;
			continue;
		}

		if (g_strcmp0(cb->event, event) != 0) {
			l = l->next;
			continue;
		}

		if (at)
			_remove_at_callback(at, cb);

		l = l->next;
		co->callbacks = g_slist_remove(co->callbacks, cb);
		free(cb->event);
		free(cb);
	}

	return TCORE_RETURN_SUCCESS;
}

TReturn tcore_object_emit_callback(CoreObject *co,
		const char *event, const void *event_info)
{
	struct callback_type *cb = NULL;
	GSList *l = NULL;
	TReturn ret;

	if (!co || !event)
		return TCORE_RETURN_EINVAL;

	l = co->callbacks;
	while (l) {
		cb = l->data;
		if (!cb) {
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

static GSList *_set_property_real (CoreObject *co, const char *key,
		const char *value, GSList *list)
{
	char *prev;

	if (!co || !key)
		return list;

	if (!value) {
		g_hash_table_remove (co->property, key);
		return g_slist_append (list, (char *)key);
	}

	prev = g_hash_table_lookup(co->property, key);
	if (prev != NULL) {
		/*
		 * If same data, no change & no callback emit
		 */
		if (strcmp (prev, value) == 0)
			return list;

		g_hash_table_replace(co->property, strdup(key), strdup(value));
	}
	else {
		g_hash_table_insert(co->property, strdup(key), strdup(value));
	}

	return g_slist_append (list, (char *)key);
}

TReturn tcore_object_set_property_full(CoreObject *co, const char *first_property, ...)
{
	va_list argptr;
	GSList *list = NULL;
	const char *k;
	const char *v;

	if (!co || !first_property)
		return TCORE_RETURN_EINVAL;

	va_start (argptr, first_property);

	k = first_property;
	v = va_arg (argptr, char *);
	list = _set_property_real (co, k, v, list);

	while (1) {
		k = va_arg (argptr, char *);
		if (!k)
			break;

		v = va_arg (argptr, char *);
		list = _set_property_real (co, k, v, list);
	}

	va_end (argptr);

	if (!list)
		return TCORE_RETURN_SUCCESS;

	if (g_slist_length(list) > 0)
		tcore_object_emit_callback (co,
				CORE_OBJECT_EVENT_PROPERTY_CHANGED, list);

	g_slist_free (list);

	return TCORE_RETURN_SUCCESS;
}

const char *tcore_object_ref_property(CoreObject *co, const char *key)
{
	if (!co || !key)
		return NULL;

	return g_hash_table_lookup(co->property, key);
}

GHashTable *tcore_object_ref_property_hash(CoreObject *co)
{
	if (!co)
		return NULL;

	return co->property;
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
	tbl_entry->object_type = g_slist_append(tbl_entry->object_type, (gpointer)(unsigned long)object_type);
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
	tbl_entry->object_type = g_slist_remove(tbl_entry->object_type, (gconstpointer)(unsigned long)co_type);
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
		GSList *mtbl_list;
		GSList *object_type_list;
		unsigned int type;

		/* Create each Core Object based on the Mapping Table entries */
		mtbl_list = mapping_tbl_list;
		for ( ; mtbl_list ; mtbl_list = mtbl_list->next) {
			tbl_entry = mtbl_list->data;
			if (tbl_entry != NULL) {
				CoreObject *co;

				object_type_list = tbl_entry->object_type;
				for ( ; object_type_list ; object_type_list = object_type_list->next) {
					type = (unsigned long)object_type_list->data;

					co = _create_core_object_by_type(type, plugin, tbl_entry->hal);
					if (co == NULL) {
						err("Failed to create Core Object - Type: [0x%x]", type);
						return TCORE_RETURN_FAILURE;
					}
					dbg("Created Core Object - Type: [0x%x]", type);
				}
			}
		}


		/* Initialize each Core Object based on the Mapping Table entries */
		mtbl_list = mapping_tbl_list;
		for ( ; mtbl_list ; mtbl_list = mtbl_list->next) {
			tbl_entry = mtbl_list->data;
			if (tbl_entry != NULL) {
				/* To handle NULL 'init' function case */
				ret = FALSE;

				object_type_list = tbl_entry->object_type;

				for ( ; object_type_list ; object_type_list = object_type_list->next) {
					type = (unsigned long)object_type_list->data;
					dbg("Core Object type: [0x%x]", type);

					ret = _init_core_object_by_type(type, plugin, initializer_list);
					if (ret == FALSE) {
						err("Failed to initialize Core Object Type [0x%x]", type);
						return TCORE_RETURN_FAILURE;
					}
					dbg("Initialized Core Object - Type: [0x%x]", type);
				}
			}
		}
	} else {
		err("Mapping Table is NOT present");
	}
	if (ret == FALSE) {
		err("Failed to create/initialize Core Objects");
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
		unsigned long type;

		/* De-initialize each Core Object based on the Mapping Table entries */
		for ( ; mapping_tbl_list ; mapping_tbl_list = mapping_tbl_list->next) {
			tbl_entry = mapping_tbl_list->data;
			if (tbl_entry == NULL)
				continue;

			object_type_list = tbl_entry->object_type;

			for ( ; object_type_list ; object_type_list = object_type_list->next) {
				type = (unsigned long)object_type_list->data;
				dbg("Core Object type: [0x%x]", type);

				_deinit_core_object_by_type(type, plugin, deinitializer_list);
			}
		}
		dbg("Successfully de-initialized Core Objects");
	} else
		err("Mapping Table is NOT present");
}
