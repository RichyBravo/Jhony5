#include "AutoCommands.h"

Auto3SwitchRight::Auto3SwitchRight(double wait) {
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
	//				bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true);

	//	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro, double timeout = 0, bool resetCancel = true);

	AddSequential	(new WaitCommand(wait));															// Wait if specified
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imOff, false, true));							// Grip Cube
	AddParallel		(new CmdCubeIntakePosition(dvIntakeStow, false));									// Move Arm to Stow

	AddSequential	(new CmdDrivePID(136, 0, 0.9 ,Drive::encoderAverage, true, true, 4, false));		// Drive to Switch
	AddSequential	(new CmdLiftPosition(dvLiftSwitch, false, false));									// Move Lift to Switch position
	AddSequential	(new CmdRotatePID(-90, 0.6, false, 3, false));										// Rotate toward Switch
	AddSequential	(new CmdDrivePID(24, -90, 0.7 ,Drive::encoderAverage, true, false, -1,5, false));	// Drive to Switch
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imEject, true, false));							// Eject Cube into Switch
	AddSequential	(new CmdDrivePID(-30, -90, 0.7 ,Drive::encoderAverage, true, false, 3, false));		// Back awary from Switch
	AddSequential	(new CmdLiftPosition(dvLiftTravel, false, false));									// Move Lift to Travel
	AddSequential   (new CmdRotatePID(0, 0.6, false, 3, false));										// Rotate toward Scale
}
