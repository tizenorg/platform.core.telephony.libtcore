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
	gchar *event;
	TcoreObjectCallback callback;
	void *user_data;
};

/* Hooks related structures */
typedef struct {
	GSList *request_hooks_list;	/* List of (CoreObjectRequestCallbacks*) nodes */
	GSList *request_list;			/* List of (CoreObjectRequestList*) nodes */
} CoreObjectRequestHook;

typedef struct {
	TcoreRequestHook func;		/* Request hook callback */
	void *user_data;				/* Request hook callback user_data */
} CoreObjectRequestCallbacks;

typedef struct {
	void *request;				/* Executing 'request' */
	GSList *response_hook_list;	/* List of (CoreObjectResponseHook*) nodes */
} CoreObjectRequestList;

typedef struct {
	TcoreResponseHook func;		/* Response hook callback */
	void *user_data;				/* Response hook callback user_data */
} CoreObjectResponseHook;

typedef struct {
	TcoreNotification command;
	TcoreNotificationHook func;
	void *user_data;
} CoreObjectNotificationHook;

struct tcore_object_type {
	guint type;

	TcorePlugin *parent_plugin;

	void *object;
	void *user_data;

	TcoreObjectFreeHook free_hook;
	TcoreObjectCloneHook clone_hook;
	TcoreObjectDispatcher dispatcher;
	GSList *callbacks;

	/*
	 * Request Hooks Hash table
	 *
	 * key - TcoreCommand
	 * value - CoreObjectRequestHook*
	 */
	GHashTable *request_hooks;

	/* Notification Hooks */
	GSList *notification_hooks;

	TcoreHal *hal;
};

/* Mapping Table */
struct tcore_object_mapping_tbl {
	TcoreHal *hal;
	GSList *object_type;
};

typedef struct {
	TcorePluginResponseCallback resp_cb;
	void *resp_cbdata;

	TcoreCommand command;
	void *request;
} TcoreObjectRespData;

static void _util_print_mapping_tbl_entry(TcoreObjectMappingTable *tbl_entry)
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
		if (co_list->data != NULL) {
			msg("		Core Object type: [0x%x]", co_list->data);
		}
	}
}

static void _free_tbl_entry(gpointer data)
{
	TcoreObjectMappingTable *tbl_entry;

	if (data == NULL) {
		err("data is NULL");
		return;
	}

	tbl_entry = data;

	dbg("Removing Mapping Table Entry - HAL: [0x%x]", tbl_entry->hal);
	_util_print_mapping_tbl_entry(tbl_entry);

	/* Free Core Object types list */
	g_slist_free(tbl_entry->object_type);

	/* Free Table entry */
	tcore_free(tbl_entry);
}

static void __free_callback_data(gpointer data)
{
	struct callback_type *cb = data;

	if (cb) {
		tcore_free(cb->event);
		tcore_free(cb);
	}
}

static void __free_responses(gpointer data)
{
	CoreObjectRequestList *request_node = data;

	/* Free resources */
	g_slist_free_full(request_node->response_hook_list, g_free);
}

static void __free_request_hook(gpointer data)
{
	CoreObjectRequestHook *req_hook = data;

	/* Free Request hook callback nodes */
	g_slist_free_full(req_hook->request_hooks_list, g_free);

	/* Free 'request' and correcponding Response hook nodes */
	g_slist_free_full(req_hook->request_list, __free_responses);
}

static void __free_response_hook(CoreObject *co,
		TcoreCommand command, void *request)
{
	CoreObjectRequestHook *req_hook;

	req_hook = g_hash_table_lookup(co->request_hooks, (gconstpointer)command);
	if (req_hook != NULL) {
		GSList *req_list = req_hook->request_list;
		CoreObjectRequestList *request_node;

		for ( ; req_list ; req_list = req_list->next) {
			request_node = req_list->data;
			if (request_node != NULL)
				if (request_node->request == request)
					g_slist_free_full(request_node->response_hook_list, g_free);
		}
	}

}

static CoreObject *_object_new(TcorePlugin *plugin, guint type)
{
	CoreObject *co;

	co = tcore_malloc0(sizeof(CoreObject));
	if (co == NULL) {
		err("Failed allocate memory");
		return NULL;
	}

	/* Update parameters */
	co->parent_plugin = plugin;
	co->type = type;

	/* Request Hook Hash table */
	co->request_hooks = g_hash_table_new_full(g_direct_hash, g_direct_equal,
					NULL, __free_request_hook);

	return co;
}

static gboolean _on_at_event(TcoreAT *at, const GSList *lines, void *user_data)
{
	struct callback_type *cb = user_data;
	gboolean ret;

	ret = cb->callback(cb->co, lines, cb->user_data);
	if (ret == FALSE) {
		err("Callback failed");
	}

	return ret;
}

static void _remove_at_callback(TcoreAT *at, struct callback_type *cb)
{
	tcore_at_remove_notification_full(at, cb->event, _on_at_event, cb);
}

static TcoreObjectMappingTable *_object_search_mapping_tbl_entry(GSList *mapping_tbl_list,
														TcoreHal *hal)
{
	GSList *list = mapping_tbl_list;
	TcoreObjectMappingTable *tbl_entry = NULL;

	for ( ; list ; list = list->next) {
		tbl_entry = list->data;
		if (tbl_entry != NULL) {
			/* Search for Table entry with matching 'hal' */
			if (tbl_entry->hal == hal)
				return tbl_entry;
		}
	}

	return tbl_entry;
}

static TcoreObjectMappingTable *_object_search_mapping_tbl_entry_by_type(
								GSList *mapping_tbl_list, guint type)
{
	GSList *list = mapping_tbl_list;
	TcoreObjectMappingTable *tbl_entry = NULL;

	for ( ; list ; list = list->next) {
		tbl_entry = list->data;
		if (tbl_entry != NULL) {
			GSList *co_list = tbl_entry->object_type;

			/* Search all the Table entries with matching 'type' */
			for ( ; co_list ; co_list = co_list->next) {
				if (co_list->data != NULL)
					if (type == (guint)co_list->data)
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
		co = tcore_modem_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_CALL:
		/* Create Core Object */
		co = tcore_call_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SS:
		/* Create Core Object */
		co = tcore_ss_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_NETWORK:
		/* Create Core Object */
		co = tcore_network_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_PS:
		/* Create Core Object */
		co = tcore_ps_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SIM:
		/* Create Core Object */
		co = tcore_sim_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SAT:
		/* Create Core Object */
		co = tcore_sat_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SAP:
		/* Create Core Object */
		co = tcore_sap_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_SMS:
		/* Create Core Object */
		co = tcore_sms_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_PHONEBOOK:
		/* Create Core Object */
		co = tcore_phonebook_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_GPS:
		/* Create Core Object */
		co = tcore_gps_new(plugin, NULL, hal);
	break;

	case CORE_OBJECT_TYPE_CUSTOM:		/* Fall through */
	default:
		err("Unsupport/Invalid Core Object Type [0x%x]", type);
	}

	return co;
}

static gboolean _init_core_object_by_type(guint type,
	TcorePlugin *plugin, TcoreObjectInitializer *initializer_list)
{
	CoreObject *co = tcore_plugin_ref_core_object(plugin, type);
	gboolean ret = FALSE;

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

static void _deinit_core_object_by_type(guint type,
	TcorePlugin *plugin, TcoreObjectDeinitializer *deinitializer_list)
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
#if 0
		if (deinitializer_list->sat_deinit) {
			/* Invoke deinitializer */
			deinitializer_list->sat_deinit(plugin, co);
		}
#endif
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

static TcoreHookReturn __object_exec_request_hooks(CoreObject *co,
	TcoreCommand command, const void *request,
	TcoreObjectResponseCallback cb, const void *cb_data)
{
	CoreObjectRequestHook *req_hook;

	/* Look-up Request Hook Hash table if Request hooks are added for the 'command' */
	req_hook = g_hash_table_lookup(co->request_hooks, (gconstpointer)command);
	if (req_hook != NULL) {
		CoreObjectRequestCallbacks *req_hook_node;
		GSList *req_hook_list = req_hook->request_hooks_list;
		CoreObjectRequestList *req_list;

		/* Request list to hold Response hooks */
		req_list = tcore_try_malloc0(sizeof(CoreObjectRequestList));
		if (req_list == NULL) {
			err("Failed to allocate memory");
			return TCORE_HOOK_RETURN_CONTINUE;
		}
		req_list->request = (void *)request;

		/* Prepend the 'request' to Request list for Response hook addition */
		req_hook->request_list = g_slist_prepend(req_hook->request_list, req_list);

		for ( ; req_hook_list ; req_hook_list = req_hook_list->next) {
			req_hook_node = req_hook_list->data;
			if ((req_hook_node != NULL) && req_hook_node->func) {
				/* Invoke Request hook */
				if (req_hook_node->func(co, command, request,
						req_hook_node->user_data,
						cb, cb_data)
						== TCORE_HOOK_RETURN_STOP_PROPAGATION) {
					dbg("Request hook requests Stop propogation");
					return TCORE_HOOK_RETURN_STOP_PROPAGATION;
				}
			}
		}
	}

	return TCORE_HOOK_RETURN_CONTINUE;
}

static TcoreHookReturn __object_exec_response_hooks(CoreObject *co,
	TcoreCommand command, const void *request, gint result, const void *response)
{
	CoreObjectRequestHook *req_hook;

	/* Look-up Request Hook Hash table if Request hooks are added for the 'command' */
	req_hook = g_hash_table_lookup(co->request_hooks, (gconstpointer)command);
	if (req_hook != NULL) {
		GSList *req_list = req_hook->request_list;
		CoreObjectRequestList *req_list_node;

		for ( ; req_list ; req_list = req_list->next) {
			req_list_node = req_list->data;
			if (req_list_node != NULL) {
				if (req_list_node->request == request) {
					GSList *resp_list = req_list_node->response_hook_list;
					CoreObjectResponseHook *resp_hook_node;

					for ( ; resp_list ; resp_list = resp_list->next) {
						resp_hook_node = resp_list->data;
						if (resp_hook_node != NULL && resp_hook_node->func) {
							/* Invoke Response hook */
							if (resp_hook_node->func(co, result, command,
										response, resp_hook_node->user_data)
										== TCORE_HOOK_RETURN_STOP_PROPAGATION) {
								dbg("Response hook requests Stop propogation");
								return TCORE_HOOK_RETURN_STOP_PROPAGATION;
							}
						}
					}
				}
			}
		}
	}

	return TCORE_HOOK_RETURN_CONTINUE;
}

CoreObject *tcore_object_new(TcorePlugin *plugin, TcoreHal *hal)
{
	CoreObject *co;

	co = _object_new(plugin, CORE_OBJECT_TYPE_DEFAULT);
	if (co == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	/* Set HAL */
	tcore_object_set_hal(co, hal);

	/*
	 * Add Core Obejct to Plug-in is Core Object list
	 * if Plug-in is available
	 */
	if (plugin)
		tcore_plugin_add_core_object(plugin, co);

	return co;
}

void tcore_object_free(CoreObject *co)
{
	if (co == NULL) {
		err("Core object is NULL");
		return;
	}
	dbg("Type: [0x%x]", co->type);

	if (co->free_hook)
		co->free_hook(co);

	/* Free resources */
	g_slist_free_full(co->callbacks, __free_callback_data);
	g_hash_table_destroy(co->request_hooks);
	tcore_free(co);
}

CoreObject *tcore_object_clone(CoreObject *src, TcorePlugin *new_parent)
{
	CoreObject *dest;
	TcorePlugin *p;
	GSList *list;
	struct callback_type *cb = NULL;

	if (src == NULL) {
		err("Source Core object is NULL");
		return NULL;
	}

	if (new_parent)
		p = new_parent;
	else
		p = src->parent_plugin;

	dest = _object_new(p, src->type);
	if (dest == NULL) {
		err("Destination Core object is NULL");
		return NULL;
	}

	/* Clone the parameters */
	dest->object = src->object;
	dest->user_data = src->user_data;
	dest->free_hook = src->free_hook;
	dest->clone_hook = src->clone_hook;
	dest->dispatcher = src->dispatcher;
	dest->hal = src->hal;

	for (list = src->callbacks; list; list = list->next) {
		cb = list->data;
		if (cb != NULL)
			tcore_object_add_callback(dest, cb->event, cb->callback, cb->user_data);
	}

	/* Add Cloned Core Object to Plug-in */
	tcore_plugin_add_core_object(p, dest);

	/* Invoke Clone hook */
	if (src->clone_hook)
		src->clone_hook(src, dest);

	return dest;
}

CoreObject *tcore_object_clone_template_object(TcorePlugin *p,
				guint co_type)
{
	CoreObject *template_co;

	/* Find Template Core object */
	template_co = tcore_server_find_template_object(tcore_plugin_ref_server(p), co_type);
	if (template_co == NULL) {
		err("Failed to find Template Core object");
		return NULL;
	}

	return tcore_object_clone(template_co, p);
}

TelReturn tcore_object_set_free_hook(CoreObject *co,
		TcoreObjectFreeHook free_hook)
{
	if (co == NULL) {
		err("Core object is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	co->free_hook = free_hook;

	return TEL_RETURN_SUCCESS;
}


TelReturn tcore_object_set_clone_hook(CoreObject *co,
		TcoreObjectCloneHook clone_hook)
{
	if (co == NULL) {
		err("Core object is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	co->clone_hook = clone_hook;

	return TEL_RETURN_SUCCESS;
}

TcorePlugin *tcore_object_ref_plugin(CoreObject *co)
{
	if (co == NULL) {
		err("Core object is NULL");
		return NULL;
	}

	return co->parent_plugin;
}

TelReturn tcore_object_link_object(CoreObject *co, void *object)
{
	if (co == NULL) {
		err("Core object is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	co->object = object;

	return TEL_RETURN_SUCCESS;
}

void *tcore_object_ref_object(CoreObject *co)
{
	if (co == NULL) {
		err("Core object is NULL");
		return NULL;
	}

	return co->object;
}

TelReturn tcore_object_set_type(CoreObject *co, guint type)
{
	if (co == NULL) {
		err("Core object is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	co->type = type;
	dbg("Set core_object to type: [0x%x])", type);

	return TEL_RETURN_SUCCESS;
}

guint tcore_object_get_type(CoreObject *co)
{
	if (co == NULL) {
		err("Core object is NULL");
		return 0;
	}

	return co->type;
}

TelReturn tcore_object_set_hal(CoreObject *co, TcoreHal *hal)
{
	TcoreAT *at;
	struct callback_type *cb = NULL;
	GSList *list;

	if (co == NULL) {
		err("Core object is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	dbg("Core Object Type: [0x%x] HAL: [0x%x]", co->type, hal);

	if (co->hal) {
		/* Remove unsolicited callbacks */
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT) {
			at = tcore_hal_get_at(co->hal);
			for (list = co->callbacks; list != NULL; list = list->next) {
				cb = list->data;
				if (cb != NULL) {
					tcore_at_remove_notification_full(at,
							cb->event, _on_at_event, cb);
				}
			}
		}
	}

	co->hal = hal;
	if (hal == NULL) {
		dbg("hal is NULL");
		return TEL_RETURN_SUCCESS;
	}

	/* Register unsolicited callbacks */
	if (tcore_hal_get_mode(hal) == TCORE_HAL_MODE_AT) {
		at = tcore_hal_get_at(hal);
		for (list = co->callbacks; list != NULL; list = list->next) {
			cb = list->data;
			if (cb != NULL) {
				if (cb->event[0] == 27)
					tcore_at_add_notification(at,
							cb->event + 1, TRUE, _on_at_event, cb);
				else
					tcore_at_add_notification(at,
							cb->event, FALSE, _on_at_event, cb);
			}
		}
	}

	return TEL_RETURN_SUCCESS;
}

TcoreHal *tcore_object_get_hal(CoreObject *co)
{
	if (co == NULL) {
		err("Core object is NULL");
		return NULL;
	}

	return co->hal;
}

TelReturn tcore_object_link_user_data(CoreObject *co,
		void *user_data)
{
	if (co == NULL) {
		err("Core object is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	co->user_data = user_data;

	return TEL_RETURN_SUCCESS;
}

void *tcore_object_ref_user_data(CoreObject *co)
{
	if (co == NULL) {
		err("Core object is NULL");
		return NULL;
	}

	return co->user_data;
}

/* TODO - Need to move the static function and rename accordingly */
static void tcore_object_response_callback(CoreObject *co,
	gint result, const void *response, void *user_data)
{
	TcoreObjectRespData *resp_cb_data = (TcoreObjectRespData *)user_data;

	if (resp_cb_data == NULL) {
		err("Response Callback data is NULL");
		return;
	}

	/* Invoke Response hooks */
	if (__object_exec_response_hooks(co, resp_cb_data->command,
				resp_cb_data->request, result, response)
				!= TCORE_HOOK_RETURN_STOP_PROPAGATION) {
		/* Invoke response callback */
		if (resp_cb_data->resp_cb)
			resp_cb_data->resp_cb(tcore_object_ref_plugin(co),
					result, response, resp_cb_data->resp_cbdata);
	}

	/* Remove Request and Response Hooks */
	__free_response_hook(co, resp_cb_data->command, resp_cb_data->request);

	/* Free resource */
	tcore_free(resp_cb_data->request);
	tcore_free(resp_cb_data);
}

TelReturn tcore_object_dispatch_request(CoreObject *co,
		gboolean exec_hooks, TcoreCommand command,
		const void *request, guint request_len,
		TcorePluginResponseCallback cb, const void *user_data)
{
	void *dispatch_request;
	TcoreObjectRespData *resp_cb_data;
	TelReturn ret = TEL_RETURN_FAILURE;

	if (co == NULL) {
		err("Core object is NULL");
		return TEL_RETURN_FAILURE;
	}

	if (request_len)
		dispatch_request = tcore_memdup(request, request_len);
	else
		dispatch_request = tcore_malloc0(sizeof(guint));

	/* Allocate Response callback data */
	resp_cb_data = tcore_malloc0(sizeof(TcoreObjectRespData));
	resp_cb_data->resp_cb = cb;
	resp_cb_data->resp_cbdata = (void *)user_data;

	resp_cb_data->command = command;
	resp_cb_data->request = dispatch_request;

	if (exec_hooks == TRUE) {
		/* Invoke Request hooks */
		if (__object_exec_request_hooks(co, command, dispatch_request,
				tcore_object_response_callback, (const void *)resp_cb_data)
				== TCORE_HOOK_RETURN_STOP_PROPAGATION) {
			dbg("Hooks return - [STOP PROPOGATION]");
			return TEL_RETURN_SUCCESS;
		}
	}

	/*
	 * Dispatch Request to corresponding Core object
	 * if 'dispatcher' is available.
	 */
	if (co->dispatcher) {
		ret = co->dispatcher(co, command, dispatch_request,
			tcore_object_response_callback, (const void *)resp_cb_data);
		dbg("Dispatch Request - [%s]",
			(ret == TEL_RETURN_SUCCESS) ? "SUCCESS" : "FAIL");
	} else {
		err("Dispatcher is NULL");
	}

	/*
	 * Free resources,
	 * in case Request processing is not Successful
	 */
	if (ret != TEL_RETURN_SUCCESS) {
		tcore_free(dispatch_request);
		tcore_free(resp_cb_data);
	}

	return ret;
}

TelReturn tcore_object_set_dispatcher(CoreObject *co,
		TcoreObjectDispatcher func)
{
	if ((co == NULL) || (func == NULL)) {
		err("co: [%p] func: [%p]", co, func);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	co->dispatcher = func;

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_object_override_callback(CoreObject *co,
		const gchar *event, TcoreObjectCallback callback, void *user_data)
{
	struct callback_type *cb;
	GSList *list;
	TcoreAT *at = NULL;

	if ((co == NULL) || (event == NULL) || (callback == NULL)) {
		err("co: [%p] event: [%p] callback: [%p]", co, event, callback);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (strlen(event) < 1) {
		err("Invalid event!!!");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (co->hal)
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT)
			at = tcore_hal_get_at(co->hal);

	for (list = co->callbacks; list ; list = list->next) {
		cb = list->data;
		if (cb != NULL) {
			if (g_strcmp0(cb->event, event) == 0) {
				if (at)
					_remove_at_callback(at, cb);

				/* Free resources */
				co->callbacks = g_slist_remove(co->callbacks, cb);
				__free_callback_data(cb);
			}
		}
	}

	return tcore_object_add_callback(co, event, callback, user_data);
}

TelReturn tcore_object_add_callback(CoreObject *co,
		const gchar *event, TcoreObjectCallback callback, void *user_data)
{
	struct callback_type *cb = NULL;
	TcoreAT *at = NULL;

	if ((co == NULL) || (event == NULL) || (callback == NULL)) {
		err("co: [%p] event: [%p] callback: [%p]", co, event, callback);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (strlen(event) < 1) {
		err("Invalid event!!!");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	cb = tcore_malloc0(sizeof(struct callback_type));
	cb->co = co;
	cb->event = tcore_strdup(event);
	cb->callback = callback;
	cb->user_data = user_data;

	/* Callbacks are appended - FCFS principle */
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

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_object_del_callback(CoreObject *co,
		const gchar *event, TcoreObjectCallback callback)
{
	struct callback_type *cb;
	GSList *list;
	TcoreAT *at = NULL;

	if ((co == NULL) || (event == NULL)
			|| (callback == NULL) || (co->callbacks == NULL)) {
		err("co: [%p] event: [%p] callback: [%p] co->callbacks: [%p]",
				co, event, callback, (co ? co->callbacks : NULL));
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (strlen(event) < 1) {
		err("Invalid event!!!");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	if (co->hal)
		if (tcore_hal_get_mode(co->hal) == TCORE_HAL_MODE_AT)
			at = tcore_hal_get_at(co->hal);

	for (list = co->callbacks; list ; list = list->next) {
		cb = list->data;
		if (cb != NULL) {
			if (cb->callback == callback) {
				if (g_strcmp0(cb->event, event) == 0) {
					if (at)
						_remove_at_callback(at, cb);

					/* Free resources */
					co->callbacks = g_slist_remove(co->callbacks, cb);
					__free_callback_data(cb);
				}
			}
		}
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_object_emit_callback(CoreObject *co,
		const gchar *event, const void *event_info)
{
	struct callback_type *cb;
	GSList *list;

	if ((co == NULL) || (event == NULL)) {
		err("co: [%p] event: [%p]", co, event);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	for (list = co->callbacks ; list ; list = list->next) {
		cb = list->data;
		if (cb != NULL) {
			if (g_strcmp0(cb->event, event) == 0) {
				if (cb->callback) {
					if (cb->callback(co, event_info, cb->user_data) == FALSE) {
						err("Callback failed");

						/* Free resource */
						co->callbacks = g_slist_remove(co->callbacks, cb);
						__free_callback_data(cb);
					}
				}
			}
		}
	}

	return TEL_RETURN_SUCCESS;
}

TelReturn tcore_object_add_request_hook(CoreObject *co,
	TcoreCommand command, TcoreRequestHook func, void *user_data)
{
	CoreObjectRequestHook *req_hook;
	CoreObjectRequestCallbacks *req_hook_node;

	if ((co == NULL) || (func == NULL)) {
		err("co: [%p] func: [%p]", co, func);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/* Create Request hook list node */
	req_hook_node = tcore_malloc0(sizeof(CoreObjectRequestCallbacks));
	req_hook_node->func = func;
	req_hook_node->user_data = user_data;

	req_hook = g_hash_table_lookup(co->request_hooks, (gconstpointer)command);
	if (req_hook == NULL) {
		/* Create Request Hook (Hash table) node */
		req_hook = tcore_malloc0(sizeof(CoreObjectRequestHook));
		if (req_hook == NULL) {
			err("Failed to allocate memory");

			/* Free resource */
			tcore_free(req_hook_node);
			return TEL_RETURN_MEMORY_FAILURE;
		}

		/* Insert Request Hook node to Hash table */
		g_hash_table_insert(co->request_hooks, (gpointer)command, req_hook);
	}

	/* Append request hook node to Request hooks list */
	req_hook->request_hooks_list = g_slist_append(req_hook->request_hooks_list, req_hook_node);
	dbg("Request hook added for Command: [0x%x]", command);

	return TEL_RETURN_SUCCESS;
}

void tcore_object_remove_request_hook(CoreObject *co,
	TcoreCommand command, TcoreRequestHook func)
{
	CoreObjectRequestHook *req_hook;
	CoreObjectRequestCallbacks *req_hook_node;
	GSList *req_hook_list;

	if ((co == NULL) || (func == NULL)) {
		err("co: [%p] func: [%p]", co, func);
		return;
	}

	req_hook = g_hash_table_lookup(co->request_hooks, (gconstpointer)command);
	if (req_hook == NULL) {
		err("No request hooks for Command: [0x%x]", command);
		return;
	}

	req_hook_list = req_hook->request_hooks_list;
	while (req_hook_list) {
		req_hook_node = req_hook_list->data;
		if (req_hook_node != NULL) {
			if (req_hook_node->func == func) {
				GSList *tmp_list = req_hook_list->next;

				/* Remove node */
				req_hook->request_hooks_list = g_slist_remove(req_hook->request_hooks_list, req_hook_node);
				dbg("Request hook removed for Command: [0x%x]", command);

				/* Free resource */
				tcore_free(req_hook_node);

				/* Continue to next node */
				req_hook_list = tmp_list;
				continue;
			}
		}

		/* Continue to next node */
		req_hook_list = req_hook_list->next;
	}
}

TelReturn tcore_object_add_response_hook(CoreObject *co,
	TcoreCommand command, const void *request, TcoreResponseHook func, void *user_data)
{
	CoreObjectRequestHook *req_hook;

	if ((co == NULL) || (func == NULL)) {
		err("co: [%p] func: [%p]", co, func);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/* Look-up Request Hook Hash table if Request hooks are added for the 'command' */
	req_hook = g_hash_table_lookup(co->request_hooks, (gconstpointer)command);
	if (req_hook != NULL) {
		GSList *req_list = req_hook->request_list;
		CoreObjectRequestList *req_list_node;

		for ( ; req_list ; req_list = req_list->next) {
			req_list_node = req_list->data;
			if (req_list_node != NULL) {
				if (req_list_node->request == request) {
					CoreObjectResponseHook *resp_hook_node;

					/* Response hook node */
					resp_hook_node = tcore_try_malloc0(sizeof(CoreObjectResponseHook));
					if (resp_hook_node == NULL) {
						err("Failed to allocate memory");
						return TEL_RETURN_MEMORY_FAILURE;
					}
					resp_hook_node->func = func;
					resp_hook_node->user_data = user_data;

					/* Append Response hook node to Response hook list */
					req_list_node->response_hook_list =
							g_slist_append(req_list_node->response_hook_list, resp_hook_node);
					dbg("Response hook added for Command: [0x%x]", command);

					return TEL_RETURN_SUCCESS;
				}
			}
		}
	}
	else {
		err("Abnormal behavior... Response Hooks need to be added in Request hook callbacks!!!");
	}

	return TEL_RETURN_FAILURE;
}

void tcore_object_remove_response_hook(CoreObject *co,
	TcoreCommand command, TcoreResponseHook func)
{
	CoreObjectRequestHook *req_hook;

	if ((co == NULL) || (func == NULL)) {
		err("co: [%p] func: [%p]", co, func);
		return;
	}

	req_hook = g_hash_table_lookup(co->request_hooks, (gconstpointer)command);
	if (req_hook != NULL) {
		GSList *req_list = req_hook->request_list;
		CoreObjectRequestList *request_node;

		for ( ; req_list ; req_list = req_list->next) {
			request_node = req_list->data;
			if (request_node != NULL) {
				CoreObjectResponseHook *response_node;
				GSList *resp_list = request_node->response_hook_list;

				while (resp_list) {
					GSList *tmp_list = resp_list->next;

					response_node = resp_list->data;
					if (response_node != NULL) {
						if (response_node->func == func) {
							/* Remove and free response hook node */
							request_node->response_hook_list =
								g_slist_remove(request_node->response_hook_list, response_node);
							dbg("Response hook removed for Command: [0x%x]", command);

							tcore_free(response_node);
						}
					}

					resp_list = tmp_list;
				}
			}
		}
	}
}

TelReturn tcore_object_add_notification_hook(CoreObject *co,
	TcoreNotification command, TcoreNotificationHook func, void *user_data)
{
	CoreObjectNotificationHook *noti_hook;

	if ((co == NULL) || (func == NULL)) {
		err("co: [%p] func: [%p]", co, func);
		return TEL_RETURN_INVALID_PARAMETER;
	}

	/* Notification hook node */
	noti_hook = tcore_try_malloc0(sizeof(CoreObjectNotificationHook));
	if (noti_hook == NULL) {
		err("Failed to allocate memory");
		return TEL_RETURN_MEMORY_FAILURE;
	}
	noti_hook->command = command;
	noti_hook->func = func;
	noti_hook->user_data = user_data;

	/* Append Notification hook node to Notification hook list */
	co->notification_hooks =
			g_slist_append(co->notification_hooks, noti_hook);

	dbg("Notification hook added for Command: [0x%x]", command);

	return TEL_RETURN_SUCCESS;
}

void tcore_object_remove_notification_hook(CoreObject *co,
	TcoreNotification command, TcoreNotificationHook func)
{
	GSList *noti_list;
	CoreObjectNotificationHook *noti_hook;

	if ((co == NULL) || (func == NULL)) {
		err("co: [%p] func: [%p]", co, func);
		return;
	}

	noti_list = co->notification_hooks;
	while (noti_list) {
		GSList *tmp_list = noti_list->next;

		noti_hook = noti_list->data;
		if (noti_hook != NULL) {
			if (noti_hook->func == func) {
				/* Remove and free Notification hook node */
				co->notification_hooks =
					g_slist_remove(co->notification_hooks, noti_hook);
				dbg("Notification hook removed for Command: [0x%x]", command);

				tcore_free(noti_hook);
			}
		}

		noti_list = tmp_list;
	}
}

TelReturn tcore_object_send_notification(CoreObject *co,
	TcoreNotification command, guint data_len, void *data)
{
	TcorePlugin *plugin;
	GSList *noti_list;
	CoreObjectNotificationHook *noti_hook;

	if (co == NULL) {
		err("Core object is NULL");
		return TEL_RETURN_INVALID_PARAMETER;
	}

	plugin = tcore_object_ref_plugin(co);

	/* Invoke Notification Hooks */
	for (noti_list = co->notification_hooks; noti_list; noti_list = noti_list->next) {
		noti_hook = noti_list->data;
		if (noti_hook != NULL) {
			dbg("noti_hook->command[0x%x], Command: [0x%x]",noti_hook->command, command);
			if (noti_hook->command == command) {
				dbg("Invoking hook_func() for Command: [0x%x]", command);
				if (noti_hook->func(plugin, command, data_len, data, noti_hook->user_data)
						== TCORE_HOOK_RETURN_STOP_PROPAGATION) {
					dbg("Notification hook requests Stop propogation");
					return TEL_RETURN_SUCCESS;
				}
			}
		}
	}

	/* Dispatch the Notification to Server */
	return tcore_plugin_send_notification(plugin, command, data_len, data);
}

void *tcore_object_add_mapping_tbl_entry(void *mapping_tbl,
						guint object_type, TcoreHal *hal)
{
	GSList *mapping_tbl_list = mapping_tbl;
	TcoreObjectMappingTable *tbl_entry;

	if (hal == NULL) {
		err("Mapping Table: [0x%x] HAL: [0x%x]", mapping_tbl, hal);
		return NULL;
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

		tbl_entry = tcore_malloc0(sizeof(TcoreObjectMappingTable));
		tbl_entry->hal = hal;

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
	TcoreObjectMappingTable *tbl_entry;

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
	tcore_free(tbl_entry);

	return mapping_tbl_list;
}

void tcore_object_remove_mapping_tbl_entry_by_type(void *mapping_tbl,
							guint co_type)
{
	GSList *mapping_tbl_list;
	TcoreObjectMappingTable *tbl_entry;

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
	TcoreObjectMappingTable *tbl_entry = NULL;

	if (mapping_tbl == NULL) {
		err("Mapping Table is NULL");
		return;
	}

	mapping_tbl_list = mapping_tbl;

	for ( ; mapping_tbl_list ; mapping_tbl_list = mapping_tbl_list->next) {
		tbl_entry = mapping_tbl_list->data;
		if (tbl_entry != NULL) {
			_util_print_mapping_tbl_entry(tbl_entry);
		}
	}
}

/* Initialize Core Objects */
TelReturn tcore_object_init_objects(TcorePlugin *plugin,
	TcoreObjectInitializer *initializer_list)
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
		TcoreObjectMappingTable *tbl_entry;
		GSList *mtbl_list;
		GSList *object_type_list;
		guint type;

		/* Create each Core Object based on the Mapping Table entries */
		mtbl_list = mapping_tbl_list;
		for ( ; mtbl_list ; mtbl_list = mtbl_list->next) {
			tbl_entry = mtbl_list->data;
			if (tbl_entry != NULL) {
				CoreObject *co;

				object_type_list = tbl_entry->object_type;
				for ( ; object_type_list ; object_type_list = object_type_list->next) {
					type = (guint)object_type_list->data;

					co = _create_core_object_by_type(type, plugin, tbl_entry->hal);
					if (co == NULL) {
						err("Failed to create Core Object - Type: [0x%x]", type);
						return TEL_RETURN_FAILURE;
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
					type = (guint)object_type_list->data;

					ret = _init_core_object_by_type(type, plugin, initializer_list);
					if (ret == FALSE) {
						err("Failed to initialize Core Object - Type: [0x%x]", type);
						return TEL_RETURN_FAILURE;
					}
					dbg("Initialized Core Object - Type: [0x%x]", type);
				}
			}
		}
	}
	else {
		err("Mapping Table is NOT present");
	}

	if (ret == FALSE) {
		err("Failed to create/initialize Core Objects");
		return TEL_RETURN_FAILURE;
	}

	dbg("Successfully initialized Core Objects");
	return TEL_RETURN_SUCCESS;
}

/* De-initialize Core Objects */
void tcore_object_deinit_objects(TcorePlugin *plugin,
	TcoreObjectDeinitializer *deinitializer_list)
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
		TcoreObjectMappingTable *tbl_entry;
		GSList *object_type_list;
		guint type;

		/* De-initialize each Core Object based on the Mapping Table entries */
		for ( ; mapping_tbl_list ; mapping_tbl_list = mapping_tbl_list->next) {
			tbl_entry = mapping_tbl_list->data;
			if (tbl_entry != NULL) {
				object_type_list = tbl_entry->object_type;
				for ( ; object_type_list ; object_type_list = object_type_list->next) {
					type = (guint)object_type_list->data;
					dbg("Core Object type: [0x%x]", type);

					_deinit_core_object_by_type(type, plugin, deinitializer_list);
				}
			}
		}
		dbg("Successfully de-initialized Core Objects");
	}
	else {
		err("Mapping Table is NOT present");
	}
}
