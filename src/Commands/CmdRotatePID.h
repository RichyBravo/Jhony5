#ifndef CmdRotatePID_H
#define CmdRotatePID_H

#include "../CommandBase.h"

class CmdRotatePID : public CommandBase {
public:
	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro,
			     double timeout = 0, bool resetCancel = true);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	double		m_angle;
	double		m_maxSpeed;
	bool		m_resetCancel;
	bool		m_resetGyro;
	int			m_runStatus;
	double		m_timeout;
};

#endif  // CmdRotatePID_H
