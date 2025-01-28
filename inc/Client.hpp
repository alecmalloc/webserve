#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>

// forward declaration
class HttpRequest;

class	Client{
	public:
		//constructor
		Client( void );
		Client( int, int );
		Client( const Client& );
		~Client( void );

		//operator overloads
		Client&	operator =( const Client& );

		//getters
		int		getEventFd( void ) const;
		int		getSocketFd( void ) const;
		std::string	getContent( void ) const;
		bool		getClosed( void ) const;
		bool		getError( void ) const;
		
		//setters
		void		setEventFd( int );
		void		setSocketFd( int );
		void		setContent( std::string );
		void		setClosed( bool );
		void		setError( bool );

		//memberfunctions

	private:
		//stores event and socket fd
		int		_eventFd;
		int		_socketFd;

		//checks if recv got 0 so the client closed the connection on sending
		bool		_closed;

		//check if recv got -1 so the client had an error on connection 
		bool		_error;

		//stores read content
		std::string	_content;

		// all parsed data from request
		HttpRequest*	_request;
};

#endif
