/*
 * Robot.h
 *
 *  Created on: Jan 21, 2018
 *      Author: Owner
 */

#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_


#include "WPILib.h"
#include "Dashboard.h"
#include "PIDControl.h"
#include "RobotCamera.h"
#include "RobotLog.h"

class Robot: public TimedRobot{
	public:
		static std::unique_ptr<Dashboard> dashboard;
		static std::unique_ptr<RobotCamera> robotCamera;
		static std::unique_ptr<RobotLog> robotLog;

	private:
		void RobotInit();
		void RobotPeriodic();
		void DisabledInit();
		void DisabledPeriodic();
		void AutonomousInit();
		void AutonomousPeriodic();
		void TeleopInit();
		void TeleopPeriodic();
		void TestInit();
		void TestPeriodic();

		CommandGroup* 	m_autoCommand;
		double 			m_autoDelay;
		int  			m_autoSelect;

//		cs::UsbCamera 	m_cameraFloor;
//		int				m_cameraOn;
//		cs::UsbCamera 	m_cameraScale;
//		cs::VideoSink 	m_cameraServer;
//		int				m_cameraView;

		char 			m_log[100];

		PIDControl*		m_tunePID;

};

#endif /* SRC_ROBOT_H_ */
