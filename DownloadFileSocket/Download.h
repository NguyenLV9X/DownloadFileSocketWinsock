#include<iostream>
#include<map>
#include<string>
#include<list>
#include<future>
#include<winsock2.h>
using namespace std;

struct strData
{
	string Data;
	int numthread;
};

class Download
{
private:

	list<future<strData>> listThreads;
	map<int, string> mapData;
	double douFilesizeSV;
	double douFilesizeLC;
	string strFilename;
	string strOutput;
	int inTotalConnectDataIntoFile;
	int inTotalThreadsRunning;
	string get_range(int connection_count, int numthead);
	
	bool add_data_in_map_into_file();
	bool set_link_output(string link);
	bool check_threads_running(int Thread_count);
	bool check_data_finished(int Connection_count);

	string Request;
	string Host;
	u_short Port;
	int WinsockInitialize();
	int SetNameFile(string url);
	int SetRequest(string url);
	SOCKET InitSocket();
	strData PushOneConnection(int Connection_count, int numthread);

public:

	Download();
	~Download();

	
	int Initialize(string url);
	int SetSizeFileSV();
	bool check_size_file_lc(string link);
	void StartDownload(int Connection_count, int Thread_count);
	void Finalize();

};
