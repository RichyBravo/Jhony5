#ifndef CmdLiftPosition_H
#define CmdLiftPosition_H

#include "../CommandBase.h"

class CmdLiftPosition : public CommandBase {
public:
	CmdLiftPosition(int positionIndex,bool waitForFinish = false, bool resetCancel = true);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	int  m_positionIndex;
	bool m_resetCancel;
	int  m_waitCount;
	bool m_waitForFinish;
};

#endif  // CmdLiftPosition_H
