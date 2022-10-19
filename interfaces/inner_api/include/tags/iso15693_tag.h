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
#ifndef ISO15693_TAG_H
#define ISO15693_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class Iso15693Tag final : public BasicTagSession {
public:
    static const int ISO15693_MAX_BLOCK_INDEX = 256;
    static const int ISO15693_MAX_FLAG_COUNT = 256;

    static const int CMD_READ_SINGLE_BLOCK = 0x20;
    static const int CMD_WRITE_SINGLE_BLOCK = 0x21;
    static const int CMD_LOCK_SINGLE_BLOCK = 0x22;
    static const int CMD_READ_MULTIPLE_BLOCK = 0x23;
    static const int CMD_WRITE_MULTIPLE_BLOCK = 0x24;

public:
    explicit Iso15693Tag(std::weak_ptr<TagInfo> tag);
    ~Iso15693Tag();

    /**
     * @Description Get an object of Iso15693 for the given tag
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<Iso15693Tag>
     */
    static std::shared_ptr<Iso15693Tag> GetTag(std::weak_ptr<TagInfo> tag);
    /**
     * @Description Read a block
     * @param flag If the Option_flag is not set, the VICC shall return its response when it has completed the lock
     * operation. If it is set, the VICC shall wait for the reception of an EOF from the VCD and upon such reception
     * shall return its response.
     * @param blockIndex index of block to read
     * @param hexRespData the hex response data read.
     * @return the error code of calling function.
     */
    int ReadSingleBlock(uint32_t flag, uint32_t blockIndex, std::string &hexRespData);
    /**
     * @Description Write a block
     * @param flag If the Option_flag is not set, the VICC shall return its response when it has completed the lock
     * operation. If it is set, the VICC shall wait for the reception of an EOF from the VCD and upon such reception
     * shall return its response.
     * @param blockIndex index of block to write
     * @param hexCmdData block data to write
     * @return Errorcode of write. if return 0, means successful.
     */
    int WriteSingleBlock(uint32_t flag, uint32_t blockIndex, const std::string& hexCmdData);
    /**
     * @Description Lock a block. A locked block can only be read, not written.
     * @param flag If the Option_flag is not set, the VICC shall return its response when it has completed the lock
     * operation. If it is set, the VICC shall wait for the reception of an EOF from the VCD and upon such reception
     * shall return its response.
     * @param blockIndex index of block to lock
     * @return Errorcode of lock. if return 0, means successful.
     */
    int LockSingleBlock(uint32_t flag, uint32_t blockIndex);
    /**
     * @Description Read multiple blocks
     * @param flag If the Option_flag is not set, the VICC shall return its response when it has completed the lock
     * operation. If it is set, the VICC shall wait for the reception of an EOF from the VCD and upon such reception
     * shall return its response.
     * @param blockIndex index of block to read
     * @param blockNum num of block to read
     * @param hexRespData the hex response data read.
     * @return the error code of calling function.
     */
    int ReadMultipleBlock(uint32_t flag, uint32_t blockIndex, uint32_t blockNum, std::string &hexRespData);
    /**
     * @Description Write multiple blocks
     * @param flag If the Option_flag is not set, the VICC shall return its response when it has completed the lock
     * operation. If it is set, the VICC shall wait for the reception of an EOF from the VCD and upon such reception
     * shall return its response.
     * @param blockIndex index of block to write
     * @param blockNum num of block to write
     * @param hexCmdData block data to write
     * @return Errorcode of write. if return 0, means successful.
     */
    int WriteMultipleBlock(uint32_t flag, uint32_t blockIndex, uint32_t blockNum, const std::string& hexCmdData);
    /**
     * @Description Get DsfId bytes of the tag.
     * @param void
     * @return DsfId bytes
     */
    char GetDsfId() const;
    /**
     * @Description Get RespFlags bytes of the tag.
     * @param void
     * @return RespFlags bytes
     */
    char GetRespFlags() const;

private:
    char respFlags_ {};
    char dsfId_ {};
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // ISO15693_TAG_H
