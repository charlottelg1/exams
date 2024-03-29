/*
	setup_server() creates the listening socket and initialize all variables in the server struct
	add_client() accepts and sets up the new client and announces its arrival to all others
	dispatch_msg() tries to read from the client, and sends its messages to all others. If the client left, it is removed from the client list
	extract_message()	is a function provided by the exam subject.
						It looks for a \n in the string passed as first argument.
						If there is none, it returns 0 and leaves the string untouched.
						If there is a \n, it copies whatever is after it and reassign the first arg to point to that copy.
						It then makes the second argument point to the original string and adds a \0 to it to make it stop at the first \n
						Look at how the function is used down below.
						The second arg must be freed INSIDE the while scope. The first arg must be freed only once, at the very end (its done when removing the client in my case)
						Both args must be freeable pointers
	Note :	A message is defined as a string ending with \n. If there are several \n in a string, that makes several messages.
			If there is no \n in a string, it is considered that the message is not complete.
			That string (with no \n) is saved in (t_client::buf), and only if after reading some more, a \n is found, is it then considered a complete message and sent to other clients.
			If a client sends a string without \n and then leaves, the "in waiting" string in (t_client::buf) is deleted, and never sent to the other clients.
			That can be tested with something like : echo -n "some string with no new line" | nc localhost PORT
			In the 42 exam session, use /bin/echo instead of the normal echo command.
*/

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>

typedef struct s_client
{
	int					id;
	char				id_str[25];
	int					sock; // don't forget to close it when removing client
	struct sockaddr_in	addr;
	socklen_t			addr_len;
	char				*buf; // set to NULL when client is added, and deallocated with free() when client is removed
	struct s_client		*next;
}	t_client;

typedef struct s_server
{
	int					sock;
	struct sockaddr_in	addr;
	t_client			*client_list;
	int					id_next_client;
	fd_set				save_set, read_set, write_set; // don't forget to use FD_CLR() when removing a client
}	t_server;

void	fatal()
{
	char	*str = "Fatal error\n";
	write(2, str, strlen(str));
	exit(1);
}

void	setup_server(t_server *s, int port)
{
	s->client_list = NULL;
	s->id_next_client = 0;
	FD_ZERO(&s->save_set);
		//FD_ZERO(fd_set *fdset) initializes the fd fdset to have zero bits for all file descriptors

	// socket create and verification
	s->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (s->sock == -1)
		fatal();

	// assign IP, PORT
	bzero(&s->addr, sizeof(struct sockaddr_in));
	s->addr.sin_family = AF_INET;
	s->addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	s->addr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification
	if (0 != bind(s->sock, (const struct sockaddr *)&s->addr, sizeof(struct sockaddr_in)))
		fatal();

	// Listening
	if (0 != listen(s->sock, SOMAXCONN))
		fatal();

	FD_SET(s->sock, &s->save_set);
		//FD_SET(int fd, fd_set *fdset) sets the bit for the file descriptor un fd in the file descriptor set fdset
}

void	add_client(t_server *server)
{
	// creating the client
	t_client	*c = calloc(1, sizeof(t_client));
	if (c == NULL)
		fatal();
	c->addr_len = sizeof(struct sockaddr_in);
	c->sock = accept(server->sock, (struct sockaddr *)&c->addr, &c->addr_len);
	if (c->sock < 0)
	{
		write(2, "error with accept()\n", 20);
		free(c);
		return;
	}
	FD_SET(c->sock, &server->save_set);
	c->id = server->id_next_client;
	sprintf(c->id_str, "client %d: ", c->id);
	server->id_next_client = server->id_next_client + 1;

	// adding the client to client list (add front)
	c->next = server->client_list;
	server->client_list = c;

	// announcing the new client
	char	welcome_msg[50];
	sprintf(welcome_msg, "server: client %d just arrived\n", c->id);
	t_client	*browse = c->next;
	while (browse)
	{
		if (FD_ISSET(browse->sock, &server->write_set)) // This cannot evaluate as true for the new client, as it has not gotten a chance to go through select() yet
			send(browse->sock, welcome_msg, strlen(welcome_msg), 0);
		browse = browse->next;
	}
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

void	dispatch_msg(t_client **client, t_server *server, int *skip_increment)
{
	char	read_buf[10];
	int		recv_return = recv((*client)->sock, read_buf, 9, 0);

	if (recv_return > 0)
	{
		read_buf[recv_return] = 0;
		(*client)->buf = str_join((*client)->buf, read_buf);
		char	*msg = NULL;
		char	*to_send = NULL;
		while (extract_message(&(*client)->buf, &msg))
		{
			to_send = str_join(to_send, (*client)->id_str);
			to_send = str_join(to_send, msg);
			free(msg);
		}
		t_client	*browse = server->client_list;
		while (browse && to_send)
		{
			if (browse->id != (*client)->id && FD_ISSET(browse->sock, &server->write_set))
				send(browse->sock, to_send, strlen(to_send), 0);
			browse = browse->next;
		}
		free(to_send);
	}
	//else if (recv_return == -1)
	//{
	//	write(2, "recv = -1\n", 10);
	//}
	else //if (recv_return <= 0) // if error (-1), we remove the client too
	{
		t_client	*to_delete = (*client);
		*client = to_delete->next;
		*skip_increment = 1; // ready for next loop cycle

		char	goodbye_msg[50];
		sprintf(goodbye_msg, "server: client %d just left\n", to_delete->id);
		free(to_delete->buf); // can be either NULL or something allocated
		FD_CLR(to_delete->sock, &server->save_set);
		close(to_delete->sock);

		// reconnecting the list around the deleted node
		if (to_delete == server->client_list) // deleting the first node of the list
			server->client_list = to_delete->next;
		else
		{
			t_client	*parent_of_to_delete = server->client_list;
			while (parent_of_to_delete && parent_of_to_delete->next != to_delete)
				parent_of_to_delete = parent_of_to_delete->next;
			parent_of_to_delete->next = to_delete->next;
		}

		// announcing the departure, to_delete is not in the client list at this point
		t_client	*browse = server->client_list;
		while (browse)
		{
			if (FD_ISSET(browse->sock, &server->write_set))
				send(browse->sock, goodbye_msg, strlen(goodbye_msg), 0);
			browse = browse->next;
		}
		free(to_delete);
	}
}

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		char	*str = "Wrong number of arguments\n";
		write(2, str, strlen(str));
		exit(1);
	}

	t_server	server;
	setup_server(&server, atoi(argv[1]));
	//setup_server() creates the listening socket and initialize all variables in the server struct

	while (1)
	{
		server.read_set = server.save_set;
		server.write_set = server.save_set;
		if (-1 == select(FD_SETSIZE, &server.read_set, &server.write_set, NULL, NULL))
		{
				//int seclect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
				//indicates which of the specified fd is ready for reading
			//write(2, "error with select\n", 18);
			continue;
		}
		if (FD_ISSET(server.sock, &server.read_set))
			add_client(&server); //accepts and sets up the new client and announces its arrival to all others
			//int FD_ISSET(int fd, fd_set *set) verifie si un descritpeur est contenu dans un ensemble
		t_client	*browse = server.client_list;
		while (browse)
		{
			int	skip_increment = 0;
			if (FD_ISSET(browse->sock, &server.read_set))
				dispatch_msg(&browse, &server, &skip_increment); //tries to read from the client, and sends its messages to all others. If the client left, it is removed from the client list
			if (skip_increment == 0) // if we delete the node in the list, we have to increment the pointer in dispatch_msg and skip here
				browse = browse->next;
		}
	}
	return 0;
}

/*This is a C program that implements a basic server that can accept multiple 
clients using sockets. It allows the clients to communicate with each other in 
real-time through the server.

The code contains two structs, t_server and t_client, to represent the server and 
clients respectively. t_server contains the server socket file descriptor, address, 
a list of connected clients, and some file descriptor sets used to manage connections. t_client contains the client socket file descriptor, address, ID, a buffer to store incoming data, and a pointer to the next client in the list.

The fatal() function is used to handle fatal errors by printing a message to stderr 
and exiting the program.

The setup_server() function initializes the server socket and address, binds it to a 
specified port, and starts listening for incoming connections. It also sets up the 
file descriptor sets.

The add_client() function is called whenever a new client connects to the server. 
It accepts the connection, assigns an ID to the client, adds it to the list of 
connected clients, and broadcasts a welcome message to all connected clients.

The extract_message() function extracts a complete message from a client buffer, 
which contains partial or complete messages. It returns 1 if a message was 
successfully extracted, 0 if there are no complete messages in the buffer, and -1 if 
an error occurred.

The str_join() function concatenates two strings and returns the resulting string. It 
also frees the memory allocated for the first string.

The dispatch_msg() function is called whenever a client sends a message. It reads 
the incoming data and adds it to the client buffer. It then extracts complete
 messages from the buffer and sends them to all connected clients except for the 
 sender.

The main function sets up the server, enters an infinite loop, and waits for 
incoming connections or data from clients using the select() function. If a new 
connection is detected, it calls add_client(). If there is data to be read from a 
client, it calls dispatch_msg(). If a client disconnects, it removes it from the 
list of connected clients and broadcasts a goodbye message.*/