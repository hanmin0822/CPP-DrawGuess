#include "TCPKernel.h"

#include "TCPNet.h"

static long long roomid_auto = 0;

TCPKernel::TCPKernel()
{
	m_pNet = new TCPNet(this);
	m_hNullEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

TCPKernel::~TCPKernel()
{
	delete m_pNet;
	m_pNet = NULL;
}

_BEGIN_PROTOCOL_MAP
	PM(_DEF_PROTOCOL_REGISTER_RQ,&TCPKernel::RegisterRq)
	PM(_DEF_PROTOCOL_LOGIN_RQ,&TCPKernel::LoginRq)
	PM(_DEF_PROTOCOL_LOGOUT_RQ,&TCPKernel::LogoutRq)

	//对于服务器而言，流信息头和内容一样转发，故绑定同一个函数
	PM(_DEF_PROTOCOL_STREAMINFO_RQ,&TCPKernel::StreamInfoRq)
	PM(_DEF_PROTOCOL_STREAMCONTENT_RQ,&TCPKernel::StreamInfoRq)

	PM(_DEF_PROTOCOL_ENTERROOM_RQ,&TCPKernel::SelectRoomRq)
	PM(_DEF_PROTOCOL_CREATEROOM_RQ,&TCPKernel::CreateRoomRq)
	PM(_DEF_PROTOCOL_GAMESTART_RQ,&TCPKernel::GameStartRq)
	PM(_DEF_PROTOCOL_PLAYERMESSAG_RQ,&TCPKernel::PlayerMessageRq)
	PM(_DEF_PROTOCOL_LEAVEROOM_RQ,&TCPKernel::PlayerLeaveRoomRq)

_END_PROTOCOL_MAP

void StreamItask::RunItask(){
	//等待信号\事件
	HANDLE hary[] = {m_pRoom->m_hSemaphore,m_pAudience->m_hEvent};
	DWORD  dwIndex;
	while(1)
	{
		dwIndex = WaitForMultipleObjects(2,hary,FALSE,INFINITE);
		if(dwIndex ==0)
		{
			//推流
			m_pKernel->m_pNet->SendData(m_pAudience->m_sock,m_pRoom->m_szbuffer,_DEF_STREAMPACKDEF);
		}
		else if(dwIndex == 1)
			//退出房间
			break;
	}
}

void TCPKernel::StreamInfoRq(SOCKET sock,char* szbuf){
	StreamInfo *psi = (StreamInfo*)szbuf;
	Room_Info *pRoom = NULL;
	
	cout << "接收到图片转发请求，大小" << psi->m_nLen << endl;

	//根据发来的数据的socket对象判断是哪个房间并记录
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(sock== (*iteRoom)->m_sock)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}

	//将流推给观众map<主播，list<观众>>
	int nSize = m_mapRoomToAudience[pRoom].size();
	//如果房间没有观众，流丢弃
	//如果有观众，则释放相应个数的信号量
	if(nSize >0)
	{
		memcpy(pRoom->m_szbuffer,szbuf,_DEF_STREAMPACKDEF);
		ReleaseSemaphore(pRoom->m_hSemaphore,nSize,NULL);
	}
}

//选择房间
void TCPKernel::SelectRoomRq(SOCKET sock,char* szbuf)
{
	STRU_ENTERROOM_RQ *pssr = (STRU_ENTERROOM_RQ*)szbuf;
	Room_Info *pRoom = NULL;
	Audience_Info *pAudienceif = new Audience_Info;
	//选择对应房间号的主播（房主）信息
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(pssr->m_roomid == (*iteRoom)->m_roomid)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}

	if(pRoom == NULL){
		//没找到房间
		STRU_ENTERROOMFAIL_RS erfrs;
		erfrs.m_nType = _DEF_PROTOCOL_ENTEREOOMRES_RS;
		erfrs.result = 0;
		m_pNet->SendData(sock,(char*)&erfrs,sizeof(erfrs));
		return;
	}

	pAudienceif->m_uid = pssr->m_uid;
	pAudienceif->m_sock = sock;
	pAudienceif->m_hEvent = WSACreateEvent();
	//map<主播，list<观众>>
	m_mapRoomToAudience[pRoom].push_back(pAudienceif);

	STRU_PLAYERNUMRENEW_RS pnnrs;
	pnnrs.m_nType = _DEF_PROTOCOL_PLAYERNUMRENEW_RS;
	pnnrs.m_roomid = pRoom->m_roomid;
	pnnrs.m_playernum = m_mapRoomToAudience[pRoom].size() + 1;
	pnnrs.m_uid = pRoom->m_ownerid;

	//给房主发消息
	m_pNet->SendData(pRoom->m_sock,(char*)&pnnrs,sizeof(pnnrs));

	//给房间里的每个观众发消息
	list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
	auto pAudience = lstAudience.begin();
	while(pAudience != lstAudience.end())
	{
		m_pNet->SendData((*pAudience)->m_sock,(char*)&pnnrs,sizeof(pnnrs));
		pAudience++;
	}

	//向线程池投递任务--分配一个线程
	Itask *pItask = new StreamItask(pRoom,pAudienceif,this);
	m_threadpool.Push(pItask);
}

//房主创建房间请求
void TCPKernel::CreateRoomRq(SOCKET sock,char* szbuf){
	STRU_CREATEROOM_RQ *crrq = (STRU_CREATEROOM_RQ *)szbuf;
	Room_Info* rif = new Room_Info;
	rif->m_good = NULL;
	rif->m_hThreadGameTimer = NULL;
	rif->m_ownerid = crrq->m_uid;
	ZeroMemory(rif->m_szbuffer,sizeof(rif->m_szbuffer));
	rif->m_sock = sock;
	rif->m_roomid = roomid_auto;
	rif->m_hSemaphore = CreateSemaphore(NULL,0,_DEF_SEMAPHORENUM,NULL);
	m_lstRoomInfo.push_back(rif);

	STRU_CREATEROOM_RS crrs;
	crrs.m_nType = _DEF_PROTOCOL_CREATEROOM_RS;
	strcpy_s(crrs.m_PlayerName,_DEF_SIZE,crrq->m_PlayerName);
	crrs.m_roomid = rif->m_roomid;
	crrs.m_uid = crrq->m_uid;

	m_pNet->SendData(sock,(char*)&crrs,sizeof(crrs));

	roomid_auto++;
}

//游戏开始请求
void TCPKernel::GameStartRq(SOCKET sock,char* szbuf){
	STRU_GAMESTART_RQ* gsrq = (STRU_GAMESTART_RQ *)szbuf;
	STRU_GOODSINFO_RS gsrs;
	Room_Info *pRoom = NULL;
	goodinfo* gifo = new goodinfo;

	//从数据库中取得物品信息
	char szsql[_DEF_SQLLEN] = {0};
	list<string> lststr;
	
	m_sql.ConnectMySql(MYSQL_HOST,MYSQL_USER,MYSQL_PWD,MYSQL_DB);
	int gid = GetRandomGoodsID();
	sprintf_s(szsql,"SELECT * FROM goods_name_list WHERE id = %d;",gid);
	m_sql.SelectMySql(szsql,7,lststr);
	m_sql.DisConnect();
	
	if(lststr.size() >0){
		gifo->m_gid = atoi(lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_ThingName,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_FirstHint,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_SecondHint,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_ThirdHint,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
		gifo->m_GuessTime = atoi(lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_AuthorName,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
	}


	//选择对应房间并写入房间信息结构体的goods成员变量
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(gsrq->m_roomid == (*iteRoom)->m_roomid)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}

	//如果已经存在则删除对应内存
	if(pRoom->m_good != NULL){
		delete pRoom->m_good;
	}

	pRoom->m_good = gifo;
	pRoom->m_passnum = 0;

	//准备发送包
	gsrs.m_nType = _DEF_PROTOCOL_GOODSINFO_RS;
	gsrs.m_roomid = gsrq->m_roomid;
	gsrs.m_uid = gsrq->m_uid;
	gsrs.m_GuessTime = gifo->m_GuessTime;
	strcpy_s(gsrs.m_ThingName,_DEF_SIZE,gifo->m_ThingName);
	strcpy_s(gsrs.m_FirstHint,_DEF_SIZE,gifo->m_FirstHint);
	strcpy_s(gsrs.m_SecondHint,_DEF_SIZE,gifo->m_SecondHint);
	strcpy_s(gsrs.m_ThirdHint,_DEF_SIZE,gifo->m_ThirdHint);
	strcpy_s(gsrs.m_AuthorName,_DEF_SIZE,gifo->m_AuthorName);

	//给房主发消息
	m_pNet->SendData(pRoom->m_sock,(char*)&gsrs,sizeof(gsrs));

	//给房间里的每个观众发消息
	list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
	auto pAudience = lstAudience.begin();
	while(pAudience != lstAudience.end())
	{
		m_pNet->SendData((*pAudience)->m_sock,(char*)&gsrs,sizeof(gsrs));
		pAudience++;
	}

	pRoom->isGameon = true;
	//开启定时线程
	Timer_Thread_Info *tti = new Timer_Thread_Info;
	tti->m_pKernel = this;
	tti->pRoom = pRoom;
	pRoom->m_hThreadGameTimer = (HANDLE)_beginthreadex(NULL,0,&ThreadGameTimer,tti,0,0);
}

//玩家请求离开房间
void TCPKernel::PlayerLeaveRoomRq(SOCKET sock,char* szbuf){
	STRU_LEAVEROOM_RQ *lrrq = (STRU_LEAVEROOM_RQ *)szbuf;
	STRU_CLOSEROOM_RS lrrs;
	Room_Info *pRoom = NULL;
	lrrs.m_nType = _DEF_PROTOCOL_CLOSEROOM_RS;
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(lrrq->m_roomid == (*iteRoom)->m_roomid)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}
	lrrs.m_roomid = lrrq->m_roomid;
	lrrs.m_playeruid = lrrq->m_playeruid;
	lrrs.m_isRoomOwner = lrrq->m_isRoomOwner;

	if(lrrq->m_isRoomOwner){
		//如果是房主退房，通知该房间所有玩家退房
		
		pRoom->isGameon = false;
		//给房间里的每个观众发消息
		list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
		auto pAudience = lstAudience.begin();
		while(pAudience != lstAudience.end())
		{
			m_pNet->SendData((*pAudience)->m_sock,(char*)&lrrs,sizeof(lrrs));
			delete *pAudience;
			pAudience++;
		}

		m_mapRoomToAudience.erase(pRoom);
		if(pRoom->m_good != NULL){
			delete pRoom->m_good;
		}
		delete pRoom;

	}else{


		list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
		auto pAudience = lstAudience.begin();
		while(pAudience != lstAudience.end())
		{
			if((*pAudience)->m_uid == lrrq->m_playeruid){
				break;
			}
			pAudience++;
		}

		m_mapRoomToAudience[pRoom].erase(pAudience);


		STRU_PLAYERNUMRENEW_RS pnnrs;
		pnnrs.m_nType = _DEF_PROTOCOL_PLAYERNUMRENEW_RS;
		pnnrs.m_roomid = pRoom->m_roomid;
		pnnrs.m_playernum = m_mapRoomToAudience[pRoom].size() + 1;
		pnnrs.m_uid = pRoom->m_ownerid;

		//给房主发消息
		m_pNet->SendData(pRoom->m_sock,(char*)&pnnrs,sizeof(pnnrs));

		//给房间里的每个观众发消息
		pAudience = lstAudience.begin();
		while(pAudience != lstAudience.end())
		{
			m_pNet->SendData((*pAudience)->m_sock,(char*)&pnnrs,sizeof(pnnrs));
			pAudience++;
		}

	}
}

//玩家消息请求（服务器转发+判断）
void TCPKernel::PlayerMessageRq(SOCKET sock,char* szbuf){
	STRU_PLAYERMESSAG_RQ* pmrq = (STRU_PLAYERMESSAG_RQ *)szbuf;
	STRU_PLAYERMESSAG_RS pmrs;
	Room_Info *pRoom = NULL;
	pmrs.m_nType = _DEF_PROTOCOL_PLAYERMESSAG_RS;
	strcpy_s(pmrs.m_MessageContent,_DEF_PLAYERMESSAG_SIZE,pmrq->m_MessageContent);
	pmrs.m_playeruid = pmrq->m_playeruid;
	strcpy_s(pmrs.m_playername,_DEF_SIZE,pmrq->m_playername);
	pmrs.m_roomid = pmrq->m_roomid;

	//选择对应房间
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(pmrq->m_roomid == (*iteRoom)->m_roomid)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}

	if(pRoom->isGameon){
		if(strcmp(pmrq->m_MessageContent,pRoom->m_good->m_ThingName) == 0){
			pmrs.m_GuessResult = 1;
			pRoom->m_passnum++;
		}else{
			pmrs.m_GuessResult = 0;
		}
	}else{
		pmrs.m_GuessResult = 2;
	}

	//给房主发消息
	m_pNet->SendData(pRoom->m_sock,(char*)&pmrs,sizeof(pmrs));

	//给房间里的每个观众发消息
	list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
	auto pAudience = lstAudience.begin();
	while(pAudience != lstAudience.end())
	{
		m_pNet->SendData((*pAudience)->m_sock,(char*)&pmrs,sizeof(pmrs));
		pAudience++;
	}

}

//收到客户端注册请求的处理
void TCPKernel::RegisterRq(SOCKET sock,char* szbuf)
{
	STRU_REGISTER_RQ *psrr = (STRU_REGISTER_RQ*)szbuf;
	char szsql[_DEF_SQLLEN] = {0};
	STRU_REGISTER_RS srr;
	list<string> lststr;
	srr.m_nType = _DEF_PROTOCOL_REGISTER_RS;
	srr.m_szResult = _register_fail;
	srr.m_userid = -1;
	strcpy_s(srr.m_szUserName,"");
	//先判断是否已经注册过
	m_sql.ConnectMySql(MYSQL_HOST,MYSQL_USER,MYSQL_PWD,MYSQL_DB);
	char szcmpsql[_DEF_SQLLEN] = {0};
	sprintf_s(szcmpsql,"SELECT * FROM user_personal_info WHERE username = '%s' OR email = '%s';",psrr->m_szUserName,psrr->m_szEmail);
	m_sql.SelectMySql(szcmpsql,1,lststr);
	if(lststr.size() >0)
	{
		m_pNet->SendData(sock,(char*)&srr,sizeof(srr));
		return ;
	}

	//将用户信息写入数据库
	sprintf_s(szsql,"INSERT INTO user_personal_info VALUES(NULL,'%s','%s',%d,'%s');",
		psrr->m_szUserName,psrr->m_szPassWord,psrr->m_szGender,psrr->m_szEmail);
    
	if(m_sql.UpdateMySql(szsql))
	{
		srr.m_szResult = _register_success;
	}
	//回复
	m_pNet->SendData(sock,(char*)&srr,sizeof(srr));
	m_sql.DisConnect();
}

//收到客户端登录请求的处理
void TCPKernel::LoginRq(SOCKET sock,char* szbuf)
{
	STRU_LOGIN_RQ *pslr = (STRU_LOGIN_RQ*)szbuf;
	//通过用户名或者邮箱 校验密码是否正确
	char szsql[_DEF_SQLLEN] = {0};
	list<string> lststr;
	STRU_LOGIN_RS slr;
	slr.m_nType = _DEF_PROTOCOL_LOGIN_RS;
	slr.m_szResult = _login_fail;
	sprintf_s(szsql,"SELECT uid,password FROM user_personal_info WHERE username = '%s' OR email = '%s';",pslr->m_szUserName,pslr->m_szEmail);
	m_sql.ConnectMySql(MYSQL_HOST,MYSQL_USER,MYSQL_PWD,MYSQL_DB);
	m_sql.SelectMySql(szsql,2,lststr);
	m_sql.DisConnect();
	if(lststr.size() >0)
	{
		string userid = lststr.front();
		lststr.pop_front();

		string strPassword = lststr.front();
		lststr.pop_front();


		if(!strcmp(pslr->m_szPassWord,strPassword.c_str()))
		{
			slr.m_userid = _atoi64(userid.c_str());
			strcpy_s(slr.m_szUserName,pslr->m_szUserName);
			slr.m_szResult = _login_success;
		}


		//检查玩家是否已经在线，如果已经在线则登录失败，否则记录当前玩家
		auto pPlayer = m_OnlinePlayer.begin();
		while(pPlayer != m_OnlinePlayer.end())
		{
			if(_atoi64(userid.c_str()) == (*pPlayer)){
				slr.m_szResult = _login_fail;
				break;
			}
			pPlayer++;
		}
		m_OnlinePlayer.push_back(_atoi64(userid.c_str()));
	}

	//回复
	m_pNet->SendData(sock,(char*)&slr,sizeof(slr));
}

//玩家请求下线
void TCPKernel::LogoutRq(SOCKET sock,char* szbuf){
	STRU_LOGOUT_RQ * lorq = (STRU_LOGOUT_RQ *)szbuf;
	auto pPlayer = m_OnlinePlayer.begin();
	while(pPlayer != m_OnlinePlayer.end())
	{
		if(lorq->m_userid == (*pPlayer)){
			break;
		}
		pPlayer++;
	}
	m_OnlinePlayer.erase(pPlayer);
}

//创建线程池，初始化TCP连接
bool TCPKernel::Open()
{
	if(!m_threadpool.CreateThreadPool(1,_DEF_THREADPOOLNUM))
		return false;

	if(!m_pNet->InitNetWork())
		return false;

	return true;
}

//销毁线程池，关闭TCP连接
void TCPKernel::Close()
{
	m_threadpool.DestroyThreadPool();

	m_sql.DisConnect();

	m_pNet->UnInitNetWork();
}

//挨个处理消息，根据消息类型查映射表去实现对应函数
bool TCPKernel::DealData(SOCKET sock,char* szbuf)
{
	PackType *pType =(PackType*)szbuf;
	//遍历协议映射表（结构体数组）
	int i = 0;
	while(1)
	{
		if(m_aryProtocolMapEntries[i].m_nType == *pType)
		{
			//调用函数指针
			(this->*m_aryProtocolMapEntries[i].m_npfun)(sock,szbuf);
			break;
		}
		else if(m_aryProtocolMapEntries[i].m_nType ==0 
			&& m_aryProtocolMapEntries[i].m_npfun ==0)
			break;

		i++;
	}
	
	return true;
}

//取随机物品ID（算法防重复暂未实现）
int TCPKernel::GetRandomGoodsID(){
	srand(time(NULL));

	list<string> lststr;
	char szsql[_DEF_SQLLEN] = {0};
	sprintf_s(szsql,"SELECT COUNT(*) FROM goods_name_list;");
	m_sql.SelectMySql(szsql,1,lststr);
	int k = atoi(lststr.front().c_str());
	lststr.pop_front();

	int id = rand() % k + 1;//产生1到当前单词列表总数的随机数


	return id;
}

unsigned _stdcall TCPKernel::ThreadGameTimer(void * lpvoid){
	Timer_Thread_Info *tti = (Timer_Thread_Info*)lpvoid;

	DWORD dwWait = ::WaitForSingleObject(tti->m_pKernel->m_hNullEvent, (tti->pRoom->m_good->m_GuessTime + 2) * 1000);

	if(tti->pRoom->isGameon == true){
		//必须保证游戏是进行中的，防止中途退出
		tti->pRoom->isGameon = false;
		STRU_TIMEOVER_RS tors;
		tors.m_nType = _DEF_PROTOCOL_TIMEOVER_RS;
		tors.m_roomid = tti->pRoom->m_roomid;
		tors.m_PassNum = tti->pRoom->m_passnum;

		//给房主发消息
		tti->m_pKernel->m_pNet->SendData(tti->pRoom->m_sock,(char*)&tors,sizeof(tors));

		//给房间里的每个观众发消息
		list<Audience_Info*> lstAudience = tti->m_pKernel->m_mapRoomToAudience[tti->pRoom];
		auto pAudience = lstAudience.begin();
		while(pAudience != lstAudience.end())
		{
			tti->m_pKernel->m_pNet->SendData((*pAudience)->m_sock,(char*)&tors,sizeof(tors));
			pAudience++;
		}

		
	}


	delete tti;
	return 0;
}