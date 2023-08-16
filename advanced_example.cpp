#include <iostream>

#include "include/eos.hpp"


class myInstance : public eos::server_interface
{
public:
	myInstance() {}
};

static myInstance inst;


EOS_SERVER
{
	return inst;
}


int main(int argc, char** argv)
{

	return 0;
}