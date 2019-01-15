#include "AutoCommands.h"

Auto2SwitchExchLeft::Auto2SwitchExchLeft(double wait) {
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
	//				bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true);

	//	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro, double timeout = 0, bool resetCancel = true);

	AddSequential	(new WaitCommand(wait));															// Wait if specified
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imOff, false, true));							// Grip Cube
	AddSequential	(new CmdCubeIntakePosition(dvIntakeStow, false));									// Move Arm to Stow

	AddSequential	(new CmdLiftPosition(dvLiftSwitch, false, false));									// Move Lift to Switch position
	AddSequential	(new CmdDrivePID(105, -23, 0.9, Drive::encoderRight, true, true , -2.4, false));	// Drive to Switch
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imEject, true, false));							// Eject Cube into Switch

	AddSequential	(new CmdDrivePID(-45, -23, 0.9, Drive::encoderAverage, true, false, 4, false));		// Back away from Switch
	AddSequential	(new CmdLiftPosition(dvLiftTravel, false, false));									// Move Lift to Travel
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imPickup, false, false));						// Put Intake in Pickup mode
	AddSequential	(new CmdRotatePID(35, 0.7, false, 2, false));										// Rotate toward Power Cube Zone
	AddSequential	(new CmdDrivePID(9, 35, 0.8, Drive::encoderAverage, true, false, 3, false));		// Drive to Zone
	AddSequential   (new CmdCubeIntakeWait(4, false));													// Wait until Cube on board

	AddSequential	(new CmdDrivePID(0, 35, 0.8, Drive::encoderAverage, false, false, 3, false));		// Back up to previous position
	AddSequential	(new CmdRotatePID(176, 0.7, false, 2, false));										// Rotate toward Exchange
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imPickup, false, false));						// Put Intake in Pickup mode
	AddSequential	(new CmdDrivePID(40, 176, 0.9, Drive::encoderAverage, true, false, 3, false));		// Drive to Exchange
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imLaunch, true, false));							// Launch Cube into Exchange

	AddSequential	(new CmdDrivePID(-15, 176, 0.9, Drive::encoderAverage, true, false, 3, false));		// Back away from Exchange
	AddSequential	(new CmdLiftBump(true, false));														// Move Lift to pickup stacked Cube
	AddSequential	(new CmdLiftBump(true, false));
	AddSequential	(new CmdRotatePID(26, 0.7, false, 2, false));										// Rotate toward Power Cube Zone
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoPickup, false, false));					// Put Intake in Pickup mode
	AddSequential	(new CmdDrivePID(54, 26, 0.7, Drive::encoderAverage, true, false, 3, false));		// Drive to Zone
	AddSequential   (new CmdCubeIntakeWait(4, false, false));											// Wait until Cube on board

	AddSequential	(new CmdDrivePID(-15, 26, 0.9, Drive::encoderAverage, true, false, 3, false));		// Back away from Zone
	AddSequential	(new CmdCubeIntakePosition(dvIntakeStow, false));									// Move Arm to Stow

	AddSequential	(new CmdRotatePID(GAME_DATA_SCALE, 0.7, false, 4, false));							// Turn toward Game Data Scale
}
