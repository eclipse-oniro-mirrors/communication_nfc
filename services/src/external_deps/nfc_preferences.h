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
#ifndef NFC_PREFERENCES_H
#define NFC_PREFERENCES_H
#include "preferences.h"
#include "preferences_helper.h"

namespace OHOS {
namespace NFC {
constexpr const char* ABORT_RETRY_TIME = "abort_retry_time";

class NfcPreferences {
public:
    static NfcPreferences& GetInstance();

    void SetString(const std::string& key, const std::string& value);
    std::string GetString(const std::string& key);
    void SetInt(const std::string& key, const int value);
    int GetInt(const std::string& key);
    void SetBool(const std::string& key, const bool value);
    bool GetBool(const std::string& key);
    void Clear();
    void Delete(const std::string& key);

private:
    NfcPreferences();
    ~NfcPreferences();

    std::shared_ptr<NativePreferences::Preferences> GetPreference(const std::string& fileName);
    int errCode_;
    std::string fileName_;
};
} // NFC
} // OHOS
#endif // NFC_PREFERENCES_H