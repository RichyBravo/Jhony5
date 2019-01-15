#include <Commands/CmdCubeIntakeMode.h>

CmdCubeIntakeMode::CmdCubeIntakeMode(CubeIntake::IntakeMode mode, bool waitForFinish, bool resetCancel,
									 double launchSpeed) {
	m_mode = mode;
	m_resetCancel = resetCancel;
	m_waitForFinish = waitForFinish;
	m_launchSpeed = launchSpeed;
}

void CmdCubeIntakeMode::Initialize() {
	if (m_resetCancel) cb_cmdCancel = false;
	if (!cb_cmdCancel) cubeIntake->SetIntakeMode(m_mode, m_launchSpeed);
}

void CmdCubeIntakeMode::Execute() {
}

bool CmdCubeIntakeMode::IsFinished() {
	if (m_waitForFinish) {
		return (cubeIntake->ModeChangeFinished());
	} else {
		return true;
	}
}

void CmdCubeIntakeMode::End() {
}

void CmdCubeIntakeMode::Interrupted() {
}
