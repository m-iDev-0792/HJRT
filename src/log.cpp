//
// Created by 何振邦 on 2019-08-12.
//

#include "log.h"

std::ostream &error(std::string place, std::string message) {
	std::cout << "ERROR @ " << place << ":" << message;
	return std::cout;
}

std::ostream &warning(std::string place, std::string message) {
	std::cout << "WARNING @ " << place << ":" << message;
	return std::cout;
}

std::ostream &note(std::string place, std::string message) {
	std::cout << "NOTE @ " << place << ":" << message;
	return std::cout;
}

std::ostream &log(std::string place, std::string message) {
	static std::ofstream logFile("log.txt");
	logFile << "LOG @ " << place << ":" << message;
	return logFile;
}