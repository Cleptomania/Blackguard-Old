#pragma once

class Camera {
public:
	int width;
	int height;
	int x;
	int y;
	int x2;
	int y2;

	Camera(int width, int height);
	Camera(int width, int height, int x, int y);
	void updatePos(int dx, int dy);
};