#ifndef _PACKDEF_H
#define _PACKDEF_H

#define	MYSQL_HOST	"59.110.174.158"		//Mysql主机地址
#define	MYSQL_USER	"root"					//Mysql用户名
#define	MYSQL_PWD	"sgzx123456"			//Mysql密码
#define	MYSQL_DB	"draw_guess"			//Mysql数据库

#define SERVER_IP	"127.0.0.1"//客户端专用，连接的服务器地址
#define _DEFPORT    23356	//TCP监听端口
#define _DEFSIZE	1024	//单个包大小

#define _DEF_PLAYERMESSAG_SIZE 500		//用户聊天消息最大长度

#define _DEF_STREAMPACKDEF   sizeof(StreamInfo)

#define _DEF_THREADPOOLNUM    1000		//线程池线程数量

#define _DEF_SEMAPHORENUM    10000		//信号量数目

#define _BEGIN_PROTOCOL_MAP ProtocolMap   m_aryProtocolMapEntries[]=\
{

#define _END_PROTOCOL_MAP 	{0,0} \
};

#define PM(x,y)  {x,y},

#define _DEF_PROTOCOL_BASE    10

//注册请求、回复
#define _DEF_PROTOCOL_REGISTER_RQ    _DEF_PROTOCOL_BASE +1
#define _DEF_PROTOCOL_REGISTER_RS    _DEF_PROTOCOL_BASE +2

//登录请求、回复
#define _DEF_PROTOCOL_LOGIN_RQ    _DEF_PROTOCOL_BASE +3
#define _DEF_PROTOCOL_LOGIN_RS    _DEF_PROTOCOL_BASE +4

#define _DEF_PROTOCOL_CREATEROOM_RQ    _DEF_PROTOCOL_BASE +5
#define _DEF_PROTOCOL_CREATEROOM_RS    _DEF_PROTOCOL_BASE +6
#define _DEF_PROTOCOL_ENTERROOM_RQ    _DEF_PROTOCOL_BASE +7

#define _DEF_PROTOCOL_PLAYERNUMRENEW_RS    _DEF_PROTOCOL_BASE +8
#define _DEF_PROTOCOL_GAMESTART_RQ    _DEF_PROTOCOL_BASE +9

#define _DEF_PROTOCOL_GOODSINFO_RS    _DEF_PROTOCOL_BASE +10

#define _DEF_PROTOCOL_PLAYERMESSAG_RQ    _DEF_PROTOCOL_BASE +11
#define _DEF_PROTOCOL_PLAYERMESSAG_RS    _DEF_PROTOCOL_BASE +12

#define _DEF_PROTOCOL_STREAMINFO_RQ    _DEF_PROTOCOL_BASE +13
#define _DEF_PROTOCOL_STREAMINFO_RS    _DEF_PROTOCOL_BASE +14

#define _DEF_PROTOCOL_STREAMCONTENT_RQ    _DEF_PROTOCOL_BASE +15
#define _DEF_PROTOCOL_STREAMCONTENT_RS    _DEF_PROTOCOL_BASE +16

#define _DEF_PROTOCOL_TIMEOVER_RS    _DEF_PROTOCOL_BASE +17

#define _DEF_PROTOCOL_LEAVEROOM_RQ    _DEF_PROTOCOL_BASE +18
#define _DEF_PROTOCOL_CLOSEROOM_RS    _DEF_PROTOCOL_BASE +19

#define _DEF_PROTOCOL_LOGOUT_RQ    _DEF_PROTOCOL_BASE +20

#define _DEF_PROTOCOL_ENTEREOOMRES_RS    _DEF_PROTOCOL_BASE +21


typedef char PackType;

#define _DEF_SIZE   45				//用户名、密码最大长度
#define _DEF_STREAMSIZE   4096		//流长度
#define _DEF_AUTHORLISTNUM   10
#define _DEF_SQLLEN       300
#define _role_audience   0
#define _role_author     1

#define _register_fail     0
#define _register_success   1
#define _login_fail     0
#define _login_success   1

#define UM_LOGINMSG				WM_USER + 1
#define UM_CREATROOMMSG			WM_USER + 2
#define UM_PLAYERNUMRENEWMSG	WM_USER + 3
#define UM_GETGOODSINFOMSG		WM_USER + 4
#define UM_TIMEREVENTMSG		WM_USER + 5
#define UM_CLOSEROOMMSG			WM_USER + 6
#define UM_ENTERFAILMSG			WM_USER + 7

struct goodinfo{
	int		m_gid;//物品ID
	char 	m_ThingName[_DEF_SIZE];//物品词条
	char 	m_FirstHint[_DEF_SIZE];//第一条提示
	char 	m_SecondHint[_DEF_SIZE];//第二条提示
	char 	m_ThirdHint[_DEF_SIZE];//第三条提示
	int		m_GuessTime;//物品词条决定时间
	char 	m_AuthorName[_DEF_SIZE];//出题人名
};

//===========================协议包结构定义==============================

//注册、登录请求包
typedef struct STRU_REGISTER_RQ
{
	PackType m_nType;
	char     m_szUserName[_DEF_SIZE];
	char     m_szPassWord[_DEF_SIZE];
	int      m_szGender;//0代表女，1代表男，2代表未知性别，3代表登录请求
	char	 m_szEmail[_DEF_SIZE];
}STRU_LOGIN_RQ;

//注册、登录回复包
typedef struct STRU_REGISTER_RS
{
	PackType	m_nType;
	char		m_szResult;
	long long	m_userid;//用于登录回复
	char		m_szUserName[_DEF_SIZE];//用于登录回复
}STRU_LOGIN_RS;

//流信息
struct StreamInfo
{
	PackType m_nType;
	long long m_UserId;
	char      m_szContent[_DEF_STREAMSIZE];
	int       m_nLen;
};

//房主创建房间消息、房间创建返回值消息、玩家进入房间消息
typedef struct STRU_CREATEROOM_RQ{
	PackType m_nType;
	long long m_uid;//房主UID或观众UID
	char 	m_PlayerName[_DEF_SIZE];//房主用户名或观众用户名
	long long m_roomid;//房间id，在房间创建返回值消息中有效，若为-1，则说明开房失败，						在玩家进入房间消息中有效，代表要进入的房间
}STRU_CREATEROOM_RS,STRU_ENTERROOM_RQ;

//人数刷新消息、游戏开始消息
typedef struct STRU_PLAYERNUMRENEW_RS{
	PackType m_nType;
	long long m_uid;//房主UID
	long long m_roomid;//房间id
	int m_playernum;//房间人数(包括房主)，在游戏开始消息中无效（置为-1）
}STRU_GAMESTART_RQ;

//游戏词条消息
struct STRU_GOODSINFO_RS{
	PackType m_nType;
	long long m_uid;//房主UID
	long long m_roomid;//房间id
	char 	m_ThingName[_DEF_SIZE];//物品词条
	char 	m_FirstHint[_DEF_SIZE];//第一条提示
	char 	m_SecondHint[_DEF_SIZE];//第二条提示
	char 	m_ThirdHint[_DEF_SIZE];//第三条提示
	int		m_GuessTime;//物品词条决定时间
	char 	m_AuthorName[_DEF_SIZE];//出题人名
};

//观众消息（包括服务器转发的）
typedef struct STRU_PLAYERMESSAG_RQ{
	PackType	m_nType;
	long long	m_playeruid;//发消息玩家的UID
	char		m_playername[_DEF_SIZE];//发消息玩家的用户名
	long long	m_roomid;//房间id
	char 		m_MessageContent[_DEF_PLAYERMESSAG_SIZE];//消息内容
	int			m_GuessResult;//猜测结果，0为未猜中，1为猜中，2为当前不在游戏进程中
}STRU_PLAYERMESSAG_RS;

//游戏到时消息
struct STRU_TIMEOVER_RS{
	PackType m_nType;
	long long m_roomid;//房间id
	int 		m_PassNum;//猜对的玩家人数
	//这个结构体后期可以具体更新，根据服务器存储的结构来定
};

//退房消息、房主关闭房间消息
typedef struct STRU_LEAVEROOM_RQ{
	PackType m_nType;
	long long m_roomid;//房间id
	long long m_playeruid;//退房玩家的UID
	bool 	m_isRoomOwner;//是否为房主
}STRU_CLOSEROOM_RS;
 
//下线消息
struct STRU_LOGOUT_RQ{
	PackType m_nType;
	long long	m_userid;//下线的玩家ID
};

//玩家进房失败消息
struct STRU_ENTERROOMFAIL_RS{
	PackType m_nType;
	int	result;//失败原因
};

#endif