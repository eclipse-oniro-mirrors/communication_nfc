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

#include "ohos.nfc.tag.tag.ndef.proj.hpp"
#include "ohos.nfc.tag.tag.ndef.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"

using namespace taihe;

namespace {
::ohos::nfc::tag::tag::NdefRecord makeUriRecord(string_view uri)
{
    TH_THROW(std::runtime_error, "makeUriRecord not implemented");
}

::ohos::nfc::tag::tag::NdefRecord makeTextRecord(string_view text, string_view locale)
{
    TH_THROW(std::runtime_error, "makeTextRecord not implemented");
}

::ohos::nfc::tag::tag::NdefRecord makeMimeRecord(string_view mimeType, array_view<int32_t> mimeData)
{
    TH_THROW(std::runtime_error, "makeMimeRecord not implemented");
}

::ohos::nfc::tag::tag::NdefRecord makeExternalRecord(
    string_view domainName, string_view type, array_view<int32_t> externalData)
{
    TH_THROW(std::runtime_error, "makeExternalRecord not implemented");
}

array<int32_t> messageToBytes(::nfctech::weak::NdefMessage ndefMessage)
{
    TH_THROW(std::runtime_error, "messageToBytes not implemented");
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_makeUriRecord(makeUriRecord);
TH_EXPORT_CPP_API_makeTextRecord(makeTextRecord);
TH_EXPORT_CPP_API_makeMimeRecord(makeMimeRecord);
TH_EXPORT_CPP_API_makeExternalRecord(makeExternalRecord);
TH_EXPORT_CPP_API_messageToBytes(messageToBytes);
// NOLINTEND
