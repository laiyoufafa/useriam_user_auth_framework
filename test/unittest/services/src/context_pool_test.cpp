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

#include "context_pool_test.h"

#include "context_helper.h"

#include "mock_context.h"
#include "mock_context_pool_listener.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
using namespace testing;
using namespace testing::ext;

void ContextPoolTest::SetUpTestCase()
{
}

void ContextPoolTest::TearDownTestCase()
{
}

void ContextPoolTest::SetUp()
{
}

void ContextPoolTest::TearDown()
{
}

HWTEST_F(ContextPoolTest, ContextPoolGetInstance, TestSize.Level1)
{
    auto &pool = ContextPool::Instance();
    EXPECT_NE(&pool, nullptr);
}

HWTEST_F(ContextPoolTest, ContextPoolGetUniqueInstance, TestSize.Level1)
{
    auto &pool1 = ContextPool::Instance();
    auto &pool2 = ContextPool::Instance();
    ASSERT_NE(&pool1, nullptr);
    ASSERT_NE(&pool2, nullptr);
    EXPECT_EQ(&pool1, &pool2);
}

HWTEST_F(ContextPoolTest, ContextPoolInsertNull, TestSize.Level1)
{
    auto &pool = ContextPool::Instance();
    ;
    EXPECT_EQ(pool.Insert(nullptr), false);
}

HWTEST_F(ContextPoolTest, ContextPoolInsertDuplicateId, TestSize.Level1)
{
    const uint64_t CONTEXT_ID = 100;
    auto context1 = MockContext::CreateWithContextId(CONTEXT_ID);
    auto context2 = MockContext::CreateWithContextId(CONTEXT_ID);
    ASSERT_NE(context1, context2);
    auto &pool = ContextPool::Instance();

    EXPECT_EQ(pool.Insert(context1), true);
    EXPECT_EQ(pool.Select(CONTEXT_ID).lock(), context1);
    EXPECT_EQ(pool.Insert(context2), false);
    EXPECT_EQ(pool.Select(CONTEXT_ID).lock(), context1);

    EXPECT_EQ(pool.Delete(CONTEXT_ID), true);
}

HWTEST_F(ContextPoolTest, ContextPoolDelete, TestSize.Level1)
{
    const uint64_t CONTEXT_ID = 200;
    auto &pool = ContextPool::Instance();
    EXPECT_EQ(pool.Delete(CONTEXT_ID), false);
    auto context = MockContext::CreateWithContextId(CONTEXT_ID);
    EXPECT_EQ(pool.Insert(context), true);
    EXPECT_EQ(pool.Delete(CONTEXT_ID), true);
    EXPECT_EQ(pool.Select(CONTEXT_ID).lock(), nullptr);
}

HWTEST_F(ContextPoolTest, ContextPoolInsertAndDelete, TestSize.Level1)
{
    auto &pool = ContextPool::Instance();
    const uint64_t CONTEXT_ID1 = 300;
    const uint64_t CONTEXT_ID2 = 400;
    const uint64_t CONTEXT_ID3 = 500;
    auto context1 = MockContext::CreateWithContextId(CONTEXT_ID1);
    auto context2 = MockContext::CreateWithContextId(CONTEXT_ID2);
    auto context3 = MockContext::CreateWithContextId(CONTEXT_ID3);
    EXPECT_EQ(pool.Insert(context1), true);
    EXPECT_EQ(pool.Insert(context2), true);
    EXPECT_EQ(pool.Insert(context3), true);
    EXPECT_EQ(pool.Select(CONTEXT_ID3).lock(), context3);
    EXPECT_EQ(pool.Select(CONTEXT_ID2).lock(), context2);
    EXPECT_EQ(pool.Select(CONTEXT_ID1).lock(), context1);

    EXPECT_EQ(pool.Delete(CONTEXT_ID1), true);
    EXPECT_EQ(pool.Delete(CONTEXT_ID2), true);
    EXPECT_EQ(pool.Delete(CONTEXT_ID3), true);
}

HWTEST_F(ContextPoolTest, ContextSelectScheduleNodeByScheduleId, TestSize.Level1)
{
    const uint64_t CONTEXT_ID1 = 100;
    const uint64_t SCHEDULE_ID1 = 102;
    const uint64_t CONTEXT_ID2 = 200;
    const uint64_t SCHEDULE_ID2 = 202;
    auto &pool = ContextPool::Instance();
    auto context1 = MockContext::CreateContextWithScheduleNode(CONTEXT_ID1, {SCHEDULE_ID1});
    EXPECT_EQ(pool.Insert(context1), true);
    auto context2 = MockContext::CreateContextWithScheduleNode(CONTEXT_ID2, {SCHEDULE_ID2});
    EXPECT_EQ(pool.Insert(context2), true);

    EXPECT_NE(pool.SelectScheduleNodeByScheduleId(SCHEDULE_ID1), nullptr);
    EXPECT_EQ(pool.Delete(CONTEXT_ID1), true);
    EXPECT_EQ(pool.SelectScheduleNodeByScheduleId(SCHEDULE_ID1), nullptr);

    EXPECT_NE(pool.SelectScheduleNodeByScheduleId(SCHEDULE_ID2), nullptr);
    EXPECT_EQ(pool.Delete(CONTEXT_ID2), true);
    EXPECT_EQ(pool.SelectScheduleNodeByScheduleId(SCHEDULE_ID2), nullptr);
}

HWTEST_F(ContextPoolTest, ContextPoolListenerInsert, TestSize.Level1)
{
    auto &pool = ContextPool::Instance();
    const uint64_t CONTEXT_ID1 = 300;
    const uint64_t CONTEXT_ID2 = 400;
    const uint64_t CONTEXT_ID3 = 500;
    auto context1 = MockContext::CreateWithContextId(CONTEXT_ID1);
    auto context2 = MockContext::CreateWithContextId(CONTEXT_ID2);
    auto context3 = MockContext::CreateWithContextId(CONTEXT_ID3);

    MockFunction<void(MockContextPoolListener::Action action, const std::shared_ptr<Context> &context)> callback1;
    MockFunction<void(MockContextPoolListener::Action action, const std::shared_ptr<Context> &context)> callback2;
    MockFunction<void(MockContextPoolListener::Action action, const std::shared_ptr<Context> &context)> callback3;

    auto listener1 = MockContextPoolListener::Create(
        [&callback1](MockContextPoolListener::Action action, const std::shared_ptr<Context> &context) {
            callback1.Call(action, context);
        });
    EXPECT_EQ(pool.RegisterContextPoolListener(listener1), true);

    auto listener2 = MockContextPoolListener::Create(
        [&callback2](MockContextPoolListener::Action action, const std::shared_ptr<Context> &context) {
            callback2.Call(action, context);
        });
    EXPECT_EQ(pool.RegisterContextPoolListener(listener2), true);

    auto listener3 = MockContextPoolListener::Create(
        [&callback3](MockContextPoolListener::Action action, const std::shared_ptr<Context> &context) {
            callback3.Call(action, context);
        });
    EXPECT_EQ(pool.RegisterContextPoolListener(listener3), true);

    EXPECT_CALL(callback1, Call(MockContextPoolListener::INSERT, _)).Times(1);
    EXPECT_CALL(callback2, Call(MockContextPoolListener::INSERT, _)).Times(2);
    EXPECT_CALL(callback3, Call(MockContextPoolListener::INSERT, _)).Times(3);

    EXPECT_CALL(callback1, Call(MockContextPoolListener::DELETE, _)).Times(1);
    EXPECT_CALL(callback2, Call(MockContextPoolListener::DELETE, _)).Times(2);
    EXPECT_CALL(callback3, Call(MockContextPoolListener::DELETE, _)).Times(3);

    EXPECT_EQ(pool.Insert(context1), true);
    EXPECT_EQ(pool.Delete(CONTEXT_ID1), true);
    EXPECT_EQ(pool.DeregisterContextPoolListener(listener1), true);
    EXPECT_EQ(pool.Insert(context2), true);
    EXPECT_EQ(pool.Delete(CONTEXT_ID2), true);
    EXPECT_EQ(pool.DeregisterContextPoolListener(listener2), true);
    EXPECT_EQ(pool.Insert(context3), true);
    EXPECT_EQ(pool.Delete(CONTEXT_ID3), true);
    EXPECT_EQ(pool.DeregisterContextPoolListener(listener3), true);
}

HWTEST_F(ContextPoolTest, ContextPoolCleaner, TestSize.Level1)
{
    const uint64_t CONTEXT_ID1 = 100;
    const uint64_t SCHEDULE_ID1 = 102;
    auto &pool = ContextPool::Instance();

    auto context = MockContext::CreateContextWithScheduleNode(CONTEXT_ID1, {SCHEDULE_ID1});
    EXPECT_EQ(pool.Insert(context), true);
    EXPECT_NE(pool.Select(CONTEXT_ID1).lock(), nullptr);

    ContextHelper::Cleaner clear(context);
    clear();
    EXPECT_EQ(pool.Select(CONTEXT_ID1).lock(), nullptr);
}

HWTEST_F(ContextPoolTest, ContextPoolRandomId, TestSize.Level1)
{
    std::set<uint64_t> generated;

    for (int i = 0; i < 1000; i++) {
        auto contextId = ContextPool::GetNewContextId();
        EXPECT_NE(contextId, 0U);
        EXPECT_TRUE(generated.find(contextId) == generated.end());
        generated.emplace(contextId);
    }
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
