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
#ifndef I_HCE_SESSION_H
#define I_HCE_SESSION_H

#include "element_name.h"
#include "ihce_cmd_callback.h"
#include "iremote_broker.h"
#include "nfc_sdk_common.h"
#include "parcel.h"
#include "ability_info.h"

namespace OHOS {
namespace NFC {
namespace HCE {
using AppExecFwk::AbilityInfo;
using AppExecFwk::ElementName;
class IHceSession : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.cardemulation.IHceSession");

    virtual ~IHceSession() {}
    /**
     * @brief  register on hce cmd
     * @param  callback: callback
     * @param  type: register type hcecmd
     * @return result
     */
    virtual KITS::ErrorCode RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback,
                                              const std::string &type) = 0;
    /**
     * @brief  js service send raw data
     * @param  hexCmdData: raw data from js service
     * @param  raw: raw if true
     * @param  hexRespData: response data
     * @return result
     */
    virtual int SendRawFrame(std::string hexCmdData, bool raw, std::string &hexRespData) = 0;
    /**
     * @brief  get payment services
     * @param  abilityInfos: payment services
     * @return result
     */
    virtual int GetPaymentServices(std::vector<AbilityInfo> &abilityInfos) = 0;
    /**
     * @brief  stop hce, unregister callback and unset foreground service
     * @param  element: the element service want to stop hce
     * @return result
     */
    virtual KITS::ErrorCode StopHce(ElementName &element) = 0;
    /**
     * @brief  whether the element is default service or not
     * @param  element: element to be judged
     * @param  type: card type
     * @param  isDefaultService:  is default service
     * @return result
     */
    virtual KITS::ErrorCode IsDefaultService(ElementName &element, const std::string &type,
                                             bool &isDefaultService) = 0;
    /**
     * @brief start hce
     * @param  element: foreground element
     * @param  aids: dynamic aid
     * @return result
     */
    virtual KITS::ErrorCode StartHce(const ElementName &element, const std::vector<std::string> &aids) = 0;

private:
};
} // namespace HCE
} // namespace NFC
} // namespace OHOS
#endif
