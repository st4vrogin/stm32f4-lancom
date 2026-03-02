/** ****************************************************************************
	@file		macros.h
	@brief		Definizione macros

	@author		SielTre srl
	@version	0.0.1	2023-06-16
******************************************************************************/

#ifndef __LIB_MACROS_H__
#define __LIB_MACROS_H__

//#define abs(x) (((x) < 0)? (-(x)): (x))

#define DISABLE_INTERRUPTS		__disable_irq()
#define ENABLE_INTERRUPTS		__enable_irq()

#define ATTESA_RESET_WDOG		{while(1){};}

/* Macro per accesso Variabili in BIT_BANDING */
#define Var_ResetBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (SRAM_BB_BASE | (((uint32_t)VarAddr - SRAM_BASE) << 5) | ((BitNumber) << 2)) = 0)

#define Var_SetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (SRAM_BB_BASE | (((uint32_t)VarAddr - SRAM_BASE) << 5) | ((BitNumber) << 2)) = 1)

#define Var_GetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (SRAM_BB_BASE | (((uint32_t)VarAddr - SRAM_BASE) << 5) | ((BitNumber) << 2)))

#define Var_WriteBit_BB(VarAddr, BitNumber, val)    \
          (*(__IO uint32_t *) (SRAM_BB_BASE | (((uint32_t)VarAddr - SRAM_BASE) << 5) | ((BitNumber) << 2)) = val)

/* Macro per accesso Periferiche in BIT_BANDING */
#define Perif_ResetBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | (((uint32_t)VarAddr - PERIPH_BASE) << 5) | ((BitNumber) << 2)) = 0)

#define Perif_SetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | (((uint32_t)VarAddr - PERIPH_BASE) << 5) | ((BitNumber) << 2)) = 1)

#define Perif_GetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | (((uint32_t)VarAddr - PERIPH_BASE) << 5) | ((BitNumber) << 2)))

#define Perif_WriteBit_BB(VarAddr, BitNumber, val)    \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | (((uint32_t)VarAddr - PERIPH_BASE) << 5) | ((BitNumber) << 2)) = val)

#define Perif_OutBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | (((uint32_t)VarAddr - PERIPH_BASE) << 5) | ((BitNumber) << 2)))


#endif	/* __LIB_MACROS_H__ */
