#include <Arduino.h>
#include <SPI.h>
#include <MPU9250.h>
#include <QueueArray.h>
//#include <ws2812.h>
#include <StopWatch.h>


#define SPI_CLOCK 8000000  // 8MHz clock works.
#define SS_PIN   21

//const int LED_DATA_PIN = 15;
//const uint16_t LED_COUNT = 3;
//const int LED_BRIGHTNESS = 255;

//rgbVal *pixels;

const int SERIAL_LED = 15;
const int SYNC_LED = 14;

HardwareSerial SerialBLE(0);

MPU9250 mpu(SPI_CLOCK, SS_PIN);

QueueArray <float> gyro0;
QueueArray <float> gyro1;
QueueArray <float> gyro2;

int timestampMS = 0;
StopWatch stopWatch = StopWatch(false);

struct PACKET
{
	int timestamp;

	uint8_t gyroX;
	uint8_t gyroY;
	uint8_t gyroZ;

	uint8_t accelX;
	uint8_t accelY;
	uint8_t accelZ;
};

void sendPacket(PACKET packet)
{
	Serial.write((uint8_t *) &packet, (uint16_t) sizeof(PACKET));
}


union ByteTimestamp{
	byte array[10];
	uint32_t timestamp;
};

void sync()
{
	digitalWrite(SYNC_LED, HIGH);
	delay(500);
	digitalWrite(SYNC_LED, LOW);

	SerialBLE.flush();

	while(!SerialBLE.available()){};

	char buffer[] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}; // Receive up to 10 bytes

	while (Serial.available()) {
		Serial.readBytesUntil('n', buffer, 12);
		int incomingValue = atoi(buffer);
		Serial.println(incomingValue);
	}

	//timestampMS = SerialBLE.readBytesUntil("\n");
	timestampMS = atoi(buffer);
	stopWatch.start();

	Serial.print(timestampMS);
	delay(300);

	digitalWrite(SYNC_LED, HIGH);
}

uint8_t computeAverage(float value, QueueArray<float> &values)
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

	return static_cast<uint8_t>(sum / size);
}


void setSerialLed(bool on)
{
	on ? digitalWrite(SERIAL_LED, HIGH) : digitalWrite(SERIAL_LED, LOW);
}

void setup() {
	initArduino();


	//Init communication

	Serial.begin(115200);
	SerialBLE.begin(115200, SERIAL_8N1, 2, 4);

	/*
	 * SDA mosi
	 * SCl SCK
	 * SDO/SA0 MISO
	 * SS mpu  = ncs
	 * baro ss = csb
	 */
	SPI.begin(16, 18, 17, 21);


	//End init communication

	//Init leds

	pinMode(SERIAL_LED, OUTPUT);
	pinMode(SYNC_LED, OUTPUT);

	digitalWrite(SYNC_LED, LOW);
	digitalWrite(SERIAL_LED, LOW);

/*	if(ws2812_init(LED_DATA_PIN, LED_WS2812B)) {
		Serial.println("Init FAILURE: halting");
		while (true) {};
	}

	pixels = (rgbVal*)malloc(sizeof(rgbVal) * LED_COUNT);

	pixels[0] = makeRGBVal(2, 1, 3);
	pixels[1] = makeRGBVal(2, 1, 3);
	pixels[2] = makeRGBVal(2, 1, 3);
	ws2812_setColors(LED_COUNT, pixels);
 */
	//End init leds

	//Init sensors

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


	//End init sensors

	//Sync with host
	sync();
}

void loop() {

	/*
	 * 0 -> X axis
	 * 1 -> Y axis
	 * 2 -> Z axis
	*/
	mpu.read_gyro();
	mpu.read_acc();

	PACKET packet = {
			timestampMS + stopWatch.getMs(),
			computeAverage(mpu.gyro_data[0], gyro0),
	        computeAverage(mpu.gyro_data[1], gyro1),
	        computeAverage(mpu.gyro_data[2], gyro2),
			computeAverage(mpu.accel_data[0], gyro0),
			computeAverage(mpu.accel_data[1], gyro1),
			computeAverage(mpu.accel_data[2], gyro2)
	};

	sendPacket(packet);
	/*SerialBLE.print(computeAverage(mpu.gyro_data[0], gyro0));   Serial.print('\t');
	SerialBLE.print(computeAverage(mpu.gyro_data[1], gyro1));   Serial.print('\t');
	SerialBLE.print(computeAverage(mpu.gyro_data[2], gyro2));   Serial.print('\n');
	SerialBLE.print(gyro1.count());   Serial.print('\n');
	Serial.print(gyro1.count());   Serial.print('\t');
	 Serial.print(mpu.accel_data[0]);  Serial.print('\t');
	 Serial.print(mpu.accel_data[1]);  Serial.print('\t');
	 Serial.print(mpu.accel_data[2]);  Serial.print('\t');
	 Serial.print(mpu.mag_data[0]);    Serial.print('\t');
	 Serial.print(mpu.mag_data[1]);    Serial.print('\t');
	 Serial.print(mpu.mag_data[2]);    Serial.print('\t');*/
	delay(100);
}
