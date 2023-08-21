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
#include "vendor_ext_service.h"
#include "loghelper.h"
#include <dlfcn.h>
#include <string>

namespace OHOS {
namespace NFC {
namespace NCI {

VendorExtService::VendorExtService() {}
VendorExtService::~VendorExtService() {}

static void* g_pVendorExtLibHandle = nullptr;
static VendorExtService::GET_CHIP_TYPE g_pGetChipFuncHandle = nullptr;

bool VendorExtService::OnStartExtService(void)
{
    if (g_pVendorExtLibHandle) {
        return true;
    }
    const char* pChLibName = "libvendor_ext_nfc_service.z.so";
    g_pVendorExtLibHandle = dlopen(pChLibName, RTLD_LAZY | RTLD_LOCAL);
    if (!g_pVendorExtLibHandle) {
        ErrorLog("%{public}s: cannot open library %{public}s, %{public}s", __func__, pChLibName, dlerror());
        return false;
    }
    const char* symbol = "GetChipType";
    g_pGetChipFuncHandle = (GET_CHIP_TYPE)dlsym(g_pVendorExtLibHandle, symbol);
    if (!g_pGetChipFuncHandle) {
        ErrorLog("%{public}s: cannot find symbol %{public}s, %{public}s", __func__, symbol, dlerror());
    }
    return true;
}

std::string VendorExtService::GetNfcChipType(void)
{
    if (!g_pGetChipFuncHandle) {
        ErrorLog("%{public}s: cannt find symbol GetNfcChipType.", __func__);
        return std::string();
    }
    static std::string chipType = g_pGetChipFuncHandle();
    return chipType;
}

void VendorExtService::OnStopExtService(void)
{
    g_pGetChipFuncHandle = nullptr;
    if (g_pVendorExtLibHandle) {
        dlclose(g_pVendorExtLibHandle);
        g_pVendorExtLibHandle = nullptr;
    }
}

}
}
}