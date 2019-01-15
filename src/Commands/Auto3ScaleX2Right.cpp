#include "AutoCommands.h"

Auto3ScaleX2Right::Auto3ScaleX2Right(double wait) {
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
	//				bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true);

	//	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro, double timeout = 0, bool resetCancel = true);

	AddSequential	(new WaitCommand(wait));															// Wait if specified
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imOff, false, true));							// Grip Cube
	AddParallel		(new CmdCubeIntakePosition(dvIntakeStow, false));									// Move Arm to Stow

	AddSequential	(new CmdDrivePID(242, 1.0, 1.0, Drive::encoderAverage, true, true , 5, false));		// Drive to Scale
	AddSequential	(new CmdLiftPosition(dvLiftScaleMid, false, false));								// Move Lift to Scale Mid position
	AddSequential   (new CmdAutoArmTilt(45));															// Auto Arm tilt
	AddSequential	(new CmdRotatePID(-50, 0.7, false, 3, false));										// Rotate toward Scale
	AddSequential   (new CmdLiftWait(2, false));														// Wait until Lift is in position
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imLaunch, true, false, 1.0));					// Launch Cube into Scale

	AddSequential	(new CmdLiftPosition(dvLiftTravel, false, false));									// Move Lift to Travel
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoOpen, false, false));						// Put Intake in Auto Open mode
	AddSequential	(new CmdRotatePID(-149, 0.7, false, 3, false));										// Rotate toward Switch
	AddSequential	(new CmdDrivePID(64, -149, 0.6, Drive::encoderAverage, true, false , 4, false));	// Drive toward Switch
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoPickup, false, false));					// Put Intake in Auto Pickup mode
	AddSequential   (new CmdCubeIntakeWait(4, false));													// Wait until Cube on board

	AddSequential	(new CmdDrivePID(0, -149, 0.6, Drive::encoderAverage, false, false , -2.0, false));	// Drive back to Scale
	AddSequential	(new CmdLiftPosition(dvLiftScaleMid, false, false));								// Move Lift to Scale Mid position
	AddSequential   (new CmdAutoArmTilt(45));															// Auto Arm tilt
	AddSequential	(new CmdRotatePID(-65, 0.7, false, 3, false));										// Rotate toward Scale
	AddSequential	(new CmdLiftWait(2, false));														// Wait for Lift to reach position
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imLaunch, true, false, 0.8));					// Launch Cube into Scale

	AddSequential	(new CmdRotatePID(-135, 0.7, false, 3, false));										// Rotate toward next Cube
	AddSequential   (new CmdLiftPosition(dvLiftTravel, false, false));									// Move Lift to Travel
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imPickup, false, false));						// Put Intake in Pickup mode
	AddSequential	(new CmdDrivePID(84, -135, 0.5, Drive::encoderAverage, true, false , 4, false));	// Drive toward next cube
	AddSequential   (new CmdCubeIntakeWait(4, false));													// Wait until Cube on board
}
