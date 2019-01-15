#ifndef CmdCubeIntakePosition_H
#define CmdCubeIntakePosition_H

#include "../CommandBase.h"

class CmdCubeIntakePosition : public CommandBase {
public:
	CmdCubeIntakePosition(int positionIndex, bool resetCancel = true);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	int  m_positionIndex;
	bool m_resetCancel;
};

#endif  // CmdCubeIntakePosition_H
