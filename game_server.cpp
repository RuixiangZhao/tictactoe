#include "game_server.h"
#include <winsock.h>
// #include <ws2tcpip.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <errno.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

Game::Game() {
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			board[i][j] = '-';
		}
	}
}

void Game::printBoard() {
	cout << "-------------------";
	for(int i = 0; i < 3; i++) {
		cout << '\n' << "|";
		for(int j = 0; j < 3; j++) {
			cout << setw(3) << board[i][j] << setw(3) << " |";
		}
	}
	cout << '\n' << "-------------------" << '\n';
}

bool Game::gameOver() {
	if(checkWin(CLIENT)) return true;
	else if(checkWin(SERVER)) return true;

	bool emptySpace = false;
	for(int i = 0; i < 3; i++) {
		if(board[i][0] == '-' || board[i][1] == '-' || board[i][2] == '-')
			emptySpace = true;
	}
	return !emptySpace;
}

bool Game::checkWin(Player player) {
	char playerChar;
	if(player == CLIENT) playerChar = client;
	else playerChar = server;

	for(int i = 0; i < 3; i++) {
		// Check horizontals
		if(board[i][0] == playerChar && board[i][1] == playerChar
			&& board[i][2] == playerChar)
			return true;

		// Check verticals
		if(board[0][i] == playerChar && board[1][i] == playerChar
			&& board[2][i] == playerChar)
			return true;
	}

	// Check diagonals
	if (board[0][0] == playerChar && board[1][1] == playerChar 
		&& board[2][2] == playerChar) {
		return true;
	} else if (board[0][2] == playerChar && board[1][1] == playerChar 
		&& board[2][0] == playerChar) {
		return true;
	}

	return false;
}

Move Game::getServerMove() {
    int x, y = -1; // arbitrary assignment to init loop
	while(x < 0 || x > 2 || y < 0 || y > 2) {
		// Loop until a valid move is entered
		cout << "Enter your move in coordinate form, ex: (1,3)." << endl;
		cout << "Your Move: ";
		char c;
		string restofline;
		cin >> c >> c;
		x = c - '0' - 1;
		cin >> c >> c;
		y = c - '0' - 1;
		getline(cin, restofline); // get garbage chars after move
	}
	board[x][y] = server;

    Move myMove;
    myMove.x = x;
    myMove.y = y;
    return myMove;
}

int Game::play_network_server(){
    /* connect from client (line 97 ~ line 162) */
    //初始化套接字库
    WORD w_req = MAKEWORD(2, 2); //版本号2.2
    WSADATA wsadata;
    int err;
    err = WSAStartup(w_req, &wsadata);  //和相应的socket库绑定
    if (err != 0) {
        cout << "Failed to initialize socket library!" << endl;
        return 0;
    }
    else {
        cout << "Successfully initialized socket library!" << endl;
    }
    //检测版本号
    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        cout << "Version number of socket library does not match!" << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "The version number of socket character library is correct!" << endl;
    }
    
    //定义长度变量
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	//定义发送缓冲区和接收缓冲区
	char send_buf[100];
	char recv_buf[100];
	//定义服务端套接字，接收请求套接字
	SOCKET s_server;
	SOCKET s_accept;
	//服务端地址 客户端地址
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;

    //填充服务端地址信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(DEFAULT_IP);
    server_addr.sin_port = htons(DEFAULT_PORT);

    //创建套接字 socket(地址类型，套接字类型，传输协议)
    s_server = socket(AF_INET, SOCK_STREAM, 0);

    //绑定套接字到一个IP地址和一个端口上（bind()）
	if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "Socket binding failed!" << endl;
		closesocket(s_server);
		WSACleanup();
		return 0;
	}
	else {
		cout << "Socket binding succeeded!" << endl;
	}
	
	//将套接字设置为监听模式等待连接请求（listen()）
	if (listen(s_server, SOMAXCONN) < 0) {
		cout << "Failed to set monitoring status!" << endl;
		closesocket(s_server);
		WSACleanup(); //解除与Socket库的绑定并且释放Socket库所占用的系统资源
		return 0;
	}
	else {
		cout << "Setting monitoring status succeeded!" << endl;
	}
	cout << "The server is listening......" << endl;

    //请求到来后接收连接请求，返回一个新的对应于此次连接的套接字（accept()）
    len = sizeof(SOCKADDR);
    s_accept = accept(s_server, (SOCKADDR*)&accept_addr, &len);
    if (s_accept == SOCKET_ERROR) {
        cout << "Connection failed!" << endl;
        closesocket(s_server);
        closesocket(s_accept);
        WSACleanup();
        return 0;
    }
    cout << "Connect! Prepare to receive data..." << endl;

    int turn = 0;
    cout << "Client first!" << endl;
    printBoard();
    while(!checkWin(CLIENT) && !checkWin(SERVER) && !gameOver()) {
		// client move
        Move clientMove;
        Move serverMove;
        if(turn % 2 == 0) {
            cout << endl << "Client Player Move:" << endl;
            // receive client move
            recv_len = recv(s_accept, recv_buf, 100, 0);
            if (recv_len < 0) {
                cout << "Receive failed!" << endl;
                return 0;
            }
            clientMove.x = recv_buf[0];
            clientMove.y = recv_buf[1];

            board[clientMove.x][clientMove.y] = client;
			turn++;
			printBoard();
			if(checkWin(CLIENT)) cout << "Client Player Wins" << endl;
		} else {
			serverMove = getServerMove();
			turn++;
			printBoard();
            // send server move
            send_buf[0] = serverMove.x;
            send_buf[1] = serverMove.y;
            send_len = send(s_accept, send_buf, 100, 0);
            if (send_len < 0) {
                cout << "Sending failed!" << endl;
                return 0;
            }

            if(checkWin(SERVER)) cout << "Server Player Wins" << endl;
		}
	}
}

int main()
{
    Game tictactoe;

	tictactoe.play_network_server();

	return 0;
}