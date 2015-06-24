#include <iostream>
#include "CEvt.h"
#include "IClassifyNoti.h"
#include <assert.h>

using namespace std;

class CClassResult : public IClassifyNoti{
public:
	virtual ~CClassResult(){}

	void onNewDevice(int device, DEV_TYPE devType, int index)
	{
		cout << "New device(" << device << ") type:" << devType << endl;
	}

	void CClassResult::onKeyEvent(int device, int startIndex, int endIndex)
	{
		cout << "+Key(" << device << "): index(" << startIndex <<","<<endIndex<<")"<< endl;
	}

	void CClassResult::onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex)
	{
		cout << "+Touch(" << device << "): ";
		(type == SWIPE)? cout << "Swipe" : cout << "Tap";
		cout << "index(" << startIndex << "," << endIndex << ")" << endl;
	}

	void CClassResult::onError(ERR_CODE code)
	{
		cout << "****Error:" << code <<"****"<< endl;
	}
};

int main(int argc, char *argv[])
{
	if (argc < 3) {
		cout << "usage:eRcv <ip> <port>" << endl;
		return -1;
	}

	const char *server_host = argv[1];
	unsigned short server_port = atoi(argv[2]);
	cout << "server info(addr:" << server_host << ",port:" << server_port << ")" << endl;

	CEvt er;
	CClassResult classResult;
	er.notify(&classResult);

	int rtn;
	if ((rtn = er.open(server_host, server_port)) < 0) {
		cout << "init error(" << rtn << ")";
		return -1;
	}
	cout << "init ok(" << rtn << ")" << endl;

	char inpBuff[128];
	std::string filename;
	bool bRun = true;
	while (bRun)
	{
		cout << endl << "<-Menu: Record[r],Stop[t],Play[p],PartPlay[i],Close[x]->" << endl;
		cin >> inpBuff;
		cout << "Choice:" << inpBuff << endl;

		switch (inpBuff[0])
		{
		case 'r':
			//Record start
			if (er.recordStart() < 0){
				cout << "error record start" << endl;
				return -1;
			}
			cout << "Event record start" << endl;
			break;
		case 't':
			//Record stop
			if (er.recordStop() < 0){
				cout << "error record stop" << endl;
				return -1;
			}
			cout << "Event record stop" << endl;
			break;
		case 'p':
			cout << "filename:";
			cin >> inpBuff;
			cout << "Given filename:" << inpBuff << endl;
			cout << "Record count:" << CEvt::recordCount(inpBuff) << endl;
			int rtn;
			if ((rtn = er.play(inpBuff)) != 0) {
				cout << "Play error(" << rtn << ")" << endl;
				return -1;
			}
			cout << "Event Play" << endl;
			break;
		case 'i':
			std::cout << "filename:";
			std::cin >> inpBuff;
			filename = inpBuff;
			std::cout << "Given filename:" << filename.c_str() << std::endl;
			cout << "Record count:" << CEvt::recordCount(filename.c_str()) << endl;
			long long loc[2];
			while (true)
			{
				std::cout << std::endl << "set index as -1 to quit part play" << std::endl;
				for (int i = 0; i < 2; i++){
					std::cout << "index[" << i << "]: ";
					std::cin >> inpBuff;
					loc[i] = atol(inpBuff);
					if (loc[i] == -1) break;
				}

				if (loc[0] == -1 || loc[1] == -1) {
					std::cout << "finish part play(" << filename.c_str() << ")" << std::endl;
					break;
				}

				std::cout << "start index:" << loc[0] << ",end index:" << loc[1] << std::endl;

				if ((rtn = er.play(filename.c_str(), loc[0], loc[1])) != 0) {
					cout << "Play error(" << rtn << ")" << endl;
					return -1;
				}
				std::cout <<"play("<<filename.c_str()<<",start index:" << loc[0] << ",end index:" << loc[1] <<") done"<<std::endl;
			}
			cout << "Event Play Part" << endl;
			break;
		case 'x':
			//Terminate
			bRun = false;
			cout << "terminating..." << endl;
			er.close();
			break;
		default:
			break;
		}
	}
	std::cout << "end" << std::endl;
	return 0;
}