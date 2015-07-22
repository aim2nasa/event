#ifndef __IPLAYNOTI_H__
#define __IPLAYNOTI_H__

class DLLEXPORT IPlayNoti{
public:
	virtual ~IPlayNoti(){}

	virtual void fullPlay(int err) = 0;
	virtual void partPlay(int err) = 0;
};

#endif
