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

#include "nfc_cardEmulation_controller.h"
#include "nfc_cardEmulation_ffi.h"
#include "cj_lambda.h"

namespace OHOS {
namespace NFC {
namespace KITS {
std::mutex CjNfcCardEmulationController::controllerMutex_;
std::shared_ptr<CjNfcCardEmulationController> CjNfcCardEmulationController::controller_{nullptr};
const int8_t HCE_CMD = 0;  // defined by cangjie enum NfcEventType

std::shared_ptr<CjNfcCardEmulationController> CjNfcCardEmulationController::GetInstance()
{
    if (controller_ == nullptr) {
        std::lock_guard<std::mutex> lock(controllerMutex_);
        if (controller_ == nullptr) {
            auto controller = std::make_shared<CjNfcCardEmulationController>();
            controller_ = controller;
        }
    }
    return controller_;
}

int32_t CjNfcCardEmulationController::Subscribe(int8_t type, int64_t id)
{
    RegisterListener(type, id);
    return SUCCESS_CODE;
}

int32_t CjNfcCardEmulationController::UnSubscribe(int8_t type)
{
    UnRegisterListener(type);
    return SUCCESS_CODE;
}

void CjNfcCardEmulationController::RegisterListener(int8_t type, int64_t id)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    switch (type) {
        case HCE_CMD:
            InitHceCmd(id);
            break;
        default:
            return;
    }
}

void CjNfcCardEmulationController::UnRegisterListener(int8_t type)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    switch (type) {
        case HCE_CMD:
            hceCmd_ = nullptr;
            break;
        default:
            return;
    }
}

void CjNfcCardEmulationController::InitHceCmd(int64_t id)
{
    auto callback = reinterpret_cast<void (*)(const uint8_t *, int64_t)>(id);
    hceCmd_ = [lambda = CJLambda::Create(callback)](const uint8_t *head, int64_t size) -> void { lambda(head, size); };
}

void CjNfcCardEmulationController::HceCmd(const std::vector<uint8_t> &data)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (hceCmd_ == nullptr) {
        return;
    }
    hceCmd_(data.data(), data.size());
    return;
}

}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS