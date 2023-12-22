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
#ifndef TAG_FOREGROUND_H
#define TAG_FOREGROUND_H

#include "element_name.h"
#include "iforeground_callback.h"
#include "ireader_mode_callback.h"
#include "itag_session.h"
#include "taginfo.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class TagForeground final {
public:
    explicit TagForeground();
    virtual ~TagForeground();

    static TagForeground &GetInstance();

    /**
     * @Description Registers the callback for tag foreground dispatch.
     * @param element the element name of the hap that request to register foreground dispatch.
     * @param discTech the tag technologies in int array the the hap wants to discover.
     * @param callback the callback to be registered
     * @return The status code for register operation.
     */
    ErrorCode RegForeground(AppExecFwk::ElementName &element,
        std::vector<uint32_t> &discTech, const sptr<KITS::IForegroundCallback> &callback);

    /**
     * @Description Unregisters the callback for tag foreground dispatch.
     * @param element the element name of the hap that request to unregister foreground dispatch.
     * @return The status code for unregister operation.
     */
    ErrorCode UnregForeground(AppExecFwk::ElementName &element);

    /**
     * @Description Registers the callback for tag reader mode.
     * @param element the element name of the hap that request to register reader mode.
     * @param discTech the tag technologies in int array the the hap wants to discover.
     * @param callback the callback to be registered
     * @return The status code for register operation.
     */
    ErrorCode RegReaderMode(AppExecFwk::ElementName &element,
        std::vector<uint32_t> &discTech, const sptr<KITS::IReaderModeCallback> &callback);

    /**
     * @Description Unregisters the callback for tag reader mode.
     * @param element the element name of the hap that request to unregister reader mode.
     * @return The status code for unregister operation.
     */
    ErrorCode UnregReaderMode(AppExecFwk::ElementName &element);

protected:
    OHOS::sptr<TAG::ITagSession> GetTagSessionProxy();

private:
    OHOS::sptr<TAG::ITagSession> tagSessionProxy_;
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_FOREGROUND_H
