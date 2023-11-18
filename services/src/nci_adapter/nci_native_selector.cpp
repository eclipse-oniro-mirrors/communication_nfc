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

#include "nci_native_selector.h"
#include <dlfcn.h>
#include <string>
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciNativeSelector::NciNativeSelector()
{
    InitNativeInterface();
}

void NciNativeSelector::InitNativeInterface()
{
    nativeInterface_ = GetNciNativeInterface();
}

std::shared_ptr<INciNativeInterface> NciNativeSelector::GetNciNativeInterface()
{
    loader_ = std::make_unique<NciLibsLoader>();
    if (!loader_->LoadLib()) {
        return nullptr;
    }
    return loader_->NewInstance();
}

NciNativeSelector &NciNativeSelector::GetInstance()
{
    static NciNativeSelector instance;
    return instance;
}

std::shared_ptr<INciCeInterface> NciNativeSelector::GetNciCeInterface()
{
    if (nativeInterface_) {
        return nativeInterface_->GetNciCeInterface();
    }
    return nullptr;
}

std::shared_ptr<INciNfccInterface> NciNativeSelector::GetNciNfccInterface()
{
    if (nativeInterface_) {
        return nativeInterface_->GetNciNfccInterface();
    }
    return nullptr;
}

std::shared_ptr<INciTagInterface> NciNativeSelector::GetNciTagInterface()
{
    if (nativeInterface_) {
        return nativeInterface_->GetNciTagInterface();
    }
    return nullptr;
}

NciNativeSelector::NciLibsLoader::NciLibsLoader(const std::string &newInterfaceSymbol,
    const std::string &deleteInterfaceSymbol): newInterfaceSymbol_(newInterfaceSymbol),
    deleteInterfaceSymbol_(deleteInterfaceSymbol)
{
#ifdef USE_VENDOR_NCI_NATIVE
    libPath_ = "libnci_native_vendor.z.so";
#else
    libPath_ = "libnci_native_default.z.so";
#endif
}

NciNativeSelector::NciLibsLoader::~NciLibsLoader()
{
    (void)CloseLib();
}

bool NciNativeSelector::NciLibsLoader::LoadLib()
{
    if (libPath_.empty() || handle_) {
        return false;
    }
    handle_ = dlopen(libPath_.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle_) {
        ErrorLog("load %{public}s fail, %{public}s", libPath_.c_str(), dlerror());
        return false;
    }
    InfoLog("load %{public}s success", libPath_.c_str());
    return true;
}

bool NciNativeSelector::NciLibsLoader::CloseLib()
{
    if (handle_) {
        if (dlclose(handle_) != 0) {
            handle_ = nullptr;
            ErrorLog("close %{public}s fail, %{public}s", libPath_.c_str(), dlerror());
            return false;
        }
        handle_ = nullptr;
    }
    InfoLog("close %{public}s success", libPath_.c_str());
    return true;
}

std::shared_ptr<INciNativeInterface> NciNativeSelector::NciLibsLoader::NewInstance()
{
    if (!handle_) {
        ErrorLog("fail handle is null");
        return nullptr;
    }
    using NewFuncType = INciNativeInterface *(*)(void);
    using DeleteFuncType = void (*)(INciNativeInterface *);

    auto newInterface = reinterpret_cast<NewFuncType>(dlsym(handle_, newInterfaceSymbol_.c_str()));
    auto deleteInterface = reinterpret_cast<DeleteFuncType>(dlsym(handle_, deleteInterfaceSymbol_.c_str()));
    if (!newInterface || !deleteInterface) {
        (void)CloseLib();
        ErrorLog("fail not found sym  %{public}s", libPath_.c_str());
        return nullptr;
    }
    InfoLog("new instance %{public}s success", libPath_.c_str());
    return std::shared_ptr<INciNativeInterface>(newInterface(), deleteInterface);
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS