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

#include "nfc_notification_publisher.h"

#include <dlfcn.h>

#include "loghelper.h"
#include "nfc_data_share_impl.h"

namespace OHOS {
namespace NFC {
namespace TAG {
constexpr const char* NFC_NOT_DISTURB_KEYWORD = "settings.nfc.not_disturb";

NfcNotificationPublisher& NfcNotificationPublisher::GetInstance()
{
    static NfcNotificationPublisher instance;
    return instance;
}

NfcNotificationPublisher::NfcNotificationPublisher()
{
    InfoLog("NfcNotificationPublisher constructor enter.");
    if (!isNtfLibLoaded_) {
        InitNfcNtfLib();
    }
}

NfcNotificationPublisher::~NfcNotificationPublisher()
{
    InfoLog("NfcNotificationPublisher destructor enter.");
    UnloadNfcNtfLib();
}

static void NfcNotificationCallback(int notificationId)
{
    NfcNotificationPublisher::GetInstance().OnNotificationButtonClicked(notificationId);
}

void NfcNotificationPublisher::PublishNfcNotification(int notificationId, const std::string &name, int balance)
{
    bool isNfcNotDisturb = IsNfcNtfDisabled();
    if (nfcNtfInf_.publishNotification == nullptr) {
        ErrorLog("func handle nullptr, fail to publish notification");
        return;
    }
    if (notificationId == NFC_NO_HAP_SUPPORTED_NOTIFICATION_ID) {
        usleep(NOTIFICATION_WAIT_TIME_US);
    }
    nfcNtfInf_.publishNotification(isNfcNotDisturb, notificationId, name, balance);
}

void NfcNotificationPublisher::RegNotificationCallback(std::weak_ptr<NfcService> service)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isInitialized_ || nfcService_.expired()) {
        nfcService_ = service;
        isInitialized_ = true;
    }
    if (nfcNtfInf_.regNtfCallback == nullptr) {
        ErrorLog("func handle nullptr, fail to publish notification");
        return;
    }
    nfcNtfInf_.regNtfCallback(NfcNotificationCallback);
}

void NfcNotificationPublisher::UnloadNfcNtfLib()
{
    if (nfcNtfHandle_ != nullptr) {
        dlclose(nfcNtfHandle_);
        nfcNtfHandle_ = nullptr;
    }

    isNtfLibLoaded_ = false;
}

void NfcNotificationPublisher::InitNfcNtfLib()
{
    if (isNtfLibLoaded_) {
        InfoLog("nfc notification lib already loaded.");
        return;
    }
    nfcNtfHandle_ = dlopen(NFC_NTF_LIB_PATH, RTLD_LAZY | RTLD_GLOBAL);
    if (nfcNtfHandle_ == nullptr) {
        ErrorLog("fail to dlopen nfc notification lib.");
        return;
    }
    nfcNtfInf_.regNtfCallback = reinterpret_cast<void (*)(NfcNtfCallback *)>
        (dlsym(nfcNtfHandle_, REG_NFC_CALLBACK_FUNC_NAME));
    nfcNtfInf_.publishNotification = reinterpret_cast<void (*)(bool, int, const std::string &, int)>
        (dlsym(nfcNtfHandle_, PUBLISH_NTF_FUNC_NAME));
    if (nfcNtfInf_.regNtfCallback == nullptr || nfcNtfInf_.publishNotification == nullptr) {
        ErrorLog("fail to dlsym nfc notification lib.");
        UnloadNfcNtfLib();
        return;
    }
    isNtfLibLoaded_ = true;
}

bool NfcNotificationPublisher::IsNfcNtfDisabled()
{
    const std::string NFC_NOT_DISTURB_SUFFIX =
        "/com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=settings.nfc.not_disturb";
    const std::string NFC_NOT_DISTURB_PREFIX = "datashare://";
    const std::string NFC_DATA_URI_NOT_DISTURB = NFC_NOT_DISTURB_PREFIX + NFC_NOT_DISTURB_SUFFIX;
    Uri nfcNotDisturb(NFC_DATA_URI_NOT_DISTURB);

    auto dataShare = NfcDataShareImpl::GetInstance();
    if (dataShare == nullptr) {
        ErrorLog("fail to get datashare.");
        return false; // NFC not disturb switch is off by default.
    }
    int32_t value = INVALID_VALUE;
    int32_t nfcNotDisturbOff = 0;
    int32_t nfcNotDisturbOn = 1;
    KITS::ErrorCode errCode = dataShare->GetValue(nfcNotDisturb, NFC_NOT_DISTURB_KEYWORD, value);
    if (errCode == KITS::ERR_NFC_DATABASE_NULL) {
        ErrorLog("fail to get datashare proxy.");
        return false; // NFC not disturb switch is off by default.
    }
    if (value == INVALID_VALUE) {
        WarnLog("NFC not disturb switch is off by default.");
        dataShare->SetValue(nfcNotDisturb, NFC_NOT_DISTURB_KEYWORD, nfcNotDisturbOff);
        dataShare->GetValue(nfcNotDisturb, NFC_NOT_DISTURB_KEYWORD, value);
    }

    // value = 1 : button on(not disturb, no banner), value = 0 : button off(banner on).
    InfoLog("NFC notification not disturb button value %{public}d", value);
    return (value == nfcNotDisturbOn);
}

void NfcNotificationPublisher::OnNotificationButtonClicked(int notificationId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (nfcService_.expired()) {
        ErrorLog("nfc service expired, fail to callback.");
        return;
    }
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher = nfcService_.lock()->GetTagDispatcher();
    if (tagDispatcher.expired()) {
        ErrorLog("tagDispatcher expired, fail to inform button clicking");
        return;
    }
    tagDispatcher.lock()->OnNotificationButtonClicked(notificationId);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS