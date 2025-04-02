#include "webserv.hpp"

static const std::string COOKIE_PAGE_STYLE = "<style>body{font-family:sans-serif;text-align:center;margin:40px auto;max-width:600px;background:#f8f8f8;}h1{color:#333;font-size:24px;margin-bottom:30px;}a{display:inline-block;background:#4285f4;color:#fff;padding:10px 20px;text-decoration:none;border-radius:4px;font-weight:bold;}a:hover{background:#3367d6;}</style>";

// templates for cookie html
std::string static cookiesHtmlTemplateHasCookie(std::string sessionID) {    
    std::string response;

    response += "<h1>logged in</h1>";
    
    // interpolate sessionID into h1
    response += "<p>session ID: " + sessionID + "</p>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage/deactivate\">log out</a>";

    response += COOKIE_PAGE_STYLE;

    return response;
}

std::string static cookiesHtmlTemplateHasNoCookie() {
    std::string response;

    response += "<h1>not logged in</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage/activate\">log in</a>";

    response += COOKIE_PAGE_STYLE;

    return response;
}

std::string static cookiesHtmlCookiesHaveBeenGiven() {
    std::string response;

    response += "<h1>you have recieved a cookie and been logged in</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage\">back</a>";

    response += COOKIE_PAGE_STYLE;

    //std::cout << "has no cookie" << '\n';
    return response;
}

std::string static cookiesHtmlCookiesHaveBeenTaken() {
    std::string response;

    response += "<h1>you have been logged out</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage\">back</a>";

    response += COOKIE_PAGE_STYLE;

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
void Response::giveCookie( void ) {
    // generate session ID and save file named after session ID
    // save file in root+/sessions/SESSIONID
    std::string sessionFilePath = _serverConf->getRootDir() + "/session/";
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

// remove cookies from headers
void Response::takeCookie( void ) {
    setSetCookieValue("NUM_cookie=NO; Path=/; HttpOnly");
}

std::string Response::findSession(std::vector<std::string> cookies) {
    // folder in which sessions are stored
    std::string sessionFilePath = _serverConf->getRootDir() + "/session/";

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
    return "invalid";
}

void Response::handleCookiesPage(HttpRequest& request) {
    // TODO

    // get headers from request
    std::map<std::string, std::vector<std::string> > headers = request.getHeaders();

    // check if we need to do any cookie actions
    if (request.getUri() == "/customCookiesEndpoint/CookiesPage/activate") {
        giveCookie();
        setBody(cookiesHtmlCookiesHaveBeenGiven());
        return;
    }
    if (request.getUri() == "/customCookiesEndpoint/CookiesPage/deactivate") {
        // TODO clean up session FILE -> delete from sessions
        takeCookie();
        setBody(cookiesHtmlCookiesHaveBeenTaken());
        return;
    }

    // check if cookies even exists at all
    if (headers.find("Cookie") == headers.end()) {
        setBody(cookiesHtmlTemplateHasNoCookie());
        return;
    }

    // get cookies vector from headers
    std::vector<std::string> cookies = headers["Cookie"];

    bool hasOurCookie = false;
    for (std::vector<std::string>::iterator it = cookies.begin(); it != cookies.end(); ++it) {
        if (it->find("NUM_cookie=") != std::string::npos) {
            hasOurCookie = true;
            break;
        }
    }

    // check if our cookie can be found
    if (hasOurCookie) {
        // checks if a session file exists returns invalid if no or if it doesnt match the cookie
        std::string sessionID = findSession(cookies);
        // std::cout << "SESSION ID: " << sessionID << '\n';
        // if we find a valid session we set body to cookie page and return
        if (sessionID != "invalid") {
            setBody(cookiesHtmlTemplateHasCookie(sessionID));
            return;
        }
    }

    // cookies existed but not ours
    setBody(cookiesHtmlTemplateHasNoCookie());
}