// RobotCamera.cpp

#include "RobotCamera.h"
#include "Robot.h"

RobotCamera::RobotCamera() {
	m_cameraOn		= 0;
	m_cameraView	= cvNone;
}

void RobotCamera::Init() {
	CameraServer* cameraServer	= CameraServer::GetInstance();
	m_cameraOn					= (int)Robot::dashboard->GetDashValue(dvCameraOn);

	if ((m_cameraOn & 1) == 1) {
		m_cameraFloor = cameraServer->StartAutomaticCapture(0);
		m_cameraFloor.SetResolution(320, 240);
		m_cameraFloor.SetFPS(20);
		m_cameraFloor.SetBrightness(42);
		m_cameraFloor.SetExposureAuto();

		m_cameraServer.SetSource(m_cameraFloor);
		m_cameraServer = cameraServer->GetServer();
		m_cameraView = cvFloor;
	}

	if ((m_cameraOn & 2) == 2) {
		m_cameraScale = cameraServer->StartAutomaticCapture(1);
		m_cameraScale.SetResolution(320, 240);
		m_cameraScale.SetFPS(20);
		m_cameraScale.SetBrightness(42);
		m_cameraScale.SetExposureManual(30);

		if (m_cameraOn == 2) {
			m_cameraServer.SetSource(m_cameraScale);
			m_cameraServer = cameraServer->GetServer();
			m_cameraView = cvScale;
		}
	}
}

void RobotCamera::SetCameraView(CameraView view) {
	if (m_cameraOn == 3) {
		if (view == cvFloor && m_cameraView != cvFloor) {
			m_cameraView = cvFloor;
			m_cameraServer.SetSource(m_cameraFloor);
		} else if (view == cvScale && m_cameraView != cvScale) {
			m_cameraView = cvScale;
			m_cameraServer.SetSource(m_cameraScale);
		}
	}
}

