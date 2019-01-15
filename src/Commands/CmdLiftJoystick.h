#ifndef CmdLiftJoystick_H
#define CmdLiftJoystick_H

#include "../CommandBase.h"

class CmdLiftJoystick : public CommandBase {
public:
	CmdLiftJoystick(bool useJoystick);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	bool m_useJoystick;
};

#endif  // CmdLiftJoystick_H
