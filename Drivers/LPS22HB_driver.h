/**
 ******************************************************************************
 * @file    LPS22HB_driver.h
 * @author  Levi Mariën
 * @brief   LPS22HB header driver file
 ******************************************************************************
 */

#ifndef __LPS22HB_DRIVER__H
#define __LPS22HB_DRIVER__H

typedef unsigned char u8_t;

/************** I2C Address *****************/
#define LPS22HB_I2C_ADDRESS 0xB8

/************** Read/Write functions *****************/
bool LPS22HB_read_reg(u8_t Reg, u8_t *Data);
bool LPS22HB_write_reg(u8_t Reg, u8_t Data);

/************** Register Map *****************/
#define LPS22HB_WHO_AM_I_REG 0x0F
#define LPS22HB_REF_P_XL_REG 0x15
#define LPS22HB_REF_P_L_REG 0x16
#define LPS22HB_REF_P_H_REG 0x17
#define LPS22HB_RES_CONF_REG 0x1A

#endif
