/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <Commands/CmdAutoArmTilt.h>

CmdAutoArmTilt::CmdAutoArmTilt(double tiltAngle) {
	m_tiltAngle = tiltAngle;
}

void CmdAutoArmTilt::Initialize() {
	if (!cb_cmdCancel) lift->SetAutoArmTilt(true, m_tiltAngle);
}

void CmdAutoArmTilt::Execute() {
}

bool CmdAutoArmTilt::IsFinished() {
	return true;
}

void CmdAutoArmTilt::End() {
}

void CmdAutoArmTilt::Interrupted() {
}
