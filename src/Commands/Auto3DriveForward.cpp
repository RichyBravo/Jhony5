/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AutoCommands.h"

Auto3DriveForward::Auto3DriveForward(double wait) {
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
		//				bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true,
		//              double maxRotate = 0.6);

		//	CmdRotatePID(double angle, double maxSpeed,	bool resetGyro, double timeout = 0, bool resetCancel = true);

		AddSequential	(new WaitCommand(wait));															// Wait if specified
		AddSequential	(new CmdCubeIntakeMode(CubeIntake::imOff, false, true));							// Grip Cube
		AddParallel		(new CmdCubeIntakePosition(dvIntakeStow), false);									// Move Arm to Stow

		AddSequential	(new CmdDrivePID(210, 0, 1.0, Drive::encoderAverage, true, true, 5, false));		// Drive to platform
		AddSequential	(new CmdRotatePID(90, 0.9, false, 3, false));										// Rotate toward fence
		AddSequential	(new CmdDrivePID(-125, 90, 1.0, Drive::encoderAverage, true, false, 5, false));		// Drive backward in front of platform

}
