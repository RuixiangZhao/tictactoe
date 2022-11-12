# tictactoe
## Single player version of tictactoe
A simple c++ implementation of Tic Tac Toe with the minimax algorithm. 

**Modified from `https://github.com/Chaoskira/tictactoe-minimax`**

compile:
```shell
g++ play.cpp game.cpp -o play
```

run:
```shell
./play
```

## Two players version of tictactoe
**Realization in myself!!**

one player is client, another is server

### Server
compile server:
```shell
g++ game_server.cpp -o server -lws2_32
```

run server:
```shell
./server
```

### Client
compile client:
```shell
g++ game_client.cpp -o client -lws2_32
```

run client:
```shell
./client
```