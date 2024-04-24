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

namespace OHOS {
namespace NFC {
namespace TAG {
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
    if (nfcNtfInf_.publishNotification == nullptr) {
        ErrorLog("func handle nullptr, fail to publish notification");
        return;
    }
    if (notificationId == NFC_NO_HAP_SUPPORTED_NOTIFICATION_ID) {
        usleep(NOTIFICATION_WAIT_TIME_US);
    }
    nfcNtfInf_.publishNotification(notificationId, name, balance);
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
    nfcNtfInf_.publishNotification = reinterpret_cast<void (*)(int, const std::string &, int)>
        (dlsym(nfcNtfHandle_, PUBLISH_NTF_FUNC_NAME));
    if (nfcNtfInf_.regNtfCallback == nullptr || nfcNtfInf_.publishNotification == nullptr) {
        ErrorLog("fail to dlsym nfc notification lib.");
        UnloadNfcNtfLib();
        return;
    }
    isNtfLibLoaded_ = true;
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