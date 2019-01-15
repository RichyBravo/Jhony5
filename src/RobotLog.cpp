#include "WPILib.h"
#include "RobotLog.h"
#include "Robot.h"
#include "RobotMap.h"
#include <fstream>
#include <sstream>
#include <iomanip>

// ************************* PUBLIC FUNCTIONS *************************
RobotLog::RobotLog() {
	m_robotMode 		= mInit;
	m_periodicBeginTime = 0;
	m_periodicCount 	= 0;
	m_periodicLastEnd 	= 0;
	m_periodicLastStart = 0;
	m_periodicTotalTime = 0;
	m_logFile 			= nullptr;
	m_dataFile 			= nullptr;
	m_PDP 				= new PowerDistributionPanel();

	for(int i = 0; i < 16; i++) m_pdpCurrent[i] = 0;
}

RobotLog::~RobotLog() {
}

void RobotLog::Close() {
	fclose(m_logFile);
	m_logFile = nullptr;

	if (m_dataFile != nullptr) {
		fclose(m_dataFile);
		m_dataFile = nullptr;
	}
}

void RobotLog::CheckDriveCurrent() {
	static int countLeft = 0;
	static int countRight = 0;

	if (!Robot::dashboard->GetRobotStatus(rsDriveLeftAlert)) {
		if (fabs(m_pdpCurrent[pdpDriveLeft1] - m_pdpCurrent[pdpDriveLeft2]) >
		    	(0.6 * std::max(m_pdpCurrent[pdpDriveLeft1], m_pdpCurrent[pdpDriveLeft2]))) {
			countLeft++;

			if (countLeft > 50) {
				Robot::dashboard->SetRobotStatus(rsDriveLeftAlert, true);
				Write("Drive:  Left Motor WARNING");
			}
		} else {
			countLeft = 0;
		}
	}

	if (!Robot::dashboard->GetRobotStatus(rsDriveRightAlert)) {
		if (fabs(m_pdpCurrent[pdpDriveRight1] - m_pdpCurrent[pdpDriveRight2]) >
				(0.6 * std::max(m_pdpCurrent[pdpDriveRight1], m_pdpCurrent[pdpDriveRight2]))) {
			countRight++;

			if (countRight > 50) {
				Robot::dashboard->SetRobotStatus(rsDriveRightAlert, true);
				Write("Drive:  Right Motor WARNING");
			}
		} else {
			countRight = 0;
		}
	}
}

std::string RobotLog::DataString(double value) {
																// Create string with delimiter for floating value
	std::stringstream ss;										// Output number to stream with fixed precision 0.000000
	ss << std::fixed << std::setprecision(6) << value;

	std::string ds = ss.str();									// Convert stream to string and trim trailing 0's
	ds.erase(ds.find_last_not_of('0') + 1, std::string::npos);

	std::string::iterator it = ds.end() - 1;					// Remove '.' if it is the last character
	if (*it == '.') ds.erase(it);

	return ds + "|";
}

void RobotLog::EndPeriodic() {
	m_periodicCount++;
	m_periodicLastEnd = Timer::GetFPGATimestamp() *1000;

	double runTime = m_periodicLastEnd - m_periodicLastStart;

	m_periodicTotalTime += runTime;

	if(runTime > 10.0) {
		sprintf(m_log, "Flipper: Long Periodic Execution Time=%5.1f (Start-to-End)", runTime);
		Write(m_log);
	}
}

void RobotLog::GetDataFromPDP() {
	m_pdpCurrent[pdpDriveLeft1] 	= m_PDP->GetCurrent(pdpDriveLeft1);
	m_pdpCurrent[pdpDriveLeft2] 	= m_PDP->GetCurrent(pdpDriveLeft2);
	m_pdpCurrent[pdpDriveRight1] 	= m_PDP->GetCurrent(pdpDriveRight1);
	m_pdpCurrent[pdpDriveRight2] 	= m_PDP->GetCurrent(pdpDriveRight2);
	m_pdpCurrent[pdpLiftMotorTop] 	= m_PDP->GetCurrent(pdpLiftMotorTop);
	m_pdpCurrent[pdpLiftMotorBot] 	= m_PDP->GetCurrent(pdpLiftMotorBot);

//	printf("Drive Left1=%4.1f  Left2=%4.1f  Right1=%4.1f  Right2=%4.1f\n", m_pdpCurrent[pdpDriveLeft1],
//			m_pdpCurrent[pdpDriveLeft2], m_pdpCurrent[pdpDriveRight1], m_pdpCurrent[pdpDriveRight2]);
}

RobotLog::RobotMode RobotLog::GetMode() {
	return m_robotMode;
}

double RobotLog::GetPdpCurrent(int channel) {
	double current = 0;

	if (channel >= 0 && channel < 16) current = m_pdpCurrent[channel];
	return current;
}

void RobotLog::LogData() {
	std::string data;

	data = DataString(m_pdpCurrent[pdpDriveLeft1]);
	data += DataString(m_pdpCurrent[pdpDriveLeft2]);
	data += DataString(m_pdpCurrent[pdpDriveRight1]);
	data += DataString(m_pdpCurrent[pdpDriveRight2]);
	data += DataString(m_pdpCurrent[pdpLiftMotorTop]);
	data += DataString(m_pdpCurrent[pdpLiftMotorBot]);

	WriteData(data);
}

std::string RobotLog::ModeName(RobotMode mode) {
	switch (mode) {
		case mInit:			return "Init";
		case mDisabled:		return "Disabled";
		case mAutonomous:	return "Auto";
		case mTeleop:		return "Telop";
		case mTest:			return "Test";
		default:			return "?";
	}
}

void RobotLog::SetMode(RobotMode mode) {
	m_periodicCount = 0;

	if(m_robotMode == mAutonomous || m_robotMode == mTeleop) {
		sprintf(m_log, "Flipper: Periodic Usage=%5.1f %%", (m_periodicTotalTime / (Timer::GetFPGATimestamp() *1000 - m_periodicBeginTime)) *100);
		Write(m_log);
	}

	m_robotMode = mode;
	m_periodicLastStart = Timer::GetFPGATimestamp() *1000;
	m_periodicLastEnd = m_periodicLastStart;
	m_periodicBeginTime = m_periodicLastStart;
	m_periodicTotalTime = 0;

	Write("", false);
	sprintf(m_log, "Flipper: Start %s %s", ModeName(mode).c_str(), Robot::dashboard->GetTimeStamp().c_str());
	Write(m_log);
}

void RobotLog::StartPeriodic() {
	static bool inBrownOut = false;
	double timeNow = Timer::GetFPGATimestamp() *1000;

	if((timeNow- m_periodicLastStart) > 100.0){
		sprintf(m_log, "Flipper: Long Periodic Interval=%5.1f (Start-to-Start)", timeNow - m_periodicLastStart);
		Write(m_log);
	}

	m_periodicLastStart = timeNow;

	bool brownOut = RobotController::IsBrownedOut();

	if (inBrownOut != brownOut) {
		inBrownOut = brownOut;
		if (inBrownOut) Write("Flipper: System Brown Out");
	}
}

void RobotLog::Write(std::string entry, bool includeTime) {
	const char* cEntry=entry.c_str();
	float		eventTime = (float)(m_periodicCount * 20) / 1000;
	int 		timeDiff = m_periodicCount - (int)(((Timer::GetFPGATimestamp() * 1000) - m_periodicBeginTime) / 20);

	if (includeTime) {
		printf("%7.2f %+3d: %s \n", eventTime, timeDiff, cEntry );
	} else {
		printf("%s \n", cEntry);
	}

	if (m_logFile == nullptr) m_logFile = fopen("/home/lvuser/Log525.txt", "a");

	if (m_logFile != nullptr) {
		if(m_periodicCount > 0) {
			fprintf(m_logFile, "%7.2f %+3d: %s \r\n", eventTime, timeDiff, cEntry);
		} else {
			fprintf(m_logFile,"%s \r\n", cEntry);
		}
	}
}

void RobotLog::WriteData(std::string data) {
	const char* cData=data.c_str();
	float		eventTime = (float)(m_periodicCount * 20) / 1000;

	if (m_dataFile == nullptr) {
		m_dataFile = fopen("/home/lvuser/Data525.txt", "a");
		fprintf(m_dataFile, "%s|%s\r\n", ModeName(m_robotMode).c_str(), Robot::dashboard->GetTimeStamp().c_str());
	}

	if (m_dataFile != nullptr) fprintf(m_dataFile, "%7.2f|%s\r\n", eventTime, cData);
}

