#include "ViscaAPI.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT>" << std::endl;
        return 1; // Exit with an error code
    }

    const char* ipAddress = argv[1];
    int port = std::atoi(argv[2]);

    ViscaAPI cameraControl;
    visca_error_t result;

    // Attempt to connect to the camera
    result = cameraControl.connectCamera(ipAddress, port);
 
    if (result != VOK) {
        std::cerr << "Failed to connect to camera. Error code: " << result << std::endl;
        return 1; // Exit with an error code
    }

    // Connected successfully, now get the zoom level

    short zoomLevel = 20;
    
    result = cameraControl.setZoomLevel(zoomLevel);
    if (result != VOK) {
        std::cerr << "Failed to set zoom level. Error code: " << result << std::endl;
        return 1; // Exit with an error code
    }

    result = cameraControl.getZoomLevel(zoomLevel);
    if (result != VOK) {
        std::cerr << "Failed to get zoom level. Error code: " << result << std::endl;
        return 1; // Exit with an error code
    }

    // Print the zoom level to the console
    std::cout << "Current zoom level: " << zoomLevel << std::endl;

    // Optionally, disconnect the camera
    cameraControl.disconnectCamera();

    return 0; // Successful execution
}