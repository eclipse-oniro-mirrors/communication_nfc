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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cstdarg>
#include <string>
#include <unistd.h>
#include "nfc_controller.h"
#include "nfc_sdk_common.h"

using namespace OHOS::NFC::KITS;

namespace {
constexpr int CLI_TIMEOUT_SECONDS = 15;
constexpr int MIN_NUM_INPUT_PARAMETERS = 2;
constexpr char CLI_NAME[] = "ohos-nfcManager";

// Action definitions
constexpr char ACTION_GET_STATE[] = "get-state";
constexpr char ACTION_TURN_ON[] = "turn-on";
constexpr char ACTION_TURN_OFF[] = "turn-off";
constexpr char ACTION_RESTART[] = "restart";
constexpr char ACTION_IS_AVAILABLE[] = "is-available";
constexpr char ACTION_IS_OPEN[] = "is-open";

// Error codes
constexpr char ERR_SA_UNAVAILABLE[] = "E_NFC_SA_UNAVAILABLE";
constexpr char ERR_OPERATION_FAILED[] = "E_NFC_OPERATION_FAILED";
constexpr char ERR_INVALID_ACTION[] = "E_INVALID_ACTION";
constexpr char ERR_TIMEOUT[] = "E_TIMEOUT";

bool g_timeoutTriggered = false;

void TimeoutHandler(int sig)
{
    g_timeoutTriggered = true;
}

void SetupTimeout()
{
    struct sigaction sa;
    sa.sa_handler = TimeoutHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, nullptr);
    alarm(CLI_TIMEOUT_SECONDS);
}

void CancelTimeout()
{
    alarm(0);
}

// Output single-line JSON to stdout
void OutputSuccess(const std::string &dataJson)
{
    printf("{\"success\":true,\"data\":%s}\n", dataJson.c_str());
    fflush(stdout);
}

void OutputError(const char *code, const char *message, const char *suggestion)
{
    printf("{\"success\":false,\"error\":{\"code\":\"%s\",\"message\":\"%s\"},\"suggestion\":\"%s\"}\n",
           code, message, suggestion);
    fflush(stdout);
}

// Log to stderr (not stdout!)
void LogError(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[ERROR][%s] ", CLI_NAME);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
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
        LogError("NFC SA unavailable, result=%d", result);
        OutputError(ERR_SA_UNAVAILABLE, "NFC service unavailable",
                    "Check if NFC service is running, use 'ohos-nfcManager is-available' to verify");
        return 1;
    } else {
        LogError("TurnOn failed with result=%d", result);
        OutputError(ERR_OPERATION_FAILED, "Failed to turn on NFC",
                    "Check if device supports NFC, use 'ohos-nfcManager get-state' to check current state");
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
        LogError("NFC SA unavailable, result=%d", result);
        OutputError(ERR_SA_UNAVAILABLE, "NFC service unavailable",
                    "Check if NFC service is running");
        return 1;
    } else {
        LogError("TurnOff failed with result=%d", result);
        OutputError(ERR_OPERATION_FAILED, "Failed to turn off NFC",
                    "Use 'ohos-nfcManager get-state' to check current state");
        return 1;
    }
}

int HandleRestart()
{
    int result = NfcController::GetInstance().RestartNfc();
    if (result == ErrorCode::ERR_NONE) {
        OutputSuccess("{\"status\":\"restarting\",\"message\":\"NFC is restarting\"}");
        return 0;
    } else if (result == ErrorCode::ERR_NFC_STATE_UNBIND) {
        LogError("NFC SA unavailable, result=%d", result);
        OutputError(ERR_SA_UNAVAILABLE, "NFC service unavailable",
                    "Check if NFC service is running");
        return 1;
    } else {
        LogError("RestartNfc failed with result=%d", result);
        OutputError(ERR_OPERATION_FAILED, "Failed to restart NFC",
                    "Use 'ohos-nfcManager get-state' to check current state");
        return 1;
    }
}

int HandleIsAvailable()
{
    bool available = NfcController::GetInstance().IsNfcAvailable();
    OutputSuccess("{\"available\":" + std::string(available ? "true" : "false") + "}");
    return 0;
}

int HandleIsOpen()
{
    bool isOpen = false;
    int result = NfcController::GetInstance().IsNfcOpen(isOpen);
    if (result == ErrorCode::ERR_NONE) {
        OutputSuccess("{\"is_open\":" + std::string(isOpen ? "true" : "false") + "}");
        return 0;
    } else {
        LogError("IsNfcOpen failed with result=%d", result);
        OutputError(ERR_OPERATION_FAILED, "Failed to get NFC open state",
                    "Use 'ohos-nfcManager get-state' to check current state");
        return 1;
    }
}

void PrintUsage()
{
    fprintf(stderr, "Usage: %s <action>\n", CLI_NAME);
    fprintf(stderr, "\nActions:\n");
    fprintf(stderr, "  %-15s  Get NFC state (off/turning_on/on/turning_off)\n", ACTION_GET_STATE);
    fprintf(stderr, "  %-15s  Turn on NFC\n", ACTION_TURN_ON);
    fprintf(stderr, "  %-15s  Turn off NFC\n", ACTION_TURN_OFF);
    fprintf(stderr, "  %-15s  Restart NFC\n", ACTION_RESTART);
    fprintf(stderr, "  %-15s  Check if NFC is available on this device\n", ACTION_IS_AVAILABLE);
    fprintf(stderr, "  %-15s  Check if NFC is currently open\n", ACTION_IS_OPEN);
    fprintf(stderr, "\nOutput format: Single-line JSON to stdout\n");
    fprintf(stderr, "  Success: {\"success\":true,\"data\":{...}}\n");
    fprintf(stderr, "  Failure: {\"success\":false,\"error\":{...},\"suggestion\":\"...\"}\n");
}

typedef int (*ActionHandler)();

struct ActionEntry {
    const char *action;
    ActionHandler handler;
};

// Static action dispatch table
static const ActionEntry ACTION_TABLE[] = {
    {ACTION_GET_STATE, HandleGetState},
    {ACTION_TURN_ON, HandleTurnOn},
    {ACTION_TURN_OFF, HandleTurnOff},
    {ACTION_RESTART, HandleRestart},
    {ACTION_IS_AVAILABLE, HandleIsAvailable},
    {ACTION_IS_OPEN, HandleIsOpen},
};

constexpr int ACTION_TABLE_SIZE = sizeof(ACTION_TABLE) / sizeof(ACTION_TABLE[0]);

} // anonymous namespace

int main(int argc, char **argv)
{
    // Setup timeout protection
    SetupTimeout();

    // Check arguments
    if (argc < MIN_NUM_INPUT_PARAMETERS || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 ||
        strcmp(argv[1], "help") == 0) {
        PrintUsage();
        OutputError(ERR_INVALID_ACTION,
            "Missing or invalid action parameter",
            "Use action to specify operation, e.g. 'ohos-nfcManager get-state'");
        CancelTimeout();
        return 1;
    }

    const char *action = argv[1];

    // Dispatch to handler
    for (int i = 0; i < ACTION_TABLE_SIZE; ++i) {
        if (strcmp(action, ACTION_TABLE[i].action) == 0) {
            int ret = ACTION_TABLE[i].handler();

            // Check for timeout
            if (g_timeoutTriggered) {
                OutputError(ERR_TIMEOUT, "Operation timeout",
                            "NFC operation did not complete within 15 seconds, check NFC service or hardware");
                return 1;
            }

            CancelTimeout();
            return ret;
        }
    }

    // Unknown action
    LogError("Unknown action: %s", action);
    OutputError(ERR_INVALID_ACTION, "Unknown action type",
                "Supported actions: get-state, turn-on, turn-off, restart, is-available, is-open");
    CancelTimeout();
    return 1;
}