#include"DownloadManager.h"

int main()
{
	string url;
	int connect_count;
	int thread_count;
	string out;
	DownloadManager tool;
	cout << "getfile ";
	/*cin >> url;*/
	url = "https://phunugioi.com/wp-content/uploads/2020/03/hinh-anh-gai-xinh-han-quoc-hot-girl-de-thuong.jpeg";
	cout << "--connection-count=";
	cin >> connect_count;
	cout << "--thread-count=";
	cin >> thread_count;
	cout << "--output=";
	/*cin >> out;*/
	out = "C:\\Users\\Admin\\Desktop\\DownloadFileSocket\\DownloadFileSocket\\";
	tool.set_valid(url, out, connect_count, thread_count);
	tool.app_main();

	return 0;
}