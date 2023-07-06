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

#include "widget_callback_service.h"

#include "iam_logger.h"
#include "iam_ptr.h"

#define LOG_LABEL UserIam::Common::LABEL_USER_AUTH_SDK

namespace OHOS {
namespace UserIam {
namespace UserAuth {
WidgetCallbackService::WidgetCallbackService(const std::shared_ptr<IUserAuthWidgetCallback> &impl)
    : widgetCallback_(impl),
    iamHitraceHelper_(Common::MakeShared<UserIam::UserAuth::IamHitraceHelper>("UserAuthWidget InnerKit"))
{
}

void WidgetCallbackService::SendCommand(const std::string &cmdData)
{
    IAM_LOGI("start, cmdData: %{public}s", cmdData.c_str());
    if (widgetCallback_ == nullptr) {
        IAM_LOGE("both widget callback is nullptr");
        return;
    }
    widgetCallback_->SendCommand(cmdData);
    iamHitraceHelper_= nullptr;
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS