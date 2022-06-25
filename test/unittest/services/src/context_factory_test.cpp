/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>

#include "context_factory.h"
#include "mock_user_auth_callback.h"
#include "mock_user_idm_callback.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
using namespace std;
using namespace testing;
using namespace testing::ext;
class ContextFactoryTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;
};

void ContextFactoryTest::SetUpTestCase()
{
}

void ContextFactoryTest::TearDownTestCase()
{
}

void ContextFactoryTest::SetUp()
{
}

void ContextFactoryTest::TearDown()
{
}

HWTEST_F(ContextFactoryTest, ContextFactoryCreateSimpleAuth_001, TestSize.Level1)
{
    auto factory = ContextFactory::GetInstance();
    ASSERT_NE(factory, nullptr);
    std::vector<uint8_t> challenge;
    sptr<UserAuthCallback> callback = new (nothrow) MockUserAuthCallback();
    ASSERT_NE(callback, nullptr);
    auto context = factory->CreateSimpleAuthContext(
        0, challenge, static_cast<AuthType>(0), static_cast<AuthTrustLevel>(0), 0, callback);
    ASSERT_NE(context, nullptr);
    EXPECT_NE(context->GetContextId(), 0U);
    ASSERT_EQ(context->GetContextType(), CONTEXT_SIMPLE_AUTH);
}

HWTEST_F(ContextFactoryTest, ContextFactoryCreateSimpleAuth_002, TestSize.Level1)
{
    auto factory = ContextFactory::GetInstance();
    ASSERT_NE(factory, nullptr);
    std::vector<uint8_t> challenge;
    // Error: callback is null
    sptr<UserAuthCallback> callback = nullptr;
    auto context = factory->CreateSimpleAuthContext(
        0, challenge, static_cast<AuthType>(0), static_cast<AuthTrustLevel>(0), 0, callback);
    ASSERT_EQ(context, nullptr);
}

HWTEST_F(ContextFactoryTest, ContextFactoryCreateIdentify_001, TestSize.Level1)
{
    auto factory = ContextFactory::GetInstance();
    ASSERT_NE(factory, nullptr);
    std::vector<uint8_t> challenge;
    sptr<UserAuthCallback> callback = new (nothrow) MockUserAuthCallback();
    ASSERT_NE(callback, nullptr);
    auto context = factory->CreateIdentifyContext(challenge, static_cast<AuthType>(0), 0, callback);
    ASSERT_NE(context, nullptr);
    EXPECT_NE(context->GetContextId(), 0U);
    ASSERT_EQ(context->GetContextType(), CONTEXT_IDENTIFY);
}

HWTEST_F(ContextFactoryTest, ContextFactoryCreateIdentify_002, TestSize.Level1)
{
    auto factory = ContextFactory::GetInstance();
    ASSERT_NE(factory, nullptr);
    std::vector<uint8_t> challenge;
    // Error: callback is null
    sptr<UserAuthCallback> callback = nullptr;
    auto context = factory->CreateIdentifyContext(challenge, static_cast<AuthType>(0), 0, callback);
    ASSERT_EQ(context, nullptr);
}

HWTEST_F(ContextFactoryTest, ContextFactoryCreateEnrollContext_001, TestSize.Level1)
{
    auto factory = ContextFactory::GetInstance();
    ASSERT_NE(factory, nullptr);
    std::vector<uint8_t> token;
    sptr<IdmCallback> callback = new (nothrow) MockIdmCallback();
    ASSERT_NE(callback, nullptr);
    auto context =
        factory->CreateEnrollContext(0, static_cast<AuthType>(0), static_cast<PinSubType>(0), token, 0, callback);
    ASSERT_NE(context, nullptr);
    EXPECT_NE(context->GetContextId(), 0U);
    ASSERT_EQ(context->GetContextType(), CONTEXT_ENROLL);
}

HWTEST_F(ContextFactoryTest, ContextFactoryCreateEnrollContext_002, TestSize.Level1)
{
    auto factory = ContextFactory::GetInstance();
    ASSERT_NE(factory, nullptr);
    std::vector<uint8_t> token;
    // Error: callback is null
    sptr<IdmCallback> callback = nullptr;
    auto context =
        factory->CreateEnrollContext(0, static_cast<AuthType>(0), static_cast<PinSubType>(0), token, 0, callback);
    ASSERT_EQ(context, nullptr);
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS