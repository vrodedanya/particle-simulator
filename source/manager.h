#ifndef MANAGER_H
#define MANAGER_H


class Manager
{
private:
public:
	virtual ~Manager(){};
	virtual void update(){};
	virtual void update_range(int begin, int end){};
	virtual void draw(){};
	virtual bool event_handler(){return false;}
};

#endif
