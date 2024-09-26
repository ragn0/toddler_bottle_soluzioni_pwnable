#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#define HOST "pwnable.kr"
#define PORT 9007
#define BUFFER_SZ 65536 

void find_counterfeit(int sock, int N, int C, int counter){
	int left = 0, right = N - 1;
    while (C > 0 && left < right){
	int middle = (left + right + 1) / 2;
	char payload[BUFFER_SZ];
	char response[1024];
	for(int i = 0, j = left; j < middle; j++){
		int len = snprintf(&payload[i], sizeof(payload), "%d ", j); 
		if (len < 0 || len >= (int)(sizeof(payload) - i)){
			break;
		}
		i += len;
		if ((j + 1) == middle){
			payload[i - 1] = '\n';
		}
	}
	printf("%s\n", payload);
	if (send(sock, payload, strlen(payload), 0) < 0){
		perror("Noooo dio diavolo stava a gi cossi bene send.");
		close(sock);
		exit(1);
	}
	if (recv(sock, response, sizeof(response), 0) < 0){
		perror("Noooo dio diavolo stava a gi cossi bene. recv");
		close(sock);
		exit(1);
	}
	int weight = atoi(response);
	printf("%s", response);
	if (weight % 2) {
		right = middle - 1;
	}
	else {
	  	left = middle;
	}
	C--;
	}

	while (C >  0){
		int n = random() % N;
		int track = 0;
		char temp[1025];
		snprintf(temp, sizeof(temp), "%d\n", n);
	 	printf("%s", temp);			
		send(sock, temp, strlen(temp), 0);
		recv(sock, temp, sizeof(temp), 0);
	 	printf("%s", temp);			

		C--;
	}
	printf("%d\n", left);
	if (left == right && C == 0){
		char answer[16];
		char res[50];
		snprintf(answer, sizeof(answer), "%d\n", left);
		send(sock, answer, strlen(answer) , 0);
		recv(sock, res, sizeof(res) , 0);
		if (strncmp(res, "Correct!", 8) == 0){
			printf("Tuttappoooooooo\n");
		}
		else {
			printf("Non tuttappoo");
			close(sock);
			exit(1);

		}
	}
	printf("Passata aa challenge nummero: %d\n", counter);


}
// 						!!!ATTENZIONE!!!
// Questa é la versione fatta senza loggare nel server di pwnable, che perció non utilizza localhost, ma si connette da remoto, questa versione é
// dannatamente piú lenta rispetto a una versione fatta localmente nel server, perció potrebbe non funzionare dato che la connessione viene 
// interrotta dopo un minuto, perció il tempo potrebbe non bastare per far risolvere al programma tutte le 100 sfide, in quel caso basta loggare
// con ssh ad una qualsiasi delle altre sfide e copiare il programma in una cartella temp, inoltre siccome non saremo piú da remoto occorrerȧ
// cambiare l'indirizzo del socket address, potremo eliminare hints e res e anziché usare getaddrinfo ci basterá usare la funzione inet_pton cosí
// if (inet_pton(AF_INET, "127.0.0.1", &indirizzo.sin_addr) <= 0) {
//        	perror("Indirizzo invalido");
//		close(s);
//       	exit(1);
//    }
// Flag: b1NaRy_S34rch1nG_1s_3asy_p3asy
//

int main(){
	int s;
	struct sockaddr_in indirizzo;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;

	int status = getaddrinfo(HOST, NULL, &hints, &res);
	if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }
	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	memset(&indirizzo, 0, sizeof(struct sockaddr_in));	
	indirizzo.sin_family = AF_INET;
	indirizzo.sin_port = htons(PORT);
	indirizzo.sin_addr = addr->sin_addr;

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Dio socket");
		return 1;
	}

	if (connect(s, (const struct sockaddr *)&indirizzo, sizeof(indirizzo)) < 0){
		perror("Dio connect");
		return 1;
	}
	int bytes_received = 0;
	char buffer[BUFFER_SZ];
	while ((bytes_received = recv(s, buffer, sizeof(buffer), 0)) > 0) {
		buffer[bytes_received] = '\0';
		printf("Ricevuto: %s", buffer);

		if (strstr(buffer, "- Ready? starting in 3 sec... -") != NULL){
			break;
		}
	}
	int counter = 1;
	while ((bytes_received = recv(s, buffer, sizeof(buffer), 0)) > 0) {
		buffer[bytes_received] = '\0';
		printf("Ricevuto: %s", buffer);
		int N, C;
		if (sscanf(buffer, "N=%d C=%d", &N, &C) == 2){
		find_counterfeit(s, N, C, counter);
		counter++;
		}
		
	}
	
	
}
