/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CmdDriveTurn.h"

CmdDriveTurn::CmdDriveTurn(double distance, double heading) {
	m_distance = distance;
	m_heading = heading;
}

void CmdDriveTurn::Initialize() {
	drive->DrivePidAddTurn(m_distance, m_heading);
}

void CmdDriveTurn::Execute() {
}

bool CmdDriveTurn::IsFinished() {
	return true;
}

void CmdDriveTurn::End() {
}

void CmdDriveTurn::Interrupted() {
}
