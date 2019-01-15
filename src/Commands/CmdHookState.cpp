/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CmdHookState.h"

CmdHookState::CmdHookState(bool state) {
	m_hookState = state;
}

void CmdHookState::Initialize() {
	climber->SetHookState(m_hookState);

	if (m_hookState) {
		cubeIntake->SetArmPosition(dvIntakeStow);
		cubeIntake->SetGripState(CubeIntake::gsGrip);
	}
}

void CmdHookState::Execute() {
}

bool CmdHookState::IsFinished() {
	return true;
}

void CmdHookState::End() {
}

void CmdHookState::Interrupted() {
}
