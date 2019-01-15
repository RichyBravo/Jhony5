/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CmdClimberWinch.h"

CmdClimberWinch::CmdClimberWinch(bool enable) {
	m_enable = enable;
}

void CmdClimberWinch::Initialize() {
	climber->SetWinchEnable(m_enable);
}

void CmdClimberWinch::Execute() {
}

bool CmdClimberWinch::IsFinished() {
	return true;
}

void CmdClimberWinch::End() {
}

void CmdClimberWinch::Interrupted() {
}
