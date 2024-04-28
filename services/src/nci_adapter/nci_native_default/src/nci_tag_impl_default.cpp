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

#include "nci_tag_impl_default.h"
#include "nfcc_nci_adapter.h"
#include "tag_native_impl.h"

namespace OHOS {
namespace NFC {
namespace NCI {
void NciTagImplDefault::SetTagListener(std::weak_ptr<ITagListener> listener)
{
    TagNativeImpl::GetInstance().SetTagListener(listener);
}

std::vector<int> NciTagImplDefault::GetTechList(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->GetTechList();
    }
    return {};
}

uint32_t NciTagImplDefault::GetConnectedTech(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->GetConnectedTech();
    }
    return 0;
}

std::vector<AppExecFwk::PacMap> NciTagImplDefault::GetTechExtrasData(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->GetTechExtrasData();
    }
    return {};
}

std::string NciTagImplDefault::GetTagUid(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->GetTagUid();
    }
    return {};
}

bool NciTagImplDefault::Connect(uint32_t tagDiscId, uint32_t technology)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->Connect(technology);
    }
    return false;
}

bool NciTagImplDefault::Disconnect(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->Disconnect();
    }
    return false;
}

bool NciTagImplDefault::Reconnect(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->Reconnect();
    }
    return false;
}

int NciTagImplDefault::Transceive(uint32_t tagDiscId, const std::string &command, std::string &response)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->Transceive(command, response);
    }
    return 0;
}

std::string NciTagImplDefault::ReadNdef(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->ReadNdef();
    }
    return {};
}

std::string NciTagImplDefault::FindNdefTech(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->FindNdefTech();
    }
    return {};
}

bool NciTagImplDefault::WriteNdef(uint32_t tagDiscId, std::string &command)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->WriteNdef(command);
    }
    return false;
}

bool NciTagImplDefault::FormatNdef(uint32_t tagDiscId, const std::string &key)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->FormatNdef(key);
    }
    return false;
}

bool NciTagImplDefault::CanMakeReadOnly(uint32_t ndefType)
{
    return TagNativeImpl::GetInstance().CanMakeReadOnly(ndefType);
}

bool NciTagImplDefault::SetNdefReadOnly(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->SetNdefReadOnly();
    }
    return false;
}

bool NciTagImplDefault::DetectNdefInfo(uint32_t tagDiscId, std::vector<int> &ndefInfo)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->DetectNdefInfo(ndefInfo);
    }
    return false;
}

bool NciTagImplDefault::IsTagFieldOn(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->IsTagFieldOn();
    }
    return false;
}

void NciTagImplDefault::StartFieldOnChecking(uint32_t tagDiscId, uint32_t delayedMs)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        tagDiscId_ = tagDiscId;
        return tag->StartFieldOnChecking(delayedMs);
    }
}

void NciTagImplDefault::StopFieldChecking()
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId_).lock();
    if (tag) {
        return tag->StopFieldChecking();
    }
}

void NciTagImplDefault::SetTimeout(uint32_t tagDiscId, uint32_t timeout, uint32_t technology)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        return tag->SetTimeout(timeout, technology);
    }
}

void NciTagImplDefault::GetTimeout(uint32_t tagDiscId, uint32_t &timeout, uint32_t technology)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        timeout = tag->GetTimeout(technology);
    }
    return;
}

void NciTagImplDefault::ResetTimeout(uint32_t tagDiscId)
{
    auto tag = TagNativeImpl::GetInstance().GetTag(tagDiscId).lock();
    if (tag) {
        tag->ResetTimeout();
    }
    return;
}

uint32_t NciTagImplDefault::GetIsoDepMaxTransceiveLength()
{
    return TagNativeImpl::GetInstance().GetIsoDepMaxTransceiveLength();
}

bool NciTagImplDefault::IsExtendedLengthApduSupported()
{
    return TagNativeImpl::GetInstance().GetIsoDepMaxTransceiveLength() > ISO_DEP_FRAME_MAX_LEN;
}

uint16_t NciTagImplDefault::GetTechMaskFromTechList(const std::vector<uint32_t> &discTech)
{
    return TagNativeImpl::GetInstance().GetTechMaskFromTechList(discTech);
}

std::string NciTagImplDefault::GetVendorBrowserBundleName()
{
    return "";
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS