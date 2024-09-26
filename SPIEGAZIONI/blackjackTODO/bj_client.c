#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define HOST "pwnable.kr"
#define PORT 9009
#define BUFFER_SZ 32769


int main(){
	int sock;
	struct sockaddr_in s_addr;
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));	
	hints.ai_family = AF_INET;
	hints.ai_flags = 0;

	if (getaddrinfo(HOST, NULL, &hints, &res) != 0){
		perror("Errore getaddrinfo dio boia");
		return 1;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	memset(&s_addr, 0, sizeof(struct sockaddr_in));
	s_addr.sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if (connect(sock, (const struct sockaddr *)&s_addr, sizeof(s_addr)) < 0){
		perror("Errore connect dio boia");
		return 1;
	}
	char buffer[BUFFER_SZ];
	int bytes = 0;
	while((bytes = recv(sock, buffer, sizeof(buffer), 0)) > 0){
		buffer[bytes] = '\0';
		printf("Ricevuto: %s\n", buffer);
		
		if (strstr(buffer, "(Y/N)")){
		 	char ans[3] = "Y\n\0";	
			send(sock, ans, strlen(ans), 0);
		}
		else if (strstr(buffer, "Choice:")){
			char ans[3] = "1\n\0";
			send(sock, ans, strlen(ans), 0);
			break;
		}
	}
	while ((bytes = recv(sock, buffer, sizeof(buffer), 0)) > 0){
		buffer[bytes] = '\0';
		char card;
		int cash, player, dealer;

		if (strstr(buffer, "Cash: $")){
			sscanf(buffer, "%*[^0123456789]%d", &cash);
			if (cash >= 1000000){
				printf("%s\n", buffer);
				close(sock);
				exit(1);
			}
		}
		else if (strstr(buffer, "Your Total is ")){
			sscanf("%*[^AJQK0123456789]%c%*[^0123456789]%d%*[^0123456789]%d", &card, &player, &dealer);
		}
		if (strstr(buffer, "Enter Bet: ")){
			char bet[50] = "10000000\n";
			send(sock, bet, strlen(bet), 0);
			}
		else if (strstr(buffer, "Would You Like To Play Again?")){
			char play[2] = "Y\n";
			send(sock, play, 2, 0);
			}
		else if (strstr(buffer, "Would You Like to Hit or Stay?")){
			if (player <= 11 || (player <= 16 && dealer >= 7)){
				char play[2] = "H\n";
				send(sock, play, 2, 0);
			}
			else {
				char play[2] = "S\n";
				send(sock, play, 2, 0);
			}
			}
				
		}

	}

