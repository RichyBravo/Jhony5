/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CmdCubeIntakeWait.h"
#include "Robot.h"

CmdCubeIntakeWait::CmdCubeIntakeWait(double timeout, bool resetCancel, bool backup) {
	m_timeout = timeout;
	m_resetCancel = resetCancel;
	m_runStatus = 0;
	m_backup = backup;
}

void CmdCubeIntakeWait::Initialize() {
	if (m_resetCancel) cb_cmdCancel = false;

	if (cb_cmdCancel) {
		m_runStatus = 1;
	} else if (m_timeout > 0) {
		cb_driveInUse = true;
		SetTimeout(m_timeout);

		sprintf(cb_log, "Auto: Wait for Cube START");
		Robot::robotLog->Write(cb_log);
	}
}

void CmdCubeIntakeWait::Execute() {
	if (m_runStatus == 0) {
		if (cb_cmdCancel) {
			m_runStatus = 2;
		} else if (IsTimedOut()) {
			m_runStatus = 3;
			cb_cmdCancel = true;
		} else if (cubeIntake->CubeDetected() && m_backup) {
			drive->ArcadeDrive(-0.12, 0);
		} else {
			drive->ArcadeDrive(0, 0);
		}
	}
}

bool CmdCubeIntakeWait::IsFinished() {
	return cb_cmdCancel || cubeIntake->CubeOnboard();
}

void CmdCubeIntakeWait::End() {
	cb_driveInUse = false;

	switch (m_runStatus) {
		case 0: Robot::robotLog->Write("----- Wait for Cube DONE");
				break;
		case 1: Robot::robotLog->Write("----- Wait for Cube SKIP");
				break;
		case 2: Robot::robotLog->Write("----- Wait for Cube CANCELED");
				break;
		case 3: Robot::robotLog->Write("----- Wait for Cube TIMED OUT");
				cubeIntake->SetIntakeMode(CubeIntake::imOff);
				break;
	}
}

void CmdCubeIntakeWait::Interrupted() {
}

