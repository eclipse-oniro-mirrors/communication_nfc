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

#include <vector>
#include <functional>
#include "error_code.h"
#include "connected_tag_impl.h"
#include "log.h"
#include "nfc_napi_adapter.h"

namespace OHOS {
namespace ConnectedTag {
std::string g_writtenNdefData = "";
napi_value Init(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    ConnectedTagImpl connectedTagPtr = OHOS::ConnectedTag::ConnectedTagImpl::GetInstance();
    ErrCode ret = connectedTagPtr.Init();
    napi_value result;
    napi_get_boolean(env, ret == NFC_OPT_SUCCESS, &result);
    return result;
}

napi_value Uninit(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    ConnectedTagImpl connectedTagPtr = OHOS::ConnectedTag::ConnectedTagImpl::GetInstance();
    ErrCode ret = connectedTagPtr.Uninit();
    napi_value result;
    napi_get_boolean(env, ret == NFC_OPT_SUCCESS, &result);
    return result;
}

napi_value ReadNdefTag(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    HILOGI("ReadNdefTag argc = %{public}d", argc);

    ReadAsyncContext *asyncContext = new ReadAsyncContext(env);
    napi_create_string_latin1(env, "readNdefTag", NAPI_AUTO_LENGTH, &asyncContext->resourceName);

    asyncContext->executeFunc = [&](void* data) -> void {
        ReadAsyncContext *context = static_cast<ReadAsyncContext *>(data);
        TRACE_FUNC_CALL_NAME("connectedTagPtr->ReadNdefTag");
        ConnectedTagImpl connectedTagPtr = OHOS::ConnectedTag::ConnectedTagImpl::GetInstance();
        context->errorCode = connectedTagPtr.ReadNdefTag(context->respNdefData);
        HILOGI("ReadNdefTag end errorCode = %{public}d", context->errorCode);
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        ReadAsyncContext *context = static_cast<ReadAsyncContext *>(data);
        napi_create_string_utf8(context->env, context->respNdefData.c_str(), NAPI_AUTO_LENGTH, &context->result);
        HILOGI("ReadNdefTag respNdefData=%{public}s, len = %{public}d",
            context->respNdefData.c_str(), context->respNdefData.length());
    };

    size_t nonCallbackArgNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, nonCallbackArgNum);
}

napi_value WriteNdefTag(napi_env env, napi_callback_info info)
{
    TRACE_FUNC_CALL;
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));

    WriteAsyncContext *asyncContext = new WriteAsyncContext(env);
    napi_create_string_latin1(env, "writeNdefTag", NAPI_AUTO_LENGTH, &asyncContext->resourceName);

    std::string inputWrittenNdefData = "";
    ParseString(env, inputWrittenNdefData, argv[0]);
    HILOGI("WriteNdefTag argc = %{public}d, data=%{public}s, len = %{public}d", argc,
        inputWrittenNdefData.c_str(), inputWrittenNdefData.length());
    asyncContext->writtenNdefData = inputWrittenNdefData;

    asyncContext->executeFunc = [&](void* data) -> void {
        WriteAsyncContext *context = static_cast<WriteAsyncContext *>(data);
        TRACE_FUNC_CALL_NAME("connectedTagPtr->WriteNdefTag");
        ConnectedTagImpl connectedTagPtr = OHOS::ConnectedTag::ConnectedTagImpl::GetInstance();
        HILOGI("WriteNdefTag start ndefData = %{public}s", context->writtenNdefData.c_str());
        context->errorCode = connectedTagPtr.WriteNdefTag(context->writtenNdefData);
        HILOGI("WriteNdefTag end errorCode = %{public}d", context->errorCode);
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        HILOGI("WriteNdefTag completeFunc in, no return value");
    };

    size_t nonCallbackArgNum = 1;
    return DoAsyncWork(env, asyncContext, argc, argv, nonCallbackArgNum);
}
}  // namespace ConnectedTag
}  // namespace OHOS
