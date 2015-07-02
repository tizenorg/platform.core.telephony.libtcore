/*
 * libtcore
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd. All rights reserved.
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

#ifndef __MANAGER_H__
#define __MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum tcore_manager_return (*ManagerRequestDispatcher)(Manager *manager, UserRequest *ur);
typedef enum tcore_manager_return (*ManagerNotificationHandler)(Manager *manager, CoreObject *source,
	enum tcore_notification_command command,
	unsigned int data_len, void *data);


/* APIs */
Manager *tcore_manager_new(TcorePlugin *manager_plugin);
void tcore_manager_free(TcorePlugin *manager_plugin, Manager *manager);

TReturn tcore_manager_set_request_dispatcher(Manager *manager,
	ManagerRequestDispatcher request_dispatcher);
TReturn tcore_manager_set_notification_handler(Manager *manager,
	ManagerNotificationHandler notification_handler);

enum tcore_manager_return tcore_manager_dispatch_request(Manager *manager, UserRequest *ur);
enum tcore_manager_return tcore_manager_send_notification(Manager *manager, CoreObject *source,
	enum tcore_notification_command command,
	unsigned int data_len, void *data);

TcorePlugin *tcore_manager_get_plugin(Manager *manager);

#ifdef __cplusplus
}
#endif

#endif /* __MANAGER_H__ */
