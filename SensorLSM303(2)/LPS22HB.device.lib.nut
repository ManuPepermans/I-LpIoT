// MIT License
//
// Copyright 2016 Electric Imp
//
// SPDX-License-Identifier: MIT
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
// EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

enum LPS22HB_MODE {
    ONE_SHOT,
    CONTINUOUS,
    AUTO_ZERO,
    AUTO_RIF_P
}

class LPS22HB {

    static VERSION = "2.0.0";

    static MAX_MEAS_TIME_SECONDS = 0.5; // seconds; time to complete one-shot pressure conversion

    static INT_CFG         = 0x0B;
    static THS_P_L         = 0x0C;
    static THS_P_H         = 0x0D;
    static WHO_AM_I        = 0x0F;
    static CTRL_REG1       = 0x10;
    static CTRL_REG2       = 0x11;
    static CTRL_REG3       = 0x12;
    static FIFO_CTRL       = 0x14;
    static RES_CONF        = 0x1A;
    static INT_SOURCE      = 0x25;
    static FIFO_STATUS     = 0x26;
    static STATUS_REG      = 0x27;
    static PRESS_OUT_XL    = 0x28;
    static PRESS_OUT_L     = 0x29;
    static PRESS_OUT_H     = 0x2A;
    static TEMP_OUT_L      = 0x2B;
    static TEMP_OUT_H      = 0x2C;
    static LPFP_RES        = 0x33;

    static PRESSURE_SCALE           = 4096.0;
    static THRESHOLD_PRESSURE_SCALE = 16.0;
    static TEMPERATURE_SCALE        = 100.0;

    // interrupt settings
    static INT_PIN_ACTIVELOW        = 0x80; // REG3 bit setting
    static INT_PIN_OPENDRAIN        = 0x40; // REG3 bit setting
    static INT_LATCH                = 0x04; // INT_CFG bitt setting
    static INT_LOW_PRESSURE         = 0x02; // Both REG3 pin routing & INT_CFG bit settings
    static INT_HIGH_PRESSURE        = 0x01; // Both REG3 pin routing & INT_CFG bit settings
    static INT_DDRY_MASK            = 0xFC; // REG3 pin routing
    static INT_DIFF_ENABLE          = 0x08; // INT_CFG bit setting to enable threshold interrupts
    static INT_DDRY_ENABLE          = 0x04; // REG3 bit setting to enable data ready interrupts

    // low pass filter settings/masks
    static LPF_OFF                  = 0xF3;
    static LPF_BANDWIDTH_ODR_9      = 0x08;
    static LPF_BANDWIDTH_ODR_20     = 0x0C;


    _i2c        = null;
    _addr       = null;

    _mode = null;

    // -------------------------------------------------------------------------
    constructor(i2c, addr = 0xB8) {
        _i2c = i2c;
        _addr = addr;
        _mode = getMode();
    }

    // -------------------------------------------------------------------------
    function getDeviceID() {
        return _getReg(WHO_AM_I);
    }

    // -------------------------------------------------------------------------
    function softReset() {
        _setRegBit(CTRL_REG2, 2, 1);
    }

    // -------------------------------------------------------------------------
    function setMode(mode, dataRate =  null) {
        // Get data rate from sensor if not passed in
        if (dataRate == null) dataRate = getDataRate();

        switch (mode) {
            case LPS22HB_MODE.CONTINUOUS :
                // Set to continuous only if data rate is not 0
                if (dataRate != null && dataRate != 0) {
                    // Set data rate
                    dataRate = _setDataRate(dataRate);
                    // Store mode locally
                    _mode = LPS22HB_MODE.CONTINUOUS;
                } else {
                    dataRate = _setDataRate(0);
                    _mode = LPS22HB_MODE.ONE_SHOT;
                }
                break;
            default :
                // Set to power down mode
                dataRate = _setDataRate(0);
                // Store mode locally
                _mode = LPS22HB_MODE.ONE_SHOT;
                break;
        }
        return dataRate;
    }

    // -------------------------------------------------------------------------
    function getMode() {
        local rate = getDataRate();
        return (rate == 0) ? LPS22HB_MODE.ONE_SHOT : LPS22HB_MODE.CONTINUOUS;
    }

    // -------------------------------------------------------------------------
    function enableLowCurrentMode(enable) {
        _setRegBit(RES_CONF, 0, (enable) ? 1 : 0);
    }

    // -------------------------------------------------------------------------
    function getDataRate() {
        local val = (_getReg(CTRL_REG1) & 0x70) >> 4;
        if (val == 0) {
            return 0.0;
        } else if (val == 0x01) {
            return 1.0;
        } else if (val == 0x02) {
            return 10.0;
        } else if (val == 0x03) {
            return 25.0;
        } else if (val == 0x04) {
            return 50.0;
        } else {
            return 75.0;
        }
    }

    // -------------------------------------------------------------------------
    function read(cb = null) {
        // try/catch so errors thrown by I2C methods can be handed to the callback
        // instead of just thrown again
        try {
            local meas_time = 0;
            if (_mode == LPS22HB_MODE.ONE_SHOT) {
                _setRegBit(CTRL_REG2, 0, 1);
                meas_time = MAX_MEAS_TIME_SECONDS;
            }
            if (cb) {
                imp.wakeup(meas_time, function() {
                    cb({ "pressure": _getPressure(), "temperature": _getTemp() });
                }.bindenv(this));
            } else {
                if (meas_time > 0) imp.sleep(meas_time);
                return { "pressure": _getPressure(), "temperature": _getTemp() };
            }
        } catch (err) {
            if (cb == null) {
                return {"error": err, "pressure": null, "temperature" : null };
            } else {
                imp.wakeup(0, function() {
                    cb({"error": err, "pressure": null, "temperature" : null });
                });
            }
        }
    }

    // -------------------------------------------------------------------------
    function configureDifferentalPressureMode(mode, enable) {
        local val = _getReg(INT_CFG);
        local refPress = null;
        switch(mode) {
            case LPS22HB_MODE.AUTO_ZERO:
                if(enable) {
                    // clear AUTO_RIF_P & enable AUTO_ZERO
                    _setReg(INT_CFG,  val | 0x60)
                    // if in one shot mode take a reading to set reference pressure
                    if(_mode == LPS22HB_MODE.ONE_SHOT) read();
                } else {
                    // reset AUTO_ZERO
                    _setRegBit(INT_CFG, 4, 1);
                }
                break;
            case LPS22HB_MODE.AUTO_RIF_P:
                if(enable) {
                    // clear AUTO_ZERO & enable AUTO_RIF_P
                    _setReg(INT_CFG,  val | 0x90)
                    // if in one shot mode take a reading to set reference pressure
                    if(_mode == LPS22HB_MODE.ONE_SHOT) read();
                } else {
                    // reset AUTO_RIF_P
                    _setRegBit(INT_CFG, 6, 1);
                }
                break;
        }
    }

    // -------------------------------------------------------------------------
    function configureLowPassFilter(bandwidth, reset = false) {
        local val = _getReg(CTRL_REG1);

        switch(bandwidth) {
            case LPF_BANDWIDTH_ODR_20:
                val  = val | LPF_BANDWIDTH_ODR_20;
                break;
            case LPF_BANDWIDTH_ODR_9:
                val  = val | LPF_BANDWIDTH_ODR_9;
                break;
            default :
                val = val & LPF_OFF;
        }

        _setReg(CTRL_REG1, val & 0xFF);

        // reset to aviod transitory phase
        if (reset) _getReg(LPFP_RES);
    }

    // -------------------------------------------------------------------------
    function configureDataReadyInterrupt(enable, options = 0) {
        // Check and set the options ------------------------------------------
        // Interrupt Config options
        local val = (options & INT_LATCH) ? 1 : 0;
        _setRegBit(INT_CFG, 2, val);

        // Register 3 options
        val = _getReg(CTRL_REG3);
        val = (options & INT_PIN_ACTIVELOW) ?  (val | INT_PIN_ACTIVELOW) : (val & ~ INT_PIN_ACTIVELOW);
        val = (options & INT_PIN_OPENDRAIN) ?  (val | INT_PIN_OPENDRAIN) : (val & ~ INT_PIN_OPENDRAIN);

        // route interrupt pin to data ready config
        val = val & INT_DDRY_MASK;
        val = (enable) ? (val | INT_DDRY_ENABLE) : (val & ~INT_DDRY_ENABLE);

        _setReg(CTRL_REG3, val & 0xFF);
    }

    // new board active high/pushpull (default settings)
    function configureThresholdInterrupt(enable, threshold = null, options = 0) {
        // Datasheet recommends setting threshold before enabling/disabling int gen
        // set the threshold, if it was given ---------------------------------
        if (threshold != null) _setInterruptThreshold(threshold);

        // Check and set the options ------------------------------------------
        // Register 3 options
        local val = _getReg(CTRL_REG3);
        val = (options & INT_PIN_ACTIVELOW) ?  (val | INT_PIN_ACTIVELOW) : (val & ~ INT_PIN_ACTIVELOW);
        val = (options & INT_PIN_OPENDRAIN) ?  (val | INT_PIN_OPENDRAIN) : (val & ~ INT_PIN_OPENDRAIN);
        // route interrupt pin to pressure high/low events
        val = (enable) ? (val | INT_LOW_PRESSURE | INT_HIGH_PRESSURE) : (val & ~(INT_LOW_PRESSURE | INT_HIGH_PRESSURE));

        _setReg(CTRL_REG3, val & 0xFF);

        // Interrupt Config options
        local val = _getReg(INT_CFG);
        val = (options & INT_LATCH) ? (val | INT_LATCH) : (val & ~ INT_LATCH);
        val = (options & INT_LOW_PRESSURE) ? (val | INT_LOW_PRESSURE) : (val & ~ INT_LOW_PRESSURE);
        val = (options & INT_HIGH_PRESSURE) ? (val | INT_HIGH_PRESSURE) : (val & ~ INT_HIGH_PRESSURE);
        val = (enable) ? (val | INT_DIFF_ENABLE) : (val & ~INT_DIFF_ENABLE);
        _setReg(INT_CFG, val & 0xFF);
    }

    // -------------------------------------------------------------------------
    function getInterruptSrc() {
        local val = _getReg(INT_SOURCE);
        return { "int_active" : (val & 0x04) ? true : false,
                 "high_pressure" : (val & 0x02) ? true : false,
                 "low_pressure" : (val & 0x01) ? true : false }
    }


    // ------------------ PRIVATE METHODS -------------------------------------//

    // -------------------------------------------------------------------------
    function _twosComp(value, mask) {
        value = ~(value & mask) + 1;
        return -1 * (value & mask);
    }

    // -------------------------------------------------------------------------
    function _getReg(reg) {
        local result = _i2c.read(_addr, reg.tochar(), 1);

        if (result == null) {
            throw "I2C read error: " + _i2c.readerror();
        }
        return result[0];
    }

    // -------------------------------------------------------------------------
    function _getRegisters(reg, numRegs) {
        local result = _i2c.read(_addr, reg.tochar(), numRegs);

        if (result == null) {
            throw "I2C read error: " + _i2c.readerror();
        }
        return result;
    }

    // -------------------------------------------------------------------------
    function _setReg(reg, val) {
        local result = _i2c.write(_addr, format("%c%c", reg, (val & 0xff)));
        if (result) {
            throw "I2C write error: " + result;
        }
        return result;
    }

    // -------------------------------------------------------------------------
    function _setRegBit(reg, bit, bitVal) {
        local regVal = _getReg(reg);
        if (bitVal == 0) {
            regVal = regVal & ~(0x01 << bit);
        } else {
            regVal = regVal | (0x01 << bit);
        }
        return _setReg(reg, regVal);
    }

    // -------------------------------------------------------------------------
    function _setDataRate(datarate) {
        local val = (_getReg(CTRL_REG1) & 0x8F);
        local actualRate = 0.0;

        if (datarate < 1) {
            datarate = 0x00;
        } else if (datarate < 10) {
            actualRate = 1.0;
            datarate = 0x01;
        } else if (datarate < 25) {
            actualRate = 10.0;
            datarate = 0x02;
        } else if (datarate < 50) {
            actualRate = 25.0;
            datarate = 0x03;
        } else if (datarate < 75) {
            actualRate = 50.0;
            datarate = 0x04;
        } else {
            actualRate = 75.0;
            datarate = 0x05;
        }

        _setReg(CTRL_REG1, (val | (datarate << 4)));
        return actualRate;
    }

    // -------------------------------------------------------------------------
    // Returns raw pressure register values
    function _getPressure() {
        local press = _getRegisters(PRESS_OUT_XL, 3);
        local raw = ( (press[2] << 16) | (press[1] << 8) | press[0]);
        if (raw & 0x800000) { raw = _twosComp(raw, 0x7FFFFF); }
        return (raw / PRESSURE_SCALE);
    }

    // -------------------------------------------------------------------------
    function _getTemp() {
        local temp = _getRegisters(TEMP_OUT_L, 2);
        local raw = (temp[1] << 8) | temp[0];
        if (raw & 0x8000) { raw = _twosComp(raw, 0x7FFF); }
        return (raw / TEMPERATURE_SCALE);
    }

    // -------------------------------------------------------------------------
    function _setInterruptThreshold(threshold) {
        threshold = (threshold * THRESHOLD_PRESSURE_SCALE).tointeger();
        _setReg(THS_P_H, (threshold & 0xFF00) >> 8);
        _setReg(THS_P_L, threshold & 0xFF);
    }
}
