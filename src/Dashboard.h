#ifndef DASHBOARD_H_
#define DASHBOARD_H_

#include "WPILib.h"
//#include "RobotLog.h"

class Dashboard
{
public:
	Dashboard(int robotStatusCount, int robotValueCount, int dashButtonCount, int dashValueCount);
	~Dashboard();

	std::string CountReply();										// Returns current state of Robot, DbData, DbButton values
	std::string DataString(int32_t number, int delimiter);			// Returns string with integer followed by a delimiter
	std::string DataString(double number, int delimiter);			// Returns string with double followed by a delimiter
	bool		GetDashButton(int buttonIndex);						// Returns state of a Dashboard Button
	bool		GetDashButtonPress(int buttonIndex);				// Returns whether or not a Dashboard Button has been Pressed
	double		GetDashValue(int index);							// Returns Dashboard value at specified index
	std::string GetReply();											// Returns GET reply for all Robot Values
	bool		GetRobotStatus(int statusIndex);
	std::string GetTimeStamp();

	void        SaveDashValues();
	bool		SetDashButton(int group, int32_t value);			// Sets Dashboard Button group at specified index
	bool		SetDashValue(int index, double value);				// Sets Dashboard Value at specified index
	bool		SetRobotStatus(int statusIndex, bool value);		// Sets Robot Status at specified index
	bool		SetRobotValue(int index, double value);				// Sets Robot Value at specified index
	void		SetRobotMode(int mode);								// Sets Robot Mode
	void        SetTimeStamp(std::string now);
	static void TcpLoop(Dashboard *host);
	void		WriteToLog(std::string entry);

private:
	struct button {
		int32_t state;
		int32_t pressed;
	};

	int			m_dashboardButtonCount;
	int			m_dashboardValueCount;
	int			m_robotStatusCount;
	int			m_robotValueCount;
	int			m_robotMode;
	std::string m_timeStamp;

	button*		m_dashboardButton;
	double*		m_dashboardValue;
	char		m_log[100];
	int32_t*	m_robotStatus;
	double*		m_robotValue;
};

constexpr int rsCubeSensorNear		= 0;				// Robot Status Indexes	(16 per group)
constexpr int rsCubeSensorFar		= 1;
constexpr int rsCubeGrip			= 2;
constexpr int rsCubeDrop			= 3;
constexpr int rsDriveShiftState		= 4;
constexpr int rsLiftDirectionUp     = 5;
constexpr int rsCubeOnboard			= 6;
constexpr int rsDriveLeftAlert		= 7;
constexpr int rsDriveRightAlert		= 8;

constexpr int rvGyro				= 0;				// Robot Value Indexes
constexpr int rvEncoderLeft			= 1;
constexpr int rvEncoderRight		= 2;
constexpr int rvDriveLeftPwm		= 3;
constexpr int rvDriveRightPwm		= 4;
constexpr int rvIntakeArmPosition	= 5;
constexpr int rvIntakeArmSetpoint	= 6;
constexpr int rvIntakeMode			= 7;
constexpr int rvIntakeRollerState	= 8;
constexpr int rvIntakeArmPwm		= 9;
constexpr int rvLiftPosition		= 10;
constexpr int rvLiftSetpoint		= 11;
constexpr int rvLiftIndex			= 12;
constexpr int rvLiftPwm				= 13;
constexpr int rvLiftAmpsTop			= 14;
constexpr int rvLiftAmpsBottom		= 15;
constexpr int rvLiftPotTop			= 16;
constexpr int rvLiftPotBot			= 17;
constexpr int rvHookPosition		= 18;

constexpr int dbRunPid				= 0;				// Dashboard Button Indexes (16 per group)
constexpr int dbLiftScaleTop		= 1;
constexpr int dbLiftScaleMid		= 2;
constexpr int dbLiftScaleBot		= 3;
constexpr int dbLiftSwitch			= 4;
constexpr int dbLiftTravel			= 5;
constexpr int dbLiftBumpDown		= 6;
constexpr int dbLiftBumpUp			= 7;
constexpr int dbIntakePickup		= 8;
constexpr int dbIntakeStow			= 9;
constexpr int dbIntakeEject			= 10;
constexpr int dbIntakeLaunch		= 11;
constexpr int dbIntakeDrop			= 12;
constexpr int dbIntakeTilt			= 13;
constexpr int dbDataCapture			= 14;

constexpr int dvAutoStart			= 0;				// Dashboard Value Indexes
constexpr int dvAutoDelay			= 1;
constexpr int dvAutoSelectLLL		= 2;
constexpr int dvAutoSelectLRL		= 3;
constexpr int dvAutoSelectRLR		= 4;
constexpr int dvAutoSelectRRR		= 5;
constexpr int dvPidSelect			= 6;
constexpr int dvPidSetpoint			= 7;
constexpr int dvPidMaxOut			= 8;
constexpr int dvPthreshold			= 9;
constexpr int dvPabove				= 10;
constexpr int dvPbelow				= 11;
constexpr int dvIthreshold			= 12;
constexpr int dvIabove				= 13;
constexpr int dvIbelow				= 14;
constexpr int dvDthreshold			= 15;
constexpr int dvDabove				= 16;
constexpr int dvDbelow				= 17;
constexpr int dvIntakeInSpeed		= 18;
constexpr int dvIntakeEjectSpeed	= 19;
constexpr int dvIntakeLaunchSpeed	= 20;
constexpr int dvIntakePotOffset		= 21;
constexpr int dvIntakeLimit			= 22;
constexpr int dvIntakeStow			= 23;
constexpr int dvIntakePickup		= 24;
constexpr int dvIntakeTilt			= 25;
constexpr int dvLiftPotTopOffset	= 26;
constexpr int dvLiftPotBotOffset	= 27;
constexpr int dvLiftHighLimit		= 28;
constexpr int dvLiftHighThres		= 29;
constexpr int dvLiftLowLimit 		= 30;
constexpr int dvLiftLowThres		= 31;
constexpr int dvLiftScaleTop		= 32;
constexpr int dvLiftScaleMid		= 33;
constexpr int dvLiftScaleBot		= 34;
constexpr int dvLiftSwitch			= 35;
constexpr int dvLiftTravel			= 36;
constexpr int dvLiftBumpStep		= 37;
constexpr int dvCameraOn			= 38;
constexpr int dvHookPotOffset		= 39;
constexpr int dvHookPotLimit		= 40;

#endif
