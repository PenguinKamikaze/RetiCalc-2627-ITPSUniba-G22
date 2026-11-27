/*
 ============================================================================
 Name        : clientTCP.c
 Author      : G22 - Paparusso Pietro, Monticelli Riccardo
 Description : Client TCP - Esonero Reti di Calcolatori 2026-27
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> // per gethostbyname()
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 512

// i tre interi da inviare al server
struct numeri {
	int n1;
	int n2;
	int n3;
};

void ErrorHandler(char *errorMessage) {
	printf("%s", errorMessage);
}

void ClearWinSock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(void) {
	// senza questa riga la console di Eclipse mostra le printf in ritardo
	setvbuf(stdout, NULL, _IONBF, 0);

#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("error at WSAStartup\n");
		return -1;
	}
#endif

	// LETTURA DA TASTIERA DI IP E PORTA DEL SERVER
	char ip[BUFFERSIZE];
	int port;
	printf("Inserisci l'indirizzo IP (o il nome, es. localhost) del server: ");
	scanf("%s", ip);
	printf("Inserisci il numero di porta del server: ");
	scanf("%d", &port);

	// CREAZIONE DELLA SOCKET
	int Csocket;
	Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Csocket < 0) {
		ErrorHandler("socket creation failed.\n");
		ClearWinSock();
		return -1;
	}

	// COSTRUZIONE DELL'INDIRIZZO DEL SERVER
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr(ip); // IP del server (es. "127.0.0.1")
	if (sad.sin_addr.s_addr == INADDR_NONE) {
		// non e' un IP in notazione puntata: e' un nome (es. "localhost"),
		// lo si traduce in indirizzo IP con il DNS
		struct hostent *host = gethostbyname(ip);
		if (host == NULL) {
			ErrorHandler("gethostbyname() failed.\n");
			closesocket(Csocket);
			ClearWinSock();
			return -1;
		}
		struct in_addr *ina = (struct in_addr*) host->h_addr_list[0];
		sad.sin_addr = *ina;
	}
	sad.sin_port = htons(port); // porta del server

	// CONNESSIONE AL SERVER
	if (connect(Csocket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		ErrorHandler("Failed to connect.\n");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}

	// RICEZIONE DELLA STRINGA "connessione avvenuta"
	char buf[BUFFERSIZE];
	int bytesRcvd;
	if ((bytesRcvd = recv(Csocket, buf, BUFFERSIZE - 1, 0)) <= 0) {
		ErrorHandler("recv() failed or connection closed prematurely\n");
		closesocket(Csocket);
		ClearWinSock();
		return -1;
	}
	buf[bytesRcvd] = '\0'; // \0 per far capire a printf dove finisce la stringa
	printf("%s\n", buf);

	while (1) {
		// LETTURA DEI TRE INTERI
		int n1, n2, n3;
		printf("Inserisci tre interi (n1 n2 n3): ");
		int letti = scanf("%d %d %d", &n1, &n2, &n3);
		if (letti == EOF) {
			break; // input terminato
		}
		if (letti != 3) {
			printf("Input non valido, inserisci tre numeri interi.\n");
			int c;
			while ((c = getchar()) != '\n' && c != EOF) {
				// svuota il resto della riga sbagliata
			}
			continue;
		}

		// INVIO DEI NUMERI AL SERVER (in formato di rete)
		struct numeri num;
		num.n1 = htonl(n1);
		num.n2 = htonl(n2);
		num.n3 = htonl(n3);
		if (send(Csocket, (char*) &num, sizeof(num), 0) != sizeof(num)) {
			ErrorHandler("send() sent a different number of bytes than expected\n");
			break;
		}

		// RICEZIONE DELLA RISPOSTA
		if ((bytesRcvd = recv(Csocket, buf, BUFFERSIZE - 1, 0)) <= 0) {
			ErrorHandler("recv() failed or connection closed prematurely\n");
			break;
		}
		buf[bytesRcvd] = '\0';
		printf("Ricevuto dal server: %s\n", buf);

		if (strcmp(buf, "QUIT") == 0) {
			break; // il server ha inviato QUIT: si chiude
		}
		printf("Risultato: %s\n", buf);
	}

	// CHIUSURA DELLA CONNESSIONE
	closesocket(Csocket);
	ClearWinSock();
	return 0;
}
