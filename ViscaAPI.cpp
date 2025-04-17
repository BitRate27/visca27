#include "visca27.h"
#include "ViscaAPI.h"
#include <iostream>

ViscaAPI::ViscaAPI() {
	_url = "";
	_port = 0;
	_sequence = 0;
	_encapsulated = false;
	_udp = false;
	_connectSocket = INVALID_SOCKET;
	_result = VCONNECT_ERR;
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

	return _result;
	/*
	char buffer[1];
	ssize_t result = recv(_connectSocket, buffer, sizeof(buffer), MSG_PEEK);
	if (result == 0) {

		// The recv function returns zero if the connection has been gracefully closed.
		return VCLOSED;
	}
	else if (result == -1) {
#if defined(_WIN32) || defined(_WIN64)
    	if ((result < 0) && (WSAGetLastError() == WSAEWOULDBLOCK)) {
#else
   		if ((result < 0) && (errno == EINPROGRESS)) {
#endif
            // The socket is non-blocking and there's no data to read.
            return VOK;
        } else {
            // An error occurred. This could mean that the connection was lost.
            return VCONNECT_ERR;
        }
	}
	else {
		// If recv() didn't return 0 or -1, the socket is still connected.
		return VOK;
	}
	*/
}
visca_error_t ViscaAPI::connectCamera(std::string url, int port, bool udp, bool encapsulated) {
	if (_result == VOK) {
		disconnectCamera();
	}
	_url = url;
	_port = port;
	_sequence = 0;
	_encapsulated = encapsulated;
	_udp = udp;
	_result = OpenSocket(&_connectSocket, _url, _port, _sequence, _udp);
	return _result;
}
visca_error_t ViscaAPI::reconnectCamera()
{
	if (_connectSocket != INVALID_SOCKET) {
		CloseSocket(_connectSocket);
	}
	_result = OpenSocket(&_connectSocket, _url, _port, _sequence, _udp);
	return _result;
};
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
	_result = GetCamera(_connectSocket, "81 09 06 12 FF", &returnhex, _sequence, _encapsulated);
	out.value1 = 0;
	out.value2 = 0;
	if (_result == VOK) {
		out.value1 = getPanTiltVC.getValue('w',returnhex);
		out.value2 = getPanTiltVC.getValue('z',returnhex);
	}
	return _result;
};

static ValueConverter setPanTiltVC("81 01 06 02 18 18 0y 0y 0y 0y 0z 0z 0z 0z FF",'y','z');
visca_error_t ViscaAPI::setAbsolutePanTilt(visca_tuple_t pan_tilt) {
	setPanTiltVC.setValue('y', pan_tilt.value1);
	setPanTiltVC.setValue('z', pan_tilt.value2);
	_result = SetCamera(_connectSocket, setPanTiltVC.getCommand(),
			       _sequence, _encapsulated);
	return _result;
};

visca_error_t ViscaAPI::getZoomLevel(int& out) {
	std::string returnhex;
	_result = GetCamera(_connectSocket, "81 09 04 47 FF", &returnhex,
			       _sequence, _encapsulated);
	out = 0;
	if (_result == VOK) {
		out = getShortVC.getValue('p', returnhex);
	}
	return _result;
};

static ValueConverter setZoomVC("81 01 04 47 0p 0p 0p 0p FF", 'p');
visca_error_t ViscaAPI::setZoomLevel(int level) {
	setZoomVC.init();
	setZoomVC.setValue('p', level);
	_result = SetCamera(_connectSocket, setZoomVC.getCommand(),
			       _sequence, _encapsulated);
	return _result;
};

visca_error_t ViscaAPI::getHorizontalFlip(bool& out) {
	std::string returnhex;
	_result = GetCamera(_connectSocket, "81 09 04 61 FF", &returnhex,
			       _sequence, _encapsulated);
	out = 0;
	if (_result == VOK) {
		int flip = getHexVC.getValue('p', returnhex);
		out = (flip == 2);
	}
	return _result;
};

static ValueConverter setHFlipVC("81 01 04 61 0p FF", 'p');
visca_error_t ViscaAPI::setHorizontalFlip(bool flip) {
	setHFlipVC.setValue('p', flip ? 2 : 3);
	_result = SetCamera(_connectSocket, setHFlipVC.getCommand(),
			       _sequence, _encapsulated);
	return _result;
};

visca_error_t ViscaAPI::getVerticalFlip(bool& out) {
	std::string returnhex;
	_result = GetCamera(_connectSocket, "81 09 04 66 FF", &returnhex,
			       _sequence, _encapsulated);
	out = 0;
	if (_result == VOK) {
		int flip = getHexVC.getValue('p', returnhex);
		out = (flip == 2);
	}
	return _result;
};

static ValueConverter setVFlipVC("81 01 04 66 0p FF", 'p');
visca_error_t ViscaAPI::setVerticalFlip(bool flip) {
	setVFlipVC.setValue('p', flip ? 2 : 3);
	_result = SetCamera(_connectSocket, setVFlipVC.getCommand(),
			       _sequence, _encapsulated);
	return _result;
};

static ValueConverter recallPresetVC("81 01 04 3f 02 pp FF", 'p');
visca_error_t ViscaAPI::recallPreset(int preset) {
	recallPresetVC.setValue('p', preset);
	_result = SetCamera(_connectSocket, recallPresetVC.getCommand(),
			       _sequence, _encapsulated);
	return _result;
};

static ValueConverter setPresetVC("81 01 04 3f 01 pp FF", 'p');
visca_error_t ViscaAPI::setPreset(int preset) {
	setPresetVC.setValue('p', preset);
	_result = SetCamera(_connectSocket, setPresetVC.getCommand(),
			       _sequence, _encapsulated);
	return _result;
};
