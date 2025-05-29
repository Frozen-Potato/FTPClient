#ifndef FTPCONTROLLER_H
#define FTPCONTROLLER_H

#include "FTPModel.h"
#include "FTPVIEW.h"

class FTPController {
	FTPModel model;
	FTPView view;
public:
	void run();
};

#endif 
