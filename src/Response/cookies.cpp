#include "webserv.hpp"

// templates for cookie html
std::string static cookiesHtmlTemplateHasCookie() {
    std::string response;

    response += "<h1>NUM NUM NUM. thank you for using our cookie</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage/deactivate\">eat your cookie!</a>";

    //std::cout << "has cookie" << '\n';
    return response;
}

std::string static cookiesHtmlTemplateHasNoCookie() {
    std::string response;

    response += "<h1>damn bro you aren't using our cookie ...</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage/activate\">grab a cookie!</a>";

    //std::cout << "has no cookie" << '\n';
    return response;
}

std::string static cookiesHtmlCookiesHaveBeenGiven() {
    std::string response;

    response += "<h1>you got a cookie!</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage\">return to cookies page</a>";

    //std::cout << "has no cookie" << '\n';
    return response;
}

std::string static cookiesHtmlCookiesHaveBeenTaken() {
    std::string response;

    response += "<h1>we stole your cookie...</h1>";

    response += "<a href=\"/customCookiesEndpoint/CookiesPage\">return to cookies page</a>";

    //std::cout << "has no cookie" << '\n';
    return response;
}

// add cookie to headers (headers are generated with the ones we have so changing headers in request effectively changes them in response too)
void Response::giveCookie( void ) {
    setSetCookieValue("NUM_cookie=YES; Path=/; HttpOnly");
}

// remove cookies from headers
void Response::takeCookie( void ) {
    setSetCookieValue("NUM_cookie=NO; Path=/; HttpOnly");
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

    // check if our cookie can be found
    if (std::find(cookies.begin(), cookies.end(), "NUM_cookie=YES") != cookies.end()) {
        setBody(cookiesHtmlTemplateHasCookie());
        return;
    }

    // cookies existed but not ours
    setBody(cookiesHtmlTemplateHasNoCookie());

}