#include "XNucleoIKS01A2.h"
#include "mbed.h"

/* Instantiate the expansion board */
static XNucleoIKS01A2 *mems_expansion_board =
    XNucleoIKS01A2::instance(D14, D15, D4, D5);
/* Retrieve the composing elements of the expansion board */
static LSM303AGRMagSensor *magnetometer = mems_expansion_board->magnetometer;
static HTS221Sensor *hum_temp = mems_expansion_board->ht_sensor;
static LPS22HBSensor *press_temp = mems_expansion_board->pt_sensor;
static LSM6DSLSensor *acc_gyro = mems_expansion_board->acc_gyro;
static LSM303AGRAccSensor *accelerometer = mems_expansion_board->accelerometer;

// UART via the serial interface for the DASH7 board
// https://os.mbed.com/docs/latest/reference/serial.html
Serial DASH7UART(D1, D0);

InterruptIn button_int(USER_BUTTON);
bool wake_up = false;
DigitalOut led(LED1);

void call_back() { wake_up = true; }

void init() {

  printf("Start initialization\r\n");

  /* Enable/disable sensors */
  hum_temp->disable();
  press_temp->disable();
  acc_gyro->disable_x();
  acc_gyro->disable_g();
  magnetometer->enable();
  accelerometer->enable();

  // Correcte waarden naar het register schrijven (zo low-power mogelijk @ 10hz
  // continious mode) LP = 1 (low-power mode) ODR1, ODR0 = 00 (10hz) MD1, MD0 =
  // 00 (continious mode)
  magnetometer->write_reg(0x60, 0x10);
  // OFF_CANC = 1 (enables offset cancellation
  magnetometer->write_reg(0x61, 0x03);
  // No interrupt enabled (for testing purpose)
  magnetometer->write_reg(0x62, 0x00);
  // LPen = 1 (low-power mode)
  // Zen,Yen, Xen = 1 (X,Y,Z-axis enabled)
  magnetometer->write_reg(0x20, 0x2f);
  // BDU = 1 (continious update)
  magnetometer->write_reg(0x23, 0x80);

  /* Writing correct values to the registers
   * Initialize on board accelerometer LSM303AGR:
   *  - FS: 2G
   *  - ODR: 100Hz
   *  - operating mode: Normal mode
   */
  // accelerometer->write_reg(0x5f, 0x2d);

  /* Initialize on board magnetometer LSM303AGR:
   *   - FS: 50Gauss (always)
   *   - ODR: 100Hz
   *   - operating mode: High-resolution mode
   */
  // magnetometer->write_reg(0x2f, 0x60);

  printf("Finished initialization\r\n");

  uint8_t id;

  printf("\r\n--- Starting new run ---\r\n");
  magnetometer->read_id(&id);
  printf("LSM303AGR magnetometer            = 0x%X\r\n", id);
  accelerometer->read_id(&id);
  printf("LSM303AGR accelerometer           = 0x%X\r\n", id);
}

/*
 * Function to perform magnetometer offset cancellation.
 * This can be found in the datasheet of the LSM303AGR.
 * Extra information can be found in the following source:
 * https://github.com/kriswiner/MPU-6050/wiki/Simple-and-Effective-Magnetometer-Calibration
 */
void callibration() {

  int16_t axes[3];
  int16_t x_min, y_min, z_min;
  int16_t x_max, y_max, z_max;
  int16_t x_bias, y_bias, z_bias;

  printf("Start offset cancellation\r\n");

  magnetometer->get_m_axes_raw(axes);
  axes[0] = x_min = x_max;
  axes[1] = y_min = y_max;
  axes[2] = z_min = z_max;

  for (uint8_t i = 0; i < 127; ++i) {
    magnetometer->get_m_axes_raw(axes);

    if (axes[0] > x_max)
      x_max = axes[0];
    else if (axes[0] < x_min)
      x_min = axes[0];

    if (axes[1] > y_max)
      y_max = axes[1];
    else if (axes[1] < y_min)
      y_min = axes[1];

    if (axes[2] > z_max)
      z_max = axes[2];
    else if (axes[2] < z_min)
      z_min = axes[2];

    wait(0.1);
  }

  x_bias = (x_min + x_max) / 2;
  y_bias = (y_min + y_max) / 2;
  z_bias = (z_min + z_max) / 2;

  printf("bias x: %6ld, bias y: %6ld, bias z: %6ld\r\n", x_bias, y_bias,
         z_bias);
}

void calculate() {

  led = !led;

  int32_t axes_m[3];
  int32_t axes_a[3];
  float axes_aF[3];
  float axes_aT[3];
  float roll, pitch, x, y, h, dir;
  uint16_t dirUint16_t;

  // The sensitivity is already taken into account.
  magnetometer->get_m_axes(axes_m);
  accelerometer->get_x_axes(axes_a);
  axes_aF[0] = (float)axes_a[0];
  axes_aF[1] = (float)axes_a[1];
  axes_aF[2] = (float)axes_a[2];

  // Resolution is 2^(8) = 256
  axes_aT[0] = (axes_aF[0] * 2 / 256);
  axes_aT[1] = (axes_aF[1] * 2 / 256);
  axes_aT[2] = (axes_aF[2] * 2 / 256);

  // Some math to get the thing working
  roll = atan2f(axes_aT[1], axes_aT[2]);
  pitch =
      atan2f(-axes_aT[0], axes_aT[1] * sinf(roll) + axes_aT[2] * cosf(roll));

  x = axes_m[0] * cosf(pitch) +
      (axes_m[1] * sinf(roll) + axes_m[2] * cosf(roll)) * sinf(pitch);
  y = axes_m[2] * sinf(roll) - axes_m[1] * cosf(roll);

  // Convert to degrees, look in what quadrant the value lays and if it's
  // negative add 360° to achieve correct positioning.
  h = atan2f(y, x);
  dir = h * (180 / 3.1415);
  if (dir < 0)
    dir += 360;

  // Convert for transmission over DASH7
  dirUint16_t = (uint16_t)dir;

  printf("LSM303AGR [mag/mgauss]:  %6ld, %6ld, %6ld\r\n", axes_m[0], axes_m[1],
         axes_m[2]);
  printf("LSM303AGR [acc/mg]:  %6ld, %6ld, %6ld\r\n", axes_a[0], axes_a[1],
         axes_a[2]);
  printf("richting: %6ld graden\r\n", dirUint16_t);

  led = !led;
}

int main() {

  button_int.rise(&call_back);

  init();

  while (true) {
    if (wake_up) {
      calculate();
      wake_up = false;
    } else {
      printf("Entering sleep mode\r\n");
      sleep();
    }
  }
}
