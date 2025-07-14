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

#include "ohos.nfc.tag.tag.proj.hpp"
#include "ohos.nfc.tag.tag.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"

#include "basic_tag_session.h"
#include "loghelper.h"
#include "ndef_tag.h"

using namespace nfctech;
using namespace taihe;
using namespace ohos::nfc::tag::tag;

namespace {
class TagSessionImpl {
public:
    TagSessionImpl()
    {
        // Don't forget to implement the constructor.
    }

    bool connectTag()
    {
        TH_THROW(std::runtime_error, "connectTag not implemented");
    }

    void connect()
    {
        TH_THROW(std::runtime_error, "connect not implemented");
    }

    void reset()
    {
        TH_THROW(std::runtime_error, "reset not implemented");
    }

    void resetConnection()
    {
        TH_THROW(std::runtime_error, "resetConnection not implemented");
    }

    bool isTagConnected()
    {
        TH_THROW(std::runtime_error, "isTagConnected not implemented");
    }

    bool isConnected()
    {
        TH_THROW(std::runtime_error, "isConnected not implemented");
    }

    bool setSendDataTimeout(int32_t timeout)
    {
        TH_THROW(std::runtime_error, "setSendDataTimeout not implemented");
    }

    void setTimeout(int32_t timeout)
    {
        TH_THROW(std::runtime_error, "setTimeout not implemented");
    }

    int32_t getSendDataTimeout()
    {
        TH_THROW(std::runtime_error, "getSendDataTimeout not implemented");
    }

    int32_t getTimeout()
    {
        TH_THROW(std::runtime_error, "getTimeout not implemented");
    }

    array<int32_t> sendData(array_view<int32_t> data)
    {
        TH_THROW(std::runtime_error, "sendData not implemented");
    }

    array<int32_t> transmit(array_view<int32_t> data)
    {
        TH_THROW(std::runtime_error, "transmit not implemented");
    }

    int32_t getMaxSendLength()
    {
        TH_THROW(std::runtime_error, "getMaxSendLength not implemented");
    }

    int32_t getMaxTransmitSize()
    {
        TH_THROW(std::runtime_error, "getMaxTransmitSize not implemented");
    }
};

class NfcATagImpl : public TagSessionImpl {
public:
    NfcATagImpl()
    {
        // Don't forget to implement the constructor.
    }

    int32_t getSak()
    {
        TH_THROW(std::runtime_error, "getSak not implemented");
    }

    array<int32_t> getAtqa()
    {
        TH_THROW(std::runtime_error, "getAtqa not implemented");
    }
};

class NfcBTagImpl : public TagSessionImpl {
public:
    NfcBTagImpl()
    {
        // Don't forget to implement the constructor.
    }

    array<int32_t> getRespAppData()
    {
        TH_THROW(std::runtime_error, "getRespAppData not implemented");
    }

    array<int32_t> getRespProtocol()
    {
        TH_THROW(std::runtime_error, "getRespProtocol not implemented");
    }
};

class NfcFTagImpl : public TagSessionImpl {
public:
    NfcFTagImpl()
    {
        // Don't forget to implement the constructor.
    }

    array<int32_t> getSystemCode()
    {
        TH_THROW(std::runtime_error, "getSystemCode not implemented");
    }

    array<int32_t> getPmm()
    {
        TH_THROW(std::runtime_error, "getPmm not implemented");
    }
};

class NfcVTagImpl : public TagSessionImpl {
public:
    NfcVTagImpl()
    {
        // Don't forget to implement the constructor.
    }

    int32_t getResponseFlags()
    {
        TH_THROW(std::runtime_error, "getResponseFlags not implemented");
    }

    int32_t getDsfId()
    {
        TH_THROW(std::runtime_error, "getDsfId not implemented");
    }
};

class IsoDepTagImpl : public TagSessionImpl {
public:
    IsoDepTagImpl()
    {
        // Don't forget to implement the constructor.
    }

    array<int32_t> getHistoricalBytes()
    {
        TH_THROW(std::runtime_error, "getHistoricalBytes not implemented");
    }

    array<int32_t> getHiLayerResponse()
    {
        TH_THROW(std::runtime_error, "getHiLayerResponse not implemented");
    }

    bool isExtendedApduSupported()
    {
        TH_THROW(std::runtime_error, "isExtendedApduSupported not implemented");
    }
};

class NdefMessageImpl {
public:
    NdefMessageImpl()
    {
        // Don't forget to implement the constructor.
    }

    array<::ohos::nfc::tag::tag::NdefRecord> getNdefRecords()
    {
        TH_THROW(std::runtime_error, "getNdefRecords not implemented");
    }
};

class NdefTagImpl : public TagSessionImpl {
public:
    NdefTagImpl()
    {
        // Don't forget to implement the constructor.
    }

    ::ohos::nfc::tag::tag::NfcForumType getNdefTagType()
    {
        TH_THROW(std::runtime_error, "getNdefTagType not implemented");
    }

    NdefMessage getNdefMessage()
    {
        // The parameters in the make_holder function should be of the same type
        // as the parameters in the constructor of the actual implementation class.
        return make_holder<NdefMessageImpl, NdefMessage>();
    }

    bool isNdefWritable()
    {
        TH_THROW(std::runtime_error, "isNdefWritable not implemented");
    }

    NdefMessage readNdef()
    {
        // The parameters in the make_holder function should be of the same type
        // as the parameters in the constructor of the actual implementation class.
        return make_holder<NdefMessageImpl, NdefMessage>();
    }

    void writeNdef(weak::NdefMessage msg)
    {
        TH_THROW(std::runtime_error, "writeNdef not implemented");
    }

    bool canSetReadOnly()
    {
        TH_THROW(std::runtime_error, "canSetReadOnly not implemented");
    }

    void setReadOnly()
    {
        TH_THROW(std::runtime_error, "setReadOnly not implemented");
    }

    string getNdefTagTypeString(string_view type)
    {
        TH_THROW(std::runtime_error, "getNdefTagTypeString not implemented");
    }
};

class MifareClassicTagImpl : public TagSessionImpl {
public:
    MifareClassicTagImpl()
    {
        // Don't forget to implement the constructor.
    }

    void authenticateSector(int32_t sectorIndex, array_view<int32_t> key, bool isKeyA)
    {
        TH_THROW(std::runtime_error, "authenticateSector not implemented");
    }

    array<int32_t> readSingleBlock(int32_t blockIndex)
    {
        TH_THROW(std::runtime_error, "readSingleBlock not implemented");
    }

    void writeSingleBlock(int32_t blockIndex, array_view<int32_t> data)
    {
        TH_THROW(std::runtime_error, "writeSingleBlock not implemented");
    }

    void incrementBlock(int32_t blockIndex, int32_t value)
    {
        TH_THROW(std::runtime_error, "incrementBlock not implemented");
    }

    void decrementBlock(int32_t blockIndex, int32_t value)
    {
        TH_THROW(std::runtime_error, "decrementBlock not implemented");
    }

    void transferToBlock(int32_t blockIndex)
    {
        TH_THROW(std::runtime_error, "transferToBlock not implemented");
    }

    void restoreFromBlock(int32_t blockIndex)
    {
        TH_THROW(std::runtime_error, "restoreFromBlock not implemented");
    }

    int32_t getSectorCount()
    {
        TH_THROW(std::runtime_error, "getSectorCount not implemented");
    }

    int32_t getBlockCountInSector(int32_t sectorIndex)
    {
        TH_THROW(std::runtime_error, "getBlockCountInSector not implemented");
    }

    ::ohos::nfc::tag::tag::MifareClassicType getType()
    {
        TH_THROW(std::runtime_error, "getType not implemented");
    }

    int32_t getTagSize()
    {
        TH_THROW(std::runtime_error, "getTagSize not implemented");
    }

    bool isEmulatedTag()
    {
        TH_THROW(std::runtime_error, "isEmulatedTag not implemented");
    }

    int32_t getBlockIndex(int32_t sectorIndex)
    {
        TH_THROW(std::runtime_error, "getBlockIndex not implemented");
    }

    int32_t getSectorIndex(int32_t blockIndex)
    {
        TH_THROW(std::runtime_error, "getSectorIndex not implemented");
    }
};

class MifareUltralightTagImpl : public TagSessionImpl {
public:
    MifareUltralightTagImpl()
    {
        // Don't forget to implement the constructor.
    }

    array<int32_t> readMultiplePages(int32_t pageIndex)
    {
        TH_THROW(std::runtime_error, "readMultiplePages not implemented");
    }

    void writeSinglePage(int32_t pageIndex, array_view<int32_t> data)
    {
        TH_THROW(std::runtime_error, "writeSinglePage not implemented");
    }

    ::ohos::nfc::tag::tag::MifareUltralightType getType()
    {
        TH_THROW(std::runtime_error, "getType not implemented");
    }
};

class NdefFormatableTagImpl : public TagSessionImpl {
public:
    NdefFormatableTagImpl()
    {
        // Don't forget to implement the constructor.
    }

    void format(weak::NdefMessage message)
    {
        TH_THROW(std::runtime_error, "format not implemented");
    }

    void formatReadOnly(weak::NdefMessage message)
    {
        TH_THROW(std::runtime_error, "formatReadOnly not implemented");
    }
};

::nfctech::NfcATag getNfcATag(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NfcATagImpl, ::nfctech::NfcATag>();
}

::nfctech::NfcATag getNfcA(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NfcATagImpl, ::nfctech::NfcATag>();
}

::nfctech::NfcBTag getNfcBTag(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NfcBTagImpl, ::nfctech::NfcBTag>();
}

::nfctech::NfcBTag getNfcB(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NfcBTagImpl, ::nfctech::NfcBTag>();
}

::nfctech::NfcFTag getNfcFTag(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NfcFTagImpl, ::nfctech::NfcFTag>();
}

::nfctech::NfcFTag getNfcF(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NfcFTagImpl, ::nfctech::NfcFTag>();
}

::nfctech::NfcVTag getNfcVTag(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NfcVTagImpl, ::nfctech::NfcVTag>();
}

::nfctech::NfcVTag getNfcV(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NfcVTagImpl, ::nfctech::NfcVTag>();
}

::nfctech::IsoDepTag getIsoDep(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<IsoDepTagImpl, ::nfctech::IsoDepTag>();
}

::nfctech::NdefTag getNdef(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NdefTagImpl, ::nfctech::NdefTag>();
}

::nfctech::MifareClassicTag getMifareClassic(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<MifareClassicTagImpl, ::nfctech::MifareClassicTag>();
}

::nfctech::MifareUltralightTag getMifareUltralight(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<MifareUltralightTagImpl, ::nfctech::MifareUltralightTag>();
}

::nfctech::NdefFormatableTag getNdefFormatable(TagInfo const& tagInfo)
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<NdefFormatableTagImpl, ::nfctech::NdefFormatableTag>();
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getNfcATag(getNfcATag);
TH_EXPORT_CPP_API_getNfcA(getNfcA);
TH_EXPORT_CPP_API_getNfcBTag(getNfcBTag);
TH_EXPORT_CPP_API_getNfcB(getNfcB);
TH_EXPORT_CPP_API_getNfcFTag(getNfcFTag);
TH_EXPORT_CPP_API_getNfcF(getNfcF);
TH_EXPORT_CPP_API_getNfcVTag(getNfcVTag);
TH_EXPORT_CPP_API_getNfcV(getNfcV);
TH_EXPORT_CPP_API_getIsoDep(getIsoDep);
TH_EXPORT_CPP_API_getNdef(getNdef);
TH_EXPORT_CPP_API_getMifareClassic(getMifareClassic);
TH_EXPORT_CPP_API_getMifareUltralight(getMifareUltralight);
TH_EXPORT_CPP_API_getNdefFormatable(getNdefFormatable);
// NOLINTEND
