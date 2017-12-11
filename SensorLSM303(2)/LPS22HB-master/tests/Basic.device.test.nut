// Copyright 2017 Electric Imp
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


// Tests written for an Imp 001 Explorer kit

class BasicTestCase extends ImpTestCase {

    _press = null;
    _int = null;

    // Initialize sensor
    function setUp() {
        // imp001 explorer kit i2c bus
        local i2c = hardware.i2c89;
        i2c.configure(CLOCK_SPEED_400_KHZ);
        // imp001 explorer kit i2c address
        local addr = 0xB8;

        _int = hardware.pin1;

        _press = LPS22HB(i2c, addr);
        _press.softReset();

        return "Sensor initialized";
    }

    // helper function
    function _getRegisterBit(reg, mask) {
        local val = _press._getReg(reg);
        // mask other bits
        return val & mask;
    }

    function testSensorDevID() {
        local id = _press.getDeviceID();
        this.assertEqual(0xB1, id, "Device id doen't match datasheet");
        return "Sensor device id matches datasheet";
    }

    function testLowCurrentMode() {
        local expected, result;
        local mask = 0x01;
        _press.enableLowCurrentMode(true);
        // no good way to test so just check register
        expected = 1;
        result = _getRegisterBit(LPS22HB.RES_CONF, mask);
        this.assertEqual(expected, result, "Register not set");

        _press.enableLowCurrentMode(false);
        // no good way to test so just check register
        expected = 0;
        result = _getRegisterBit(LPS22HB.RES_CONF, mask);
        this.assertEqual(expected, result, "Register not set");

        return "Low current mode toggles the correct register bit";
    }

    function testDifferentialAutoZeroMode() {
        // Set mode to one-shot
        _press.setMode(LPS22HB_MODE.ONE_SHOT);

        // Take an absolute reading
        local r1 = _press.read();
        this.info("r1: " + r1.pressure);
        this.assertTrue(r1.pressure > 950 && r1.pressure < 1100, "Pressure reading not within acceptable range");

        // Enable Auto Zero
        _press.configureDifferentalPressureMode(LPS22HB_MODE.AUTO_ZERO, true);

        // Take a relative reading
        local r2 = _press.read();
        this.info("r2: " + r2.pressure);
        this.assertTrue(r2.pressure > -2 && r2.pressure < 2, "Pressure reading not within acceptable range");

        imp.sleep(0.5);

        // Take a relative reading
        local r3 = _press.read();
        this.info("r3: " + r3.pressure);
        this.assertTrue(r3.pressure != r2.pressure, "Pressure reading should not be exactally the same");
        this.assertClose(0, r3.pressure, 2, "Pressure reading not within acceptable range");

        // Disable Auto Zero
        _press.configureDifferentalPressureMode(LPS22HB_MODE.AUTO_ZERO, false);

        // Take an absolute reading
        local r4 = _press.read();
        this.info("r4: " + r4.pressure);
        this.assertTrue(r4.pressure > 950 && r4.pressure < 1100, "Pressure reading not within acceptable range");

        return "Auto zero enable changes read() result to relative reading"
    }

    function testDifferentialAutoZeroModeInterrupt() {
        // check that interrupt is relative
        return Promise(function(resolve, reject) {
            _int.configure(DIGITAL_IN_WAKEUP, function() {
                if (_int.read() != 0) {
                    local r1 = _press.read();
                    local result = _press.getInterruptSrc();
                    this.info("int active: " + result.int_active)
                    this.info("int low: " + result.low_pressure)
                    this.info("int high: " + result.high_pressure)
                    this.assertTrue(result.int_active, "Threshold interrupt triggered, int_active val not expected")
                    this.assertTrue(result.low_pressure || result.high_pressure, "Threshold interrupt triggered, but event not found");
                    _press.configureThresholdInterrupt(false);
                    _press.configureDifferentalPressureMode(LPS22HB_MODE.AUTO_ZERO, false);
                    resolve("Differential auto-zero threshold interrupt triggered as expected");
                }
            }.bindenv(this))

            local threshold = 0.0001;
            _press.setMode(LPS22HB_MODE.CONTINUOUS, 1);
            _press.configureDifferentalPressureMode(LPS22HB_MODE.AUTO_ZERO, true);
            // give time to set the reference pressure (must wait for over one second to get the next reading);
            imp.sleep(1.5);
            // set the interrupt
            _press.configureThresholdInterrupt(true, threshold, LPS22HB.INT_LATCH | LPS22HB.INT_HIGH_PRESSURE | LPS22HB.INT_LOW_PRESSURE);
        }.bindenv(this));
    }

    function testDifferentialAutoRifPMode() {
        return Promise(function(resolve, reject) {
            // check that interrupt is relative
            _int.configure(DIGITAL_IN_WAKEUP, function() {
                if (_int.read() != 0) {
                    // check interrupt
                    local result = _press.getInterruptSrc();
                    this.info("int active: " + result.int_active)
                    this.info("int low: " + result.low_pressure)
                    this.info("int high: " + result.high_pressure)
                    this.assertTrue(result.int_active, "Threshold interrupt triggered, int_active val not expected")
                    this.assertTrue(result.low_pressure || result.high_pressure, "Threshold interrupt triggered, but event not found");
                    // disable interrupt
                    _press.configureThresholdInterrupt(false);
                    // disable auto rif_p mode
                    _press.configureDifferentalPressureMode(LPS22HB_MODE.AUTO_RIF_P, false);
                    resolve("Differential auto-zero threshold interrupt triggered as expected");
                }
            }.bindenv(this))

            // enable auto rif_p
            _press.setMode(LPS22HB_MODE.CONTINUOUS, 1);
            _press.configureDifferentalPressureMode(LPS22HB_MODE.AUTO_RIF_P, true);

            // give time to set the reference pressure (must wait for over one second to get the next reading);
            imp.sleep(1.5);

            // check that reading is absolute
            local r1 = _press.read();
            this.assertTrue(r1.pressure > 950 && r1.pressure < 1100, "Pressure reading not within acceptable range");

            // enable interrupt
            local threshold = 0.0001;
            _press.getInterruptSrc();
            _press.configureThresholdInterrupt(true, threshold, LPS22HB.INT_LATCH | LPS22HB.INT_HIGH_PRESSURE | LPS22HB.INT_LOW_PRESSURE);
        }.bindenv(this));
    }

    function testLowPassFilter() {
        local expected, result;
        local mask = 0x0C;
        _press.setMode(LPS22HB_MODE.CONTINUOUS, 25);

        _press.configureLowPassFilter(LPS22HB.LPF_BANDWIDTH_ODR_9);
        // no good way to test so just check register
        expected = 0x08;
        result = _getRegisterBit(LPS22HB.CTRL_REG1, mask);
        this.assertEqual(expected, result, "Register not set");

        _press.configureLowPassFilter(LPS22HB.LPF_BANDWIDTH_ODR_20);
        // no good way to test so just check register
        expected = 0x0C;
        result = _getRegisterBit(LPS22HB.CTRL_REG1, mask);
        this.assertEqual(expected, result, "Register not set");

        _press.configureLowPassFilter(LPS22HB.LPF_OFF);
        // no good way to test so just check register
        expected = 0x00;
        result = _getRegisterBit(LPS22HB.CTRL_REG1, mask);
        this.assertEqual(expected, result, "Register not set");

        return "Low pass filter toggles correct register bits";
    }

    function testDataReadyInterrupt() {
        local result_1, result_2;
        return Promise(function(resolve, reject) {
            _int.configure(DIGITAL_IN_WAKEUP, function() {
                if (_int.read() != 0) {
                    result_2 = _press.read();
                    this.info(format("Last Pressure: %0.4f hPa, last Temperature: %0.4f °C", result_1.pressure, result_1.temperature));
                    this.info(format("New Pressure: %0.4f hPa, new Temperature: %0.4f °C", result_2.pressure, result_2.temperature));
                    // readings should not be identical
                    this.assertTrue( (result_2.pressure !=  result_1.pressure) || (result_2.temperature !=  result_1.temperature), "Data ready interrupt not triggerd as expected");
                    _press.configureDataReadyInterrupt(false);
                    resolve("Data ready interrupt triggered as expected");
                }
            }.bindenv(this))
            _press.setMode(LPS22HB_MODE.CONTINUOUS, 1);
            _press.getInterruptSrc();
            result_1 = _press.read();
            _press.configureDataReadyInterrupt(true);
        }.bindenv(this))
    }

    function testThresholdInterrupt() {
        return Promise(function(resolve, reject) {
            _int.configure(DIGITAL_IN_WAKEUP, function() {
                if (_int.read() != 0) {
                    local result = _press.getInterruptSrc();
                    this.assertTrue(result.int_active, "Threshold interrupt triggered, int_active val not expected")
                    this.assertTrue(result.low_pressure, "Threshold interrupt triggered, low_pressure val not expected");
                    this.assertTrue(!result.high_pressure, "Threshold interrupt triggered, high_pressure val not expected");
                    _press.configureThresholdInterrupt(false);
                    resolve("Threshold interrupt triggered as expected");
                }
            }.bindenv(this))
            _press.setMode(LPS22HB_MODE.CONTINUOUS, 1);
            _press.getInterruptSrc();
            local reading = _press.read();
            _press.configureThresholdInterrupt(true, (reading.pressure - 5), LPS22HB.INT_LATCH | LPS22HB.INT_HIGH_PRESSURE);
        }.bindenv(this))
    }

    function testSyncRead() {
        _press.setMode(LPS22HB_MODE.ONE_SHOT);
        local result = _press.read();
        this.assertTrue( !("err" in result) );
        this.info(format("Current Pressure: %0.2f hPa, Current Temperature: %0.2f °C", result.pressure, result.temperature));
        this.assertTrue(result.temperature > 0 && result.temperature < 50, "Temperature reading not within acceptable range");
        this.assertTrue(result.pressure > 950 && result.pressure < 1100, "Pressure reading not within acceptable range");
        return "Pressure and Temperature readings with in acceptable range";
    }

    function testAsyncRead() {
        _press.setMode(LPS22HB_MODE.ONE_SHOT);
        return Promise(function(resolve, reject) {
            _press.read(function(result) {
                if ("err" in result) {
                    reject(result.err);
                } else {
                    this.info(format("Current Pressure: %0.2f hPa, Current Temperature: %0.2f °C", result.pressure, result.temperature));
                    this.assertTrue(result.temperature > 0 && result.temperature < 50, "Temperature reading not within acceptable range");
                    this.assertTrue(result.pressure > 950 && result.pressure < 1100, "Pressure reading not within acceptable range");
                    resolve("Pressure and Temperature readings with in acceptable range");
                }
            }.bindenv(this));
        }.bindenv(this));
    }

    function testReset() {
        local expected, result;
        local mask = 0xFF;

        // Check that register has expected defaults
        _press.softReset();
        expected = 0x00;
        result = _getRegisterBit(LPS22HB.CTRL_REG1, mask);
        this.assertEqual(expected, result, "Register not set");

        // Check that register has expected settings
        _press.setMode(LPS22HB_MODE.CONTINUOUS, 25);
        expected = 0x30;
        result = _getRegisterBit(LPS22HB.CTRL_REG1, mask);
        this.assertEqual(expected, result, "Register not set");

        // Check that register has expected defaults
        _press.softReset();
        expected = 0x00;
        result = _getRegisterBit(LPS22HB.CTRL_REG1, mask);
        this.assertEqual(expected, result, "Register not set");

        return "Reset restores default settings"
    }

    function testSetGetMode() {
        local dataRate = _press.setMode(LPS22HB_MODE.ONE_SHOT);
        this.assertEqual(0, dataRate, "Set mode return value not equal to expected data rate");
        local mode = _press.getMode();
        this.assertEqual(LPS22HB_MODE.ONE_SHOT, mode, "Get mode return value not equal to expected mode");

        return "Get and set mode return expected values"
    }

    function testGetSupportedDataRate() {
        local supportedDataRate = 25;
        local dataRate = _press.setMode(LPS22HB_MODE.CONTINUOUS, supportedDataRate);
        this.assertEqual(supportedDataRate, dataRate, "Set mode return value not equal to expected data rate");
        local getDR = _press.getDataRate();
        this.assertEqual(supportedDataRate, getDR, "Get data rate not equal to expected data rate");

        return "Supported data rate set as expected"
    }

    function testGetUnupportedDataRate() {
        local unsupportedDataRate = 30;
        local expectedDR = 25;
        local dataRate = _press.setMode(LPS22HB_MODE.CONTINUOUS, unsupportedDataRate);
        this.assertEqual(expectedDR, dataRate, "Set mode return value not equal to expected data rate");
        local getDR = _press.getDataRate();
        this.assertEqual(expectedDR, getDR, "Get data rate not equal to expected data rate");

        return "Unsupported data rate rounded down to expected rate"
    }

    function tearDown() {
        _press.softReset();
        return "Pressure sensor reset"
    }

}
