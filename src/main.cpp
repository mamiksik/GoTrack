#include <Arduino.h>
#include <SPI.h>
#include <MPU9250.h>
#include <QueueArray.h>


#define WAITFORINPUT(){            \
	while(!Serial.available()){};  \
	while(Serial.available()){     \
		Serial.read();             \
	};                             \
}                                  \

#define SPI_CLOCK 8000000  // 8MHz clock works.
#define SS_PIN   21


MPU9250 mpu(SPI_CLOCK, SS_PIN);

QueueArray <float> gyro0;
QueueArray <float> gyro1;
QueueArray <float> gyro2;

float computeAverage(float value, QueueArray<float> &values)
{
	if (values.count() >= 100){
		values.pop();
	}

	values.push(value);
	int size = values.count();
	float sum = 0;

	for(int i = 0; i <= size; i++){
		float val = values.pop();
		sum += val;
		values.push(val);
	}

	return sum / size;
}



void setup() {
	initArduino();
	Serial.begin(115200);

	//SPI.begin(int8_t sck, optional int8_t miso, optional int8_t mosi, optional int8_t ss)();
	//SDA mosi
	//SCl SCK
	//SDO/SA0 MISO
	//SS mpu  = ncs
	//baro ss = csb
	SPI.begin(16, 18, 17, 21);

	Serial.println("Press any key to continue");
	WAITFORINPUT();

	mpu.init(true);

	int wai = mpu.whoami();
	if (wai == 0x71){
		Serial.println("Successful connection");
	}
	else{
		Serial.print("Failed connection: ");
		Serial.println(wai, HEX);
	}

	uint8_t wai_AK8963 = mpu.AK8963_whoami();
	if (wai_AK8963 == 0x48){
		Serial.println("Successful connection to mag");
	}
	else{
		Serial.print("Failed connection to mag: ");
		Serial.println(wai_AK8963, HEX);
	}

	mpu.calib_acc();
	mpu.calib_mag();

	Serial.println("Send any char to begin main loop.");
	WAITFORINPUT();
	Serial.println("Run");
}

void loop() {
	//mpu.read_all();
	mpu.read_gyro();

	Serial.print(computeAverage(mpu.gyro_data[0], gyro0));   Serial.print('\t');
	Serial.print(computeAverage(mpu.gyro_data[1], gyro1));   Serial.print('\t');
	Serial.print(computeAverage(mpu.gyro_data[2], gyro2));   Serial.print('\t');
	Serial.print(gyro1.count());   Serial.print('\t');
	// Serial.print(mpu.accel_data[0]);  Serial.print('\t');
	// Serial.print(mpu.accel_data[1]);  Serial.print('\t');
	// Serial.print(mpu.accel_data[2]);  Serial.print('\t');
	// Serial.print(mpu.mag_data[0]);    Serial.print('\t');
	// Serial.print(mpu.mag_data[1]);    Serial.print('\t');
	// Serial.print(mpu.mag_data[2]);    Serial.print('\t');
	Serial.println(mpu.temperature);

	delay(10);
}
