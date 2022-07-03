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
#ifndef IAM_MOCK_USER_IDM_SERVICE_H
#define IAM_MOCK_USER_IDM_SERVICE_H
#include <memory>

#include <gmock/gmock.h>

#include "user_idm_service.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class MockUserIdmService final : public UserIdmStub {
public:
    MOCK_METHOD2(OpenSession, int32_t(std::optional<int32_t> userId, std::vector<uint8_t> &challenge));
    MOCK_METHOD1(CloseSession, void(std::optional<int32_t> userId));
    MOCK_METHOD3(GetCredentialInfo,
        int32_t(std::optional<int32_t> userId, AuthType authType, const sptr<IdmGetCredentialInfoCallback> &callback));
    MOCK_METHOD2(GetSecInfo,
        int32_t(std::optional<int32_t> userId, const sptr<IdmGetSecureUserInfoCallback> &callback));
    MOCK_METHOD6(AddCredential, void(std::optional<int32_t> userId, AuthType authType, PinSubType pinSubType,
        const std::vector<uint8_t> &token, const sptr<IdmCallback> &callback, bool isUpdate));
    MOCK_METHOD5(UpdateCredential, void(std::optional<int32_t> userId, AuthType authType, PinSubType pinSubType,
                                       const std::vector<uint8_t> &token, const sptr<IdmCallback> &callback));
    MOCK_METHOD2(Cancel, int32_t(std::optional<int32_t> userId, const std::optional<std::vector<uint8_t>> &challenge));
    MOCK_METHOD2(EnforceDelUser, int32_t(int32_t userId, const sptr<IdmCallback> &callback));
    MOCK_METHOD3(DelUser,
        void(std::optional<int32_t> userId, const std::vector<uint8_t> authToken, const sptr<IdmCallback> &callback));
    MOCK_METHOD4(DelCredential, void(std::optional<int32_t> userId, uint64_t credentialId,
                                    const std::vector<uint8_t> &authToken, const sptr<IdmCallback> &callback));
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // IAM_MOCK_USER_IDM_SERVICE_H