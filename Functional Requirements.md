- Dave Kindel
- Rajat Gupta
- Kevin Malachowski

Final Project Functional Requirements

*	The final project for our group will consist of client server chat room. 
*	The project will have a multi-threaded sever design to connect with several clients at once
*	Each client will have its own GUI which will display messages from all users, the custom message the user wishes to send, and send and quit push buttons. 
*	The messages will be written into a socket which will be processed by the server. 
*	The sever will thus handle all incoming messages and broadcast them to all connected clients
*	The client and server will be modeled after assignment 7
*	Each user will observe the following GUI after connecting to the server:

SHOW THE IMAGE HERE!


* On the server side, there will be no graphical interface
   * Information will be logged to stdout and clients will communicate via TCP/IP
      * A custom protocol will be used to communicate between the server and client
      * Therefore all the chat room will actually be client-independant
      * Testing can be done via telnet
   * Protocol summary:
     * Every message from server to client will be be of the following form:
       `COMMAND ARGS`
     * Every message is deliminated with a newline ('\n')
     * The argument requirements can be different for every command
     * The command identifier ("COMMAND" above) is case insensitive
     * More commands can be added later, with certain versions of clients and servers supporting certain commands and ignoring unsupported ones.
     * There are a set of commands which are for client->server and some for server->client
       * There may or may not be an intersection between these sets of commands


Client->Server
<table>
<tr><th>Command</th><th>Args</th><th>Description</th></tr>
<tr><td>LOGIN</td><td>name</td><td>Gives a name to the given user. This must be done before sending any other commands. Not doing so will cause the server to simply return error messages. The name may not contain a newline nor a zero byte and must be at least 1 character long.</td></tr>
<tr><td>MSG</td><td>message-contents</td><td>Send a message to the chatroom. This will cause the message to be broadcasted to everyone currently in the chatroom. This may not contain a newline.</td></tr>
<tr><td>STATUS</td><td>status-contents</td><td>Set your status. Will be reported along with your name in the list of people in the chatroom. This may not contain a newline nor a zero byte.</td></tr>
</table>

Server->Client
<table>
<tr><th>Command</th><th>Args</th><th>Description</th></tr>
<tr><td>MSG</td><td>FROM\0message-contents</td><td>Notifies users about a user sending a message to the chatroom</td></tr>
<tr><td>LIST</td><td>user1\0status1\0user2\0status2\0...\0userN\0statusN</td><td>Represents a list of users currently in the chatroom along with their current statuses. Note that the `\0' in the args is the zero byte and is used to delimit statuses and users.</td></tr>
<tr><td>ERROR</td><td>description</td><td>Represents some sort of error from a client command. The description will describe what's up</td></tr>
</table>



Testing:

- GUI:
  - Test 1: Generate Connect key press event - catch that it's handled.
  - Test 2: Generate Quit key press event - catch that it's handled.
  - Test 3: Generate Send Message key press event - catch that it's handled.
  - Test 4: Emit sendConenctInfo() correctly - make sure proper arguments are caught
  - Test 5: Emit sendMsg() correctly - make sure proper arguments are caught
  - Test 6: Emit quitServer() correctly
  - Test 7: Emit updateStatus() correctly - make sure proper arguments are caught
  - Test 8: Make sure that wasConnected() was caught properly.
  - Test 9: Make sure that gotMessage() was caught properly.
  - Test 10: Make sure that updateList() was caught properly.
  - Test 11: Make sure that onServerDisconnect() was caught properly.
  - Test 12: Make sure that caughtError() was caught properly.
  

- Client:
  - Test 1: Write to a sever, and have the server print the message
  - Test 2: Recieve and print message from a server
  - Test 3: Quitting and closing the client properly
  - Test 4: Client signals server when quitting so quit messages captured by the server
  
- Server: Use straight sockets to mock clients
  - Test 1: Test LOGIN
    * Socket 1 connects but does not log in
    * Socket 2 connects and logs in: Socket 1 should not receive any information
    * Socket 1 logs in: Socket 2 should receive a LIST with the fact that both socket 1 and 2 are in the chatroom.
  - Test 2: Test MSG
    * Socket 1 and 2 connect and log in
    * Socket 1 sends a message: Socket 2 should receive the message
  - Test 3: Test STATUS
    * Socket 1 and 2 connect and log in
    * Socket 1 sends a status update: Socket 2 should receive the status update in a LIST 
  - Test 4: Test ERROR before login
    * Socket 1 connects
    * Socket 1 sends MSG, STATUS: Should receive error codes each time
  - Test 5: Test ERROR for malformed commands
    * Socket 1 connects and logs in
    * Socket 1 sends various malformed/invalid commands: Should receive an error response each time.
