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
#ifndef NFC_CONTROLLER_H
#define NFC_CONTROLLER_H

#include "ndef_msg_callback_stub.h"
#include "nfc_controller_callback_stub.h"
#include "nfc_controller_proxy.h"
#include "nfc_sdk_common.h"
#include "infc_controller_callback.h"
#include "infc_controller_service.h"
#ifdef VENDOR_APPLICATIONS_ENABLED
#include "iquery_app_info_callback.h"
#endif

namespace OHOS {
namespace NFC {
namespace KITS {
static const std::string NFC_SERVICE_NAME = "nfc";

class NfcController final {
public:
    explicit NfcController();
    ~NfcController();

    /**
     * @Description Get an object of nfc controller.
     * @param void
     * @return an object of nfc controller
     */
    static NfcController &GetInstance();
    /**
     * @Description Turn on Nfc of the device.
     * @param void
     * @return Errorcode of turn on nfc. if return 0, means successful.
     */
    int TurnOn();
    /**
     * @Description Turn off Nfc of the device.
     * @param void
     * @return Errorcode of turn off nfc. if return 0, means successful.
     */
    int TurnOff();
    /**
     * @Description Get nfc state of device.
     * @param void
     * @return nfc state.
     */
    int GetNfcState();
    /**
     * @Checks whether a device supports NFC.
     * @param void
     * @return If the device supports NFC return 1; otherwise return 0.
     */
    bool IsNfcAvailable();
    /**
     * @Description Checks whether NFC is enabled.
     * @param isOpen The output for checking nfc is open or not.
     * @return The status code of calling function.
     */
    int IsNfcOpen(bool &isOpen);
    /**
     * @Description Registers the callback for nfc state changed notification.
     * @param callback the callback to be registered.
     * @param type the type for this callback, it's "nfcStateChange"
     * @return The status code for register operation.
     */
    ErrorCode RegListener(const sptr<INfcControllerCallback> &callback, const std::string& type);
    /**
     * @Description Unregisters the callback for nfc state changed notification.
     * @param type the type for this callback, it's "nfcStateChange"
     * @return The status code for unregister operation.
     */
    ErrorCode UnregListener(const std::string& type);

    /**
     * @brief Get the Tag Service Iface object
     *
     * @return OHOS::sptr<IRemoteObject> the remote object of tag service.
     */
    OHOS::sptr<IRemoteObject> GetTagServiceIface();

    OHOS::sptr<IRemoteObject> GetHceServiceIface();

    void OnRemoteDied(const wptr<IRemoteObject> &remoteObject);

    ErrorCode RegNdefMsgCb(const sptr<INdefMsgCallback> &callback);

#ifdef VENDOR_APPLICATIONS_ENABLED
    ErrorCode RegQueryApplicationCb(const std::string& type,
        QueryApplicationByVendor tagCallback, QueryHceAppByVendor hceCallback);

    ErrorCode RegCardEmulationNotifyCb(OnCardEmulationNotifyCb callback);
    ErrorCode NotifyEventStatus(int eventType, int arg1 = 0, std::string arg2 = "");
#endif

private:
    class NfcServiceDeathRecipient : public IRemoteObject::DeathRecipient {
        public:
            explicit NfcServiceDeathRecipient(NfcController &client) : client_(client) {}
            ~NfcServiceDeathRecipient() override = default;
            void OnRemoteDied(const wptr<IRemoteObject> &remoteObject) override
            {
                client_.OnRemoteDied(remoteObject);
            }
        private:
            NfcController &client_;
    };

private:
    static void InitNfcRemoteSA();

private:
    static bool initialized_;
    static std::shared_ptr<NfcControllerProxy> nfcControllerProxy_;
    static std::weak_ptr<OHOS::NFC::INfcControllerService> nfcControllerService_;
    static std::mutex mutex_;
    static bool remoteDied_;
    static sptr<IRemoteObject> remote_;
    static sptr<IRemoteObject::DeathRecipient> deathRecipient_;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif // NFC_CONTROLLER_H
