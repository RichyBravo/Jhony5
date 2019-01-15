#include "CmdLiftPosition.h"

CmdLiftPosition::CmdLiftPosition(int positionIndex, bool waitForFinish, bool resetCancel) {
	m_positionIndex = positionIndex;
	m_resetCancel = resetCancel;
	m_waitForFinish = waitForFinish;
	m_waitCount = 5;
}

void CmdLiftPosition::Initialize() {
	if (m_resetCancel) cb_cmdCancel = false;
	if (!cb_cmdCancel) lift->SetLiftPosition(m_positionIndex);
}

void CmdLiftPosition::Execute() {
}

bool CmdLiftPosition::IsFinished() {
	if (m_waitForFinish) {
		if (m_waitCount > 0) {
			m_waitCount--;
			return false;
		} else {
			return lift->LiftNearTarget();
		}
	} else {
		return true;
	}
}

void CmdLiftPosition::End() {
}

void CmdLiftPosition::Interrupted() {
}
