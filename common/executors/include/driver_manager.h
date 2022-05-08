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

#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H

#include <cstdint>
#include <map>
#include "driver.h"
#include "iauth_driver_hdi.h"
#include "idriver_manager.h"
#include "iservstat_listener_hdi.h"
#include "singleton.h"
#include "system_ability_status_listener.h"

namespace OHOS {
namespace UserIAM {
namespace UserAuth {
using namespace HDI::ServiceManager::V1_0;

class DriverManager : public ServStatListenerStub, public Singleton<DriverManager> {
public:
    friend SystemAbilityStatusListener;

    void OnReceive(const ServiceStatus &status) override;
    int32_t Start(const std::map<std::string, HdiConfig> &hdiName2Config);
    void OnFrameworkReady();

private:
    bool HdiConfigIsValid(const std::map<std::string, HdiConfig> &hdiName2Config);
    bool HdiDriverIsRunning(const std::string &serviceName);
    void SubscribeHdiDriverStatus();
    void SubscribeHdiManagerServiceStatus();
    void SubscribeFrameworkRedayEvent();
    void OnAllHdiDisconnect();
    void OnAllHdiConnect();

    std::recursive_mutex mutex_;
    std::map<std::string, std::shared_ptr<Driver>> serviceName2Driver_;
    std::map<std::string, HdiConfig> hdiName2Config_;
};
} // namespace UserAuth
} // namespace UserIAM
} // namespace OHOS

#endif // DRIVER_MANAGER_H