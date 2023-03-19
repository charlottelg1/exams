#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>

typedef struct s_client {
	int					id;
	char				id_str[25]; 
	int					sock; //represents the clients socket 
	// (endpoint for sending and receiving data over network)
	struct sockaddr_in	addr; //holds the client IP address 
	//and port number
	socklen_t			addr_len;
	char 				*buf; //data received from the client
	struct s_client		*next;
}	t_client;

typedef struct s_server {
	int 				sock; //server socket fd
	struct sockaddr_in	addr; // servers's internet address
	t_client			*client_list; //connected clients
	int					id_next_client; 
	fd_set 				save_set, read_set, write_set;
	// fd set to keep track of the fd monitored, fd the server 
	// is waiting to read from, fd the server is waiting to write to
}	t_server;

void	fatal()
{
	char *str = "Fatal error\n";
	write(2, str, strlen(str));
	exit(1);
}

void	setup_server(t_server *s, int port)
{
	s->client_list = NULL;
	s->id_next_client = 0;
	FD_ZERO(&s->save_set);
	// initialize the fd save set as empty
	
	s->sock = socket(AF_INET, SOCK_STREAM, 0);
	// creates a new socket with AF_INET family, SOCK_STREAM type (TCP) 
	// and default protocol
	if (s->sock == -1) // checks if the socket creation failed
		fatal();
	bzero(&s->addr, sizeof(struct sockaddr_in));
	// sets all the bytes of thhe server address structure to zero
	s->addr.sin_family - AF_INET;
	// sets the addresse family of the server as IPv4
	s->addr.sin_addr.s_addr = htonl(2130706433);
	// sets the IP addresse to 127.0.0.1 
	// echo $((127*256*256*256+1)) 
	s->addr.sin_port= htons(port);
	// sets the port number of the server to the value past as argument,
	// converted from host byte to network byte

	if (0 != bind(s->sock, (const struct sockaddr *)&s->addr, sizeof(struct sockaddr_in)))
		fatal();
	// binds the socket to the address and port specified in the s->addr structure
	if (0!= listen(s->sock, SOMAXCONN))
		fatal();
	// puts the socket in listening mode, allowing it to accept incoming connections
	// taht can be queue for the socket 
	FD_SET(s->sock, &s->save_set);
	// adds the socket to the save_set fd_set, indicating that the server schould 
	// check for incoming connections on this socket 
}

void	add_client(t_server *server)
{
	t_client	*c = calloc(1, sizeof(t_client));
	if (c == NULL)
		fatal();
	c->addr_len = sizeof(struct sockaddr_in);
	c->sock = accept(server->sock, (struct sockaddr *)&c->addr, &c->addr_len);
	// accept the incoming connection on the server's socket and fill the addr field
	// of the t_client struct with the client's address information
	// the newly created client socket fd is stored in sock of t_client 
	if (c->sock < 0)
	{ 
		write(2, "error with accept()\n", 20);
		free(c);
		return;
	}
	FD_SET(c->sock, &server->save_set);
	// add the newly created client socket fd to save_set
	c->id = server->id_next_client;
	sprintf(c->id_str, "client %d :", c->id);
	server->id_next_client = server->id_next_client +1;
	// assign a unique ID to the new client and store it in the clientstruct
	// also store the announce to the id_str field of the client struct

	c->next = server->client_list;
	server->client_list = c;
	// add the newly created client to the server's client list 

	char welcome_msg[50];
	sprintf(welcome_msg, "server: client %d just arrived\n", c->id);
	t_client	*browse = c->next;
	while (browse)
	{
		if (FD_ISSET(browse->sock, &server->write_set))
			send(browse->sock, welcome_msg, strlen(welcome_msg), 0);
		browse = browse->next;
	}
	// send a welcome message to all existing clients, informint them 
	// that a new client has joined 
}

int	extract_message(char **buf, char **msg)
{
	char *newbuf;
	int	i=0;

	*msg = 0;
	if (*buf == 0)
		return (0);
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n') // if newline, copies te part of te string that comes 
		// after into a new string and assigns the pointer to the new string
		{
			newbuf = calloc(1, sizeof(*newbuf)*(strlen(*buf + i +1 ) +1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			// modifies msg so tat it ends at the newline caracter replaced by \0
			(*msg)[i+1] = 0;
			return (1);
		}
		i++;
	}
	return (0); // if no new line, does not modify the string
}

char *str_join(char *buf, char *add)
{
	// concatenates two strings buf and add and returns a pointer to the string
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
	// if buf is not NULL, the contents of buf are copied to the new buffer using 
	// strcat before adding the contents of add
	free(buf);
	strcat(newbuf, add);
	// concatenates buf and add
	return (newbuf);
}

void	dispatch_msg(t_client **client, t_server *server, int *skip_increment)
{
	char read_buf[10];
	int recv_return = recv((*client)->sock, read_buf, 9, 0);

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
		*skip_increment = 1;
		
		char goodbye_msg[50];
		sprintf(goodbye_msg, "server: client %d just left\n", to_delete->id);
		free(to_delete->buf);
		FD_CLLR(to_delete->sock, &server->save_set);
		close(to_delete->sock);

		if (to_delete == server->client_list)
			server->client_list = to_delete->next;
		else 
		{
			t_client	*parent_of_to_delete = server->client_list;
			while (parent_of_to_delete && parent_of_to_delete->next != to_delete)
				parent_of_to_delete = parent_of_to_delete->next;
			t_client  *browse =server->client_list;;
			while(browse)
			{
				if (FD_ISSET(browse->sock, &server->wrtite_set))
					send(browse->sock, goodbye_msg, strlen(goodbye_msg), 0);
			}
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
	// initialize the server struct with the provided port number and creates 
	// a listening socket

	while (1)
	{
		server.read_set = server.save_set;
		server.write_set = server.save_set;
		if (-1 == select(FD_SETSIZE, &server.read_set, &server.write_set, NULL, NULL))
			continue;
			// select waits until there is activity on the listening socket or any of the connected client sockets
			// return when there is activity on any of the sockets in the set of fd passed to it
		if (FD_ISSET(server.sock, &server.read_set))
			add_client(&server);
			// if the listening socket becomes readable, it means that a new client is trying
			// to connect to the server and the add_client function is called to accept
			// new connection and add the new client to the linked list of client maintained
			// by the server
		t_client *browse = server.client_list;
		while (browse)
		{
			int	skip_increment = 0;
			if (FD_ISSET(browse->sock, &server.read_set))
				dispatch_msg(&browse, &server, &skip_increment);
				// to process a message, takes a pointer to the client, the server and a flag
				// skip_increment, if its 1, it means that the current client has disconnected
				// and has been removed from the linked list of clientts and the loop skips to 
				// the next client without incrementing the pointer
			if (skip_increment == 0) // if we delete the node in the list, we have to increment the pointer in dispatch_msg and skip here
				browse = browse->next;
				// otherwise, the loop continuers to next client in the linked list
		}
	}
	return (0);
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