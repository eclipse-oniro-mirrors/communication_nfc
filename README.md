# NFC<a name="EN-US_TOPIC_0000001133207781"></a>

-   [Introduction](#section13587125816351)
-   [Architecture](#section13587185873516)
-   [Directory Structure](#section161941989596)
-   [Constraints](#section119744591305)
-   [Usage](#section1312121216216)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section13587125816351"></a>

Near-field communication \(NFC\) is a non-contact identification and interconnection technology for short-distance wireless communication between mobile devices, consumer electronic products, PCs, and smart devices.

The NFC module provides connected tag reading and writing.

## Architecture<a name="section13587185873516"></a>

**Figure  1**  NFC architecture<a name="fig4460722185514"></a>  


![](figures/en-us_image_0000001086731550.gif)

## Directory Structure<a name="section161941989596"></a>

The main code directory structure of Intelligent Soft Bus is as follows:

```
/foundation/communication
├── interfaces                        # Interface code
│   └── kits
│       └── native_cpp                # Native SDK
│           └── connected_tag_base    # NFC connected tag SDK
│           └── napi                  # Native api
│               └── connected_tag     # Native api of NFC connected tag
└── sa_profile                        # Declare of sub system attribute
│   └── connected_tag                 # Declare of NFC connected tag attribute
└── services                          # Sub system service code folder
    └── connected_tag                 # NFC connected tag folder
        ├── etc                       # System service config
        ├── include                   # Include code
        └── src                       # Source code
```

## Constraints<a name="section119744591305"></a>

-   Devices must has the connected tag chip.

## Usage<a name="section1312121216216"></a>

-  connected tag reading and writing.

Devices must has the connected tag chip to connected tag reading and writing. Please reference "js-apis-connectedTag.md”。

## Repositories Involved<a name="section1371113476307"></a>

hmf/communication/nfc

