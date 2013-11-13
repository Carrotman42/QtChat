

Implementation Requirements
===========
Kevin: Server
 * class chatserver
 * class session
Dave: Client GUI
 * class clientGUI
Rajat: Client backend
 * class client

Test assignments:
===========
 * Kevin:
  - Server Test 1: Test LOGIN
    * Socket 1 connects but does not log in
    * Socket 2 connects and logs in: Socket 1 should not receive any information
    * Socket 1 logs in: Socket 2 should receive a LIST with the fact that both socket 1 and 2 are in the chatroom.
  - Server Test 2: Test MSG
    * Socket 1 and 2 connect and log in
    * Socket 1 sends a message: Socket 2 should receive the message
  - Server Test 3: Test STATUS
    * Socket 1 and 2 connect and log in
    * Socket 1 sends a status update: Socket 2 should receive the status update in a LIST 
  - Server Test 4: Test ERROR before login
    * Socket 1 connects
    * Socket 1 sends MSG, STATUS: Should receive error codes each time
  - Server Test 5: Test ERROR for malformed commands
    * Socket 1 connects and logs in
    * Socket 1 sends various malformed/invalid commands: Should receive an error response each time.
 * Dave:
 
 * Rajat: