#ifndef SDDRIVER_H
#define SDDRIVER_H

#include "stm32_inc.h"
#include "diskiodriver.h"

#define SD_PRESENT      0x01
#define SD_NOT_PRESENT  0x00
#define SD_DATATIMEOUT  ((uint32_t)100000000)

#ifdef STM32_USE_SD

class SDDriver: public DiskIODriver
{
public:
    virtual uint8_t init(uint8_t);
    virtual void link_data(void* pdata) { (void)(pdata); }
    virtual uint8_t status(uint8_t);
    virtual DSTATUS read(uint8_t lun, uint8_t*buf, uint32_t sector, uint16_t count);
    #if _USE_WRITE == 1
    virtual DSTATUS write(uint8_t, uint8_t*, uint32_t, uint16_t);
    #endif
    #if _USE_IOCTL == 1
    virtual uint8_t ioctl(uint8_t, FAT_FS::ECTRL, void*);
    #endif

    static inline void init_gpio() {
        debug_fn();
        STM32_RCC::STM32_FATFS_CARD_EN_CLK();
                                     STM32_FATFS_CARD_DETECT_PORT.set_config(STM32_FATFS_CARD_DETECT_PIN,
                                                                      STM32_GPIO::EMode::INPUT, 0,
                                                                      STM32_GPIO::ESpeed::LOW, STM32_GPIO::EPull::PULLUP); }

    static inline bool is_card_present() { return (STM32_FATFS_CARD_DETECT_PORT.pin_read(STM32_FATFS_CARD_DETECT_PIN) == RESET); }
private:
    uint8_t m_state;
};

#endif //STM32_USE_SD

#endif // SDDRIVER_H
