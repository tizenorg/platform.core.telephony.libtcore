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

#ifndef __TCORE_CO_CUSTOM_H__
#define __TCORE_CO_CUSTOM_H__

__BEGIN_DECLS

typedef void *tcore_custom_operations;

CoreObject *tcore_custom_new(TcorePlugin *p,
	const char *name, tcore_custom_operations ops,
	CoreObjectDispatcher dispatcher, TcoreHal *hal);
void tcore_custom_free(CoreObject *co);

__END_DECLS

#endif	/* __TCORE_CO_CUSTOM_H__ */
