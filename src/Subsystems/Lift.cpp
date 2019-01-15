#include "Lift.h"
#include "../RobotMap.h"
#include "CommandBase.h"
#include "Robot.h"
#include "Dashboard.h"

Lift::Lift() : Subsystem("Lift") {																// Create objects
	m_liftMotorBot	= new VictorSP(pwmLiftBottom);
	m_liftMotorTop	= new VictorSP(pwmLiftTop);

	m_liftPID		= new PIDControl();

	m_liftPotTop	= new AnalogInput(aiLiftPotTop);
	m_liftPotBot	= new AnalogInput(aiLiftPotBottom);

	m_liftMotorBot->SetInverted(true);															// Set motor controller outputs and
	m_liftMotorBot->SetSafetyEnabled(true);														// safety mode
	m_liftMotorBot->SetExpiration(0.1);

	m_liftMotorTop->SetInverted(true);
	m_liftMotorTop->SetSafetyEnabled(true);
	m_liftMotorTop->SetExpiration(0.1);

	m_liftPID->SetInputRange(0, 100.0);															// Set PID input and output ranges
	m_liftPID->SetOutputRange(-0.4, 1);
	m_liftPID->SetOutputRamp(0.2, 0.05);

	m_liftPotTop->SetAverageBits(2);															// Set potentiometer input options
	m_liftPotTop->SetOversampleBits(0);

	m_liftPotBot->SetAverageBits(2);
	m_liftPotBot->SetOversampleBits(0);

	m_autoArmTilt		= false;																// Initialize global variables
	m_autoArmTiltAngle  = 0;
	m_autoArmStow   	= true;
	m_liftDirection		= liftDown;
	m_liftSetpoint		= GetLiftPosition(potTop);
	m_potInUse			= potTop;
	m_useJoystick		= false;
	m_waitForArm		= false;

	SetLiftDirection(liftUp);																	// Initialize Lift PID
	m_liftPID->SetSetpoint(m_liftSetpoint);
}

double Lift::GetLiftPosition(LiftPot pot) {														// Get Lift Position in Inches for a
	double position = 0;																		// specific pot or the pot in use

	if (pot == potTop || (pot == potInUse && m_potInUse == potTop)) {
		position = ((double)m_liftPotTop->GetAverageValue() / COUNT_PER_INCH) - Robot::dashboard->GetDashValue(dvLiftPotTopOffset);
	} else if (pot == potBot || (pot == potInUse && m_potInUse == potBot)) {
		position = ((double)m_liftPotBot->GetAverageValue() / COUNT_PER_INCH) - Robot::dashboard->GetDashValue(dvLiftPotBotOffset);
	}

	return position;
}

double Lift::GetLiftRateOfChange(LiftPot pot) {													// Get Lift rate-of-change based on
	static double prevPositionBot = GetLiftPosition(potBot);									// specific pot or pot in use
	static double prevPositionTop = GetLiftPosition(potTop);
	double rateOfChange = 0;

	if (pot == potNone) {																		// Reset previous positions used to
		prevPositionBot = GetLiftPosition(potBot);												// calculate rate-of-change
		prevPositionTop = GetLiftPosition(potTop);

	} else if (pot == potTop || (pot == potInUse && m_potInUse == potTop)) {					// Rate-of-change (per 20ms) =
		rateOfChange = fabs(GetLiftPosition(potTop) - prevPositionTop);							// (current position - position last periodic)

	} else if (pot == potBot || (pot == potInUse && m_potInUse == potBot)) {
		rateOfChange = fabs(GetLiftPosition(potBot) - prevPositionBot);
	}

	return rateOfChange;
}

double Lift::GetLiftSetpoint(){																	// Get current Lift PID setpoint
	return m_liftSetpoint;
}

bool Lift::GetUseJoystick() {																	// Get Joystick in use
	return m_useJoystick;
}

void Lift::InitDefaultCommand() {
}

bool Lift::InArmDownZone() {																	// Determine if Lift in zone that
	double 	liftPosition = GetLiftPosition(potInUse);											// requires Intake Arm to be down
	return (liftPosition > armDownLow && liftPosition < armDownHigh);
}

bool Lift::LiftNearTarget() {																	// Lift is near Setpoint Target
	return (fabs(m_liftPID->GetError()) < 6);
}

void Lift::LiftPeriodic() {																		// Lift periodic called every 20ms
	bool			armDown = CommandBase::cubeIntake->ArmIsDown();
	LiftDirection 	liftDirection = liftUp;
	bool			liftHold = false;
	double 			liftPosition = GetLiftPosition(potInUse);
	double 			liftSpeed = 0;
	static int 		stallCounter = 0;

	if (m_useJoystick) {																		// Use joystick to run Lift
		m_autoArmTilt = false;
		liftSpeed = CommandBase::oi->GetLiftY();												// Get joystick value

		if (liftSpeed < 0) {																	// Move Lift down
			if (!armDown && (liftPosition > Robot::dashboard->GetDashValue(dvLiftSwitch))) {	// Hold if Arm not down
				liftHold = true;
				CommandBase::cubeIntake->SetArmPosition(dvIntakePickup);

			} else {
				liftSpeed /= 2;																	// Change joystick range to -0.5 to +0.5
				liftDirection = liftDown;

				m_liftSetpoint = liftPosition;													// Set PID setpoint to current position
				m_liftPID->SetSetpoint(m_liftSetpoint);

				if (armDown && liftPosition < 24) 												// Move Intake Arm to Stored
					CommandBase::cubeIntake->SetArmPosition(dvIntakeStow);

				if (liftPosition <= Robot::dashboard->GetDashValue(dvLiftLowLimit) + 1) {		// Turn off when at bottom
					liftSpeed = 0;
				} else if (liftPosition <= Robot::dashboard->GetDashValue(dvLiftLowThres)) {	// Limit output when near bottom
					liftSpeed /= 2;
				}
			}

		} else {																				// Move Lift Up
			if (!armDown && (liftSpeed > 0.1) &&
					(liftPosition < Robot::dashboard->GetDashValue(dvLiftSwitch))) {			// Hold if Arm not down
				liftHold = true;
				CommandBase::cubeIntake->SetArmPosition(dvIntakePickup);

			} else {
				liftDirection = liftUp;

				if (liftSpeed > m_liftPID->Calculate(liftPosition, false) &&					// Use joystick value if greater than
						m_liftSetpoint < Robot::dashboard->GetDashValue(dvLiftHighLimit)) {		// PID output and below high limit
					if (liftPosition >= Robot::dashboard->GetDashValue(dvLiftHighThres)) 		// Limit output when near top
						liftSpeed /= 2;

					m_liftSetpoint = liftPosition;
					m_liftPID->SetSetpoint(m_liftSetpoint);

				} else {
					liftHold = true;
				}
			}
		}

		if (liftHold) {																			// Use PID to Hold position
			liftDirection = liftUp;
			liftSpeed = m_liftPID->Calculate(liftPosition, false);
		}

		Robot::dashboard->SetRobotValue(rvLiftIndex, -1);

	} else {
		if (m_waitForArm && armDown) {															// Intake Arm is now down
			m_waitForArm = false;																// Set setpoint to requested target
			m_liftPID->SetSetpoint(m_liftSetpoint);

			if (m_liftSetpoint > liftPosition) SetLiftDirection(liftUp);
			else							   SetLiftDirection(liftDown);
		}

		liftDirection = m_liftDirection;
		liftSpeed = m_liftPID->Calculate(liftPosition, false);

		if (m_liftSetpoint < 2 && liftPosition < 2) {											// Turn Off if near bottom
			liftSpeed = 0;
		} else if (fabs(m_liftPID->GetError()) < 1) {											// Change direction to Up when near setpoint
			SetLiftDirection(liftUp);
			m_autoArmTilt = false;
		}

		if (m_autoArmTilt && liftDirection == liftUp && liftPosition > armDownHigh) {
			m_autoArmTilt = false;
			CommandBase::cubeIntake->SetArmPosition(m_autoArmTiltAngle);
		}

		if (m_autoArmStow) {
			if (armDown && liftSpeed < 0 && m_liftSetpoint < 5 && liftPosition < 24)			// Move intake arm to stow position
				CommandBase::cubeIntake->SetArmPosition(dvIntakeStow);
		}
	}

	m_liftMotorTop->Set(liftSpeed);																// Always run Top motor

	if (liftDirection == liftUp) {																// Only run bottom motor when moving up
		m_liftMotorBot->Set(liftSpeed);
	} else {
		m_liftMotorBot->Set(0);
	}

	if (liftPosition > 34 && liftDirection == liftUp) {
		Robot::robotCamera->SetCameraView(RobotCamera::cvScale);
	} else if (liftPosition < 26 && liftDirection == liftDown) {
		Robot::robotCamera->SetCameraView(RobotCamera::cvFloor);
	}

	Robot::dashboard->SetRobotStatus(rsLiftDirectionUp, (liftDirection == liftUp));

	if ((Robot::robotLog->GetPdpCurrent(pdpLiftMotorTop) >= 8) && (GetLiftRateOfChange(potInUse) < 0.4)) {  // Stall detection
		stallCounter++;
		if (stallCounter == 5) Robot::robotLog->Write("Lift:   Stall Detected");

	} else {
		if (stallCounter >= 5) {
			sprintf(m_log, "Lift:   Stall Duration=%d msec", stallCounter * 20);
			Robot::robotLog->Write(m_log);
		}
		stallCounter = 0;
	}

	GetLiftRateOfChange(potNone);																// Set previous lift positions
//	PotCheck();																					// Rationality Check Pot
}

std::string Lift::LiftPositionName(int index) {													// Return name of Lift position
	switch (index){
		case dvLiftTravel:		return "Travel";
		case dvLiftSwitch: 		return "Switch";
		case dvLiftScaleBot:	return "Scale-Low";
		case dvLiftScaleMid: 	return "Scale-Balanced";
		case dvLiftScaleTop:	return "Scale-High";
		default:				return "?";
	}
}

void Lift::PotCheck() {
	static int clearCount = 0;
	static bool potFailed = false;
	double potTopChange = 0;
	double potBotChange = 0;
	double potMisMatch  = fabs(GetLiftPosition(potTop) - GetLiftPosition(potBot));

	if (potMisMatch > 1) {
		if (!potFailed) {
			potFailed = true;
			clearCount = 0;
			potTopChange = GetLiftRateOfChange(potTop);
			potBotChange = GetLiftRateOfChange(potBot);

			sprintf(m_log, "Lift:   Pot Check Mismatch=%3.1f  Top ROC=%6.4f  Bottom ROC=%6.4f",
					potMisMatch, potTopChange, potBotChange);
			Robot::robotLog->Write(m_log);
		}
	} else if (potFailed) {
		clearCount++;
		if (clearCount >= 250) potFailed = false;
	}
}

void Lift::SetAutoArmTilt(bool enable, double angle) {
	if (enable && m_liftSetpoint > Robot::dashboard->GetDashValue(dvLiftScaleBot) - 4) {
		m_autoArmTilt = true;
		m_autoArmTiltAngle = angle;
	} else {
		m_autoArmTilt = false;
	}
}

void Lift::SetAutoArmStow(bool enable) {
	m_autoArmStow = enable;
}

void Lift::SetDashboardValue() {																// Set Lift values in dashboard database
	static int ampCount = 1;

	Robot::dashboard->SetRobotValue(rvLiftPosition, GetLiftPosition(potInUse));
	Robot::dashboard->SetRobotValue(rvLiftSetpoint, m_liftSetpoint);
	Robot::dashboard->SetRobotValue(rvLiftPwm, m_liftMotorTop->Get());
	Robot::dashboard->SetRobotValue(rvLiftPotTop, GetLiftPosition(potTop));
	Robot::dashboard->SetRobotValue(rvLiftPotBot, GetLiftPosition(potBot));

	ampCount--;
	if (ampCount <= 0) {
		Robot::dashboard->SetRobotValue(rvLiftAmpsTop, Robot::robotLog->GetPdpCurrent(pdpLiftMotorTop));
		Robot::dashboard->SetRobotValue(rvLiftAmpsBottom, Robot::robotLog->GetPdpCurrent(pdpLiftMotorBot));
		ampCount = 5;
	}
}

void Lift::SetLiftDirection(LiftDirection direction) {											// Set PID coefficients based on
	if (m_liftDirection != direction) {
		m_liftDirection = direction;

		if (direction == liftUp) {
			m_liftPID->SetCoefficient('P', 2, 0.11,  0.05);
			m_liftPID->SetCoefficient('I', 2, 0.002, 0.001);
			m_liftPID->SetCoefficient('D', 4, 0.1,   0);
		} else {
			m_liftPID->SetCoefficient('P', 0, 0.11, 0);
			m_liftPID->SetCoefficient('I', 0, 0.0,  0);
			m_liftPID->SetCoefficient('D', 0, 0.65, 0);
		}
	}
}

void Lift::SetLiftPosition(int positionIndex) {													// Set new Lift setpoint based on
	double setpoint = Robot::dashboard->GetDashValue(positionIndex);							// a defined position index
	SetLiftPosition(setpoint, positionIndex);
}

void Lift::SetLiftPosition(double setpoint, int positionIndex) {								// Set new Lift setpoint
	double liftPosition = GetLiftPosition(potInUse);

	m_waitForArm = false;

	if (setpoint < 0 ) setpoint = 0;															// Limit to range of Lift positions
	else if (setpoint > Robot::dashboard->GetDashValue(dvLiftHighLimit)) setpoint = Robot::dashboard->GetDashValue(dvLiftHighLimit);

	m_liftSetpoint = setpoint;

	if (!CommandBase::cubeIntake->ArmIsDown()) {												// Arm is NOT Down
		if (liftPosition < 15 && setpoint > 24) {
			CommandBase::cubeIntake->SetArmPosition(dvIntakePickup);
		} else if (liftPosition < setpoint && setpoint > 24) {
			CommandBase::cubeIntake->SetArmPosition(dvIntakePickup);
			m_waitForArm = true;
		} else if (liftPosition > setpoint && liftPosition > 36) {
			CommandBase::cubeIntake->SetArmPosition(dvIntakePickup);
			m_waitForArm = true;
		}
	}

	if (m_waitForArm) {
		SetLiftDirection(liftUp);
		setpoint = liftPosition;
	} else if (setpoint < liftPosition) {
		SetLiftDirection(liftDown);
	} else {
		SetLiftDirection(liftUp);
	}

	m_liftPID->SetSetpoint(setpoint);														// Set PID setpoint

	if (setpoint > 34) {
		Robot::robotCamera->SetCameraView(RobotCamera::cvScale);
	} else if (setpoint < 26) {
		Robot::robotCamera->SetCameraView(RobotCamera::cvFloor);
	}

	if (positionIndex < 0) {
		if (m_waitForArm) {
			sprintf(m_log, "Lift:   Lift to %3.0f [Wait for Arm]", m_liftSetpoint);
		} else {
			sprintf(m_log, "Lift:   Lift to %3.0f", m_liftSetpoint);
		}
	} else if (m_waitForArm) {
		sprintf(m_log, "Lift:   Lift to %3.0f (%s) [Wait for Arm]", m_liftSetpoint,
				        LiftPositionName(positionIndex).c_str());
	} else {
		sprintf(m_log, "Lift:   Lift to %3.0f (%s)", m_liftSetpoint,
				        LiftPositionName(positionIndex).c_str());
	}

	Robot::robotLog->Write(m_log);
	Robot::dashboard->SetRobotValue(rvLiftIndex, (double) positionIndex);
}

void Lift::SetUseJoystick(bool useJoystick) {													// Set UseJoystick flag
	m_useJoystick = useJoystick;

	if (m_useJoystick) {																		// Use Joystick
		SetLiftDirection(liftUp);
		m_liftSetpoint = GetLiftPosition(potInUse);												// Set PID setpoint to current position
		m_liftPID->SetSetpoint(m_liftSetpoint);
	}
}

void Lift::TuneLiftPID(double speed, LiftDirection direction) {									// Used to Tune Lift PID in Test mode
	m_liftSetpoint = GetLiftPosition(potInUse);
	m_liftPID->SetSetpoint(m_liftSetpoint);

	m_liftMotorTop->Set(speed);

	if (m_liftDirection == liftUp) {
		m_liftMotorBot->Set(speed);
	} else {
		m_liftMotorBot->Set(0);
	}
}
