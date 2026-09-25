/**
*  If not stated otherwise in this file or this component's LICENSE
*  file the following copyright and licenses apply:
*
*  Copyright 2026 RDK Management
*
*  Licensed under the Apache License, Version 2.0 (the License);
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an AS IS BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
/**
 * @addtogroup HPK Hardware Porting Kit
 * @{
 * @par The Hardware Porting Kit
 * HPK is the next evolution of the well-defined Hardware Abstraction Layer
 * (HAL), but augmented with more comprehensive documentation and test suites
 * that OEM or SOC vendors can use to self-certify their ports before taking
 * them to RDKM for validation or to an operator for final integration and
 * deployment. The Hardware Porting Kit effectively enables an OEM and/or SOC
 * vendor to self-certify their own Video Accelerator devices, with minimal RDKM
 * assistance
 *
 */
/** @defgroup MFR MFR Module
 *  @{
 */
/** @defgroup MFR_HAL MFR HAL
 *  @{
 * @par Application API Specification
 * MFR HAL provides an interface for reading and writing device serialization information and doing image flashing operations
 */

/** @defgroup PLAT_MFR_DATA PLAT MFR DATA
 *  @{
 */

/**
 * @file mfr.h
 * 
 * @brief MFR HAL header
 *
 * This file defines APIs, datatypes and error codes used by the MFR HAL
 *
 * @par Document
 * Document reference.
 *
 * @par Open Issues (in no particular order)
 * -# None
 *
 * @par Assumptions
 * -# None
 *
 * @par Abbreviations
 * - MFR:      Manufacturer library
 * - HDMI:     High-Definition multimedia Interface
 * - HDCP:     High-Bandwidth digital content protection
 * - MOCA:     Multimedia over coax alliance
 * - auth:     Authentication
 * - DTCP:     Digital transmission content protection
 * - CDL:      Code download
 * - RCDL:     Remote code download
 * - CA:       Certificate authority
 * - DVR:      Digital video recording
 * - SVN:      Software version number
 * - CRC:      Cyclic redundancy check
 * - oui:      Organizationally unique identifier
 * - DRI:      Disaster recovery image
 * - PDRI:     Peripheral disaster recovery image
 * - WIFI:     Wireless fidelity
 * - MAC:      Media access control address
 * - RF4CE:    Radio frequency for consumer electronics
 * - DTB:      Device tree binary
 * - PMI:      Product manufacturer information
 * - SOC:      System on chip
 * - TV:       Television
 * - BDRI:     Backup disaster recovery image
 * - CPD:      Critical panel data
 * - WB:       White balancing
 * - ALS:      Ambient light sensor
 * - LUX:      Unit of luminance or illumination of a one metre square area
 * - PCI:      Peripheral component interconnect
 * - AV:       Audio video
 * - TPV:      TPV technology limited
 * - FTA:      Factory test app
 * - WPS:      Wi-Fi protected setup
 */


#ifndef _MFR_H
#define _MFR_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/**
 * @brief Initializes the MFR library
 *
 * This function will initialize all the respective internal components responsible for MFR functionalities.
 * This API need to be called before any other APIs in this module@n
 * 
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_ALREADY_INITIALIZED      - Module is already initialised
 * @retval mfrERR_MEMORY_EXHAUSTED         - memory allocation failure 
 * 
 * @warning  This API is Not thread safe
 * 
 */
mfrError_t mfr_init( void );

/**
 * @brief Initialize the mfr partition.
 *
 * mfr_init invokes this mfr_partition_init.
 * This function should be call once before accessing serialized data.
 *
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_ALREADY_INITIALIZED      - Module is already initialised
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * 
 *
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED. 
*/
mfrError_t mfr_partition_init(void);

/**
 * @brief Uninitializes the MFR library
 *
 * This function will uninitialize all the respective internal components responsible for MFR functionalities.
 * 
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * 
 * @warning  This API is Not thread safe
 * 
 */
mfrError_t mfr_term( void );

/**
 * @brief Retrieves serialized Read-Only data from device
 * 
 * 
 * @param [in] type :  specifies the serialized data type to be read. @see mfrSerializedType_t
 * @param [in] data :  serialized data for the specific type requested. (buffer location, length, and func to free the buffer). @see mfrSerializedData_t
 *
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_INVALID_PARAM            - Parameter passed to this function is invalid
 * @retval mfrERR_MEMORY_EXHAUSTED         - memory allocation failure
 * @retval mfrERR_FAILED_CRC_CHECK         - CRC check failed
 * @retval mfrERR_FLASH_READ_FAILED        - Flash read failed
 * 
 * @note The serialized data is returned as a byte stream. It is upto the  application to deserialize and make sense of the data returned.
 *  Even if the serialized data returned is "string", the buffer is not required to contain the null-terminator
 * 
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED. 
 * @warning  This API is Not thread safe
 * 
 */
mfrError_t mfrGetSerializedData( mfrSerializedType_t type,  mfrSerializedData_t *data );

/**
 * @brief Sets the read write Serialization data on device
 *
 * @param [in] type :  specifies the serialized data type to write. @see mfrSerializedType_t
 * @param [in] data :  serialized data to set for the specific type requested. (buffer location, length, and func to free the buffer). @see mfrSerializedData_t
 *
 * @return mfrError_t                       - Status
 * @retval mfrERR_NONE                      - Success
 * @retval mfrERR_NOT_INITIALIZED           - Module is not initialised
 * @retval mfrERR_INVALID_PARAM             - Parameter passed to this function is invalid
 * @retval mfrERR_MEMORY_EXHAUSTED          - memory allocation failure
 * @retval mfrERR_FAILED_CRC_CHECK          - CRC check failed
 * @retval mfrERR_WRITE_FLASH_FAILED        - Flash write failed
 * @retval mfrERR_FLASH_READ_FAILED        - Flash read failed
 * @retval mfrERR_FLASH_VERIFY_FAILED       - Flash verification failed
 * 
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED. 
 * @warning  This API is Not thread safe
 * 
 */
mfrError_t mfrSetSerializedData( mfrSerializedType_t type,  mfrSerializedData_t *data);

/**
 * @brief Writes the image into flash
 * 
 *    The process should follow these major steps:
 *    1) Verify the validity of the image and flash 
 *    2) Update boot params and switch banks to prepare for a reboot event
 *    3) All upgrades should be done in the alternate bank. The current bank should not be disturbed
 *
 *    State Transition:
 *    0) Before the API is invoked, the Upgrade process should be in PROGRESS_NOT_STARTED state
 *    1) After the API returns with success, the Upgrade process moves to PROGRESS_STARTED state
 *    2) After the API returns with error,   the Upgrade process stays in PROGRESS_NOT_STARTED state. Notify function will not be invoked
 *    3) The notify function is called at regular interval with process = PROGRESS_STARTED
 *    4) The last invocation of notify function should have either progress = PROGRESS_COMPLETED or progress = PROGRESS_ABORTED with error code set
 *
 *  @note mfrWriteImage() should work without any issue when device transition to DEEPSLEEP state and Wakeup. During DEEPSLEEP state processor will
 * cache all the pc and stack state and will enter to low power state. On wakeup system will use the saved pc and stack and resume from the same point.
 *   
 * @param [in] name :  the filename of the image file
 * @param [in] path :  the path of the image file in the file system
 * @param [in] type :  the type (format, signature type) of the image.  This can dictate the handling of the image within the MFR library. @see mfrImageType_t
 * @param[in] notify: function to provide status of the image flashing process.  @see mfrUpgradeStatusNotify_t
 * 
 * 
 * @return mfrError_t                              - Status
 * 
 * @retval mfrERR_NONE                             - Success
 * @retval mfrERR_NOT_INITIALIZED                  - Module is not initialised
 * @retval mfrERR_INVALID_PARAM                    - Parameter passed to this function is invalid
 * @retval mfrERR_MEMORY_EXHAUSTED                 - memory allocation failure
 * @retval mfrERR_FAILED_CRC_CHECK                 - CRC is failed
 * @retval mfrERR_WRITE_FLASH_FAILED               - Flash write failed
 * @retval mfrERR_FLASH_VERIFY_FAILED              - Flash verification failed
 * @retval mfrERR_BAD_IMAGE_HEADER                 - Image header is corrupted
 * @retval mfrERR_IMPROPER_SIGNATURE               - Image signature is invalid
 * @retval mfrERR_IMAGE_TOO_BIG                    - Image size is more than allocated maximum
 * @retval mfrERR_FAILED_INVALID_SIGNING_TIME      - Image signing time invalid
 * @retval mfrERR_FAILED_IMAGE_SVN_OLDER           - software version number is older than existing image
 * @retval mfrERR_FAILED_SAME_DRI_CODE_VERSION     - DRI code version is same
 * @retval mfrERR_FAILED_SAME_PCI_CODE_VERSION     - PCI code version is same
 * @retval mfrERR_IMAGE_FILE_OPEN_FAILED           - Not able to open the input image file
 * @retval mfrERR_GET_FLASHED_IMAGE_DETAILS_FAILED - Not able to get the current image version details
 * 
 * 
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED. .
 * @warning  This API is Not thread safe
 * 
 */
mfrError_t mfrWriteImage(const char *name,  const char *path, mfrImageType_t type,  mfrUpgradeStatusNotify_t notify);

/**
 * @brief Deletes the PDRI image if it is present
 * 
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_WRITE_FLASH_FAILED       - Flash write failed
 * @retval mfrERR_FLASH_VERIFY_FAILED      - Flash verification failed
 * 
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED. 
 * @warning  This API is Not thread safe
 * 
 */
mfrError_t mfrDeletePDRI(void);

/**
 * @brief Deletes the platform images. Deletes the main image from primary and secondary bank
 * 
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_WRITE_FLASH_FAILED       - Flash write failed
 * @retval mfrERR_FLASH_VERIFY_FAILED      - Flash verification failed
 * 
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED. 
 * @warning  This API is Not thread safe
 * 
 */
mfrError_t mfrScrubAllBanks(void);

/**
 * @brief Sets bootloader LED pattern
 * 
 * This function stores the bootup pattern in the persistance storage for bootloader to read 
 * and control the front panel LED and/or TV backlight sequence on bootup
 * 
 * @param [in] pattern : options are defined by enum mfrBlPattern_t. @see mfrBlPattern_t
 * 
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_INVALID_PARAM            - Parameter passed to this function is invalid
 * @retval mfrERR_WRITE_FLASH_FAILED       - Flash write failed
 * @retval mfrERR_FLASH_VERIFY_FAILED      - Flash verification failed
 * 
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED. 
 * @warning  This API is Not thread safe
 * 
 */
mfrError_t mfrSetBootloaderPattern(mfrBlPattern_t pattern);

/**
 * @brief API to update Primary Splash screen Image and to override the default the Splash screen image
 *
 * @param [in] path : char pointer which holds the path of input bootloader OSD image.
 *
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_INVALID_PARAM            - Parameter passed to this function is invalid
 * @retval mfrERR_IMAGE_FILE_OPEN_FAILED   - Failed to open the downloaded splash screen file
 * @retval mfrERR_MEMORY_EXHAUSTED         - memory allocation failure
 *
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED. 
 * @warning  This API is Not thread safe
 *
 */
mfrError_t mfrSetBlSplashScreen(const char *path);

/**
 * @brief API to clear the primary Splash screen Image and to make
 * use of default Splash screen image
 *
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_IMAGE_FILE_OPEN_FAILED   - Failed to open the downloaded splash screen file
 * @retval mfrERR_MEMORY_EXHAUSTED         - memory allocation failure
 *
 * @pre  mfr_init() should be called before calling this API. If this precondition is not met, the API will return mfrERR_NOT_INITIALIZED.
 * @warning  This API is Not thread safe
 *
 */
mfrError_t mfrClearBlSplashScreen(void);

/**
* @brief API to retrive the secure time from TEE
*
* @param [in] params : unit32 timeptr to get the UTC time in seconds
*
* @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
*/
mfrError_t mfrGetSecureTime(uint32_t *timeptr);

/**
* @brief API to set the secure time from TEE
*
* @param [in] params : unit32 timeptr to set the UTC time in seconds
*
* @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
*/
mfrError_t mfrSetSecureTime(uint32_t *timeptr);


/**
 * @brief API to set the fsr flag into the emmc raw area
 *
 * @param [in] params : uint16_t fsrflag to set the FSR flag
 *
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @retval mfrERR_INVALID_PARAM            - Parameter passed to this function is invalid
 * @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
 *
 **/
mfrError_t mfrSetFSRflag(uint16_t *newFsrFlag);

/**
 * @brief API to get the fsr flag from emmc
 *
 * @param [in] params : uint16_t fsrflag to get the FSR flag
 *
 * @return mfrError_t                      - Status
 * @retval mfrERR_NONE                     - Success
 * @retval mfrERR_INVALID_PARAM            - Parameter passed to this function is invalid
 * @retval mfrERR_NOT_INITIALIZED          - Module is not initialised
 * @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
 *
 **/
mfrError_t mfrGetFSRflag(uint16_t *newFsrFlag);

/**
* @brief API to retrive the secure time from TEE
*
* @param [in] params : unit32 timeptr to get the UTC time in seconds
*
* @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
*/
mfrError_t mfrGetSecureTime(uint32_t *timeptr);

/**
* @brief API to set the secure time from TEE
*
* @param [in] params : unit32 timeptr to set the UTC time in seconds
*
* @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
*/
mfrError_t mfrSetSecureTime(uint32_t *timeptr);


/**
* @brief API to set the fsr flag into the emmc raw area
*
* @param [in] params : unit32 fsrflag to set the FSR flag
*
* @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
*/
mfrError_t mfrSetFSRflag(uint16_t *newFsrFlag);


/**
* @brief API to get the fsr flag from emmc
*
* @param [in] params : unit32 fsrflag to get the FSR flag
*
* @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
*/
mfrError_t mfrGetFSRflag(uint16_t *newFsrFlag);

/**
* @brief API to set the blocklist data to BL runtime data block of emmc
*
* @param [in] params : unsigned integer pointer with the blocklist value
*
* @return Error Code:  Return mfrERR_NONE if operation is successful, mfrERR_GENERAL if it fails
*/
mfrError_t mfr_setConfigData(unsigned int blocklist);


/**
 * @brief API to get blocklist data from the BL runtime data block in eMMC.
 *
 * @details
 * This API expects a pointer to a structure of type
 * IARM_Bus_MFRLib_Platformblockdata_Param_t, which contains the blocklist
 * value. Although the function parameter is declared as 'unsigned int *' for
 * backward compatibility, the caller actually provides a pointer to the
 * platform-block-data structure, and the API writes the blocklist value into
 * its 'blocklist' field.

*
 * @param[in,out] blocklist
 *     Pointer used to carry the blocklist output value.
 *     In practice, this corresponds to &params->blocklist from the
 *     IARM_Bus_MFRLib_Platformblockdata_Param_t structure.
 *
 * @return mfrError_t
 *     - mfrERR_NONE     : Operation successful
 *     - mfrERR_GENERAL  : Operation failed
 */
mfrError_t mfr_getConfigData(unsigned int *blocklist);

#endif

/** @} */ // End of PLAT_MFR_DATA
/** @} */ // End of MFR_HAL
/** @} */ // End of MFR Module
/** @} */ // End of HPK