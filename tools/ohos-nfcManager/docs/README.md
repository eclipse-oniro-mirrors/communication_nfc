# ohos-nfcManager Interface Report

## Overview

`ohos-nfcManager` is a CLI tool for querying and controlling device NFC function status. It provides NFC state query and control capabilities through the `NfcController` singleton interface.

**Target Users**: LLM Agent / Automation scripts
**Use Cases**: NFC state query, switch control, operation and maintenance inspection

## Interface Source

**Selected Layer**: `NfcController` singleton class in `interfaces/inner_api/controller/nfc_controller.h`

**Selection Reasons**:
1. This class encapsulates IDL proxy calls and provides synchronous interfaces
2. It is the official entry point for system NFC control
3. No need to handle underlying IPC details
4. Clear interface signatures, suitable for CLI encapsulation

**Related Files**:
| File | Description |
|------|-------------|
| `interfaces/inner_api/controller/nfc_controller.h` | Header file |
| `interfaces/inner_api/controller/idl/INfcController.idl` | IDL definition |
| `interfaces/inner_api/controller/nfc_controller.cpp` | Implementation |

## Architecture

```
ohos-nfcManager (CLI Entry)
        |
        v
    main.cpp
        |
        v
NfcController::GetInstance() ---> NfcService (SA)
        |
        v
    Synchronous IPC Call
```

### Flow Description
1. CLI receives action argument from command line
2. Lookup action in `ACTION_TABLE` dispatch table
3. Call corresponding handler function
4. Handler invokes `NfcController` singleton method
5. `NfcController` makes synchronous IPC call to NfcService
6. Return result via JSON output to stdout

## CLI Subcommand Mapping

| CLI Subcommand | Corresponding Interface | Description |
|---------------|----------------------|-------------|
| `get-state` | `NfcController::GetNfcState()` | Get current NFC state |
| `turn-on` | `NfcController::TurnOn()` | Turn on NFC |
| `turn-off` | `NfcController::TurnOff()` | Turn off NFC |
| `is-available` | `NfcController::IsNfcAvailable()` | Check if device supports NFC |

## Unencapsulated Interfaces

| Interface Name | Reason for Not Encapsulating |
|---------------|------------------------------|
| `RegListener` | Callback interface, requires event loop support, not suitable for CLI |
| `UnregListener` | Callback interface, used with RegListener |
| `RegNdefMsgCb` | Callback interface, used for NDEF message subscription |
| `GetTagServiceIface` | Returns IRemoteObject, non-atomic data, not suitable for direct CLI output |
| `GetHceServiceIface` | Returns IRemoteObject, non-atomic data |

## Usage

### Syntax
```
ohos-nfcManager <action>
```

### Examples

#### Get NFC State
```bash
ohos-nfcManager get-state
# Output: {"type":"result","status":"success","data":{"state":"on","code":3}}
```

#### Turn On NFC
```bash
ohos-nfcManager turn-on
# Output: {"type":"result","status":"success","data":{"status":"turning_on","message":"NFC is turning on"}}
```

#### Turn Off NFC
```bash
ohos-nfcManager turn-off
# Output: {"type":"result","status":"success","data":{"status":"turning_off","message":"NFC is turning off"}}
```

#### Check Device NFC Support
```bash
ohos-nfcManager is-available
# Output: {"type":"result","status":"success","data":{"available":true}}
```

#### Error Response Example
```bash
ohos-nfcManager turn-on
# Output: {"type":"result","status":"failed","errCode":"ERR_NFC_SA_UNAVAILABLE","errMsg":"NFC service unavailable","suggestion":"Check if NFC service is running, use 'ohos-nfcManager is-available' to verify"}
```

#### Help
```bash
ohos-nfcManager --help
# or
ohos-nfcManager -h
# or
ohos-nfcManager help
```

## Output Format

### Success Response
```json
{"type":"result","status":"success","data":{...}}
```

### Error Response
```json
{"type":"result","status":"failed","errCode":"<error_code>","errMsg":"<error_message>","suggestion":"<suggestion>"}
```

## Error Codes

| Code | Description |
|------|-------------|
| `E_NFC_SA_UNAVAILABLE` | NFC service unavailable |
| `E_NFC_OPERATION_FAILED` | NFC operation failed |
| `E_INVALID_ACTION` | Invalid action parameter |
| `E_TIMEOUT` | Operation timeout (15 seconds) |

## Behavior Constraints

- **Timeout Protection**: 15 second timeout using alarm signal
- **No Interactive Confirmation**: All operations are non-interactive
- **Idempotent Design**: Repeated calls return current state without side effects
- **Output Channels**: stdout for JSON output, stderr for error logs

## NFC State Codes

| Code | State | Description |
|------|-------|-------------|
| 1 | `off` | NFC is turned off |
| 2 | `turning_on` | NFC is turning on |
| 3 | `on` | NFC is turned on |
| 4 | `turning_off` | NFC is turning off |

## File List

| File Path | Description |
|-----------|-------------|
| `tools/ohos-nfcManager/src/main.cpp` | Main program implementation |
| `tools/ohos-nfcManager/BUILD.gn` | Build configuration |
| `tools/ohos-nfcManager/ohos-nfcManager.json` | CLI configuration file |
| `tools/ohos-nfcManager/docs/README.md` | This document |
| `tools/ohos-nfcManager/docs/USAGE.md` | Usage guide |

## Dependencies

```gn
deps = [
    "//foundation/communication/nfc/interfaces/inner_api/controller:nfc_inner_kits_controller",
]

external_deps = [
    "c_utils:utils",
    "hilog:libhilog",
    "ipc:ipc_single",
    "samgr:samgr_proxy",
]
```