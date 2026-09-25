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

/** @addtogroup MFR MFR Module
 *  @{
 */
/** @addtogroup MFR_HAL MFR HAL
 *  @{
 * @par Application API Specification
 * MFR HAL provides an interface for image flashing operations
 */

/** @defgroup PLAT_MFR_HAL PLAT MFR HAL
 *  @{
 */

/**
 * @file mfrhal.h
 *
 * @brief MFR HAL image flashing API
 *
 * This file defines APIs and data types used for image flashing operations.
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
 * - DRI: Disaster recovery image
 * - PCI: Peripheral component interconnect
 *
 * @par Implementation Notes
 * -# None
 *
 */

#ifndef __MFRHAL_H__
#define __MFRHAL_H__
/*---------------------------------------------------------------------------
** Includes
*/
#include "mfrTypes.h"
#include "xsign.h"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/* TODO-The flash parameters and configuration, should come from the linux .dtb file -- tracked by redmine ticket #11704*/
#define DEVICEPARTITION                    "/dev/mmcblk0"
#define PROCCMDLINEFILE                    "/proc/cmdline"
#define WATCHDOG_DEV                       "/dev/watchdog"
#define SKY_OTP_DEV            	           "/dev/skyotp"
#define READ_OTP_FIELD                     _IOWR('x',0x01,OtpParams_t)

#define COLOR_NONE                         "\033[0m"
#define COLOR_RED                          "\033[31m"
#define COLOR_MAGENTA                      "\033[35m"
#define MFR_WRN(fmt, ...)                  printf(COLOR_MAGENTA "[WARNING]%s:[%d] " fmt COLOR_NONE "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MFR_ERR(fmt, ...)                  printf(COLOR_RED "[ERROR]%s:[%d] " fmt COLOR_NONE "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define MFR_DBG(fmt, ...)                  printf(COLOR_NONE "[DEBUG]%s:[%d] " fmt COLOR_NONE "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

/* common defines for all CAD versions */
#ifndef TRUE
#define TRUE                               	(1)
#endif

#ifndef FALSE
#define FALSE                              	(0)
#endif

#define MAXLENGTH                          	(512)
#define MFR_MAX_FILENAME_LENGTH             (512)
#define INNER_FILE_SUFFIX                   "_inner"
#define XI_BOOT_AMGR_FLASHMAP_BLDATA_SIZE  	(512)
#define MFR_FLASHMAP_ERASING_BLOCK_SIZE     (128)
#define BLOCKSIZE                          	(10*1048576) //write in blocks of 10 MB
#define HEADER_IDENTIFIER                  "package header"
#ifndef DEFAULT_RETRY_COUNT
#define DEFAULT_RETRY_COUNT		   10
#endif
#define DEFAULT_ALL_IMAGE_INVALID_RETRY_COUNT		   2

#define INVALID_RETRY_COUNT		   0
#define NUM_DATA_OFFSETS	2
#define DEFAULT_FORCED_DRI_VALUE	0

#define MFR_SHA256_LENGTH 32

#define BL_MAIN_BANK                0
#define BL_BKUP_BANK                1
#ifndef DOWNLOAD_PREFIX
#define DOWNLOAD_PREFIX                    ("/opt/CDL")
#endif
/*---------------------------------------------------------------------------
** Typedefs
*/
typedef struct {
    char     rawdevname[MAXLENGTH];
    int      Fd;
    off_t    start_address;
    off_t    size;
    size_t   block_size;
    pthread_mutex_t lock;
}FlashDevice_t;

typedef struct typeTable
{
    /* data */
    mfrSerializedType_t type;
    unsigned char *pData;
    unsigned long size;
}typeTable_t;

/* include platform specific header */
#include "platform_hardware.h"

/* more common defines */
#define BL_DRI_NVDATA_SIG_START		   (0xAAAAAAAA)
#define BL_DRI_NVDATA_SIG_END		   (0x55555555)

typedef struct headerInfoFields{
    unsigned int  year;
    unsigned int  month;
    unsigned int  day;
    unsigned int  hour;
    unsigned int  minute;
    unsigned int  second;
    unsigned long long  image_size;
    unsigned long long  kernel_size;
    unsigned long long  rootfs_size;
}headerInfoFields_t;

typedef enum _mfrCurrentImage_t{
   mfrIMAGE_PCI,
   mfrIMAGE_PCI1,
   mfrIMAGE_PCI2,
   mfrIMAGE_PDRI,
   mfrIMAGE_BDRI,
   mfrIMAGE_MAX
}mfrCurrentImage_t;

/*
 * This structure contains the params for the write
 * to flash detachable thread
 */
typedef struct writeThreadParams
{
	char FilName[MAXLENGTH];
}writeThreadParams_t;

typedef struct __attribute__((__packed__)) bl_runtime_share_nvdata
{
    unsigned int    bl_nvdata_sig_start;
    unsigned int    pci1_retrycount;
    unsigned int    pci2_retrycount;
    unsigned int    pdri_retrycount;
    unsigned int    bdri_retrycount;
    unsigned int    all_invalid_retrycount;
    unsigned char   forcedDRI;
    unsigned char   rsvdfiled[451];
    unsigned char   sha_256[MFR_SHA256_LENGTH];
    unsigned int    bl_nvdata_sig_end;

} bl_runtime_share_nvdata_t;

/*---------------------------------------------------------------------------
** Data
*/
/*---------------------------------------------------------------------------
** Functions
*/
/**
 * @brief Validates the device model.
 *
 * @param [in] model Device model to validate.
 *
 * @return mfrError_t Status of the validation.
 */
mfrError_t mfrValidateImageModel(char *model);

/**
 * @brief Reads image header information from flash.
 *
 * @param [in] offset Flash offset containing the image header.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrReadHeaderFromFlash(off_t offset);

/**
 * @brief Retrieves the currently selected image and its name.
 *
 * @param [out] pImageName Current image type.
 * @param [out] name Buffer receiving the image name.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrReturnCurrentImage(mfrCurrentImage_t *pImageName, char *name);

/**
 * @brief Calculates and checks the CRC of a buffer.
 *
 * @param [out] crc_out Calculated CRC value.
 * @param [in] buf Buffer containing the data.
 * @param [in] size Size of the buffer in bytes.
 *
 * @return mfrError_t Status of the CRC check.
 */
mfrError_t mfrCRCCheck(unsigned int * crc_out, const unsigned char* buf, int size);

/**
 * @brief Retrieves the flash area and partition size for an image.
 *
 * @param [in] runtimeImage Currently running image.
 * @param [in] downloadedImage Image type being downloaded.
 * @param [out] offset Flash offset for the target area.
 * @param [out] partitionSize Size of the target partition.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrGetAreaToFlashDetails(mfrCurrentImage_t runtimeImage, 
					   XSign_ImageType_t downloadedImage, 
					   off_t * offset, unsigned long * partitionSize);

/**
 * @brief Reports image flashing progress.
 *
 * @param [in] error Current error status.
 * @param [in] progress Current upgrade progress state.
 * @param [in] percentage Completion percentage.
 * @param [in] badimage_codes Bad image error code, if applicable.
 */
void notifyStatus(mfrError_t error, mfrUpgradeProgress_t progress, int percentage,
			 int badimage_codes);

/**
 * @brief Calculates the SHA-256 digest of a buffer.
 *
 * @param [in] input Buffer containing the data.
 * @param [in] size Size of the input in bytes.
 * @param [out] calc_sha Buffer receiving the 32-byte digest.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrSHA256(void* input, unsigned int size, unsigned char *calc_sha);

/**
 * @brief Disables the watchdog timer.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrDisableWatchdogTimer(void);

/**
 * @brief Opens a flash device or partition.
 *
 * @param [in] filename Flash device or partition name.
 * @param [out] Fd File descriptor for the opened device.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrOpenFlash(char * filename, int * Fd);

/**
 * @brief Closes an open flash device.
 *
 * @param [in,out] pFd File descriptor to close.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrCloseFlash(int * pFd);

/**
 * @brief Resets the retry count for the image at a flash offset.
 *
 * @param [in] offset Flash offset of the image.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrResetImageRetryCount(off_t offset);

/**
 * @brief Retrieves forced DRI data.
 *
 * @param [out] data Buffer receiving the forced DRI value.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrGetForcedDriData(unsigned char *data);

/**
 * @brief Restores the system image and configuration.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrSystemRestore(void);

/**
 * @brief Gets the system restore flag.
 *
 * @return true when system restore is enabled; otherwise false.
 */
bool mfrGetSystemRestoreFlag(void);

/**
 * @brief Erases a range of flash memory.
 *
 * @param [in] offset Starting flash offset.
 * @param [in] count Number of bytes to erase.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrEraseFlash(off_t offset, size_t count);

/**
 * @brief Resets retry counts for all images.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrResetAllImageRetryCount();

/**
 * @brief Reads bootloader runtime data.
 *
 * @param [out] bldata Buffer receiving the bootloader runtime data.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrReadBlData(bl_runtime_share_nvdata_t *bldata);

/**
 * @brief Erases the PCI image.
 *
 * @return mfrError_t Status of the operation.
 */
mfrError_t mfrErasePCI(void);

/**
 * @brief Prints a SHA-256 digest for diagnostic purposes.
 *
 * @param [in] run_calculated Selects whether the calculated digest is printed.
 * @param [in] sha256 SHA-256 digest to print.
 */
void printSHA256(bool run_calculated, unsigned char *sha256);

#endif /* mfrhal_h */
/*---------------------------------------------------------------------------
** End of File
*/
