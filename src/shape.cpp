//
// Created by 何振邦 on 2019-05-15.
//

#include "shape.h"

bool Shape::getUV(const HitInfo &hitInfo, glm::vec2 *uvCoord) const {
	*uvCoord = hitInfo.uv;
	return false;
}
void Shape::transform(glm::mat4 mat) {
	//do nothing
}
