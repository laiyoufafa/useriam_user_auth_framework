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

/**
 * @file iam_executor_iauth_executor_hdi.h
 *
 * @brief Hardware device interface for authenticate executor.
 * @since 3.1
 * @version 3.2
 */

#ifndef IAM_EXECUTOR_IAUTH_EXECUTOR_HDI_H
#define IAM_EXECUTOR_IAUTH_EXECUTOR_HDI_H

#include <cstdint>
#include <vector>

#include "co_auth_client.h"
#include "iam_executor_framework_types.h"
#include "iam_common_defines.h"
#include "iam_executor_iexecute_callback.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class IAuthExecutorHdi {
public:
    /**
     * @brief Default constructor.
     */
    IAuthExecutorHdi() = default;

    /**
     * @brief Deconstructor.
     */
    virtual ~IAuthExecutorHdi() = default;

    /**
     * @brief Get executor infomation.
     *
     * @param info The executor infomation.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode GetExecutorInfo(ExecutorInfo &info) = 0;

    /**
     * @brief Get template infomation.
     *
     * @param templateId Template id.
     * @param info The template infomation.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode GetTemplateInfo(uint64_t templateId, UserAuth::TemplateInfo &info) = 0;

    /**
     * @brief Register is finish.
     *
     * @param templateIdList Template ID list.
     * @param frameworkPublicKey Framework publickey
     * @param extraInfo Extra infomation.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo) = 0;

    /**
     * @brief Begin enroll.
     *
     * @param scheduleId Current enroll schedule ID.
     * @param tokenId Current enroll token ID.
     * @param extraInfo Extra information of enroll.
     * @param callbackObj Callback of enroll result.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode Enroll(uint64_t scheduleId, uint32_t tokenId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) = 0;

    /**
     * @brief Begin authenticate.
     *
     * @param scheduleId Current authenticate schedule ID.
     * @param tokenId Current authenticate token ID.
     * @param templateIdList Template ID list.
     * @param extraInfo Extra information of authenticate.
     * @param callbackObj Callback of authenticate result.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode Authenticate(uint64_t scheduleId, uint32_t tokenId, const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) = 0;

    /**
     * @brief Begin identify.
     *
     * @param scheduleId Current identify schedule ID.
     * @param tokenId Current identify token ID.
     * @param extraInfo Extra information of identify.
     * @param callbackObj Callback of identify result.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode Identify(uint64_t scheduleId, uint32_t tokenId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) = 0;

    /**
     * @brief Delete.
     *
     * @param templateIdList Template ID list.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode Delete(const std::vector<uint64_t> &templateIdList) = 0;

    /**
     * @brief Cancel the action of executor.
     *
     * @param scheduleId Current working schedule ID.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode Cancel(uint64_t scheduleId) = 0;

    /**
     * @brief Send command.
     *
     * @param commandId Command ID.
     * @param extraInfo Extra information of send command.
     * @param callbackObj Callback of send command result.
     * @return Return the result success or error code{@link ResultCode}.
     */
    virtual ResultCode SendCommand(PropertyMode commandId, const std::vector<uint8_t> &extraInfo,
        const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj) = 0;
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS

#endif // IAM_EXECUTOR_IAUTH_EXECUTOR_HDI_H