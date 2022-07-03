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
#include "authentication_impl.h"

#include "hdi_wrapper.h"
#include "iam_logger.h"
#include "iam_hitrace_helper.h"
#include "schedule_node_helper.h"

#define LOG_LABEL UserIAM::Common::LABEL_USER_AUTH_SA
namespace OHOS {
namespace UserIam {
namespace UserAuth {
AuthenticationImpl::AuthenticationImpl(uint64_t contextId, int32_t userId, AuthType authType, AuthTrustLevel atl)
    : contextId_(contextId),
      userId_(userId),
      authType_(authType),
      atl_(atl)
{
}

AuthenticationImpl::~AuthenticationImpl()
{
    Cancel();
}

void AuthenticationImpl::SetExecutor(uint32_t executorIndex)
{
    executorIndex_ = executorIndex;
}

void AuthenticationImpl::SetChallenge(const std::vector<uint8_t> &challenge)
{
    challenge_ = challenge;
}

void AuthenticationImpl::SetCallingUid(uint32_t uid)
{
    uid_ = uid;
}

bool AuthenticationImpl::Start(std::vector<std::shared_ptr<ScheduleNode>> &scheduleList,
    std::shared_ptr<ScheduleNodeCallback> callback)
{
    using HdiAuthSolution = OHOS::HDI::UserAuth::V1_0::AuthSolution;
    using HdiAuthType = OHOS::HDI::UserAuth::V1_0::AuthType;
    using HdiScheduleInfo = OHOS::HDI::UserAuth::V1_0::ScheduleInfo;
    auto hdi = HdiWrapper::GetHdiInstance();
    if (!hdi) {
        IAM_LOGE("bad hdi");
        return false;
    }
    HdiAuthSolution solution = {
        .userId = userId_,
        .authTrustLevel = atl_,
        .authType = static_cast<HdiAuthType>(authType_),
        .executorSensorHint = executorSensorHint,
        .challenge = challenge_,
    };
    std::vector<HdiScheduleInfo> infos;
    IamHitraceHelper traceHelper("hdi BeginAuthentication");
    auto result = hdi->BeginAuthentication(contextId_, solution, infos);
    if (result != HDF_SUCCESS) {
        IAM_LOGE("hdi BeginAuthentication failed, err is %{public}d", result);
        return false;
    }
    if (infos.empty()) {
        IAM_LOGE("hdi BeginAuthentication failed, infos is empty");
        return false;
    }

    ScheduleNodeHelper::NodeOptionalPara para;
    para.uid = uid_;

    if (!ScheduleNodeHelper::BuildFromHdi(infos, callback, scheduleList, para)) {
        IAM_LOGE("BuildFromHdi failed");
        return false;
    }

    running_ = true;
    return true;
}

bool AuthenticationImpl::Update(const std::vector<uint8_t> &scheduleResult, AuthResultInfo &resultInfo)
{
    auto hdi = HdiWrapper::GetHdiInstance();
    if (!hdi) {
        IAM_LOGE("bad hdi");
        return false;
    }

    using HdiAuthResultInfo = OHOS::HDI::UserAuth::V1_0::AuthResultInfo;
    HdiAuthResultInfo info;
    auto result = hdi->UpdateAuthenticationResult(contextId_, scheduleResult, info);
    if (result != HDF_SUCCESS) {
        IAM_LOGE("hdi UpdateAuthenticationResult failed, err is %{public}d", result);
    }

    resultInfo.result = info.result;
    resultInfo.freezingTime = info.freezingTime;
    resultInfo.remainTimes = info.remainTimes;
    resultInfo.token = info.token;

    return result == HDF_SUCCESS && resultInfo.result == SUCCESS;
}

bool AuthenticationImpl::Cancel()
{
    if (!running_) {
        return false;
    }
    running_ = false;

    auto hdi = HdiWrapper::GetHdiInstance();
    if (!hdi) {
        IAM_LOGE("bad hdi");
        return false;
    }

    auto result = hdi->CancelAuthentication(contextId_);
    if (result != HDF_SUCCESS) {
        IAM_LOGE("hdi CancelAuthentication failed, err is %{public}d", result);
        return false;
    }
    return true;
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS