#ifndef STM32_DEF_H
#define STM32_DEF_H

#define STM32_BIT_FIELD_GET_SET(name, offset, type, field) \
    static inline bool get_ ## name () { return reinterpret_cast<volatile type*>(offset)->field; } \
    static inline void set_ ## name (bool val) { reinterpret_cast<volatile type*>(offset)->field = val; }
#define STM32_TYPE_FIELD_GET_SET(name, offset, type, field, field_type) \
    static inline field_type get_ ## name () { return static_cast<field_type>(reinterpret_cast<volatile type*>(offset)->field); } \
    static inline void set_ ## name (field_type val) { reinterpret_cast<volatile type*>(offset)->field = static_cast<uint32_t>(val); }

#include <stdint.h>
#if defined(STM32F100xB)
#include "stm32f1xx.h"
#elif defined(STM32F429xx) || defined (STM32F407xx)
#include "stm32f4xx.hpp"
#endif
#include "bitbanding.h"

#define RAMFUNC __attribute__ ((long_call, section (".ramfunctions")))

extern uint32_t unused_reg;
#define UNUSED(x) (void)(x)

#define STM32_RESULT_OK         0
#define STM32_RESULT_FAIL       1
#define STM32_RESULT_TIMEOUT    2
#define STM32_RESULT_BUSY       3

#define STM32_UNLOCKED 0
#define STM32_LOCKED   1

#define STM32_USB_FS_PORT gpioa
#define STM32_USB_FS_EN_CLK enable_clk_GPIOA
#define STM32_USB_FS_DM_PIN STM32_GPIO::PIN_11
#define STM32_USB_FS_DP_PIN STM32_GPIO::PIN_12

#define STM32_USB_HS_PORT gpiob
#define STM32_USB_HS_EN_CLK enable_clk_GPIOB
#define STM32_USB_HS_DM_PIN STM32_GPIO::PIN_14
#define STM32_USB_HS_DP_PIN STM32_GPIO::PIN_15

#define FORCE_INLINE __attribute__((always_inline))

#define ENDIS_REG_FLAG(name, reg, mask) \
    static inline void enable_ ## name() { BIT_BAND_PER(reg, mask) = 1; unused_reg = reg & mask; } \
    static inline void disable_ ## name() { BIT_BAND_PER(reg, mask) = 0; unused_reg = reg & mask; }

#define ENDIS_REG_FLAG_(reg, mask) \
    inline void enable() { BIT_BAND_PER(reg, mask) = 1; } \
    inline void disable() { BIT_BAND_PER(reg, mask) = 0; }
#define ENDIS_REG_FLAG_NAME(name, reg, mask) \
    inline void enable_ ## name() { BIT_BAND_PER(reg, mask) = 1; } \
    inline void disable_ ## name() { BIT_BAND_PER(reg, mask) = 0; }
#define ENDIS_REG_FLAG_NAME_SL(name, reg, mask) \
    inline void enable_ ## name() { reg |= mask; } \
    inline void disable_ ## name() { reg &= ~mask; }

#define CLK_ENDIS(enr, name) \
    ENDIS_REG_FLAG(clk_ ## name, RCC_->enr ## ENR, RCC_ ## enr ## ENR_ ## name ## EN) \
    static inline bool check_enable_clk_ ## name() { return BIT_BAND_PER(RCC_->enr ## ENR, RCC_ ## enr ## ENR_ ## name ## EN); }
#define CLK_ENDIS_EX(enr, name, bit_name) \
    ENDIS_REG_FLAG(clk_ ## name, RCC_->enr ## ENR, RCC_ ## enr ## ENR_ ## bit_name ## EN) \
    static inline bool check_enable_clk_ ## name() { return BIT_BAND_PER(RCC_->enr ## ENR, RCC_ ## enr ## ENR_ ## bit_name ## EN); }

#define PER_RESET_SLEEP(enr, name) \
    static inline void force_reset_ ## name() { BIT_BAND_PER(RCC_-> enr ## RSTR, RCC_ ## enr ## RSTR_ ## name ## RST) = 1; } \
    static inline void release_reset_ ## name() { BIT_BAND_PER(RCC_-> enr ## RSTR, RCC_ ## enr ## RSTR_ ## name ## RST) = 0; } \
    static inline void sleep_enable_ ## name() { BIT_BAND_PER(RCC_-> enr ## LPENR, RCC_ ## enr ## LPENR_ ## name ## LPEN) = 1; }

#define PER_RESET(enr, name) \
    static inline void force_reset_ ## name() { BIT_BAND_PER(RCC_-> enr ## RSTR, RCC_ ## enr ## RSTR_ ## name ## RST) = 1; } \
    static inline void release_reset_ ## name() { BIT_BAND_PER(RCC_-> enr ## RSTR, RCC_ ## enr ## RSTR_ ## name ## RST) = 0; }
#define PER_RESET_EX(enr, name, bit_name) \
    static inline void force_reset_ ## name() { BIT_BAND_PER(RCC_-> enr ## RSTR, RCC_ ## enr ## RSTR_ ## bit_name ## RST) = 1; } \
    static inline void release_reset_ ## name() { BIT_BAND_PER(RCC_-> enr ## RSTR, RCC_ ## enr ## RSTR_ ## bit_name ## RST) = 0; }

#define WAIT_TIMEOUT(condition, timeout) \
    { \
        uint32_t tout_ex = STM32::SYSTICK::get_tick() + timeout; \
        while (condition) \
        { \
            if ((STM32::SYSTICK::get_tick()) > tout_ex) \
                return STM32_RESULT_TIMEOUT; \
        } \
    }

#define WAIT_TIMEOUT_EX(condition, timeout, ret) \
    { \
        uint32_t tout_ex = STM32::SYSTICK::get_tick() + timeout; \
        while (condition) \
        { \
            if ((STM32::SYSTICK::get_tick()) > tout_ex) \
                return ret; \
        } \
    }

void Error_Handler();

class STM32_SDRAM;
extern STM32_SDRAM sdram;

enum class TXRX_MODE
{
    DIRECT,
    INTERRUPT,
    DMA,
};

#ifdef STM32_DEBUG
#include "xprintf.h"
#define debug_out(s) uart3.send_str(s, TXRX_MODE::DIRECT)
#define debug_printf(s, ...) xprintf(s, __VA_ARGS__)
#define debug_fn() xprintf(__PRETTY_FUNCTION__); xprintf("\n\r")
#else
#define debug_out(s)
#define debug_printf(s, ...)
#define debug_fn(s, ...)
#endif

#endif
