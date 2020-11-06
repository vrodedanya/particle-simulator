#ifndef MANAGER_H
#define MANAGER_H


class Manager
{
private:

public:
	virtual ~Manager(){};
	void update(){};
	void update_range(int begin, int end){};
	void draw(){};
};

#endif
