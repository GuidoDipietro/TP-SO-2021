#include "../include/sockets.h"

// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto) {
	int socket_servidor;
    struct addrinfo hints, *servinfo;

    // Inicializando hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe los addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    // Itera por cada addrinfo devuelto
    for (struct addrinfo *p = servinfo; p!=NULL; p = p->ai_next) {
        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_servidor == -1) // fallo de crear socket
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
        	// Si entra aca fallo el bind
            close(socket_servidor);
            continue;
        }
        // Ni bien conecta uno nos vamos del for
        break;
    }

	listen(socket_servidor, SOMAXCONN); // Escuchando (hasta SOMAXCONN conexiones simultaneas)

	// Aviso por pantalla...
    char msg[80];
    sprintf(msg, "Escuchando en %s:%s (%s)\n", ip, puerto, name);
    log_trace(logger, msg);

    freeaddrinfo(servinfo); //free

    return socket_servidor;
}

// ESPERAR CONEXION DE CLIENTE EN UN SERVER ABIERTO
int esperar_cliente(t_log* logger, const char* name, int socket_servidor) {
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	char msg[80];
	sprintf(msg, "Cliente conectado (a %s)\n", name);
	log_info(logger, msg);

	return socket_cliente;
}

// CLIENTE SE INTENTA CONECTAR A SERVER ESCUCHANDO EN IP:PUERTO
int crear_conexion(t_log* logger, const char* server_name, char* ip, char* puerto) {
	struct addrinfo hints, *servinfo;

	// Init de hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// Recibe addrinfo
	getaddrinfo(ip, puerto, &hints, &servinfo);

	// Crea un socket con la informacion recibida (del primero, suficiente)
	int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		// Si entra aca fallo la conexion
		char msg[80];
		sprintf(msg, "Error al conectar (a %s)\n", server_name);
		log_error(logger, msg);
	}
	else {
		char msg[80];
		sprintf(msg, "Cliente conectado en %s:%s (a %s)\n", ip, puerto, server_name);
		log_info(logger, msg);
	}

	freeaddrinfo(servinfo); //free

	return socket_cliente;
}

// CERRAR CONEXION
void liberar_conexion(int* socket_cliente) {
	close(*socket_cliente);
    *socket_cliente = -1;
}
