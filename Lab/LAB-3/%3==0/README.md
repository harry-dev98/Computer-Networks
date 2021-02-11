#### Task: 2 different groups of clients, they communicate between each other. Also a global message could be sent to server



Declared message types in queue:\
`type == 1 :: group 1 registers them with server`\
`type == 2 :: group 2 registers them with server`\
`type == 3 :: group 1 broadcasts the message to server`\
`type == 4 :: group 2 broadcasts the message to server`\
`type == 5 :: any client broadcasts the global message to server`\
`type == pid :: server sends message to clients`\


Declared message format is: `pid:message`