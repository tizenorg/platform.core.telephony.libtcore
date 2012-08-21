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

#ifndef __TCORE_CO_PS_H__
#define __TCORE_CO_PS_H__

#include <core_object.h>
#include <co_context.h>

__BEGIN_DECLS


#define PS_MAX_CID  4

struct tcore_ps_operations {
	TReturn (*activate_context)(CoreObject *co_ps, CoreObject *context, void *user_data);
	TReturn (*deactivate_context)(CoreObject *co_ps, CoreObject *context, void *user_data);
};

CoreObject*  tcore_ps_new(TcorePlugin *p, const char *name, struct tcore_ps_operations *ops, TcoreHal *hal);
void         tcore_ps_free(CoreObject *o);

TReturn      tcore_ps_add_context(CoreObject *o, CoreObject *ctx_o);
TReturn      tcore_ps_remove_context(CoreObject *o, CoreObject *ctx_o);
CoreObject*  tcore_ps_ref_context_by_role(CoreObject *o, enum co_context_role role);
GSList*      tcore_ps_ref_context_by_id(CoreObject *o, unsigned int id);

TReturn      tcore_ps_set_online(CoreObject *o, gboolean state);

TReturn      tcore_ps_assign_context_id(CoreObject *o, CoreObject *context, unsigned int cid);
TReturn      tcore_ps_clear_context_id(CoreObject *o, CoreObject *context);

TReturn      tcore_ps_activate_context(CoreObject *o, CoreObject *context, void *user_data);
TReturn      tcore_ps_deactivate_context(CoreObject *o, CoreObject *context, void *user_data);
TReturn      tcore_ps_deactivate_contexts(CoreObject *o);

__END_DECLS

#endif
