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

#ifndef __TCORE_TYPES_H__
#define __TCORE_TYPES_H__

#define tcore_check_null_ret_err(name, value, err) do { \
	if ( !value ) { \
		dbg("[error] %s : NULL", name ); \
		return err; \
	} \
} while (FALSE)

#define tcore_check_null_ret(name, value) do { \
	if ( !value ) { \
		dbg("[error] %s : NULL", name ); \
		return; \
	} \
} while (FALSE)

#endif	/* __TCORE_TYPES_H__ */
