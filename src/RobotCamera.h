#include <WPILib.h>

#ifndef SRC_ROBOTCAMERA__H_
#define SRC_ROBOTCAMERA__H_

class RobotCamera
{
public:
	typedef enum {cvNone, cvFloor, cvScale} CameraView;

	RobotCamera();
	void Init();
	void SetCameraView(CameraView view);

private:
	cs::UsbCamera 	m_cameraFloor;
	int				m_cameraOn;
	cs::UsbCamera 	m_cameraScale;
	cs::VideoSink 	m_cameraServer;
	CameraView		m_cameraView;
};

#endif /* SRC_CAMERA__H_ */
