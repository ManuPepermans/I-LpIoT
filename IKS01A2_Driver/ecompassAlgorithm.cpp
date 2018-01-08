/**
 * The ecompass algorithm, it does the following:
 * 		1. Read the data from the accelerometer and magnetomerer
 * 		2. Convert the data from integer to float
 * 		3. Apply the resolution on the accelerometer data
 * 		4. Calculate the phi (roll angle) value
 * 		5. Calculate the  theta (pitch angle or attitude) value
 * 		6. Calculate of the Psi (yaw angle or heading) value
 *
 * @return the value of the heading in uint16_t format.
 */
uint16_t ecompassAlgorithm() {

	printf("Start ecompassAlgorithm\r\n");

//	LSM303AGR_wakeUpAccelerometer();
//	LSM303AGR_wakeUpMagnetometer();
//	HAL_Delay(100);
	LSM303AGR_ACC_readAccelerationData(lsm303agrAccData);
	LSM303AGR_MAG_readMagneticData(lsm303agrMagData);
//	LSM303AGR_powerDownAccelerometer();
//	LSM303AGR_powerDownMagnetometer();

	printf("Data accelerometer: %ld %ld %ld\r\n", lsm303agrAccData[0],
			lsm303agrAccData[1], lsm303agrAccData[2]);
	printf("Data Magnetometer: %ld %ld %ld\r\n", lsm303agrMagData[0],
			lsm303agrMagData[1], lsm303agrMagData[2]);

	printf("---------------------------------------------------------\r\n");

//Casting from integer to float (6 castings necessary)
	lsm303agrAccDataFloat[0] = lsm303agrAccData[0];
	lsm303agrAccDataFloat[1] = lsm303agrAccData[1];
	lsm303agrAccDataFloat[2] = lsm303agrAccData[2];

	lsm303agrMagDataFloat[0] = lsm303agrMagData[0];
	lsm303agrMagDataFloat[1] = lsm303agrMagData[1];
	lsm303agrMagDataFloat[2] = lsm303agrMagData[2];

	printf("Data accelerometer: %0.2f %0.2f %0.2f\r\n",
			lsm303agrAccDataFloat[0], lsm303agrAccDataFloat[1],
			lsm303agrAccDataFloat[2]);
	printf("Data Magnetometer: %0.2f %0.2f %0.2f\r\n", lsm303agrMagDataFloat[0],
			lsm303agrMagDataFloat[1], lsm303agrMagDataFloat[2]);

	printf("---------------------------------------------------------\r\n");

	lsm303agrAccDataFloat[0] = (lsm303agrAccDataFloat[0] * 2)
			/ pow(2, (resolution - 1));
	lsm303agrAccDataFloat[1] = (lsm303agrAccDataFloat[1] * 2)
			/ pow(2, (resolution - 1));
	lsm303agrAccDataFloat[2] = (lsm303agrAccDataFloat[2] * 2)
			/ pow(2, (resolution - 1));

	//Computation of Phi (roll angle) in radians and degrees
	rollRad = atan2f(lsm303agrAccDataFloat[1], lsm303agrAccDataFloat[2]);
	rollDegree = rollRad * (180 / PI);

	//Computation of Theta (pitch angle or attitude) in radians and degrees
	lsm303agrAccDataTemp = lsm303agrAccData[1] * sinf(rollRad)
			+ lsm303agrAccData[2] * cosf(rollRad);
	pitchRad = atan2f(-lsm303agrAccData[0], lsm303agrAccDataTemp);
	pitchDegree = pitchRad * (180 / PI);

	printf("Phi (roll angle, in degrees) is: %0.2f\r\n", rollDegree);
	printf("Theta (pitch angle or attitude, in degrees) is: %0.2f\r\n",
			pitchDegree);

	//Computation of Psi (yaw angle, or heading)
	yawTemp[0] = lsm303agrMagDataFloat[2] * sinf(rollRad)
			- lsm303agrMagDataFloat[1] * cosf(rollRad);
	yawTemp[1] = lsm303agrMagDataFloat[1] * sinf(rollRad)
			+ lsm303agrMagDataFloat[2] * cosf(rollRad);
	yawTemp[2] = lsm303agrMagDataFloat[0] * cosf(pitchRad)
			+ yawTemp[1] * sinf(pitchRad);
	yawRad = atan2f(yawTemp[0], yawTemp[2]);
	yawDegree = yawRad * (180 / PI);

//	x_direction = lsm303agrMagDataFloat[0] * cosf(pitchRad)
//			+ (lsm303agrMagDataFloat[1] * sinf(rollRad)
//					+ lsm303agrMagDataFloat[2] * cosf(rollRad))
//					* sinf(pitchRad);
//	y_direction = lsm303agrMagDataFloat[2] * sinf(rollRad)
//			- lsm303agrMagDataFloat[1] * cosf(rollRad);
//	yawRad = atan2f(y_direction, x_direction);
//	yawDegree = yawRad * (180 / PI);

	if (yawDegree < 0)
		yawDegree += 360;

	printf("Psi (yaw angle or heading, in degrees) is: %0.2f\r\n", yawDegree);

	printf("Finished ecompassAlgorithm\r\n");
	printf("---------------------------------------------------------\r\n");

	return (uint16_t) yawDegree;

}