#ifndef CmdLiftBump_H
#define CmdLiftBump_H

#include "../CommandBase.h"

class CmdLiftBump : public CommandBase {
public:
	CmdLiftBump(bool isBumpUp, bool resetCancel = true);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	bool m_isBumpUp;
	bool m_resetCancel;
	int  m_runStatus;

};

#endif  // CmdLiftBump_H
