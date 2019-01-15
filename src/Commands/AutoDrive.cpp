#include "AutoCommands.h"
#include "Commands/CmdDrivePID.h"

AutoDrive::AutoDrive(double wait) {
	// TESTED
	//	CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
	//			    bool resetEncoder, bool resetGyro, double timeout = 0, bool resetCancel = true);

	AddSequential(new WaitCommand(wait));
	AddSequential(new CmdDrivePID(90, 0, 0.5 ,Drive::encoderLeft, true, true, 5, true));
//	AddSequential(new CmdDrivePID(200, 0, 0.9 ,Drive::encoderLeft, true, true, 5, true));
//	AddSequential(new CmdRotatePID(90, 0.7, false, 3, false));									// Rotate 90
}
