#ifndef Lift_H
#define Lift_H

#include <Commands/Subsystem.h>
#include <WPILib.h>
#include "PIDControl.h"

class Lift : public Subsystem {

public:
	typedef enum{potNone, potBot, potTop, potInUse} LiftPot;
	typedef enum{liftUp, liftDown} LiftDirection;

	Lift();
	double		GetLiftPosition(LiftPot pot);
	double		GetLiftRateOfChange(LiftPot pot);
	double		GetLiftSetpoint();
	bool		GetUseJoystick();
	void		InitDefaultCommand();
	bool		InArmDownZone();
	bool		LiftNearTarget();
	void 		LiftPeriodic();
	void		SetAutoArmTilt(bool enable, double angle);
	void		SetAutoArmStow(bool enable);
	void		SetDashboardValue();
	void 		SetLiftPosition(int positionIndex);
	void		SetLiftPosition(double setpoint, int positionIndex = -1);
	void		SetUseJoystick(bool useJoystick);
	void		TuneLiftPID(double speed, LiftDirection direction);

	#define COUNT_PER_INCH 36.64

private:
	VictorSP*				m_liftMotorBot;
	VictorSP*				m_liftMotorTop;

//	PowerDistributionPanel*	m_PDP;

	PIDControl* 			m_liftPID;

	AnalogInput* 			m_liftPotTop;
	AnalogInput* 			m_liftPotBot;

	bool			m_autoArmTilt;
	double    		m_autoArmTiltAngle;
	bool			m_autoArmStow;
	LiftDirection	m_liftDirection;
	double 			m_liftSetpoint;
	char			m_log[100];
	LiftPot			m_potInUse;
	bool 		 	m_useJoystick;
	bool			m_waitForArm;

	std::string		LiftPositionName(int index);
	void 			PotCheck();
	void			SetLiftDirection(LiftDirection direction);
};

#endif  // Lift_H
