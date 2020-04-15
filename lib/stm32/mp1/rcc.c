/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Federico Ruiz-Ugalde <memeruiz at gmail dot com>
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2010 Thomas Otto <tommi@viadmin.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>

/* MP1 family uses two registers to set and clear enable/reset bits.
 * Use clear register address when defined in the rcc_periph_clken/rst enum.
 * Otherwise, keep on using the same address for set and reset.
 */
#define _RCC_CLR_OFFSET(i)      (((i) >> (5 + 12)) & 0xFFF)
#define _RCC_SET_OFFSET(i)      (((i) >> 5) & 0xFFF)
#define _RCC_CLR_POL(i)         (((i) >> (5 + 2*12)) & 0x1)
#define _RCC_SET_REG(i)         MMIO32(RCC_BASE + _RCC_SET_OFFSET(i))
#define _RCC_CLR_REG(i)         MMIO32(RCC_BASE + ((_RCC_CLR_OFFSET(i) != 0) ? _RCC_CLR_OFFSET(i) : _RCC_SET_OFFSET(i)))
#define _RCC_BIT(i)		(1 << ((i) & 0x1f))

/*---------------------------------------------------------------------------*/
/** @brief RCC Enable Peripheral Clocks.
 *
 * Enable the clock on particular peripherals. Several peripherals could be
 * enabled simultaneously <em>only if they are controlled by the same
 * register</em>.
 * @sa rcc_periph_clock_enable for a less error prone version, if you only
 * need to enable a single peripheral.
 *
 * @param[in] *reg Unsigned int32. Pointer to a Clock Enable Register
 *
 * @param[in] en Unsigned int32. Logical OR of all enables to be set
 */

void rcc_peripheral_enable_clock(volatile uint32_t *reg, uint32_t en)
{
	*reg = en;
}

/*---------------------------------------------------------------------------*/
/** @brief RCC Disable Peripheral Clocks.
 *
 * Disable the clock on particular peripherals. Several peripherals could be
 * disabled simultaneously <em>only if they are controlled by the same
 * register</em>.
 * @sa rcc_periph_clock_disable for a less error prone version, if you only
 * need to disable a single peripheral.
 *
 * @param[in] *reg Unsigned int32. Pointer to a Clock Enable Register
 * @param[in] en Unsigned int32. Logical OR of all enables to be used for
 * disabling.
 */
void rcc_peripheral_disable_clock(volatile uint32_t *reg, uint32_t en)
{
	*reg = en;
}

/*---------------------------------------------------------------------------*/
/** @brief Enable Peripheral Clock in running mode.
 *
 * Enable the clock on particular peripheral.
 *
 * @param[in] clken rcc_periph_clken Peripheral RCC
 *
 * For available constants, see #rcc_periph_clken (RCC_UART1 for example)
 */

void rcc_periph_clock_enable(enum rcc_periph_clken clken)
{
	_RCC_SET_REG(clken) = _RCC_BIT(clken);
}

/*---------------------------------------------------------------------------*/
/** @brief Disable Peripheral Clock in running mode.
 * Disable the clock on particular peripheral.
 *
 * @param[in] clken rcc_periph_clken Peripheral RCC
 *
 * For available constants, see #rcc_periph_clken (RCC_UART1 for example)
 */

void rcc_periph_clock_disable(enum rcc_periph_clken clken)
{
	_RCC_CLR_REG(clken) = _RCC_BIT(clken);
}

bool rcc_is_osc_ready(enum rcc_osc osc)
{
	switch(osc) {
	case RCC_PLL1:
		return RCC_PLL1CR & RCC_PLLCR_PLLRDY;
	case RCC_PLL2:
		return RCC_PLL2CR & RCC_PLLCR_PLLRDY;
	case RCC_PLL3:
		return RCC_PLL3CR & RCC_PLLCR_PLLRDY;
	case RCC_PLL4:
		return RCC_PLL4CR & RCC_PLLCR_PLLRDY;
	case RCC_HSE:
		return RCC_OCRDYR & RCC_OCRDYR_HSERDY;
	case RCC_HSI:
		return RCC_OCRDYR & RCC_OCRDYR_HSIRDY;
	case RCC_LSE:
		return RCC_BDCR & RCC_BDCR_LSERDY;
	case RCC_LSI:
		return RCC_RDLSICR & RCC_RDLSICR_LSIRDY;
	case RCC_CSI:
		return RCC_OCRDYR & RCC_OCRDYR_CSIRDY;
	}
	return false;
}

void rcc_wait_for_osc_ready(enum rcc_osc osc)
{
	while (!rcc_is_osc_ready(osc));
}
