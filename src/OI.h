#pragma once
#include <WPILib.h>


class OI {
public:
	OI();

	double GetDriveX();
	double GetDriveY();

	double GetLiftY();
	double GetLiftX();

	bool   GetLightOverride();

private:
	Joystick*		m_joystickDrive;
	Joystick*		m_joystickLift;
	Joystick*		m_buttonBox;

	JoystickButton* m_buttonWinchEnable;
	JoystickButton* m_buttonHookState;

	JoystickButton*	m_buttonIntakeDrop;
	JoystickButton* m_buttonIntakeJoystick;
	JoystickButton* m_buttonIntakePickup;
	JoystickButton* m_buttonIntakeEject;
	JoystickButton* m_buttonIntakeLaunch;
	JoystickButton* m_buttonIntakeStow;
	JoystickButton*	m_buttonIntakeManual;
	JoystickButton*	m_buttonRollerJoystick;

	JoystickButton*	m_buttonLiftJoystick;
	JoystickButton* m_buttonLiftTravel;
	JoystickButton* m_buttonLiftSwitch;
	JoystickButton* m_buttonLiftScaleBot;
	JoystickButton* m_buttonLiftScaleMid;
	JoystickButton* m_buttonLiftScaleTop;
	JoystickButton* m_buttonLiftBumpUp;
	JoystickButton* m_buttonLiftBumpDown;

	JoystickButton* m_buttonShiftLowTrig;


	double ApplyDeadband(double joyValue, double deadband);
};
