/**
 * LPS22HB_driver.cpp
 * Purpose: implementation of the LPS22HB_driver.h
 *
 * @author: Levi Mariën
 * @version 1.0 11/12/2017
 */

#include "LPS22HB_driver.h"

void setInterface_LPS22HB(I2C_HandleTypeDef *hi2c) {
	hi2cLib = hi2c;
}

void reset_LPS22HB() {


}

void reboot_LPS22HB() {

}

void powerDown_LPS22HB() {

}

uint8_t configure_LPS22HB(){

}

uint8_t readRegister_LPS22HB(uint8_t buf[],uint8_t reg,uint8_t length) {

}

uint8_t writeRegister_LPS22HB(uint8_t buf[],uint8_t reg,uint8_t length) {

}



