#include "DownloadManager.h"
#include "Download.h"

DownloadManager::DownloadManager() :
	inConnection_count(1), inThread_count(1)
{
}

DownloadManager::~DownloadManager()
{
}

int DownloadManager::set_valid(string url, string out, int connection_count, int thread_count)
{
	strUrl = url;
	size_t found = strUrl.find_last_of("/");
	string strFilename = strUrl.substr(found + 1);
	strlinkoutput = out + strFilename;
	inConnection_count = connection_count;
	inThread_count = thread_count;
    return 0;
}

void DownloadManager::app_main()
{
	Download manager;
	manager.Initialize(strUrl);
	manager.SetSizeFileSV();
	manager.check_size_file_lc(strlinkoutput);
	manager.StartDownload(inConnection_count, inThread_count);
	manager.Finalize();
	return;
}
