#include <iostream>
#include "CEvt.h"
#include "IClassifyNoti.h"
#include "CFileInfo.h"
#include <stdio.h>
#include <assert.h>

using namespace std;

class CClassResult : public IClassifyNoti{
public:
	virtual ~CClassResult(){}

	void onNewDevice(int device, DEV_TYPE devType, int index)
	{
		cout << "New device(" << device << ") type:" << devType << endl;
	}

	void onKeyEvent(int device, int code, int value, int startIndex, int endIndex)
	{
		cout << "+Key(" << device << "): code:"<<code<<" value:"<<value<<" index(" << startIndex <<","<<endIndex<<")"<< endl;
	}

	void onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex)
	{
		cout << "+Touch(" << device << "): ";
		(type == SWIPE)? cout << "Swipe" : cout << "Tap";
		cout << "index(" << startIndex << "," << endIndex << ")" << endl;
	}

	void onError(ERR_CODE code)
	{
		cout << "****Error:" << code <<"****"<< endl;
	}
};

int fileInfo(const char *file, CResult *result)
{
	return CFileInfo::analyze(file, result);
}

int dispFileInfo(char *buffer)
{
	cout << "filename:";
	cin >> buffer;
	cout << "Given filename:" << buffer << endl;
	cout << "Record count:" << CEvt::recordCount(buffer) << endl;

	int rtn;
	CResult result;
	if ((rtn = fileInfo(buffer, &result)) < 0) {
		cout << "Error(" << rtn << ") in fileInfo(" << buffer << ")" << endl;
		return -1;
	}
	cout << "User Events :" << result.list().size() << endl;

	char* touchType[] = { "TAP", "SWIPE", "MULTITOUCH" };
	std::string name(buffer);
	name += ".inf";
	FILE *fp = fopen(name.c_str(),"w");
	if (!fp) return -2;
	for (UEVT_LIST::iterator it = result.list().begin(); it != result.list().end(); it++) {
		int rtn = fprintf(fp, "%s(%d) %d %d", (it->_devType == IClassifyNoti::KEY) ? "Key" : "Touch", it->_device,it->_startIndex,it->_endIndex);
		if (rtn <= 0) return -3;

		if (it->_tchType != IClassifyNoti::INVALID_TOUCH){
			rtn = fprintf(fp, " %s", touchType[it->_tchType]);
			if (rtn <= 0) return -4;
		}
		rtn = fprintf(fp, "\n");
		if (rtn <= 0) return -5;
	}
	fclose(fp);
	return 0;
}

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
		cout << endl << "<-Menu: Record[r],Stop[t],Play[p],PartPlay[i],StepPlay[s],Close[x]->" << endl;
		cin >> inpBuff;
		cout << "Choice:" << inpBuff << endl;

		switch (inpBuff[0])
		{
		case 'r':
			//Record start
			if (er.recordStart(&classResult, "nonAceClient_event.bin") < 0){
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
			if ((rtn=dispFileInfo(inpBuff)) < 0) {
				cout << "Error(" << rtn << ") in dispFileInfo(" << inpBuff << ")" << endl;
				return -1;
			}

			if ((rtn = er.play(inpBuff)) != 0) {
				cout << "Play error(" << rtn << ")" << endl;
				return -1;
			}
			cout << "Event Play" << endl;
			break;
		case 'i':
			if ((rtn = dispFileInfo(inpBuff)) < 0) {
				cout << "Error(" << rtn << ") in dispFileInfo(" << inpBuff << ")" << endl;
				return -1;
			}
			filename = inpBuff;

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
		case 's':
			cout << "Event Play Step chosen" << endl;
			if ((rtn = dispFileInfo(inpBuff)) < 0) {
				cout << "Error(" << rtn << ") in dispFileInfo(" << inpBuff << ")" << endl;
				return -1;
			}
			filename = inpBuff;

			if ((rtn = er.playFirst(filename.c_str())) != 0) {
				cout << "PlayFirst error(" << rtn << ")" << endl;
				return -1;
			}
			{
				long evtIdx = 1;
				while (true){
					std::cout << "[" << evtIdx << "] Play next[n] or escape[q]:";
					std::cin >> inpBuff;
					cout << "Choice:" << inpBuff << endl;

					if (inpBuff[0] == 'n') {
						if ((rtn = er.playNext()) != 0) {
							cout << "PlayNext, End of events(" << rtn << "), total events="<<evtIdx << endl;
							break;
						}
						cout << "[" << ++evtIdx << "] event Played" << endl;
					}
					else if (inpBuff[0] == 'q') {
						break;
					}
				}

			}
			cout << "Event Play Step" << endl;
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