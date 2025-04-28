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

/*static std::string	uploadPathhandler( void ){

	std::string	uploadDir = DEFAULT_UP_DIR;

	if( !_locationConf.getUploadDir().empty() ){

		// Make path relative
		uploadDir = "./" + _locationConf.getUploadDir(); 

		// Make sure upload dir ends with a slash
		if( uploadDir[uploadDir.length() - 1] != '/')
			uploadDir += "/";
	}

	TODO::check if upoload dir can be created or if config already fails
	Create directory recursively
	std::string	dirPath = uploadDir;
	size_t		pos = 0;

	while( ( pos = dirPath.find('/', pos + 1 ) ) != std::string::npos ){

		std::string	subPath = dirPath.substr( 0, pos );
		if( !subPath.empty() && access( subPath.c_str(), F_OK ) == -1 ){
			if( mkdir( subPath.c_str(), 0777 ) == -1 ){
				throw( 500 ); // Internal Server Error
			}
		}
	}

	// Verify directory exists and has write permissions
	if( access( uploadDir.c_str(), W_OK ) == -1 ){
		throw( 403 ); // Forbidden
	}

	// Generate unique filename with proper path
	std::string	filePath = uploadDir + "upload_" + getCurrentDateTime();

	// Replace spaces and colons in filename with underscores
	std::string::size_type position = 0;
	while( ( position = filePath.find_first_of( " :", position ) ) != std::string::npos ){
		filePath[position] = '_';
		position++; // Move to next character
	}

	return filePath;
}*/

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


/*TODO::rewrite multipart uploads!!!!!!
void	Response::handleMultipartUpload( std::string contentType ){

	// Extract boundary
	size_t	boundaryPos = contentType.find( "boundary=" );

	if( boundaryPos == std::string::npos )
		throw( 400 );

	//get boundrymarker
	std::string		boundary = contentType.substr( boundaryPos + 9 );
	const std::string&	postData = _request.getBody();

	//bondry marker in body is always with leading "--"
	std::string boundaryMarker = "--" + boundary;

	// Find the start of the first part with either format
	size_t			partStart = postData.find( boundaryMarker );
	if( partStart == std::string::npos )
		throw( 400 );

	// Skip to the end of the boundary line
	partStart = postData.find( "\r\n", partStart );
	if( partStart == std::string::npos )
		throw( 400 );
	
	partStart += 2; // Skip \r\n

	// Find the Content-Disposition header
	size_t dispPos = postData.find("Content-Disposition:", partStart);
	if (dispPos == std::string::npos) {
		throw 400;
	}


	// Extract filename
	size_t filenamePos = postData.find("filename=\"", dispPos);
	if (filenamePos == std::string::npos) {
		throw 400;
	}
	filenamePos += 10; // Skip 'filename="'

	size_t filenameEnd = postData.find("\"", filenamePos);
	if (filenameEnd == std::string::npos) {
		throw 400;



	}

	std::string filename = postData.substr(filenamePos, filenameEnd - filenamePos);

	// Find the blank line that marks the start of file content
	size_t contentStart = postData.find("\r\n\r\n", filenameEnd);
	if (contentStart == std::string::npos) {
		throw 400;
	}
	contentStart += 4; // Skip \r\n\r\n

	// Find the end boundary
	size_t contentEnd = postData.find("\r\n--" + boundary, contentStart);
	if (contentEnd == std::string::npos) {
		throw 400;
	}

	// Extract file content
	std::string fileContent = postData.substr(contentStart, contentEnd - contentStart);

	// Create the upload directory (use your existing function)
	std::string uploadDir = uploadPathhandler();

	// But instead of using the generated filename, use the original filename
	// Extract the directory path from the uploadPathhandler result
	std::string dirPath = uploadDir.substr(0, uploadDir.find_last_of("/") + 1);

	// Use the original filename (sanitize it if needed)
	std::string filePath = dirPath + filename;

	// Write only the file content to the file
	std::ofstream outFile(filePath.c_str(), std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		throw 500; // Internal Server Error
	}

	outFile.write(fileContent.c_str(), fileContent.length());
	outFile.close();

	if (outFile.fail()) {
		throw 500; // Internal Server Error
	}

	// Success response
	genarateUploadSucces();
}*/

void	Response::handlePostRequest( void ){

	std::string	postData = _request.getBody();
	std::string	contentType = _request.getHeaders()["Content-Type"][0];
	std::string	uploadDir = _locationConf.getUploadDir().empty() ? \
				    _locationConf.getUploadDir() : DEFAULT_UP_DIR;
	
	static int	uploadCount = 0;

	//check for contentlength
	checkContentLength();

	//if empty upload send 201
	if( postData.empty() )
		setStatus( 201 );

	//multipart upload
	else if( contentType.find( "multipart/form-data" ) != std::string::npos ){
	
		handleMultipartUpload( contentType, _request.getBody(), uploadDir );

		setStatus( 201 );
		setBody( UPLOAD_SUCCESS );
	}

	//basic upload
	else{
		std::string	filePath = uploadDir + "upload_" + intToString( uploadCount );
		writeToFile( filePath, postData );

		setStatus( 201 );
		setBody( UPLOAD_SUCCESS );
	}
}
