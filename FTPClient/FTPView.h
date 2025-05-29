#pragma once
#ifndef FTPVIEW_H
#define FTPVIEW_H

#include <vector>
#include <string>

class FTPView {
public:
	void showMenu();
	void showCurrentDirectory(const std::string& cwd);
	void showList(const std::vector<std::string>& items);
	std::string prompt(const std::string& msg);
	void showMsg(const std::string& msg);
};

#endif // !FTPVIEW_H
