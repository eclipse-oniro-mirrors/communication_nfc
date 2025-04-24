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
#ifndef NFC_CARDEMULATION_CONTROLLER_H
#define NFC_CARDEMULATION_CONTROLLER_H

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

namespace OHOS {
namespace NFC {
namespace KITS {

class CjNfcCardEmulationController {
public:
    CjNfcCardEmulationController() = default;
    ~CjNfcCardEmulationController() = default;
    static std::shared_ptr<CjNfcCardEmulationController> GetInstance();
    int32_t Subscribe(int8_t type, int64_t id);
    int32_t UnSubscribe(int8_t type);
    void HceCmd(const std::vector<uint8_t> &data);

private:
    void RegisterListener(int8_t type, int64_t id);
    void UnRegisterListener(int8_t type);
    void InitHceCmd(int64_t id);
    std::recursive_mutex mutex_;
    std::function<void(const uint8_t *head, int64_t size)> hceCmd_;
    static std::mutex controllerMutex_;
    static std::shared_ptr<CjNfcCardEmulationController> controller_;
};

} // namespace KITS
} // namespace NFC
} // namespace OHOS

#endif