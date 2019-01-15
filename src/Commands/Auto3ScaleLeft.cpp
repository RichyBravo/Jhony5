#include "AutoCommands.h"

Auto3ScaleLeft::Auto3ScaleLeft(double wait) {
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
	//				bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true),
	//              double maxRotate = 0.6);

	//	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro, double timeout = 0, bool resetCancel = true);

	AddSequential	(new WaitCommand(wait));															// Wait if specified
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imOff, false, true));							// Grip Cube
	AddParallel		(new CmdCubeIntakePosition(dvIntakeStow), false);									// Move Arm to Stow

	AddSequential	(new CmdDrivePID(208, 0, 1.0, Drive::encoderAverage, true, true, 5, false));		// Drive to platform
	AddSequential	(new CmdRotatePID(90, 0.9, false, 3, false));										// Rotate toward fence
	AddSequential	(new CmdLiftPosition(dvLiftSwitch, false, false));									// Move Lift to Switch position
	AddSequential	(new CmdDrivePID(-174, 90, 1.0, Drive::encoderAverage, true, false, 5, false));		// Drive backward in front of platform

	AddSequential	(new CmdLiftPosition(dvLiftScaleTop, false, false));								// Move Lift to Scale Top position
	AddSequential	(new CmdRotatePID(0, 0.9, false, 3, false));										// Rotate toward Scale
	AddSequential	(new CmdDrivePID(36, 0, 0.7, Drive::encoderAverage, true, false, 3, false));		// Drive toward Scale
	AddSequential   (new CmdLiftWait(2, false));														// Wait until Lift is in position
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imLaunch, false, false, 0.8));					// Eject Cube into Scale

	AddSequential	(new CmdDrivePID(-30, 0, 0.9, Drive::encoderAverage, true, false, 3, false));		// Back away from Scale
	AddSequential	(new CmdLiftPosition(dvLiftTravel, false, false));									// Move Lift to Travel
	AddSequential	(new CmdRotatePID(-88, 0.9, false, 3, false));										// Rotate toward Fence
	AddSequential	(new CmdDrivePID(-150, -88, 0.9, Drive::encoderAverage, true, false, 4, false));	// Drive backward in front of platform
	AddSequential	(new CmdRotatePID(-155, 0.9, false, 3, false));										// Rotate toward Switch
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imPickup, false, false));						// Put Intake in Pickup mode
	AddSequential	(new CmdDrivePID(20, -155, 0.6, Drive::encoderAverage, true, false, 3, false));		// Drive toward Switch
	AddSequential   (new CmdCubeIntakeWait(4, false));													// Wait until Cube on board
	AddSequential	(new CmdDrivePID(0, -155, 0.6, Drive::encoderAverage, false, false, 3, false));		// Back away from Switch
}
