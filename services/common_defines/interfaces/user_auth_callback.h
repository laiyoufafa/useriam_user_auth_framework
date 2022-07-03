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

#ifndef USER_AUTH_CALLBACK_H
#define USER_AUTH_CALLBACK_H

#include <cstdint>
#include <iremote_broker.h>

#include "attributes.h"
#include "iam_types.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class UserAuthCallback : public IRemoteBroker {
public:
    /*
     * returns the acquire info.
     */
    virtual void OnAcquireInfo(int32_t module, uint32_t acquireInfo, int32_t extraInfo) = 0;

    /*
     * returns the auth result.
     */
    virtual void OnAuthResult(int32_t result, const Attributes &extraInfo) = 0;

    /*
     * returns the identify result.
     */
    virtual void OnIdentifyResult(int32_t result, const Attributes &extraInfo) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.UserIAM.UserAuth.UserAuthCallback");
};

class GetExecutorPropertyCallback : public IRemoteBroker {
public:
    /*
     * returns executor property information, such as remaining authentication times and remaining freezing time.
     */
    virtual void OnGetExecutorPropertyResult(int32_t result, const Attributes &attributes) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.UserIAM.UserAuth.GetExecutorPropertyCallback");
};

class SetExecutorPropertyCallback : public IRemoteBroker {
public:
    /*
     * returns a number value indicating whether the property setting was successful.
     */
    virtual void OnSetExecutorPropertyResult(int32_t result) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.UserIAM.UserAuth.SetExecutorPropertyCallback");
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // USER_AUTH_CALLBACK_H