#pragma once
#include <iostream>
#include <string>



namespace jun{
	enum class LOG_LEVEL{
		OFF = 0,
		FATAL,
		ERROR,
		WARN,
		INFO,
		DEBUG,
		ALL,
	};



	class Trace{

		virtual LOG_LEVEL level(){
			return LOG_LEVEL::ERROR;
		}

		virtual std::string className(){
			std::string type_name{ typeid(*this).name() };
			auto pos = type_name.find_last_of(' ');
			return type_name.substr(pos);
		}

		void info( const std::string& msg){
			if (level() >= LOG_LEVEL::INFO){
				std::cout << msg << std::endl;
			}
		}

		void debug(const std::string& msg){
			if (level() >= LOG_LEVEL::DEBUG){
				std::cout << msg << std::endl;
			}
		}

		void warning(const std::string& msg){
			if (level() >= LOG_LEVEL::WARN){
				std::cout << msg << std::endl;
			}
		}

		void error(const std::string& msg){
			if (level() >= LOG_LEVEL::ERROR){
				std::cout << msg << std::endl;
			}
		}
	};
}



