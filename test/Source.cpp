#include "test_set.h"
#include <iostream>
#include <string>
#include "spdlog\spdlog.h"

using namespace std;



int main(){
	try{
		auto console = spdlog::stdout_logger_mt("console", true);
		console->info("Welcome to spdlog");
		console->info("An info message example {}..", 1);

		auto my_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic.txt");
		my_logger->info("Some log message {} {} ,{}",3,2,"three");


	}
	catch (spdlog::spdlog_ex& ex){
		cout << "log failed: " << ex.what() << endl;
	}
	catch (...){
		cout << "other exception";
		throw;
	}
	
	getchar();
	return 0;
}