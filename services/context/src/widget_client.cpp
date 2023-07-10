/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "widget_client.h"

#include "system_ability_definition.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "nlohmann/json.hpp"
#include "widget_callback_interface.h"

#define LOG_LABEL UserIam::Common::LABEL_USER_AUTH_SA

namespace OHOS {
namespace UserIam {
namespace UserAuth {
WidgetClient &WidgetClient::Instance()
{
    static WidgetClient widgetClient;
    return widgetClient;
}

void WidgetClient::SetWidgetSchedule(const std::shared_ptr<WidgetScheduleNode> &schedule)
{
    IF_FALSE_LOGE_AND_RETURN(schedule != nullptr);
    schedule_ = schedule;
}

ResultCode WidgetClient::OnNotice(NoticeType type, const std::string &eventData)
{
    // handle notice from widget
    if (eventData.empty()) {
        IAM_LOGE("OnNotice eventData is empty");
        return ResultCode::INVALID_PARAMETERS;
    }
    IAM_LOGI("recv notice eventData: %{public}s", eventData.c_str());
    auto root = nlohmann::json::parse(eventData.c_str());
    if (root == nullptr) {
        IAM_LOGE("OnNotice eventData is not json format");
        return ResultCode::INVALID_PARAMETERS;
    }
    WidgetNotice notice = root.get<WidgetNotice>();
    std::vector<AuthType> authTypeList = notice.AuthTypeList();
    if (notice.event == "EVENT_AUTH_TYPE_READY") {
        schedule_->StartAuthList(authTypeList);
    } else if (notice.event == "EVENT_AUTH_USER_CANCEL") {
        if (authTypeList.size() == 1 && authTypeList[0] == AuthType::ALL) {
            schedule_->StopSchedule();
        } else {
            schedule_->StopAuthList(authTypeList);
        }
    } else if (notice.event == "EVENT_AUTH_USER_NAVIGATION") {
        schedule_->NaviPinAuth();
    }
    return ResultCode::SUCCESS;
}

void WidgetClient::SendCommand(const WidgetCommand &command)
{
    if (widgetCallback_ == nullptr) {
        IAM_LOGE("SendCommand widget callback is null");
        return;
    }
    nlohmann::json root = command;
    std::string cmdData = root.dump();
    IAM_LOGI("SendCommand cmdData: %{public}s", cmdData.c_str());
    widgetCallback_->SendCommand(cmdData);
}

void WidgetClient::ReportWidgetResult(int32_t result, AuthType authType,
    int32_t lockoutDuration, int32_t remainAttempts)
{
    // sendCommand of CMD_NOTIFY_AUTH_RESULT
    WidgetCommand::Cmd cmd {
        .event = "CMD_NOTIFY_AUTH_RESULT",
        .version = version_,
        .result = result,
        .type = AuthType2Str(authType),
        .lockoutDuration = lockoutDuration,
        .remainAttempts = remainAttempts
    };
    if (authType == AuthType::FINGERPRINT && !sensorInfo_.empty()) {
        cmd.sensorInfo = sensorInfo_;
    }
    WidgetCommand widgetCmd {
        .widgetContextId = widgetContextId_,
        .typeList = { AuthType2Str(authType) },
        .title = widgetParam_.title,
        .windowModeType = WinModeType2Str(widgetParam_.windowMode),
        .navigationButtonText = widgetParam_.navigationButtonText,
        .cmdList = { cmd }
    };
    if (!pinSubType_.empty()) {
        widgetCmd.pinSubType = pinSubType_;
    }
    SendCommand(widgetCmd);
}

void WidgetClient::SetWidgetContextId(uint64_t contextId)
{
    widgetContextId_ = contextId;
}

void WidgetClient::SetWidgetParam(const WidgetParam &param)
{
    widgetParam_ = param;
}

const std::string& WidgetClient::GetVersion() const
{
    return version_;
}

void WidgetClient::SetWidgetCallback(const sptr<WidgetCallbackInterface> &callback)
{
    widgetCallback_ = callback;
}

void WidgetClient::SetUserAndTokenId(uint32_t userId, uint32_t tokenId)
{
    userId_ = userId;
    tokenId_ = tokenId;
    IAM_LOGI("WidgetClient SetUserAndTokenId userId: %{public}d, tokenId_: %{public}d", userId_, tokenId_);
}

uint32_t WidgetClient::GetUserId() const
{
    return userId_;
}

uint32_t WidgetClient::GetTokenId() const
{
    return tokenId_;
}

void WidgetClient::Reset()
{
    IAM_LOGI("WidgetClient Reset tokenId_: %{public}d", tokenId_);
    widgetParam_.title.clear();
    widgetParam_.navigationButtonText.clear();
    widgetParam_.windowMode = WindowModeType::DIALOG_BOX; // default
    widgetContextId_ = 0;
    tokenId_ = 0;
    userId_ = 0;
    schedule_ = nullptr;
    widgetCallback_ = nullptr;
    pinSubType_.clear();
    sensorInfo_.clear();
}

void WidgetClient::SetPinSubType(const PinSubType &subType)
{
    if (subType == PinSubType::PIN_SIX) {
        pinSubType_ = "PIN_SIX";
    } else if (subType == PinSubType::PIN_NUMBER) {
        pinSubType_ = "PIN_NUMBER";
    } else if (subType == PinSubType::PIN_MIXED) {
        pinSubType_ = "PIN_MIXED";
    } else if (subType == PinSubType::PIN_MAX) {
        pinSubType_ = "PIN_MAX";
    }
}

void WidgetClient::SetSensorInfo(const std::string &info)
{
    sensorInfo_ = info;
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS