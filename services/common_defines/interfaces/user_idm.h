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

#ifndef USER_IDM_H
#define USER_IDM_H

#include <cstdint>
#include <optional>

#include "refbase.h"
#include "user_idm_callback.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class UserIdm : public IRemoteBroker {
public:
    enum : uint32_t {
        USER_IDM_OPEN_SESSION = 0,
        USER_IDM_OPEN_SESSION_BY_ID,
        USER_IDM_CLOSE_SESSION,
        USER_IDM_CLOSE_SESSION_BY_ID,
        USER_IDM_GET_AUTH_INFO,
        USER_IDM_GET_AUTH_INFO_BY_ID,
        USER_IDM_GET_SEC_INFO,
        USER_IDM_ADD_CREDENTIAL,
        USER_IDM_ADD_CREDENTIAL_BY_ID,
        USER_IDM_UPDATE_CREDENTIAL,
        USER_IDM_UPDATE_CREDENTIAL_BY_ID,
        USER_IDM_CANCEL,
        USER_IDM_CANCEL_BY_ID,
        USER_IDM_ENFORCE_DEL_USER,
        USER_IDM_DEL_USER,
        USER_IDM_DEL_USER_BY_ID,
        USER_IDM_DEL_CRED,
        USER_IDM_DEL_CREDENTIAL,
    };

    /*
     * start an IDM operation to obtain challenge value, a challenge value of 0 indicates that open session failed.
     *
     * param userId user id.
     * return challenge value.
     */
    virtual int32_t OpenSession(std::optional<int32_t> userId, std::vector<uint8_t> &challenge) = 0;

    /*
     * end an IDM operation.
     *
     * param userId user id.
     */
    virtual void CloseSession(std::optional<int32_t> userId) = 0;

    /*
     * get authentication information.
     *
     * param userId current user id.
     * param authType credential type.
     * param callback returns all registered credential information of this type for the specific user.
     */
    virtual int32_t GetCredentialInfo(std::optional<int32_t> userId, AuthType authType,
        const sptr<IdmGetCredentialInfoCallback> &callback) = 0;

    /*
     * get user security ID.
     *
     * param userId current user id.
     * param callback returns all registered security information for the specific user.
     */
    virtual int32_t GetSecInfo(std::optional<int32_t> userId, const sptr<IdmGetSecureUserInfoCallback> &callback) = 0;

    /**
     * add user credential information, pass in credential addition method and credential information
     * (credential type, subtype, if adding user's non password credentials, pass in password authentication token),
     * and get the result / acquire info callback.
     *
     * param userId user id.
     * param credInfo Incoming credential addition method and credential information
     * (credential type, subtype, password authentication token).
     * param callback get results / acquire info callback.
     */
    virtual void AddCredential(std::optional<int32_t> userId, AuthType authType, PinSubType pinSubType,
        const std::vector<uint8_t> &token, const sptr<IdmCallback> &callback, bool isUpdate) = 0;
    /*
     * update user credential information.
     *
     * param userId user id.
     * param credInfo Incoming credential addition method and credential information
     * (credential type, subtype, password authentication token).
     * param callback update results / acquire info callback.
     */
    virtual void UpdateCredential(std::optional<int32_t> userId, AuthType authType, PinSubType pinSubType,
        const std::vector<uint8_t> &token, const sptr<IdmCallback> &callback) = 0;

    /*
     * Cancel entry and pass in user id.
     *
     * param userId user id.
     */
    virtual int32_t Cancel(std::optional<int32_t> userId, const std::optional<std::vector<uint8_t>> &challenge) = 0;

    /*
     * enforce delete the user credential information, pass in the callback,
     * and obtain the deletion result through the callback.
     *
     * param authToken user password authentication token.
     * param callback get deletion result through callback.
     */
    virtual int32_t EnforceDelUser(int32_t userId, const sptr<IdmCallback> &callback) = 0;

    /*
     * delete all users credential information, pass in the user password authentication token and callback,
     * and obtain the deletion result through the callback.
     *
     * param userId user id.
     * param authToken user password authentication token.
     * param callback get deletion result through callback.
     */
    virtual void DelUser(std::optional<int32_t> userId, const std::vector<uint8_t> authToken,
        const sptr<IdmCallback> &callback) = 0;

    /*
     * delete the user credential information, pass in the credential id, password authentication token and callback,
     * and obtain the deletion result through the callback.
     * Only deleting non password credentials is supported.
     *
     * param userId user id.
     * param credentialId credential index.
     * param authToken password authentication token.
     * param callback get deletion result through callback.
     */
    virtual void DelCredential(std::optional<int32_t> userId, uint64_t credentialId,
        const std::vector<uint8_t> &authToken, const sptr<IdmCallback> &callback) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.useridm.IUserIDM");
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // USER_IDM_H