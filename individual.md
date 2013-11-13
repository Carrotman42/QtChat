

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
  - Server Test 2: Test MSG
  - Server Test 3: Test STATUS
  - Server Test 4: Test ERROR before login
  - Server Test 5: Test ERROR for malformed commands
 * Dave:
  - Client GUI Test 1: Generate Connect key press event - catch that it's handled.
  - Client GUI Test 2: Generate Quit key press event - catch that it's handled.
  - Client GUI Test 3: Generate Send Message key press event - catch that it's handled.
  - Client GUI Test 4: Emit sendConenctInfo() correctly - make sure proper arguments are caught
  - Client GUI Test 5: Emit sendMsg() correctly - make sure proper arguments are caught
  - Client GUI Test 6: Emit quitServer() correctly
  - Client GUI Test 7: Emit updateStatus() correctly - make sure proper arguments are caught
  - Client GUI Test 8: Make sure that wasConnected() was caught properly.
  - Client GUI Test 9: Make sure that gotMessage() was caught properly.
  - Client GUI Test 10: Make sure that updateList() was caught properly.
  - Client GUI Test 11: Make sure that onServerDisconnect() was caught properly.
  - Client GUI Test 12: Make sure that caughtError() was caught properly.
 * Rajat:
  - Client Test 1: Test slot sendMsg
  - Client Test 2: Test slot quitFromServer
  - Client Test 3: Test slot login
  - Client Test 4: Test slot status
  - Client Test 5: Test signal sendError
  - Client Test 6: Test signal newMessage
  - Client Test 7: Test signal listUpdate
  - Client Test 8: Test signal connected
  - Client Test 9: Test signal sendServerDisconnect