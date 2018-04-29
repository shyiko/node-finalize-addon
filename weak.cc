#include <node_api.h>

// All #define blocks below were taken from 
// https://github.com/nodejs/node/blob/v9.10.1/test/addons-napi/common.h
// https://github.com/nodejs/node/blob/v9.10.1/LICENSE

#define NAPI_RETVAL_NOTHING

#define GET_AND_THROW_LAST_ERROR(env)                                    \
  do {                                                                   \
    const napi_extended_error_info *error_info;                          \
    napi_get_last_error_info((env), &error_info);                        \
    bool is_pending;                                                     \
    napi_is_exception_pending((env), &is_pending);                       \
    /* If an exception is already pending, don't rethrow it */           \
    if (!is_pending) {                                                   \
      const char* error_message = error_info->error_message != NULL ?    \
        error_info->error_message :                                      \
        "empty error message";                                           \
      napi_throw_error((env), NULL, error_message);                      \
    }                                                                    \
  } while (0)

#define NAPI_ASSERT_BASE(env, assertion, message, ret_val)               \
  do {                                                                   \
    if (!(assertion)) {                                                  \
      napi_throw_error(                                                  \
          (env),                                                         \
        NULL,                                                            \
          "assertion (" #assertion ") failed: " message);                \
      return ret_val;                                                    \
    }                                                                    \
  } while (0)

#define NAPI_ASSERT(env, assertion, message)                             \
  NAPI_ASSERT_BASE(env, assertion, message, NULL)

#define NAPI_ASSERT_RETURN_VOID(env, assertion, message)                 \
  NAPI_ASSERT_BASE(env, assertion, message, NAPI_RETVAL_NOTHING)

#define NAPI_CALL_BASE(env, the_call, ret_val)                           \
  do {                                                                   \
    if ((the_call) != napi_ok) {                                         \
      GET_AND_THROW_LAST_ERROR((env));                                   \
      return ret_val;                                                    \
    }                                                                    \
  } while (0)

#define NAPI_CALL(env, the_call)                                         \
  NAPI_CALL_BASE(env, the_call, NULL)

#define NAPI_CALL_RETURN_VOID(env, the_call)                             \
  NAPI_CALL_BASE(env, the_call, NAPI_RETVAL_NOTHING)

// end of #define(s)

void ExecuteFinalizer(napi_env env, void* data, void* hint) {
  napi_ref func_ref = (napi_ref) data;
  napi_value func;
  NAPI_CALL_RETURN_VOID(env, 
    napi_get_reference_value(env, func_ref, &func));
  NAPI_CALL_RETURN_VOID(env, 
    napi_delete_reference(env, func_ref));

  napi_value global;
  NAPI_CALL_RETURN_VOID(env, 
    napi_get_global(env, &global));

  napi_value rs_name;
  NAPI_CALL_RETURN_VOID(env, 
    napi_create_string_utf8(env, "finalizer", NAPI_AUTO_LENGTH, &rs_name));

  napi_async_context ctx;
  NAPI_CALL_RETURN_VOID(env, 
    napi_async_init(env, func, rs_name, &ctx));

  napi_make_callback(env, ctx, global, func, 0, nullptr, nullptr);

  NAPI_CALL_RETURN_VOID(env, 
    napi_async_destroy(env, ctx));
}

napi_value Weak(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];  
  NAPI_CALL(env, 
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
  NAPI_ASSERT(env, 
    argc == 1, "Expected exactly 1 argument");

  napi_value func = args[0];

  napi_valuetype func_type;
  NAPI_CALL(env, 
    napi_typeof(env, func, &func_type));
  NAPI_ASSERT(env, 
    func_type == napi_function, "Expected arguments[0] to be a function");

  napi_ref func_ref;
  NAPI_CALL(env,
    napi_create_reference(env, func, 1, &func_ref));

  napi_value result;
  NAPI_CALL(env,
    napi_create_external(env, func_ref, ExecuteFinalizer, nullptr, &result));
  return result;
}

napi_value Init(napi_env env, napi_value exports) {
  NAPI_CALL(env, 
    napi_create_function(env, "exports", -1, Weak, nullptr, &exports));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)