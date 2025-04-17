#pragma once
#include "visca27.h"
#include <string>
#define VOK 0
#define VERR -1
#define VCANTEXEC_ERR 1
#define VSYNTAX_ERR 2
#define VBUFFERFULL_ERR 3
#define VCANCEL_ERR 4
#define VNOSOCKET_ERR 5
#define VCONNECT_ERR 6
#define VTIMEOUT_ERR 7
#define VCLOSE_ERR 8
#define VDISCONNECT_ERR 9
#define VIP_ERR 10
#define VCLOSED 11
#define VSELECT_ERR 12
#define VSOCKET_ERR 13
#define VPORT_ERR 14

typedef struct {
	int value1;
	int value2;
} visca_tuple_t;
typedef int visca_error_t;
typedef SOCKET visca_socket_t;

class ViscaAPI
{
public:
	ViscaAPI();
	~ViscaAPI();
	visca_error_t connectCamera(std::string url, int port, bool udp = false, bool encapsulated = false);
	visca_error_t reconnectCamera();
	visca_error_t connectionStatus();
	visca_error_t getPanTilt(visca_tuple_t& out);
	visca_error_t setAbsolutePanTilt(visca_tuple_t pan_tilt);
	visca_error_t getZoomLevel(int& out);
	visca_error_t setZoomLevel(int level);
	visca_error_t getHorizontalFlip(bool& out);
	visca_error_t setHorizontalFlip(bool flip);
	visca_error_t getVerticalFlip(bool& out);
	visca_error_t setVerticalFlip(bool flip);
	visca_error_t recallPreset(int preset);
	visca_error_t setPreset(int preset);
	visca_error_t disconnectCamera();

private:
	std::string _url;
	int _port;
	int32_t _sequence;
	bool _udp;
	bool _encapsulated;
	visca_error_t _result;
	visca_socket_t _connectSocket;
};

