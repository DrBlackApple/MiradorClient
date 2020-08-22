#include "Client.h"

#pragma comment(lib, "Ws2_32.lib")

std::map<std::string, Hook*> Client::_hooks{};
bool Client::_connected = false;

Client::Client(const std::string& ip, const int& port) :
	 _connection{INVALID_SOCKET}
{
	if (WSAStartup(MAKEWORD(2, 2), &this->_ws_d))
		exit(0);
	
	/*struct addrinfo hint;
	ZeroMemory(&hint, sizeof(struct sockaddr));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;

	int res;
	while((res = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hint, &_res_addr)) != 0) {
		std::cout << res << std::endl;
		Sleep(Settings::timeout);
	}*/
	_sin = new SOCKADDR;
	int af;
	SOCKADDR_IN sin;
	ZeroMemory(&sin, sizeof(SOCKADDR));
	if (ip.find(':') != std::string::npos) {
		//ipv6
		af = AF_INET6;
	}
	else {
		af = AF_INET;
	}
	ZeroMemory(&sin, sizeof(SOCKADDR_IN));
	sin.sin_family = af;
	sin.sin_port = htons(Settings::server_port);
	inet_pton(af, Settings::server_ip.c_str(), &sin.sin_addr);
		
	CopyMemory(_sin, &sin, sizeof(SOCKADDR));
	
}

Client::~Client()
{
	if (_connection)
		closesocket(_connection);

	delete _sin;
	WSACleanup();
}

void Client::Connect()
{
	_connection = socket(_sin->sa_family, SOCK_STREAM, NULL);
	if (connect(_connection, _sin, sizeof(*_sin)) == 0) {
		_connected = true;
		_hdl = (HANDLE)_beginthread(Client::Handle, 0, (void *)&_connection);
	}
	else {
#ifdef _DEBUG
		std::cout << WSAGetLastError() << std::endl;
#endif
		_connected = false;
	}
}

bool Client::IsConnected()
{
	/*int error_code;
	int error_code_size = sizeof(error_code);
	getsockopt(_connection, SOL_SOCKET, SO_ERROR, (char *)&error_code, &error_code_size);
	_connected = error_code == WSAECONNRESET;

	if (!_connected && _hdl) {
		TerminateThread(_hdl, 0);
		_hdl = NULL;
	}*/

	return _connected;
}

SOCKET Client::getSocket()
{
	return _connection;
}

//static method
void Client::RegisterHook(const std::vector<std::string>& hooks, Hook* cb)
{
	for(auto &hook : hooks)
		if(_hooks.find(hook) == _hooks.end())
			_hooks.emplace(hook, cb);
}

void Client::Handle(void* arg)
{
	SOCKET s = *((SOCKET*)arg);
	std::string read;
	while (_connected) {
		int state = NetworkHelper::RecvString(s, read);
		if (state < 1)
			_connected = false;
		//call the hook
		std::string cmd = read.substr(0, read.find(" "));
		auto it = _hooks.find(cmd);
		if (it != _hooks.end())
			it->second->HandleHook(s, read);
	}
#ifdef _DEBUG
	std::cout << "disconnected..." << std::endl;
#endif
	closesocket(s);
}