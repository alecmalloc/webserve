#include "webserv.hpp"

Client::Client( void ) : _eventFd( -1 ), _socketFd( -1 ), _closed( false ), _error( false ) {
	;
}

Client::Client( int eventFd, int socketFd ) : _eventFd( eventFd), _socketFd( socketFd ), \
		_closed( false ), _error( false ){
	;
}

Client::Client( const Client& og ) : _eventFd( og._eventFd ), _socketFd( og._socketFd ), \
		_closed( og._closed ), _error( og._error),  _content( og._content ), _request(og._request){
	;
}

Client::~Client( void ){
	;
}

//operator overloads
Client&	Client::operator =( const Client& og ){
	if( this != &og ){
		_eventFd = og._eventFd;
		_socketFd = og._socketFd;
		_closed = og._closed;
		_error = og._error;
		_content = og._content;
		_request = og._request;
	}
	return( *this );
}

//getters
int	Client::getEventFd( void ) const{
	return( _eventFd );
}

int	Client::getSocketFd( void ) const{
	return( _socketFd );
}

std::string	Client::getContent( void ) const{
	return( _content );
}

bool		Client::getClosed( void ) const{
	return( _closed );
}

bool		Client::getError( void ) const{
	return( _error );
}

//setters
void	Client::setEventFd( int fd ){
	_eventFd = fd;
}

void	Client::setSocketFd( int fd ){
	_socketFd = fd;
}

void	Client::setContent( std::string content ){
	//std::cout << "Before setContent: _content = " << _content << ", content = " << content << std::endl;
    _content += content;
    //std::cout << "After setContent: _content = " << _content << std::endl;
}

void	Client::setClosed( bool closed ){
	_closed = closed;
}

void	Client::setError( bool error ){
	_error = error;
}
