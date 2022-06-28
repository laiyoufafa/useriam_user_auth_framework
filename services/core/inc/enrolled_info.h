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

#ifndef IAM_ENROLLED_INFO_H
#define IAM_ENROLLED_INFO_H

#include <cstdint>
#include <memory>

#include "iam_types.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class EnrolledInfo {
public:
    virtual ~EnrolledInfo() = default;
    virtual AuthType GetAuthType() const = 0;
    virtual int32_t GetUserId() const = 0;
    virtual uint64_t GetEnrolledId() const = 0;
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // IAM_ENROLLED_INFO_H