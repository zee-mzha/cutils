#include "tcpServer.h"

CUTILS_DEF_DYNARRAY_C(cutilsTcpServerClient, cutilsTcpServerClientArr);

int cutilsTcpServerInit(cutilsTcpServer *server, const char *service, int backlog){
	server->backlog = backlog;
	server->sockfd = -1;
	int err = cutilsTcpServerClientArrInit(&server->clients, 0);

	int yes = 1;

	struct addrinfo *res, hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, service, &hints, &res) != 0){
		return CUTILS_GETADDRINFO;
	}

	struct addrinfo *p = res;
	while(p != NULL){
		if((server->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) != -1){
			if(setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != -1){
				if(bind(server->sockfd, p->ai_addr, p->ai_addrlen) == -1){
					break;
				}
				else{
					close(server->sockfd);
					server->sockfd = -1;
				}
			}
		}
		p = p->ai_next;
	}

	if(server->sockfd == -1){
		return CUTILS_SOCKET;
	}

	freeaddrinfo(res);

	if(listen(server->sockfd, server->backlog) == -1){
		return CUTILS_LISTEN;
	}

	return CUTILS_OK;
}

void cutilsTcpServerFree(cutilsTcpServer *server){
	for(size_t i = 0; i < server->clients.size; i++){
		close(server->clients.data[i].sockfd);
		cutilsStringFree(&server->clients.data[i].address);
		cutilsByteStreamFree(&server->clients.data[i].buffer);
	}
	cutilsTcpServerClientArrFree(&server->clients);

	close(server->sockfd);
	server->sockfd = -1;
}