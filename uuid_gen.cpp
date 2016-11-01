#include <iostream>
#include <uuid++.hh>

int main()
{
	uuid uu;
	uu.make(UUID_MAKE_V4);
	std::cout << uu.string() << std::endl;
	
	uu.make(UUID_MAKE_V4);
	std::cout << uu.string() << std::endl;	
	return 0;
}
