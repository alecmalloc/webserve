#include "webserv.hpp"

// templates for cookie html
std::string static cookiesHtmlTemplateHasCookie(std::string sessionID) {    
    std::string response;

    response += "<h1>logged in</h1>";
    
    // interpolate sessionID into h1
    response += "<p>session ID: " + sessionID + "</p>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage/deactivate\">log out</a>";

    response += "<a href=\"/\">home</a>";

    response += CSS_GLOBAL;

    return response;
}

std::string static cookiesHtmlTemplateHasNoCookie() {
    std::string response;

    response += "<h1>not logged in</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage/activate\">log in</a>";

    response += "<a href=\"/\">home</a>";

    response += CSS_GLOBAL;

    return response;
}

std::string static cookiesHtmlCookiesHaveBeenGiven() {
    std::string response;

    response += "<h1>you have recieved a cookie and been logged in</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage\">back</a>";

    response += CSS_GLOBAL;

    //std::cout << "has no cookie" << '\n';
    return response;
}

std::string static cookiesHtmlCookiesHaveBeenTaken() {
    std::string response;

    response += "<h1>you have been logged out</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage\">back</a>";

    response += CSS_GLOBAL;

    //std::cout << "has no cookie" << '\n';
    return response;
}

// generate a session ID randomly
std::string generateSessionID(std::size_t length = 32) {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    // seed the random number generator if it hasn't been seeded yet
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(time(NULL)));
        seeded = true;
    }
    
    std::string sessionID;
    for (std::size_t i = 0; i < length; ++i) {
        int index = rand() % CHARACTERS.size();
        sessionID += CHARACTERS[index];
    }
    return sessionID;
}

// add cookie to headers (headers are generated with the ones we have so changing headers in request effectively changes them in response too)
// not renaming but it also creates session
void Response::createSession( void ) {
    // generate session ID and save file named after session ID
    // save file in root+/sessions/SESSIONID
    std::string sessionFilePath = _serverConf.getRootDir() + "/session/";
    std::string sessionID = generateSessionID();

    sessionFilePath += sessionID;
    //  std::cout << sessionFilePath << '\n';
    std::ofstream outFile(sessionFilePath.c_str());

    // check if file opened successfully
    if (outFile.is_open()) {
        // write data to the file
        outFile << "session ID muahahaha" << '\n';
        outFile.close();
    }
    setSetCookieValue("NUM_cookie=" + sessionID + "; Path=/; HttpOnly");
}

// deletes session file and sets cookie to NO
void Response::deleteSession( std::string sessionID ) {
    
    // full path to session file
    std::string sessionFilePath = _serverConf.getRootDir() + "/session/" + sessionID;

    // delete the session file
    // if this fails its not the end of the world but we should print smt
    // just means that we have an extra file in /sessions/ -> but wont harm operations
    if (std::remove(sessionFilePath.c_str()) != 0) {
        std::cerr << "Error deleting session file: " << sessionFilePath << std::endl;
    }

    setSetCookieValue("NUM_cookie=NO; Path=/; HttpOnly");
}

std::string Response::findSession(std::vector<std::string> cookies) {
    std::string defaultState = "none";

    // folder in which sessions are stored
    std::string sessionFilePath = _serverConf.getRootDir() + "/session/";

    // find the cookie that contains "NUM_cookie="
    for (std::vector<std::string>::iterator it = cookies.begin(); it != cookies.end(); ++it) {
        std::string cookie = *it;
        
        // beginning of cookie we are trying to find
        size_t pos = cookie.find("NUM_cookie=");
        
        // if we can find our cookie
        if (pos != std::string::npos) {
            // extract the session ID value after "NUM_cookie="
            std::string sessionID = cookie.substr(pos + 11); // 11 is the length of "NUM_cookie="

            // check if the session file exists
            std::string filePath = sessionFilePath + sessionID;
            // open session file
            std::ifstream fileCheck(filePath.c_str());
            bool fileExists = fileCheck.good();
            fileCheck.close();
            if (fileExists) {
                return sessionID;
            }
        }
    }

    // default case
    return defaultState;
}

void Response::handleCookiesPage(HttpRequest& request) { 
    // check for activate hook before handling any other state or cookie checking
    // doesnt need any cookies etc just a clean start
    if (request.getUri() == "/customCookiesEndpoint/CookiesPage/activate") {
        createSession();
        setBody(cookiesHtmlCookiesHaveBeenGiven());
        return;
    }
    
    // get headers from request
    std::map<std::string, std::vector<std::string> > headers = request.getHeaders();

    // check if cookies even exists at all
    // return and set to no cookie html template if none are found
    if (headers.find("Cookie") == headers.end()) {
        setBody(cookiesHtmlTemplateHasNoCookie());
        return;
    }

    // get cookies vector from headers
    std::vector<std::string> cookies = headers["Cookie"];

    // SESSION MANAGEMENT
    // see if there is an active session (session file in sessions with matched cookie value NUM_VAL=) -> default none
    // only needed for scope of this function
    // need to do this before custom handlers because we need sessionID for deactivation and cleanup
    std::string activeSessionID = findSession(cookies);
    // std::cout << "activeSession: " << activeSessionID << '\n';

    // check if deactivate hook has been triggered
    if (request.getUri() == "/customCookiesEndpoint/CookiesPage/deactivate") {
        // TODO clean up session FILE -> delete from sessions
        deleteSession(activeSessionID);
        setBody(cookiesHtmlCookiesHaveBeenTaken());
        return;
    }

    // COOKIE PAGE DEFAULT
    // if there is an active session that matches with cookie etc -> state of activeSessionID is not none
    if (activeSessionID != "none") {
        setBody(cookiesHtmlTemplateHasCookie(activeSessionID));
        return;
    }
    
    // no session exists
    setBody(cookiesHtmlTemplateHasNoCookie());
}