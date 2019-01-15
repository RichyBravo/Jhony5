/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CmdLiftWait.h"
#include "Robot.h"

CmdLiftWait::CmdLiftWait(double timeout, bool resetCancel) {
	m_timeout = timeout;
	m_resetCancel = resetCancel;
	m_runStatus = 0;
}

void CmdLiftWait::Initialize() {
	if (m_resetCancel) cb_cmdCancel = false;

	if (cb_cmdCancel) {
		m_runStatus = 1;
	} else if (m_timeout > 0) {
		SetTimeout(m_timeout);
		Robot::robotLog->Write("Auto: Wait for Lift INIT");
	}
}

void CmdLiftWait::Execute() {
	if (m_runStatus == 0) {
		if (cb_cmdCancel) {
			m_runStatus = 2;
		} else if (IsTimedOut()) {
			m_runStatus = 3;
			cb_cmdCancel = true;
		}
	}
}

bool CmdLiftWait::IsFinished() {
	return cb_cmdCancel || lift->LiftNearTarget();
}

// Called once after isFinished returns true
void CmdLiftWait::End() {
	switch (m_runStatus) {
		case 0: Robot::robotLog->Write("----- Wait for Lift DONE");
				break;
		case 1: Robot::robotLog->Write("----- Wait for Lift SKIP");
				break;
		case 2: Robot::robotLog->Write("----- Wait for Lift CANCELED");
				break;
		case 3: Robot::robotLog->Write("----- Wait for Lift TIMED OUT");
				break;
	}
}

void CmdLiftWait::Interrupted() {
}
