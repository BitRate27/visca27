#include "visca27.h"
#include "ViscaAPI.h"
#include <iostream>

ViscaAPI::ViscaAPI() {
	_url = "";
	_port = -1;
	_connectSocket = INVALID_SOCKET;
};
ViscaAPI::~ViscaAPI() {
	if (_connectSocket != INVALID_SOCKET) {
		CloseSocket(_connectSocket);
	}
};
visca_error_t ViscaAPI::connectionStatus() {
	if (_connectSocket == INVALID_SOCKET) {
		return VNOSOCKET_ERR;
	}
	char buffer[1];
	ssize_t result = recv(_connectSocket, buffer, sizeof(buffer), MSG_PEEK);
	if (result == 0) {

		// The recv function returns zero if the connection has been gracefully closed.
		return VCLOSED;
	}
	else if (result == -1) {
		// An error occurred. This could mean that the connection was lost.
		// Use WSAGetLastError() to get more info about the error.
		return VCONNECT_ERR;
	}
	else {
		// If recv() didn't return 0 or -1, the socket is still connected.
		return VOK;
	}
}
visca_error_t ViscaAPI::connectCamera(std::string url, u_short port) {
	_url = url;
	_port = port;
	int result = OpenSocket(&_connectSocket, _url, _port);
	return result;
}
visca_error_t ViscaAPI::disconnectCamera() {
	if (_connectSocket != INVALID_SOCKET) {
		CloseSocket(_connectSocket);
	}
	_url = "";
	_port = -1;
	_connectSocket = INVALID_SOCKET;
	return VOK;
};

static ValueConverter getShortVC("90 50 0p 0p 0p 0p FF", 'p');
static ValueConverter getHexVC("90 50 0p FF", 'p');
static ValueConverter getPanTiltVC("90 50 0w 0w 0w 0w 0z 0z 0z 0z FF", 'w', 'z');
visca_error_t ViscaAPI::getPanTilt(visca_tuple_t& out) {
	std::string returnhex;
	int result = GetCamera(_connectSocket, "81 09 06 12 FF", &returnhex);
	out.value1 = 0;
	out.value2 = 0;
	if (result == VOK) {
		out.value1 = getPanTiltVC.getValue('w',returnhex);
		out.value2 = getPanTiltVC.getValue('z',returnhex);
	}
	return result;
};

static ValueConverter setPanTiltVC("81 01 06 02 18 18 0y 0y 0y 0y 0z 0z 0z 0z FF",'y','z');
visca_error_t ViscaAPI::setAbsolutePanTilt(visca_tuple_t pan_tilt) {
	setPanTiltVC.setValue('y', pan_tilt.value1);
	setPanTiltVC.setValue('z', pan_tilt.value2);
	int result = SetCamera(_connectSocket, setPanTiltVC.getCommand());
	return result;
};

visca_error_t ViscaAPI::getZoomLevel(short& out) {
	std::string returnhex;
	int result = GetCamera(_connectSocket, "81 09 04 47 FF", &returnhex);
	out = 0;
	if (result == VOK) {
		out = getShortVC.getValue('p', returnhex);
	}
	return result;
};

static ValueConverter setZoomVC("81 01 04 47 0p 0p 0p 0p FF", 'p');
visca_error_t ViscaAPI::setZoomLevel(short level) {
	setZoomVC.init();
	setZoomVC.setValue('p', level);
	int result = SetCamera(_connectSocket, setZoomVC.getCommand());
	return result;
};

visca_error_t ViscaAPI::getHorizontalFlip(bool& out) {
	std::string returnhex;
	int result = GetCamera(_connectSocket, "81 09 04 61 FF", &returnhex);
	out = 0;
	if (result == VOK) {
		short flip = getHexVC.getValue('p', returnhex);
		out = (flip == 2);
	}
	return result;
};

static ValueConverter setHFlipVC("81 01 04 61 0p FF", 'p');
visca_error_t ViscaAPI::setHorizontalFlip(bool flip) {
	setHFlipVC.setValue('p', flip ? 2 : 3);
	int result = SetCamera(_connectSocket, setHFlipVC.getCommand());
	return result;
};

visca_error_t ViscaAPI::getVerticalFlip(bool& out) {
	std::string returnhex;
	int result = GetCamera(_connectSocket, "81 09 04 66 FF", &returnhex);
	out = 0;
	if (result == VOK) {
		short flip = getHexVC.getValue('p', returnhex);
		out = (flip == 2);
	}
	return result;
};

static ValueConverter setVFlipVC("81 01 04 66 0p FF", 'p');
visca_error_t ViscaAPI::setVerticalFlip(bool flip) {
	setVFlipVC.setValue('p', flip ? 2 : 3);
	int result = SetCamera(_connectSocket, setVFlipVC.getCommand());
	return result;
};
