#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "RecordGameList.h"
#include "ApplyUserList.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//筹码定义
#define JETTON_COUNT				7									//筹码数目
#define JETTON_RADII				68									//筹码半径

//消息定义
#define IDM_PLACE_JETTON			WM_USER+200							//加住信息
#define IDM_APPLY_BANKER			WM_USER+201							//申请信息
#define IDM_ONBANK					WM_USER+202
//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//历史记录
#define MAX_SCORE_HISTORY			256									//历史个数

#define Status_DisCard				1
#define Status_Free					2
#define Status_Jetton				3

//////////////////////////////////////////////////////////////////////////
//结构定义

//筹码信息
struct tagJettonInfo
{
	int								nXPos;								//筹码位置
	int								nYPos;								//筹码位置
	BYTE							cbJettonIndex;						//筹码索引
};

//筹码数组
typedef CArrayTemplate<tagJettonInfo,tagJettonInfo&> CJettonInfoArray;

//操作结果
enum enOperateResult
{
	enOperateResult_NULL,
	enOperateResult_Win,
	enOperateResult_Lost
};
//记录信息
struct tagClientGameRecord
{
	enOperateResult					enOperateFlags;						//操作标识
	BYTE							cbPlayerCount;						//闲家点数
	BYTE							cbBankerCount;						//庄家点数
};

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//下注信息
protected:
	__int64							m_lMeMaxScore;						//最大下注
	__int64							m_lMeTieScore;						//买平总注
	__int64							m_lMeBankerScore;					//买庄总注
	__int64							m_lMePlayerScore;					//买闲总注
	__int64							m_lMeTieSamePointScore;				//同点平注
	__int64							m_lMePlayerKingScore;				//闲天王注
	__int64							m_lMeBankerKingScore;				//庄天王注

	//全体下注
protected:
	__int64							m_lAllTieScore;						//买平总注
	__int64							m_lAllBankerScore;					//买庄总注
	__int64							m_lAllPlayerScore;					//买闲总注
	__int64							m_lAllTieSamePointScore;			//平天王注
	__int64							m_lAllBankerKingScore;				//庄天王注
	__int64							m_lAllPlayerKingScore;				//闲天王注

	//位置信息
protected:
	int								m_nWinFlagsExcursionX;				//偏移位置
	int								m_nWinFlagsExcursionY;				//偏移位置
	int								m_nWinPointsExcursionX;				//偏移位置
	int								m_nWinPointsExcursionY;				//偏移位置
	int								m_nScoreHead;						//成绩位置

	//历史信息
protected:
	WORD							m_wDrawCount;						//游戏局数
	__int64							m_lMeResultCount;					//游戏成绩
	tagClientGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];//游戏记录
	int								m_nRecordFirst;						//开始记录
	int								m_nRecordLast;						//最后记录

	//状态变量
public:
	WORD							m_wMeChairID;						//我的位置
	BYTE							m_cbWinnerSide;						//胜利玩家
	BYTE							m_cbWinnerFlash;					//胜利玩家
	BYTE							m_cbKingWinner;						//胜利玩家
	__int64							m_lCurrentJetton;					//当前筹码
	CString							m_strGameCardResult;				//游戏结果
	BYTE							m_cbPreJettonArea;					//之前区域
	bool							m_bDispatchCard;					//发牌标识
	CString							m_strDispatchCardTips;				//发牌提示

	//庄家信息
public:
	bool							m_bShowChangeBanker;				//轮换庄家
	WORD							m_wCurrentBankerChairID;			//当前庄家
	BYTE							m_cbBankerTime;						//做庄次数
	__int64							m_lBankerScore;						//庄家成绩
	__int64							m_lBankerTreasure;					//庄家金币

	//当局成绩
public:
	__int64							m_lMeCurGameScore;					//我的成绩
	__int64							m_lMeCurGameReturnScore;			//我的成绩
	__int64							m_lBankerCurGameScore;				//返还积分
	bool							m_bShowGameResult;					//显示积分

	//数据变量
protected:
	CPoint							m_PointJetton[6];					//筹码位置
	CJettonInfoArray				m_JettonInfoArray[6];				//筹码数组

	//控件变量
public:
	HCURSOR							m_AllhCursor[7];
	CSkinButton						m_btJetton10000000;						//筹码按钮
	CSkinButton						m_btJetton1000;						//筹码按钮
	CSkinButton						m_btJetton10000;					//筹码按钮
	CSkinButton						m_btJetton100000;					//筹码按钮
	CSkinButton						m_btJetton500000;					//筹码按钮
	CSkinButton						m_btJetton1000000;					//筹码按钮
	CSkinButton						m_btJetton5000000;					//筹码按钮	
	
	CSkinButton						m_btApplyBanker;					//申请庄家
	CSkinButton						m_btCancelBanker;					//取消庄家

	CSkinButton						m_btScoreMoveL;						//移动成绩
	CSkinButton						m_btScoreMoveR;						//移动成绩
	CSkinButton						m_btnQuqian;	
	CSkinButton						m_btUp;								//去上一个申请庄家信息
	CSkinButton						m_btDown;							//去下一个申请庄家信息

//	CSkinButton						m_btnCunqian;
	//控件变量
public:
	CApplyUser						m_ApplyUser;						//申请列表
	CGameRecord						m_GameRecord;						//记录列表
	CCardControl					m_CardControl[2];					//扑克控件
	

	//界面变量
protected:
	CSkinImage						m_ImageViewFill;					//背景位图
	CSkinImage						m_ImageViewBack;					//背景位图
	CSkinImage						m_ImageWinFlags;					//标志位图
	CSkinImage						m_ImageJettonView;					//筹码视图
	CSkinImage						m_ImageScoreNumber;					//数字视图
	CSkinImage						m_ImageMeScoreNumber;				//数字视图
	CSkinImage						m_ImageTimeFlag;					//时间标识

	//边框资源
protected:
	CSkinImage						m_ImageFrameXianJia;				//边框图片
	CSkinImage						m_ImageFrameZhuangJia;				//边框图片
	CSkinImage						m_ImageFrameXianTianWang;			//边框图片
	CSkinImage						m_ImageFrameZhuangTianWang;			//边框图片
	CSkinImage						m_ImageFramePingJia;				//边框图片
	CSkinImage						m_ImageFrameTongDianPing;			//边框图片
	CSkinImage						m_ImageMeBanker;					//切换庄家
	CSkinImage						m_ImageChangeBanker;				//切换庄家
	CSkinImage						m_ImageNoBanker;					//切换庄家

	//结束资源
protected:
	CSkinImage						m_ImageGameEnd;						//成绩图片
	CSkinImage						m_ImageGameEndFrame;				//边框图片
	CSkinImage						m_ImageGamePoint;					//点数图片
	CSkinImage						m_ImageCard;						//图片资源

public:
	BYTE							m_bstate;							//当前游戏状态
	__int64							m_nJettonScoreDec[6];

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);

	//功能函数
public:
	//设置信息
	void SetMeMaxScore(__int64 lMeMaxScore);
	//设置信息
	void SetMeTieScore(__int64 lMeTieScore);
	//设置信息
	void SetMeBankerScore(__int64 lMeBankerScore);
	//设置信息
	void SetMePlayerScore(__int64 lMePlayerScore);
	//设置信息
	void SetMePlayerKingScore(__int64 lMePlayerKingScore);
	//设置信息
	void SetMeBankerKingScore(__int64 lMeBankerKingScore);
	//设置信息
	void SetMeTieSamePointScore(__int64 lMeTieSamePointScore);

	//设置筹码
	void SetCurrentJetton(__int64 lCurrentJetton);
	//历史成绩
	void SetHistoryScore(WORD wDrawCount,__int64 lMeResultCount);
	//历史记录
	void SetGameHistory(enOperateResult OperateResult, BYTE cbPlayerCount, BYTE cbBankerCount);
	//轮换庄家
	void ShowChangeBanker( bool bChangeBanker );
	//庄家信息
	void SetBankerInfo( WORD wChairID, BYTE cbBankerTime, __int64 lScore );
	//庄家金币
	void SetBankerTreasure(__int64 lBankerTreasure);

	//成绩设置
	void SetGameScore(__int64 lMeCurGameScore, __int64 lMeCurGameReturnScore, __int64 lBankerCurGameScore);
	//设置标识
	void SetShowGameFlag(bool bShowGameResult);
	//发牌标识
	void SetDispatchCardFalg(bool bDispatchCard){ m_bDispatchCard = bDispatchCard; UpdateGameView(NULL);};
	//发牌提示
	void SetDispatchCardTips(CString const &strTips) { m_strDispatchCardTips = strTips; UpdateGameView(NULL);};

	//计算限制
public:
	//最大下注
	__int64 GetMaxPlayerScore();
	//最大下注
	__int64 GetMaxPlayerKingScore();
	//最大下注
	__int64 GetMaxBankerScore();
	//最大下注
	__int64 GetMaxBankerKingScore();
	//最大下注
	__int64 GetMaxTieScore();
	//最大下注
	__int64 GetMaxTieKingScore();
	//计算所有下注总和
	__int64 CalcAllJetton();
	//界面函数
public:
	//清理筹码
	void CleanUserJetton();
	//设置筹码
	void PlaceUserJetton(BYTE cbViewIndex, __int64 lScoreCount);
	//设置胜方
	void SetWinnerSide(LPCTSTR pszGameResult, BYTE cbWinnerSide, BYTE cbKingWinner);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//绘画数字
	void DrawMeJettonNumber(CDC *pDC);
	//删除筹码
	void RemoveUserJetton(BYTE cbViewIndex, __int64 lScoreCount);

	//内联函数
public:
	//当前筹码
	inline __int64 GetCurrentJetton() { return m_lCurrentJetton; }
	//我的位置
	inline void SetMeChairID(WORD wMeChairID) { m_wMeChairID=wMeChairID; }
	inline WORD GetMeChairID() const {return m_wMeChairID;}
	//内部函数
private:
	//获取区域
	BYTE GetJettonArea(CPoint MousePoint);
	//绘画数字
	void DrawNumberString(CDC * pDC, __int64 lNumber, INT nXPos, INT nYPos, bool bMeScore = false);
	//绘画标识
	void DrawWinFlags(CDC * pDC);
	//显示结果
	void ShowGameResult(CDC *pDC, int nWidth, int nHeight);
	//透明绘画
	bool DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha);

	//按钮消息
protected:
	//筹码按钮
	afx_msg void OnJettonButton10000000();
	//筹码按钮
	afx_msg void OnJettonButton1000();
	//筹码按钮
	afx_msg void OnJettonButton10000();
	//筹码按钮
	afx_msg void OnJettonButton100000();
	//筹码按钮
	afx_msg void OnJettonButton500000();
	//筹码按钮
	afx_msg void OnJettonButton1000000();
	//筹码按钮
	afx_msg void OnJettonButton5000000();
	//上庄按钮
	afx_msg void OnApplyBanker();
	//下庄按钮
	afx_msg void OnCancelBanker();
	//移动按钮
	afx_msg void OnScoreMoveL();
	//移动按钮
	afx_msg void OnScoreMoveR();
	afx_msg void OnBank();
	afx_msg void OnUp();

	afx_msg void OnDown();
	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);	
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);	
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif