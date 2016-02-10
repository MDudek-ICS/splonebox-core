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

#include <stdlib.h>
#include <stddef.h>
#include <sodium.h>

#include "api/sb-api.h"
#include "sb-common.h"

int api_run(string pluginlongtermpk, string function_name, uint64_t callid,
    struct message_params_object args, msgpack_packer *pk,
    struct api_error *api_error)
{
  struct message_object *data;
  struct message_object *meta;
  struct message_request *request;
  struct message_params_object run_params;

  if (!api_error || !pk)
    return (-1);

  /* check if id is in database */
  if (db_apikey_verify(pluginlongtermpk) == -1) {
    error_set(api_error, API_ERROR_TYPE_VALIDATION, "API key is invalid.");
    return (-1);
  }

  if (db_function_verify(pluginlongtermpk, function_name, &args) == -1) {
    error_set(api_error, API_ERROR_TYPE_VALIDATION,
        "run() verification failed.");
    return (-1);
  }

  request = CALLOC(1, struct message_request);

  if (!request)
    return (-1);

  run_params.size = 3;
  run_params.obj = CALLOC(3, struct message_object);

  if (!run_params.obj)
    return (-1);

  /* data refs to first run_params parameter */
  meta = &run_params.obj[0];

  meta->type = OBJECT_TYPE_ARRAY;

  /* meta = [nil, callid] */
  meta->data.params.size = 2;
  meta->data.params.obj = CALLOC(2, struct message_object);

  if (!meta->data.params.obj)
    return (-1);

  /* add nil */
  data = &meta->data.params.obj[0];
  data->type = OBJECT_TYPE_NIL;

  /* add callid */
  data = &meta->data.params.obj[1];
  data->type = OBJECT_TYPE_UINT;
  data->data.uinteger = callid;

  /* add function name, data refs to second run_params parameter */
  data = &run_params.obj[1];
  data->type = OBJECT_TYPE_STR;
  data->data.string = function_name;

  /* add function parameters, data refs to third run_params parameter */
  data = &run_params.obj[2];

  data->type = OBJECT_TYPE_ARRAY;
  data->data.params = args;

  request->type = MESSAGE_TYPE_REQUEST;
  request->msgid = randombytes_random();
  request->method = cstring_copy_string("run");
  request->params = run_params;

  message_serialize_request(request, pk);

  return (0);
}
