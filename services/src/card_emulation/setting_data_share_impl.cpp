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
#include "loghelper.h"
#include "iremote_broker.h"
#include "iremote_stub.h"
#include "setting_data_share_impl.h"
namespace OHOS {
namespace NFC {
OHOS::NFC::SettingDataShareImpl::SettingDataShareImpl()
{
    Initialize();
}
SettingDataShareImpl::~SettingDataShareImpl()
{
    remoteObj_ = nullptr;
}
KITS::ErrorCode SettingDataShareImpl::RegisterDataObserver(const Uri& uri,
                                                           const sptr<AAFwk::IDataAbilityObserver>& dataObserver)
{
    if (dataShareHelper_ == nullptr) {
        ErrorLog("%{public}s: dataShareHelper_ is nullptr.", __func__);
        Initialize();
        if (dataShareHelper_ == nullptr) {
            ErrorLog("%{public}s: dataShareHelper_ is nullptr. Retry failed.", __func__);
            return KITS::ERR_NFC_DATABASE_RW;
        }
    }
    dataShareHelper_->RegisterObserver(uri, dataObserver);
    return KITS::ERR_NONE;
}
KITS::ErrorCode SettingDataShareImpl::ReleaseDataObserver(const Uri& uri,
                                                          const sptr<AAFwk::IDataAbilityObserver>& dataObserver)
{
    if (dataShareHelper_ == nullptr) {
        ErrorLog("%{public}s: dataShareHelper_ is nullptr.", __func__);
        Initialize();
        if (dataShareHelper_ == nullptr) {
            ErrorLog("%{public}s: dataShareHelper_ is nullptr. Retry failed.", __func__);
            return KITS::ERR_NFC_DATABASE_RW;
        }
    }
    dataShareHelper_->UnregisterObserver(uri, dataObserver);
    return KITS::ERR_NONE;
}
KITS::ErrorCode SettingDataShareImpl::GetElementName(Uri& uri, const std::string& column, ElementName& value)
{
    if (dataShareHelper_ == nullptr) {
        ErrorLog("%{public}s: dataShareHelper_ is nullptr.", __func__);
        Initialize();
        if (dataShareHelper_ == nullptr) {
            ErrorLog("%{public}s: dataShareHelper_ is nullptr. Retry failed.", __func__);
            return KITS::ERR_NFC_DATABASE_RW;
        }
    }
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> columns;
    predicates.EqualTo(KITS::NFC_DATA_COLUMN_KEYWORD, column);
    auto rows = dataShareHelper_->Query(uri, predicates, columns);
    if (rows == nullptr) {
        ErrorLog("%{public}s: can't get rows.", __func__);
        return KITS::ERR_NFC_DATABASE_RW;
    }
    rows->GoToFirstRow();
    int32_t columnIndex;
    rows->GetColumnIndex(KITS::NFC_DATA_COLUMN_VALUE, columnIndex);
    std::string valueStr;
    int32_t ret = rows->GetString(columnIndex, valueStr);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("%{public}s: can't get value.", __func__);
        return KITS::ERR_NFC_DATABASE_RW;
    }
    rows->Close();
    ParseElementURI(valueStr, value);
    InfoLog("%{public}s: success, value = %{public}s, element = %{public}s.", __func__, valueStr.c_str(),
            value.GetURI().c_str());
    return KITS::ERR_NONE;
}

bool SettingDataShareImpl::ParseElementURI(const std::string& uri, ElementName& value)
{
    const size_t memberNum = 2;
    if (std::count(uri.begin(), uri.end(), '/') != memberNum - 1) {
        ErrorLog("Invalid uri: %{public}s.", uri.c_str());
        return false;
    }

    std::vector<std::string> uriVec;
    Split(uri, "/", uriVec);
    uriVec.resize(memberNum);

    int index = 0;
    value.SetBundleName(uriVec[index++]);
    value.SetAbilityName(uriVec[index++]);
    return true;
}

void SettingDataShareImpl::Split(const std::string& str, const std::string& delim, std::vector<std::string>& vec)
{
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = str.find(delim);
    while (std::string::npos != pos2) {
        vec.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + delim.size();
        pos2 = str.find(delim, pos1);
    }
    if (pos1 != str.size()) {
        vec.push_back(str.substr(pos1));
    }
}
KITS::ErrorCode SettingDataShareImpl::SetElementName(Uri& uri, const std::string& column, ElementName& value)
{
    if (dataShareHelper_ == nullptr) {
        ErrorLog("%{public}s: dataShareHelper_ is nullptr.", __func__);
        Initialize();
        if (dataShareHelper_ == nullptr) {
            ErrorLog("%{public}s: dataShareHelper_ is nullptr. Retry failed.", __func__);
            return KITS::ERR_NFC_DATABASE_RW;
        }
    }
    ElementName oldVal;
    int errorCode = GetElementName(uri, column, oldVal);
    DataShare::DataShareValueObject keyObj(column);
    DataShare::DataShareValueObject valueObj(value.GetURI());
    DataShare::DataShareValuesBucket bucket;
    bucket.Put(KITS::NFC_DATA_COLUMN_VALUE, valueObj);
    bucket.Put(KITS::NFC_DATA_COLUMN_KEYWORD, keyObj);
    int32_t result;
    if (errorCode != KITS::ERR_NONE) {
        result = dataShareHelper_->Insert(uri, bucket);
    } else {
        DataShare::DataSharePredicates predicates;
        predicates.EqualTo(KITS::NFC_DATA_COLUMN_KEYWORD, column);
        result = dataShareHelper_->Update(uri, predicates, bucket);
    }
    // INVALID_VALUE is -1 DataShare's errorCode
    if (result == KITS::DATA_SHARE_INVALID_VALUE) {
        ErrorLog("%{public}s: can't set value.", __func__);
        return KITS::ERR_NFC_DATABASE_RW;
    }
    return KITS::ERR_NONE;
}
std::shared_ptr<DataShare::DataShareHelper> SettingDataShareImpl::CreateDataShareHelper()
{
    if (remoteObj_ == nullptr) {
        ErrorLog("%{public}s: remoteObject is nullptr, reInitialize.", __func__);
        Initialize();
    }
    return DataShare::DataShareHelper::Creator(remoteObj_, KITS::NFC_DATA_URI_PAYMENT_DEFAULT_APP);
}
void SettingDataShareImpl::Initialize()
{
    auto remote = sptr<ISettingData>(new (std::nothrow) IRemoteStub<ISettingData>());
    if (remote == nullptr) {
        ErrorLog("%{public}s: remoteObject is nullptr.", __func__);
        return;
    }
    remoteObj_ = remote->AsObject();
    dataShareHelper_ = CreateDataShareHelper();
}
} // namespace NFC
} // namespace OHOS