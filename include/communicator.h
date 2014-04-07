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

#ifndef __COMMUNICATOR_H__
#define __COMMUNICATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TelReturn (*send_notification)(Communicator *comm,
	TcorePlugin *source, gint command, unsigned int data_len, const void *data);
} TcoreCommunicatorOps ;

Communicator *tcore_communicator_new(TcorePlugin *plugin,
		const gchar *name, TcoreCommunicatorOps *ops);
void tcore_communicator_free(Communicator *comm);

TcorePlugin *tcore_communicator_ref_plugin(Communicator *comm);
const gchar *tcore_communicator_ref_name(Communicator *comm);

TelReturn tcore_communicator_link_user_data(Communicator *comm, void *data);
void *tcore_communicator_ref_user_data(Communicator *comm);

TelReturn tcore_communicator_send_notification(Communicator *comm,
	TcorePlugin *source, gint command, guint data_len, const void *data);

#ifdef __cplusplus
}
#endif

#endif	/* __COMMUNICATOR_H__ */
