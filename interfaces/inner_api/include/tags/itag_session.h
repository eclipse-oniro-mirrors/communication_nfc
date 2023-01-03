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
     * @brief Set the Timeout for tag operations
     *
     * @param timeout the timeout value to set for tag operations
     * @param technology the tag technology
     * @return the tatus code for function calling.
     */
    virtual int SetTimeout(int timeout, int technology) = 0;
    /**
     * @brief Get the Timeout value of tag operations
     *
     * @param technology the tag technology
     * @param timeout the output argument to read the timeout.
     * @return the tatus code for function calling.
     */
    virtual int GetTimeout(int technology, int &timeout) = 0;
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
     * @param hexCmdData the sent data
     * @param hexRespData the response hex data.
     * @param raw to send whether original data or un-original data
     * @return the error code of calling function.
     */
    virtual int SendRawFrame(int tagRfDiscId, std::string hexCmdData, bool raw, std::string &hexRespData) = 0;
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
     * @param ndefType the ndef type.
     * @param canSetReadOnly the output for read only or not.
     * @return the error code of calling function.
     */
    virtual int CanMakeReadOnly(int ndefType, bool &canSetReadOnly) = 0;
    /**
     * @brief Get Max Transceive Length
     * @param technology the tag technology
     * @return Max Transceive Length
     */
    virtual int GetMaxTransceiveLength(int technology, int &maxSize) = 0;
    /**
     * @brief Checking the NfccHost whether It supported the extended Apdus
     * @param isSupported the output for checking supportting extended apdu or not.
     * @return the error code of calling function.
     */
    virtual int IsSupportedApdusExtended(bool &isSupported) = 0;

private:
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // I_TAG_SESSION_H
