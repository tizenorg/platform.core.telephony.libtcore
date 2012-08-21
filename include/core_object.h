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

#ifndef __TCORE_CORE_OBJECT_H__
#define __TCORE_CORE_OBJECT_H__

__BEGIN_DECLS

#define CORE_OBJECT_TYPE_DEFAULT       0xB0000000
#define CORE_OBJECT_TYPE_MODEM         (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_MODEM)
#define CORE_OBJECT_TYPE_CALL          (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_CALL)
#define CORE_OBJECT_TYPE_SS            (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SS)
#define CORE_OBJECT_TYPE_NETWORK       (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_NETWORK)
#define CORE_OBJECT_TYPE_PS            (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_PS)
#define CORE_OBJECT_TYPE_PS_CONTEXT    (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_PDP)
#define CORE_OBJECT_TYPE_SIM           (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SIM)
#define CORE_OBJECT_TYPE_SAT           (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SAT)
#define CORE_OBJECT_TYPE_SAP           (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SAP)
#define CORE_OBJECT_TYPE_SMS           (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SMS)
#define CORE_OBJECT_TYPE_PHONEBOOK     (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_PHONEBOOK)
#define CORE_OBJECT_TYPE_SOUND         (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_SOUND)
#define CORE_OBJECT_TYPE_CUSTOM        (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_CUSTOM)
#define CORE_OBJECT_TYPE_GPS           (CORE_OBJECT_TYPE_DEFAULT | TCORE_TYPE_GPS)

#define CORE_OBJECT_CHECK(o,t) \
	if (!o) { warn("core_object is NULL"); return; } \
	if (tcore_object_get_type(o) != t) { warn("type(0x%x != 0x%x) mismatch", tcore_object_get_type(o), t); return; }
#define CORE_OBJECT_CHECK_RETURN(o,t,r) \
	if (!o) { warn("core_object is NULL"); return r; } \
	if (tcore_object_get_type(o) != t) { warn("type(0x%x != 0x%x) mismatch", tcore_object_get_type(o), t); return r; }

typedef void (*CoreObjectFreeHook)(CoreObject *co);
typedef void (*CoreObjectCloneHook)(CoreObject *src, CoreObject *dest);
typedef gboolean (*CoreObjectCallback)(CoreObject *co, const void *event_info, void *user_data);
typedef TReturn (*CoreObjectDispatcher)(CoreObject *co, UserRequest *ur);

CoreObject*      tcore_object_new(TcorePlugin *plugin, const char *name, TcoreHal *hal);
void             tcore_object_free(CoreObject *co);

TReturn          tcore_object_set_free_hook(CoreObject *co, CoreObjectFreeHook free_hook);
TReturn          tcore_object_set_clone_hook(CoreObject *co, CoreObjectCloneHook clone_hook);

CoreObject*      tcore_object_clone(CoreObject *src, TcorePlugin *new_parent, const char *new_name);

TReturn          tcore_object_set_name(CoreObject *co, const char *name);
const char*      tcore_object_ref_name(CoreObject *co);

TReturn          tcore_object_set_plugin(CoreObject *co, TcorePlugin *plugin);
TcorePlugin*     tcore_object_ref_plugin(CoreObject *co);

TReturn          tcore_object_link_object(CoreObject *co, void *object);
void*            tcore_object_ref_object(CoreObject *co);

TReturn          tcore_object_set_type(CoreObject *co, unsigned int type);
unsigned int     tcore_object_get_type(CoreObject *co);

TReturn          tcore_object_set_hal(CoreObject *co, TcoreHal *hal);
TcoreHal*        tcore_object_get_hal(CoreObject *co);

TReturn          tcore_object_link_user_data(CoreObject *co, void *user_data);
void*            tcore_object_ref_user_data(CoreObject *co);

TReturn          tcore_object_set_dispatcher(CoreObject *co, CoreObjectDispatcher func);
TReturn          tcore_object_dispatch_request(CoreObject *co, UserRequest *ur);

TReturn          tcore_object_add_callback(CoreObject *co, const char *event, CoreObjectCallback callback, void *user_data);
TReturn          tcore_object_del_callback(CoreObject *co, const char *event, CoreObjectCallback callback);
TReturn          tcore_object_emit_callback(CoreObject *co, const char *event, const void *event_info);

__END_DECLS

#endif
