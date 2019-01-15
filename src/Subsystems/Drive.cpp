#include "Drive.h"
#include "../RobotMap.h"
#include "Commands/CmdDriveJoystick.h"
#include "Robot.h"

Drive::Drive() : Subsystem("Drive") {
														//Initialize objects and variables
	m_encoderLeft	= new Encoder(dioEncoderLeftA, dioEncoderLeftB, false);
	m_encoderRight	= new Encoder(dioEncoderRightA, dioEncoderRightB, true);

	m_gyro			= new ADXRS450_Gyro();

	m_motorLeft1	= new VictorSP(pwmDriveLeft1);
	m_motorLeft2	= new VictorSP(pwmDriveLeft2);
	m_motorRight1	= new VictorSP(pwmDriveRight1);
	m_motorRight2	= new VictorSP(pwmDriveRight2);

	m_pidDrive 		= new PIDControl();
	m_pidRotate		= new PIDControl();

	m_solShifter 	= new Solenoid(solShift);

	m_encoderLeft->SetDistancePerPulse(.02220);			// Set encoder inches per pulse
	m_encoderRight->SetDistancePerPulse(.02220);		// 0.02283  0.02226

	m_pidDrive->SetCoefficient('P', 0, 0.04, 0);		// Set PID controller coefficients
	m_pidDrive->SetCoefficient('I', 12, 0, 0.001);		// WAS 6, 0, 0.0008
	m_pidDrive->SetCoefficient('D', 40, 0, 0.4);		// 30 0.5
	m_pidDrive->SetInputRange(-500.0, 500.0);
	m_pidDrive->SetOutputRange(-0.5, 0.5);
	m_pidDrive->SetOutputRamp(0.2, 0.02);
//	m_pidDrive->SetOutputRamp(0.2, 0.025);
//	m_pidDrive->SetOutputRamp(0.25, 0.05);
	m_pidDrive->SetSetpoint(0);

	m_pidRotate->SetCoefficient('P', 0, 0.04, 0);
	m_pidRotate->SetCoefficient('I', 10, 0, 0.0025);	// 0.0025
	m_pidRotate->SetCoefficient('D', 0, 0.25, 0);		// 0.1
	m_pidRotate->SetInputRange(-360.0, 360.0);
	m_pidRotate->SetOutputRange(-0.5, 0.5);
	m_pidRotate->SetOutputRamp(0.25, 0.05);
	m_pidRotate->SetSetpoint(0);

	m_motorLeft1->SetSafetyEnabled(true);
	m_motorLeft1->SetExpiration(0.1);
	m_motorLeft1->SetInverted(true);

	m_motorLeft2->SetSafetyEnabled(true);
	m_motorLeft2->SetExpiration(0.1);
	m_motorLeft2->SetInverted(true);

	m_motorRight1->SetSafetyEnabled(true);
	m_motorRight1->SetExpiration(0.1);

	m_motorRight2->SetSafetyEnabled(true);
	m_motorRight2->SetExpiration(0.1);

	m_solShifter->Set(false);

	m_shifterState  = shifterHigh;
	m_useEncoder	= encoderLeft;

	m_turn			= new turn[2];
	m_turnCount		= 0;
	m_turnIndex		= 0;
}

void Drive::ArcadeDrive(double drive, double rotation) {
	drive = PwmLimit(drive);
	rotation = PwmLimit(rotation);

	double maxInput = std::copysign(std::max(fabs(drive), fabs(rotation)), drive);

	double motorLeft;
	double motorRight;
														//Joystick Quadrants Check
	if (drive >= 0.0) {									//Check if drive is positive
		if (rotation >= 0.0) {							//Check if rotate is positive
			motorLeft = maxInput;						//Left motors have max input
			motorRight = drive - rotation;				//right motors have reverse output
		} else {
			motorLeft = drive + rotation;
			motorRight = maxInput;
		}
	} else {
		if (rotation >= 0.0) {
			motorLeft = drive + rotation;
			motorRight = maxInput;
		} else {
			motorLeft = maxInput;
			motorRight = drive - rotation;
		}
	}

	motorLeft = PwmLimit(motorLeft);
	motorRight = PwmLimit(motorRight);

	m_motorLeft1->Set(motorLeft);
	m_motorLeft2->Set(motorLeft);
	m_motorRight1->Set(motorRight);
	m_motorRight2->Set(motorRight);

	Robot::robotLog->CheckDriveCurrent();
}

void Drive::DrivePidAddTurn(double distance, double heading) {
	if (m_turnCount < 2) {
		m_turnCount++;
		m_turn[m_turnCount - 1].distance = fabs(distance);
		m_turn[m_turnCount - 1].heading = heading;
	}
}

void Drive::DrivePidClearTurn() {
	m_turnCount = 0;
	m_turnIndex = 0;
}

bool Drive::DrivePidFinished() {						//Drive PID finished when error is less than 1.0"
	return (fabs(m_pidDrive->GetError()) <= 1.0);
}

void Drive::DrivePidInit(double setpoint, double maxSpeed, UseEncoder encoder, bool resetEncoder) {
	m_pidDrive->SetSetpoint(setpoint);					//Initalize Drive PID controller
	m_pidDrive->SetOutputRange(-fabs(maxSpeed), fabs(maxSpeed));
	m_pidDrive->Reset();

	if (encoder != encoderInUse) m_useEncoder = encoder;

	if (resetEncoder) {									//Reset encoder
		m_encoderLeft->Reset();
		m_encoderRight->Reset();
	}
}

double Drive::DrivePidOutput(){
	double distance = GetDistance(encoderInUse);		//Get current distance from encoder

	if (m_turnCount > 0 && m_turnIndex < m_turnCount) {
		if (fabs(distance) >= m_turn[m_turnIndex].distance) {
			m_pidRotate->SetSetpoint(m_turn[m_turnIndex].heading);
			m_turnIndex++;
		}
	}

	return m_pidDrive->Calculate(distance, false);		// Return Drive PID output
}

void Drive::DrivePidTune() {
	m_useEncoder = encoderLeft;
	m_encoderLeft->Reset();
	m_encoderRight->Reset();
}

double Drive::GetAngle(){
	return	m_gyro->GetAngle();							//Return current gyro angle
}

double Drive::GetDistance(UseEncoder encoder) {			//Return current encoder distance
	double 		distance = 0;
	double 		left  = m_encoderLeft->GetDistance();
	double 		right = m_encoderRight->GetDistance();
	UseEncoder	source = encoder;

	if (source == encoderInUse) source = m_useEncoder;

	switch (source) {
		case encoderInUse:
		case encoderLeft:
			distance = left;
			break;
		case encoderRight:
			distance = right;
			break;
		case encoderAverage:
			if (fabs(left) < (fabs(right) * 0.9)) {
				distance = right;
			} else if (fabs(right) < (fabs(left) * 0.9)) {
				distance = left;
			} else {
				distance = (left + right) / 2;
			}
			break;
		default:;
	}

	return distance;
}

Drive::ShifterState Drive::GetShifterState() {
	return m_shifterState;
}

void Drive::InitDefaultCommand() {						//Command set to execute if no other
	SetDefaultCommand(new CmdDriveJoystick());			//command is running
}

bool Drive::RotatePidFinished() {						//Rotate PID finished when error is
	return (fabs(m_pidRotate->GetError()) <= 1);		//less than 0.5 degrees
}

void Drive::RotatePidInit (double setpoint, double maxSpeed, bool resetGyro) {
	m_pidRotate->SetSetpoint(setpoint);					//Initialize rotate PID controller
	m_pidRotate->SetOutputRange(-fabs(maxSpeed), fabs(maxSpeed));
	m_pidRotate->Reset();

	if (resetGyro) m_gyro->Reset();
}

double Drive::RotatePidOutput() {						//Return rotate PID output
	return m_pidRotate->Calculate(m_gyro->GetAngle(), false);
}

void  Drive::RotatePidSetpoint(double setpoint){		//Set rotate PID setpoint
	m_pidRotate->SetSetpoint(setpoint);
}

void Drive::RotatePidTune() {
	m_gyro->Reset();
}

double Drive::PwmLimit(double value) {
	if 		(value < -1.0) value = -1.0;
	else if (value > 1.0)  value = 1.0;
	return value;
}

void Drive::SetDashboardValues(){
	Robot::dashboard->SetRobotValue(rvGyro, m_gyro->GetAngle());
	Robot::dashboard->SetRobotValue(rvEncoderLeft, m_encoderLeft->GetDistance());
	Robot::dashboard->SetRobotValue(rvEncoderRight, m_encoderRight->GetDistance());
	Robot::dashboard->SetRobotValue(rvDriveLeftPwm, m_motorLeft1->Get());
	Robot::dashboard->SetRobotValue(rvDriveRightPwm, m_motorRight1->Get());
}

void Drive::SetShifterState(ShifterState state){
	 if(m_shifterState != state){
		 m_shifterState = state;
		 m_solShifter->Set(state == shifterLow);
		 Robot::dashboard->SetRobotStatus(rsDriveShiftState, (state == shifterLow));

		 if (m_shifterState == shifterLow) {
			 Robot::robotLog->Write("Drive:  Shift to Low");
		 } else {
			 Robot::robotLog->Write("Drive:  Shift to High");
		 }
	 }
 }
