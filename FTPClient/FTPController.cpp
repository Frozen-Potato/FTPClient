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

        if (choice == "1") {
            auto list = model.list();
            view.showList(list);
        }
        else if (choice == "2") {
            std::string dir = view.prompt("Enter directory: ");
            if (model.cd(dir))
                view.showMsg("Directory changed.");
            else
                view.showMsg("Failed to change directory.");
        }
        else if (choice == "3") {
            std::string remote = view.prompt("Remote file URL: ");
            std::string local = view.prompt("Save as local file: ");
            if (model.download(remote, local))
                view.showMsg("Download succeeded.");
            else
                view.showMsg("Download failed.");
        }
        else if (choice =="4") {
            std::string local = view.prompt("Uploadng from local file: ");
            std::string remote = view.prompt("To remote file URL: ");
            if (model.upload(remote, local))
                view.showMsg("Upload succeeded.");
            else
                view.showMsg("Upload failed.");
        }
        else if (choice == "5") {
            std::string remote = view.prompt("Remote File for Delete: ");
            if (model.deleteFile(remote))
                view.showMsg("Delete succeeded");
            else 
                view.showMsg("Delete Failed");
        }
        else if (choice == "6") {
            model.disconnect();
            view.prompt("Disconnected");
            running = false;
        }
        else {
            view.showMsg("Invalid option.");
        }
    }
};