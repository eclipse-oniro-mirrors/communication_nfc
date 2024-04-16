/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef NFC_BT_CONN_MGR_H
#define NFC_BT_CONN_MGR_H

#include <shared_mutex>
#include "bluetooth_a2dp_src.h"
#include "bluetooth_hfp_ag.h"
#include "bluetooth_hid_host.h"
#include "bluetooth_host.h"
#include "bluetooth_remote_device.h"
#include "ndef_bt_data_parser.h"
#include "nfc_service.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class BtConnectionInfo {
public:
    std::string macAddr_;
    int32_t state_;
    uint8_t type_;
};

class BtConnectionManager {
public:
    static BtConnectionManager& GetInstance();
    void Initialize(std::weak_ptr<NfcService> nfcService);

    void TryPairBt(std::shared_ptr<BtData> data);
    void OnBtNtfClicked();
    void HandleBtEnableFailed();
    void HandleBtPairFailed();
    void HandleBtConnectFailed();
    void OnBtEnabled();
    void OnPairStatusChanged(std::shared_ptr<BtConnectionInfo> info);
    void OnConnectionStateChanged(std::shared_ptr<BtConnectionInfo> info);

    enum BtProfileType {
        A2DP_SRC,
        HFP_AG,
        HID_HOST
    };

    // Bt status observers
    class BtStateObserver : public Bluetooth::BluetoothHostObserver {
    public:
        BtStateObserver() = default;
        virtual ~BtStateObserver() = default;
        static std::shared_ptr<BtStateObserver> &GetInstance()
        {
            static std::shared_ptr<BtStateObserver> instance = std::make_shared<BtStateObserver>();
            return instance;
        }
        void OnStateChanged(const int transport, const int status) override;
        void OnDiscoveryStateChanged(int status) override {};
        void OnDiscoveryResult(const Bluetooth::BluetoothRemoteDevice &device,
                               int rssi, const std::string deviceName, int deviceClass) override {};
        void OnPairRequested(const Bluetooth::BluetoothRemoteDevice &device) override {};
        void OnPairConfirmed(const Bluetooth::BluetoothRemoteDevice &device, int reqType, int number) override {};
        void OnScanModeChanged(int mode) override {};
        void OnDeviceNameChanged(const std::string &deviceName) override {};
        void OnDeviceAddrChanged(const std::string &address) override {};
    };

    class BtRemoteDevObserver : public Bluetooth::BluetoothRemoteDeviceObserver {
    public:
        BtRemoteDevObserver() = default;
        virtual ~BtRemoteDevObserver() = default;
        void OnAclStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
                               int state, unsigned int reason) override {};
        void OnPairStatusChanged(const Bluetooth::BluetoothRemoteDevice &device, int status, int cause) override;
        void OnRemoteUuidChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                 const std::vector<Bluetooth::ParcelUuid> &uuids) override {};
        void OnRemoteNameChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                 const std::string &deviceName) override {};
        void OnRemoteAliasChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                  const std::string &alias) override {};
        void OnRemoteCodChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                const Bluetooth::BluetoothDeviceClass &cod) override {};
        void OnRemoteBatteryLevelChanged(const Bluetooth::BluetoothRemoteDevice &device, int batteryLevel) override {};
        void OnReadRemoteRssiEvent(const Bluetooth::BluetoothRemoteDevice &device, int rssi, int status) override {};
    };

    class BtA2dpObserver : public Bluetooth::A2dpSourceObserver {
    public:
        BtA2dpObserver() = default;
        virtual ~BtA2dpObserver() = default;
        void OnPlayingStatusChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                    int playingState, int error) override {};
        void OnConfigurationChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                    const Bluetooth::A2dpCodecInfo &info, int error) override {};
        void OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int state, int cause) override;
        void OnMediaStackChanged(const Bluetooth::BluetoothRemoteDevice &device, int action) override {};
    };

    class BtHfpObserver : public Bluetooth::HandsFreeAudioGatewayObserver {
    public:
        BtHfpObserver() = default;
        virtual ~BtHfpObserver() = default;
        void OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
            int32_t state, int32_t cause) override;
        void OnScoStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
                               int32_t state, int32_t reason) override {};
        void OnActiveDeviceChanged(const Bluetooth::BluetoothRemoteDevice &device) override {};
        void OnHfEnhancedDriverSafetyChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                             int32_t indValue) override {};
        void OnHfpStackChanged(const Bluetooth::BluetoothRemoteDevice &device, int32_t action) override {};
    };

    class BtHidObserver : public Bluetooth::HidHostObserver {
    public:
        BtHidObserver() = default;
        virtual ~BtHidObserver() = default;
        void OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device, int state, int cause) override;
    };

private:
    BtConnectionManager();
    ~BtConnectionManager() {}
    // Bt observers
    void RegisterBtObserver();
    void UnregisterBtObserverAndProfile();
    void RegisterProfileObserver(BtProfileType type);
    // timeout event messages
    void SendMsgToEvtHandler(NfcCommonEvent evt, int64_t delay);
    void SendConnMsgToEvtHandler(NfcCommonEvent evt, const Bluetooth::BluetoothRemoteDevice &device,
                                 int32_t state, BtProfileType type);
    void RemoveMsgFromEvtHandler(NfcCommonEvent evt);
    // Bt state check
    bool IsBtEnabled();
    bool HandleEnableBt();
    // status and action jump
    void NextAction();
    void NextActionInit();
    void NextActionConnect();
    void NextActionDisconnect();

    bool GetProfileList();
    bool IsA2dpSupported();
    bool IsHfpSupported();
    bool HandleBtInit();
    bool DecideInitNextAction();

    bool HandleBtPair();
    bool HandleBtConnect();
    bool HandleBtConnectWaiting();
    bool HandleBtDisconnect();
    bool HandleBtDisconnectWaiting();

    // clear function
    void OnFinish();

private:
    std::weak_ptr<NfcService> nfcService_ {};
    std::shared_mutex mutex_ {};
    bool isInitialized_ = false;

    std::shared_ptr<BtStateObserver> btObserver_ {};
    std::shared_ptr<BtRemoteDevObserver> btRemoteDevObserver_ {};
    std::shared_ptr<BtA2dpObserver> btA2dpObserver_ {};
    std::shared_ptr<BtHfpObserver> btHfpObserver_ {};
    std::shared_ptr<BtHidObserver> btHidObserver_ {};
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif // NFC_BT_CONN_MGR_H