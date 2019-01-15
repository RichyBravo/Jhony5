
#include <WPILib.h>
#include "CommandBase.h"
#include "Robot.h"
#include "Commands/AutoCommands.h"
#include "LiveWindow/LiveWindow.h"

std::unique_ptr<Dashboard> Robot::dashboard;
std::unique_ptr<RobotCamera> Robot::robotCamera;
std::unique_ptr<RobotLog> Robot::robotLog;

void Robot::RobotInit() {
	LiveWindow::GetInstance()->DisableAllTelemetry();

	robotLog.reset(new RobotLog());
	dashboard.reset(new Dashboard(1, 19, 1, 41));
	robotCamera.reset(new RobotCamera());

	robotLog->SetMode(RobotLog::mInit);

	SetPeriod(0.02);
	CommandBase::Init();
	robotCamera->Init();

	m_autoSelect	= -1;
	m_autoCommand	= nullptr;
	m_autoDelay		=  0;
	m_tunePID    	=  new PIDControl();
}

void Robot::RobotPeriodic() {
	CommandBase::drive->SetDashboardValues();
	CommandBase::cubeIntake->SetDashboardValues();
	CommandBase::lift->SetDashboardValue();
}

void Robot::DisabledInit() {
	dashboard->SetRobotMode(1);
	robotLog->SetMode(RobotLog::mDisabled);
	robotLog->Close();

	CommandBase::lift->SetAutoArmStow(true);
}

void Robot::DisabledPeriodic() {
	frc::Scheduler::GetInstance()->Run();
}

void Robot::AutonomousInit() {
	dashboard->SetRobotMode(2);
	robotLog->SetMode(RobotLog::mAutonomous);

	CommandBase::lift->SetLiftPosition(CommandBase::lift->GetLiftPosition(Lift::potInUse));
	CommandBase::lift->SetAutoArmStow(false);
	CommandBase::cubeIntake->SetArmPosition(CommandBase::cubeIntake->GetArmPosition());
	CommandBase::drive->SetShifterState(Drive::shifterHigh);

	std::string gameData;
	gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();

	if (gameData.compare("LLL") == 0){
		CommandBase::cb_scaleLeft = true;
		m_autoSelect	= (int) dashboard->GetDashValue(dvAutoSelectLLL);
	} else if (gameData.compare("LRL") == 0) {
		CommandBase::cb_scaleLeft = false;
		m_autoSelect	= (int) dashboard->GetDashValue(dvAutoSelectLRL);
	} else if (gameData.compare("RLR") == 0) {
		CommandBase::cb_scaleLeft = true;
		m_autoSelect	= (int) dashboard->GetDashValue(dvAutoSelectRLR);
	} else if (gameData.compare("RRR") == 0) {
		CommandBase::cb_scaleLeft = false;
		m_autoSelect	= (int) dashboard->GetDashValue(dvAutoSelectRRR);
	} else {
		m_autoSelect 	= 0;
	}

	m_autoDelay = dashboard->GetDashValue(dvAutoDelay) / 4;
	std::string autoName = "";

	switch (m_autoSelect){
		case 1:  m_autoCommand = new AutoDrive(m_autoDelay);
				 autoName = "Drive to Auto Line";
				 break;
		case 11: m_autoCommand = new Auto1SwitchLeft(m_autoDelay);
				 autoName = "1: Left Switch";
				 break;
		case 12: m_autoCommand = new Auto1ScaleLeft(m_autoDelay);
				 autoName = "1: Left Scale";
				 break;
		case 13: m_autoCommand = new Auto1ScaleX2Left(m_autoDelay);
				 autoName = "1: Left Scale-Scale";
				 break;
		case 14: m_autoCommand = new Auto1ScaleSwitchLeft(m_autoDelay);
				 autoName = "1: Left Scale-Switch";
				 break;
		case 15: m_autoCommand = new Auto1ScaleRight(m_autoDelay);
				 autoName = "1: Right Scale";
				 break;
		case 16: m_autoCommand = new Auto1ScaleX2Right(m_autoDelay);
				 autoName = "1: Right Scale-Scale";
				 break;
		case 17: m_autoCommand = new Auto1DriveForward(m_autoDelay);
				 autoName = "1: Drive-Turn";
				 break;
		case 21: m_autoCommand = new Auto2SwitchLeft(m_autoDelay);
				 autoName = "2: Left Switch";
				 break;
		case 22: m_autoCommand = new Auto2SwitchX2Left(m_autoDelay);
				 autoName = "2: Left Switch-Switch";
				 break;
		case 23: m_autoCommand = new Auto2SwitchExchLeft(m_autoDelay);
				 autoName = "2: Left Switch-Exchange";
				 break;
		case 24: m_autoCommand = new Auto2SwitchRight(m_autoDelay);
		     	 autoName = "2: Right Switch";
				 break;
		case 25: m_autoCommand = new Auto2SwitchX2Right(m_autoDelay);
				 autoName = "2: Right Switch-Switch";
				 break;
		case 26: m_autoCommand = new Auto2SwitchExchRight(m_autoDelay);
				 autoName = "2: Right Switch-Exchange";
				 break;
		case 31: m_autoCommand = new Auto3SwitchRight(m_autoDelay);
				 autoName = "3: Right Switch";
				 break;
		case 32: m_autoCommand = new Auto3ScaleRight(m_autoDelay);
				 autoName = "3: Right Scale";
				 break;
		case 33: m_autoCommand = new Auto3ScaleX2Right(m_autoDelay);
				 autoName = "3: Right Scale-Scale";
				 break;
		case 34: m_autoCommand = new Auto3ScaleSwitchRight(m_autoDelay);
				 autoName = "3: Right Scale-Switch";
				 break;
		case 35: m_autoCommand = new Auto3ScaleLeft(m_autoDelay);
				 autoName = "3: Left Scale";
				 break;
		case 36: m_autoCommand = new Auto3ScaleX2Left(m_autoDelay);
				 autoName = "3: Left Scale-Scale";
				 break;
		case 37: m_autoCommand = new Auto3DriveForward(m_autoDelay);
				 autoName = "3: Drive-Turn";
				 break;
		default: m_autoCommand = nullptr;
				 autoName = "None";
				 break;
	}

	sprintf(m_log, "Auto:   Game Data=%s  Delay=%4.2f  Select=%s", gameData.c_str(), m_autoDelay, autoName.c_str());
	robotLog->Write(m_log);

	if (m_autoCommand != nullptr) m_autoCommand->Start();
}

void Robot::AutonomousPeriodic() {
	robotLog->StartPeriodic();
	robotLog->GetDataFromPDP();

	frc::Scheduler::GetInstance()->Run();

	CommandBase::lift->LiftPeriodic();
	CommandBase::cubeIntake->CubePeriodic();
	CommandBase::climber->ClimberPeriodic();

	if (!CommandBase::cb_driveInUse) CommandBase::drive->ArcadeDrive(0, 0);
	if (dashboard->GetDashButton(dbDataCapture)) robotLog->LogData();

	robotLog->EndPeriodic();
}

void Robot::TeleopInit() {
	dashboard->SetRobotMode(3);
	robotLog->SetMode(RobotLog::mTeleop);

	if (m_autoCommand != nullptr) m_autoCommand->Cancel();

	CommandBase::lift->SetLiftPosition(CommandBase::lift->GetLiftPosition(Lift::potInUse));
	CommandBase::cubeIntake->SetArmPosition(CommandBase::cubeIntake->GetArmPosition());
	CommandBase::cubeIntake->SetIntakeMode(CubeIntake::imOff);
	CommandBase::climber->SetWinchEnable(false);
}

void Robot::TeleopPeriodic() {
	robotLog->StartPeriodic();
	robotLog->GetDataFromPDP();

	if (dashboard->GetDashButtonPress(dbLiftScaleTop)) 	CommandBase::lift->SetLiftPosition(dvLiftScaleTop);
	if (dashboard->GetDashButtonPress(dbLiftScaleMid)) 	CommandBase::lift->SetLiftPosition(dvLiftScaleMid);
	if (dashboard->GetDashButtonPress(dbLiftScaleBot)) 	CommandBase::lift->SetLiftPosition(dvLiftScaleBot);
	if (dashboard->GetDashButtonPress(dbLiftSwitch)) 	CommandBase::lift->SetLiftPosition(dvLiftSwitch);
	if (dashboard->GetDashButtonPress(dbLiftTravel)) 	CommandBase::lift->SetLiftPosition(dvLiftTravel);

	if (dashboard->GetDashButtonPress(dbLiftBumpUp))	CommandBase::lift->SetLiftPosition(
														CommandBase::lift->GetLiftSetpoint() + dashboard->GetDashValue(dvLiftBumpStep));
	if (dashboard->GetDashButtonPress(dbLiftBumpDown))	CommandBase::lift->SetLiftPosition(
														CommandBase::lift->GetLiftSetpoint() - dashboard->GetDashValue(dvLiftBumpStep));

	if (dashboard->GetDashButtonPress(dbIntakePickup))	CommandBase::cubeIntake->SetIntakeMode(CubeIntake::imPickup);
	if (dashboard->GetDashButtonPress(dbIntakeStow)) 	CommandBase::cubeIntake->SetIntakeMode(CubeIntake::imStow);
	if (dashboard->GetDashButtonPress(dbIntakeEject))	CommandBase::cubeIntake->SetIntakeMode(CubeIntake::imEject);
	if (dashboard->GetDashButtonPress(dbIntakeLaunch))	CommandBase::cubeIntake->SetIntakeMode(CubeIntake::imLaunch);
	if (dashboard->GetDashButtonPress(dbIntakeDrop))	CommandBase::cubeIntake->SetIntakeMode(CubeIntake::imDrop);
	if (dashboard->GetDashButtonPress(dbIntakeTilt))	CommandBase::cubeIntake->SetArmPosition(dvIntakeTilt);

	frc::Scheduler::GetInstance()->Run();

	CommandBase::lift->LiftPeriodic();
	CommandBase::cubeIntake->CubePeriodic();
	CommandBase::climber->ClimberPeriodic();

	if (dashboard->GetDashButton(dbDataCapture)) robotLog->LogData();

	robotLog->EndPeriodic();
}

void Robot::TestInit() {
	dashboard->SetRobotMode(4);
	robotLog->SetMode(RobotLog::mTest);

	CommandBase::lift->SetLiftPosition(CommandBase::lift->GetLiftPosition(Lift::potInUse));
	CommandBase::cubeIntake->SetArmPosition(CommandBase::cubeIntake->GetArmPosition());
}

void Robot::TestPeriodic() {
	static double				setpoint = 0;
	static int 					tunePID = -1;
	static Lift::LiftDirection 	direction = Lift::liftUp;

	robotLog->GetDataFromPDP();

	if(dashboard->GetDashButton(dbRunPid)) {
		if(tunePID < 0){
			tunePID 	= dashboard->GetDashValue(dvPidSelect);
			setpoint 	= dashboard->GetDashValue(dvPidSetpoint);

			m_tunePID->SetOutputRamp(0.2, 0.05);
			m_tunePID->SetSetpoint(setpoint);
			m_tunePID->SetOutputRange(-dashboard->GetDashValue(dvPidMaxOut),
									   dashboard->GetDashValue(dvPidMaxOut));

			m_tunePID->SetCoefficient('P', dashboard->GetDashValue(dvPthreshold),
										   dashboard->GetDashValue(dvPabove),
										   dashboard->GetDashValue(dvPbelow));
			m_tunePID->SetCoefficient('I', dashboard->GetDashValue(dvIthreshold),
										   dashboard->GetDashValue(dvIabove),
										   dashboard->GetDashValue(dvIbelow));
			m_tunePID->SetCoefficient('D', dashboard->GetDashValue(dvDthreshold),
										   dashboard->GetDashValue(dvDabove),
										   dashboard->GetDashValue(dvDbelow));
			m_tunePID->Reset();

			switch(tunePID) {
				case 0: CommandBase::drive->DrivePidTune();
						CommandBase::drive->RotatePidInit(0, 0.6, true);
						break;

				case 1: CommandBase::drive->RotatePidTune();
						break;

				case 2: if(setpoint > CommandBase::lift->GetLiftPosition(Lift::potInUse)) direction = Lift::liftUp;
						else direction = Lift::liftDown;
						CommandBase::cubeIntake->SetGripState(CubeIntake::gsGrip);
						break;
			}
		}

		switch(tunePID) {
			case 0: CommandBase::drive->ArcadeDrive(m_tunePID->Calculate(CommandBase::drive->GetDistance(), true),
													CommandBase::drive->RotatePidOutput());
					break;

			case 1: CommandBase::drive->ArcadeDrive(0, m_tunePID->Calculate(CommandBase::drive->GetAngle(), true));
					break;

			case 2: CommandBase::lift->TuneLiftPID(m_tunePID->Calculate(CommandBase::lift->GetLiftPosition(Lift::potInUse), true), direction);
					break;

			case 3:	CommandBase::cubeIntake->SetArmSpeed(m_tunePID->Calculate(CommandBase::cubeIntake->GetArmPosition(), true));
					break;
		}

	} else {
		tunePID = -1;
		CommandBase::drive->ArcadeDrive(0,0);
		CommandBase::lift->LiftPeriodic();
		CommandBase::cubeIntake->CubePeriodic();
	}
}

START_ROBOT_CLASS(Robot)
