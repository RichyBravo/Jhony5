#include "CommandBase.h"

bool CommandBase::cb_cmdCancel;
bool CommandBase::cb_driveInUse;
char CommandBase::cb_log[100];
bool CommandBase::cb_scaleLeft;

std::unique_ptr<Climber> CommandBase::climber;
std::unique_ptr<CubeIntake> CommandBase::cubeIntake;
std::unique_ptr<Drive> CommandBase::drive;
std::unique_ptr<Lift> CommandBase::lift;

std::unique_ptr<OI> CommandBase::oi;

CommandBase::CommandBase(const std::string &name) : frc::Command(name) {
}

void CommandBase::Init() {
	cb_cmdCancel = false;					// Initialize global variables
	cb_driveInUse = false;
	cb_scaleLeft = false;

	climber.reset(new Climber());			// Initialize a single static instance of each subsystem
	cubeIntake.reset(new CubeIntake());
	drive.reset(new Drive());
	lift.reset(new Lift());

	oi.reset(new OI());
}
