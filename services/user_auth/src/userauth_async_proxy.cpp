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

#include "userauth_async_proxy.h"
#include "userauth_hilog_wrapper.h"
#include "iuser_auth.h"

namespace OHOS {
namespace UserIAM {
namespace UserAuth {
void UserAuthAsyncProxy::onAcquireInfo(const int32_t module, const uint32_t acquireInfo, const int32_t extraInfo)
{
    USERAUTH_HILOGD(MODULE_SERVICE, "UserAuthAsyncProxy onAcquireInfo start");

    MessageParcel data;
    MessageParcel reply;

    if (!data.WriteInterfaceToken(UserAuthAsyncProxy::GetDescriptor())) {
        USERAUTH_HILOGE(MODULE_SERVICE, "write descriptor failed");
        return;
    }
    if (!data.WriteInt32(module)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write module");
        return;
    }
    if (!data.WriteUint32(acquireInfo)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write acquireInfo");
        return;
    }
    if (!data.WriteInt32(extraInfo)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write extraInfo");
        return;
    }

    bool ret = SendRequest(IUserAuth::USER_AUTH_ACQUIRE_INFO, data, reply);
    if (ret) {
        int32_t result = reply.ReadInt32();
        USERAUTH_HILOGE(MODULE_SERVICE, "result = %{public}d", result);
    }
}

void UserAuthAsyncProxy::onResult(const int32_t result, const AuthResult &extraInfo)
{
    USERAUTH_HILOGD(MODULE_SERVICE, "UserAuthAsyncProxy onResult start");
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(UserAuthAsyncProxy::GetDescriptor())) {
        USERAUTH_HILOGE(MODULE_SERVICE, "write descriptor failed");
        return;
    }
    if (!data.WriteInt32(result)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write result");
        return;
    }
    if (!data.WriteUInt8Vector(extraInfo.token)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write token");
        return;
    }
    if (!data.WriteUint32(extraInfo.remainTimes)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write remainTimes");
        return;
    }
    if (!data.WriteUint32(extraInfo.freezingTime)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write freezingTime");
        return;
    }
    bool ret = SendRequest(IUserAuth::USER_AUTH_ON_RESULT, data, reply);
    if (ret) {
        int32_t result = reply.ReadInt32();
        USERAUTH_HILOGE(MODULE_SERVICE, "result = %{public}d", result);
    }
}

void UserAuthAsyncProxy::onExecutorPropertyInfo(const ExecutorProperty &result)
{
    USERAUTH_HILOGD(MODULE_SERVICE, "UserAuthAsyncProxy onExecutorPropertyInfo start");
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(UserAuthAsyncProxy::GetDescriptor())) {
        USERAUTH_HILOGE(MODULE_SERVICE, "write descriptor failed");
        return;
    }
    if (!data.WriteInt32(result.result)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write result");
        return;
    }
    if (!data.WriteUint64(result.authSubType)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write authSubType");
        return;
    }
    if (!data.WriteUint32(result.remainTimes)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write remainTimes");
        return;
    }
    if (!data.WriteUint32(result.freezingTime)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write freezingTime");
        return;
    }
    bool ret = SendRequest(IUserAuth::USER_AUTH_GET_EX_PROP, data, reply);
    if (ret) {
        int32_t result = reply.ReadInt32();
        USERAUTH_HILOGE(MODULE_SERVICE, "result = %{public}d", result);
    }
}

void UserAuthAsyncProxy::onSetExecutorProperty(const int32_t result)
{
    USERAUTH_HILOGD(MODULE_SERVICE, "UserAuthAsyncProxy onSetExecutorProperty start");

    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(UserAuthAsyncProxy::GetDescriptor())) {
        USERAUTH_HILOGE(MODULE_SERVICE, "write descriptor failed");
        return;
    }
    if (!data.WriteInt32(result)) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to write result");
        return;
    }

    bool ret = SendRequest(IUserAuth::USER_AUTH_SET_EX_PROP, data, reply);
    if (ret) {
        int32_t result = reply.ReadInt32();
        USERAUTH_HILOGE(MODULE_SERVICE, "result = %{public}d", result);
    }
}

bool UserAuthAsyncProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    USERAUTH_HILOGD(MODULE_SERVICE, "UserAuthAsyncProxy SendRequest start");

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to get remote");
        return false;
    }
    MessageOption option(MessageOption::TF_SYNC);
    int32_t result = remote->SendRequest(code, data, reply, option);
    if (result != OHOS::UserIAM::UserAuth::SUCCESS) {
        USERAUTH_HILOGE(MODULE_SERVICE, "failed to SendRequest result = %{public}d", result);
        return false;
    }
    USERAUTH_HILOGD(MODULE_SERVICE, "SendRequest end");
    return true;
}
}  // namespace UserAuth
}  // namespace UserIAM
}  // namespace OHOS
