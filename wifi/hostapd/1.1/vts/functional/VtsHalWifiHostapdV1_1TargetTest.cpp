/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android-base/logging.h>
#include <android/hardware/wifi/1.0/IWifi.h>

#include "hostapd_hidl_test_utils.h"
#include "hostapd_hidl_test_utils_1_1.h"

class WifiHostapdHidlEnvironment_1_1 : public WifiHostapdHidlEnvironment {
   public:
    // get the test environment singleton
    static WifiHostapdHidlEnvironment_1_1* Instance() {
        static WifiHostapdHidlEnvironment_1_1* instance =
            new WifiHostapdHidlEnvironment_1_1;
        return instance;
    }

    virtual void registerTestServices() override {
        registerTestService<::android::hardware::wifi::V1_0::IWifi>();
        registerTestService<android::hardware::wifi::hostapd::V1_0::IHostapd>();
        registerTestService<android::hardware::wifi::hostapd::V1_1::IHostapd>();
    }

   private:
    WifiHostapdHidlEnvironment_1_1() {}
};

WifiHostapdHidlEnvironment* gEnv = WifiHostapdHidlEnvironment_1_1::Instance();

int main(int argc, char** argv) {
    ::testing::AddGlobalTestEnvironment(gEnv);
    ::testing::InitGoogleTest(&argc, argv);
    gEnv->init(&argc, argv);
    int status = RUN_ALL_TESTS();
    LOG(INFO) << "Test result = " << status;
    return status;
}
