/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "../CommandBase.h"

class CmdLiftWait : public CommandBase{
public:
	CmdLiftWait(double timeout = 0, bool resetCancel = true);
	void Initialize() override;
	void Execute() override;
	bool IsFinished() override;
	void End() override;
	void Interrupted() override;

private:
	bool	m_resetCancel;
	int		m_runStatus;
	double 	m_timeout;
};

