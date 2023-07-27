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

static void* g_pLibHandle = nullptr;
static VendorExtService::GET_CHIP_TYPE pFuncHandle = nullptr;

bool VendorExtService::OnStartExtService(void)
{
    if (g_pLibHandle) return true;
    const char* pChLibName = "/vendor/lib64/libvendor_ext_nfc_service.z.so";
    g_pLibHandle = dlopen(pChLibName, RTLD_LAZY | RTLD_LOCAL);
    if (!g_pLibHandle) {
        ErrorLog("%{public}s: cannot open library %{public}s, %{public}s", __func__, pChLibName, dlerror());
        return false;
    }
    const char* symbol = "GetChipType";
    pFuncHandle = (GET_CHIP_TYPE)dlsym(g_pLibHandle, symbol);
    if (!pFuncHandle) {
        ErrorLog("%{public}s: cannot find symbol %{public}s, %{public}s", __func__, symbol, dlerror());
        OnStopExtService();
        return false;
    }
    return true;
}

std::string VendorExtService::GetNfcChipType(void)
{
    static std::string chipType = pFuncHandle();
    return chipType;
}

void VendorExtService::OnStopExtService(void)
{
    pFuncHandle = nullptr;
    if (g_pLibHandle) {
        dlclose(g_pLibHandle);
        g_pLibHandle = nullptr;
    }
}

}
}
}