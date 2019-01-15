#ifndef PIDCONTROL_H_
#define PIDCONTROL_H_

#include <WPILib.h>

class PIDControl {
public:
	PIDControl();
	~PIDControl();

	double	Calculate(double input, bool showResults);
	double 	GetError();
	void	Reset();
	void 	SetCoefficient(char kWhich, double errorThreshold, double kAbove, double kBelow);
	void	SetInputRange(double inputMinimum, double inputMaximum);
	void	SetOutputRange(double outputMinimum, double outputMaximum);
	void 	SetOutputRamp(double rampMinimum, double rampStep);
	void	SetSetpoint(double setpoint);

private:
	struct Coefficient {
		double		threshold;
		double		kAbove;
		double		kBelow;
		double		kNow;
	};

	Coefficient	m_P;
	Coefficient m_I;
	Coefficient m_D;

	double	m_errorPrev;
	bool	m_errorReset;
	double	m_errorTotal;
	double	m_inputMax;
	double	m_inputMin;
	double	m_outputMax;
	double 	m_outputMin;
	double  m_rampMin;
	double  m_rampNow;
	double  m_rampStep;
	double	m_setpoint;
	bool	m_useRamp;
};

#endif







