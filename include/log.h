//
// Created by 何振邦 on 2019-08-12.
//

#ifndef HJRT_LOG_H
#define HJRT_LOG_H

#include <iostream>
#include <fstream>

#define FUNCNAME __func__
std::ostream &error(std::string place, std::string message);

#define Error(msg) error(FUNCNAME, msg)

std::ostream &warning(std::string place, std::string message);

#define Warning(msg) warning(FUNCNAME, msg)

std::ostream &note(std::string place, std::string message);

#define Note(msg) note(FUNCNAME, msg)

std::ostream &log(std::string place, std::string message);

#define Log(msg) log(FUNCNAME, msg)

#endif //HJRT_LOG_H
