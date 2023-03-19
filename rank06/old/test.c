#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

typedef struct	s_client {
	int sock;
	char *buf;
	struct sockaddr_in addr;
	socklen_t	addr_len;
	struct s_client	*next;
	int	id;
	char id_str[25];
}	t_client;

typedef struct t_server {
	int	sock;
	struct sockaddr_in addr;
	int id_next_client;
	struct s_client *client_list;
	fd_set save_set, write_set, read_set;
}	t_server;

void	fatal()
{
	char *str = "Fatal error\n";
	write(2, str, strlen(str));
	exit(1);
}

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void	setup_server(t_server *server, int port)
{
	server->client_list = NULL;
	server->id_next_client = 0;
	FD_ZERO(&server->save_set);

	server->sock = socket(AF_INET, SOCK_STREAM, 0); 
	if (server->sock == -1)
		fatal();
	bzero(&server->addr, sizeof(server->addr)); 

	server->addr.sin_family = AF_INET; 
	server->addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	server->addr.sin_port = htons(port); 

	if ((bind(server->sock, (const struct sockaddr *)&server->addr, sizeof(server->addr))) != 0)
		fatal();
	if (listen(server->sock, SOMAXCONN) != 0)
		fatal();

	FD_SET(server->sock, &server->save_set);
}

void	add_client(t_server *server)
{
	t_client *c = calloc(1, sizeof(t_client));
	if (c == NULL)
		fatal();
	c->addr_len = sizeof(struct sockaddr_in);
	c->sock = accept(server->sock, (struct sockaddr *)&c->addr, &c->addr_len);
	if (c->sock < 0) 
	{
		char *str = "error with accept\n";
		write(2, str, strlen(str));
		free(c);
		return;
	}
	FD_SET(c->sock, &server->save_set);

	c->id = server->id_next_client;
	server->id_next_client += 1;
	c->next = server->client_list;
	server->client_list = c;

	sprintf(c->id_str, "client %d: ", c->id);

	char welcome[50];
	sprintf(welcome, "server: client %d just arrived\n", c->id);
	t_client *browse = c->next;
	while (browse)
	{
		if (FD_ISSET(browse->sock, &server->write_set))
			send(browse->sock, welcome, strlen(welcome), 0);
		browse = browse->next;
	}
}

void	dispatch_msg(t_client **client, t_server *server, int *skip)
{
	char read_buf[10];
	int recv_return = recv((*client)->sock, read_buf, 9,0);
	if (recv_return > 0)
	{
		read_buf[recv_return] = 0;
		(*client)->buf = str_join((*client)->buf, read_buf);
		char *msg = NULL;
		char *to_send = NULL;
		while (extract_message(&(*client)->buf, &msg))
		{
			to_send = str_join(to_send, (*client)->id_str);
			to_send = str_join(to_send, msg);
			free(msg);
		}
		t_client *browse = server->client_list;
		while (browse && to_send)
		{
			if (browse->id != (*client)->id && FD_ISSET(browse->sock, &server->write_set))
				send(browse->sock, to_send, strlen(to_send), 0);
			browse = browse->next;
		}
		free(to_send);
	}
	else 
	{
		t_client *to_delete = (*client);
		*client = to_delete->next;
		*skip = 1;
		char bye[50];
		sprintf(bye, "server: client %d just left\n", to_delete->id);
		free(to_delete->buf);
		FD_CLR(to_delete->sock, &server->save_set);
		close(to_delete->sock);

		if (to_delete == server->client_list)
			server->client_list = to_delete->next;
		else {
			t_client *parent = server->client_list;
			while (parent && parent->next != to_delete->next)
				parent = parent->next;
			parent->next = to_delete->next;
		}

		t_client *browse = server->client_list;
		while (browse)
		{
			if (FD_ISSET(browse->sock, &server->write_set))
				send(browse->sock, bye, strlen(bye), 0);
			browse = browse->next;
		}
		free(to_delete);
	}
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		char *str = "Wrong number of arguments\n";
		write(2, str, strlen(str));
		exit(1);
	}

	t_server	server;
	setup_server(&server, atoi(av[1]));

	while (1)
	{
		server.write_set = server.save_set;
		server.read_set = server.save_set;
		if (-1 == select(FD_SETSIZE, &server.read_set, &server.write_set, NULL, NULL))
			continue;
		if (FD_ISSET(server.sock, &server.read_set))
			add_client(&server);
		t_client	*browse = server.client_list;
		while (browse)
		{
			int skip = 0;
			if (FD_ISSET(browse->sock, &server.read_set))
				dispatch_msg(&browse, &server, &skip);
			if (skip == 0)
				browse = browse->next;
		}
	}
	return (0);
}