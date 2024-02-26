#ifndef ARGP_HPP
#define ARGP_HPP

#include <cstring>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <cassert>

namespace argp {

	class sstream {
	private:
		std::stringstream stream;
	
	public:
		sstream() {};
		
		sstream(std::string str) {
			stream << str;
		};

		sstream& operator >> (std::string& str) {
			this->stream >> str;
			return *this;
		};
	};

	enum parser_option { SINGLE_DASH_IS_MULTIFLAG = 0, SINGLE_DASH_IS_SINGLE_FLAG = 1};

	class parser {
	private:
		std::map<std::string, std::string> parameters;
		std::set<std::string> options;

	public:
		
		parser() {};
		
		inline bool parse(int argc, char *argv[], int option=SINGLE_DASH_IS_MULTIFLAG) {
			char* token;
			int argc_length;
			
			if (argc < 2)
				return true;
			
			for ( int i=1; i<argc; i++ ) {
				token = argv[i];
				argc_length = strlen(token);
				
				if ( token[0] != '-' && argc_length > 1 ) {
					continue;
				}

				if ( i+1 == argc || argv[i+1][0] == '-') {											// OPTION
					if ( token[1] == '-' )
						goto two_dashes;

					if ( option == SINGLE_DASH_IS_MULTIFLAG )
						goto multiflag_parser;

					assert( argc_length == 2 );
					this->options.emplace( "-" + std::string (1,  token[1] ) );
					goto done;
					
					multiflag_parser:
					for ( int j=1; j<argc_length; j++) {
						this->options.emplace( "-" + std::string(1,  token[j] ) );
					}
					goto done;
					
					two_dashes:
					assert( argc_length > 2 && token[2] != '-' );
					this->options.emplace( std::string( token ) );
					goto done;

					done:
					continue;
				}
				assert( ( argc_length == 2 && token[1]  != '-' ) || ( argc_length > 2 && token[1] == '-' && token[2] != '-' ) );

				this->parameters[ token ] = argv[i+1];												// PARAMETER
				i++;	
			}
			return true;
		};

		inline sstream operator()(std::initializer_list<std::string> list) const {
			for ( std::initializer_list<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
				if (this->parameters.find(std::string(*it)) != this->parameters.end())
					return sstream(this->parameters.find(std::string(*it))->second);
			}
			return sstream();
		};

		inline sstream operator()(std::string element) const {
			if (this->parameters.find(std::string(element)) != this->parameters.end())
				return sstream(this->parameters.find(std::string(element))->second);
			return sstream();
		};


		inline bool operator[](std::initializer_list<std::string> list) const {
			for ( std::initializer_list<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
				if (this->options.find(*it) != this->options.end())
					return true;		
			}
			return false;
		};
		
		inline bool operator[](std::string element) const {
			return this->options.find(element) != this->options.end();
		};

		inline void summary() {
			std::cout << "Options: ";
			for ( std::set<std::string>::iterator it = this->options.begin(); it != this->options.end(); it++ ) {
				std::cout << *it << " ";
			}
			
			std::cout << std::endl << "Parameters: ";
			
			for ( std::map<std::string, std::string>::iterator it2 = this->parameters.begin(); it2 != this->parameters.end(); it2++ ) {
				std::cout << it2->first << " " << it2->second << " ";
			}
			std::cout << std::endl;
		};
	};
};

#endif
