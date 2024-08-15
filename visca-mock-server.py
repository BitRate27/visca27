import socket
import threading
import time

class ViscaMockServer:
    def __init__(self, host='localhost', port=5678):
        self.host = host
        self.port = port
        self.server_socket = None
        self.running = False
        self.clients = []
        self.pan_tilt = [0, 0]
        self.zoom_level = 0
        self.horizontal_flip = False
        self.vertical_flip = False

    def start(self):
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(5)
        self.running = True
        print(f"Server started on {self.host}:{self.port}")

        while self.running:
            try:
                client_socket, addr = self.server_socket.accept()
                print(f"New connection from {addr}")
                client_thread = threading.Thread(target=self.handle_client, args=(client_socket,))
                client_thread.start()
                self.clients.append(client_thread)
            except Exception as e:
                print(f"Error accepting connection: {e}")

    def stop(self):
        self.running = False
        for client in self.clients:
            client.join()
        if self.server_socket:
            self.server_socket.close()

    def handle_client(self, client_socket):
        while self.running:
            try:
                data = client_socket.recv(1024)
                if not data:
                    break
                command = data.hex().upper()
                response = self.process_command(command)
                client_socket.sendall(bytes.fromhex(response))
            except Exception as e:
                print(f"Error handling client: {e}")
                break
        client_socket.close()

    def process_command(self, command):
        print(f"Received command: {command}")
        if command.startswith('8109'):  # Inquiry commands
            if command == '81090612FF':  # Get Pan/Tilt
                return f'9050{self.pan_tilt[0]:04X}{self.pan_tilt[1]:04X}FF'
            elif command == '81090447FF':  # Get Zoom
                return f'90500{self.zoom_level:03X}FF'
            elif command == '81090461FF':  # Get Horizontal Flip
                return f'905002FF' if self.horizontal_flip else f'905003FF'
            elif command == '81090466FF':  # Get Vertical Flip
                return f'905002FF' if self.vertical_flip else f'905003FF'
        elif command.startswith('8101'):  # Set commands
            if command.startswith('81010602'):  # Set Pan/Tilt
                self.pan_tilt = [int(command[12:16], 16), int(command[16:20], 16)]
                return '904101FF'
            elif command.startswith('81010447'):  # Set Zoom
                self.zoom_level = int(command[8:12], 16)
                return '904101FF'
            elif command.startswith('81010461'):  # Set Horizontal Flip
                self.horizontal_flip = (command[8:10] == '02')
                return '904101FF'
            elif command.startswith('81010466'):  # Set Vertical Flip
                self.vertical_flip = (command[8:10] == '02')
                return '904101FF'
        
        # Unknown command
        return '904100FF'

if __name__ == "__main__":
    server = ViscaMockServer()
    try:
        server.start()
    except KeyboardInterrupt:
        print("Server stopping...")
    finally:
        server.stop()
