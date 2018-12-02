#include<M5Stack.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include"ShakingManager.hpp"

namespace hardware{


//From sample program
ShakingManager::ShakingManager(int _threshold_swing_angle_axis) :
	threshold_swing_angle_axis(_threshold_swing_angle_axis),
	count(0),
	sampling_period(100),
	shaking_state(ShakingState::Stop)
{

	Wire.begin();

	// Read the WHO_AM_I register, this is a good test of communication
	byte c = IMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
	Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
	Serial.print(" I should be "); Serial.println(0x71, HEX);

	// if (c == 0x71) // WHO_AM_I should always be 0x68
	{
		Serial.println("MPU9250 is online...");

		// Start by performing self test and reporting values
		IMU.MPU9250SelfTest(IMU.SelfTest);
		Serial.print("x-axis self test: acceleration trim within : ");
		Serial.print(IMU.SelfTest[0],1); Serial.println("% of factory value");
		Serial.print("y-axis self test: acceleration trim within : ");
		Serial.print(IMU.SelfTest[1],1); Serial.println("% of factory value");
		Serial.print("z-axis self test: acceleration trim within : ");
		Serial.print(IMU.SelfTest[2],1); Serial.println("% of factory value");
		Serial.print("x-axis self test: gyration trim within : ");
		Serial.print(IMU.SelfTest[3],1); Serial.println("% of factory value");
		Serial.print("y-axis self test: gyration trim within : ");
		Serial.print(IMU.SelfTest[4],1); Serial.println("% of factory value");
		Serial.print("z-axis self test: gyration trim within : ");
		Serial.print(IMU.SelfTest[5],1); Serial.println("% of factory value");

		// Calibrate gyro and accelerometers, load biases in bias registers
		IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);

		IMU.initMPU9250();
		// Initialize device for active mode read of acclerometer, gyroscope, and
		// temperature
		Serial.println("MPU9250 initialized for active data mode....");

		// Read the WHO_AM_I register of the magnetometer, this is a good test of
		// communication
		byte d = IMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
		Serial.print("AK8963 "); Serial.print("I AM "); Serial.print(d, HEX);
		Serial.print(" I should be "); Serial.println(0x48, HEX);


		// Get magnetometer calibration from AK8963 ROM
		IMU.initAK8963(IMU.magCalibration);
		// Initialize device for active mode read of magnetometer
		Serial.println("AK8963 initialized for active data mode....");
		if (Serial)
		{
			//  Serial.println("Calibration values: ");
			Serial.print("X-Axis sensitivity adjustment value ");
			Serial.println(IMU.magCalibration[0], 2);
			Serial.print("Y-Axis sensitivity adjustment value ");
			Serial.println(IMU.magCalibration[1], 2);
			Serial.print("Z-Axis sensitivity adjustment value ");
			Serial.println(IMU.magCalibration[2], 2);
		}

	} // if (c == 0x71)
	// else
	// {
	//   Serial.print("Could not connect to MPU9250: 0x");
	//   Serial.println(c, HEX);
	//   while(1) ; // Loop forever if communication doesn't happen
	// }
}


//From Sample Program
void ShakingManager::updateMeasurement()
{
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {  
    IMU.readAccelData(IMU.accelCount);  // Read the x/y/z adc values
    IMU.getAres();

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    IMU.ax = (float)IMU.accelCount[0]*IMU.aRes; // - accelBias[0];
    IMU.ay = (float)IMU.accelCount[1]*IMU.aRes; // - accelBias[1];
    IMU.az = (float)IMU.accelCount[2]*IMU.aRes; // - accelBias[2];

    IMU.readGyroData(IMU.gyroCount);  // Read the x/y/z adc values
    IMU.getGres();

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    IMU.gx = (float)IMU.gyroCount[0]*IMU.gRes;
    IMU.gy = (float)IMU.gyroCount[1]*IMU.gRes;
    IMU.gz = (float)IMU.gyroCount[2]*IMU.gRes;

    IMU.readMagData(IMU.magCount);  // Read the x/y/z adc values
    IMU.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    IMU.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    IMU.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    IMU.magbias[2] = +125.;

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    IMU.mx = (float)IMU.magCount[0]*IMU.mRes*IMU.magCalibration[0] -
               IMU.magbias[0];
    IMU.my = (float)IMU.magCount[1]*IMU.mRes*IMU.magCalibration[1] -
               IMU.magbias[1];
    IMU.mz = (float)IMU.magCount[2]*IMU.mRes*IMU.magCalibration[2] -
               IMU.magbias[2];
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  IMU.updateTime();

}


void ShakingManager::updateCount(){

		//とりあえずz方向の角速度のみを使って検知
		auto swing_angle_velocity = IMU.gz;

		switch(shaking_state){

			case ShakingState::CountingUpperSwing:

				if(swing_angle_velocity > threshold_swing_angle_axis){
					count++;
					shaking_state = ShakingState::CountingLowerSwing;
				}
				break;

			case ShakingState::CountingLowerSwing:
				
				if(swing_angle_velocity < - threshold_swing_angle_axis){
					count++;
					shaking_state = ShakingState::CountingUpperSwing;
				}
				break;

			case ShakingState::Stop:
				break;
		}
}


};