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
├── interfaces                        # 接口代码
│   └── kits
│       └── native_cpp                # 本地SDK库
│           └── connected_tag_base    # NFC有源标签SDK实现
│           └── napi                  # native api
│               └── connected_tag     # NFC有源标签native api
└── sa_profile           # 服务属性声明
│   └── connected_tag    # NFC有源标签服务属性声明
└── services             # 子系统服务代码
    └── connected_tag    # NFC有源标签服务
        ├── etc       # 系统服务配置
        ├── include   # 头文件
        └── src       # 源文件
```

## Constraints<a name="section119744591305"></a>

-   Devices must has the connected tag chip.

## Usage<a name="section1312121216216"></a>



## Repositories Involved<a name="section1371113476307"></a>

hmf/communication/nfc

