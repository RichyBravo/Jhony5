#include "AutoCommands.h"


Auto2SwitchRight::Auto2SwitchRight(double wait) {
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
	//				bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true);

	//	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro, double timeout = 0, bool resetCancel = true);

	AddSequential	(new WaitCommand(wait));															// Wait if specified
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imOff, false, true));							// Grip Cube
	AddSequential	(new CmdCubeIntakePosition(dvIntakeStow, false));									// Move Arm to Stow

	AddSequential	(new CmdLiftPosition(dvLiftSwitch, false, false));									// Move Lift to Switch position
	AddSequential	(new CmdDrivePID(98, 21, 0.9, Drive::encoderLeft, true, true , -2.4, false));		// Drive to Switch
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imEject, true, false));							// Eject Cube into Switch

	AddSequential	(new CmdDrivePID(-85, 21, 0.9, Drive::encoderAverage, true, false, 4, false));		// Back away from Switch
	AddSequential	(new CmdLiftPosition(dvLiftTravel, false));											// Move Lift to Travel
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoOpen, false, false));						// Put Intake in Auto Open mode
	//AddSequential	(new CmdCubeIntakeMode(CubeIntake::imPickup, false, false));						// Put Intake in Pickup mode
	AddSequential	(new CmdRotatePID(-7, 0.7, false, 2, false));										// Rotate toward Power Cube Zone
	AddSequential	(new CmdDrivePID(42, -7, 0.8, Drive::encoderAverage, true, false , 3, false));		// Drive to Zone
	AddSequential	(new CmdCubeIntakeMode(CubeIntake::imAutoPickup, false, false));					// Put Intake in Auto Pickup mode
	AddSequential   (new CmdCubeIntakeWait(4, false));													// Wait until Cube on board

	AddSequential	(new CmdRotatePID(GAME_DATA_SCALE, 0.6, false, 2, false));							// Turn toward Game Data Scale
}
