#ifndef CmdCubeRollerMode_H
#define CmdCubeRollerMode_H

#include "../CommandBase.h"

class CmdCubeIntakeMode : public CommandBase {
public:
	CmdCubeIntakeMode(CubeIntake::IntakeMode mode, bool waitForFinish = false, bool resetCancel = true,
			          double launchSpeed = 0);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	double					m_launchSpeed;
	CubeIntake::IntakeMode 	m_mode;
	bool					m_resetCancel;
	bool					m_waitForFinish;
};

#endif  // CmdCubeRollerMode_H
