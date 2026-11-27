/*
 ============================================================================
 Name        : serverTCP.c
 Author      : G22 - Paparusso Pietro, Monticelli Riccardo
 Description : Server TCP - Esonero Reti di Calcolatori 2026-27
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROTOPORT 27015   // numero di porta di default
#define QLEN 6            // dimensione della coda delle richieste
#define BUFFERSIZE 512

// i tre interi inviati dal client
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

int main(int argc, char *argv[]) {
	// senza questa riga la console di Eclipse mostra le printf in ritardo
	setvbuf(stdout, NULL, _IONBF, 0);

	int port;
	if (argc > 1) {
		port = atoi(argv[1]); // porta passata come argomento
	} else {
		port = PROTOPORT; // porta di default
	}
	if (port < 0) {
		printf("bad port number %s \n", argv[1]);
		return 0;
	}

#if defined WIN32 // inizializzazione di Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		ErrorHandler("Error at WSAStartup()\n");
		return 0;
	}
#endif

	// CREAZIONE DELLA SOCKET
	int MySocket;
	MySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (MySocket < 0) {
		ErrorHandler("socket creation failed.\n");
		ClearWinSock();
		return -1;
	}

	// ASSEGNAZIONE DI UN INDIRIZZO ALLA SOCKET
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad)); // i byte non usati vengono messi a 0
	sad.sin_family = AF_INET;
	// 0.0.0.0 = tutte le interfacce di rete del PC: il server accetta client
	// sia dallo stesso PC (127.0.0.1) sia da altri PC (es. 192.168.1.x)
	sad.sin_addr.s_addr = inet_addr("0.0.0.0");
	sad.sin_port = htons(port);
	if (bind(MySocket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		ErrorHandler("bind() failed.\n");
		closesocket(MySocket);
		ClearWinSock();
		return -1;
	}

	// SETTAGGIO DELLA SOCKET ALL'ASCOLTO
	if (listen(MySocket, QLEN) < 0) {
		ErrorHandler("listen() failed.\n");
		closesocket(MySocket);
		ClearWinSock();
		return -1;
	}

	// ACCETTARE UNA NUOVA CONNESSIONE
	struct sockaddr_in cad; // indirizzo del client
	int clientSocket;       // socket dedicata al client
	int clientLen;          // dimensione dell'indirizzo del client
	char buf[BUFFERSIZE];

	while (1) { // il server resta sempre in ascolto
		printf("In attesa di un client sulla porta %d...\n", port);
		clientLen = sizeof(cad);
		if ((clientSocket = accept(MySocket, (struct sockaddr*) &cad, &clientLen)) < 0) {
			ErrorHandler("accept() failed.\n");
			continue; // un errore su un client non deve fermare il server
		}
		printf("connesso con il client il cui nome IP: %s\n", inet_ntoa(cad.sin_addr));

		// INVIO DELLA STRINGA "connessione avvenuta"
		char *benvenuto = "connessione avvenuta";
		if (send(clientSocket, benvenuto, strlen(benvenuto), 0) != (int) strlen(benvenuto)) {
			ErrorHandler("send() sent a different number of bytes than expected\n");
			closesocket(clientSocket);
			continue;
		}

		// DIALOGO CON IL CLIENT
		while (1) {
			struct numeri num;
			if (recv(clientSocket, (char*) &num, sizeof(num), 0) <= 0) {
				printf("Il client ha chiuso la connessione.\n");
				break;
			}

			// i numeri arrivano in formato di rete (big endian)
			int n1 = ntohl(num.n1);
			int n2 = ntohl(num.n2);
			int n3 = ntohl(num.n3);
			int res = n1 + n2 - n3;
			printf("la somma di %d+%d-%d=%d\n", n1, n2, n3, res);

			if (res == 100) {
				strcpy(buf, "QUIT");
			} else {
				sprintf(buf, "%d", res); // il risultato viene inviato come stringa
			}
			if (send(clientSocket, buf, strlen(buf), 0) != (int) strlen(buf)) {
				ErrorHandler("send() sent a different number of bytes than expected\n");
				break;
			}
			if (res == 100) {
				break; // dopo QUIT il client chiude la connessione
			}
		}

		// CHIUSURA DELLA CONNESSIONE CON IL CLIENT
		closesocket(clientSocket);
	}

	// non si arriva mai qui: il server resta sempre in ascolto
	closesocket(MySocket);
	ClearWinSock();
	return 0;
}
