#include "diskio.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "ff.h"
#include "f_util.h"
#include "config/config.h"

#include "rtc/rtc.h"

#define SECTOR_SZ 512

static uint8_t* data = NULL;
static size_t   img_sz = 0;

void disk_image_initialize()
{
	// create disk image

	if (config.sd_image) {
		int fd = open(config.sd_image, O_RDWR, 0);
		if (!fd) {
			perror("Error opening SD image file");
			exit(EXIT_FAILURE);
		}

		struct stat sb;
		if (fstat(fd, &sb) == -1) {
			perror("fstat");
			exit(EXIT_FAILURE);
		}

		data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (data == MAP_FAILED) {
			perror("mmap");
			exit(EXIT_FAILURE);
		}

		img_sz = sb.st_size;

	} else {
		img_sz = config.sd_ram_sz * 1024 * 1024;
		data = malloc(img_sz);
	}

	// format disk

    if (config.sd_image == NULL || config.format) {
        BYTE work[FF_MAX_SS];
        FRESULT res = f_mkfs("", 0, work, sizeof work);
        if (res != FR_OK)
            printf("Error formatting SDCard: %s\n", FRESULT_str(res));
        else
            printf("SDCard formatted.\n");
    }

	// test disk

    FATFS fs;
    FRESULT res = f_mount(&fs, "", 1);
    if (res != FR_OK)
        printf("Error mounting SDCard: %s\n", FRESULT_str(res));

	// create a sample file

	FIL f;
	if (f_open(&f, "hello.txt", FA_CREATE_NEW | FA_WRITE) == FR_OK) {
		UINT bw;
		f_write(&f, "Hello world!\r\n", 14, &bw);
		f_close(&f);
	}

	// unmount

	f_mount(0, "", 0);
}

DSTATUS disk_initialize(BYTE pdrv)
{
	if (pdrv != 0)
        return STA_NOINIT;

	return RES_OK;
}

DSTATUS disk_status(BYTE pdrv)
{
	if (pdrv != 0)
        return STA_NOINIT;
	return RES_OK;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	if (pdrv != 0)
		return RES_PARERR;
	memcpy(buff, &data[sector * SECTOR_SZ], count * 512);
	return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	if (pdrv != 0)
		return RES_PARERR;
	memcpy(&data[sector * SECTOR_SZ], buff, count * 512);
	return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
	if (pdrv != 0)
		return RES_PARERR;
	switch (cmd) {
		case CTRL_SYNC:
			return RES_OK;
		case GET_SECTOR_COUNT:
			*(LBA_t *) buff = img_sz / SECTOR_SZ;
			return RES_OK;
		case GET_SECTOR_SIZE:
			*(WORD *) buff = SECTOR_SZ;
			return RES_OK;
		case GET_BLOCK_SIZE:
			*(DWORD *) buff = 4096;
			return RES_OK;
	}
	return RES_PARERR;
}

DWORD get_fattime(void)
{
	DateTime time = rtc_get();
	return (DWORD)(time.year - 1980) << 25 |
           (DWORD)time.month << 21 |
           (DWORD)time.day << 16 |
           (DWORD)time.hours << 11 |
           (DWORD)time.minutes << 5 |
           (DWORD)time.seconds >> 1;
}