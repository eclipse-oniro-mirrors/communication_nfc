/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "tagSession.ani.hpp"
#include "nfctech.ani.hpp"
#include "ohos.nfc.tag.tag.ndef.ani.hpp"
#include "ohos.nfc.tag.tag.ani.hpp"
#if __has_include(<ani.h>)
#include <ani.h>
#elif __has_include(<ani/ani.h>)
#include <ani/ani.h>
#else
#error "ani.h not found. Please ensure the Ani SDK is correctly installed."
#endif

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        return ANI_ERROR;
    }
    ani_status status = ANI_OK;
    if (ANI_OK != tagSession::ANIRegister(env)) {
        std::cerr << "Error from tagSession::ANIRegister" << std::endl;
        status = ANI_ERROR;
    }
    if (ANI_OK != nfctech::ANIRegister(env)) {
        std::cerr << "Error from nfctech::ANIRegister" << std::endl;
        status = ANI_ERROR;
    }
    if (ANI_OK != ohos::nfc::tag::tag::ndef::ANIRegister(env)) {
        std::cerr << "Error from ohos::nfc::tag::tag::ndef::ANIRegister" << std::endl;
        status = ANI_ERROR;
    }
    if (ANI_OK != ohos::nfc::tag::tag::ANIRegister(env)) {
        std::cerr << "Error from ohos::nfc::tag::tag::ANIRegister" << std::endl;
        status = ANI_ERROR;
    }
    *result = ANI_VERSION_1;
    return status;
}
