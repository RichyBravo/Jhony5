#ifndef CmdDriveArc_H
#define CmdDriveArc_H

#include "../CommandBase.h"

class CmdDriveArc : public CommandBase {
public:
	CmdDriveArc(double distance, double finalHeading, double maxSpeed, bool resetGyro,
				double timeout = 0, bool resetCancel = true);
	void Initialize();
	void Execute();
	bool IsFinished();
	void End();
	void Interrupted();

private:
	double	m_angle;
	double 	m_distance;
	double	m_finalHeading;
	double	m_maxSpeed;
	bool	m_resetCancel;
	bool	m_resetGyro;
	int		m_runStatus;
	double	m_startAngle;
	double	m_timeout;
};

#endif  // CmdDriveArc_H
