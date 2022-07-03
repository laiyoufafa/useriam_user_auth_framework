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

#ifndef CONTEXT_CALLBACK_IMPL_H
#define CONTEXT_CALLBACK_IMPL_H

#include "iam_hitrace_helper.h"

#include "context_callback.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class ContextCallbackImpl : public ContextCallback, public NoCopyable {
public:
    explicit ContextCallbackImpl(sptr<IdmCallback> idmCallback, OperationType operationType);
    explicit ContextCallbackImpl(sptr<UserAuthCallback> userAuthCallback, OperationType operationType);
    ~ContextCallbackImpl() override = default;
    void onAcquireInfo(ExecutorRole src, int32_t moduleType, const std::vector<uint8_t> &acquireMsg) const override;
    void OnResult(int32_t resultCode, Attributes &finalResult) override;
    void SetTraceUserId(int32_t userId) override;
    void SetTraceRemainTime(int32_t remainTime) override;
    void SetTraceFreezingTime(int32_t freezingTime) override;
    void SetTraceSdkVersion(int32_t version) override;
    void SetTraceCallingUid(uint64_t callingUid) override;
    void SetTraceAuthType(AuthType authType) override;
    void SetTraceAuthTrustLevel(AuthTrustLevel atl) override;
    void SetCleaner(Context::ContextStopCallback callback) override;

private:
    sptr<IdmCallback> idmCallback_;
    sptr<UserAuthCallback> userAuthCallback_;
    Context::ContextStopCallback stopCallback_ {nullptr};
    ContextCallbackNotifyListener::MetaData metaData_;
    std::shared_ptr<IamHitraceHelper> iamHitraceHelper_;
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // CONTEXT_CALLBACK_IMPL_H