#include "AutoCommands.h"

Auto1ScaleLeft::Auto1ScaleLeft(double wait) {
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
	//				bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true);

	//	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro, double timeout = 0, bool resetCancel = true);

	AddSequential	(new WaitCommand(wait));															// Wait if specified
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imOff, false, true));							// Grip Cube
	AddParallel		(new CmdCubeIntakePosition(dvIntakeStow, false));									// Move Arm to Stow

	AddSequential	(new CmdDrivePID(240, 0, 1.0, Drive::encoderAverage, true, true , 5, false));		// Drive to Scale
	AddSequential	(new CmdLiftPosition(dvLiftScaleMid, false, false));								// Move Lift to Scale Mid position
	AddSequential   (new CmdAutoArmTilt(45));															// Auto Arm tilt
	AddSequential	(new CmdRotatePID(50, 0.6, false, 3, false));										// Rotate toward Scale
	AddSequential   (new CmdLiftWait(2, false));														// Wait until Lift is in position
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imLaunch, true, false, 1.0));					// Launch Cube into Scale

	AddSequential	(new CmdLiftPosition(dvLiftTravel, false, false));									// Move Lift to Travel
	AddSequential	(new CmdDrivePID(-120, 0, 0.8, Drive::encoderAverage, true, false , 4, false));		// Drive toward Switch
	AddSequential	(new CmdCubeIntakePosition(dvIntakeStow, false));									// Move Arm to Stow

//	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoOpen, false, false));						// Put Intake in Auto Open mode
//	AddSequential	(new CmdRotatePID(145, 0.6, false, 3, false));										// Rotate toward Switch
//	AddSequential	(new CmdDrivePID(60, 145, 0.5, Drive::encoderAverage, true, false , 4, false));		// Drive toward Switch
//	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoPickup, false, false));					// Put Intake in Auto Pickup mode
//	AddSequential   (new CmdCubeIntakeWait(4, false));													// Wait until Cube on board
}
