#include "FTPVIEW.h"
#include <iostream>

void FTPView::showMenu() {
	std::cout
		<< "Menu\n"
		<< "1. List Directory\n"
		<< "2. Chenge Directory\n"
		<< "3. Download File\n"
		<< "4. Upload File\n"
		<< "5. Delete File\n"
		<< "6. Quit\n"
		<< "Select: ";
}

void FTPView::showCurrentDirectory(const std::string& cwd) {
	std::cout << "Current directory: " << cwd << "\n";
}

void FTPView::showList(const std::vector<std::string>& items) {
	for (auto& item : items) std::cout << item << "\n";
	std::cout << std::endl;
}

std::string FTPView::prompt(const std::string& msg) {
	std::cout << msg;
	std::string input;
	std::getline(std::cin, input);
	return input;
}

void FTPView::showMsg(const std::string& msg) {
	std::cout << msg << "\n";
}