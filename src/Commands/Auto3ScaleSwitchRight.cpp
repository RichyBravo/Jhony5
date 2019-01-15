#include "AutoCommands.h"

Auto3ScaleSwitchRight::Auto3ScaleSwitchRight(double wait) {
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
	//				bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true);

	//	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro, double timeout = 0, bool resetCancel = true);

	AddSequential	(new WaitCommand(wait));															// Wait if specified
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imOff, false, true));							// Grip Cube
	AddParallel		(new CmdCubeIntakePosition(dvIntakeStow, false));									// Move Arm to Stow

	AddSequential	(new CmdDrivePID(242, 1.0, 1.0, Drive::encoderAverage, true, true , 5, false));		// Drive to Scale
	AddSequential	(new CmdLiftPosition(dvLiftScaleTop, false, false));								// Move Lift to Scale Top position
	AddSequential   (new CmdAutoArmTilt(45));															// Auto Arm tilt
	AddSequential	(new CmdRotatePID(-50, 0.6, false, 3, false));										// Rotate toward Scale
	AddSequential   (new CmdLiftWait(2, false));														// Wait until Lift is in position
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imLaunch, true, false, 1.0));					// Launch Cube into Scale

	AddSequential	(new CmdLiftPosition(dvLiftTravel, false, false));									// Move Lift to Travel
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoOpen, false, false));						// Put Intake in Auto Open mode
	AddSequential	(new CmdRotatePID(-149, 0.6, false, 3, false));										// Rotate toward Switch
	AddSequential	(new CmdDrivePID(64, -149, 0.6, Drive::encoderAverage, true, false , 4, false));	// Drive to Switch
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoPickup, false, false));					// Put Intake in Auto Pickup mode
	AddSequential   (new CmdCubeIntakeWait(4, false));													// Wait until Cube on board

	AddSequential	(new CmdLiftPosition(dvLiftSwitch, true, false));									// Move Lift to Switch position
	AddSequential   (new CmdCubeIntakePosition(dvIntakePickup, false));									// Lower Arm to Pickup position
	AddSequential	(new CmdDrivePID(82, -148, 0.6, Drive::encoderAverage, false, false , -1.5, false));// Drive forward to Switch
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imLaunch, false, false, 0.8));					// Launch Cube into Switch

	AddSequential	(new CmdDrivePID(-34, -148, 0.6, Drive::encoderAverage, true, false , 4, false));	// Back away from Switch
	AddSequential   (new CmdLiftPosition(dvLiftTravel, false, false));									// Move Lift to Travel
	AddSequential	(new CmdRotatePID(-125, 0.6, false, 3, false));										// Rotate toward next Cube
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imPickup, false, false));						// Put Intake in Pickup mode
	AddSequential	(new CmdDrivePID(30, -125, 0.6, Drive::encoderAverage, true, false , 3, false));	// Drive toward Switch
	AddSequential   (new CmdCubeIntakeWait(4, false));													// Wait until Cube on board
}
