/**
 *    Copyright (C) 2015 splone UG
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

#include "helper-unix.h"

void unit_server_start(void **state);
void unit_server_stop(void **state);
void unit_dispatch_table_get(void **state);

void functional_client_connect(void **state);
void functional_db_connect(void **state);
void functional_db_plugin_add(void **state);
void functional_db_pluginkey_verify(void **state);
void functional_db_function_add(void **state);
void functional_db_function_verify(void **state);
void functional_db_function_flush_args(void **state);
void functional_filesystem_load(void **state);
void functional_filesystem_save_sync(void **state);
void functional_dispatch_handle_register(void **state);
void functional_dispatch_handle_run(void **state);
void functional_dispatch_handle_result(void **state);
void functional_dispatch_handle_subscribe(void **state);
void functional_dispatch_handle_broadcast(void **state);
void functional_msgpack_rpc_helper(void **state);
void functional_crypto(void **state);
void functional_confparse(void **state);
void functional_db_whitelist(void **state);

const struct CMUnitTest tests[] = {
  cmocka_unit_test(unit_dispatch_table_get),
  cmocka_unit_test(unit_server_start),
  cmocka_unit_test(unit_server_stop),
  cmocka_unit_test(functional_db_connect),
  cmocka_unit_test(functional_db_plugin_add),
  cmocka_unit_test(functional_db_pluginkey_verify),
  cmocka_unit_test(functional_db_function_add),
  cmocka_unit_test(functional_db_function_verify),
  cmocka_unit_test(functional_db_function_flush_args),
  cmocka_unit_test(functional_filesystem_load),
  cmocka_unit_test(functional_filesystem_save_sync),
  cmocka_unit_test(functional_dispatch_handle_register),
  cmocka_unit_test(functional_dispatch_handle_run),
  cmocka_unit_test(functional_dispatch_handle_result),
  cmocka_unit_test(functional_dispatch_handle_subscribe),
  cmocka_unit_test(functional_dispatch_handle_broadcast),
  cmocka_unit_test(functional_msgpack_rpc_helper),
  cmocka_unit_test(functional_crypto),
  cmocka_unit_test(functional_confparse),
  cmocka_unit_test(functional_db_whitelist),
};
