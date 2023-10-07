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
#include "nfc_database_helper.h"
#include "loghelper.h"
#include "nfc_data_share_impl.h"

namespace OHOS {
namespace NFC {
NfcDatabaseHelper::NfcDatabaseHelper()
{
    fileName_ = "/data/nfc/nfc_preferences.xml";
    errCode_ = 0;
}
NfcDatabaseHelper::~NfcDatabaseHelper()
{
}
NfcDatabaseHelper& NfcDatabaseHelper::GetInstance()
{
    static NfcDatabaseHelper sNfcPrefImpl;
    return sNfcPrefImpl;
}
std::shared_ptr<NativePreferences::Preferences> NfcDatabaseHelper::GetPreference(const std::string& fileName)
{
    DebugLog("Getting preference from distributed data management system");
    return NativePreferences::PreferencesHelper::GetPreferences(fileName, errCode_);
}

void NfcDatabaseHelper::SetString(const std::string& key, const std::string& value)
{
    std::shared_ptr<NativePreferences::Preferences> pref = GetPreference(fileName_);
    if (!pref) {
        ErrorLog("NfcDatabaseHelper: Preference get null");
        return;
    }
    DebugLog("Set preference with key %{public}s, value %{public}s", key.c_str(), value.c_str());
    pref->PutString(key, value);
    pref->Flush();
}

std::string NfcDatabaseHelper::GetString(const std::string& key)
{
    std::shared_ptr<NativePreferences::Preferences> pref = GetPreference(fileName_);
    if (!pref) {
        ErrorLog("NfcDatabaseHelper: Preference get null");
        return "";
    }
    DebugLog("Get preference with key %{public}s", key.c_str());
    return pref->GetString(key, "");
}

void NfcDatabaseHelper::SetInt(const std::string& key, const int value)
{
    std::shared_ptr<NativePreferences::Preferences> pref = GetPreference(fileName_);
    if (!pref) {
        ErrorLog("NfcDatabaseHelper: Preference get null");
        return;
    }
    DebugLog("Set preference with key %{public}s, value %{public}d", key.c_str(), value);
    pref->PutInt(key, value);
    pref->Flush();
}

int NfcDatabaseHelper::GetInt(const std::string& key)
{
    std::shared_ptr<NativePreferences::Preferences> pref = GetPreference(fileName_);
    if (!pref) {
        ErrorLog("NfcDatabaseHelper: Preference get null");
        return 0;
    }
    DebugLog("Get preference with key %{public}s", key.c_str());
    return pref->GetInt(key, 0);
}

void NfcDatabaseHelper::Clear()
{
    std::shared_ptr<NativePreferences::Preferences> pref = GetPreference(fileName_);
    if (!pref) {
        ErrorLog("NfcDatabaseHelper: Preference get null");
        return;
    }
    pref->Clear();
    DebugLog("NfcDatabaseHelper: Clear preferences");
    NativePreferences::PreferencesHelper::DeletePreferences(fileName_);
}

void NfcDatabaseHelper::Delete(const std::string& key)
{
    std::shared_ptr<NativePreferences::Preferences> pref = GetPreference(fileName_);
    if (!pref) {
        ErrorLog("NfcDatabaseHelper: Preference get null");
        return;
    }
    DebugLog("NfcDatabaseHelper: Delete preference with key %{public}s", key.c_str());
    pref->Delete(key);
    pref->FlushSync();
}

void NfcDatabaseHelper::UpdateNfcState(int newState)
{
    SetInt(PREF_KEY_STATE, newState);

    Uri nfcEnableUri(KITS::NFC_DATA_URI);
    DelayedSingleton<NfcDataShareImpl>::GetInstance()->SetValue(nfcEnableUri, KITS::DATA_SHARE_KEY_STATE, newState);
}

int NfcDatabaseHelper::GetNfcState()
{
    return GetInt(PREF_KEY_STATE);
}
} // NFC
} // OHOS