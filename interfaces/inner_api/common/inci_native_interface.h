/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef I_NCI_NATIVE_INTERFACE_H
#define I_NCI_NATIVE_INTERFACE_H

#include <memory>
#include "inci_ce_interface.h"
#include "inci_nfcc_interface.h"
#include "inci_tag_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class INciNativeInterface {
public:
    virtual ~INciNativeInterface() = default;

    /**
     * @brief Get the ptr of INciCeInterface
     * @return thr ptr of INciCeInterface
     */
    virtual std::shared_ptr<INciCeInterface> GetNciCeInterface() = 0;

    /**
     * @brief Get the ptr of INciNfccInterface
     * @return thr ptr of INciNfccInterface
     */
    virtual std::shared_ptr<INciNfccInterface> GetNciNfccInterface() = 0;

    /**
     * @brief Get the ptr of INciTagInterface
     * @return thr ptr of INciTagInterface
     */
    virtual std::shared_ptr<INciTagInterface> GetNciTagInterface() = 0;
};

#define DECLARE_NATIVE_INTERFACE(interfaceClass)                         \
    extern "C" INciNativeInterface *NewInterface(void)                   \
    {                                                                    \
        return static_cast<INciNativeInterface *>(new interfaceClass()); \
    }                                                                    \
    extern "C" void DeleteInterface(INciNativeInterface *p)              \
    {                                                                    \
        delete p;                                                        \
    }
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS

#endif