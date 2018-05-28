#include "sddriver.h"

#ifdef STM32_FATFS_USE

SDDriver sd_driver;

/* Block Size in Bytes */
#define BLOCK_SIZE                512

uint8_t SDDriver::init(uint8_t)
{
    uint8_t sd_state = SD_OK;
    m_state = STA_NOINIT;
    if (!is_card_present())
        return SD_ERROR;
    sd_state = STM32_SD::init();
    #ifdef STM32_FATFS_BUS_4BITS
    if (sd_state == SD_OK)
    {
        if (STM32_SD::wide_bus_config(STM32_SDIO_BUS) != SD_OK)
            sd_state = SD_ERROR;
    }
    #endif
    return sd_state;
}

uint8_t SDDriver::status(uint8_t)
{
    return 0;
}

uint8_t SDDriver::read(uint8_t lun, uint8_t*buf, uint32_t sector, uint32_t count)
{
    UNUSED(lun);
    if (STM32_SD::read_blocks(buf, sector * BLOCK_SIZE, BLOCK_SIZE, count) != SD_OK)
        return SD_ERROR;
    return SD_OK;
}

#if _USE_WRITE == 1
uint8_t SDDriver::write(uint8_t lun, uint8_t*buf, uint32_t sector, uint32_t count)
{
    UNUSED(lun);
    if (STM32_SD::write_blocks(buf, sector * BLOCK_SIZE, BLOCK_SIZE, count) != SD_OK)
        return SD_ERROR;
    return SD_OK;
}

#endif
#if _USE_IOCTL == 1
uint8_t SDDriver::ioctl(uint8_t, uint8_t, void*)
{
    return 0;
}

#endif

#endif //STM32_FATFS_USE
