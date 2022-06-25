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

#ifndef USER_AUTH_H
#define USER_AUTH_H

#include <cstdint>
#include <iremote_broker.h>
#include <optional>

#include "attributes.h"
#include "refbase.h"
#include "user_auth_callback.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class UserAuth : public IRemoteBroker {
public:
    /* Message ID */
    enum : uint32_t {
        USER_AUTH_GET_AVAILABLE_STATUS = 0,
        USER_AUTH_GET_PROPERTY,
        USER_AUTH_GET_PROPERTY_BY_ID,
        USER_AUTH_SET_PROPERTY,
        USER_AUTH_AUTH,
        USER_AUTH_AUTH_USER,
        USER_AUTH_CANCEL_AUTH,
        USER_AUTH_GET_VERSION,
        USER_AUTH_ON_RESULT,
        USER_AUTH_GET_EX_PROP,
        USER_AUTH_SET_EX_PROP,
        USER_AUTH_ACQUIRE_INFO,
        USER_AUTH_IDENTIFY,
        USER_AUTH_CANCEL_IDENTIFY,
        USER_AUTH_ON_IDENTIFY_RESULT,
    };

    virtual int32_t GetAvailableStatus(AuthType authType, AuthTrustLevel authTrustLevel) = 0;

    virtual void GetProperty(std::optional<int32_t> userId, AuthType authType,
        const std::vector<Attributes::AttributeKey> &keys, sptr<GetExecutorPropertyCallback> &callback) = 0;

    virtual void SetProperty(std::optional<int32_t> userId, AuthType authType, const Attributes &attributes,
        sptr<SetExecutorPropertyCallback> &callback) = 0;

    virtual uint64_t AuthUser(std::optional<int32_t> userId, const std::vector<uint8_t> &challenge, AuthType authType,
        AuthTrustLevel authTrustLevel, sptr<UserAuthCallback> &callback) = 0;

    virtual uint64_t Identify(const std::vector<uint8_t> &challenge, AuthType authType,
        sptr<UserAuthCallback> &callback) = 0;

    virtual int32_t CancelAuthOrIdentify(uint64_t contextId) = 0;

    virtual int32_t GetVersion() = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.UserIAM.UserAuth.IUserAuth");
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // USER_AUTH_H