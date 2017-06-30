#pragma once
#include <QThread>
class XVideoThread:public QThread
{
public:
	static XVideoThread *Get(){					//创建单例模式，不用用户去维护
		static XVideoThread vt;
		return &vt;
	}
	void run();								//线程函数
	XVideoThread();
	virtual ~XVideoThread();
};

