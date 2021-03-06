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

#ifndef __TCORE_UDEV_H__
#define __TCORE_UDEV_H__

#include <gudev/gudev.h>

__BEGIN_DECLS

typedef gboolean (*TcoreUdevEnumerCallback)(TcoreUdev *udev, GList *list, void *user_data);
typedef gboolean (*TcoreUdevCallback)(TcoreUdev *udev, GUdevDevice *device, void *user_data);

TcoreUdev*       tcore_udev_new(Server *s, const gchar **subsystems);
void             tcore_udev_free(TcoreUdev *udev);

Server*          tcore_udev_ref_server(TcoreUdev *udev);
GUdevClient*     tcore_udev_ref_client(TcoreUdev *udev);
GUdevEnumerator* tcore_udev_ref_enumerator(TcoreUdev *udev);

TReturn          tcore_udev_add_enumerator_callback(TcoreUdev *udev, TcoreUdevEnumerCallback func, void *user_data);
GList*           tcore_udev_exec_enumerator(TcoreUdev *udev, gboolean event_emit_flag);

TReturn          tcore_udev_add_callback(TcoreUdev *udev, const char *subsystem, const char *action, TcoreUdevCallback func, void *user_data);

__END_DECLS

#endif
