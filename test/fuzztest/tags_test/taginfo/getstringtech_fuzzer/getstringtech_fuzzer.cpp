/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "getstringtech_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "taginfo.h"
#include "tag_session_proxy.h"
#include <securec.h>

namespace OHOS {
    using namespace OHOS::NFC::KITS;

    const uint8_t *g_baseFuzzData_ = nullptr;
    size_t g_baseFuzzSize_ = 0;
    size_t g_baseFuzzPos_;

    template <class T> T GetData()
    {
        T object{};
        size_t objectSize = sizeof(object);
        if (g_baseFuzzData_ == nullptr || objectSize > g_baseFuzzSize_ - g_baseFuzzPos_) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_baseFuzzData_ + g_baseFuzzPos_, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_baseFuzzPos_ += objectSize;
        return object;
    }

    void FuzzGetStringTech(const uint8_t* data, size_t size)
    {
        g_baseFuzzData_ = data;
        g_baseFuzzSize_ = size;
        g_baseFuzzPos_ = 0;

        int tech = GetData<int>();
        NFC::KITS::TagInfo::GetStringTech(tech);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FuzzGetStringTech(data, size);
    return 0;
}

