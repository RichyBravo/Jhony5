#include "CmdDriveShift.h"

CmdDriveShift::CmdDriveShift(Drive::ShifterState state) {
	Requires (drive.get());
	m_state = state;
	m_shiftRequired = false;
	m_shiftTimer = -1;
}

void CmdDriveShift::Initialize() {
	m_shiftRequired = (drive->GetShifterState() != m_state);
	m_shiftTimer = -1;
}


void CmdDriveShift::Execute() {
	if (m_shiftRequired) {
		m_shiftTimer ++;
		if(m_shiftTimer == 2) drive->SetShifterState(m_state); 		// Shift after 100 msec wait

		drive->ArcadeDrive(0, 0);
	}
}

bool CmdDriveShift::IsFinished() {
	return (!m_shiftRequired || m_shiftTimer >= 4);				// Wait additional 100 msec for shift completion
}

void CmdDriveShift::End() {
}

void CmdDriveShift::Interrupted() {
}
