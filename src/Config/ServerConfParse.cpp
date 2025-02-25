#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <string>

static std::string	cutEnding( std::string tmp ){
	//if string ends on ; cut it
	if( tmp.at( tmp.size() - 1 ) == ';' )
		tmp = tmp.substr( 0, tmp.size() - 1 );

	return( tmp );
}

static std::string	makePath( std::string tmp, int root ){
	if( tmp.at( 0 ) != '/' && !root )
		tmp = "/" + tmp;
	if( tmp.at( tmp.size() - 1 ) == '/' )
		tmp = tmp.substr( 0, tmp.size() - 1 );
	return( tmp );
}

static bool	checkIp( std::string tmp ){
	//check if ip is valid 0-255 -> 0.0.0.0
	int	i = 0;
	int	check;
	size_t	pos;

	pos = tmp.find( '.' );
	while( pos != tmp.npos ){
		std::string iptmp = tmp.substr( i, pos );
		std::stringstream ss( iptmp );
		ss >> check;
		if ( check > 255 || check < 0 )
			return( false );
		if( tmp.at( pos + 1 ) )
			tmp = tmp.substr( pos + 1, tmp.size() );
		i = pos;
		pos = tmp.find( '.' );
	}	
	return( true );
}

static int	checkPort( std::string tmp ){
	//check if ports are valid ports 0 - 65535
	std::stringstream	ss( tmp );
	int			check;

	ss >> check;
	if ( check < 0 || check > 65535 )
		return( -1 );
	return( check );
}

void	parseListen( ServerConf& server, std::stringstream& ss ){
	//parse listen statement
	std::string	tmp;
	std::string	ip( DEFAULT_IP );
	int		port =  DEFAULT_PORT;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	if( tmp.find( ':' ) != tmp.npos ){
		if ( !checkIp( tmp.substr( 0, tmp.find( ':' ) ) ) )
			throw( std::runtime_error( "Wrong IpAddress" + \
				tmp.substr( 0, tmp.find( ':' ) ) ) );
		if( checkPort( tmp.substr( tmp.find( ':' ) + 1, tmp.size() ) ) == -1 )
			throw( std::runtime_error( "Wrong Port" + \
				tmp.substr( tmp.find( ':' ), tmp.size()  ) ) );
		ip = tmp.substr( 0, tmp.find( ':' ) );
		port = checkPort( tmp.substr( tmp.find( ':' ) + 1, tmp.size() ) ); 
	}
	else if( tmp.find( '.' ) != tmp.npos ){
		if( !checkIp( tmp ) )
			throw( std::runtime_error( "Wrong IpAddress" + tmp ) );
		ip = tmp.substr( 0, tmp.find( ':' ) );
	}
	else{
		if( checkPort( tmp ) == -1 )
			throw( std::runtime_error( "Wrong Port" + tmp ) );
		port = checkPort( tmp.substr( tmp.find( ':' ) + 1, tmp.size() ) ); 
	}
	server.setIpPort( cutEnding( ip ), port );
}

void	parseServerConfName( ServerConf& server, std::stringstream& ss ){
	//parse Server name
	std::string	tmp;

	while( ss >> tmp ){
		server.setServerConfName( cutEnding( tmp ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseErrorPage( ServerConf& server, std::stringstream& ss ){
	//set error page
	std::string	tmp;

	while( ss >> tmp ){
		int	error;
		std::stringstream	sstmp( tmp );
		sstmp >> error;
		ss >> tmp;
		server.setErrorPage( error, makePath( cutEnding( tmp ), 0 ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseBodySize( ServerConf& server, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		std::stringstream	sstmp( tmp );
		int			size;

		sstmp >> size; 
		if( size < 0 || size > MAXBODYSIZE )
			throw( std::runtime_error( "Wrong Max Body Size" ) );
		server.setBodySize( size );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void parseChunkedEncoding(ServerConf& server, std::stringstream& ss) {
    std::string tmp;
    ss >> tmp;
	std::cout << "CHUNKKKKKKKK" << tmp << "ENDDDDDD\n";
    if (tmp == "true;") {
        server.setChunkedTransfer(true);
    } else if (tmp == "false;") {
        server.setChunkedTransfer(false);
    } else {
        throw(std::runtime_error("Invalid value for use_chunked_encoding: " + tmp));
    }
    if (tmp.at(tmp.size() - 1) != ';')
        throw(std::runtime_error("Line not ended on ; " + tmp));
}

void parseChunkSize(ServerConf& server, std::stringstream& ss) {
    std::string tmp;
    ss >> tmp;
    std::stringstream sstmp(tmp);
    size_t size;
    sstmp >> size;
    server.setChunkSize(size);
	std::cout << "SIZEEEE" << size << ";\n";
    if (tmp.at(tmp.size() - 1) != ';')
        throw(std::runtime_error("Line not ended on ; " + tmp));
}

template< typename T >
void	parseRootDir( T& temp, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp )
		temp.setRootDir( makePath( cutEnding( tmp ), 1 ) );
	
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

template< typename T>
void	parseIndex( T& temp, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp )
		temp.setIndex( makePath( cutEnding( tmp ), 0 ) );

	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	Config::parseServerConfBlock( ServerConf& server ){
	const std::string	optionsArray[] =  \
	{ LISTEN, SERVER, ERROR, CLIENT, ROOT, INDEX, "use_chunked_encoding", "chunk_size"};

	void ( *functionArray[] )( ServerConf&, std::stringstream& ) = \
	{ parseListen, parseServerConfName, parseErrorPage, parseBodySize, \
		parseRootDir, parseIndex, parseChunkedEncoding, parseChunkSize};

	std::string		tmp;

	getline( _configFile, tmp );
	if ( tmp == "{" )
		getline( _configFile, tmp );
	while( tmp.find( '}' ) == tmp.npos ){
		std::stringstream	ss( tmp );
		std::string		key;
		ss >> key;
		for( int i = 0; i < 8; i++ ){
			if( !key.empty() && key.at( 0 ) == '#' ){
				break;
			}
			if( optionsArray[ i ] == key ){
				functionArray[ i ]( server, ss );
				break;
			}
			else if ( key == "location" ){
				parseLocationConfBlock( server, ss );
				break;
			}
			else if ( i == 7 && !key.empty() ){
				throw( std::runtime_error( "Not an valid configuration "\
							+ tmp ) );

			}
		}
		getline( _configFile, tmp );
	}
	server.checkAccess();
		
}
