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

#ifndef __TCORE_COMMUNICATOR_H__
#define __TCORE_COMMUNICATOR_H__

__BEGIN_DECLS

struct tcore_communitor_operations {
	TReturn (*send_response)(Communicator *comm, UserRequest *ur,
        enum tcore_response_command command,
        unsigned int data_len, const void *data);

	TReturn (*send_notification)(Communicator *comm, CoreObject *source,
        enum tcore_notification_command command,
        unsigned int data_len, const void *data);
};

Communicator* tcore_communicator_new(TcorePlugin *plugin, const char *name,
                  struct tcore_communitor_operations *ops);
void          tcore_communicator_free();

TcorePlugin*  tcore_communicator_ref_plugin(Communicator *comm);
const char*   tcore_communicator_ref_name(Communicator *comm);

TReturn       tcore_communicator_link_user_data(Communicator *comm, void *data);
void*         tcore_communicator_ref_user_data(Communicator *comm);

TReturn       tcore_communicator_send_response(Communicator *comm,
                  UserRequest *ur,  enum tcore_response_command command,
                  unsigned int data_len, const void *data);

TReturn       tcore_communicator_send_notification(Communicator *comm,
                  CoreObject *source, enum tcore_notification_command command,
                  unsigned int data_len, const void *data);

TReturn       tcore_communicator_dispatch_request(Communicator *comm, UserRequest *ur);

__END_DECLS

#endif
