/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Commands/Subsystem.h>
#include <WPILib.h>
#include "ctre/Phoenix.h"
#include "Dashboard.h"
#include "Robot.h"

class Climber : public frc::Subsystem {
public:
	Climber();

	void 	ClimberPeriodic();
	bool 	GetWinchEnabled();
	void 	InitDefaultCommand() override;
	void	SetHookState(bool state);
	void 	SetWinchEnable(bool enable);


private:
	Solenoid*		m_hookAir;
	VictorSP*		m_winchMotor;

	bool			m_hookState;
	bool			m_winchEnable;
};

