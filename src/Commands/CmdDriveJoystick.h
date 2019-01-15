#ifndef CmdDriveJoystick_H
#define CmdDriveJoystick_H

#include "../CommandBase.h"

class CmdDriveJoystick : public CommandBase {
public:
	CmdDriveJoystick();
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();
};

#endif  // CmdDriveJoystick_H
