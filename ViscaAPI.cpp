#include "ViscaAPI.h"
#include "visca27.h"
#include <iostream>

ViscaAPI::ViscaAPI() {
	_url = "";
	_port = -1;
	_connectSocket = INVALID_SOCKET;
};
ViscaAPI::~ViscaAPI() {
	if (_connectSocket != INVALID_SOCKET) {
		int result = CloseSocket(_connectSocket);
	}
};
visca_error_t ViscaAPI::connectCamera(std::string url, int port) {
	_url = url;
	_port = port;
	int result = OpenSocket(&_connectSocket, _url, _port);
	std::cout << "error=" << result << std::endl;
	return result;
}
visca_error_t ViscaAPI::disconnectCamera() {
	if (_connectSocket != INVALID_SOCKET) {
		int result = CloseSocket(_connectSocket);
	}
	_url = "";
	_port = -1;
	_connectSocket = INVALID_SOCKET;
	return VOK;
};
visca_error_t ViscaAPI::getPanTilt(visca_tuple_t& out) {
	if (_connectSocket == INVALID_SOCKET) return VCONNECT_ERR;
	std::string returnhex;
	int result = GetCamera(_connectSocket, "81 09 06 12 FF", &returnhex);
	out = { 0,0 };
	ValueConverter getVC("90 50 0w 0w 0w 0w 0z 0z 0z 0z FF", 'w', 'z');
	if (result == VOK) {
		out.value1 = getVC.getValue1(returnhex);
		out.value2 = getVC.getValue2(returnhex);
	}
	return result;
};
visca_error_t ViscaAPI::setAbsolutePanTilt(visca_tuple_t pan_tilt) {
	if (_connectSocket == INVALID_SOCKET) return VCONNECT_ERR;
	ValueConverter setVC("81 01 06 02 10 10 0y 0y 0y 0y 0z 0z 0z 0z FF", 'y', 'z');
	std::string setcommand = setVC.setValue(pan_tilt.value1, pan_tilt.value2);
	int result = SetCamera(_connectSocket, setcommand);
	return result;
};