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

#ifndef IAM_USER_IDM_SESSION_CONTROLLER_H
#define IAM_USER_IDM_SESSION_CONTROLLER_H

#include <unordered_map>
#include <vector>

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class UserIdmSessionController {
public:
    using SessionMap = std::unordered_map<int32_t, std::vector<uint8_t>>;
    static UserIdmSessionController &Instance();

    virtual bool OpenSession(int32_t userId, std::vector<uint8_t> &challenge) = 0;
    virtual bool CloseSession(int32_t userId) = 0;
    virtual bool CloseSession(const std::vector<uint8_t> &challenge) = 0;
    virtual bool IsSessionOpened(int32_t userId) const = 0;
    virtual bool IsSessionOpened(const std::vector<uint8_t> &challenge) const = 0;
    virtual bool ForceReset() = 0;
    virtual SessionMap GetOpenedSessions() const = 0;
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // IAM_USER_IDM_SESSION_CONTROLLER_H