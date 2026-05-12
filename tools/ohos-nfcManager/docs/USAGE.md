# ohos-nfcManager Usage Guide

## Overview

`ohos-nfcManager` is a command-line tool for querying and controlling device NFC function status.

## Syntax

```
ohos-nfcManager <action>
```

## Actions

### get-state

Get current NFC state.

```
ohos-nfcManager get-state
```

**Output:**
```json
{"type":"result","status":"success","data":{"state":"on","code":3}}
```

### turn-on

Turn on NFC function.

```
ohos-nfcManager turn-on
```

**Output:**
```json
{"type":"result","status":"success","data":{"status":"turning_on","message":"NFC is turning on"}}
```

### turn-off

Turn off NFC function.

```
ohos-nfcManager turn-off
```

**Output:**
```json
{"type":"result","status":"success","data":{"status":"turning_off","message":"NFC is turning off"}}
```

### is-available

Check if the device supports NFC.

```
ohos-nfcManager is-available
```

**Output:**
```json
{"type":"result","status":"success","data":{"available":true}}
```

## Error Handling

All commands return JSON with error information on failure:

```json
{"type":"result","status":"failed","errCode":"E_NFC_SA_UNAVAILABLE","errMsg":"NFC service unavailable","suggestion":"Check if NFC service is running"}
```

### Common Error Codes

| Code | Description |
|------|-------------|
| E_NFC_SA_UNAVAILABLE | NFC service unavailable |
| E_NFC_OPERATION_FAILED | NFC operation failed |
| E_INVALID_ACTION | Invalid action parameter |
| E_TIMEOUT | Operation timeout (15 seconds) |

## Help

```
ohos-nfcManager --help
```

or

```
ohos-nfcManager -h
```

or

```
ohos-nfcManager help
```

## Notes

- NFC service must be running
- 15 second timeout protection for all operations
- All operations are idempotent