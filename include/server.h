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

#ifndef __TCORE_SERVER_H__
#define __TCORE_SERVER_H__

__BEGIN_DECLS

typedef enum tcore_hook_return (*TcoreServerRequestHook)(Server *s,
    UserRequest *ur, void *user_data);
typedef enum tcore_hook_return (*TcoreServerNotificationHook)(Server *s,
    CoreObject *source, enum tcore_notification_command command,
    unsigned int data_len, void *data, void *user_data);

Server*       tcore_server_new();
void          tcore_server_free(Server *s);

TReturn       tcore_server_run(Server *s);
TReturn       tcore_server_exit(Server *s);

TReturn       tcore_server_add_plugin(Server *s, TcorePlugin *plugin);
GSList*       tcore_server_ref_plugins(Server *s);
TcorePlugin*  tcore_server_find_plugin(Server *s, const char *name);

TReturn       tcore_server_add_communicator(Server *s, Communicator *comm);
GSList*       tcore_server_ref_communicators(Server *s);
Communicator* tcore_server_find_communicator(Server *s, const char *name);

TReturn       tcore_server_add_storage(Server *s, Storage *strg);
GSList*       tcore_server_ref_storages(Server *s);
Storage*      tcore_server_find_storage(Server *s, const char *name);

TReturn       tcore_server_add_hal(Server *s, TcoreHal *hal);
GSList*       tcore_server_ref_hals(Server *s);
TcoreHal*     tcore_server_find_hal(Server *s, const char *name);

TReturn       tcore_server_dispatch_request(Server *s, UserRequest *ur);
TReturn       tcore_server_send_notification(Server *s, CoreObject *source,
                  enum tcore_notification_command command,
                  unsigned int data_len, void *data);

TReturn       tcore_server_add_request_hook(Server *s,
                  enum tcore_request_command command,
                  TcoreServerRequestHook func, void *user_data);
TReturn       tcore_server_remove_request_hook(Server *s,
                  TcoreServerRequestHook func);
TReturn       tcore_server_add_notification_hook(Server *s,
                  enum tcore_notification_command command,
                  TcoreServerNotificationHook func, void *user_data);
TReturn       tcore_server_remove_notification_hook(Server *s,
                  TcoreServerNotificationHook func);

__END_DECLS

#endif
