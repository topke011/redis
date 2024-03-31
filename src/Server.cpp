#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

const std::string PING_COMMAND = "*1\r\n$4\r\nping\r\n";
const size_t DEFAULT_BUFFER_SIZE = 1024;

int main(int argc, char **argv)
{
	// You can use print statements as follows for debugging, they'll be visible when running tests.
	std::cout << "Logs from your program will appear here!\n";

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
	 std::cerr << "Failed to create server socket\n";
	 return 1;
	}

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		std::cerr << "setsockopt failed\n";
		return 1;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(6379);

	if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0)
	{
		std::cerr << "Failed to bind to port 6379\n";
		return 1;
	}

	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0)
	{
		std::cerr << "listen failed\n";
		return 1;
	}

	struct sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	std::cout << "Waiting for a client to connect...\n";

	int client_socket = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
	if (client_socket == -1)
	{
		std::cerr << "Failed to accept client connection\n";
		return 1;
	}

    int commands_to_process = 2;
    while (commands_to_process)
    {
        commands_to_process--;
        char buffer[DEFAULT_BUFFER_SIZE] = { 0 };
        int bytes_read = read(client_socket, buffer, DEFAULT_BUFFER_SIZE);
        if (bytes_read == -1)
        {
            std::cerr << "Failed to read from client" << std::endl;
            return 1;
        } 

        std::cout << "Received: " << bytes_read << std::endl;
        std::string command = std::string(buffer, bytes_read);
        if (command == PING_COMMAND)
        {
            std::string response = "+PONG\r\n";
            int bytes_written = write(client_socket, response.c_str(), response.size());
            std::cout << "Send: " << bytes_written << std::endl;
        }
    }

	close(server_fd);

	return 0;
}
