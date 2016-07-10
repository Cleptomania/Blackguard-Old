#include "Camera.h"

Camera::Camera(int width, int height) {
	Camera(width, height, 0, 0);
}

Camera::Camera(int width, int height, int x, int y) : width(width), height(height), x(x), y(y) {
	x2 = x + width;
	y2 = y + height;
}

void Camera::updatePos(int dx, int dy) {
	if (dx != 0 || dy != 0) {
		x += dx;
		y += dy;
	}
}