/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CmdRollerJoystick.h"

CmdRollerJoystick::CmdRollerJoystick(bool useJoystick) {
	m_useJoystick = useJoystick;
}

// Called just before this Command runs the first time
void CmdRollerJoystick::Initialize() {
	cubeIntake->SetRollerJoystick(m_useJoystick);
}

// Called repeatedly when this Command is scheduled to run
void CmdRollerJoystick::Execute() {

}

// Make this return true when this Command no longer needs to run execute()
bool CmdRollerJoystick::IsFinished() {
	return true;
}

// Called once after isFinished returns true
void CmdRollerJoystick::End() {

}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void CmdRollerJoystick::Interrupted() {

}
