#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

void clear(char *arr, int len);

int main(int argc, char const* argv[])
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char return_buff[1024];
    char cmd[128];
    int recv_val = 0;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return -1;
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))< 0) {
        printf("bind failed");
        return -1;
    }
    while (1) {
        if (listen(server_fd, 3) < 0) {
            printf("listen error");
            return -1;
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, &addrlen)) < 0) {
            printf("accept error");
            break;
        }
        valread = read(new_socket, buffer,1024 - 1);
        sscanf(buffer, " %s: %d", cmd, &recv_val);
        if (!strncmp(cmd, "Temperatuur", 11)){
            if (recv_val > 15000) {
                strncpy(return_buff, "Ventilator: aan\n", 16);
            } else {
                strncpy(return_buff, "Ventilator: uit\n", 16);
            }
        } else if (!strncmp(cmd, "CO2", 3)) {
            if (recv_val > 15) {
                strncpy(return_buff, "Ventilator: aan (CO2)\n", 21);
            } else {
                strncpy(return_buff, "Ventilator: uit (CO2)\n", 21);
            }
        }
        printf("Nieuwe bericht: \n");
        printf("%s\n", buffer);
        send(new_socket, return_buff, strlen(return_buff), 0);
        printf("Message answered\n");
        printf("%zd\n", valread);
        clear(buffer, 1024);
    }
    close(new_socket);
    close(server_fd);
    return 0;
}


void clear(char *arr, int len){
    for (int i = 0; i <= len; i++){
        arr[i] = '\0';
    }
}
