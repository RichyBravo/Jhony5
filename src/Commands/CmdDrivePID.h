#ifndef CmdDrivePID_H
#define CmdDrivePID_H


#include "../CommandBase.h"

class CmdDrivePID : public CommandBase {
public:
	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
			    bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true,
				double maxRotate = 0.6);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	bool				m_continue;
	double 				m_distance;
	double 				m_heading;
	double				m_maxRotate;
	double 				m_maxSpeed;
	bool				m_resetCancel;
	bool 				m_resetEncoder;
	bool 				m_resetGyro;
	int 				m_runStatus;
	double 				m_timeout;
	Drive::UseEncoder	m_useEncoder;
};

#endif  // CmdDrivePID_H
