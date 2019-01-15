#ifndef CubeIntake_H
#define CubeIntake_H

#include <Commands/Subsystem.h>
#include "WPILib.h"
#include "PIDControl.h"

class CubeIntake : public Subsystem {
public:
	typedef enum {imOff, imPickup, imStow, imDrop, imEject, imLaunch, imManual, imAutoOpen, imAutoPickup} IntakeMode;
	typedef enum {gsOff, gsDrop, gsClose, gsGrip} GripState;

	CubeIntake();
	bool 		ArmIsDown();
	bool		CubeDetected();
	bool		CubeOnboard();
	void 		CubePeriodic();
	double		GetArmPosition();
	void 		InitDefaultCommand();
	bool		ModeChangeFinished();
	void 		SetArmPosition(int positionIndex);
	void		SetArmPosition(double setpoint, int positionIndex = -1);
	void 		SetArmSpeed(double speed);
	void		SetDashboardValues();
	void 		SetGripState(GripState state);
	void 		SetIntakeMode(IntakeMode mode, double launchSpeed = 0);
	void		SetUseJoystick(bool useJoystick);
	void		SetRollerJoystick(bool useJoystick);

	#define COUNT_PER_DEGREE 33.33

private:
	VictorSP*		m_armMotor;
	AnalogInput*	m_armPot;
	PIDControl*		m_armPID;

	DigitalInput*	m_cubeSensorNear;
	DigitalInput*	m_cubeSensorFar;

	VictorSP*		m_rollerLeft;
	VictorSP*		m_rollerRight;

	Solenoid*		m_solGrip;
	Solenoid*		m_solDrop;

	double			m_armSetpoint;
	bool			m_earlyFinish;
	int				m_ejectCount;
	int				m_gripCount;
	GripState 		m_gripState;
	IntakeMode		m_intakeMode;
	double			m_launchSpeed;
	char			m_log[100];
	double			m_rollerState;
	bool			m_useJoystick;
	bool			m_rollerJoystick;

	std::string ArmPositionName(int positionIndex);
	std::string CubeRotateName(int option);
	std::string GripStateName(int gripIndex);
	GripState	GetGripState();
	std::string IntakeModeName(int index);
};

#endif  // CubeIntake_H
