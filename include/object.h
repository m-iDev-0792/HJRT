//
// Created by 何振邦 on 2019-08-04.
//

#ifndef HJRT_OBJECT_H
#define HJRT_OBJECT_H

#include <iostream>

struct Object {
	std::string name;

	virtual void describe() const = 0;
};

#endif //HJRT_OBJECT_H
