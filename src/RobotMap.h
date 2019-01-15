#ifndef ROBOTMAP_H
#define ROBOTMAP_H

constexpr int pwmDriveLeft1 	= 0;
constexpr int pwmDriveLeft2 	= 1;
constexpr int pwmDriveRight1 	= 2;
constexpr int pwmDriveRight2 	= 3;
constexpr int pwmLiftBottom		= 4;
constexpr int pwmLiftTop		= 5;
constexpr int pwmRollerLeft		= 6;
constexpr int pwmRollerRight	= 7;
constexpr int pwmArmMotor		= 8;
constexpr int pwmWinchMotor		= 9;

constexpr int dioEncoderLeftA	= 0;
constexpr int dioEncoderLeftB	= 1;
constexpr int dioEncoderRightA	= 2;
constexpr int dioEncoderRightB	= 3;
constexpr int dioCubeSensorFar	= 4;
constexpr int dioCubeSensorNear	= 5;

constexpr int aiLiftPotBottom	= 0;
constexpr int aiLiftPotTop		= 1;
constexpr int aiArmPot			= 2;
constexpr int aiHookPot			= 3;

constexpr int solShift			= 0;
constexpr int solGrip			= 1;
constexpr int solDrop			= 2;
constexpr int solHook			= 3;

constexpr int pdpDriveLeft1		= 12;
constexpr int pdpDriveLeft2     = 13;
constexpr int pdpDriveRight1    = 14;
constexpr int pdpDriveRight2	= 15;
constexpr int pdpLiftMotorTop	= 4;
constexpr int pdpLiftMotorBot	= 8;

constexpr int canHookMotor		= 1;

constexpr double armDownLow		= 30;
constexpr double armDownHigh 	= 50;

#endif
