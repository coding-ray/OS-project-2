ALL: server client
server: server.c
		@$(CC) $? $(CFLAGS) -o $@

client: client.c
		@$(CC) $? $(CFLAGS) -o $@

clear:
		@rm server client

CFLAGS = -Wall -Wextra -Wsign-conversion

# Note:
#   $? evaluates to all the prerequisites (server.c ... or client.c ...).
#   $@ evaluates to the target (server or client).
#   "gcc server.c -o server" command generates "server" as an executable.