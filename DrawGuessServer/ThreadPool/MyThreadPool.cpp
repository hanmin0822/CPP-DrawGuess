#include "MyThreadPool.h"


CMyThreadPool::CMyThreadPool(void)
{
	m_bFlagQuit = true;
	m_hSemaphore = NULL;
	m_lMaxThreadNum = 0; 
	m_lCreateThreadNum = 0;
	m_lRunThreadNum = 0;
	m_hMutex = CreateMutex(0,FALSE,0);
}


CMyThreadPool::~CMyThreadPool(void)
{
}

bool CMyThreadPool::CreateThreadPool(long lMinThreadNum,long lMaxThreadNum)
{
	if(lMinThreadNum <=0 || lMaxThreadNum <lMinThreadNum)
		return false;
	//创建资源
	m_hSemaphore = CreateSemaphore(0,0,lMaxThreadNum,0);
	HANDLE hThread;
	for(long i = 0;i < lMinThreadNum;i++)
	{
		 hThread = (HANDLE)_beginthreadex(0,0,&ThreadProc,this,0,0);
		 if(hThread)
			 m_lstThread.push_back(hThread);
	}
	m_lMaxThreadNum = lMaxThreadNum;
	m_lCreateThreadNum = lMinThreadNum;
	return true;
}


void CMyThreadPool::DestroyThreadPool()
{
	m_bFlagQuit = false;
	auto ite = m_lstThread.begin();
	while(ite != m_lstThread.end())
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(*ite,100))
			TerminateThread(*ite,-1);

		CloseHandle(*ite);
		*ite = NULL;
		ite++;
	}
	m_lstThread.clear();

	if(m_hSemaphore)
	{
		CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}
	if(m_hMutex)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}

	//等任务
	Itask *pItask  = NULL;
	while(!m_qItask.empty())
	{
		pItask = m_qItask.front();
		m_qItask.pop();
		
		delete pItask;
		pItask = NULL;
	}

}
unsigned  __stdcall CMyThreadPool::ThreadProc( void * lpvoid)
{
	CMyThreadPool *pthis = (CMyThreadPool*)lpvoid;
	Itask *pItask = NULL;
	while(pthis->m_bFlagQuit)
	{
		//处于睡眠状态--阻塞
		WaitForSingleObject(pthis->m_hSemaphore,INFINITE);
		//从睡眠---工作
	//	pthis->m_lRunThreadNum++;
		InterlockedIncrement(&pthis->m_lRunThreadNum);
		//等任务
		while(!pthis->m_qItask.empty())
		{
			WaitForSingleObject(pthis->m_hMutex,INFINITE);
			if(pthis->m_qItask.empty())
			{
				ReleaseMutex(pthis->m_hMutex);
				break;
			}
			pItask = pthis->m_qItask.front();
			pthis->m_qItask.pop();
			ReleaseMutex(pthis->m_hMutex);
			//执行任务
			pItask->RunItask();

			delete pItask;
			pItask = NULL;
		}

		//从工作 -- 睡眠
		//pthis->m_lRunThreadNum--;
		InterlockedDecrement(&pthis->m_lRunThreadNum);

	}

	return 0;
}
bool CMyThreadPool::Push(Itask* pItask)
{
	if(!pItask)
		return false;

	//将任务加入到队列中
	WaitForSingleObject(m_hMutex,INFINITE);
	m_qItask.push(pItask);
	ReleaseMutex(m_hMutex);
	//1.有空闲线程
	
	if(m_lRunThreadNum == m_lCreateThreadNum 
		&& m_lCreateThreadNum < m_lMaxThreadNum)
	{
		HANDLE hThread = (HANDLE)_beginthreadex(0,0,&ThreadProc,this,0,0);
		 if(hThread)
			 m_lstThread.push_back(hThread);
		 m_lCreateThreadNum++;
	}
	
	//释放信号量
	 ReleaseSemaphore(m_hSemaphore,1,0);

	return true;
}

