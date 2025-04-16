#include "webserv.hpp" 

void    Response::HandleRedirectRequest(HttpRequest& ReqObj) {
    // int redCode = locationConfCodeGet()

    // set req code to the first string (301, 302) of redirect vec
    ReqObj.setResponseCode(redCode)

    // i dont think we need to set a body here
    setBody("");
}