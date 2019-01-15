#include "CmdLiftBump.h"
#include "Robot.h"

CmdLiftBump::CmdLiftBump(bool isBumpUp, bool resetCancel) {
	m_isBumpUp = isBumpUp;
	m_resetCancel = resetCancel;
	m_runStatus = 0;
}

void CmdLiftBump::Initialize() {
	if (m_resetCancel) cb_cmdCancel = false;

	if (cb_cmdCancel) {
		m_runStatus = 1;

	} else {
		if (m_isBumpUp) {
			lift->SetLiftPosition(lift->GetLiftSetpoint() + Robot::dashboard->GetDashValue(dvLiftBumpStep));
		} else {
			lift->SetLiftPosition(lift->GetLiftSetpoint() - Robot::dashboard->GetDashValue(dvLiftBumpStep));
		}
	}
}

void CmdLiftBump::Execute() {
}

bool CmdLiftBump::IsFinished() {
	return true;
}

void CmdLiftBump::End() {
	if (m_runStatus == 1) Robot::robotLog->Write("Auto: Lift Bump SKIP");
}

void CmdLiftBump::Interrupted() {
}
