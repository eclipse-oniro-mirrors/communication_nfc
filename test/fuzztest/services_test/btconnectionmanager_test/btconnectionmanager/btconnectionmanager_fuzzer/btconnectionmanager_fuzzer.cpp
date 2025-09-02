/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "btconnectionmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "bt_connection_manager.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzTryPairBt(const uint8_t* data, size_t size)
    {
        std::shared_ptr<BtData> btData = std::make_shared<BtData>();
        btData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        btData->name_ = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        BtConnectionManager::GetInstance().Initialize(service);
        BtConnectionManager::GetInstance().TryPairBt(btData);
    }

    void FuzzOnBtNtfClicked(const uint8_t* data, size_t size)
    {
        std::shared_ptr<BtData> btData = std::make_shared<BtData>();
        btData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        btData->name_ = NfcSdkCommon::BytesVecToHexString(data, size);
        btData->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
        btData->macAddress_ = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        BtConnectionManager::GetInstance().Initialize(service);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        BtConnectionManager::GetInstance().OnBtNtfClicked();
    }

    void FuzzHandleBtEnableFailed(const uint8_t* data, size_t size)
    {
        std::shared_ptr<BtData> btData = std::make_shared<BtData>();
        btData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        btData->name_ = NfcSdkCommon::BytesVecToHexString(data, size);
        btData->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
        btData->macAddress_ = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        BtConnectionManager::GetInstance().Initialize(service);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        BtConnectionManager::GetInstance().HandleBtEnableFailed();
    }

    void FuzzHandleBtPairFailed(const uint8_t* data, size_t size)
    {
        std::shared_ptr<BtData> btData = std::make_shared<BtData>();
        btData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        btData->name_ = NfcSdkCommon::BytesVecToHexString(data, size);
        btData->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
        btData->macAddress_ = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        BtConnectionManager::GetInstance().Initialize(service);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        BtConnectionManager::GetInstance().HandleBtPairFailed();
    }

    void FuzzHandleBtConnectFailed(const uint8_t* data, size_t size)
    {
        std::shared_ptr<BtData> btData = std::make_shared<BtData>();
        btData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        btData->name_ = NfcSdkCommon::BytesVecToHexString(data, size);
        btData->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
        btData->macAddress_ = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        BtConnectionManager::GetInstance().Initialize(service);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        BtConnectionManager::GetInstance().HandleBtConnectFailed();
    }

    void FuzzOnBtEnabled(const uint8_t* data, size_t size)
    {
        std::shared_ptr<BtData> btData = std::make_shared<BtData>();
        btData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        btData->name_ = NfcSdkCommon::BytesVecToHexString(data, size);
        btData->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
        btData->macAddress_ = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        BtConnectionManager::GetInstance().Initialize(service);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        BtConnectionManager::GetInstance().OnBtEnabled();
    }

    void FuzzOnPairStatusChanged(const uint8_t* data, size_t size)
    {
        std::shared_ptr<BtData> btData = std::make_shared<BtData>();
        btData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        btData->name_ = NfcSdkCommon::BytesVecToHexString(data, size);
        btData->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
        btData->macAddress_ = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
        info->macAddr_ = NfcSdkCommon::BytesVecToHexString(data, size);
        info->state_ = Bluetooth::PAIR_NONE;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        BtConnectionManager::GetInstance().Initialize(service);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        BtConnectionManager::GetInstance().OnPairStatusChanged(info);
    }

    void FuzzOnConnectionStateChanged(const uint8_t* data, size_t size)
    {
        std::shared_ptr<BtData> btData = std::make_shared<BtData>();
        btData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        btData->name_ = NfcSdkCommon::BytesVecToHexString(data, size);
        btData->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
        btData->macAddress_ = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
        info->macAddr_ = NfcSdkCommon::BytesVecToHexString(data, size);
        info->state_ = static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED);
        info->type_ = static_cast<uint8_t>(BtConnectionManager::BtProfileType::HFP_AG);
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        BtConnectionManager::GetInstance().Initialize(service);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    }

}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzTryPairBt(data, size);
    OHOS::FuzzOnBtNtfClicked(data, size);
    OHOS::FuzzHandleBtEnableFailed(data, size);
    OHOS::FuzzHandleBtPairFailed(data, size);
    OHOS::FuzzHandleBtConnectFailed(data, size);
    OHOS::FuzzOnBtEnabled(data, size);
    OHOS::FuzzOnPairStatusChanged(data, size);
    OHOS::FuzzOnConnectionStateChanged(data, size);
    return 0;
}

