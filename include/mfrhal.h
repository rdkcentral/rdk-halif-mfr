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
mfrError_t mfrReadfromFlash(unsigned int srcAddr,  unsigned char * destAddr, unsigned int size);
mfrError_t mfrValidateImageModel(char *model);
mfrError_t mfrReadHeaderFromFlash(off_t offset);
mfrError_t mfrReturnCurrentImage(mfrCurrentImage_t *pImageName, char *name);
mfrError_t mfrCRCCheck(unsigned int * crc_out, const unsigned char* buf, int size);
mfrError_t mfrGetAreaToFlashDetails(mfrCurrentImage_t runtimeImage, 
					   XSign_ImageType_t downloadedImage, 
					   off_t * offset, unsigned long * partitionSize);
void notifyStatus(mfrError_t error, mfrUpgradeProgress_t progress, int percentage,
			 int badimage_codes);
mfrError_t mfrSHA256(void* input, unsigned int size, unsigned char *calc_sha);
mfrError_t mfrDisableWatchdogTimer(void);
mfrError_t mfrOpenFlash(char * filename, int * Fd);
mfrError_t mfrCloseFlash(int * pFd);
mfrError_t mfrResetImageRetryCount(off_t offset);
mfrError_t mfrGetForcedDriData(unsigned char *data);
mfrError_t mfrSystemRestore(void);
bool mfrGetSystemRestoreFlag(void);
mfrError_t mfrEraseFlash(off_t offset, size_t count);
mfrError_t mfrResetAllImageRetryCount();
mfrError_t mfrResetPCIImageRetryCount(void);
mfrError_t mfrReadBlData(bl_runtime_share_nvdata_t *bldata);
mfrError_t mfrErasePCI(void);
void printSHA256(bool run_calculated, unsigned char *sha256);
mfrError_t mfrInvalidateAllPCIBanks(void);

#endif /* mfrhal_h */
/*---------------------------------------------------------------------------
** End of File
*/
