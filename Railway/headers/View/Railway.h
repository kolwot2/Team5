#pragma once

class Railway {
	int windowHeight, windowWidth;
public:
	Railway() = delete;
	Railway(const Railway&) = delete;
	Railway(int, int);
	void start();
};