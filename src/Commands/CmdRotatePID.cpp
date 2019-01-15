#include "CmdRotatePID.h"
#include "Robot.h"

CmdRotatePID::CmdRotatePID(double angle, double maxSpeed, bool resetGyro,
						   double timeout, bool resetCancel) {
	Requires(drive.get());

	m_angle			= angle;									// Initialize Variables
	m_maxSpeed		= fabs(maxSpeed);
	m_resetCancel 	= resetCancel;
	m_resetGyro		= resetGyro;
	m_runStatus		= 0;
	m_timeout		= timeout;
}

void CmdRotatePID::Initialize() {
	m_runStatus = 0;

	if (m_resetCancel) cb_cmdCancel = false;

	if (cb_cmdCancel) {											//Skip the command if part of a command group
		m_runStatus = 1;										//that has been canceled

	} else {
		cb_driveInUse = true;

		if (m_angle == GAME_DATA_SCALE) {
			double currentAngle = drive->GetAngle();

			if (cb_scaleLeft) {
				if (currentAngle > 90)	m_angle = 270;
				else 					m_angle = -90;
			} else {
				if (currentAngle < -90) m_angle = -270;
				else					m_angle = 90;
			}
		}
																//Initialize Rotate PID
		drive->RotatePidInit(m_angle, m_maxSpeed, m_resetGyro);

		if(m_timeout > 0) SetTimeout(m_timeout);				//Set timeout if applicable

		sprintf(cb_log, "Auto: Rotate INIT Angle=%5.1f to %5.1f  MaxSpeed=%3.1f", drive->GetAngle(), m_angle, m_maxSpeed);
		Robot::robotLog->Write(cb_log);
	}
}

void CmdRotatePID::Execute() {
	if (m_runStatus == 0) {										//Execute if not Skipped, Canceled, or Timed out
		if (cb_cmdCancel) {										//Check run flag to determine if command was canceled
			m_runStatus = 2;

		} else if (IsTimedOut()) {								//Check to see if command timed out
			m_runStatus = 3;
			cb_cmdCancel = true;								//Set cancel to true to skip any subsequent commands

		} else {												//Drive robot using Rotate PID outputs
			drive->ArcadeDrive(0, drive->RotatePidOutput());
		}
	}
}

bool CmdRotatePID::IsFinished() {								//Command is finished if skipped, canceled, timed out, or
	return (cb_cmdCancel || drive->RotatePidFinished());		//if the the robot traveled the specified distance
}

void CmdRotatePID::End() {										//Print message to console when command has finished
	cb_driveInUse = false;

	switch (m_runStatus) {
		case 0: sprintf(cb_log, "----- Rotate DONE Angle=%5.1f", drive->GetAngle());
		 	 	break;
		case 1: sprintf(cb_log, "----- Rotate SKIP");
				break;
		case 2: sprintf(cb_log, "----- Rotate CANCELED");
				break;
		case 3: sprintf(cb_log, "----- Rotate TIMED OUT");
				break;
		default:;
	}

	Robot::robotLog->Write(cb_log);
}

void CmdRotatePID::Interrupted() {

}
