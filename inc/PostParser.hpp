#ifndef POST_PARSER_HPP
#define POST_PARSER_HPP

#include "webserv.hpp"

class PostParser {
	private:
		struct FormPart {
			std::string name;
			std::string filename;
			std::string contentType;
			std::string content;
		};
	
		std::string boundary;
		std::vector<FormPart> parts;
		
		// Helper methods
		std::string urlDecode(const std::string& encoded);
		bool parseMultipart(const std::string& data);
		bool parseUrlEncoded(const std::string& data);
	
	public:
		explicit PostParser(const std::string& boundary = "");
		bool parse(const std::string& data, const std::string& contentType);
		const std::vector<FormPart>& getParts() const;
	};

#endif