/*
 * custom
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Anga Santhosh Kumar <santhosh.a@samsung.com>
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

#include <glib.h>

#include "tcore.h"
#include "core_object.h"
#include "co_custom.h"

static void _free_hook(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_CUSTOM);

	tcore_object_link_object(co, NULL);
}

CoreObject *tcore_custom_new(TcorePlugin *p,
	const char *name, tcore_custom_operations ops,
	CoreObjectDispatcher dispatcher, TcoreHal *hal)
{
	CoreObject *co = NULL;

	if (!p)
		return NULL;

	co = tcore_object_new(p, name, hal);
	if (!co)
		return NULL;

	tcore_object_set_type(co, CORE_OBJECT_TYPE_CUSTOM);

	tcore_object_link_object(co, ops);
	tcore_object_set_free_hook(co, _free_hook);
	tcore_object_set_dispatcher(co, dispatcher);

	return co;
}

void tcore_custom_free(CoreObject *co)
{
	CORE_OBJECT_CHECK(co, CORE_OBJECT_TYPE_CUSTOM);

	tcore_object_free(co);
}
