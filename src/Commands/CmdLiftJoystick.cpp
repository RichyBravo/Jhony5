#include "CmdLiftJoystick.h"

CmdLiftJoystick::CmdLiftJoystick(bool useJoystick) {
	m_useJoystick = useJoystick;
}

void CmdLiftJoystick::Initialize() {
	lift->SetUseJoystick(m_useJoystick);
	cubeIntake->SetUseJoystick(false);
}

void CmdLiftJoystick::Execute() {
}

bool CmdLiftJoystick::IsFinished() {
	return true;
}

void CmdLiftJoystick::End() {
}

void CmdLiftJoystick::Interrupted() {
}
