#ifndef Drive_H
#define Drive_H

#include <Commands/Subsystem.h>
#include "WPILib.h"
#include "PIDControl.h"

class Drive : public Subsystem {
public:
	typedef enum {shifterLow, shifterHigh} ShifterState;
	typedef enum {encoderInUse, encoderLeft, encoderRight, encoderAverage} UseEncoder;

	Drive();
	void 			ArcadeDrive(double drive, double rotation);

	void			DrivePidAddTurn(double distance, double heading);
	void			DrivePidClearTurn();
	bool			DrivePidFinished();
	void			DrivePidInit(double setpoint, double maxSpeed, UseEncoder encoder, bool resetEncoder = true);
	double 			DrivePidOutput();
	void			DrivePidTune();

	ShifterState	GetShifterState();
	double			GetAngle();
	double			GetDistance(UseEncoder encoder = encoderInUse);
	void 			InitDefaultCommand();

	bool			RotatePidFinished();
	void			RotatePidInit(double setpoint, double maxSpeed, bool resetGyro = true);
	double			RotatePidOutput();
	void			RotatePidSetpoint(double setpoint);
	void			RotatePidTune();

	void 			SetDashboardValues();
	void			SetShifterState(ShifterState state);

private:
	Encoder*		m_encoderLeft;
	Encoder*		m_encoderRight;

	ADXRS450_Gyro*	m_gyro;

	VictorSP*		m_motorLeft1;
	VictorSP*		m_motorLeft2;
	VictorSP*		m_motorRight1;
	VictorSP*		m_motorRight2;

	PIDControl*		m_pidDrive;
	PIDControl*		m_pidRotate;

	Solenoid*		m_solShifter;

	ShifterState	m_shifterState;

	UseEncoder		m_useEncoder;

	struct turn {
		double distance;
		double heading;
	};

	turn*			m_turn;
	int				m_turnCount;
	int				m_turnIndex;

	double 			PwmLimit(double value);
};

#endif  // Drive_H
