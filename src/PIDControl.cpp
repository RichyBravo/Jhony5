#include "PIDControl.h"
#include <math.h>
#include <stdio.h>

// ************************* PUBLIC FUNCTIONS *************************

PIDControl::PIDControl() {
	m_P.threshold	= 0;
	m_P.kAbove 		= 0;
	m_P.kBelow 		= 0;
	m_P.kNow 		= 0;

	m_I.threshold 	= 0;
	m_I.kAbove 		= 0;
	m_I.kBelow 		= 0;
	m_I.kNow 		= 0;

	m_D.threshold 	= 0;
	m_D.kAbove 		= 0;
	m_D.kBelow 		= 0;
	m_D.kNow 		= 0;

	m_errorPrev 	= 0;
	m_errorReset 	= true;
	m_errorTotal	= 0;
	m_inputMax 		= 0;
	m_inputMin 		= 0;
	m_outputMax 	= 1.0;
	m_outputMin 	= -1.0;
	m_rampMin		= 0;
	m_rampNow		= 0;
	m_rampStep		= 0;
	m_setpoint 		= 0;
	m_useRamp		= false;
}

PIDControl::~PIDControl() {
}

double PIDControl::Calculate(double input, bool showResults) {		// Calculate output based on input
	double error = m_setpoint - input;								// Current Error
	double errorDiff = error - m_errorPrev;							// Difference between current error and previous error

	if (m_errorReset) {
		m_errorReset = false;
		errorDiff = 0;
	}

	if (m_P.threshold > 0) {										// Compare error to threshold for variable coefficients
		if (fabs(error) < m_P.threshold) {
			m_P.kNow = m_P.kBelow;
		} else {
			m_P.kNow = m_P.kAbove;
		}
	}

	if (m_I.threshold > 0) {
		if (fabs(error) < m_I.threshold) {
			if (m_I.kNow != m_I.kBelow) {
				m_I.kNow = m_I.kBelow;
			}
		} else if (m_I.kNow != m_I.kAbove) {
			m_I.kNow = m_I.kAbove;
		}
	}

	if (m_D.threshold > 0) {
		if (fabs(error) < m_D.threshold) {
			m_D.kNow = m_D.kBelow;
		} else {
			m_D.kNow = m_D.kAbove;
		}
	}

	double output = m_P.kNow * error + m_D.kNow * errorDiff;		// Calculate P and D portion of output

	if (m_I.kNow != 0) {											// If non-zero I coefficient
		double iGain = (m_errorTotal + error) * m_I.kNow;			// Calculate potential I gain

		if ((output + iGain) <= m_outputMax) {						// Total Output < Maximum
			if ((output + iGain) < m_outputMin) {					// Total Output < Mimimum
				output = m_outputMin;								// Set Total = Minimum
			} else {
				m_errorTotal += error;								// Integrate current error
				output += iGain;									// Add I Gain to Output
			}
		} else {
			output = m_outputMax;									// Set Total = Maximum
		}
	} else {
		m_errorTotal = 0;
	}

	if (m_useRamp) {												// Output ramping
		if (m_rampNow < 1.0) m_rampNow += m_rampStep;				// Limit output change to size of ramp Step
		output = std::copysign(std::min(fabs(output), m_rampNow), output);
	}

	if (output > m_outputMax) {										// Limit result to within Minimum-Maximum range
		output = m_outputMax;
	} else if (output < m_outputMin) {
		output = m_outputMin;
	}

	if (showResults) {												// Show Results
		printf("PID Calculate: Error=%7.1f  P=%6.2f  I=%6.2f  D=%6.2f  Output=%6.2f\n", error, m_P.kNow * error,
				m_I.kNow * m_errorTotal, m_D.kNow * errorDiff, output);
	}

	m_errorPrev = error;											// Set Previous error for next calculation

	return output;
}

double PIDControl::GetError() {
	return m_errorPrev;
}

void PIDControl::Reset() {
	m_errorReset = true;
	m_errorTotal = 0;
	m_rampNow = m_rampMin;
}

void PIDControl::SetCoefficient(char kWhich, double threshold, double kAbove, double kBelow) {
	switch(kWhich) {
		case 68:													// D Coefficient
		case 100:
			m_D.threshold = threshold;
			m_D.kAbove = kAbove;
			m_D.kBelow = kBelow;
			m_D.kNow = kAbove;
			break;

		case 73:													// I Coefficient
		case 105:
			m_I.threshold = threshold;
			m_I.kAbove = kAbove;
			m_I.kBelow = kBelow;
			m_I.kNow = kAbove;
			break;

		case 80:													// P Coefficient
		case 112:
			m_P.threshold = threshold;
			m_P.kAbove = kAbove;
			m_P.kBelow = kBelow;
			m_P.kNow = kAbove;
			break;

		default:;
	}
}

void PIDControl::SetInputRange(double inputMinimum, double inputMaximum) {
	m_inputMin = inputMinimum;
	m_inputMax = inputMaximum;
}

void PIDControl::SetOutputRange(double outputMinimum, double outputMaximum) {
	m_outputMin = outputMinimum;
	m_outputMax = outputMaximum;
}

void PIDControl::SetOutputRamp(double rampMinimum, double rampStep) {
	m_rampMin = fabs(rampMinimum);
	m_rampStep = fabs(rampStep);
	m_useRamp = (m_rampMin > 0);
}

void PIDControl::SetSetpoint(double setpoint) {
	if (m_inputMax > m_inputMin) {
		if (setpoint > m_inputMax) {
			setpoint = m_inputMax;
		} else if (setpoint < m_inputMin) {
			setpoint = m_inputMin;
		}
	}

	if (m_setpoint != setpoint) {
		m_setpoint = setpoint;
		Reset();
	}
}

