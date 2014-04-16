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

#ifndef __CO_PS_H__
#define __CO_PS_H__

#include <core_object.h>
#include <tel_return.h>
#include <co_context.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TCORE_PS_MAX_CID  4

typedef struct {
	TelReturn (*define_context)(CoreObject *co_ps, CoreObject *context,
		TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*activate_context)(CoreObject *co_ps, CoreObject *context,
		TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*deactivate_context)(CoreObject *co_ps, CoreObject *context,
		TcoreObjectResponseCallback cb, void *cb_data);
} TcorePsOps;

CoreObject *tcore_ps_new(TcorePlugin *p,
	TcorePsOps *ops, TcoreHal *hal);
void tcore_ps_free(CoreObject *o);

gboolean tcore_ps_set_ops(CoreObject *o, TcorePsOps *ps_ops);
void tcore_ps_override_ops(CoreObject *o, TcorePsOps *ps_ops);

gboolean tcore_ps_set_online(CoreObject *o, gboolean state);

gboolean tcore_ps_assign_context_id(CoreObject *o, CoreObject *context, guint cid);
gboolean tcore_ps_clear_context_id(CoreObject *o, CoreObject *context);

CoreObject *tcore_ps_ref_context_by_role(CoreObject *o, TcoreContextRole role);
gboolean tcore_ps_ref_context_by_id(CoreObject *o, guint id, GSList **list);

gboolean tcore_ps_is_active_apn(CoreObject *o, const char* apn);
#ifdef __cplusplus
}
#endif

#endif	/* __CO_PS_H__ */
