//#include "../inc/Response.hpp"
#include "webserv.hpp"
#include <sstream>
#include <ctime>

Response::Response(void){}

std::string getCurrentDateTime() {
    std::time_t now = std::time(0);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}

Response::Response(HttpRequest &reqObj){
    try{
		if(reqObj.getResponseCode() != 200){
			std::cout << "error";
			throw reqObj.getResponseCode();
		}
        processResponse(reqObj);
    }
    catch(int errror)
    {
		std::cerr << "Caught error: "<< std::endl;
		generateErrorResponse(reqObj);
		//make error header look for custom errror sites if none genarate one
    }
}

// Mandatory Fields:






void	Response::generateStatusLine(HttpRequest &reqObj){
	std::string httpVersion = reqObj.getVersion();
	std::string statusCode = Response::intToString(reqObj.getResponseCode());
	std::string reasonPhrase;
	switch (reqObj.getResponseCode()){
		case 200:
			reasonPhrase = "OK";
			break;
		case 404:
            reasonPhrase = "Not Found";
            break;
        case 500:
            reasonPhrase = "Internal Server Error";
            break;
        // Add more status codes and reason phrases as needed
        default:
            reasonPhrase = "Error";
            break;
    }
	std::stringstream statusLine;
    statusLine << httpVersion << " " << statusCode << " " << reasonPhrase << "\r\n";
    setStatusLine(statusLine.str());

}

std::string getServerName(){
	std::string server;
	//if()is set in conf

	//else set to default value and return
	server = "Webserv/1.0";
	return(server);
}

void	Response::generateHeader(HttpRequest &reqObj){
	generateStatusLine(reqObj);

	std::map<std::string, std::string> headerMap;
	headerMap["Status-Line"] = getStatusLine();
    headerMap["Date"] = getCurrentDateTime();
	headerMap["Server"] = getServerName();
	//headerMap["Content-Type"] = contentType; Type = get from allec or build function that gets from file ending like .jpeg
	headerMap["Content-Length"] =  intToString(getBody().length());
	headerMap["Connection"] = "keep alive";//example value set to this default but in reguest could ask for close

	// Optional Fields
    headerMap["Cache-Control"] = "no-cache"; // Example value
    headerMap["Set-Cookie"] = "sessionId=abc123; Path=/; HttpOnly"; // Example value
    headerMap["Last-Modified"] = getCurrentDateTime(); // Example value
    headerMap["ETag"] = "\"123456789\""; // Example value
    headerMap["Location"] = "/new-resource"; // Example value

	setHeaderMap(headerMap);

	//for (std::map<std::string, std::string>::const_iterator it = _headerMap.begin(); it != _headerMap.end(); ++it) {
    //    std::cout << it->first << ": " << it->second << std::endl;
    //}
	std::stringstream header;
    header << headerMap["Status-Line"] << "\r\n";
    header << "Date: " << headerMap["Date"] << "\r\n";
    header << "Server: " << headerMap["Server"] << "\r\n";
    header << "Content-Type: " << headerMap["Content-Type"] << "\r\n";
    header << "Content-Length: " << headerMap["Content-Length"] << "\r\n";
    header << "Connection: " << headerMap["Connection"] << "\r\n";
    header << "Cache-Control: " << headerMap["Cache-Control"] << "\r\n";
    header << "Set-Cookie: " << headerMap["Set-Cookie"] << "\r\n";
    header << "Last-Modified: " << headerMap["Last-Modified"] << "\r\n";
    header << "ETag: " << headerMap["ETag"] << "\r\n";
    header << "Location: " << headerMap["Location"] << "\r\n";
    header << "\r\n"; // End of headers

	std::string responseString = header.str() + getBody();
	setHttpResponse(responseString);
}



void Response::generateErrorResponse(HttpRequest &reqObj){
	//check for custom error page saved by moritz somewhere
	//if ittarate thrugg list found custom page
    // else {

		// Generate a default error page if custom error page is not found
        std::string defaultErrorPage =  "<html><body><h1>" + Response::intToString(reqObj.getResponseCode()) + " Error</h1></body></html>" + "\n";
        setBody(defaultErrorPage);
		generateHeader(reqObj);
}


void		Response::processResponse(HttpRequest &ReqObj){
    std::cout << std::endl << "IN RESPONSE PROCESSING" <<std::endl << std::endl;
    try{
        //try get body from file and send
        if(ReqObj.getMethod() == "GET"){
            std::cout << "GET REQUEST" << std::endl;
           // Response.set
            //check dir or file
            //check available
            //check permissions
            //count len and see if chuncking is needed

        }

        if(ReqObj.getMethod() == "POST"){
            std::cout << "POST REQUEST" << std::endl;

        }


        if(ReqObj.getMethod() == "DELETE"){
            std::cout << "DELETE REQUEST" << std::endl;

        }



    }
    catch(int error)
    {
        //add the error handeling here
        //make error body and send that string to client
    }

    //return the string i gonna send to the client
}

std::string Response::intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}
