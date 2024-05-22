/////////////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////
#include <Stdio.h>
#include "CommonClass.h"
#include "LessonX.h"

#include <ctime>
#include <cstdlib>
#include <vector>
#include <vld.h>

//#include <mysql.h>
using namespace std;
////////////////////////////////////////////////////////////////////////////////
//
//
CGameMain		g_GameMain;

//==============================================================================
//
// 大体的程序流程为：GameMainLoop函数为主循环函数，在引擎每帧刷新屏幕图像之后，都会被调用一次。

//==============================================================================
//
// 构造函数
CGameMain::CGameMain()
{
	_CrtSetBreakAlloc(106);//

	m_iGameState = 1;
	paizi = new CSprite("paizi");
	countdown = new CTextSprite("countdown");
	kaishi = new CSprite("kaishi");
	score = new CTextSprite("score");
	gameTime = new CTextSprite("gameTime");

	mapState = false;
	map1 = new CSprite("Map1");
	map2 = new CSprite("Map2");

	//error = new CTextSprite("error");
}
//==============================================================================
//
// 析构函数
CGameMain::~CGameMain()
{
	free(paizi);
	free(countdown);
	free(kaishi);
	free(score);
	free(gameTime);
	free(map1);
	free(map2);
}

//==============================================================================
//
// 游戏主循环，此函数将被不停的调用，引擎每刷新一次屏幕，此函数即被调用一次
// 用以处理游戏的开始、进行中、结束等各种状态.
// 函数参数fDeltaTime : 上次调用本函数到此次调用本函数的时间间隔，单位：秒
void CGameMain::GameMainLoop(float	fDeltaTime)
{
	switch (GetGameState())
	{
		// 初始化游戏，清空上一局相关数据
	case 1:
	{
		GameInit();
		SetGameState(2); // 初始化之后，将游戏状态设置为进行中
	}
	break;

	// 游戏进行中，处理各种游戏逻辑
	case 2:
	case 3:
	case 4:

	{
		// TODO 修改此处游戏循环条件，完成正确游戏逻辑
		if (true)
		{
			GameRun(fDeltaTime);
		}
		else // 游戏结束。调用游戏结算函数，并把游戏状态修改为结束状态
		{
			SetGameState(0);
			GameEnd();
		}
	}
	break;

	// 游戏结束/等待按空格键开始
	case 0:
	default:
		break;
	};
}
//=============================================================================
//
// 每局开始前进行初始化，清空上一局相关数据
void CGameMain::GameInit()
{
	CSystem::ShowCursor(false);	                                //隐藏鼠标
	m_fRotateTime = 0.f;
	m_fOldRotation = paizi->GetSpriteRotation();		//获取拍子的初始角度

	countdownTime = 6;		                        //倒计时时间为5s
	countPassedTime = 0.f;
	countdown->SetSpriteVisible(false);
	//设置倒计时不可见

	m_iMosquitoCount = 15;

	m_iGameScore = 0;                                //初始化得分

	m_fGameTime = 30.f;		                        //初始化游戏时间为30s

	m_fAppearTime = 0.5f;
	m_fScreenLeft = CSystem::GetScreenLeft();
	m_fScreenRight = CSystem::GetScreenRight();
	m_fScreenBottom = CSystem::GetScreenBottom();
	m_fScreenTop = CSystem::GetScreenTop();

}
//=============================================================================
//
// 每局游戏进行中
void CGameMain::GameRun(float fDeltaTime)
{
	if (m_iGameState == 3) {	//m_iGameState 为3时才执行
		countPassedTime += fDeltaTime;	//统计经过的时间，并存储到countPassedTime
		countdown->SetTextValue(countdownTime - countPassedTime);
		if (countPassedTime >= countdownTime) {	//经过的时间超过countdownTime
			countdown->SetSpriteVisible(false);	//隐藏countdown
			//MakeSprite_NoFly();//生成飞虫
			countPassedTime = 0;		//重置经过的时间，为下一轮游戏做准备
			m_iGameState = 4;		//切换g_iGameState 3->4
		}
	}
	else if (m_iGameState == 4) {
		m_fGameTime -= fDeltaTime;
		if (m_fGameTime > 0) {
			gameTime->SetTextValue(m_fGameTime);	//若游戏时间未结束，则显示剩余的游戏时间
			MakeSprite_Fly(fDeltaTime);//生成飞虫
		}
		else {
			m_iGameState = 2;		//若游戏时间结束，完成g_iGameState 2->0的转换
			//实现地图切换
			//ConnectMySQL(m_iGameScore);
			map1->SetSpriteVisible(mapState);
			mapState = !mapState;
			map2->SetSpriteVisible(mapState);
			kaishi->SetSpriteVisible(true);            //显示“空格开始”
			m_fGameTime = 0;
			gameTime->SetTextValue(m_fGameTime);
		}
	}
	if (m_fRotateTime > 0) {
		m_fRotateTime -= fDeltaTime;	//每经过fDeltaTime，拍子旋转的时间也少了fDeltaTime
		if (m_fRotateTime <= 0) {	//经过了0.2s以后
			paizi->SetSpriteRotation(m_fOldRotation);	//恢复拍子的初始旋转角
		}
	}

}
//=============================================================================
//
// 本局游戏结束
void CGameMain::GameEnd()
{
}
//使拍子的位置和鼠标一致
void CGameMain::OnMouseMove(const float fMouseX, const float fMouseY) {
	if (m_iGameState == 4)
		paizi->SetSpritePosition(fMouseX, fMouseY);	//使拍子的位置和鼠标一致
}
//
void CGameMain::OnMouseClick(const int iMouseType, const float fMouseX, const float fMouseY) {
	if (iMouseType == MOUSE_LEFT && m_iGameState == 4)	//鼠标左键按下
	{
		paizi->SetSpriteRotation(m_fOldRotation - 20);		//设置拍子的旋转角，为初始值-10
		m_fRotateTime = 0.2f;	//拍打的时间间隔 0.2s，即0.2s后恢复拍子初始角度
		//遍历判定蚊子是否被击中
		float fX, fY;
		for (int i = 0; i < m_mosquitos.size(); i++) {		//遍历vector中的蚊子
			fX = m_mosquitos[i]->cs->GetSpritePositionX();	//获取蚊子所在的横坐标
			fY = m_mosquitos[i]->cs->GetSpritePositionY();	//获取蚊子所在的纵坐标
			if (paizi->IsPointInSprite(fX, fY))			//判断蚊子是否在拍子的范围内
			{
				m_mosquitos[i]->cs->DeleteSprite();	//删除该蚊子
				if (m_mosquitos[i]->speed < 25) {//每打到的飞虫的速度加分
					m_iGameScore += m_mosquitos[i]->speed / 5;
				}
				else {
					m_iGameScore += (m_mosquitos[i]->speed / 5 + m_mosquitos[i]->speed / 10);
				}
				score->SetTextValue(m_iGameScore);
				break;
			}
		}
	}
}

void CGameMain::OnKeyDown(const int iKey, const bool bAltPress, const bool bShiftPress, const bool bCtrlPress) {
	if (KEY_SPACE == iKey && 2 == m_iGameState)	//按下空格且游戏状态为2时
	{
		m_iGameState = 3;	//设置游戏状态进入倒计时，即3
		countdown->SetSpriteVisible(true);	//显示倒计时文本框
		kaishi->SetSpriteVisible(false);		//隐藏“空格开始”
		m_iGameScore = 0.f;		//初始化新一轮游戏分数
		m_fGameTime = 30.f;		//初始化新一轮游戏时间
		score->SetTextValue(m_iGameScore);
		gameTime->SetTextValue(m_fGameTime);
		for (int i = 0; i < m_mosquitos.size(); i++) {	//删除上一轮游戏中剩余的蚊子
			m_mosquitos[i]->cs->DeleteSprite();
		}
		m_mosquitos.clear();	//清空vector
	}
	//if (KEY_H == iKey) {
	//	char SqlText[256];	//sql语句
	//	MYSQL mysql;		//句柄
	//	MYSQL_RES* res; 	//结果集
	//	MYSQL_FIELD* fd; //字段信息
	//	MYSQL_ROW row;	//一行数据
	//	//连接数据库test
	//	mysql_init(&mysql);
	//	if (!mysql_real_connect(&mysql, "localhost", "root", "030425", "test", 3306, nullptr, 0)) {
	//		puts("数据库连接失败");
	//		error->SetTextValue(404);
	//		mysql_close(&mysql);
	//		//return false;
	//	}
	//	else {
	//		puts("数据库连接成功");
	//	}
	//	sprintf_s(SqlText, "create table animals(id int(4) not null primary key auto_increment,name char(20) not null, kg double(7,1))");
	//	if (mysql_query((MYSQL*)&mysql, SqlText)) {
	//		puts("Can't create table");
	//		printf("%s\n", mysql_error(&mysql));
	//		mysql_close(&mysql);
	//		//return false;
	//	}
	//	//插入数据
	//	sprintf_s(SqlText, "insert into animals(name, kg) values ('chicken',6), ('dog', 4)");
	//	if (mysql_query((MYSQL*)&mysql, SqlText)) {
	//		puts("Can't insert data to table");
	//		printf("%s\n", mysql_error(&mysql));
	//		mysql_close(&mysql);
	//		//return false;
	//	}
	//	//查询数据
	//	sprintf_s(SqlText, "select * from animals");
	//	if (!mysql_query((MYSQL*)&mysql, SqlText)) {
	//		res = mysql_store_result(&mysql);
	//		int i = (int)mysql_num_rows(res);
	//		printf("Query: %s\n%d records \n", SqlText, i);
	//		//输出各字段名
	//		for (i = 0; fd = mysql_fetch_field(res); i++)
	//			printf("%-6s\t", fd->name);
	//		puts("");
	//		//依次读取各条记录
	//		while (row = mysql_fetch_row(res))
	//			printf("%-6s\t%-6s\t%-6s\n", row[0], row[1], row[2]);
	//		mysql_free_result(res);
	//	}
	//}
}
/*
void CGameMain::MakeSprite_NoFly(){
	int minX		=	CSystem::GetScreenLeft() + 5;
	int maxX		=	CSystem::GetScreenRight() - 5;
	int minY		=	CSystem::GetScreenBottom() - 5;
	int maxY		=	CSystem::GetScreenTop() + 5;
	int	iPosX = 0, iPosY = 0;		//随机产生的横纵坐标位置
	int iLoop;	//循环控制变量
	for( iLoop = 0; iLoop < m_iMosquitoCount; iLoop++ )
	{
		char *szName = CSystem::MakeSpriteName("wenzi",iLoop);	//利用iLoop产生不同的蚊子名
		CSprite *wenzi = new CSprite(szName);
		wenzi->CloneSprite("wenziTemplate");	//复制wenziTemplate
		iPosX	=	CSystem::RandomRange(minX, maxX);		//随机产生X坐标
		iPosY	=	CSystem::RandomRange(minY, maxY);		//随机产生Y坐标
		wenzi->SetSpritePosition(iPosX,iPosY);  //设置产生蚊子的位置
		m_mosquitos.push_back(wenzi);		//压入vector中集中管理
	}
}*/

void CGameMain::MakeSprite_Fly(float fDeltaTime) {
	m_fAppearTime -= fDeltaTime;	//计算飞虫的剩余产生时间
	if (m_fAppearTime < 0) {	//时间到，可以产生飞虫
		m_fAppearTime = 0.5f;		//重置产生飞虫的时间
		int iDir = CSystem::RandomRange(0, 3);	//随机一个方向
		float fposX = 0, fposY = 0;
		switch (iDir) {
		case 0:		//top
			fposX = CSystem::RandomRange(m_fScreenLeft - 5.f, m_fScreenRight + 5.f);
			fposY = CSystem::RandomRange(m_fScreenTop - 5.f, m_fScreenTop + 5.f);
			break;
		case 1:		//bottom
			fposX = CSystem::RandomRange(m_fScreenLeft - 5.f, m_fScreenRight + 5.f);
			fposY = CSystem::RandomRange(m_fScreenBottom, m_fScreenBottom + 5.f);
			break;
		case 2:		//left
			fposX = CSystem::RandomRange(m_fScreenLeft - 5.f, m_fScreenLeft);
			fposY = CSystem::RandomRange(m_fScreenTop - 5.f, m_fScreenBottom + 5.f);
			break;
		case 3:		//right
			fposX = CSystem::RandomRange(m_fScreenRight, m_fScreenRight + 5.f);
			fposY = CSystem::RandomRange(m_fScreenTop - 5.f, m_fScreenBottom + 5.f);
			break;
		}
		srand((unsigned)time(0));
		int randTemplate = (rand() % (2 - 0 + 1)) + 0;//使用 (rand() % (b-a+1))+ a，取得 [a,b] 的随机整数
		Mosquito* m = new Mosquito;	//动态分配空间
		m->speed = CSystem::RandomRange(10, 40);		//随机速度
		switch (randTemplate) {
		case 0:
			m->szName = CSystem::MakeSpriteName("wenzi", m_iMosquitoCount++);		//命名
			m->cs = new CSprite(m->szName);	//蚊子精灵指针也保存在struct数组中
			m->cs->CloneSprite("wenziTemplate");	//两重引用
			break;
		case 1:
			m->szName = CSystem::MakeSpriteName("maggot", m_iMosquitoCount++);		//命名
			m->cs = new CSprite(m->szName);	//蚊子精灵指针也保存在struct数组中
			m->cs->CloneSprite("maggotTemplate");	//两重引用
			break;
		case 2:
			m->szName = CSystem::MakeSpriteName("ufo", m_iMosquitoCount++);		//命名
			m->cs = new CSprite(m->szName);	//蚊子精灵指针也保存在struct数组中
			m->cs->CloneSprite("ufoTemplate");	//两重引用
			break;
		}
		m->cs->SetSpritePosition(fposX, fposY);
		m->next = NULL;
		float fDesX = CSystem::RandomRange(m_fScreenLeft + 15.f, m_fScreenRight - 15.f);
		float fDesY = CSystem::RandomRange(m_fScreenTop + 15.f, m_fScreenBottom - 15.f);
		m->cs->SpriteMoveTo(fDesX, fDesY, m->speed, false);
		m_mosquitos.push_back(m);		//压入vector中集中管理
	}
}

/*
bool CGameMain::ConnectMySQL(int m_iGameScore) {
	char SqlText[256];  // 用于存储 SQL 语句的字符串数组
	MYSQL mysql;        // MySQL 句柄
	MYSQL_RES* res;     // 结果集
	MYSQL_FIELD* fd;    // 字段信息
	MYSQL_ROW row;      // 一行数据

	// 初始化 MySQL 句柄
	mysql_init(&mysql);

	// 连接到 MySQL 数据库 test
	if (!mysql_real_connect(&mysql, "localhost", "root", "030425", "killinsect", 3303, nullptr, 0)) {
		puts("数据库连接失败");
		mysql_close(&mysql);
		return false;
	} else {
		puts("数据库连接成功");
	}

	sprintf(SqlText, "create table H_GameScore(id int(5) not null primary key auto_increment, source int(4) not null");
	if (mysql_query((MYSQL*)&mysql, SqlText)) {
		puts("Can't create table");
		printf("%s\n", mysql_error(&mysql));
	}

	sprintf(SqlText, "insert into H_GameScore(source) values (%d)", m_iGameScore);
	if (mysql_query((MYSQL*)&mysql, SqlText)) {
		puts("Can't insert data to table");
		printf("%s\n", mysql_error(&mysql));
		mysql_close(&mysql);
		return false;
	}
	mysql_close(&mysql);

	return true;
}*/
