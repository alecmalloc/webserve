#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

#include "ServerConf.hpp"
#include "Location.hpp"
#include "Defines.hpp"

class Config {
    private:
	//safe filedisriptor
        std::ifstream			_configFile;

	//safe servers
        std::vector< ServerConf >   	_servers;

    public:
	//cosntructors
        Config( void );
        Config( const Config& og );
        ~Config( void );

	//operator overloads
	Config&		operator =( const Config& og );

	//getters
	std::vector< ServerConf >	getServerConfs( void );

	//setters
	void	setServerConf( ServerConf server );

	//member fucnitons
    void	parse( std::string& filename );
	void	parseServerConfBlock( ServerConf& server );
	void	parseLocationConfBlock( ServerConf&, std::stringstream& );
};

#endif
