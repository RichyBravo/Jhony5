#include "CmdDriveJoystick.h"

CmdDriveJoystick::CmdDriveJoystick() {
	Requires(drive.get());
}

void CmdDriveJoystick::Initialize() {
}

void CmdDriveJoystick::Execute() {
	if(climber->GetWinchEnabled()){
		drive->ArcadeDrive(0, 0);
	}else {
		drive->ArcadeDrive(oi->GetDriveY(), oi->GetDriveX() * 0.8);
	}
}

bool CmdDriveJoystick::IsFinished() {
	return false;
}

void CmdDriveJoystick::End() {
}

void CmdDriveJoystick::Interrupted() {
}
