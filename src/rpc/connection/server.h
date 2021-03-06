/**
 *    Copyright (C) 2016 splone UG
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <uv.h>         // for uv_handle_t, uv_stream_t
#include "sb-common.h"  // for STATIC

STATIC void connection_cb(uv_stream_t *server_stream, int status);
STATIC void client_free_cb(uv_handle_t *handle);
STATIC void server_free_cb(uv_handle_t *handle);
