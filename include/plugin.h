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

#ifndef __TCORE_PLUGIN_H__
#define __TCORE_PLUGIN_H__

__BEGIN_DECLS

enum tcore_plugin_priority {
	TCORE_PLUGIN_PRIORITY_HIGH = -100,
	TCORE_PLUGIN_PRIORITY_MID = 0,
	TCORE_PLUGIN_PRIORITY_LOW = +100
};

struct tcore_plugin_define_desc {
	gchar *name;
	enum tcore_plugin_priority priority;
	int version;
	gboolean (*load)();
	gboolean (*init)(TcorePlugin *);
	void (*unload)(TcorePlugin *);
};


TcorePlugin* tcore_plugin_new(Server *server,
                 const struct tcore_plugin_define_desc *desc,
                 const char *filename, void *handle);
void         tcore_plugin_free(TcorePlugin *plugin);

const struct tcore_plugin_define_desc*
             tcore_plugin_get_description(TcorePlugin *plugin);

char*        tcore_plugin_get_filename(TcorePlugin *plugin);
char*        tcore_plugin_ref_plugin_name(TcorePlugin *plugin);
Server*      tcore_plugin_ref_server(TcorePlugin *plugin);

TReturn      tcore_plugin_set_hal(TcorePlugin *plugin, TcoreHal *hal);
TcoreHal*    tcore_plugin_ref_hal(TcorePlugin *plugin);

TReturn      tcore_plugin_link_user_data(TcorePlugin *plugin, void *user_data);
void*        tcore_plugin_ref_user_data(TcorePlugin *plugin);

TReturn      tcore_plugin_add_core_object(TcorePlugin *plugin, CoreObject *co);
CoreObject*  tcore_plugin_ref_core_object(TcorePlugin *plugin, const char *name);
GSList*      tcore_plugin_get_core_objects_bytype(TcorePlugin *plugin,
                 unsigned int type);

TReturn      tcore_plugin_core_object_event_emit(TcorePlugin *plugin,
                 const char *event, const void *event_info);

TReturn      tcore_plugin_link_property(TcorePlugin *plugin, const char *key,
                 void *data);
void*        tcore_plugin_ref_property(TcorePlugin *plugin, const char *key);

__END_DECLS

#endif
