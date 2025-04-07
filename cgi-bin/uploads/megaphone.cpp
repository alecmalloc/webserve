#include <iostream>


int main(int argc, char *argv[])
{
	char c;
	if(argc == 1)
	{
		std::cout << " * LOUD AND UNBEARABLE FEEDBACK NOISE *" << std::endl;
		return(0);
	}
	int i = 1;
	int j = 0;
	while(i < argc)
	{
		while(argv[i][j])
		{

			c = toupper(argv[i][j]);
			std::cout << c;
			j++;
		}
		j = 0;
		i++;
	}
	std::cout << std::endl;
	return(0);
}