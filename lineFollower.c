#pragma config(Sensor, S3,     lightSensor,    sensorLightActive)
#pragma config(Motor,  motorA,          rightWheel,    tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          leftWheel,     tmotorNXT, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
int black = 25;
int white = 40;
TFileHandle fileHandle;
TFileIOResult IOResult;
string fileName = "movements.txt";
void turnToTrack();
void continueTrack();

bool isBlack() {
	return abs(SensorValue[lightSensor] - black) <= 2;
}

bool isWhite() {
	return abs(SensorValue[lightSensor] - white) <= 2;
}

void turnLeft(int power) {
	motor[leftWheel] = -power;
	motor[rightWheel] = power;
}

void turnRight(int power) {
	motor[leftWheel] = power;
	motor[rightWheel] = -power;
}

void goStraight(int power) {
	motor[leftWheel] = power;
	motor[rightWheel] = power;
}

void stopWheels() {
	motor[leftWheel] = 0;
	motor[rightWheel] = 0;
}

void writeMotion(int leftWheelSpeed, int rightWheelSpeed, int duration){
	byte CR = 0x13;
	byte LF = 0x10;
	word fileSize = 100;

	string motionString;
	sprintf(motionString, "%d %d %d", leftWheelSpeed, rightWheelSpeed, duration);

	OpenWrite(fileHandle, IOResult, fileName, fileSize);
	WriteText(fileHandle, IOResult, motionString);
	WriteByte(fileHandle, IOResult, CR);
	WriteByte(fileHandle, IOResult, LF);
	Close(fileHandle, IOResult);
}

void continueTrack() {
	clearTimer(T1);
	while(isBlack()) {
		goStraight(50);
	}
	int motionTime = time1[T1];
	writeMotion(50, 50, motionTime);
	turnToTrack();
}

void turnToTrack() {
	clearTimer(T1);
	// Customize tick to measure how long to check turn
	int tick = 0;
	clearTimer(T1);
	// Check left turn first for number of ticks
	while(isWhite() || tick < 50) {
		turnLeft(50);
		tick = tick + 1;
	}
	// If left turn worked, record in txt file and continue on path
	int leftMotionTime = time1[T1];
	if (isBlack()) {
		writeMotion(-50, 50, leftMotionTime);
		continueTrack();
	}
	// Else, we use a right turn and record that instead
	clearTimer(T1);
	while(isWhite()) {
		turnRight(50);
	}
	// Subtract left motion time to compensate for left turn adding time
	int rightMotionTime = time1[T1] - leftMotionTime;
	writeMotion(50, -50, rightMotionTime);
	continueTrack();
}
void init() {
	Delete(fileName, IOResult);
	stopWheels();
}

task main()
{
	init();
	continueTrack();
}
