/////////////////////////////////////////////////////////////////////////////////
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////
#ifndef _LESSON_X_H_
#define _LESSON_X_H_
//
#include <Windows.h>
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////
//
// 游戏总管类。负责处理游戏主循环、游戏初始化、结束等工作
class	CGameMain
{
private:
	int				m_iGameState;		// 游戏状态，0：结束或者等待开始；1：初始化；2表示未开始（此时拍子不随鼠标移动），3 表示倒计时阶段， 4表示开始拍子可以随鼠标移动并拍打
    float			m_fRotateTime;		// 拍子拍下后距离复位还差多少时间
    float			m_fOldRotation;		// 拍子的初始角度
    CSprite		    *paizi;

    int				countdownTime ;	    //倒计时时间
	float			countPassedTime;	//倒计时经过的时间
	CTextSprite		*countdown;		    //倒计时的文字精灵
    CSprite			*kaishi;			//“空格开始”精灵


    vector<CSprite*>m_mosquitos;
    int				m_iMosquitoCount;

public:
	CGameMain();                        //构造函数
	~CGameMain();                       //析构函数

	// Get方法
	int				GetGameState()											{ return m_iGameState; }

	// Set方法
	void			SetGameState( const int iState )				{ m_iGameState	=	iState; }

	// 游戏主循环等
	void			GameMainLoop( float	fDeltaTime );
	void			GameInit();
	void			GameRun( float fDeltaTime );
	void			GameEnd();

	void			OnMouseMove( const float fMouseX, const float fMouseY );
	void			OnMouseClick( const int iMouseType, const float fMouseX, const float fMouseY );

    void            OnKeyDown( const int iKey, const bool bAltPress, const bool bShiftPress, const bool bCtrlPress );

    void            MakeSprite();
};

/////////////////////////////////////////////////////////////////////////////////
//
extern CGameMain	g_GameMain;

#endif // _LESSON_X_H_
