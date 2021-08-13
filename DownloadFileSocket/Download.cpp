#include "Download.h"
#include <windows.h>
#include <fstream>
#include <iterator>
#pragma warning( disable : 4996)
#pragma comment(lib, "ws2_32.lib")

strData Download::PushOneConnection(int connection_count, int numthread)
{
	strData datathread;
	datathread.numthread = numthread;

	SOCKET connectsock = INVALID_SOCKET;
	connectsock = InitSocket();
	string strRange = get_range(connection_count, numthread);

	string gethttprequest = Request + strRange + "\r\n\r\n";
	send(connectsock, gethttprequest.c_str(), strlen(gethttprequest.c_str()), 0);
	
	bool done = false;
	bool body = false;
	bool endheader = false;
	char buffer;
	int iResult;
	while (!done)
	{
		iResult = recv(connectsock, &buffer, 1, 0);
		if (iResult <= 0)
			done = true;

		switch (buffer)
		{
		case '\r':
			break;
		case '\n': // "\n\n" OR "\n\r\n" are End Headers
			if (endheader == true)
				body = true;

			endheader = true;
			break;
		default:
			endheader = false;
			break;
		}

		if(body)
		 datathread.Data.push_back(buffer);
	}
	
	closesocket(connectsock);
	
	datathread.Data.erase(datathread.Data.begin());
	return datathread;
}

bool Download::add_data_in_map_into_file()
{
	ofstream filedata;
	filedata.open(strOutput, ios::app | ios::binary | ios::out);
	map<int, string>::iterator itr;
	for (itr = mapData.begin(); itr != mapData.end(); ++itr)
	{
		if (itr->first == inTotalConnectDataIntoFile)
		{
			filedata << itr->second;
			mapData.erase(itr);
			inTotalConnectDataIntoFile++;
		}
		else if (itr->first > inTotalConnectDataIntoFile)
			break;
	}
	filedata.close();

	return true;
}

string Download::get_range(int connection_count, int numthead)
{
	int inStart = (((int)douFilesizeSV - (int)douFilesizeLC) / connection_count * numthead) + numthead;
	if (douFilesizeLC >= 0)
		inStart += (int)douFilesizeLC;
	int inEnd = inStart + ((int)douFilesizeSV - (int)douFilesizeLC) / connection_count;
	string strStart = to_string(inStart);
	string strEnd = to_string(inEnd);
	if (connection_count == numthead + 1)
		strEnd = to_string((int)douFilesizeSV - 1);
	string strRange = "Range: bytes=" + strStart + "-" + strEnd;
	return strRange;
}

Download::Download() :
	douFilesizeSV(0.0), douFilesizeLC(0.0), inTotalConnectDataIntoFile(0), inTotalThreadsRunning(0)
{
}

Download::~Download()
{
}

int Download::WinsockInitialize()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	int iResult = WSAStartup(wVersionRequested, &wsaData);
    if (iResult != 0)
        return 1;

	return 0;
}

int Download::SetNameFile(string url)
{
	size_t found = url.find_last_of("/");
	if (found == -1)
	{
		return 1;
	}
	strFilename = url.substr(found + 1);
	return 0;
}

int Download::SetRequest(string url)
{
	size_t found = url.find("https://");
	Port = 443;
	if (found == string::npos)
	{
		found = url.find("http://");
		Port = 80;
		if (found == string::npos)
			return 1;
		else
			found = sizeof("http://");
	}
	else
	{
		found = sizeof("https://");
	}

	string tempUrl = url.substr(found - 1);
	found = tempUrl.find_first_of("/");
	if (found == -1)
		return 1;

	string strHost = tempUrl.substr(0, found);
	bool checkempty = strHost.empty();
	if (checkempty)
		return 1;

	string strRequestLine = tempUrl.substr(found);
	checkempty = strRequestLine.empty();
	if (checkempty)
		return 1;

	Host = strHost;
	Request = "GET " + strRequestLine + " HTTP/1.0\r\nHost: " + strHost + "\r\n";

	return 0;
}

SOCKET Download::InitSocket()
{
	SOCKET connectsock = INVALID_SOCKET;
	connectsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectsock == INVALID_SOCKET)
	{
		closesocket(connectsock);
		return INVALID_SOCKET;
	}
		
	struct hostent* host;
	host = gethostbyname(Host.c_str());
	if (host == NULL)
	{
		closesocket(connectsock);
		return INVALID_SOCKET;
	}

	SOCKADDR_IN clientService;
	clientService.sin_port = htons(Port);
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	int iResult = connect(connectsock, (SOCKADDR*)&clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR)
	{
		closesocket(connectsock);
		return INVALID_SOCKET;
	}

	return connectsock;
}


int Download::Initialize(string url)
{
	int iResult = WinsockInitialize();
	if (iResult != 0)
		return 1;
	
	iResult = SetRequest(url);
	if (iResult != 0)
		return 1;

	iResult = SetNameFile(url);
	if (iResult != 0)
		return 1;

	return 0;
}

int Download::SetSizeFileSV()
{
	SOCKET connectsock = INVALID_SOCKET;
	connectsock = InitSocket();
	if (connectsock == INVALID_SOCKET)
		return 1;

	string gethttprequest = Request + "\r\n";
	int iResult = send(connectsock, gethttprequest.c_str(), strlen(gethttprequest.c_str()), 0);

	if (iResult == SOCKET_ERROR) 
	{	
		closesocket(connectsock);
		return 1;
	}

	char buffer;
	string header;
	bool endheader = false;
	bool done = false;
	while (!done)
	{
		iResult = recv(connectsock, &buffer, 1, 0);
		if (iResult <= 0)
			done = true;

		switch (buffer) 
		{
			case '\r':
				break;
			case '\n': // "\n\n" OR "\n\r\n" are End Headers
				if (endheader == true)
					done = true;

				endheader = true;
				break;
			default:
				endheader = false;
				break;
		}

		header.push_back(buffer);	
	}

	size_t ffound = header.find("Content-Length:");
	size_t efound = header.find("\r", ffound);
	size_t sizedata = efound - sizeof("Content-Length:") - ffound;
	string lenght = header.substr(ffound + sizeof("Content-Length:"), sizedata);
	
	douFilesizeSV = atoi(lenght.c_str());
	closesocket(connectsock);
	return 0;
}

bool Download::check_size_file_lc(string link)
{
	set_link_output(link);
	streampos begin, end;
	ifstream filelc(strOutput, ios::binary);
	begin = filelc.tellg();
	filelc.seekg(0, ios::end);
	end = filelc.tellg();
	filelc.close();
	douFilesizeLC = end - begin;
	if (douFilesizeLC >= douFilesizeSV)
	{
		remove(strOutput.c_str());
		douFilesizeLC = 0;
	}
	return true;
}

void Download::StartDownload(int Connection_count, int Thread_count)
{
	for (int i = 0; i < Connection_count; i++)
	{
		while (1)
		{
			if (!check_threads_running(Thread_count))
			{
				check_data_finished(Connection_count);
				break;
			}
		}

		listThreads.push_back(async(launch::async, &Download::PushOneConnection, this, Connection_count, i));
	}
	while (1)
	{
		if (!check_data_finished(Connection_count))
			break;
	}
	return;
}

void Download::Finalize()
{
	WSACleanup();
	return;
}


bool Download::set_link_output(string link)
{
	strOutput = link;
	return true;
}

bool Download::check_threads_running(int Thread_count)
{
	if (Thread_count > inTotalThreadsRunning)
	{
		inTotalThreadsRunning++;
		return false;
	}
	else
	{
		list<future<strData>>::iterator itr;
		chrono::milliseconds span(0);
		for (itr = listThreads.begin(); itr != listThreads.end(); ++itr)
		{
			if (itr->wait_for(span) == future_status::ready)
				return false;
		}
	}
	return true;
}

bool Download::check_data_finished(int Connection_count)
{
	list<future<strData>>::iterator itr;
	chrono::milliseconds span(0);
	for (itr = listThreads.begin(); itr != listThreads.end(); itr++)
	{
		int size = listThreads.size();
		if (size == 0)
			return true;

		if (itr->wait_for(span) == future_status::ready)
		{
			strData datathread = itr->get();
			mapData.insert(pair<int, string>(datathread.numthread, datathread.Data));
			listThreads.erase(itr);
			add_data_in_map_into_file();	
		}
	}
	if (inTotalConnectDataIntoFile == Connection_count)
		return false;

	return true;
}