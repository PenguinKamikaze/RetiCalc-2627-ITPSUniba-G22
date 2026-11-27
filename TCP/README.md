# Esonero Reti di Calcolatori 2026-27 – Socket TCP

**Gruppo G22:** Paparusso Pietro, Monticelli Riccardo

**Sistema operativo in uso:** Windows, Eclipse IDE for C/C++ Developers con compilatore MinGW GCC

## Contenuto

- `serverTCP_G22` – progetto Eclipse del server
- `clientTCP_G22` – progetto Eclipse del client

Il codice è portabile: su Windows usa Winsock, su Linux/macOS le socket BSD.

## Protocollo

1. Il server viene avviato su una porta a scelta (default 27015) e resta sempre in ascolto.
2. Il client legge da tastiera l'IP e la porta del server.
3. Il client si connette al server.
4. Il server stampa `connesso con il client il cui nome IP: <ip>`.
5. Il server invia la stringa `connessione avvenuta`.
6. Il client la riceve e la stampa.
7. Il client legge tre interi n1 n2 n3 e li invia al server.
8. Il server stampa `la somma di n1+n2-n3=res`.
9. Se res = 100 il server invia `QUIT`, altrimenti invia res.
10. Il client stampa la stringa ricevuta: se è `QUIT` chiude la connessione e termina,
    altrimenti stampa il risultato e torna al punto 7.
11. Il server rimane in ascolto.

## Come eseguire

1. In Eclipse: File → Import → General → Existing Projects into Workspace, selezionare la cartella `TCP`.
2. Compilare i due progetti (Project → Build All).
3. Avviare prima `serverTCP_G22`, poi `clientTCP_G22`.
4. Nel client inserire l'IP del server e la porta `27015`:
   `127.0.0.1` o `localhost` se il server è sullo stesso PC, altrimenti l'IP (o il nome) del PC
   del server (comando `ipconfig`). I nomi vengono tradotti in IP con `gethostbyname()`.
   Il server accetta connessioni su tutte le interfacce di rete (`0.0.0.0`).
