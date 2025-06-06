#include "FTPController.h"
#include <sstream>

void FTPController::run() {
	std::string mode = view.prompt("Use passive mode? (y/n): ");
	bool passive = (mode == "y" || mode == "Y");

    if (passive) {
        view.showMsg("Mode set to passive");
    }
    else {
        view.showMsg("Mode set to active");
    }

	model.setPassiveMode(passive);

	std::string server_ip = view.prompt("Server (host:port) : ");
	std::string user = view.prompt("Username: ");
	std::string pwd = view.prompt("Password: ");

    if (!model.connect(server_ip, user, pwd)) {
        view.showMsg("Connection failed.");
        return;
    }

    bool running = true;
    while (running) {
        std::string current = model.getCurrentDirectory();
        view.showCurrentDirectory(current);
        view.showMenu();
        std::string choice = view.prompt("");

        int userChoice = -69;

        try {
            userChoice = std::stoi(choice);
        }
        catch (...) {
            userChoice = -69;
        }

        switch (userChoice) {
            case 1: {
                auto list = model.list();
                view.showList(list);
                break;
            }
            case 2: {
                std::string dir = view.prompt("Enter directory: ");
                if (model.cd(dir))
                    view.showMsg("Directory changed.");
                else
                    view.showMsg("Failed to change directory.");
                break;
            }
            case 3: {
                std::string remote = view.prompt("Remote file URL: ");
                std::string local = view.prompt("Save as local file: ");
                if (model.download(remote, local))
                    view.showMsg("Download succeeded.");
                else
                    view.showMsg("Download failed.");
                break;
            }
            case 4: {
                std::string local = view.prompt("Uploadng from local file: ");
                std::string remote = view.prompt("To remote file URL: ");
                if (model.upload(remote, local))
                    view.showMsg("Upload succeeded.");
                else
                    view.showMsg("Upload failed.");
                break;
            }
            case 5: {
                std::string remote = view.prompt("Remote File for Delete: ");
                if (model.deleteFile(remote))
                    view.showMsg("Delete succeeded");
                else
                    view.showMsg("Delete Failed");
                break;
            }
            case 6:
            {
                model.disconnect();
                view.prompt("Disconnected");
                running = false;
                break;
            }
            default: {
                view.showMsg("Invalid option.");
                break;
            }
        }
    }
};