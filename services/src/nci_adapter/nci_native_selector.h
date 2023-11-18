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

#ifndef NCI_NATIVE_SELECTOR_H
#define NCI_NATIVE_SELECTOR_H

#include <memory>
#include <string>
#include "inci_native_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NciNativeSelector {
public:
    static NciNativeSelector &GetInstance();

    /**
     * @brief Get the ptr of INciCeInterface
     * @return thr ptr of INciCeInterface
     */
    std::shared_ptr<INciCeInterface> GetNciCeInterface();

    /**
     * @brief Get the ptr of INciNfccInterface
     * @return thr ptr of INciNfccInterface
     */
    std::shared_ptr<INciNfccInterface> GetNciNfccInterface();

    /**
     * @brief Get the ptr of INciTagInterface
     * @return thr ptr of INciTagInterface
     */
    std::shared_ptr<INciTagInterface> GetNciTagInterface();

private:
    class NciLibsLoader {
    public:
        explicit NciLibsLoader(const std::string &newInterfaceSymbol = "NewInterface",
            const std::string &deleteInterfaceSymbol = "DeleteInterface");

        ~NciLibsLoader();

        NciLibsLoader(const NciLibsLoader &) = delete;
        NciLibsLoader &operator=(NciLibsLoader &) = delete;

        /**
         * @brief dlopen native so
         * @param True if success, otherwise false.
         */
        bool LoadLib();

        /**
         * @brief dlclose native so
         * @param True if success, otherwise false.
         */
        bool CloseLib();

        /**
         * @brief dlsym symbol
         * @param The ptr of INciNativeInterface
         */
        std::shared_ptr<INciNativeInterface> NewInstance();

    private:
        void *handle_{nullptr};
        std::string libPath_;
        std::string newInterfaceSymbol_;
        std::string deleteInterfaceSymbol_;
    };

    NciNativeSelector();
    NciNativeSelector(const NciNativeSelector &) = delete;
    NciNativeSelector &operator=(NciNativeSelector &) = delete;

    /**
     * @brief Init natvie interface
     */
    void InitNativeInterface();

    /**
     * @brief Get the ptr of INciNativeInterface
     * @return thr ptr of INciNativeInterface
     */
    std::shared_ptr<INciNativeInterface> GetNciNativeInterface();

    std::shared_ptr<INciNativeInterface> nativeInterface_;
    static inline std::unique_ptr<NciLibsLoader> loader_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS

#endif