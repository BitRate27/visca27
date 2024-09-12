#include "ViscaAPI.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT>" << std::endl;
        return 1; // Exit with an error code
    }

    char* ipAddress = argv[1];
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

    int zoomLevel = 10;

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
    std::cout << "Current zoomLevel: " << zoomLevel << std::endl;

    visca_tuple_t pantilt = {20,30};
    
    result = cameraControl.setAbsolutePanTilt(pantilt);
    if (result != VOK) {
        std::cerr << "Failed to set pan/tilt. Error code: " << result << std::endl;
        return 1; // Exit with an error code
    }

    result = cameraControl.getPanTilt(pantilt);
    if (result != VOK) {
        std::cerr << "Failed to get pan/tilt. Error code: " << result << std::endl;
        return 1; // Exit with an error code
    }
    // Print the pantilt to the console
    std::cout << "Current pan/tilt: " << pantilt.value1 << "," << pantilt.value2 << std::endl;

    // Command loop
    int command = 0;
    while (true) {
        std::cout << "Enter command (-1 = exit, 1 = check connection, 2 = close connection, 3 = new connection): ";
        std::cin >> command;

        if (command == -1) {
            break;
        } else if (command == 1) {
            // Check connection
            result = cameraControl.connectionStatus();
            if (result == VOK) {
                std::cout << "Connection is active." << std::endl;
            } else {
                std::cerr << "Connection is not active. Error code: " << result << std::endl;
            }
        } else if (command == 2) {
            // Close connection
            cameraControl.disconnectCamera();
            std::cout << "Connection closed." << std::endl;
        } else if (command == 3) {
            // New connection
            std::cout << "Enter new IP address: ";
            std::cin >> ipAddress;
            std::cout << "Enter new port: ";
            std::cin >> port;

            result = cameraControl.connectCamera(ipAddress, port);
            if (result == VOK) {
                std::cout << "Connected to new camera successfully." << std::endl;
            } else {
                std::cerr << "Failed to connect to new camera. Error code: " << result << std::endl;
            }
        } else {
            std::cerr << "Invalid command." << std::endl;
        }
    }
    // Optionally, disconnect the camera
    cameraControl.disconnectCamera();

    return 0; // Successful execution
}
