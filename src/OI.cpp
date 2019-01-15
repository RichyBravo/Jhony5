/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "OI.h"
#include "Dashboard.h"
#include "Commands/CmdCubeIntakeJoystick.h"
#include "Commands/CmdCubeIntakeMode.h"
#include "Commands/CmdDriveShift.h"
#include "Commands/CmdLiftBump.h"
#include "Commands/CmdLiftJoystick.h"
#include "Commands/CmdLiftPosition.h"
#include "Commands/CmdClimberWinch.h"
#include "Commands/CmdRollerJoystick.h"
#include "Commands/CmdHookState.h"

OI::OI() {
	m_joystickDrive			= new Joystick(0);
	m_joystickLift			= new Joystick(1);
	m_buttonBox				= new Joystick(2);

	m_buttonHookState		= new JoystickButton(m_joystickLift, 8);
	m_buttonWinchEnable 	= new JoystickButton(m_joystickDrive, 8);

	m_buttonLiftJoystick 	= new JoystickButton(m_joystickLift, 1);
	m_buttonIntakeJoystick  = new JoystickButton(m_joystickLift, 2);
	m_buttonRollerJoystick	= new JoystickButton(m_joystickLift, 3);
	m_buttonLiftBumpUp      = new JoystickButton(m_joystickLift, 11);
	m_buttonLiftBumpDown    = new JoystickButton(m_joystickLift, 10);

	m_buttonLiftSwitch		= new JoystickButton(m_buttonBox, 1);
	m_buttonLiftScaleBot	= new JoystickButton(m_buttonBox, 2);
	m_buttonLiftScaleMid	= new JoystickButton(m_buttonBox, 3);
	m_buttonLiftScaleTop	= new JoystickButton(m_buttonBox, 4);
	m_buttonIntakePickup   	= new JoystickButton(m_buttonBox, 5);
	m_buttonLiftTravel		= new JoystickButton(m_buttonBox, 6);
	m_buttonIntakeStow		= new JoystickButton(m_buttonBox, 7);
	m_buttonIntakeManual	= new JoystickButton(m_buttonBox, 8);
	m_buttonIntakeDrop		= new JoystickButton(m_buttonBox, 9);
	m_buttonIntakeEject 	= new JoystickButton(m_buttonBox, 10);
	m_buttonIntakeLaunch	= new JoystickButton(m_buttonBox, 11);

	m_buttonShiftLowTrig	= new JoystickButton(m_joystickDrive, 1);

	m_buttonHookState->WhenPressed(new CmdHookState(true));
	m_buttonHookState->WhenReleased(new CmdHookState(false));

	m_buttonWinchEnable->WhenPressed(new CmdClimberWinch(true));
	m_buttonWinchEnable->WhenReleased(new CmdClimberWinch(false));

	m_buttonIntakeJoystick->WhenPressed(new CmdCubeIntakeJoystick(true));
	m_buttonIntakeJoystick->WhenReleased(new CmdCubeIntakeJoystick(false));
	m_buttonRollerJoystick->WhenPressed(new CmdRollerJoystick(true));
	m_buttonRollerJoystick->WhenReleased(new CmdRollerJoystick(false));

	m_buttonIntakeDrop->	WhenPressed(new CmdCubeIntakeMode(CubeIntake::imDrop));
	m_buttonIntakePickup->	WhenPressed(new CmdCubeIntakeMode(CubeIntake::imPickup));
	m_buttonIntakeEject-> 	WhenPressed(new CmdCubeIntakeMode(CubeIntake::imEject));
	m_buttonIntakeLaunch->	WhenPressed(new CmdCubeIntakeMode(CubeIntake::imLaunch));
	m_buttonIntakeStow->	WhenPressed(new CmdCubeIntakeMode(CubeIntake::imStow));

	m_buttonIntakeManual->	WhenPressed(new CmdCubeIntakeMode(CubeIntake::imManual));
	m_buttonIntakeManual->	WhenReleased(new CmdCubeIntakeMode(CubeIntake::imOff));

	m_buttonLiftJoystick->	WhenReleased(new CmdLiftJoystick(false));
	m_buttonLiftJoystick->	WhenPressed(new CmdLiftJoystick(true));

	m_buttonLiftTravel->	WhenPressed(new CmdLiftPosition(dvLiftTravel));
	m_buttonLiftSwitch->	WhenPressed(new CmdLiftPosition(dvLiftSwitch));
	m_buttonLiftScaleBot->	WhenPressed(new CmdLiftPosition(dvLiftScaleBot));
	m_buttonLiftScaleMid->	WhenPressed(new CmdLiftPosition(dvLiftScaleMid));
	m_buttonLiftScaleTop->	WhenPressed(new CmdLiftPosition(dvLiftScaleTop));
	m_buttonLiftBumpUp->    WhenPressed(new CmdLiftBump(true));
	m_buttonLiftBumpDown->  WhenPressed(new CmdLiftBump(false));

	m_buttonShiftLowTrig->  WhenPressed(new CmdDriveShift(Drive::shifterLow));
	m_buttonShiftLowTrig->  WhenReleased(new CmdDriveShift(Drive::shifterHigh));
}

double OI::ApplyDeadband(double joyValue, double deadband) {
	deadband = fabs (deadband);

	if(fabs(joyValue) < deadband) {
		return 0;
	} else if(joyValue > 0) {
		return (joyValue - deadband)/(1 - deadband);
	} else {
		return (joyValue + deadband)/(1 - deadband);
	}
}

double OI::GetDriveX() {
	return ApplyDeadband(m_joystickDrive->GetX(), 0.05);
//	return 0;
}

double OI::GetDriveY(){
	return ApplyDeadband(-m_joystickDrive->GetY(), 0.05);
//	return 0;
}

double OI::GetLiftY() {
	return ApplyDeadband(m_joystickLift->GetY(), 0.1);
//	return 0;
}

double OI::GetLiftX() {
	return ApplyDeadband(m_joystickLift->GetX(), 0.1);
}

bool OI::GetLightOverride() {
	return m_joystickLift->GetRawButton(7);
}
