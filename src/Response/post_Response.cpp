#include "webserv.hpp"


//check contentlength againt defined content length 
void	Response::checkContentLength( void ){

	//get max body size if defined for location otherwise from server
	size_t maxBodySize = _locationConf.getBodySizeInitilized() ? \
			     _locationConf.getBodySize() : _serverConf.getBodySize();

	//cehck if its chunked encoding
	std::map<std::string, std::vector<std::string> >	headers = _request.getHeaders();
	std::map<std::string, std::vector<std::string> >::const_iterator transferEncodingIt = \
		headers.find("Transfer-Encoding");

	//check size
   	if (transferEncodingIt != headers.end() && !transferEncodingIt->second.empty() &&
        	transferEncodingIt->second[0] == "chunked" ) {

       		if( _request.getBody().size() > maxBodySize )
			throw( 413 );
    	}
	else{ 
		//check for content length
		std::map<std::string, std::vector<std::string> >::const_iterator it = \
			headers.find("Content-Length");
	
		//if no content length return no content length
		if( it == headers.end() || it->second.empty() )
			throw( 411 );
	
		std::istringstream	iss( it->second[0] );
		size_t 			contentLength;

		iss >> contentLength;
	
		if( iss.fail() )
			throw( 500 );
	
		if( contentLength > maxBodySize )
			throw( 413 );
	}
}

//find boundary and extract it
static std::string	extractBoundary( std::string contentType ){
	
	size_t	boundaryPos = contentType.find( "boundary=" );
	
	if( boundaryPos == std::string::npos )
		throw( 400 );

	std::string boundary = contentType.substr( boundaryPos + 9 );
	if( boundary.empty() )
		throw( 400 );

	return( boundary );
}

//find name and filename and extract it
static void	parseHeaders( std::string& header, std::string& name, std::string& filename ){

	size_t	disPos = header.find( "Content-Disposition:" );

	if( disPos == std::string::npos )
		throw( 400 );

	size_t	namePos = header.find( "name=\"" );
	if( namePos == std::string::npos )
		throw( 400 );
	namePos += 6;

	size_t	nameEndPos = header.find( "\"", namePos );
	if( nameEndPos == std::string::npos )
		throw( 400 );

	name = header.substr( namePos, nameEndPos - namePos );

	size_t	filePos = header.find( "filename=\"" );
	if( filePos != std::string::npos ){

		filePos += 10;

		size_t	fileEndPos = header.find( "\"", filePos );
		if( fileEndPos == std::string::npos )
			throw( 400 );
		filename = header.substr( filePos, fileEndPos - filePos );
	}
}

static std::string	sanitizeFilename( const std::string& filename ){

	std::string	clean;

	for( size_t i = 0; i < filename.size(); i++ ){

		char c = filename[i];

		if( c == '/' || c == '\\' )
			clean += '_';
		else
			clean += c;
	}

	return( clean );
}

static void	writeToFile( const std::string& filePath, const std::string& content ){
	
	std::ofstream	out( filePath.c_str(), std::ios::out | std::ios::binary );

	if( !out.is_open() )
		throw( 500 );

	out.write( content.c_str(), content.size() );
	out.close();

	if( out.fail() )
		throw( 500 );
}

void	trimContent( std::string& s ){
	while( !s.empty() && ( s[s.size() - 1] == '\r' || s[s.size() - 1] == '\n' ) )
		s.erase( s.size() - 1 );
}

static void	handleMultipartUpload( std::string contentType, const std::string& body, \
	       		std::string uploadDir ){
	
	std::string		boundary = extractBoundary( contentType );
	std::string		boundaryMarker = "--" + boundary;
	std::string		boundaryEndMarker = boundaryMarker + "--";

	size_t			pos = 0;

	while( ( pos = body.find( boundaryMarker, pos ) ) != std::string::npos ){

		//handle ending cases
		if( body.substr( pos, boundaryMarker.length() + 2 ) == boundaryEndMarker )
			break;
		
		//move pos to start from header
		pos += boundaryMarker.length();
		if( body.substr( pos, 2 ) == "\r\n" )
			pos += 2;

		//find end of header
		size_t	headerEnd = body.find( "\r\n\r\n", pos );

		//if end of header not found -> faulty request
		if( headerEnd == std::string::npos )
			throw( 400 );

		std::string	header = body.substr( pos, headerEnd - pos );
		pos = headerEnd + 4;

		std::string	name, filename;

		parseHeaders( header, name, filename );

		size_t	nextBoundary = body.find( boundaryMarker, pos );
		if( nextBoundary == std::string::npos )
			throw( 400 );

		std::string content = body.substr( pos, nextBoundary - pos );

		trimContent( content );

		if( !filename.empty() ){
			std::string sanitized = sanitizeFilename( filename );
			writeToFile( uploadDir + sanitized, content );
		}

		pos = nextBoundary;
	}
}

void	Response::handlePostRequest( void ){

	std::string	postData = _request.getBody();
	std::string	contentType = _request.getHeaders()["Content-Type"][0];
	std::string	uploadDir = _locationConf.getUploadDir().empty() ? \
				    _locationConf.getUploadDir() : DEFAULT_UP_DIR;
	
	static int	uploadCount = 0;
	std::string	returnBody( UPLOAD_SUCCESS );
	returnBody += CSS_GLOBAL;

	//check for contentlength
	checkContentLength();

	//if empty upload send 200
	if( postData.empty() )
		setStatus( 204 );

	//multipart upload
	else if( contentType.find( "multipart/form-data" ) != std::string::npos ){
	
		handleMultipartUpload( contentType, _request.getBody(), uploadDir );

		setStatus( 201 );
		setBody( returnBody );
	}

	//basic upload
	else{
		std::string	filePath = uploadDir + "upload_" + intToString( uploadCount );
		writeToFile( filePath, postData );

		uploadCount++;
		setStatus( 201 );
		setBody( returnBody );
	}
}
