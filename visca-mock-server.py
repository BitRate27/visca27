import socket
import threading
import time

class ViscaMockServer:
    def __init__(self, host='127.0.0.1', port=5678):
        self.host = host
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.bind((self.host, self.port))
        self.running = False
        self.clients = []
        
        # Camera state
        self.pan_tilt = [0x8000, 0x8000]  # Mid position
        self.zoom_level = 0x0000  # Wide end
        self.horizontal_flip = False
        self.vertical_flip = False

    def start(self):
        self.running = True
        self.socket.listen(5)
        print(f"Server listening on {self.host}:{self.port}")
        
        while self.running:
            client, address = self.socket.accept()
            print(f"New connection from {address}")
            self.clients.append(client)
            client_thread = threading.Thread(target=self.handle_client, args=(client,))
            client_thread.start()

    def stop(self):
        self.running = False
        for client in self.clients:
            client.close()
        self.socket.close()

    def handle_client(self, client):
        while self.running:
            try:
                data = client.recv(1024)
                if not data:
                    break
                responses = self.process_command(data)
                for response in responses:
                    print(f"Sending response: {response.hex()}")
                    client.send(response)
            except Exception as e:
                print(f"Error handling client: {e}")
                break
        
        self.clients.remove(client)
        client.close()

    def process_command(self, data):
        hex_data = data.hex()
        print(f"Received command: {hex_data}")
        
        responses = []
        if hex_data.startswith('81090612'):  # Get pan/tilt
            responses.append(bytes.fromhex(f'9050{self.pan_tilt[0]}{self.pan_tilt[1]}ff'))
        elif hex_data.startswith('8101060218'):  # Set pan/tilt
            self.pan_tilt[0] = hex_data[12:20]
            self.pan_tilt[1] = hex_data[20:28]
            responses.append(bytes.fromhex('9040ff'))  # ACK
            responses.append(bytes.fromhex('9050ff'))  # Completion
        elif hex_data.startswith('81090447'):  # Get zoom
            responses.append(bytes.fromhex(f'9050{self.zoom_level}ff'))
        elif hex_data.startswith('81010447'):  # Set zoom
            self.zoom_level = hex_data[8:16]
            responses.append(bytes.fromhex('9040ff'))  # ACK
            responses.append(bytes.fromhex('9050ff'))  # Completion
        elif hex_data.startswith('81090461'):  # Get horizontal flip
            responses.append(bytes.fromhex('905002ff' if self.horizontal_flip else '905003ff'))
        elif hex_data.startswith('81010461'):  # Set horizontal flip
            self.horizontal_flip = (hex_data[8:10] == '02')
            responses.append(bytes.fromhex('9040ff'))  # ACK
            responses.append(bytes.fromhex('9050ff'))  # Completion
        elif hex_data.startswith('81090466'):  # Get vertical flip
            responses.append(bytes.fromhex('905002ff' if self.vertical_flip else '905003ff'))
        elif hex_data.startswith('81010466'):  # Set vertical flip
            self.vertical_flip = (hex_data[8:10] == '02')
            responses.append(bytes.fromhex('9040ff'))  # ACK
            responses.append(bytes.fromhex('9050ff'))  # Completion
        else:
            responses.append(bytes.fromhex('9060ff'))  # Error response
        
        return responses
    
if __name__ == "__main__":
    server = ViscaMockServer()
    try:
        server.start()
    except KeyboardInterrupt:
        print("Shutting down server...")
        server.stop()
