#include "CmdDriveArc.h"
#include "Robot.h"

CmdDriveArc::CmdDriveArc(double  distance, double finalHeading, double maxSpeed, bool resetGyro,
						 double timeout, bool resetCancel) {
	Requires(drive.get());									//Specify needed subsystems

	m_angle = 0;											//Initialize Variables
	m_distance = distance;
	m_finalHeading = finalHeading;
	m_maxSpeed = fabs(maxSpeed);
	m_resetCancel = resetCancel;
	m_resetGyro = resetGyro;
	m_runStatus = 0;
	m_startAngle = 0;
	m_timeout = timeout;
}

void CmdDriveArc::Initialize() {
	m_runStatus = 0;

	if (m_resetCancel) cb_cmdCancel = false;

	if (cb_cmdCancel) {										//Skip the command if part of a command group
		m_runStatus = 1;									//that has been canceled

	} else {
		cb_driveInUse = true;

		if (!m_resetGyro) m_startAngle = drive->GetAngle();	//Get current angle if gyro will not be reset
		m_angle = m_finalHeading - m_startAngle;			//Determine the angle of the arc

		Drive::UseEncoder useEncoder = Drive::encoderLeft;	//Determine encoder to use based on direction of rotation
		if (m_angle < 0) useEncoder = Drive::encoderRight;	//(Use encoder on the outside of the arc)

															//Initialize Drive PID and Rotate PID
		drive->DrivePidInit(m_distance, m_maxSpeed, useEncoder, true);
		drive->RotatePidInit(m_startAngle, 0.7 , m_resetGyro);

		if (m_timeout > 0) SetTimeout(m_timeout);			//Set timeout if applicable

		sprintf(cb_log, "Auto: Arc INIT Distance= 0 to %5.1f  Arc=%5.1f  Final Heading=%5.1f  MaxSpeed=%3.1f",
			    m_distance, m_angle, m_finalHeading, m_maxSpeed);
		Robot::robotLog->Write(cb_log);
	}
}

void CmdDriveArc::Execute() {
	if (m_runStatus == 0) {									//Execute if not Skipped, Canceled, or Timed out
		if (cb_cmdCancel) {									//Check run flag to determine if command was canceled
			m_runStatus = 2;

		} else if (IsTimedOut()) {							//Check to see if command timed out
			m_runStatus = 3;
			cb_cmdCancel = true;							//Set cancel to true to skip any subsequent commands

		} else {											//Set Rotate PID setpoint based on distance traveled
			drive->RotatePidSetpoint(m_angle * (fabs(drive->GetDistance()) /
									 fabs(m_distance)) + m_startAngle);		//Drive robot using Drive and Rotate PID outputs
			drive->ArcadeDrive(drive->DrivePidOutput(), drive->RotatePidOutput());
		}
	}
}

bool CmdDriveArc::IsFinished() {							//Command is finished if skipped, canceled, timed out, or
	return (cb_cmdCancel || drive->DrivePidFinished());		//if the the robot traveled the specified distance
}

void CmdDriveArc::End() {									//Print message to console when command has finished
	cb_driveInUse = false;

	switch (m_runStatus) {
		case 0: sprintf(cb_log, "----- Arc DONE Distance Left=%5.1f  Right=%5.1f  Heading=%5.1f",
						drive->GetDistance(Drive::encoderLeft), drive->GetDistance(Drive::encoderRight), drive->GetAngle());
		        break;
		case 1: sprintf(cb_log, "----- Arc SKIP");
				break;
		case 2: sprintf(cb_log, "----- Arc CANCELED");
				break;
		case 3: sprintf(cb_log, "----- Arc TIMED OUT");
				break;
		default:;
	}

    Robot::robotLog->Write(cb_log);
}

void CmdDriveArc::Interrupted() {

}
