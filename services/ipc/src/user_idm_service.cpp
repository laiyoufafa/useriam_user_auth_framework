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

#include "user_idm_service.h"

#include "string_ex.h"

#include "accesstoken_kit.h"

#include "context_factory.h"
#include "context_helper.h"
#include "context_pool.h"
#include "hdi_wrapper.h"
#include "iam_logger.h"
#include "iam_para2str.h"
#include "ipc_common.h"
#include "resource_node_pool.h"
#include "resource_node_utils.h"
#include "result_code.h"
#include "user_idm_callback_proxy.h"
#include "user_idm_database.h"
#include "user_idm_session_controller.h"

#define LOG_LABEL UserIAM::Common::LABEL_USER_AUTH_SA
namespace OHOS {
namespace UserIam {
namespace UserAuth {
namespace {
    const std::string MANAGE_USER_IDM_PERMISSION = "ohos.permission.MANAGE_USER_IDM";
    const std::string USE_USER_IDM_PERMISSION = "ohos.permission.USE_USER_IDM";
} // namespace

REGISTER_SYSTEM_ABILITY_BY_ID(UserIdmService, SUBSYS_USERIAM_SYS_ABILITY_USERIDM, true);

UserIdmService::UserIdmService(int32_t systemAbilityId, bool runOnCreate) : SystemAbility(systemAbilityId, runOnCreate)
{
}

void UserIdmService::OnStart()
{
    IAM_LOGI("start service");
    if (!Publish(this)) {
        IAM_LOGE("failed to publish service");
    }
}

void UserIdmService::OnStop()
{
    IAM_LOGI("stop service");
}

int32_t UserIdmService::OpenSession(std::optional<int32_t> userId, std::vector<uint8_t> &challenge)
{
    IAM_LOGI("start");
    if (IpcCommon::GetCallingUserId(*this, userId) != SUCCESS) {
        IAM_LOGE("failed to get userId");
        return INVALID_PARAMETERS;
    }
    if (!IpcCommon::CheckPermission(*this, MANAGE_USER_IDM_PERMISSION)) {
        IAM_LOGE("failed to check permission");
        return CHECK_PERMISSION_FAILED;
    }

    auto contextList = ContextPool::Instance().Select(CONTEXT_ENROLL);
    for (const auto &context : contextList) {
        if (auto ctx = context.lock(); ctx != nullptr) {
            IAM_LOGE("force stop the old context ****%{public}hx", static_cast<uint16_t>(ctx->GetContextId()));
            ctx->Stop();
            ContextPool::Instance().Delete(ctx->GetContextId());
        }
    }

    if (!UserIdmSessionController::Instance().OpenSession(userId.value(), challenge)) {
        IAM_LOGE("failed to open session");
        return FAIL;
    }

    return SUCCESS;
}

void UserIdmService::CloseSession(std::optional<int32_t> userId)
{
    IAM_LOGI("start");
    if (!IpcCommon::CheckPermission(*this, MANAGE_USER_IDM_PERMISSION)) {
        IAM_LOGE("failed to check permission");
        return;
    }
    if (IpcCommon::GetCallingUserId(*this, userId) != SUCCESS) {
        IAM_LOGE("failed to get userId");
        return;
    }

    if (!UserIdmSessionController::Instance().CloseSession(userId.value())) {
        IAM_LOGE("failed to get close session");
    }
}

int32_t UserIdmService::GetCredentialInfo(std::optional<int32_t> userId, AuthType authType,
    const sptr<IdmGetCredentialInfoCallback> &callback)
{
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return INVALID_PARAMETERS;
    }

    if (IpcCommon::GetCallingUserId(*this, userId) != SUCCESS) {
        IAM_LOGE("failed to get userId");
        return INVALID_PARAMETERS;
    }
    if (!IpcCommon::CheckPermission(*this, MANAGE_USER_IDM_PERMISSION)) {
        IAM_LOGE("failed to check permission");
        return CHECK_PERMISSION_FAILED;
    }

    std::optional<PinSubType> pinSubType = std::nullopt;
    auto credInfos = UserIdmDatabase::Instance().GetCredentialInfo(userId.value(), authType);
    if (credInfos.empty()) {
        IAM_LOGI("no cred enrolled");
        callback->OnCredentialInfos(credInfos, pinSubType);
        return SUCCESS;
    }

    auto userInfo = UserIdmDatabase::Instance().GetSecUserInfo(userId.value());
    if (userInfo == nullptr) {
        IAM_LOGE("failed to get userInfo");
        return INVALID_PARAMETERS;
    }

    pinSubType = userInfo->GetPinSubType();
    IAM_LOGE("before OnCredentialInfos");
    callback->OnCredentialInfos(credInfos, pinSubType);

    return SUCCESS;
}

int32_t UserIdmService::GetSecInfo(std::optional<int32_t> userId, const sptr<IdmGetSecureUserInfoCallback> &callback)
{
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return INVALID_PARAMETERS;
    }

    if (IpcCommon::GetCallingUserId(*this, userId) != SUCCESS) {
        IAM_LOGE("failed to get userId");
        return INVALID_PARAMETERS;
    }

    auto userInfos = UserIdmDatabase::Instance().GetSecUserInfo(userId.value());
    if (userInfos == nullptr) {
        IAM_LOGE("current userid %{public}d is not existed", userId.value());
        return INVALID_PARAMETERS;
    }
    callback->OnSecureUserInfo(userInfos);

    return SUCCESS;
}

void UserIdmService::AddCredential(std::optional<int32_t> userId, AuthType authType, PinSubType pinSubType,
    const std::vector<uint8_t> &token, const sptr<IdmCallback> &callback, bool isUpdate)
{
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return;
    }

    Attributes extraInfo;
    auto contextCallback = ContextCallback::NewInstance(callback,
        isUpdate ? TRACE_UPDATE_CREDENTIAL : TRACE_ADD_CREDENTIAL);
    if (contextCallback == nullptr) {
        IAM_LOGE("failed to construct context callback");
        callback->OnResult(GENERAL_ERROR, extraInfo);
        return;
    }
    uint64_t callingUid = static_cast<uint64_t>(this->GetCallingUid());
    contextCallback->SetTraceAuthType(authType);
    contextCallback->SetTraceCallingUid(callingUid);
    if (IpcCommon::GetCallingUserId(*this, userId) != SUCCESS) {
        IAM_LOGE("failed to get userId");
        contextCallback->OnResult(INVALID_PARAMETERS, extraInfo);
        return;
    }
    contextCallback->SetTraceUserId(userId.value());

    if (!IpcCommon::CheckPermission(*this, MANAGE_USER_IDM_PERMISSION)) {
        IAM_LOGE("failed to check permission");
        contextCallback->OnResult(CHECK_PERMISSION_FAILED, extraInfo);
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    CancelCurrentEnrollIfExist();

    auto context =
        ContextFactory::CreateEnrollContext(userId.value(), authType, pinSubType, token, callingUid, contextCallback);
    if (!ContextPool::Instance().Insert(context)) {
        IAM_LOGE("failed to insert context");
        contextCallback->OnResult(FAIL, extraInfo);
        return;
    }

    auto cleaner = ContextHelper::Cleaner(context);
    contextCallback->SetCleaner(cleaner);

    if (!context->Start()) {
        IAM_LOGE("failed to start enroll");
        contextCallback->OnResult(FAIL, extraInfo);
    }
}

void UserIdmService::UpdateCredential(std::optional<int32_t> userId, AuthType authType, PinSubType pinSubType,
    const std::vector<uint8_t> &token, const sptr<IdmCallback> &callback)
{
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return;
    }

    if (token.empty()) {
        IAM_LOGE("token is empty in update");
        return;
    }
    Attributes extraInfo;
    if (IpcCommon::GetCallingUserId(*this, userId) != SUCCESS) {
        IAM_LOGE("failed to get userId");
        callback->OnResult(FAIL, extraInfo);
        return;
    }

    auto credInfos = UserIdmDatabase::Instance().GetCredentialInfo(userId.value(), authType);
    if (credInfos.empty()) {
        IAM_LOGE("current userid %{public}d has no credential for type %{public}u", userId.value(), authType);
        callback->OnResult(FAIL, extraInfo);
        return;
    }

    AddCredential(userId, authType, pinSubType, token, callback, true);
}

int32_t UserIdmService::Cancel(std::optional<int32_t> userId, const std::optional<std::vector<uint8_t>> &challenge)
{
    if (!IpcCommon::CheckPermission(*this, MANAGE_USER_IDM_PERMISSION)) {
        IAM_LOGE("failed to check permission");
        return CHECK_PERMISSION_FAILED;
    }

    bool userIdIsValid = userId.has_value() && UserIdmSessionController::Instance().IsSessionOpened(userId.value());
    bool challengeIsValid = challenge.has_value() &&
        UserIdmSessionController::Instance().IsSessionOpened(challenge.value());
    if (!userIdIsValid && !challengeIsValid) {
        IAM_LOGE("both user id and challenge are invalid");
        return FAIL;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    return CancelCurrentEnroll();
}

void UserIdmService::CancelCurrentEnrollIfExist()
{
    if (ContextPool::Instance().Select(CONTEXT_ENROLL).size() == 0) {
        return;
    }

    IAM_LOGI("cancel current enroll due to new add credential request or delete");
    CancelCurrentEnroll();
}

int32_t UserIdmService::CancelCurrentEnroll()
{
    IAM_LOGD("start");
    auto contextList = ContextPool::Instance().Select(CONTEXT_ENROLL);
    int32_t ret = FAIL;
    for (const auto &context : contextList) {
        if (auto ctx = context.lock(); ctx != nullptr) {
            IAM_LOGE("stop the old context %{public}s", GET_MASKED_STRING(ctx->GetContextId()).c_str());
            ctx->Stop();
            ContextPool::Instance().Delete(ctx->GetContextId());
            ret = SUCCESS;
        }
    }
    IAM_LOGI("result %{public}d", ret);
    return ret;
}

int32_t UserIdmService::EnforceDelUser(int32_t userId, const sptr<IdmCallback> &callback)
{
    IAM_LOGI("to delete userid: %{public}d", userId);
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return INVALID_PARAMETERS;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    CancelCurrentEnrollIfExist();

    Attributes extraInfo;
    auto contextCallback = ContextCallback::NewInstance(callback, TRACE_ENFORCE_DELETE_USER);
    if (contextCallback == nullptr) {
        IAM_LOGE("failed to construct context callback");
        callback->OnResult(GENERAL_ERROR, extraInfo);
        return FAIL;
    }
    contextCallback->SetTraceUserId(userId);

    auto userInfo = UserIdmDatabase::Instance().GetSecUserInfo(userId);
    if (userInfo == nullptr) {
        IAM_LOGE("current userid %{public}d is not existed", userId);
        contextCallback->OnResult(INVALID_PARAMETERS, extraInfo);
        return INVALID_PARAMETERS;
    }

    std::vector<std::shared_ptr<CredentialInfo>> credInfos;
    int32_t ret = UserIdmDatabase::Instance().DeleteUserEnforce(userId, credInfos);
    if (ret != SUCCESS) {
        IAM_LOGE("failed to enforce delete user");
        static_cast<void>(extraInfo.SetUint64Value(Attributes::ATTR_CREDENTIAL_ID, 0));
        contextCallback->OnResult(ret, extraInfo);
        return ret;
    }

    ret = ResourceNodeUtils::NotifyExecutorToDeleteTemplates(credInfos);
    if (ret != SUCCESS) {
        IAM_LOGE("failed to delete executor info, error code : %{public}d", ret);
    }

    IAM_LOGI("delete user success");
    contextCallback->OnResult(SUCCESS, extraInfo);
    return SUCCESS;
}

void UserIdmService::DelUser(std::optional<int32_t> userId, const std::vector<uint8_t> authToken,
    const sptr<IdmCallback> &callback)
{
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    CancelCurrentEnrollIfExist();

    Attributes extraInfo;
    auto contextCallback = ContextCallback::NewInstance(callback, TRACE_DELETE_USER);
    if (contextCallback == nullptr) {
        IAM_LOGE("failed to construct context callback");
        callback->OnResult(GENERAL_ERROR, extraInfo);
        return;
    }
    if (IpcCommon::GetCallingUserId(*this, userId) != SUCCESS) {
        IAM_LOGE("failed to get userId");
        contextCallback->OnResult(INVALID_PARAMETERS, extraInfo);
        return;
    }
    contextCallback->SetTraceUserId(userId.value());

    if (!IpcCommon::CheckPermission(*this, MANAGE_USER_IDM_PERMISSION)) {
        IAM_LOGE("failed to check permission");
        contextCallback->OnResult(CHECK_PERMISSION_FAILED, extraInfo);
        return;
    }

    if (authToken.empty()) {
        IAM_LOGE("token is empty in delete");
        return;
    }

    std::vector<std::shared_ptr<CredentialInfo>> credInfos;
    int32_t ret = UserIdmDatabase::Instance().DeleteUser(userId.value(), authToken, credInfos);
    if (ret != SUCCESS) {
        IAM_LOGE("failed to delete user");
        contextCallback->OnResult(ret, extraInfo);
        return;
    }

    ret = ResourceNodeUtils::NotifyExecutorToDeleteTemplates(credInfos);
    if (ret != SUCCESS) {
        IAM_LOGE("failed to delete executor info, error code : %{public}d", ret);
    }
    IAM_LOGI("delete user end");

    contextCallback->OnResult(ret, extraInfo);
}

void UserIdmService::DelCredential(std::optional<int32_t> userId, uint64_t credentialId,
    const std::vector<uint8_t> &authToken, const sptr<IdmCallback> &callback)
{
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    CancelCurrentEnrollIfExist();

    Attributes extraInfo;
    auto contextCallback = ContextCallback::NewInstance(callback, TRACE_DELETE_CREDENTIAL);
    if (contextCallback == nullptr) {
        IAM_LOGE("failed to construct context callback");
        callback->OnResult(GENERAL_ERROR, extraInfo);
        return;
    }
    if (IpcCommon::GetCallingUserId(*this, userId) != SUCCESS) {
        IAM_LOGE("failed to get userId");
        contextCallback->OnResult(INVALID_PARAMETERS, extraInfo);
        return;
    }
    contextCallback->SetTraceUserId(userId.value());

    if (!IpcCommon::CheckPermission(*this, MANAGE_USER_IDM_PERMISSION)) {
        IAM_LOGE("failed to check permission");
        contextCallback->OnResult(CHECK_PERMISSION_FAILED, extraInfo);
        return;
    }

    std::shared_ptr<CredentialInfo> oldInfo;
    auto ret = UserIdmDatabase::Instance().DeleteCredentialInfo(userId.value(), credentialId, authToken, oldInfo);
    if (ret != SUCCESS) {
        IAM_LOGE("failed to delete CredentialInfo");
        contextCallback->OnResult(ret, extraInfo);
        return;
    }
    if (oldInfo != nullptr) {
        contextCallback->SetTraceAuthType(oldInfo->GetAuthType());
    }

    IAM_LOGI("delete credentialInfo success");
    std::vector<std::shared_ptr<CredentialInfo>> list = {oldInfo};
    ret = ResourceNodeUtils::NotifyExecutorToDeleteTemplates(list);
    if (ret != SUCCESS) {
        IAM_LOGE("failed to delete executor info, error code : %{public}d", ret);
    }

    contextCallback->OnResult(ret, extraInfo);
}

int UserIdmService::Dump(int fd, const std::vector<std::u16string> &args)
{
    IAM_LOGI("start");
    if (fd < 0) {
        IAM_LOGE("invalid parameters");
        dprintf(fd, "Invalid parameters.\n");
        return INVALID_PARAMETERS;
    }
    std::string arg0 = (args.empty() ? "" : Str16ToStr8(args[0]));
    if (arg0.empty() || arg0.compare("-h") == 0) {
        dprintf(fd, "Usage:\n");
        dprintf(fd, "      -h: command help.\n");
        dprintf(fd, "      -l: active user info dump.\n");
        return SUCCESS;
    }
    if (arg0.compare("-l") == 0) {
        std::optional<int32_t> activeUserId;
        if (IpcCommon::GetActiveAccountId(activeUserId) != SUCCESS) {
            dprintf(fd, "Internal error.\n");
            IAM_LOGE("failed to get active id");
            return GENERAL_ERROR;
        }
        dprintf(fd, "Active user is %d\n", activeUserId.value());
        auto userInfo = UserIdmDatabase::Instance().GetSecUserInfo(activeUserId.value());
        if (userInfo != nullptr) {
            auto enrolledInfo = userInfo->GetEnrolledInfo();
            for (auto &info : enrolledInfo) {
                if (info != nullptr) {
                    dprintf(fd, "AuthType %s is enrolled.\n", AuthTypeToStr(info->GetAuthType()));
                }
            }
        }
        return SUCCESS;
    }
    IAM_LOGE("invalid option");
    dprintf(fd, "Invalid option\n");
    return FAIL;
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS