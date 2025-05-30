#include "FTPModel.h"
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <iostream>

static size_t parsingListing(void* ptr, size_t size, size_t nmenb, void* userdata) {
	std::string* s = static_cast<std::string*>(userdata);
	s->append(static_cast<char*>(ptr), size * nmenb);
	return size * nmenb;
}

static size_t downloadingFile(void* ptr, size_t size, size_t nmemb, void* userdata) {
	std::ofstream* file = static_cast<std::ofstream*>(userdata);
	file->write(static_cast<char*>(ptr), size * nmemb);
	return size * nmemb;
}

static size_t uploadingFile(void* ptr, size_t size, size_t nmemb, void* userdata) {
	std::ifstream* ifs = static_cast<std::ifstream*>(userdata);
	if (!ifs->good() || ifs->eof()) return 0;
	ifs->read(static_cast<char*>(ptr), size * nmemb);
	return static_cast<size_t>(ifs->gcount());
}

FTPModel::FTPModel() : curl(nullptr), passiveMode(true) {
	CURL* easyCurl = static_cast<CURL*>(curl);
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(easyCurl, CURLOPT_FTPPORT, nullptr);
	}
}

FTPModel::~FTPModel() {
	CURL* easyCurl = static_cast<CURL*>(curl);
	disconnect();
	if (curl) {
		curl_easy_cleanup(easyCurl);
	};
	curl_global_cleanup();
}

void FTPModel::setPassiveMode(bool passive) {
	passiveMode = passive;
	CURL* easyCurl = static_cast<CURL*>(curl);
	if (curl) {
		if (passiveMode) {
			curl_easy_setopt(easyCurl, CURLOPT_FTPPORT, nullptr);
		}
		else {
			curl_easy_setopt(easyCurl, CURLOPT_FTPPORT, "-");
		}
	}
}

std::string FTPModel::getCurrentDirectory() {
	std::string current = "";
	CURL* easyCurl = static_cast<CURL*>(curl);
	if (curl) {
		char* effectiveUrl = nullptr;
		curl_easy_getinfo(easyCurl, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
		current = effectiveUrl ? effectiveUrl : "";
	}
	return current;
}

bool FTPModel::connect(const std::string& server, const std::string& user, const std::string& pwd) {
	if (!curl) return false;
	CURL* easyCurl = static_cast<CURL*>(curl);

	std::string url = "ftp://" + server + "/";
	curl_easy_setopt(easyCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(easyCurl, CURLOPT_USERPWD, (user + ":" + pwd).c_str());

	curl_easy_setopt(easyCurl, CURLOPT_NOBODY, 1L);
	CURLcode res = curl_easy_perform(easyCurl);
	curl_easy_setopt(easyCurl, CURLOPT_NOBODY, 0L);

	return (res == CURLE_OK);
}

std::vector<std::string> FTPModel::list() {
	std::vector<std::string> entries;
	if (!curl) return entries;
	CURL* easyCurl = static_cast<CURL*>(curl);
	std::string data;
	curl_easy_setopt(easyCurl, CURLOPT_DIRLISTONLY, 1L);
	curl_easy_setopt(easyCurl, CURLOPT_WRITEFUNCTION, parsingListing);
	curl_easy_setopt(easyCurl, CURLOPT_WRITEDATA, &data);

	CURLcode res = curl_easy_perform(easyCurl);
	if (res == CURLE_OK) {
		std::istringstream iss(data);
		std::string line;
		while (std::getline(iss, line)) {
			if (!line.empty()) entries.push_back(line);
		}
	}

	return entries;
}

bool FTPModel::cd(const std::string& path) {
	if (!curl) return false;
	CURL* easyCurl = static_cast<CURL*>(curl);

	char* effectiveUrl = nullptr;
	curl_easy_getinfo(easyCurl, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
	std::string base = effectiveUrl ? effectiveUrl : "";

	auto pos = base.find('/', 7); 
	std::string newUrl = base.substr(0, pos) + "/" + path + "/";
	curl_easy_setopt(easyCurl, CURLOPT_URL, newUrl.c_str());

	curl_easy_setopt(easyCurl, CURLOPT_NOBODY, 1L);
	CURLcode res = curl_easy_perform(easyCurl);
	curl_easy_setopt(easyCurl, CURLOPT_NOBODY, 0L);

	if (res != CURLE_OK) {
		curl_easy_setopt(easyCurl, CURLOPT_URL, base.c_str());
	}

	return (res == CURLE_OK);
}

bool FTPModel::download(const std::string& remoteFile, const std::string localFile) {
	if (!curl) return false;

	CURL* easyCurl = static_cast<CURL*>(curl);

	std::ofstream ofs(localFile, std::ios::binary);
	if (!ofs) return false;

	char* eff_url = nullptr;
	curl_easy_getinfo(easyCurl, CURLINFO_EFFECTIVE_URL, &eff_url);

	std::string base = eff_url ? eff_url : "";

	if (!base.empty() && base.back() != '/' ) base.push_back('/');

	std::string remoteFileUrl = base + remoteFile;

	std::cout << "Downloading from: " << remoteFileUrl << "\n"
		<< "Saving to:      " << localFile << "\n";

	curl_easy_setopt(easyCurl, CURLOPT_DIRLISTONLY, 0L);
	curl_easy_setopt(easyCurl, CURLOPT_URL, remoteFileUrl.c_str());
	curl_easy_setopt(easyCurl, CURLOPT_WRITEFUNCTION, downloadingFile);
	curl_easy_setopt(easyCurl, CURLOPT_WRITEDATA, &ofs);

	CURLcode res = curl_easy_perform(easyCurl);

	curl_easy_setopt(easyCurl, CURLOPT_URL, base.c_str());

	return (res == CURLE_OK);
}

bool FTPModel::upload(const std::string& remoteFile, const std::string localFile) {
	if (!curl) return false;
	
	CURL* easyCurl = static_cast<CURL*>(curl);

	char* eff_url = nullptr;
	curl_easy_getinfo(easyCurl, CURLINFO_EFFECTIVE_URL, &eff_url);

	std::string base = eff_url ? eff_url : "";

	if (!base.empty() && base.back() != '/') base.push_back('/');

	std::string remoteFileUrl = base + remoteFile;

	std::cout << "Uploading to: " << remoteFileUrl << "\n"
		<< "From :      " << localFile << "\n";

	std::ifstream ifs(localFile, std::ios::binary);

	curl_easy_setopt(easyCurl, CURLOPT_URL, remoteFileUrl.c_str());

	curl_easy_setopt(easyCurl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(easyCurl, CURLOPT_READFUNCTION, uploadingFile);
	curl_easy_setopt(easyCurl, CURLOPT_READDATA, &ifs);

	CURLcode res = curl_easy_perform(easyCurl);

	curl_easy_setopt(easyCurl, CURLOPT_URL, base.c_str());

	return (res == CURLE_OK);
}

void FTPModel::disconnect() {
	if (!curl) return;

	CURL* easyCurl = static_cast<CURL*>(curl);

	curl_easy_setopt(easyCurl, CURLOPT_QUICK_EXIT, 1L);
	curl_easy_perform(easyCurl);
}