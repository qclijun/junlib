// ÖÐÎÄ

#define	SPDLOG_DEBUG_ON

#include "test_set.h"
#include <iostream>
#include <string>
#include <algorithm>
//#include "spdlog\spdlog.h"
#include "junlib\string_alg.h"
#include "junlib\files.h"
#include "junlib\util.h"
//#include "junlib\spdlog\mylogger.h"
#include "fmt\ostream.h"
using namespace std;

namespace aa{
	struct AA{
		int i;
		AA(int xx) :i(xx){}
	};




	void swap1(AA& a, AA& b){
		cout << "aa::swap1(AA&, AA&)" << endl;
		int tmp = a.i;
		a.i = b.i;
		b.i = tmp;
	}

	template<typename T>
	struct TT{
		T t;
	};

	//template<typename T>
	//void swap1(TT<T>& t1, TT<T>& t2){
	//	cout << "aa::swap1<T>(TT&, TT&)" << endl;
	//	T tmp = t1.t;
	//	t1.t = t2.t;
	//	t2.t = tmp;
	//}

}

namespace jun{
	template<typename T>
	void swap1(T& t1, T& t2){
		cout << "jun::swap1(T&, T&)" << endl;
		T tmp(t1);
		t1 = t2;
		t2 = tmp;
	}


	template<typename T>
	void swap1(aa::TT<T>& t1, aa::TT<T>& t2){
		cout << "aa::swap1<T>(TT&, TT&)" << endl;
		T tmp = t1.t;
		t1.t = t2.t;
		t2.t = tmp;
	}

	template<typename T>
	void call_swap(T& t1, T& t2){
		swap1(t1, t2);
	}
}

inline ostream& operator<<(ostream& out, const aa::AA& aa){
	return out << aa.i << " ";
}

int main(){

	aa::AA abc(3);

	string s = fmt::format("ssdf {}", abc);
	
	
	//test_set();
	
	//try{
	//	auto console = spdlog::stdout_logger_mt("console", false);
	//	console->info("Welcome to spdlog");
	//	console->info("An info message example {}..", 1);

	//	auto my_logger = spdlog::basic_logger_mt("basic_logger", "basic.txt");
	//	my_logger->info("Some log message {} {} ,{}",3,2,"three");

	//	auto my_logger2 = spdlog::get("console");
	//	my_logger2->info("error {2}, {1}", 3, 4,5);

	//} Íí»á
	//catch (spdlog::spdlog_ex& ex){
	//	cout << "log failed: " << ex.what() << endl;
	//}
	//catch (...){
	//	cout << "other exception";
	//	throw;
	//}
	//vector<string> lines = jun::files::read_all_lines("Source.cpp");

	//std::locale l = std::locale();
	//cout << wcin.getloc().name() << endl;
	//std::for_each(begin(lines), end(lines), [](const string& line){cout << line << endl; });


	//string allbytes = jun::files::read_all_bytes("Source.cpp");
	//cout << allbytes << endl;

	//mylogger::spdlog_ptr root_logger = nullptr;

	////try{
	//root_logger = mylogger::root_logger();
	//}
	//catch (mylogger::logger_config_failure& e){
	//	cerr <<typeid(e).name()<<" "<< e.what() << endl;
	//	throw;
	//}

	//root_logger->info("info message");
	//root_logger->debug("debug message");
	//SPDLOG_DEBUG(root_logger, "only SPDLOG_TRACE_ON {} {}", 3, 4);

	

	getchar();
	return 0;
}