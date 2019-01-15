/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CmdCubeIntakeJoystick.h"

CmdCubeIntakeJoystick::CmdCubeIntakeJoystick(bool useJoystick) {
	m_useJoystick = useJoystick;
}

void CmdCubeIntakeJoystick::Initialize() {
	if (m_useJoystick) {
		if (!lift->GetUseJoystick()) cubeIntake->SetUseJoystick(true);
	} else {
		cubeIntake->SetUseJoystick(false);
	}
}

void CmdCubeIntakeJoystick::Execute() {
}

bool CmdCubeIntakeJoystick::IsFinished() {
	return true;
}

void CmdCubeIntakeJoystick::End() {
}

void CmdCubeIntakeJoystick::Interrupted() {
}
