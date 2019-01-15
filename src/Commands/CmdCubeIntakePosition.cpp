#include "CmdCubeIntakePosition.h"

CmdCubeIntakePosition::CmdCubeIntakePosition(int positionIndex, bool resetCancel) {
	m_positionIndex = positionIndex;
	m_resetCancel = resetCancel;
}

void CmdCubeIntakePosition::Initialize() {
	if (m_resetCancel) cb_cmdCancel = false;
	if (!cb_cmdCancel) cubeIntake->SetArmPosition(m_positionIndex);
}

void CmdCubeIntakePosition::Execute() {
}

bool CmdCubeIntakePosition::IsFinished() {
	return true;
}

void CmdCubeIntakePosition::End() {

}

void CmdCubeIntakePosition::Interrupted() {

}
