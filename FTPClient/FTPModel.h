#ifndef FTPMODEL_H
#define FTPMODEL_H

#include <string>
#include <vector>

class FTPModel {
	void* curl;
	bool passiveMode;
public:
	FTPModel();
	~FTPModel();

	bool connect(const std::string& server, const std::string& user, const std::string& pwd);
	void setPassiveMode(bool passive);
	std::string getCurrentDirectory();
	std::vector<std::string> list();
	bool cd(const std::string& path);
	bool download(const std::string& remoteFile, const std::string localFile);
	bool upload(const std::string& remoteFile, const std::string localFile);
	bool deleteFile(const std::string& remoteFile);
	void disconnect();

};

#endif // !FTPMODEL_H

