#pragma once
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

typedef struct {
	int value1;
	int value2;
} visca_tuple_t;
typedef int visca_error_t;
typedef unsigned __int64 visca_socket_t;

class ViscaAPI
{
public:
	ViscaAPI();
	~ViscaAPI();
	visca_error_t connectCamera(std::string url, int port);
	visca_error_t getPanTilt(visca_tuple_t& out);
	visca_error_t setAbsolutePanTilt(visca_tuple_t pan_tilt);
	visca_error_t disconnectCamera();
private:
	std::string _url;
	int _port;
	visca_socket_t _connectSocket;
};

