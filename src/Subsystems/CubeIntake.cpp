#include "CubeIntake.h"
#include "../RobotMap.h"
#include "Robot.h"
#include "Dashboard.h"
#include "../CommandBase.h"

CubeIntake::CubeIntake() : Subsystem("CubeIntake") {
	m_armMotor			= new VictorSP(pwmArmMotor);
	m_armPID			= new PIDControl();
	m_armPot			= new AnalogInput(aiArmPot);

	m_cubeSensorNear	= new DigitalInput(dioCubeSensorNear);
	m_cubeSensorFar		= new DigitalInput(dioCubeSensorFar);

	m_rollerLeft		= new VictorSP(pwmRollerLeft);
	m_rollerRight		= new VictorSP(pwmRollerRight);

	m_solGrip			= new Solenoid(solGrip);
	m_solDrop			= new Solenoid(solDrop);

	m_armMotor->SetSafetyEnabled(true);
	m_armMotor->SetExpiration(0.1);
	m_armMotor->SetInverted(true);

	m_armPID->SetCoefficient('P', 9, 0.066, 0.033);
	m_armPID->SetCoefficient('I', 0, 0.0016, 0);
	m_armPID->SetCoefficient('D', 3, 0.066, 0);
	m_armPID->SetInputRange(0, 90.0);
	m_armPID->SetOutputRange(-0.4, 0.6);

	m_armPot->SetAverageBits(2);
	m_armPot->SetOversampleBits(0);

	m_rollerRight->SetInverted(true);

	m_armSetpoint		= GetArmPosition();
	m_earlyFinish		= false;
	m_ejectCount		= 0;
	m_gripCount			= 0;
	m_gripState			= gsOff;
	m_intakeMode		= imOff;
	m_launchSpeed		= 0;
	m_rollerState		= 0;
	m_useJoystick		= false;
	m_rollerJoystick	= false;

	m_armPID->SetSetpoint(m_armSetpoint);
	SetIntakeMode(imOff);

	Robot::dashboard->SetRobotValue(rvIntakeArmSetpoint, m_armSetpoint);
}

bool CubeIntake::ArmIsDown() {
	return (GetArmPosition() < 10 && m_armSetpoint < 10);
}

std::string CubeIntake::ArmPositionName(int positionIndex) {
	switch (positionIndex) {
		case dvIntakeStow:		return "Stow";
		case dvIntakePickup:	return "Pickup";
		case dvIntakeTilt:		return "Tilt";
		default:				return "?";
	}
}

bool CubeIntake::CubeDetected() {
	return !m_cubeSensorFar->Get();
}

bool CubeIntake::CubeOnboard() {
	return ((m_intakeMode == imOff) && !m_cubeSensorFar->Get() && m_gripCount <= 0);
}

void CubeIntake::CubePeriodic() {
	static int 	counterCubeRotate = 0;
	static int 	counterRollerIn   = 0;
	static int  counterRollerOff  = 0;
	static int  prevFlags 		  = 0;
	static int	rotateOption 	  = 0;

	int			cubeFlags 	= 0;
	double 		intakeSpeed = 0;
	double 		rollerLeft 	= 0;
	double 		rollerRight = 0;

	if (m_gripCount > 0) {
		m_gripCount--;
		if (m_gripCount <= 0 && m_gripState == gsClose) SetGripState(gsOff);
	}

	if(m_rollerJoystick){
		rollerLeft = -CommandBase::oi->GetLiftY() * 0.5;
		rollerRight = rollerLeft;

	} else {
		switch (m_intakeMode) {
			case imOff:
				counterCubeRotate = 0;
				counterRollerIn = 0;
				counterRollerOff = 0;
				rotateOption = 0;
				break;

			case imPickup:															// PICKUP CUBE
				//	cubeFlags: 	0 = No Cube in CubeIntake        	Binary: 00
				//  		 	1 = Cube Detected					Binary: 01
				//			 	2 = Cube Captured					Binary: 10
				//			 	3 = Cube Detected and Captured		Binary: 11

				if (CommandBase::oi->GetLightOverride()){
					cubeFlags = 3;
				} else {
					cubeFlags = (((m_cubeSensorNear->Get()) ? 0:1) << 1) | ((m_cubeSensorFar->Get()) ? 0:1);
				}

				intakeSpeed = Robot::dashboard->GetDashValue(dvIntakeInSpeed);

				switch (cubeFlags) {
					case 0:															// NO CUBE
						if (m_gripState == gsOff) {
							if (prevFlags != 0) Robot::robotLog->Write("   Cube None");
							Robot::dashboard->SetRobotStatus(rsCubeOnboard, false);
							rollerLeft =  intakeSpeed;								// Pull In
							rollerRight = intakeSpeed;
							counterCubeRotate = 0;
							counterRollerIn = 0;
							counterRollerOff = 0;
						}
						break;

					case 1:															// CUBE DETECTED
						if (counterRollerIn < 25) {		// 35  25
							if (counterRollerIn == 0) Robot::robotLog->Write("   Cube Detected");
							counterRollerIn++;
							rollerLeft =  intakeSpeed;								// Continue In
							rollerRight =  intakeSpeed;

						} else if (counterCubeRotate < 20) {	// 25
							if (counterCubeRotate == 0) {
								sprintf(m_log, "   Cube Rotate %s", CubeRotateName(rotateOption).c_str());
								Robot::robotLog->Write(m_log);
							}
							counterCubeRotate++;

							switch (rotateOption) {
								case 0:	rollerLeft = intakeSpeed;					// Rotate CCW
										rollerRight = -intakeSpeed;
										break;
								case 1: rollerLeft = -intakeSpeed;					// Rotate CW
										rollerRight = intakeSpeed;
										break;
								case 2: rollerLeft = -intakeSpeed;					// Eject
										rollerRight = -intakeSpeed;
										break;
								default:;
							}

						} else {
							Robot::robotLog->Write("   Cube Rotate Done");

							rollerLeft =  intakeSpeed;								// Pull In
							rollerRight =  intakeSpeed;
							counterRollerIn = 1;
							counterRollerOff = 0;
							counterCubeRotate = 0;
							if (rotateOption < 2) rotateOption++;
							else				  rotateOption = 0;
						}
						break;

					case 2:															// CUBE CAPTURED
					case 3:															// CUBE DETECTED and CAPTURED
						if (counterRollerOff < 10) {
							if (counterRollerOff == 0) {
								Robot::robotLog->Write("   Cube In Gripper");
								counterRollerIn = 0;
							}

							counterRollerOff++;
							rollerLeft 	= intakeSpeed;								// Pull In
							rollerRight = intakeSpeed;

						} else {
							Robot::robotLog->Write("   Cube Captured");
							Robot::dashboard->SetRobotStatus(rsCubeOnboard, true);

							SetIntakeMode(imOff);
							if (CommandBase::lift->GetLiftPosition(Lift::potInUse) < Robot::dashboard->GetDashValue(dvLiftSwitch))
								SetArmPosition(dvIntakeStow);
						}
						break;

					default:;
				}

				prevFlags = cubeFlags;
				break;

			case imDrop:																// DROP CUBE
				m_ejectCount--;															// Delay to allow cube to drop
				if (m_ejectCount <= 0) SetIntakeMode(imOff);
				break;

			case imEject:																// EJECT CUBE
				m_ejectCount--;															// Delay to complete eject
				m_earlyFinish = (m_ejectCount < 30);									// Early finish to allow next command
				if (m_ejectCount <= 0) SetIntakeMode(imOff);

				rollerLeft = Robot::dashboard->GetDashValue(dvIntakeEjectSpeed);
				rollerRight = rollerLeft;
				break;

			case imLaunch:																// LAUNCH CUBE
				if (fabs(m_armPID->GetError() < 6)) {
					m_ejectCount--;														// Delay to complete launch
					m_earlyFinish = (m_ejectCount < 15);								// Early finish to allow next command
					if (m_ejectCount <= 0) SetIntakeMode(imOff);

					rollerLeft = m_launchSpeed;
					rollerRight = rollerLeft;
				}
				break;

			case imManual:																// MANUAL
				intakeSpeed = Robot::dashboard->GetDashValue(dvIntakeInSpeed);
				rollerLeft =  intakeSpeed;												// Pull In
				rollerRight =  intakeSpeed;
				counterRollerIn = 0;
				break;

			case imAutoOpen:															// AUTO OPEN
				rollerLeft = -0.5;
				rollerRight = -0.5;
				break;

			default:
				rotateOption = 0;
		}
	}

	m_rollerLeft->Set(rollerLeft);
	m_rollerRight->Set(rollerRight);

// 	*****************************************************************************

	double		armPosition = GetArmPosition();
	double 		armSpeed 	= 0;

	if (m_useJoystick) {															// Control with Joystick
		armSpeed = CommandBase::oi->GetLiftY() / 2;

		if (armSpeed < 0) {															// Move Arm Down
			m_armSetpoint = armPosition;
			m_armPID->SetSetpoint(m_armSetpoint);
			if (armPosition < 5) armSpeed = 0;

		} else if (CommandBase::lift->InArmDownZone()) {							// Inhibit Up Movement
			armSpeed = m_armPID->Calculate(armPosition, false);

		} else if ((armSpeed > m_armPID->Calculate(armPosition, false)) &&
				   (armPosition < Robot::dashboard->GetDashValue(dvIntakeLimit))) {	// Move Arm Up
			m_armSetpoint = armPosition;
			m_armPID->SetSetpoint(m_armSetpoint);

		} else {																	// Hold in place
			armSpeed = m_armPID->Calculate(armPosition, false);
		}

	} else {																		// Control with PID
		armSpeed =  m_armPID->Calculate(armPosition, false);
		if (armPosition < 5 && m_armSetpoint < 5) armSpeed = 0;
	}

	m_armMotor->Set(armSpeed);
}

double CubeIntake::GetArmPosition() {
	return ((double)m_armPot->GetAverageValue() / COUNT_PER_DEGREE) - Robot::dashboard->GetDashValue(dvIntakePotOffset);
}

CubeIntake::GripState CubeIntake::GetGripState() {
	return m_gripState;
}

std::string CubeIntake::GripStateName(int gripIndex){
	switch (gripIndex) {
		case gsOff:				return "OFF";
		case gsDrop: 			return "DROP";
		case gsClose:			return "CLOSE";
		case gsGrip:			return "GRIP";
		default:				return "?";
	}
}

void CubeIntake::InitDefaultCommand() {
}

std::string CubeIntake::IntakeModeName(int index) {
	switch (index) {
		case imOff: 			return "OFF";
		case imPickup: 			return "PICKUP";
		case imStow:			return "STOW";
		case imEject: 			return "EJECT";
		case imDrop:			return "DROP";
		case imLaunch: 			return "LAUNCH";
		case imManual:			return "MANUAL";
		case imAutoOpen:		return "AUTO OPEN";
		case imAutoPickup:		return "AUTO PICKUP";
		default:				return "?";
	}
}

bool CubeIntake::ModeChangeFinished() {
	return ((m_intakeMode == imOff) || m_earlyFinish);
}

std::string CubeIntake::CubeRotateName(int option) {
	switch (option) {
		case 0: 	return "CCW";
		case 1: 	return "CW";
		case 2: 	return "EJECT";
		default:	return "?";
	}
}

void CubeIntake::SetArmPosition(int positionIndex) {
	SetArmPosition(Robot::dashboard->GetDashValue(positionIndex), positionIndex);
}

void CubeIntake::SetArmPosition(double setpoint, int positionIndex) {
	if (CommandBase::lift->InArmDownZone() && setpoint > 10) setpoint = 0;

	if (m_armSetpoint != setpoint) {
		m_armSetpoint = setpoint;
		m_armPID->SetSetpoint(m_armSetpoint);

		if (setpoint > 60) SetIntakeMode(imOff);

		if (positionIndex < 0) {
			sprintf(m_log, "Cube:   Arm  to %3.0f", m_armSetpoint);
		} else {
			sprintf(m_log, "Cube:   Arm  to %3.0f (%s)", m_armSetpoint, ArmPositionName(positionIndex).c_str());
		}

		Robot::robotLog->Write(m_log);
	}
}

void CubeIntake::SetArmSpeed(double speed){
	m_armMotor->Set(speed);
}

void CubeIntake::SetDashboardValues() {
	Robot::dashboard->SetRobotStatus(rsCubeSensorNear, !m_cubeSensorNear->Get());
	Robot::dashboard->SetRobotStatus(rsCubeSensorFar, !m_cubeSensorFar->Get());

	Robot::dashboard->SetRobotValue(rvIntakeArmPosition, GetArmPosition());
	Robot::dashboard->SetRobotValue(rvIntakeArmSetpoint, m_armSetpoint);
	Robot::dashboard->SetRobotValue(rvIntakeRollerState, m_rollerState);
	Robot::dashboard->SetRobotValue(rvIntakeArmPwm, -m_armMotor->Get());
}

void CubeIntake::SetGripState(GripState state){
	if(m_gripState != state){
		if ((m_gripState == gsOff || m_gripState == gsDrop) &&
			(state == gsClose || state == gsGrip)) m_gripCount = 20;

		m_gripState = state;

		m_solGrip->Set(state == gsGrip || state == gsClose);
		m_solDrop->Set(state == gsDrop);

		Robot::dashboard->SetRobotStatus(rsCubeGrip, (m_gripState == gsGrip || m_gripState == gsClose));
		Robot::dashboard->SetRobotStatus(rsCubeDrop, (m_gripState == gsDrop));

		sprintf(m_log, "Cube:   Gripper %s", GripStateName(m_gripState).c_str());
		Robot::robotLog->Write(m_log);
	}
}

void CubeIntake::SetIntakeMode(IntakeMode mode, double launchSpeed) {
	switch (mode) {
		case imOff:
			m_rollerState = 0;
			if (m_cubeSensorNear->Get()) SetGripState(gsDrop);				// No Cube
			else						 SetGripState(gsGrip);
			break;

		case imPickup:
		case imAutoPickup:
			SetGripState(gsClose);
			SetArmPosition(dvIntakePickup);

			if (mode == imAutoPickup) {
				m_rollerState = 1;
				mode = imPickup;
			} else if (m_cubeSensorFar->Get()) { 							// No Cube
				m_rollerState = 1;
			} else {
				m_rollerState = 0;
				mode = imOff;
			}
			break;

		case imStow:
			m_rollerState = 0;
			if (m_cubeSensorNear->Get()) SetGripState(gsDrop);
			else						 SetGripState(gsGrip);

			if (CommandBase::lift->GetLiftPosition(Lift::potInUse) < Robot::dashboard->GetDashValue(dvLiftSwitch))
				SetArmPosition(dvIntakeStow);
			break;

		case imDrop:
			m_rollerState = 0;
			m_ejectCount = 20;
			SetGripState(gsDrop);
			break;

		case imEject:
			m_rollerState = -1;
			m_ejectCount = 60;
			break;

		case imLaunch:
			if (launchSpeed == 0) m_launchSpeed = Robot::dashboard->GetDashValue(dvIntakeLaunchSpeed);
			else				  m_launchSpeed = launchSpeed;

			m_rollerState = -1;
			m_ejectCount = 30;
			break;

		case imManual:
			SetGripState(gsClose);
			SetArmPosition(dvIntakePickup);
			break;

		case imAutoOpen:
			m_rollerState = 1;
			SetGripState(gsDrop);
			break;
		default:;
	}

	if (m_intakeMode != mode) {
		m_intakeMode = mode;
		m_earlyFinish = false;

		sprintf(m_log, "Cube:   Intake  %s", IntakeModeName(mode).c_str());
		Robot::robotLog->Write(m_log);
		Robot::dashboard->SetRobotValue(rvIntakeMode, m_intakeMode);
	}
}

void CubeIntake::SetUseJoystick(bool useJoystick){
	m_useJoystick = useJoystick;

	if (m_useJoystick) {																// Use Joystick
		m_armSetpoint = GetArmPosition();												// Set PID setpoint to current position
		m_armPID->SetSetpoint(m_armSetpoint);
	}
}

void CubeIntake::SetRollerJoystick(bool useJoystick){
	m_rollerJoystick = useJoystick;
}
