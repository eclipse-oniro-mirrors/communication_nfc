/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include "nfc_controller.h"
#include "nfc_sdk_common.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

using namespace OHOS::NFC::KITS;

namespace {
constexpr int MIN_NUM_INPUT_PARAMETERS = 2;
constexpr char CLI_NAME[] = "ohos-nfcManager";

// Action definitions
constexpr char ACTION_GET_STATE[] = "get-state";
constexpr char ACTION_TURN_ON[] = "turn-on";
constexpr char ACTION_TURN_OFF[] = "turn-off";
constexpr char ACTION_IS_AVAILABLE[] = "is-available";

// Error codes
constexpr char ERR_SA_UNAVAILABLE[] = "E_NFC_SA_UNAVAILABLE";
constexpr char ERR_OPERATION_FAILED[] = "E_NFC_OPERATION_FAILED";
constexpr char ERR_INVALID_ACTION[] = "E_INVALID_ACTION";

void OutputSuccess(const std::string &data)
{
    json result = {
        {"type", "result"},
        {"status", "success"},
        {"data", data}
    };
    std::cout << result.dump() << std::endl;
}

void OutputError(const std::string &code, const std::string &message, const std::string &suggestion,
                 const std::string &logMsg = {})
{
    if (!logMsg.empty()) {
        std::cout << "[ERROR][" << CLI_NAME << "] " << logMsg << "\n";
    }
    json result = {
        {"type", "result"},
        {"status", "failed"},
        {"errCode", code},
        {"errMsg", message},
        {"suggestion", suggestion}
    };
    std::cout << result.dump() << std::endl;
}

std::string NfcStateToString(int state)
{
    switch (state) {
        case NfcState::STATE_OFF:
            return "off";
        case NfcState::STATE_TURNING_ON:
            return "turning_on";
        case NfcState::STATE_ON:
            return "on";
        case NfcState::STATE_TURNING_OFF:
            return "turning_off";
        default:
            return "unknown";
    }
}

int HandleGetState()
{
    int state = NfcController::GetInstance().GetNfcState();
    std::string stateStr = NfcStateToString(state);
    OutputSuccess("{\"state\":\"" + stateStr + "\",\"code\":" + std::to_string(state) + "}");
    return 0;
}

int HandleTurnOn()
{
    int result = NfcController::GetInstance().TurnOn();
    if (result == ErrorCode::ERR_NONE) {
        OutputSuccess("{\"status\":\"turning_on\",\"message\":\"NFC is turning on\"}");
        return 0;
    } else if (result == ErrorCode::ERR_NFC_STATE_UNBIND) {
        OutputError(ERR_SA_UNAVAILABLE, "NFC service unavailable",
                    "Check if NFC service is running, use 'ohos-nfcManager is-available' to verify",
                    "NFC SA unavailable, result=" + std::to_string(result));
        return 1;
    } else {
        OutputError(ERR_OPERATION_FAILED, "Failed to turn on NFC",
                    "Check if device supports NFC, use 'ohos-nfcManager get-state' to check current state",
                    "TurnOn failed with result=" + std::to_string(result));
        return 1;
    }
}

int HandleTurnOff()
{
    int result = NfcController::GetInstance().TurnOff();
    if (result == ErrorCode::ERR_NONE) {
        OutputSuccess("{\"status\":\"turning_off\",\"message\":\"NFC is turning off\"}");
        return 0;
    } else if (result == ErrorCode::ERR_NFC_STATE_UNBIND) {
        OutputError(ERR_SA_UNAVAILABLE, "NFC service unavailable",
                    "Check if NFC service is running",
                    "NFC SA unavailable, result=" + std::to_string(result));
        return 1;
    } else {
        OutputError(ERR_OPERATION_FAILED, "Failed to turn off NFC",
                    "Use 'ohos-nfcManager get-state' to check current state",
                    "TurnOff failed with result=" + std::to_string(result));
        return 1;
    }
}

int HandleIsAvailable()
{
    bool available = NfcController::GetInstance().IsNfcAvailable();
    OutputSuccess("{\"available\":" + std::string(available ? "true" : "false") + "}");
    return 0;
}

void PrintUsage()
{
    std::cout << "Usage: " << CLI_NAME << " <action>\n";
    std::cout << "\nActions:\n";
    std::cout << "  " << ACTION_GET_STATE << "  Get NFC state (off/turning_on/on/turning_off)\n";
    std::cout << "  " << ACTION_TURN_ON << "  Turn on NFC\n";
    std::cout << "  " << ACTION_TURN_OFF << "  Turn off NFC\n";
    std::cout << "  " << ACTION_IS_AVAILABLE << "  Check if NFC is available on this device\n";
    std::cout << "\nOutput format: Single-line JSON to stdout\n";
    std::cout << "  Success: {\"success\":true,\"data\":{...}}\n";
    std::cout << "  Failure: {\"success\":false,\"error\":{...},\"suggestion\":\"...\"}\n";
}

using ActionHandler = std::function<int()>;

struct ActionEntry {
    const std::string action;
    ActionHandler handler;
};

const std::vector<ActionEntry> ACTION_TABLE = {
    {ACTION_GET_STATE, HandleGetState},
    {ACTION_TURN_ON, HandleTurnOn},
    {ACTION_TURN_OFF, HandleTurnOff},
    {ACTION_IS_AVAILABLE, HandleIsAvailable},
};
} // anonymous namespace

int main(int argc, char **argv)
{
    if (argc < MIN_NUM_INPUT_PARAMETERS || std::strcmp(argv[1], "-h") == 0 ||
        std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "help") == 0) {
        PrintUsage();
        OutputError(ERR_INVALID_ACTION,
            "Missing or invalid action parameter",
            "Use action to specify operation, e.g. 'ohos-nfcManager get-state'");
        return 1;
    }

    const std::string action = argv[1];

    for (const auto &entry : ACTION_TABLE) {
        if (action == entry.action) {
            return entry.handler();
        }
    }

    OutputError(ERR_INVALID_ACTION, "Unknown action type",
                "Supported actions: get-state, turn-on, turn-off, is-available",
                "Unknown action: " + action);
    return 1;
}
