/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Climber.h"
#include "../RobotMap.h"
#include "../CommandBase.h"

Climber::Climber() : Subsystem("Climber") {
	m_hookAir	 	= new Solenoid(solHook);
	m_winchMotor 	= new VictorSP(pwmWinchMotor);

	m_hookAir->Set(false);
	m_winchMotor->SetSafetyEnabled(true);
	m_winchMotor->SetExpiration(0.1);

	m_hookState	 	= false;
	m_winchEnable 	= false;
}

void Climber::ClimberPeriodic() {
	if (m_winchEnable) {
		double winchSpeed = CommandBase::oi->GetDriveY();
		if (winchSpeed > 0) winchSpeed = 0;
		m_winchMotor->Set(winchSpeed);
	} else {
		m_winchMotor->Set(0);
	}
}

bool Climber::GetWinchEnabled(){
	return m_winchEnable;
}

void Climber::InitDefaultCommand() {
}

void Climber::SetHookState(bool state) {
	if (m_hookState != state) {
		m_hookState = state;
		m_hookAir->Set(m_hookState);
	}
}

void Climber::SetWinchEnable(bool enable){
	m_winchEnable = enable;
}

