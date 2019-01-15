#ifndef COMMAND_BASE_H
#define COMMAND_BASE_H

#include <string>
#include <Commands/Command.h>
#include "OI.h"
#include "Subsystems/Drive.h"
#include "Subsystems/Lift.h"
#include "Subsystems/CubeIntake.h"
#include "Subsystems/Climber.h"

class CommandBase: public frc::Command {
public:
	CommandBase(const std::string& name);
	CommandBase() = default;

	static bool	cb_cmdCancel;
	static bool cb_driveInUse;
	static char	cb_log[100];
	static bool cb_scaleLeft;

	static std::unique_ptr<Climber> climber;
	static std::unique_ptr<CubeIntake> cubeIntake;
	static std::unique_ptr<Drive> drive;
	static std::unique_ptr<Lift> lift;

	static std::unique_ptr<OI> oi;

	static void Init();

	#define GAME_DATA_SCALE 525
};

#endif
