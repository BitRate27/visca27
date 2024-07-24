#include "../ViscaAPI.h"
#include <iostream>

int main() {
    ViscaAPI cameraControl;
    visca_error_t result;

    // Attempt to connect to the camera
    result = cameraControl.connectCamera("10.0.10.24", 5678);
    if (result != VOK) {
        std::cerr << "Failed to connect to camera. Error code: " << result << std::endl;
        return 1; // Exit with an error code
    }

    // Connected successfully, now get the zoom level
    short zoomLevel = 0;
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