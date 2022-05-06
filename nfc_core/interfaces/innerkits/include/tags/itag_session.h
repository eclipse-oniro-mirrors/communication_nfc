/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef I_TAG_SESSION_H
#define I_TAG_SESSION_H

#include "iremote_broker.h"
#include "nfc_sdk_common.h"
#include "parcel.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class ResResult : public OHOS::Parcelable {
public:
    ResResult() : result(RESULT_FAILURE), resData("") {}
    virtual ~ResResult() {}

    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        parcel.WriteInt32(result);
        parcel.WriteString(resData);
        return true;
    }

    static ResResult* Unmarshalling(OHOS::Parcel &parcel)
    {
        ResResult* res = new ResResult();
        res->SetResult(parcel.ReadInt32());
        res->SetResData(parcel.ReadString());
        return res;
    }

    void SetResult(int32_t r)
    {
        result = r;
    }
    int32_t GetResult() const
    {
        return result;
    }
    void SetResData(const std::string data)
    {
        resData = data;
    }
    std::string GetResData() const
    {
        return resData;
    }
    enum ResponseResult { RESULT_SUCCESS = 0, RESULT_EXCEEDED_LENGTH, RESULT_TAGLOST, RESULT_FAILURE };

private:
    int32_t result;
    std::string resData;
};

class ITagSession : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.TAG.ITagSession");

    virtual ~ITagSession() {}
    /**
     * @brief To connect the tagRfDiscId by technology.
     * @param tagRfDiscId the rf disc id of tag
     * @param technology the tag technology
     * @return the result to connect the tag
     */
    virtual int Connect(int tagRfDiscId, int technology) = 0;
    /**
     * @brief To reconnect the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     * @return the result to reconnect the tag
     */
    virtual int Reconnect(int tagRfDiscId) = 0;
    /**
     * @brief To disconnect the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     */
    virtual void Disconnect(int tagRfDiscId) = 0;
    /**
     * @brief Get the TechList of the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     * @return TechList
     */
    virtual std::vector<int> GetTechList(int tagRfDiscId) = 0;
    /**
     * @brief Checking the tagRfDiscId is field on.
     * @param tagRfDiscId the rf disc id of tag
     * @return true - Presnet; the other - No Presnet
     */
    virtual bool IsTagFieldOn(int tagRfDiscId) = 0;
    /**
     * @brief Checking the tagRfDiscId is a Ndef Tag.
     * @param tagRfDiscId the rf disc id of tag
     * @return true - Ndef Tag; the other - No Ndef Tag
     */
    virtual bool IsNdef(int tagRfDiscId) = 0;
    /**
     * @brief To send the data to the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     * @param data the sent data
     * @param raw to send whether original data or un-original data
     * @return The response result from the host tag
     */
    virtual std::unique_ptr<ResResult> SendRawFrame(int tagRfDiscId, std::string data, bool raw) = 0;
    /**
     * @brief Reading from the host tag
     * @param tagRfDiscId the rf disc id of tag
     * @return the read data
     */
    virtual std::string NdefRead(int tagRfDiscId) = 0;
    /**
     * @brief Writing the data into the host tag.
     * @param tagRfDiscId the rf disc id of tag
     * @param msg the wrote data
     * @return the Writing Result
     */
    virtual int NdefWrite(int tagRfDiscId, std::string msg) = 0;
    /**
     * @brief Making the host tag to read only.
     * @param tagRfDiscId the rf disc id of tag
     * @return the making result
     */
    virtual int NdefMakeReadOnly(int tagRfDiscId) = 0;
    /**
     * @brief format the tag by Ndef
     * @param tagRfDiscId the rf disc id of tag
     * @param key the format key
     * @return the format result
     */
    virtual int FormatNdef(int tagRfDiscId, const std::string& key) = 0;
    /**
     * @brief Checking the host tag is Read only
     * @param technology the tag technology
     * @return true - ReadOnly; false - No Read Only
     */
    virtual bool CanMakeReadOnly(int technology) = 0;
    /**
     * @brief Get Max Transceive Length
     * @param technology the tag technology
     * @return Max Transceive Length
     */
    virtual int GetMaxTransceiveLength(int technology) = 0;
    /**
     * @brief Checking the NfccHost whether It supported the extended Apdus
     * @return true - yes; false - no
     */
    virtual bool IsSupportedApdusExtended() = 0;

private:
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // I_TAG_SESSION_H
