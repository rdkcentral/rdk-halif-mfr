# MFR HAL Documentation

## Table of Contents

- [Overview](#overview)
- [Architectural Context](#architectural-context)
- [Scope](#scope)
- [Vendor-Layer Responsibilities](#vendor-layer-responsibilities)
- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Description](#description)
- [Component Runtime Execution Requirements](#component-runtime-execution-requirements)
  - [Initialization and Startup](#initialization-and-startup)
  - [Threading Model](#threading-model)
  - [Process Model](#process-model)
  - [Memory Model](#memory-model)
  - [Power Management Requirements](#power-management-requirements)
  - [Asynchronous Notification Model](#asynchronous-notification-model)
  - [Blocking calls](#blocking-calls)
  - [Internal Error Handling](#internal-error-handling)
  - [Persistence Model](#persistence-model)
- [Non-functional requirements](#non-functional-requirements)
  - [Logging and debugging requirements](#logging-and-debugging-requirements)
  - [Memory and performance requirements](#memory-and-performance-requirements)
  - [Quality Control](#quality-control)
  - [Licensing](#licensing)
  - [Build Requirements](#build-requirements)
  - [Variability Management](#variability-management)
  - [Platform or Product Customization](#platform-or-product-customization)
- [Interface API Documentation](#interface-api-documentation)
  - [Theory of operation and key concepts](#theory-of-operation-and-key-concepts)
  - [Diagrams](#diagrams)

## Overview

MFR is the manufacturer-data interface between platform middleware and device-specific information provided by the vendor. It provides access to device identity, product metadata, boot and image information, secure and non-secure persistent data, temperature thresholds, and Wi-Fi credentials where supported by the product.

This document defines the lifecycle, runtime, integration, and quality requirements for a vendor MFR implementation. It does not prescribe a storage technology or product-specific data layout.

## Architectural Context

The MFR HAL provides the integration boundary between its caller and the platform-specific MFR SoC driver. The HAL translates the common MFR API contract into the device-specific storage, flash, temperature, and Wi-Fi operations required by the product.

The caller remains independent of device-specific storage layout and hardware details. The vendor integration owns those assumptions and must expose only capabilities supported by the active product variant.

## Scope

This specification covers lifecycle management, serialized manufacturer data, persistent secure and non-secure data, image operations, firmware-upgrade status, temperature queries and thresholds, and optional Wi-Fi credential operations.

The implementation must support the common MFR API contract and return the documented unsupported-operation status when a capability is not available on the underlying hardware.

## Vendor-Layer Responsibilities

The vendor MFR implementation shall:

 - Provide the common MFR API symbols and ABI required by the product.
 - Initialize the storage, flash, and hardware dependencies needed by supported MFR operations.
 - Return product identity, metadata, image status, temperature data, and credentials from the device-owned source appropriate to the active product variant.
 - Preserve the confidentiality and integrity of secure NVRAM data, credentials, and other sensitive manufacturer information.
 - Report initialization, storage, validation, and query failures through the documented error codes.
 - Release resources created for asynchronous operations after those operations complete.

## Acronyms, Terms and Abbreviations

- `HAL`    - Hardware Abstraction Layer
- `API`    - Application Programming Interface
- `Caller` - Any user of the interface via the `APIs`
- `NVRAM`  - Non-volatile random-access memory
- `DTCP`   - Digital Transmission Content Protection
- `MFR`    - Manufacturer library
- `PDRI`   - Peripheral disaster recovery image
- `WIFI`   - Wireless Fidelity


## Description
The diagram below describes a high-level software architecture of the module stack.

```mermaid
%%{ init : { "theme" : "forest", "flowchart" : { "curve" : "linear" }}}%%
flowchart TD
y[caller]<-->x[MFR HAL];
x[MFR HAL]<-->z[MFR SOC Driver];
style y fill:#99CCFF,stroke:#333,stroke-width:0.3px,align:left
style z fill:#fcc,stroke:#333,stroke-width:0.3px,align:left
style x fill:#9f9,stroke:#333,stroke-width:0.3px,align:left
 ```

`MFR` interface provides `APIs` that can interface with other interfaces in order to persist the data required for their operational needs. These `APIs` are used by various system components which are not limited to the below ones and can be persisted through the interfaces provided in `MFR`.

`MFR` can provide following major functionalities:
 - Read/write serialization data including manufacturer, model name, product class, hardware/software versions, etc.
 - Read/write data from/to secure `NVRAM`, including `DTCP` certificates.
 - Read/write data from/to non-secure `NVRAM`, including boot image information.
 - Flash memory operations for `STB` image upgrade this includes capability to clear and update the respective flash area
 - Perform a `CRC` check after flashing the image to the `NVRAM` partition and write the `CRC` to a partition in `NVRAM`
 - Get current firmware upgrade status.
 - Measure the cabinet temperature data.
 - Set and get the temperature thresholds.
 - Read, write and erase `WIFI` credentials.

The `HAL` implementation is largely specific to the device/`OEM`.

This interface should be scalable, extensible and maintainable.

## Component Runtime Execution Requirements

This interface should manage system resources appropriately to avoid memory leaks and excessive resource utilization. Efficient memory management and resource cleanup is essential for stable and reliable execution. Additionally, it should meet specified performance requirements, including response time, throughput, and resource usage based on the underlying platform's capabilities. The interface should also be designed to scale effectively with increased load, being able to handle higher levels of usage without significant degradation in performance or stability.

Failure to meet these requirements will likely result in undefined and unexpected behaviour.

### Initialization and Startup

The `caller` is expected to have complete control over the life cycle of the `MFR` interface. The `caller` should initialize this interface by calling `mfr_init()` before invoking any other `API`. This `API` will initialize and configure the respective hardware. If the component or sub-system is not ready and cannot respond within a reasonable time, the `HAL` should return the corresponding error code. The `caller` is expected to handle the error codes returned from the HAL.


### Threading Model

This interface is not required to be thread safe. `HAL` is allowed to create internal threads for its operation without excessively consuming system resources. Any signal handling scenarios should be handled gracefully and respective errors codes should be returned if any corresponding `API` fails.

### Process Model

This interface is required to support a single instantiation with a single process.

### Memory Model

For read and write operations, `caller` is responsible for memory management. For call back operations, `HAL` is responsible for memory management.

### Power Management Requirements

Although this interface is not required to be involved in any of the power management operations, the state transitions MUST not affect its operation. e.g. on resumption from a low power state (Deepsleep), the interface should operate as if no transition has occurred. Any flash operation which takes longer cycles should be handled properly during the deepsleep initiation.


### Asynchronous Notification Model

 - This interface should support asynchronous notifications for image write operations.
 - `mfrWriteImage()` should facilitate asynchronous status notifications using the callback `mfrUpgradeStatusNotify_t`.
 - This interface is allowed to establish its own thread context for its operation, ensuring minimal impact on system resources.
 - Additionally, this interface is responsible for releasing the resources it creates for its operation once the respective operation concludes.

### Blocking calls

There are no blocking calls. Any synchronous call that can fail due to the lack of response from the connected device, should have a reasonable timeout period.

### Internal Error Handling

All the `APIs` should return errors synchronously as a return argument. `HAL` is responsible for handling system errors (e.g. out of memory) internally.

### Persistence Model

This interface holds the responsibility for persistently storing different serialized data as specified within the `mfrSerializedType_t` structure, `WIFI_DATA`, and temperature thresholds. Additionally, it facilitates the storage of bootable full stack images within their respective partitions. The related `APIs` enable reading and writing to respective configurations. The task of resetting configurations to their defaults will be handled by the `caller`.

## Non-functional requirements

Following non-functional requirements will be supported by the interface:

### Logging and debugging requirements

This interface is required to support DEBUG, INFO and ERROR messages. DEBUG and INFO is required to be disabled by default and enabled when needed.

### Memory and performance requirements

This interface will ensure optimal use of memory and CPU according to the specific capabilities of the system.

### Quality Control

- The interface will be expected to pass static analysis, our preferred tool is `Coverity`.
- Have a zero-warning policy with regards to compiling. All warnings are required to be treated as errors.
- Copyright validation is required to be performed, e.g.: `Black duck`, `FossID`.
- Use of memory analysis tools like `Valgrind` are encouraged, to identify leaks/corruptions.
- `HAL` Tests will endeavour to create worst case scenarios to assist investigations.
- Improvements by any party to the testing suite are required to be fed back.

### Licensing

This interface is expected to be released under the Apache License 2.0.

### Build Requirements

The source code should build into a shared library and should be named as `libRDKMfrLib.so`. The build mechanism should be independent of Yocto.

### Variability Management

 - Any changes in the `APIs` should be reviewed and approved by the component architects.
 - Any modification should support backward compatibility for the generic operations like image upgrade and downgrade
 - `MFR` should return the `mfrERR_OPERATION_NOT_SUPPORTED` error code, If any of the interface `APIs` are not supported by the underlying hardware
 - Providers of this interface should keep a well-defined version history for tracking alterations across diverse library versions, along with their corresponding verification results.


### Platform or Product Customization

Any potential platform specific customization opportunities need to be communicate well in advance to the respective architect team for the purpose of effective planning.


## Interface API Documentation

`API` documentation will be provided by Doxygen which will be generated from the header files.

### Theory of operation and key concepts

The interface is initialized by the `caller` who will have the complete control over the life cycle of this interface.

  - `mfr_init()` initializes all the internal components which `MFR` is responsible for before making any other `API` calls. If `mfr_init()` call fails, the `HAL` should return the respective error code, so that the `caller` can retry the operation.

  - For asynchronous image write operation use `mfrWriteImage()`

  - For read serialization data use `mfrGetSerializedData()`

  - For write serialization data use `mfrSetSerializedData()`

  - For deleting the P-DRI image use `mfrDeletePDRI()`

  - For deleting the platform image use `mfrScrubAllBanks()`

  - For retrieving the cabinet temperature use `mfrGetTemperature()`

  - For setting the temperature threshold use `mfrSetTempThresholds()`

  - For reading the temperature threshold use `mfrGetTempThresholds()`

  - For writing the `WIFI` credentials use `WIFI_SetCredentials()`

  - For reading the `WIFI` credentials use `WIFI_GetCredentials()`

  - For erasing the existing `WIFI` credentials use `WIFI_EraseAllData ()`

  - For terminating the interface use `mfr_term()`


### Diagrams

#### Operational Call Sequence for MFR HAL

```mermaid
%%{ init : { "theme" : "default", "flowchart" : { "curve" : "stepBefore" }}}%%
   sequenceDiagram
    participant caller as caller
    participant HAL as MFR HAL
    caller->>HAL:mfr_init()
    Note over HAL: Module initialization
    HAL-->>caller:return
    caller->>HAL:mfr_read()
    Note over HAL: mfr_read operations like mfrGetSerializedData()
    HAL-->>caller:return
    caller->>HAL:mfr_write()
    Note over HAL: mfr_write operations like mfrSetSerializedData(), mfrWriteImage(), mfrDeletePDRI(),<br>mfrScrubAllBanks(), mfrSetGammaCalibrationData()
    HAL-->>caller:return
    caller->>HAL:mfrSetBootloaderPattern()
    Note over HAL: sets bootloader let pattern in the mfr persistance
    HAL-->>caller:return
    caller->>HAL:mfr_term()
    Note over HAL: Module uninitialization
    HAL-->>caller:return
 ```

#### Operational Call Sequence for `MFR WIFI` Functionalities

```mermaid
%%{ init : { "theme" : "default", "flowchart" : { "curve" : "stepBefore" }}}%%
   sequenceDiagram
    participant caller as Caller
    participant HAL as MFR HAL
    caller->>HAL:mfr_init()
    Note over HAL: Module initialisation
    HAL-->>caller:return
    caller->>HAL:WIFI_SetCredentials()
    Note over HAL: Set wifi credentials
    HAL-->>caller:return
    caller->>HAL:WIFI_GetCredentials()
    Note over HAL: Get wifi credentials
    HAL-->>caller:return
    caller->>HAL:WIFI_EraseAllData ()
    Note over HAL: Erase wifi credentials
    HAL-->>caller:return
    caller->>HAL:mfr_term()
    Note over HAL: Module uninitialization
    HAL-->>caller:return
```

 #### Operational Call Sequence for `MFR Temperature` Functionalities

```mermaid
%%{ init : { "theme" : "default", "flowchart" : { "curve" : "stepBefore" }}}%%
   sequenceDiagram
    participant caller as Caller
    participant HAL as MFR HAL
    caller->>HAL:mfr_init()
    Note over HAL: Module initialisation
    HAL-->>caller:return
    caller->>HAL:mfrGetTemperature()
    Note over HAL: Get current temperature of the core
    HAL-->>caller:return
    caller->>HAL:mfrSetTempThresholds()
    Note over HAL: Set the temperature thresholds
    HAL-->>caller:return
    caller->>HAL:mfrGetTempThresholds ()
    Note over HAL: Get the temperature thresholds
    HAL-->>caller:return
    caller->>HAL:mfr_term()
    Note over HAL: Module de-initialization
    HAL-->>caller:return
 ```
