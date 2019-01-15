#include <arpa/inet.h>
#include <Dashboard.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread>
#include "Robot.h"

#define BUFFER_LEN 512
#define	TCP_PORT 5801

// ************************* PUBLIC FUNCTIONS *************************

Dashboard::Dashboard(int robotStatusCount, int robotValueCount, int dashButtonCount, int dashValueCount) {
	m_robotMode = 0;											// Initialize variables and arrays

	m_robotStatusCount		= robotStatusCount;
	m_robotValueCount		= robotValueCount;
	m_dashboardButtonCount	= dashButtonCount;
	m_dashboardValueCount	= dashValueCount;

	m_robotStatus			= new int32_t[m_robotStatusCount];			// Robot status			(To dashboard)
	m_robotValue 			= new double[m_robotValueCount];			// Robot values			(To dashboard)
	m_dashboardButton		= new button[m_dashboardButtonCount];		// Dashboard buttons	(From dashboard)
	m_dashboardValue		= new double[m_dashboardValueCount];		// Dashboard values  	(From dashboard)

	for (int i = 0; i < m_robotStatusCount; i++) m_robotStatus[i] = 0;
	for (int i = 0; i < m_robotValueCount; i++) m_robotValue[i] = 0;
	for (int i = 0; i < m_dashboardButtonCount; i++) {
		m_dashboardButton[i].state = 0;
		m_dashboardButton[i].pressed = 0;
	}
																// Initialize robot settings from file on RoboRio
	std::ifstream dashFile("/home/lvuser/Dashboard.set", std::ios::binary);
		if (dashFile.good()) {
			for (int i = 0; i < m_dashboardValueCount; i++) dashFile.read((char*)&m_dashboardValue[i], sizeof(m_dashboardValue[i]));
			WriteToLog("Dashboard: Read Values from File");
		} else {
			for (int i = 0; i < m_dashboardValueCount; i++) m_dashboardValue[i] = 0;
			WriteToLog("Dashboard: Initialize Values to 0");
		}
	dashFile.close();

	m_timeStamp = "";

	std::thread m_task(Dashboard::TcpLoop, this);				// Start host thread
	m_task.detach();
}

Dashboard::~Dashboard() {
	delete [] m_robotValue;
	delete [] m_dashboardValue;
	delete [] m_dashboardButton;
}

std::string Dashboard::CountReply() {							// Format reply to COUNT request
	return "COUNT:" + DataString(m_robotMode, 1) + DataString(m_robotStatusCount, 1) + DataString(m_robotValueCount, 1)
					+ DataString(m_dashboardButtonCount, 1) + DataString(m_dashboardValueCount, 0) + "\r\n";
}

std::string Dashboard::DataString(int32_t number, int delimiter) {	// Create string with delimiter for integer value
	std::stringstream ss;
	ss << number;

	switch (delimiter) {
		case 1:		return ss.str() + ",";
		case 2: 	return ss.str() + "|";
		default:	return ss.str();
	}
}

std::string Dashboard::DataString(double number, int delimiter) {
																// Create string with delimiter for floating value
	std::stringstream ss;										// Output number to stream with fixed precision 0.000000
	ss << std::fixed << std::setprecision(6) << number;

	std::string ds = ss.str();									// Convert stream to string and trim trailing 0's
	ds.erase(ds.find_last_not_of('0') + 1, std::string::npos);

	std::string::iterator it = ds.end() - 1;					// Remove '.' if it is the last character
	if (*it == '.') ds.erase(it);

	switch (delimiter) {
		case 1:		return ds + ",";
		case 2: 	return ds + "|";
		default:	return ds;
	}
}

bool Dashboard::GetDashButton(int buttonIndex) {				// Get current state of dashboard button
	int group = buttonIndex / 16;
	int index = buttonIndex % 16;

	if (index < 16 && group < m_dashboardButtonCount) {
		return ((m_dashboardButton[group].state & (1 << index)) != 0);
	} else {
		return false;
	}
}

bool Dashboard::GetDashButtonPress(int buttonIndex) {			// Determine whether dashboard button was pressed
	int  group = buttonIndex / 16;								// since previous call
	int  index = buttonIndex % 16;
	bool vReturn = false;

	if (group < m_dashboardButtonCount) {
		int32_t buttonValue = 1 << index;

		if ((m_dashboardButton[group].state & buttonValue) != 0) {
			vReturn = ((m_dashboardButton[group].pressed & buttonValue) == 0);
			m_dashboardButton[group].pressed |= buttonValue;

		} else if ((m_dashboardButton[group].pressed & buttonValue) != 0) {
			m_dashboardButton[group].pressed ^= buttonValue;
		}
	}

	return vReturn;
}

double Dashboard::GetDashValue(int index) {						// Get current value from dashboard data array
	if (index < m_dashboardValueCount) return m_dashboardValue[index];
	return 0;
}

std::string Dashboard::GetReply() {	// Format reply to GET robot data request
	std::string data = "GET:" + DataString(m_robotMode, 2);

	if (m_robotStatusCount > 0) {
		for (int i = 0; i < m_robotStatusCount; i++) data += DataString(m_robotStatus[i], 1);
		data = data.substr(0, data.length() - 1);
	}

	data += "|";

	if (m_robotValueCount > 0) {
		for (int i = 0; i < m_robotValueCount; i++) data += DataString(m_robotValue[i], 1);
		data = data.substr(0, data.length() - 1);
	}

	return data += "\r\n";
}

bool Dashboard::GetRobotStatus(int statusIndex) {
	int group = statusIndex / 16;
	int index = statusIndex % 16;

	if (index < 16 && group < m_robotStatusCount) {
		return ((m_robotStatus[group] & (1 << index)) != 0);
	} else {
		return false;
	}
}

std::string Dashboard::GetTimeStamp() {							// Get driver station time stamp
	return m_timeStamp;
}

void Dashboard::SaveDashValues() {								// Save dashboard values to settings file on RoboRio
	std::ofstream dashFile("/home/lvuser/Dashboard.set", std::ios::binary);
		if (dashFile.good()) {
			for (int i = 0; i < m_dashboardValueCount; i++) dashFile.write((char*)&m_dashboardValue[i], sizeof(m_dashboardValue[i]));
			WriteToLog("Dashboard: Save Values to File");
		}
	dashFile.close();
}

bool Dashboard::SetDashButton(int group, int32_t value) {		// Set a dashboard button state
	if (group < 0 || group >= m_dashboardButtonCount) {			// (Used by SET B command from dashboard)
		return false;
	} else {
		m_dashboardButton[group].state = value;
		return true;
	}
}

bool Dashboard::SetDashValue(int index, double value) {			// Set a dashboard value
	if (index < 0 || index >= m_dashboardValueCount) {			// (Used by SET V command from dashboard)
		return false;
	} else {
		m_dashboardValue[index] = value;
		return true;
	}
}

bool Dashboard::SetRobotStatus(int statusIndex, bool value) {	// Set a status in Robot status array
	int group = statusIndex / 16;
	int index = statusIndex % 16;

	if (group >= m_robotStatusCount) {
		return false;
	} else {
		int32_t statusValue = 1 << index;

		if (value) {
			m_robotStatus[group] |= statusValue;
		} else if ((m_robotStatus[group] & statusValue) != 0) {
			m_robotStatus[group] ^= statusValue;
		}

		return true;
	}
}

bool Dashboard::SetRobotValue(int index, double value) {		// Set a value in Robot value array
	if (index < 0 || index >= m_robotValueCount) {
		return false;
	} else {
		m_robotValue[index] = value;
		return true;
	}
}

void Dashboard::SetRobotMode(int mode) {						// Set the current robot mode
	m_robotMode = mode;											// (Sent in GET reply)
}

void Dashboard::SetTimeStamp(std::string now) {					// Set the driver station time stamp
	m_timeStamp = now;											// (Sent in GET reply)
}

void Dashboard::TcpLoop(Dashboard *host) {
	struct sockaddr_in	addrHost, addrClient;
	size_t				position;
	std::string			command;
	std::string			reply;
	std::string			clientMesg;
	unsigned int		clientLen;
	int					clientSocket;
	int					hostSocket;
	int					index;
	int					replySize;

	if ((hostSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {						// Create Host TCP socket
		printf("Dashboard: TCP Socket Error\n");
//		host->WriteToLog("Dashboard: TCP Socket Error");
		return;
	}

	index = 1;
	setsockopt(hostSocket, SOL_SOCKET, SO_REUSEADDR, &index, sizeof(int));					// Allow socket to be reused

	struct timeval tv;																		// Set Timeout on socket recv function
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	setsockopt(hostSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	memset(&addrHost, 0, sizeof(addrHost));													// Create and configure Socket address
	addrHost.sin_family = AF_INET;
	addrHost.sin_addr.s_addr = htonl(INADDR_ANY);
	addrHost.sin_port = htons(TCP_PORT);

	if (bind(hostSocket, (struct sockaddr *) &addrHost, sizeof(addrHost)) < 0) {			// Bind address to Host Socket
		printf("Dashboard: TCP Binding Error\n");
//		host->WriteToLog("Dashboard: TCP Binding Error");
		return;
	}

	if (listen(hostSocket, 5) < 0) {														// Open socket to listen for clients
		printf("Dashboard: TCP Listen Failed\n");
//		host->WriteToLog("Dashboard: TCP Listen Failed");
		return;
	}

	char buffer[BUFFER_LEN];

	while(true) {
		clientLen = sizeof(addrClient);
																							// Accept connection from Dashboard client
		if ((clientSocket = accept(hostSocket, (struct sockaddr*) &addrClient, &clientLen)) >= 0) {
			printf("Dashboard: Connection Accepted\n");
//			host->WriteToLog("Dashboard: Connection Accepted");

			while(recv(clientSocket, buffer, BUFFER_LEN, 0) > 0) {							// Wait to receive message from client
//				printf("Client Message: %s\n", buffer);
				clientMesg = std::string(buffer);											// Retrieve message from buffer

				if ((position = clientMesg.find(":")) != std::string::npos) {				// Look for colon at end of command
					command = clientMesg.substr(0, position);								// Parse command
					clientMesg.erase(0, position + 1);										// Erase command from message

					if (command == "COUNT") {												// COUNT command requesting data counts
						reply = host->CountReply();											// Reply from Host

					} else if (command == "GET") {											// GET command requesting Robot data
						if ((position = clientMesg.find("|")) != std::string::npos) {		// Look for pipe at end of Time Stamp
							host->SetTimeStamp(clientMesg.substr(0, position));				// Parse and Set Time Stamp
							clientMesg.erase(0, position + 1);
							reply = host->GetReply();
						}

					} else if (command == "PUT") {											// PUT command sending Dashboard data
						reply = "PUT:";
						bool saveFile = false;

						while ((position = clientMesg.find("|")) != std::string::npos) {	// Look for pipe at end of each data packet
							command = clientMesg.substr(0, position);						// Parse data packet
							clientMesg.erase(0, position + 1);								// Erase packet from message

							if ((position = command.find(",")) != std::string::npos) {		// Look for comma after PUT Group (B or V)
								std::string group = command.substr(0, position);			// Parse group
								command.erase(0, position + 1);								// Erase group from packet

								if ((position = command.find(",")) != std::string::npos) {	// Look for comma after PUT Index
									index = atoi(command.substr(0, position).c_str());		// Parse index

									if (group == "V") {										// Set Dashboard Value at index
										if (host->SetDashValue(index, atof(command.substr(position + 1).c_str()))) {
											reply += "V," + host->DataString(index, 2);		// Acknowledge reply
											saveFile = true;
										}
									} else if (group == "B") {								// Set Dashboard Button at index
										if (host->SetDashButton(index, atoi(command.substr(position + 1).c_str()))) {
											reply += "B," + host->DataString(index, 2);		// Acknowledge reply
										}
									}
								}
							}
						}

						reply += "\r\n";
						if (saveFile) host->SaveDashValues();
					}

					replySize = reply.length();

					if (replySize > 0) {													// Send reply to Dashboard client
						memcpy(buffer, reply.data(), BUFFER_LEN);
						if (send(clientSocket, buffer, replySize, 0) != replySize) host->WriteToLog("Dashboard: TCP Send Error");
					}
				}
			}

			printf("Dashboard: Connection Lost\n");
//			host->WriteToLog("Dashboard: Connection Lost");
		}
	}
}

void Dashboard::WriteToLog(std::string entry) {
	printf("%s\n", entry.c_str());
	Robot::robotLog->Write(entry);
}

