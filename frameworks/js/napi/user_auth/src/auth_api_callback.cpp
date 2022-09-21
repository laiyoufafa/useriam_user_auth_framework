/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "auth_api_callback.h"

#include <uv.h>

#include "securec.h"

#include "iam_logger.h"

#define LOG_LABEL UserIam::Common::LABEL_USER_AUTH_NAPI

namespace OHOS {
namespace UserIam {
namespace UserAuth {
using namespace OHOS::UserIam::UserAuth;

AuthApiCallback::AuthApiCallback(AuthInfo *authInfo)
{
    authInfo_ = authInfo;
    executeInfo_ = nullptr;
}

AuthApiCallback::AuthApiCallback(ExecuteInfo *executeInfo)
{
    authInfo_ = nullptr;
    executeInfo_ = executeInfo;
}

AuthApiCallback::~AuthApiCallback()
{
    if (authInfo_ != nullptr) {
        delete authInfo_;
    }
    if (executeInfo_ != nullptr) {
        delete executeInfo_;
    }
}

napi_value AuthApiCallback::Uint8ArrayToNapi(napi_env env, std::vector<uint8_t> value)
{
    size_t size = value.size();
    IAM_LOGI("size = %{public}zu", size);
    napi_value out = nullptr;
    void *data = nullptr;
    napi_value buffer = nullptr;
    NAPI_CALL(env, napi_create_arraybuffer(env, size, &data, &buffer));
    if (size != 0) {
        errno_t ret = memcpy_s(data, size, value.data(), value.size());
        if (ret != EOK) {
            IAM_LOGE("memcpy_s failed");
            return out;
        }
    }
    NAPI_CALL(env, napi_create_typedarray(env, napi_uint8_array, size, buffer, 0, &out));
    return out;
}

napi_value AuthApiCallback::BuildOnResult(
    napi_env env, uint32_t remainTimes, uint32_t freezingTime, std::vector<uint8_t> token)
{
    napi_value jsObject = nullptr;
    NAPI_CALL(env, napi_create_object(env, &jsObject));

    napi_value remainTimesValue = 0;
    NAPI_CALL(env, napi_create_uint32(env, remainTimes, &remainTimesValue));
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "remainTimes", remainTimesValue));

    napi_value freezingTimeValue = 0;
    NAPI_CALL(env, napi_create_uint32(env, freezingTime, &freezingTimeValue));
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "freezingTime", freezingTimeValue));

    napi_value jsToken = Uint8ArrayToNapi(env, token);
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "token", jsToken));
    return jsObject;
}

void AuthApiCallback::OnAuthAcquireInfo(AcquireInfoInner *acquireInfoInner)
{
    IAM_LOGI("start");
    uv_loop_s *loop(nullptr);
    napi_get_uv_event_loop(acquireInfoInner->env, &loop);
    if (loop == nullptr) {
        IAM_LOGE("loop is null");
        delete acquireInfoInner;
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        IAM_LOGE("work is null");
        delete acquireInfoInner;
        return;
    }
    work->data = reinterpret_cast<void *>(acquireInfoInner);
    uv_queue_work(loop, work, [] (uv_work_t *work) {}, [] (uv_work_t *work, int status) {
        IAM_LOGI("Do OnAuthAcquireInfo work");
        AcquireInfoInner *acquireInfoInner = reinterpret_cast<AcquireInfoInner *>(work->data);
        if (acquireInfoInner == nullptr) {
            IAM_LOGE("acquireInfoInner is null");
            delete work;
            return;
        }
        napi_env env = acquireInfoInner->env;
        napi_value returnOnAcquire = nullptr;
        napi_value callback;
        napi_status napiStatus = napi_get_reference_value(env, acquireInfoInner->onAcquireInfo, &callback);
        if (napiStatus != napi_ok) {
            IAM_LOGE("napi_get_reference_value failed");
            delete acquireInfoInner;
            delete work;
            return;
        }
        napi_value params[PARAM3];
        napi_create_int32(env, acquireInfoInner->module, &params[PARAM0]);
        napi_create_uint32(env, acquireInfoInner->acquireInfo, &params[PARAM1]);
        napi_create_int32(env, acquireInfoInner->extraInfo, &params[PARAM2]);
        napiStatus = napi_call_function(env, nullptr, callback, PARAM3, params, &returnOnAcquire);
        if (napiStatus != napi_ok) {
            IAM_LOGE("napi_call_function failed");
        }
        delete acquireInfoInner;
        delete work;
    });
}

void AuthApiCallback::OnAcquireInfo(int32_t module, uint32_t acquireInfo, const Attributes &extraInfo)
{
    IAM_LOGI("start");
    if (authInfo_ != nullptr) {
        AcquireInfoInner *acquireInfoInner = new (std::nothrow) AcquireInfoInner();
        if (acquireInfoInner == nullptr) {
            IAM_LOGE("acquireInfoInner is null");
            return;
        }
        acquireInfoInner->env = authInfo_->env;
        acquireInfoInner->onAcquireInfo = authInfo_->onAcquireInfo;
        acquireInfoInner->module = module;
        acquireInfoInner->acquireInfo = acquireInfo;
        acquireInfoInner->extraInfo = 0;
        OnAuthAcquireInfo(acquireInfoInner);
    } else {
        IAM_LOGE("authInfo_ is nullptr");
    }
    IAM_LOGI("end");
}

static void OnAuthResultWork(uv_work_t *work, int status)
{
    IAM_LOGI("start");
    AuthInfo *authInfo = reinterpret_cast<AuthInfo *>(work->data);
    if (authInfo == nullptr) {
        IAM_LOGE("authInfo is null");
        delete work;
        return;
    }
    napi_env env = authInfo->env;
    napi_value callback = nullptr;
    napi_value params[PARAM2] = {nullptr};
    napi_value return_val = nullptr;
    napi_status napiStatus = napi_get_reference_value(env, authInfo->onResult, &callback);
    if (napiStatus != napi_ok) {
        IAM_LOGE("napi_get_reference_value failed");
        goto EXIT;
    }
    napi_create_int32(env, authInfo->result, &params[PARAM0]);
    params[PARAM1] = AuthApiCallback::BuildOnResult(
        env, authInfo->remainTimes, authInfo->freezingTime, authInfo->token);
    napi_call_function(env, nullptr, callback, PARAM2, params, &return_val);
EXIT:
    delete authInfo;
    delete work;
}

void AuthApiCallback::OnAuthResult(int32_t result, const UserIam::UserAuth::Attributes &extraInfo)
{
    IAM_LOGI("start");
    uv_loop_s *loop(nullptr);
    napi_get_uv_event_loop(authInfo_->env, &loop);
    if (loop == nullptr) {
        IAM_LOGE("loop is null");
        delete authInfo_;
        authInfo_ = nullptr;
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        IAM_LOGE("work is null");
        delete authInfo_;
        authInfo_ = nullptr;
        return;
    }
    authInfo_->result = result;

    if (!extraInfo.GetUint8ArrayValue(Attributes::ATTR_SIGNATURE, authInfo_->token)) {
        IAM_LOGE("ATTR_AUTH_TOKEN is null");
    }
    if (!extraInfo.GetInt32Value(Attributes::ATTR_REMAIN_TIMES, authInfo_->remainTimes)) {
        IAM_LOGE("ATTR_REMAIN_TIMES is null");
    }
    if (!extraInfo.GetInt32Value(Attributes::ATTR_FREEZING_TIME, authInfo_->freezingTime)) {
        IAM_LOGE("ATTR_FREEZING_TIME is null");
    }

    work->data = reinterpret_cast<void *>(authInfo_);
    authInfo_ = nullptr;
    uv_queue_work(loop, work, [] (uv_work_t *work) {}, OnAuthResultWork);
}

static void OnExecuteResultWork(uv_work_t *work, int status)
{
    IAM_LOGI("start");
    ExecuteInfo *executeInfo = reinterpret_cast<ExecuteInfo *>(work->data);
    if (executeInfo == nullptr) {
        IAM_LOGE("executeInfo is null");
        delete work;
        return;
    }
    napi_env env = executeInfo->env;
    napi_value result;
    if (napi_create_int32(env, executeInfo->result, &result) != napi_ok) {
        IAM_LOGE("napi_create_int32 failed");
        goto EXIT;
    }
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    if (executeInfo->isPromise) {
        if (executeInfo->result == static_cast<int32_t>(ResultCode::SUCCESS)) {
            IAM_LOGI("resolve promise %{public}d",
                napi_resolve_deferred(env, executeInfo->deferred, result));
        } else {
            IAM_LOGE("reject promise %{public}d",
                napi_reject_deferred(env, executeInfo->deferred, result));
        }
    } else {
        napi_value callback;
        napi_get_reference_value(env, executeInfo->callbackRef, &callback);
        napi_value callResult = nullptr;
        IAM_LOGI("do callback %{public}d",
            napi_call_function(env, undefined, callback, 1, &result, &callResult));
    }
EXIT:
    delete executeInfo;
    delete work;
}

void AuthApiCallback::OnExecuteResult(const int32_t result)
{
    IAM_LOGI("start");
    uv_loop_s *loop(nullptr);
    napi_get_uv_event_loop(executeInfo_->env, &loop);
    if (loop == nullptr) {
        IAM_LOGE("loop is null");
        delete executeInfo_;
        executeInfo_ = nullptr;
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        IAM_LOGE("work is null");
        delete executeInfo_;
        executeInfo_ = nullptr;
        return;
    }

    auto res = result2ExecuteResult.find(result);
    if (res == result2ExecuteResult.end()) {
        executeInfo_->result = static_cast<int32_t>(ResultCode::GENERAL_ERROR);
        IAM_LOGE("result %{public}d not found, set execute result GENERAL_ERROR", result);
    } else {
        executeInfo_->result = static_cast<int32_t>(res->second);
        IAM_LOGI("convert result %{public}d to execute result %{public}d", result, executeInfo_->result);
    }

    work->data = reinterpret_cast<void *>(executeInfo_);
    executeInfo_ = nullptr;
    uv_queue_work(loop, work, [] (uv_work_t *work) {}, OnExecuteResultWork);
}

void AuthApiCallback::OnResult(int32_t result, const Attributes &extraInfo)
{
    IAM_LOGI("start result = %{public}d", result);
    if (authInfo_ != nullptr) {
        OnAuthResult(result, extraInfo);
    } else {
        IAM_LOGI("authInfo_ is nullptr");
    }
    if (executeInfo_ != nullptr) {
        OnExecuteResult(result);
    } else {
        IAM_LOGI("executeInfo_ is nullptr ");
    }
    IAM_LOGI("end");
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS