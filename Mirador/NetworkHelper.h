#pragma once
#include "common.h"

class NetworkHelper
{
	public:
		NetworkHelper() = delete;

		static int RecvString(SOCKET s, std::string& dest, const char &delimiter = '\n')
		{
			char r;
			int read = 0, state;
			dest = "";
			while((state = recv(s, &r, 1, NULL)) && r != delimiter) {
				if (state < 0)
					return state;
				read += state;
				dest += r;
			}

			return read;
		}

		static int RecvStringNonBlocking(SOCKET s, std::string& dest, const char& delimiter = '\n')
		{
			fd_set reads_socks;
			TIMEVAL t;
			t.tv_sec = 0;
			reads_socks.fd_count = 1;
			reads_socks.fd_array[0] = s;

			if (select(NULL, &reads_socks, NULL, NULL, &t) < 1)
				return 0;

			return RecvString(s, dest, delimiter);
		}

		static int RecvData(SOCKET s, std::vector<char>& out, const char& delimiter = '\n')
		{
			char r;
			int read = 0;
			while ((read += recv(s, &r, 1, NULL)) && r != delimiter) {
				out.push_back(r);
			}

			return read;
		}

		static int SendString(SOCKET s, const std::string& send)
		{
			return ::send(s, send.c_str(), send.length(), NULL);
		}

		static int SendData(SOCKET s, std::vector<char>& send)
		{
			int sent = 0;
			for (char& c : send) {
				sent += ::send(s, &c, 1, NULL);
			}
			return sent;
		}
};