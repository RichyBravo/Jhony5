#ifndef CmdDriveShift_H
#define CmdDriveShift_H

#include "../CommandBase.h"

class CmdDriveShift : public CommandBase {
public:
	CmdDriveShift(Drive::ShifterState state);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	Drive::ShifterState m_state;
	bool				m_shiftRequired;
	int 				m_shiftTimer;
};

#endif  // CmdDriveShift_H
