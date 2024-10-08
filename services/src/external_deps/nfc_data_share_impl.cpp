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
#include "nfc_data_share_impl.h"
#include "loghelper.h"
#include "iremote_broker.h"
#include "iremote_stub.h"

namespace OHOS {
namespace NFC {
NfcDataShareImpl::NfcDataShareImpl()
{
    CreateDataShareHelper();
}

NfcDataShareImpl::~NfcDataShareImpl()
{
    remoteObj_ = nullptr;
}

std::shared_ptr<DataShare::DataShareHelper> NfcDataShareImpl::CreateDataShareHelper()
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (remoteObj_ == nullptr) {
        WarnLog("%{public}s: remoteObject is nullptr, reInitialize.", __func__);
        auto remote = sptr<INfcState>(new (std::nothrow) IRemoteStub<INfcState>());
        if (remote == nullptr) {
            ErrorLog("%{public}s: get remoteObject fail.", __func__);
            return nullptr;
        }
        remoteObj_ = remote->AsObject();
    }
    return DataShare::DataShareHelper::Creator(remoteObj_, KITS::NFC_DATA_URI);
}

KITS::ErrorCode NfcDataShareImpl::GetValue(Uri &uri, const std::string &column, int32_t &value)
{
    if (dataShareHelper_ == nullptr) {
        ErrorLog("%{public}s: dataShareHelper_ is nullptr, retry init.", __func__);
        CreateDataShareHelper();
        if (dataShareHelper_ == nullptr) {
            ErrorLog("%{public}s: dataShareHelper_ is nullptr, retry init failed.", __func__);
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
    int32_t result = rows->GetString(columnIndex, valueStr);
    if (result != KITS::ERR_NONE) {
        ErrorLog("%{public}s: can't get value.", __func__);
        return KITS::ERR_NFC_DATABASE_RW;
    }
    rows->Close();
    value = atoi(valueStr.c_str());
    InfoLog("GetValue success, value = %{public}d.", value);
    return KITS::ERR_NONE;
}

KITS::ErrorCode NfcDataShareImpl::SetValue(Uri &uri, const std::string &column, int &value)
{
    if (dataShareHelper_ == nullptr) {
        ErrorLog("%{public}s: dataShareHelper_ is nullptr, retry init.", __func__);
        CreateDataShareHelper();
        if (dataShareHelper_ == nullptr) {
            ErrorLog("%{public}s: dataShareHelper_ is nullptr, retry init failed.", __func__);
            return KITS::ERR_NFC_DATABASE_RW;
        }
    }
    int oldVal = 0;
    int errorCode = GetValue(uri, column, oldVal);
    DataShare::DataShareValueObject keyObj(column);
    DataShare::DataShareValueObject valueObj(std::to_string(value));
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
} // NFC
} // OHOS