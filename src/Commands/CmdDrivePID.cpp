#include "CmdDrivePID.h"
#include "Robot.h"

CmdDrivePID::CmdDrivePID(double distance, double heading, double maxSpeed, Drive::UseEncoder useEncoder,
						 bool resetEncoder, bool resetGyro, double timeout, bool resetCancel, double maxRotate) {
	Requires(drive.get());

	m_continue		= (timeout < 0);						// Initialize variables
	m_distance 		= distance;
	m_heading 		= heading;
	m_maxRotate		= maxRotate;
	m_maxSpeed 		= fabs(maxSpeed);
	m_resetCancel	= resetCancel;
	m_resetEncoder	= resetEncoder;
	m_resetGyro 	= resetGyro;
	m_runStatus 	= 0;
	m_timeout 		= fabs(timeout);
	m_useEncoder 	= useEncoder;
}

void CmdDrivePID::Initialize() {
	m_runStatus = 0;

	if (m_resetCancel) cb_cmdCancel = false;

	if (cb_cmdCancel) {										//Skip the command if part of a command group
		m_runStatus = 1;									//that has been canceled

	} else {
		cb_driveInUse = true;
															//Initialize Drive PID and Rotate PID
		drive->DrivePidInit(m_distance, m_maxSpeed, m_useEncoder, m_resetEncoder);
		drive->RotatePidInit(m_heading, m_maxRotate, m_resetGyro);

		if (m_timeout > 0) SetTimeout(m_timeout);			//Set timeout if applicable

		sprintf(cb_log, "Auto: Drive INIT Distance=%5.1f to %5.1f  Heading=%5.1f to %5.1f  MaxSpeed=%3.1f",
				drive->GetDistance(m_useEncoder), m_distance, drive->GetAngle(),  m_heading, m_maxSpeed);
		Robot::robotLog->Write(cb_log);
	}
}

void CmdDrivePID::Execute() {
	if (m_runStatus == 0) {									//Execute if not Skipped, Canceled, or Timed out
		if (cb_cmdCancel) {									//Check run flag to determine if command was canceled
			m_runStatus = 2;

		} else if (IsTimedOut()) {							//Check to see if command timed out
			if (m_continue) {
				m_runStatus = 4;
			} else {
				m_runStatus = 3;
				cb_cmdCancel = true;						// Set cancel to true to skip any subsequent commands
			}

		} else {											//Drive robot using Drive and Rotate PID outputs
			drive->ArcadeDrive(drive->DrivePidOutput(), drive->RotatePidOutput());
		}
	}
}

bool CmdDrivePID::IsFinished() {							//Command is finished if skipped, canceled, timed out, or
	return (cb_cmdCancel || m_runStatus == 4 || drive->DrivePidFinished());		//if the the robot traveled the specified distance
}

void CmdDrivePID::End() {									//Print message to console when command has finished
	cb_driveInUse = false;
	drive->DrivePidClearTurn();

	switch (m_runStatus) {
		case 0: sprintf(cb_log, "----- Drive DONE Distance Left=%5.1f  Right=%5.1f  Heading=%5.1f",
						drive->GetDistance(Drive::encoderLeft), drive->GetDistance(Drive::encoderRight), drive->GetAngle());
				break;
		case 1: sprintf(cb_log, "----- Drive SKIP");
				break;
		case 2: sprintf(cb_log, "----- Drive CANCELED");
				break;
		case 3: sprintf(cb_log, "----- Drive TIMED OUT");
				break;
		case 4: sprintf(cb_log, "      Drive CONTINUE Distance Left=%5.1f  Right=%5.1f  Heading=%5.1f",
				drive->GetDistance(Drive::encoderLeft), drive->GetDistance(Drive::encoderRight), drive->GetAngle());
				break;
		default:;
	}

	Robot::robotLog->Write(cb_log);
}

void CmdDrivePID::Interrupted() {
}
