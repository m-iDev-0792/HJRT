//
// Created by 何振邦 on 2019-08-04.
//

#ifndef HJRT_CONSTANT_H
#define HJRT_CONSTANT_H

//constant e
constexpr float C_E = 2.71828182845904523536f;
constexpr float C_INV_E = 1 / C_E;
//constant pi
constexpr float C_PI = 3.14159265358979323846f;
constexpr float C_PI_OVER_TWO = C_PI * 0.5;
constexpr float C_TWO_PI = 2 * C_PI;
constexpr float C_FOUR_PI = 4 * C_PI;
constexpr float C_INV_PI = 0.31830988618379067154f;
constexpr float C_TWO_OVER_PI = 2 * C_INV_PI;
constexpr float C_INV_TWO_PI = 0.15915494309189533577f;
constexpr float C_INV_FOUR_PI = 0.07957747154594766788f;

constexpr float C_PI_MULTI(float x) {
	return C_PI * x;
}

constexpr float C_X_OVER_PI(float x) {
	return x / C_PI;
}

//constant sqrt 2
constexpr float C_SQRT_TWO = 1.41421356237309504880f;
constexpr float C_INV_SQRT_TWO = 0.70710678118654752440f;

#endif //HJRT_CONSTANT_H
