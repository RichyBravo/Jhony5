/*
 * RobotLog.h
 *
 *  Created on: Jan 10, 2015
 *      Author: Anton
 */

#ifndef ROBOTLOG_H_
#define ROBOTLOG_H_

#include "WPILib.h"

class RobotLog {
public:
	typedef enum {mInit, mDisabled, mAutonomous, mTeleop, mTest} RobotMode;

	RobotLog();
	~RobotLog();

	void 		Close();
	void		CheckDriveCurrent();
	void 		EndPeriodic();
	void		GetDataFromPDP();
	RobotMode	GetMode();
	double		GetPdpCurrent(int channel);
	void		LogData();
	std::string ModeName(RobotMode mode);
	void 		SetMode(RobotMode mode);
	void 		StartPeriodic();
	void 		Write(std::string entry, bool includeTime = true);
	void        WriteData(std::string data);


private:
	FILE*					m_logFile;
	FILE*					m_dataFile;
	PowerDistributionPanel*	m_PDP;

	double		m_periodicBeginTime;
	int32_t		m_periodicCount;
	double		m_periodicLastEnd;
	double		m_periodicLastStart;
	double		m_periodicTotalTime;
	double      m_pdpCurrent[16];
	RobotMode	m_robotMode;

	char		m_log[100];

	std::string DataString(double value);
};

#endif /* SRC_ROBOTLOG_H_ */
