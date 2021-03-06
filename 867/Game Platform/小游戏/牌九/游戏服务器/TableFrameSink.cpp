#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//常量定义
#define SEND_COUNT					8									//发送次数

//索引定义
//#define INDEX_PLAYER1				0									//闲家索引
//#define INDEX_BANKER				1									//庄家索引

#define INDEX_BANKER				0									//庄家索引
#define INDEX_PLAYER1				1									//闲家1索引
#define INDEX_PLAYER2				2									//闲家1索引
#define INDEX_PLAYER3				3									//闲家1索引

//下注时间
#define IDI_PLACE_JETTON			1									//下注时间
#define IDI_GAME_FREE				3									//空闲阶段

#ifdef _DEBUG
#define TIME_PLACE_JETTON			25									//下注时间
#else
#define TIME_PLACE_JETTON			25									//下注时间
#endif
#define TIME_FREE					10									//空闲时间

//结束时间
#define IDI_GAME_END				2									//结束时间
#ifdef _DEBUG
#define TIME_GAME_END				20									//结束时间
#else
#define TIME_GAME_END				20									//结束时间
#endif

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//游戏人数
const enStartMode	CTableFrameSink::m_GameStartMode=enStartMode_TimeControl;	//开始模式

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//下注信息
	m_lTianMenScore=0L;
	m_lDaoMenScore=0L;
	m_lShunMenScore=0L;
	m_lQiaoScore=0L;
	m_lYouJiaoScore=0L;
	m_lZuoJiaoScore=0L;

	//状态变量
	m_dwJettonTime=0L;

	//下注信息
	ZeroMemory(m_lUserTianMenScore,sizeof(m_lUserTianMenScore));
	ZeroMemory(m_lUserDaoMenScore,sizeof(m_lUserDaoMenScore));
	ZeroMemory(m_lUserShunMenScore,sizeof(m_lUserShunMenScore));
	ZeroMemory(m_lUserQiaoScore,sizeof(m_lUserQiaoScore));
	ZeroMemory(m_lUserYouJiaoScore,sizeof(m_lUserYouJiaoScore));
	ZeroMemory(m_lUserZuoJiaoScore,sizeof(m_lUserZuoJiaoScore));

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//庄家变量
	m_lApplyBankerCondition = BANK_CONDITION_MONEY;
	ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
	m_cbBankerTimer=0;
	m_lBankerWinScore = 0;
	m_bCancelBanker=false;
	
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pITableFrameControl=NULL;

	//游戏记录
	ZeroMemory(m_GameRecordArrary, sizeof(m_GameRecordArrary));
	m_nRecordFirst=0;	
	m_nRecordLast=0;	

	//读取分数
	m_lApplyBankerCondition = BANK_CONDITION_MONEY;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void * __cdecl CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//控制接口
	m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameControl);
	if (m_pITableFrameControl==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	m_pITableFrame->SetGameStatus(GS_FREE);

	return true;
}

//复位桌子
void __cdecl CTableFrameSink::RepositTableFrameSink()
{
	//下注信息
	m_lTianMenScore=0L;
	m_lDaoMenScore=0L;
	m_lShunMenScore=0L;
	m_lQiaoScore=0L;
	m_lYouJiaoScore=0L;
	m_lZuoJiaoScore=0L;

	//下注信息
	ZeroMemory(m_lUserTianMenScore,sizeof(m_lUserTianMenScore));
	ZeroMemory(m_lUserDaoMenScore,sizeof(m_lUserDaoMenScore));
	ZeroMemory(m_lUserShunMenScore,sizeof(m_lUserShunMenScore));
	ZeroMemory(m_lUserQiaoScore,sizeof(m_lUserQiaoScore));
	ZeroMemory(m_lUserYouJiaoScore,sizeof(m_lUserYouJiaoScore));
	ZeroMemory(m_lUserZuoJiaoScore,sizeof(m_lUserZuoJiaoScore));

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	return;
}

//开始模式
enStartMode __cdecl CTableFrameSink::GetGameStartMode()
{
	return m_GameStartMode;
}

//游戏状态
bool __cdecl CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//游戏开始
bool __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_PLAYING);
	//发送扑克
	DispatchTableCard();
	RobotAI();
	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));
	//构造数据
	GameStart.cbTimeLeave=TIME_GAME_END;//剩余时间  13秒		
	CopyMemory(GameStart.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));
	CopyMemory(GameStart.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
	GameStart.lApplyBankerCondition = m_lApplyBankerCondition;//申请做庄的条件
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	//计算分数
	CalculateScore();

	return true;
}

//游戏结束
bool __cdecl CTableFrameSink::OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			GameEnd.nBankerTime = m_cbBankerTimer;
			GameEnd.lBankerTotalScore = m_lBankerWinScore;

			//推断玩家
			DeduceWinner(GameEnd.cbWinner, GameEnd.cbKingWinner);			

			//写入积分
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ) continue;

				//胜利类型
				enScoreKind ScoreKind=(m_lUserWinScore[wUserChairID]>0L)?enScoreKind_Win:enScoreKind_Lost;

				//写入积分
				if (m_lUserWinScore[wUserChairID]!=0L)
				{
					m_pITableFrame->WriteUserScore(wUserChairID,m_lUserWinScore[wUserChairID], 0, ScoreKind);
				}
				//庄家判断
				if ( m_CurrentBanker.dwUserID == pIServerUserItem->GetUserID() ) m_CurrentBanker.lUserScore = pIServerUserItem->GetUserScore()->lScore;
			}

			//发送积分
			GameEnd.cbTimeLeave=TIME_FREE;	
			if ( m_CurrentBanker.dwUserID != 0 ) GameEnd.lBankerTreasure = m_CurrentBanker.lUserScore;
			for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//发送消息					
				GameEnd.lCellScore=m_pGameServiceOption->lCellScore;
				GameEnd.lMeMaxScore=pIServerUserItem->GetUserScore()->lScore;
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			//TCHAR szPokerInfo[MAX_DB_JETTON_LEN]={0};
			//TCHAR szPointInfo[MAX_DB_JETTON_LEN]={0};

			//CString str;
			//str.Format("张数:",m_cbCardCount);
			//OutputDebugString(str);
			//str.Format()
			//OutputRoundInfomation(szPokerInfo, "张数:", m_cbCardCount, sizeof(m_cbCardCount));
			//OutputRoundInfomation(szPokerInfo, "扑克:", m_cbTableCardArray, sizeof(m_cbTableCardArray));
			//_snprintf(szPointInfo, CountArray(szPointInfo), TEXT("Winner:%d KingWinner:%d"), GameEnd.cbWinner, GameEnd.cbKingWinner);
			//m_pITableFrame->WriteRoundInfomation(szPokerInfo, szPointInfo, m_CurrentBanker.dwUserID, m_lBankerWinScore);

			//庄家判断
			if ( m_CurrentBanker.dwUserID != 0 )
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
				if (pIServerUserItem) 
				{
					//积分判断
					if ( pIServerUserItem->GetUserScore()->lScore < m_lApplyBankerCondition || m_cbBankerTimer >= 20 )
					{
						//玩家下庄
						OnUserApplyBanker( pIServerUserItem->GetUserData(), false );
					}
				}
			}

			//结束游戏
			m_pITableFrame->ConcludeGame();

			return true;
		}
	case GER_USER_LEFT:	//用户强退
		{
			__int64	allZhu=0;
			//闲家强退
			if ( pIServerUserItem->GetUserID() != m_CurrentBanker.dwUserID ) 
			{
				//变量定义
				__int64 lScore=0;
				enScoreKind ScoreKind=enScoreKind_Flee;
				if (m_pITableFrame->GetGameStatus() == GS_FREE + 1)
				{
					//在押注状态下，退出不扣分
					JettonChangeByUserLeft( wChairID, pIServerUserItem);
				}
				else if(m_pITableFrame->GetGameStatus() == GS_PLAYING)
				{
					lScore = m_lUserWinScore[wChairID];
				}

				//防止超过用户携带金币
				if(lScore < 0 && -lScore > pIServerUserItem->GetUserScore()->lScore)
					lScore = -(pIServerUserItem->GetUserScore()->lScore);

				if (lScore != 0 )
					m_pITableFrame->WriteUserScore(pIServerUserItem, lScore,0, ScoreKind);

				//清理计算的结果
				m_lUserWinScore[wChairID]=0;
			}
			else
			{
				//变量定义
				__int64 lScore=0;
				enScoreKind ScoreKind=enScoreKind_Flee;
				//计算所扣倍数
				if (m_pITableFrame->GetGameStatus()== GS_FREE+1)
				{
					//提示消息
					TCHAR szTipMsg[128];
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于庄家[ %s ]强退，游戏提前结束！"),pIServerUserItem->GetAccounts());
					SendGameMessage(INVALID_CHAIR,szTipMsg);	

					m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
					m_pITableFrameControl->StartGame();
					m_dwJettonTime=(DWORD)time(NULL);
					m_pITableFrame->SetGameTimer(IDI_GAME_END,TIME_GAME_END*1000,1,0L);
				}
				lScore=m_lUserWinScore[m_CurrentBanker.wChairID];
				if (lScore != 0)
					m_pITableFrame->WriteUserScore(pIServerUserItem, lScore,0, ScoreKind);
				m_lUserWinScore[m_CurrentBanker.wChairID]=0;	
			}
			return true;
		}
	}

	return false;
}

//发送场景
bool __cdecl CTableFrameSink::SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_FREE:			//空闲状态
		{
			//发送游戏记录
			WORD wBufferSize=0;
			BYTE cbBuffer[SOCKET_PACKAGE];
			int nIndex = m_nRecordFirst;
			while ( nIndex != m_nRecordLast )
			{
				if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
				{
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
					wBufferSize=0;
				}
				CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
				wBufferSize+=sizeof(tagServerGameRecord);

				nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
			}
			if (wBufferSize>0) 
				m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			//下注信息
			StatusFree.lTieScore=m_lTianMenScore;
			StatusFree.lBankerScore=m_lDaoMenScore;
			StatusFree.lPlayerScore=m_lShunMenScore;
			StatusFree.lTieSamePointScore = m_lQiaoScore;
			StatusFree.lBankerKingScore = m_lYouJiaoScore;
			StatusFree.lPlayerKingScore = m_lZuoJiaoScore;
			//庄家信息
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;

			//下注信息
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lMeTieScore=m_lUserTianMenScore[wChiarID];
				StatusFree.lMeBankerScore=m_lUserDaoMenScore[wChiarID];
				StatusFree.lMePlayerScore=m_lUserShunMenScore[wChiarID];
				StatusFree.lMeTieKingScore = m_lUserQiaoScore[wChiarID];
				StatusFree.lMeBankerKingScore = m_lUserYouJiaoScore[wChiarID];
				StatusFree.lMePlayerKingScore = m_lUserZuoJiaoScore[wChiarID];
				StatusFree.lMeMaxScore=pIServerUserItem->GetUserScore()->lScore;
			}
	
			if ( m_CurrentBanker.dwUserID != 0 ) 
			{
				StatusFree.cbBankerTime = m_cbBankerTimer;
				StatusFree.lCurrentBankerScore = m_lBankerWinScore;
				StatusFree.wCurrentBankerChairID = m_CurrentBanker.wChairID;
				StatusFree.lBankerTreasure = m_CurrentBanker.lUserScore;
			}
			else StatusFree.wCurrentBankerChairID = INVALID_CHAIR;
			

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(TIME_FREE-__min(dwPassTime,TIME_FREE));

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
			
			//发送申请者
			SendApplyUser(  pIServerUserItem );

			return bSuccess;
		}
	case GS_FREE + 1:
		{
			//发送游戏记录
			WORD wBufferSize=0;
			BYTE cbBuffer[SOCKET_PACKAGE];
			int nIndex = m_nRecordFirst;
			while ( nIndex != m_nRecordLast )
			{
				if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
				{
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
					wBufferSize=0;
				}
				CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
				wBufferSize+=sizeof(tagServerGameRecord);

				nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
			}
			if (wBufferSize>0) 
				m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			//下注信息
			StatusFree.lTieScore=m_lTianMenScore;
			StatusFree.lBankerScore=m_lDaoMenScore;
			StatusFree.lPlayerScore=m_lShunMenScore;
			StatusFree.lTieSamePointScore = m_lQiaoScore;
			StatusFree.lBankerKingScore = m_lYouJiaoScore;
			StatusFree.lPlayerKingScore = m_lZuoJiaoScore;
			//庄家信息
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;

			//下注信息
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lMeTieScore=m_lUserTianMenScore[wChiarID];
				StatusFree.lMeBankerScore=m_lUserDaoMenScore[wChiarID];
				StatusFree.lMePlayerScore=m_lUserShunMenScore[wChiarID];
				StatusFree.lMeTieKingScore = m_lUserQiaoScore[wChiarID];
				StatusFree.lMeBankerKingScore = m_lUserYouJiaoScore[wChiarID];
				StatusFree.lMePlayerKingScore = m_lUserZuoJiaoScore[wChiarID];
				StatusFree.lMeMaxScore=pIServerUserItem->GetUserScore()->lScore;
			}

			if ( m_CurrentBanker.dwUserID != 0 ) 
			{
				StatusFree.cbBankerTime = m_cbBankerTimer;
				StatusFree.lCurrentBankerScore = m_lBankerWinScore;
				StatusFree.wCurrentBankerChairID = m_CurrentBanker.wChairID;
				StatusFree.lBankerTreasure = m_CurrentBanker.lUserScore;
			}
			else StatusFree.wCurrentBankerChairID = INVALID_CHAIR;

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(TIME_PLACE_JETTON-__min(dwPassTime,TIME_PLACE_JETTON));
			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
			//发送申请者
			SendApplyUser(  pIServerUserItem );
			return bSuccess;
		}
	case GS_PLAYING:		//游戏状态
		{
			//发送游戏记录
			WORD wBufferSize=0;
			BYTE cbBuffer[SOCKET_PACKAGE];
			int nIndex = m_nRecordFirst;
			while ( nIndex != m_nRecordLast )
			{
				if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
				{
					m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
					wBufferSize=0;
				}
				CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
				wBufferSize+=sizeof(tagServerGameRecord);

				nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
			}
			if (wBufferSize>0) 
				m_pITableFrame->SendUserData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);

			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay,sizeof(StatusPlay));
			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusPlay.cbTimeLeave=(BYTE)(TIME_GAME_END-__min(dwPassTime,TIME_GAME_END));
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			//下注信息
			StatusPlay.lTieScore=m_lTianMenScore;
			StatusPlay.lBankerScore=m_lDaoMenScore;
			StatusPlay.lPlayerScore=m_lShunMenScore;
			StatusPlay.lTieSamePointScore = m_lQiaoScore;
			StatusPlay.lBankerKingScore = m_lYouJiaoScore;
			StatusPlay.lPlayerKingScore = m_lZuoJiaoScore;

			//庄家信息
			StatusPlay.lApplyBankerCondition = m_lApplyBankerCondition;

			//下注信息
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusPlay.lMeTieScore=m_lUserTianMenScore[wChiarID];
				StatusPlay.lMeBankerScore=m_lUserDaoMenScore[wChiarID];
				StatusPlay.lMePlayerScore=m_lUserShunMenScore[wChiarID];
				StatusPlay.lMeTieKingScore = m_lUserQiaoScore[wChiarID];
				StatusPlay.lMeBankerKingScore = m_lUserYouJiaoScore[wChiarID];
				StatusPlay.lMePlayerKingScore = m_lUserZuoJiaoScore[wChiarID];
				StatusPlay.lMeMaxScore=pIServerUserItem->GetUserScore()->lScore;
			}

			if ( m_CurrentBanker.dwUserID != 0 ) 
			{
				StatusPlay.cbBankerTime = m_cbBankerTimer;
				StatusPlay.lCurrentBankerScore = m_lBankerWinScore;
				StatusPlay.wCurrentBankerChairID = m_CurrentBanker.wChairID;
				StatusPlay.lBankerTreasure = m_CurrentBanker.lUserScore;
			}
			else 
				StatusPlay.wCurrentBankerChairID = INVALID_CHAIR;

			//扑克信息
			CopyMemory(StatusPlay.cbCardCount,m_cbCardCount,sizeof(m_cbCardCount));
			CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
			//发送申请者
			SendApplyUser( pIServerUserItem );
			return bSuccess;
		}
	}

	return false;
}

//定时器事件
bool __cdecl CTableFrameSink::OnTimerMessage(WORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_GAME_FREE:
		{
			//刷新下庄家的分数
			IServerUserItem * pBankerItem=m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
			if(pBankerItem)
			{
				if(m_CurrentBanker.dwUserID != 0L)
					m_CurrentBanker.lUserScore=pBankerItem->GetUserScore()->lScore;
			}
			OnEventStartPlaceJetton();
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON, TIME_PLACE_JETTON*1000,1,0L);
			return true;
		}
	case IDI_PLACE_JETTON:		//下注时间
		{
			if(m_pITableFrame->GetGameStatus() != GS_FREE+1)
				return true;

			//刷新下庄家的分数
			IServerUserItem * pBankerItem=m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
			if(pBankerItem)
			{
				if(m_CurrentBanker.dwUserID != 0L)
					m_CurrentBanker.lUserScore=pBankerItem->GetUserScore()->lScore;
			}
			//开始游戏
			m_pITableFrameControl->StartGame();
			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_GAME_END,TIME_GAME_END*1000,1,0L);

			return true;
		}
	case IDI_GAME_END:			//结束游戏
		{
			//刷新下庄家的分数
			IServerUserItem * pBankerItem=m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
			if(pBankerItem)
			{
				if(m_CurrentBanker.dwUserID != 0L)
					m_CurrentBanker.lUserScore=pBankerItem->GetUserScore()->lScore;
			}
			//结束游戏
			OnEventGameEnd(INVALID_CHAIR,NULL,GER_NORMAL);
			//下庄判断
			if ( m_bCancelBanker && m_CurrentBanker.dwUserID != 0 )
			{		
				//获取玩家
				IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_CurrentBanker.wChairID );

				//重置变量
				m_cbBankerTimer = 0;
				m_lBankerWinScore=0;
				ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
				m_bCancelBanker=false;
				
				//发送消息
				SendChangeBankerMsg();

				//删除庄家
				if ( pServerUserItem ) OnUserApplyBanker( pServerUserItem->GetUserData(), false );
			}

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_GAME_FREE,TIME_FREE*1000L,1,0L);
			m_pITableFrame->SetGameStatus(GS_FREE);

			//轮换庄家
			ChangeBanker();

			//庄家信息
			if ( m_CurrentBanker.dwUserID != 0 )
			{
				CMD_S_ChangeUserScore ChangeUserScore;
				ZeroMemory( &ChangeUserScore, sizeof( ChangeUserScore ) );
				ChangeUserScore.wCurrentBankerChairID = m_CurrentBanker.wChairID;
				ChangeUserScore.lCurrentBankerScore = m_lBankerWinScore;
				ChangeUserScore.cbBankerTime = m_cbBankerTimer;
				ChangeUserScore.lScore = m_CurrentBanker.lUserScore;
				ChangeUserScore.wChairID = m_CurrentBanker.wChairID;
				m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_CHANGE_USER_SCORE,&ChangeUserScore,sizeof(ChangeUserScore));
				m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHANGE_USER_SCORE,&ChangeUserScore,sizeof(ChangeUserScore));
			}

			//切换判断
			if ( m_cbBankerTimer == 0 )
			{
				//发送消息
				SendChangeBankerMsg();
			}

			return true;
		}
	}

	return false;
}

//游戏消息处理
bool __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize!=sizeof(CMD_C_PlaceJetton)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if (pUserData->cbUserStatus!=US_SIT) return true;

			//消息处理
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pDataBuffer;
			return OnUserPlaceJetton(pUserData->wChairID,pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
		}
	case SUB_C_APPLY_BANKER:		//申请做庄
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_ApplyBanker));
			if (wDataSize!=sizeof(CMD_C_ApplyBanker)) return false;

			//用户效验
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			//消息处理
			CMD_C_ApplyBanker * pApplyBanker=(CMD_C_ApplyBanker *)pDataBuffer;

			return OnUserApplyBanker( pUserData, pApplyBanker->bApplyBanker );	
		}
	}

	return false;
}

//框架消息处理
bool __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool __cdecl CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置时间
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_GAME_FREE, TIME_FREE*1000L,1,NULL);
	}

	return true;
}

//用户起来
bool __cdecl CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//记录成绩
	if (bLookonUser==false)
	{
		if ( m_pITableFrame->GetGameStatus() == GS_FREE+1 )
		{
			OnEventGameEnd( wChairID, pIServerUserItem, GER_USER_LEFT);
		}

		//存在判断
		for ( INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArrary.GetCount(); ++nUserIdx )
		{
			tagApplyUserInfo ApplyUserInfo = m_ApplyUserArrary[ nUserIdx ];

			if ( ApplyUserInfo.dwUserID == pIServerUserItem->GetUserID() )
			{
				//删除玩家
				m_ApplyUserArrary.RemoveAt( nUserIdx );

				//构造变量
				CMD_S_ApplyBanker ApplyBanker;
				CopyMemory( ApplyBanker.szAccount, pIServerUserItem->GetAccounts(), sizeof( ApplyBanker.szAccount ) );
				ApplyBanker.lScore = pIServerUserItem->GetUserScore()->lScore;
				ApplyBanker.bApplyBanker = false;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );

				//当前判断
				if ( ApplyUserInfo.dwUserID == m_CurrentBanker.dwUserID )
				{
					//重置变量
					ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
					m_cbBankerTimer=0;
					m_lBankerWinScore=0;

					//发送消息
					SendChangeBankerMsg();
				}
			}
		}
		if(m_pITableFrame->GetGameStatus() ==GS_FREE)
		{
			if (m_CurrentBanker.dwUserID == 0)
			{
				ChangeBanker();
				//庄家信息
				if ( m_CurrentBanker.dwUserID != 0 )
				{
					CMD_S_ChangeUserScore ChangeUserScore;
					ZeroMemory( &ChangeUserScore, sizeof( ChangeUserScore ) );
					ChangeUserScore.wCurrentBankerChairID = m_CurrentBanker.wChairID;
					ChangeUserScore.lCurrentBankerScore = m_lBankerWinScore;
					ChangeUserScore.cbBankerTime = m_cbBankerTimer;
					ChangeUserScore.lScore = m_CurrentBanker.lUserScore;
					ChangeUserScore.wChairID = m_CurrentBanker.wChairID;
					m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_CHANGE_USER_SCORE,&ChangeUserScore,sizeof(ChangeUserScore));
					m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHANGE_USER_SCORE,&ChangeUserScore,sizeof(ChangeUserScore));
				}
				//切换判断
				if ( m_cbBankerTimer == 0 )
				{
					SendChangeBankerMsg();
				}
			}
		}

		//WORD wNowCount=m_pITableFrame->GetNowPlayerCount();
		//CString str;
		//str.Format("房间还剩人数：%d",wNowCount);
		//OutputDebugString(str);
		//if ( wNowCount==0 )
		//{
		//	m_dwJettonTime=0;
		//	m_pITableFrame->KillGameTimer( IDI_GAME_FREE );
		//	m_pITableFrame->KillGameTimer( IDI_PLACE_JETTON );
		//}
	}

	return true;
}

bool CTableFrameSink::IsBigBanker(WORD wChairID, BYTE cbJettonArea, __int64 lJettonScore)
{
	__int64 all=m_lTianMenScore+m_lDaoMenScore+m_lShunMenScore+m_lQiaoScore+m_lYouJiaoScore+m_lZuoJiaoScore+lJettonScore;
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
	if (pIServerUserItem)
	{
		if ( all>pIServerUserItem->GetUserScore()->lScore )
		{
			return true;
		}
	}
	return false;
}

//加注事件
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, __int64 lJettonScore, bool bRobot)
{
	//效验参数
	ASSERT((cbJettonArea<=ID_QIAO)&&(lJettonScore>0L));
	if ((cbJettonArea>ID_QIAO)||(lJettonScore<=0L)) return false;

	//效验状态
	ASSERT(m_pITableFrame->GetGameStatus()==GS_FREE+1);
	if (m_pITableFrame->GetGameStatus()!=GS_FREE+1) return true;

	//刷新下庄家的分数
	IServerUserItem * pBankerItem=m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
	if(pBankerItem)
	{
		if(m_CurrentBanker.dwUserID != 0L)
			m_CurrentBanker.lUserScore=pBankerItem->GetUserScore()->lScore;
	}

	//庄家判断
	if ( m_CurrentBanker.dwUserID != 0 && m_CurrentBanker.wChairID == wChairID ) 
		return true;
	if ( m_CurrentBanker.dwUserID == 0 ) 
		return true;
	//判断玩家是不是点的按钮下注
	bool illegal=true;
	__int64 lScoreJetton[7]={1000L,10000L,100000L,500000L,1000000L,5000000L,10000000L};
	for(int i=0;i<7;++i)
	{
		if(lJettonScore == lScoreJetton[i])
			illegal = false;
	}
	if(illegal)
		return true;
	//变量定义
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
	__int64 lJettonCount=m_lUserTianMenScore[wChairID]+m_lUserDaoMenScore[wChairID]+m_lUserShunMenScore[wChairID]+
		m_lUserQiaoScore[wChairID] + m_lUserZuoJiaoScore[wChairID] + m_lUserYouJiaoScore[wChairID];
	__int64 lUserScore = pIServerUserItem->GetUserScore()->lScore;

	if ( lUserScore < lJettonCount + lJettonScore ) return true;

	if (IsBigBanker(wChairID, cbJettonArea, lJettonScore)==true)  return true;

	//合法验证
	if ( ID_SHUN_MEN == cbJettonArea )
	{
		if ( GetMaxPlayerScore(wChairID) < lJettonScore ) return true;

		//保存下注
		m_lShunMenScore += lJettonScore;
		m_lUserShunMenScore[wChairID] += lJettonScore;
	}
	else if ( ID_ZUO_JIAO == cbJettonArea )
	{
		if ( GetMaxPlayerKingScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lZuoJiaoScore += lJettonScore;
		m_lUserZuoJiaoScore[wChairID] += lJettonScore;
	}
	else if ( ID_TIAN_MEN == cbJettonArea )
	{
		if ( GetMaxTieScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lTianMenScore += lJettonScore;
		m_lUserTianMenScore[wChairID] += lJettonScore;
	}
	else if ( ID_QIAO == cbJettonArea )
	{
		if ( GetMaxTieKingScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lQiaoScore += lJettonScore;
		m_lUserQiaoScore[wChairID] += lJettonScore;
	}
	else if ( ID_DAO_MEN == cbJettonArea )
	{
		if ( GetMaxBankerScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lDaoMenScore += lJettonScore;
		m_lUserDaoMenScore[wChairID] += lJettonScore;
	}
	else if ( ID_YOU_JIAO == cbJettonArea )
	{
		if ( GetMaxBankerKingScore(wChairID) <lJettonScore ) return true;

		//保存下注
		m_lYouJiaoScore += lJettonScore;
		m_lUserYouJiaoScore[wChairID] += lJettonScore;
	}
	else
	{
		ASSERT(FALSE);
		return true;
	}

	//变量定义
	CMD_S_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//构造变量
	PlaceJetton.wChairID=wChairID;
	PlaceJetton.cbJettonArea=cbJettonArea;
	PlaceJetton.lJettonScore=lJettonScore;
	pIServerUserItem=m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);
	if (pIServerUserItem)
	{
		PlaceJetton.lKeXiaSocre=pIServerUserItem->GetUserScore()->lScore; 
	}
	PlaceJetton.lAllJettonScore=m_lTianMenScore+m_lDaoMenScore+m_lShunMenScore+m_lQiaoScore+m_lYouJiaoScore+m_lZuoJiaoScore;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

	return true;
}

//发送扑克
bool CTableFrameSink::DispatchTableCard()
{
	m_GameLogic.RandCardList(m_cbTableCardArray[0],sizeof(m_cbTableCardArray)/sizeof(m_cbTableCardArray[0][0]));
	//首次发牌
	m_cbCardCount[INDEX_PLAYER1]=2;
	m_cbCardCount[INDEX_BANKER]=2;
	return true;
}

//盈利判断
__int64 CTableFrameSink::AccountPayoffScore()
{
	//计算点数
	BYTE cbPlayerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER1],m_cbCardCount[INDEX_PLAYER1]);
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//盈利计算
	__int64 lPayoffScore=0L;
	if (cbPlayerCount>cbBankerCount) lPayoffScore=m_lTianMenScore+m_lDaoMenScore-m_lShunMenScore;
	else if (cbPlayerCount<cbBankerCount) lPayoffScore=m_lTianMenScore+m_lShunMenScore-m_lDaoMenScore;
	else lPayoffScore=-m_lTianMenScore*8L;

	return lPayoffScore;
}

//申请庄家
bool CTableFrameSink::OnUserApplyBanker( tagServerUserData *pUserData, bool bApplyBanker )
{
	//合法判断
	if ( bApplyBanker && pUserData->UserScoreInfo.lScore < m_lApplyBankerCondition ) return true;

	//保存玩家
	if ( bApplyBanker )
	{
		//存在判断
		for ( INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArrary.GetCount(); ++nUserIdx )
		{
			tagApplyUserInfo ApplyUserInfo = m_ApplyUserArrary[ nUserIdx ];
			if ( ApplyUserInfo.dwUserID == pUserData->dwUserID ) return true;
		}

		//构造信息
		tagApplyUserInfo ApplyUserInfo;
		ApplyUserInfo.dwUserID = pUserData->dwUserID;
		ApplyUserInfo.lUserScore = pUserData->UserScoreInfo.lScore;
		ApplyUserInfo.wChairID = pUserData->wChairID;
		ApplyUserInfo.dwUserType = pUserData->cbMasterOrder;

		//插入玩家
		INT_PTR nUserCount = m_ApplyUserArrary.GetCount();
		m_ApplyUserArrary.InsertAt( nUserCount, ApplyUserInfo );

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		CopyMemory( ApplyBanker.szAccount, pUserData->szAccounts, sizeof( ApplyBanker.szAccount ) );
		ApplyBanker.lScore = pUserData->UserScoreInfo.lScore;
		ApplyBanker.bApplyBanker = true;

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
	}
	else	//取消申请
	{
		//存在判断
		for ( INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArrary.GetCount(); ++nUserIdx )
		{
			tagApplyUserInfo ApplyUserInfo = m_ApplyUserArrary[ nUserIdx ];

			if ( ApplyUserInfo.dwUserID == pUserData->dwUserID )
			{
				//当前判断
				if ( m_CurrentBanker.dwUserID == ApplyUserInfo.dwUserID )
				{
					m_bCancelBanker = true;
					continue;
				}

				//删除玩家
				m_ApplyUserArrary.RemoveAt( nUserIdx );

				//构造变量
				CMD_S_ApplyBanker ApplyBanker;
				CopyMemory( ApplyBanker.szAccount, pUserData->szAccounts, sizeof( ApplyBanker.szAccount ) );
				ApplyBanker.lScore = pUserData->UserScoreInfo.lScore;
				ApplyBanker.bApplyBanker = false;

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );

				break;
			}
		}

		return true;
	}
	if ( m_pITableFrame->GetGameStatus() == GS_FREE )
	{
		if ( m_bCancelBanker && m_CurrentBanker.dwUserID != 0 )
		{		
			//获取玩家
			IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_CurrentBanker.wChairID );
			//重置变量
			m_cbBankerTimer = 0;
			m_lBankerWinScore=0;
			ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
			m_bCancelBanker=false;
			//发送消息
			SendChangeBankerMsg();
			if ( pServerUserItem )
				OnUserApplyBanker( pServerUserItem->GetUserData(), false );
		}
		if (m_CurrentBanker.dwUserID == 0)
		{
			//轮换庄家
			ChangeBanker();
			//庄家信息
			if ( m_CurrentBanker.dwUserID != 0 )
			{
				CMD_S_ChangeUserScore ChangeUserScore;
				ZeroMemory( &ChangeUserScore, sizeof( ChangeUserScore ) );
				ChangeUserScore.wCurrentBankerChairID = m_CurrentBanker.wChairID;
				ChangeUserScore.lCurrentBankerScore = m_lBankerWinScore;
				ChangeUserScore.cbBankerTime = m_cbBankerTimer;
				ChangeUserScore.lScore = m_CurrentBanker.lUserScore;
				ChangeUserScore.wChairID = m_CurrentBanker.wChairID;
				m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_CHANGE_USER_SCORE,&ChangeUserScore,sizeof(ChangeUserScore));
				m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHANGE_USER_SCORE,&ChangeUserScore,sizeof(ChangeUserScore));
			}
			//切换判断
			if ( m_cbBankerTimer == 0 )
			{
				//发送消息
				SendChangeBankerMsg();
			}
		}
	}
	return true;
}

//发送庄家
void CTableFrameSink::SendApplyUser( IServerUserItem *pRcvServerUserItem )
{
	for ( INT_PTR nUserIdx = 0; nUserIdx < m_ApplyUserArrary.GetCount(); ++nUserIdx )
	{
		tagApplyUserInfo ApplyUserInfo = m_ApplyUserArrary[ nUserIdx ];

		//获取玩家
		IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( ApplyUserInfo.wChairID );
		if ( ! pServerUserItem ) continue;

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		CopyMemory( ApplyBanker.szAccount, pServerUserItem->GetAccounts(), sizeof( ApplyBanker.szAccount ) );
		ApplyBanker.lScore = pServerUserItem->GetUserScore()->lScore;
		ApplyBanker.bApplyBanker = true;

		//发送消息
		m_pITableFrame->SendUserData( pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof( ApplyBanker ) );
	}
}

//更换庄家
void CTableFrameSink::ChangeBanker()
{
	m_lApplyBankerCondition = BANK_CONDITION_MONEY;

	//递增次数
	m_cbBankerTimer++;

	//轮庄判断
	if ( m_CurrentBanker.dwUserID != 0 )
	{
		//轮换判断
		bool bChangeBanker = false;
		IServerUserItem *pBankerUserItem = m_pITableFrame->GetServerUserItem(m_CurrentBanker.wChairID);

		__int64 lBankerScore = pBankerUserItem->GetUserScore()->lScore;

		for ( WORD wApplyUserIndex = 0; wApplyUserIndex < m_ApplyUserArrary.GetCount(); ++wApplyUserIndex )
		{
			tagApplyUserInfo &ApplyUserInfo = m_ApplyUserArrary[ wApplyUserIndex ];
			IServerUserItem *pApplyUserItem = m_pITableFrame->GetServerUserItem(ApplyUserInfo.wChairID);
			__int64 lApplyUserScore = pApplyUserItem->GetUserScore()->lScore;
			if ( lBankerScore < lApplyUserScore )
			{
				bChangeBanker = true;
				break;
			}
		}

		//次数判断
		if ( bChangeBanker && 10 <= m_cbBankerTimer )
		{
			m_cbBankerTimer = 0;
			m_lBankerWinScore=0;
			m_bCancelBanker = false;

			//客户端删除
			IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_CurrentBanker.wChairID );
			OnUserApplyBanker( pServerUserItem->GetUserData(), false ) ;

			//重新赋值
			ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
			while( ! m_ApplyUserArrary.IsEmpty() )
			{
				m_CurrentBanker = m_ApplyUserArrary[ 0 ];
				IServerUserItem * newBanker = m_pITableFrame->GetServerUserItem( m_CurrentBanker.wChairID );
				if ( newBanker->GetUserScore()->lScore < m_lApplyBankerCondition )
				{
					ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );

					//获取玩家
					IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_ApplyUserArrary[ 0 ].wChairID );
					if (  pServerUserItem  ) OnUserApplyBanker( pServerUserItem->GetUserData(), false );
				}
				else
					break;
			}
		}
	}
	else if ( 0 < m_ApplyUserArrary.GetCount() )
	{
		while( ! m_ApplyUserArrary.IsEmpty() )
		{
			m_CurrentBanker = m_ApplyUserArrary[ 0 ];

			IServerUserItem * newBanker = m_pITableFrame->GetServerUserItem( m_CurrentBanker.wChairID );
			if ( newBanker )
				m_CurrentBanker.lUserScore=newBanker->GetUserScore()->lScore;

			if ( m_CurrentBanker.lUserScore < m_lApplyBankerCondition )
			{
				ZeroMemory( &m_CurrentBanker, sizeof( m_CurrentBanker ) );
				//获取玩家
				IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem( m_ApplyUserArrary[ 0 ].wChairID );
				if (  pServerUserItem  ) OnUserApplyBanker( pServerUserItem->GetUserData(), false );
			}
			else
				break;
		}
		m_cbBankerTimer = 0;
		m_lBankerWinScore=0;
		m_bCancelBanker = false;
	}
}

//轮换庄家
void CTableFrameSink::SendChangeBankerMsg()
{
	CMD_S_ChangeBanker ChangeBanker;

	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		IServerUserItem *pSeverUserItem = m_pITableFrame->GetServerUserItem( wChairID );
		if ( ! pSeverUserItem ) continue;

		ZeroMemory( &ChangeBanker, sizeof( ChangeBanker ) );
		if ( m_CurrentBanker.dwUserID != 0 )
		{
			ChangeBanker.wChairID = m_CurrentBanker.wChairID;
			ChangeBanker.lBankerTreasure = m_CurrentBanker.lUserScore;
		}
		else
		{
			ChangeBanker.wChairID = INVALID_CHAIR;
		}
		ChangeBanker.lBankerScore = m_CurrentBanker.lUserScore;
		ChangeBanker.cbBankerTime = m_cbBankerTimer;

		//我的下注
		m_pITableFrame->SendTableData( wChairID, SUB_S_CHANGE_BANKER, &ChangeBanker, sizeof( ChangeBanker ) );
		m_pITableFrame->SendLookonData( wChairID, SUB_S_CHANGE_BANKER, &ChangeBanker, sizeof( ChangeBanker ) );
	}
}

//用户断线
bool __cdecl CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	return true;
}

//最大下注
__int64 CTableFrameSink::GetMaxPlayerScore(WORD wChairID)//顺门
{
	//其他区域

	__int64 lAllAreaScore = m_lDaoMenScore+ m_lTianMenScore+ m_lQiaoScore+ m_lYouJiaoScore+m_lZuoJiaoScore+m_lShunMenScore;
	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	__int64 lMaxPlayerScore = m_CurrentBanker.lUserScore-lAllAreaScore;

	//已下注额
	__int64 lNowJetton = 0;
	lNowJetton += m_lUserShunMenScore[wChairID];
	lNowJetton += m_lUserZuoJiaoScore[wChairID];
	lNowJetton += m_lUserDaoMenScore[wChairID];
	lNowJetton += m_lUserTianMenScore[wChairID];
	lNowJetton += m_lUserQiaoScore[wChairID];
	lNowJetton += m_lUserYouJiaoScore[wChairID];

	//我的下注
	__int64 lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	ASSERT(lMeLessScore >= 0 );
	lMeLessScore = max(lMeLessScore, 0);

	//最大下注
	__int64 lMaxJetton = min(lMaxPlayerScore, lMeLessScore);

	return lMaxJetton;
}

//最大下注
__int64 CTableFrameSink::GetMaxPlayerKingScore(WORD wChairID)//左角
{
	//其他区域
	__int64 lAllAreaScore = m_lDaoMenScore+ m_lTianMenScore+ m_lQiaoScore+ m_lYouJiaoScore+m_lZuoJiaoScore+m_lShunMenScore;
	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	__int64 lMaxPlayerScore = m_CurrentBanker.lUserScore-lAllAreaScore;

	//已下注额
	__int64 lNowJetton = 0;
	lNowJetton += m_lUserShunMenScore[wChairID];
	lNowJetton += m_lUserZuoJiaoScore[wChairID];
	lNowJetton += m_lUserDaoMenScore[wChairID];
	lNowJetton += m_lUserTianMenScore[wChairID];
	lNowJetton += m_lUserQiaoScore[wChairID];
	lNowJetton += m_lUserYouJiaoScore[wChairID];

	//我的下注
	__int64 lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	//最大下注
	__int64 lMaxJetton = min(lMaxPlayerScore, lMeLessScore);

	return lMaxJetton;
}

//最大下注
__int64 CTableFrameSink::GetMaxBankerScore(WORD wChairID)//倒门
{
	//其他区域
	__int64 lAllAreaScore = m_lDaoMenScore+ m_lTianMenScore+ m_lQiaoScore+ m_lYouJiaoScore+m_lZuoJiaoScore+m_lShunMenScore;

	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	__int64 lMaxPlayerScore =m_CurrentBanker.lUserScore-lAllAreaScore;

	//已下注额
	__int64 lNowJetton = 0;
	lNowJetton += m_lUserShunMenScore[wChairID];
	lNowJetton += m_lUserZuoJiaoScore[wChairID];
	lNowJetton += m_lUserDaoMenScore[wChairID];
	lNowJetton += m_lUserTianMenScore[wChairID];
	lNowJetton += m_lUserQiaoScore[wChairID];
	lNowJetton += m_lUserYouJiaoScore[wChairID];

	//我的下注
	__int64 lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	__int64 lMaxJetton = min(lMaxPlayerScore, lMeLessScore);

	return lMaxJetton;
}

//最大下注
__int64 CTableFrameSink::GetMaxBankerKingScore(WORD wChairID)//右角
{
	//其他区域
	__int64 lAllAreaScore = m_lDaoMenScore+ m_lTianMenScore+ m_lQiaoScore+ m_lYouJiaoScore+m_lZuoJiaoScore+m_lShunMenScore;
	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	__int64 lMaxPlayerScore =  m_CurrentBanker.lUserScore-lAllAreaScore;

	//已下注额
	__int64 lNowJetton = 0;
	lNowJetton += m_lUserShunMenScore[wChairID];
	lNowJetton += m_lUserZuoJiaoScore[wChairID];
	lNowJetton += m_lUserDaoMenScore[wChairID];
	lNowJetton += m_lUserTianMenScore[wChairID];
	lNowJetton += m_lUserQiaoScore[wChairID];
	lNowJetton += m_lUserYouJiaoScore[wChairID];

	//我的下注
	__int64 lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	__int64 lMaxJetton = min(lMaxPlayerScore, lMeLessScore);

	return lMaxJetton;
}

//最大下注
__int64 CTableFrameSink::GetMaxTieScore(WORD wChairID)//天门
{
	__int64 lAllAreaScore = m_lDaoMenScore+ m_lTianMenScore+ m_lQiaoScore+ m_lYouJiaoScore+m_lZuoJiaoScore+m_lShunMenScore;

	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	__int64 lMaxTieScore = m_CurrentBanker.lUserScore-lAllAreaScore;

	//已下注额
	__int64 lNowJetton = 0;
	lNowJetton += m_lUserShunMenScore[wChairID];
	lNowJetton += m_lUserZuoJiaoScore[wChairID];
	lNowJetton += m_lUserDaoMenScore[wChairID];
	lNowJetton += m_lUserTianMenScore[wChairID];
	lNowJetton += m_lUserQiaoScore[wChairID];
	lNowJetton += m_lUserYouJiaoScore[wChairID];

	//我的下注
	__int64 lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	__int64 lMaxJetton = min(lMaxTieScore, lMeLessScore);

	return lMaxJetton;
}

//最大下注
__int64 CTableFrameSink::GetMaxTieKingScore(WORD wChairID)//桥
{
	__int64 lAllAreaScore = m_lDaoMenScore+ m_lTianMenScore+ m_lQiaoScore+ m_lYouJiaoScore+m_lZuoJiaoScore+m_lShunMenScore;

	IServerUserItem *pServerUserItem = m_pITableFrame->GetServerUserItem(wChairID);

	//最大下注
	__int64 lMaxTieScore = m_CurrentBanker.lUserScore-lAllAreaScore;

	//已下注额
	__int64 lNowJetton = 0;
	lNowJetton += m_lUserShunMenScore[wChairID];
	lNowJetton += m_lUserZuoJiaoScore[wChairID];
	lNowJetton += m_lUserDaoMenScore[wChairID];
	lNowJetton += m_lUserTianMenScore[wChairID];
	lNowJetton += m_lUserQiaoScore[wChairID];
	lNowJetton += m_lUserYouJiaoScore[wChairID];

	//我的下注
	__int64 lMeLessScore = pServerUserItem->GetUserScore()->lScore - lNowJetton;
	__int64 lMaxJetton = min(lMaxTieScore, lMeLessScore);

	return lMaxJetton;
}

//计算得分
void CTableFrameSink::CalculateScore()
{
	//结束消息
	CMD_S_GameScore GameScore;
	ZeroMemory(&GameScore,sizeof(GameScore));

	//计算牌点
	BYTE cbPlayerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_PLAYER1],m_cbCardCount[INDEX_PLAYER1]);
	BYTE cbBankerCount=m_GameLogic.GetCardListPip(m_cbTableCardArray[INDEX_BANKER],m_cbCardCount[INDEX_BANKER]);

	//推断玩家
	DeduceWinner(GameScore.cbWinner, GameScore.cbKingWinner);

	//游戏记录
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.cbBankerCount = cbBankerCount;
	GameRecord.cbPlayerCount = cbPlayerCount;
	GameRecord.lBankerScore = m_lDaoMenScore;
	GameRecord.lPlayerScore = m_lShunMenScore;
	GameRecord.lTieScore = m_lTianMenScore;
	GameRecord.wWinner = GameScore.cbWinner;

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;

	//庄家总量
	__int64 lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	__int64 lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//计算积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//庄家判断
		if ( m_CurrentBanker.dwUserID != 0 && m_CurrentBanker.wChairID == i ) continue;

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//顺门
		if(ID_SHUN_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserShunMenScore[i];
			m_lUserReturnScore[i] += m_lUserShunMenScore[i] ;
			lBankerWinScore -= m_lUserShunMenScore[i];
		}
		else
		{
			lUserLostScore[i] -=m_lUserShunMenScore[i] ;
			lBankerWinScore += m_lUserShunMenScore[i] ;
		}
		//天门
		if(ID_TIAN_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserTianMenScore[i];
			m_lUserReturnScore[i] += m_lUserTianMenScore[i] ;
			lBankerWinScore -= m_lUserTianMenScore[i];
		}
		else
		{
			lUserLostScore[i] -=m_lUserTianMenScore[i] ;
			lBankerWinScore += m_lUserTianMenScore[i] ;
		}
		//倒门
		if(ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserDaoMenScore[i];
			m_lUserReturnScore[i] += m_lUserDaoMenScore[i] ;
			lBankerWinScore -= m_lUserDaoMenScore[i];
		}
		else
		{
			lUserLostScore[i] -=m_lUserDaoMenScore[i] ;
			lBankerWinScore += m_lUserDaoMenScore[i] ;
		}

		//左角
		if(ID_SHUN_MEN & GameScore.cbWinner && ID_TIAN_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserZuoJiaoScore[i];
			m_lUserReturnScore[i] += m_lUserZuoJiaoScore[i] ;
			lBankerWinScore -= m_lUserZuoJiaoScore[i];
		}
		else if(ID_SHUN_MEN & GameScore.cbWinner || ID_TIAN_MEN & GameScore.cbWinner)
		{
			m_lUserReturnScore[i] += m_lUserZuoJiaoScore[i] ;
		}
		else
		{
			lUserLostScore[i] -=m_lUserZuoJiaoScore[i] ;
			lBankerWinScore += m_lUserZuoJiaoScore[i] ;
		}
		//右角
		if(ID_TIAN_MEN & GameScore.cbWinner && ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserYouJiaoScore[i];
			m_lUserReturnScore[i] += m_lUserYouJiaoScore[i] ;
			lBankerWinScore -= m_lUserYouJiaoScore[i];
		}
		else if(ID_TIAN_MEN & GameScore.cbWinner || ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserReturnScore[i] += m_lUserYouJiaoScore[i] ;
		}
		else
		{
			lUserLostScore[i] -=m_lUserYouJiaoScore[i] ;
			lBankerWinScore += m_lUserYouJiaoScore[i] ;
		}
		//桥
		if(ID_SHUN_MEN & GameScore.cbWinner && ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserQiaoScore[i];
			m_lUserReturnScore[i] += m_lUserQiaoScore[i] ;
			lBankerWinScore -= m_lUserQiaoScore[i];
		}
		else if(ID_SHUN_MEN & GameScore.cbWinner || ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserReturnScore[i] += m_lUserQiaoScore[i] ;
		}
		else
		{
			lUserLostScore[i] -=m_lUserQiaoScore[i] ;
			lBankerWinScore += m_lUserQiaoScore[i] ;
		}

		//总的分数
		m_lUserWinScore[i] += lUserLostScore[i];
	}

	//庄家成绩
	if ( m_CurrentBanker.dwUserID != 0 )
	{
		WORD wBankerChairID = m_CurrentBanker.wChairID;
		m_lUserWinScore[wBankerChairID] = lBankerWinScore;
		IServerUserItem *pBankerUserItem = m_pITableFrame->GetServerUserItem(wBankerChairID);

		//累计积分
		m_lBankerWinScore += lBankerWinScore;
	}

	//买注总量
	GameScore.lDrawTieScore=m_lTianMenScore;
	GameScore.lDrawBankerScore=m_lDaoMenScore;
	GameScore.lDrawPlayerScore=m_lShunMenScore;
	GameScore.lDrawTieSamPointScore = m_lQiaoScore;
	GameScore.lDrawBankerKingScore = m_lYouJiaoScore;
	GameScore.lDrawPlayerKingScore = m_lZuoJiaoScore;

	GameScore.lBankerScore = lBankerWinScore;

	//发送积分
	for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetServerUserItem(wUserIndex);
		if ( pIServerUserItem == NULL ) continue;
		//我的下注
		GameScore.lMeTieScore=m_lUserTianMenScore[wUserIndex];
		GameScore.lMeBankerScore=m_lUserDaoMenScore[wUserIndex];
		GameScore.lMePlayerScore=m_lUserShunMenScore[wUserIndex];
		GameScore.lMeTieKingScore = m_lUserQiaoScore[wUserIndex];
		GameScore.lMeBankerKingScore = m_lUserYouJiaoScore[wUserIndex];
		GameScore.lMePlayerKingScore = m_lUserZuoJiaoScore[wUserIndex];
		GameScore.lMeGameScore=m_lUserWinScore[wUserIndex];
		GameScore.lMeReturnScore = m_lUserReturnScore[wUserIndex];
		//发送消息
		m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_SCORE,&GameScore,sizeof(GameScore));
		m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_SCORE,&GameScore,sizeof(GameScore));
	}
	return ;
}

__int64 CTableFrameSink::PreCalculateBankerWin()
{
	//结束消息
	CMD_S_GameScore GameScore;
	ZeroMemory(&GameScore,sizeof(GameScore));

	//推断玩家
	DeduceWinner(GameScore.cbWinner, GameScore.cbKingWinner);

	//庄家总量
	__int64 lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	__int64 lUserLostScore[GAME_PLAYER];
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//计算积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//庄家判断
		if ( m_CurrentBanker.dwUserID != 0 && m_CurrentBanker.wChairID == i ) continue;

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//顺门
		if(ID_SHUN_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserShunMenScore[i];
			m_lUserReturnScore[i] += m_lUserShunMenScore[i] ;
			lBankerWinScore -= m_lUserShunMenScore[i];
		}
		else
		{
			lUserLostScore[i] -=m_lUserShunMenScore[i] ;
			lBankerWinScore += m_lUserShunMenScore[i] ;
		}
		//天门
		if(ID_TIAN_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserTianMenScore[i];
			m_lUserReturnScore[i] += m_lUserTianMenScore[i] ;
			lBankerWinScore -= m_lUserTianMenScore[i];
		}
		else
		{
			lUserLostScore[i] -=m_lUserTianMenScore[i] ;
			lBankerWinScore += m_lUserTianMenScore[i] ;
		}
		//倒门
		if(ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserDaoMenScore[i];
			m_lUserReturnScore[i] += m_lUserDaoMenScore[i] ;
			lBankerWinScore -= m_lUserDaoMenScore[i];
		}
		else
		{
			lUserLostScore[i] -=m_lUserDaoMenScore[i] ;
			lBankerWinScore += m_lUserDaoMenScore[i] ;
		}

		//左角
		if(ID_SHUN_MEN & GameScore.cbWinner && ID_TIAN_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserZuoJiaoScore[i];
			m_lUserReturnScore[i] += m_lUserZuoJiaoScore[i] ;
			lBankerWinScore -= m_lUserZuoJiaoScore[i];
		}
		else if(ID_SHUN_MEN & GameScore.cbWinner || ID_TIAN_MEN & GameScore.cbWinner)
		{
			m_lUserReturnScore[i] += m_lUserZuoJiaoScore[i] ;
		}
		else
		{
			lUserLostScore[i] -=m_lUserZuoJiaoScore[i] ;
			lBankerWinScore += m_lUserZuoJiaoScore[i] ;
		}
		//右角
		if(ID_TIAN_MEN & GameScore.cbWinner && ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserYouJiaoScore[i];
			m_lUserReturnScore[i] += m_lUserYouJiaoScore[i] ;
			lBankerWinScore -= m_lUserYouJiaoScore[i];
		}
		else if(ID_TIAN_MEN & GameScore.cbWinner || ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserReturnScore[i] += m_lUserYouJiaoScore[i] ;
		}
		else
		{
			lUserLostScore[i] -=m_lUserYouJiaoScore[i] ;
			lBankerWinScore += m_lUserYouJiaoScore[i] ;
		}
		//桥
		if(ID_SHUN_MEN & GameScore.cbWinner && ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserWinScore[i]+=m_lUserQiaoScore[i];
			m_lUserReturnScore[i] += m_lUserQiaoScore[i] ;
			lBankerWinScore -= m_lUserQiaoScore[i];
		}
		else if(ID_SHUN_MEN & GameScore.cbWinner || ID_DAO_MEN & GameScore.cbWinner)
		{
			m_lUserReturnScore[i] += m_lUserQiaoScore[i] ;
		}
		else
		{
			lUserLostScore[i] -=m_lUserQiaoScore[i] ;
			lBankerWinScore += m_lUserQiaoScore[i] ;
		}

		//总的分数
		m_lUserWinScore[i] += lUserLostScore[i];
	}

	//庄家成绩
	if ( m_CurrentBanker.dwUserID != 0 )
	{
		WORD wBankerChairID = m_CurrentBanker.wChairID;
		m_lUserWinScore[wBankerChairID] = lBankerWinScore;
		IServerUserItem *pBankerUserItem = m_pITableFrame->GetServerUserItem(wBankerChairID);
	}

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));

	return lBankerWinScore;
}

//推断赢家
void CTableFrameSink::DeduceWinner(BYTE &cbWinner, BYTE &cbKingWinner)
{
	cbWinner = 0;
	cbKingWinner = 0;

	if(m_GameLogic.CompareCard(m_cbTableCardArray[INDEX_BANKER],m_cbTableCardArray[INDEX_PLAYER1],2))
	{
		cbWinner = (cbWinner & ~ID_SHUN_MEN);
	}
	else
	{
		cbWinner|=ID_SHUN_MEN;
	}

	if(m_GameLogic.CompareCard(m_cbTableCardArray[INDEX_BANKER],m_cbTableCardArray[INDEX_PLAYER2],2))
	{
		cbWinner = (cbWinner & ~ID_TIAN_MEN);
	}
	else
	{
		cbWinner |=ID_TIAN_MEN;
	}
	if(m_GameLogic.CompareCard(m_cbTableCardArray[INDEX_BANKER],m_cbTableCardArray[INDEX_PLAYER3],2))
	{
		cbWinner = (cbWinner & ~ID_DAO_MEN);
	}
	else
	{
		cbWinner |=ID_DAO_MEN;
	}
	if(cbWinner & ID_SHUN_MEN && cbWinner & ID_TIAN_MEN && cbWinner & ID_DAO_MEN)
	{
		cbWinner |=ID_QUAN_SHU;
	}
}


void CTableFrameSink::RobotAI()
{
	TCHAR szINI[512];
	::GetModulePath(szINI, sizeof(szINI));
	SafeStrCat(szINI, "\\PaiJiu.ini", sizeof(szINI));

	UINT lWinControl = GetPrivateProfileInt("Option", "OpenControl", 0, szINI);

	BYTE chCardSort[4] = { INDEX_BANKER, INDEX_PLAYER1, INDEX_PLAYER2, INDEX_PLAYER3 };
	SortCardComp(chCardSort, 4);

	if (lWinControl == 0)
	{
		LONG lWinRate=GetPrivateProfileInt("Option", "WinRate", 5, szINI);
		__int64 lMaxLose = GetPrivateProfileInt("Option", "MaxLose", 100000000, szINI);
		__int64 lPlayerMaxMin = GetPrivateProfileInt("Option", "PlayMaxWin", 100000000, szINI);
		LIMIT_VALUE(lWinRate, 1, 100);

		//获取玩家
		if (m_CurrentBanker.dwUserID != 0)
		{
			//机器人30%的概率赢钱
			if ( m_CurrentBanker.dwUserType == 10 )
			{
				int nRndWinRate = rand()%lWinRate;
				int nRnd = rand()%100;
				if ( nRnd <= nRndWinRate || (lMaxLose > 0 && m_lBankerWinScore <= (-lMaxLose)) )
				{
					while(PreCalculateBankerWin() < 0)
					{
						SwapBankerCard(chCardSort, true);
					}
				}
			}
			else
			{
				//玩家如果做庄
				if (m_lBankerWinScore >= lPlayerMaxMin)
				{
					while(PreCalculateBankerWin() > 0)
					{
						SwapBankerCard(chCardSort, false);
					}
				}
			}
		}
		return;
	}
	//INDEX_PLAYER1:顺门
	//INDEX_PLAYER2:天门
	//INDEX_PLAYER3:倒门
	switch (lWinControl)
	{
	case 1:			//庄家通吃
		{
			BYTE chDescCardSort[4] = { INDEX_BANKER, -1, -1, -1 };
			ChangeCard(chCardSort, chDescCardSort);
		}
		break;
	case 2:			//天门赢，庄家吃其他两门
		{
			BYTE chDescCardSort[4] = { INDEX_PLAYER2, INDEX_BANKER, -1, -1 };
			ChangeCard(chCardSort, chDescCardSort);
		}
		break;
	case 3:			//倒门赢，庄家吃其他两门
		{
			BYTE chDescCardSort[4] = { INDEX_PLAYER3, INDEX_BANKER, -1, -1 };
			ChangeCard(chCardSort, chDescCardSort);
		}
		break;
	case 4:			//顺门赢，庄家吃其他两门
		{
			BYTE chDescCardSort[4] = { INDEX_PLAYER1, INDEX_BANKER, -1, -1 };
			ChangeCard(chCardSort, chDescCardSort);
		}
		break;
	case 5:			//天门、倒门赢，庄家吃其他一门
		{
			BYTE chDescCardSort[4] = { -1, -1, INDEX_BANKER, INDEX_PLAYER1 };
			ChangeCard(chCardSort, chDescCardSort);
		}
		break;
	case 6:			//天门、顺门赢，庄家吃其他一门
		{
			BYTE chDescCardSort[4] = { -1, -1, INDEX_BANKER, INDEX_PLAYER3 };
			ChangeCard(chCardSort, chDescCardSort);
		}
		break;
	case 7:			//倒门、顺门赢，庄家吃其他一门
		{
			BYTE chDescCardSort[4] = { -1, -1, INDEX_BANKER, INDEX_PLAYER2 };
			ChangeCard(chCardSort, chDescCardSort);
		}
		break;
	case 8:			//庄家通赔
		{
			BYTE chDescCardSort[4] = { -1, -1, -1, INDEX_BANKER };
			ChangeCard(chCardSort, chDescCardSort);
		}
		break;
	}
}

void CTableFrameSink::SwapBankerCard( BYTE chCardSort[], bool bWin)
{
	int i = 0;
	for (i = 0; i < 4; i++)
	{
		if (chCardSort[i] == INDEX_BANKER)
			break;
	}
	bool bCanSwap = false;
	BYTE nChangeID = 0;
	if (bWin)
	{
		if ( i > 0 )
		{
			nChangeID = chCardSort[i - 1];
			chCardSort[i - 1] = INDEX_BANKER;
			chCardSort[i] = nChangeID;
			bCanSwap = true;
		}
	}
	else
	{
		if (i < 4)
		{
			nChangeID = chCardSort[i + 1];
			chCardSort[i + 1] = INDEX_BANKER;
			chCardSort[i] = nChangeID;
			bCanSwap = true;
		}
	}
	if (bCanSwap)
	{
		BYTE bFirstCard = m_cbTableCardArray[nChangeID][0];
		BYTE bNextCard = m_cbTableCardArray[nChangeID][1];
		m_cbTableCardArray[nChangeID][0] = m_cbTableCardArray[INDEX_BANKER][0];
		m_cbTableCardArray[nChangeID][1] = m_cbTableCardArray[INDEX_BANKER][1];
		m_cbTableCardArray[INDEX_BANKER][0] = bFirstCard;
		m_cbTableCardArray[INDEX_BANKER][1] = bNextCard;
	}
}

void CTableFrameSink::SortCardComp( BYTE chCardComp[], int nCount )
{
	bool bChaoguo=false;
	//如果下注成功一半 平点位和
	if (m_lTianMenScore + m_lDaoMenScore + m_lShunMenScore >= m_CurrentBanker.lUserScore / 2)
	{
		bChaoguo=true;
	}
	for (int i = 0; i < nCount - 1; i++)
	{
		for ( int j = i + 1; j < nCount; j++)
		{
			bool bTongdian=false;
			bool bSwap = false;
			if(!m_GameLogic.CompareCard(m_cbTableCardArray[chCardComp[i]], m_cbTableCardArray[chCardComp[j]], 2))
			{
				bSwap = true;
			}
			if (bSwap)
			{
				BYTE nIndex = chCardComp[i];
				chCardComp[i] = chCardComp[j];
				chCardComp[j] = nIndex; 
			}
		}
	}
}

void CTableFrameSink::JettonChangeByUserLeft( WORD wChairID, IServerUserItem * pIServerUserItem )
{
	if ( m_CurrentBanker.dwUserID != 0 && m_CurrentBanker.wChairID == wChairID ) 
		return;

	__int64 nShunmen = m_lUserShunMenScore[wChairID];
	__int64 nTianmen = m_lUserTianMenScore[wChairID];
	__int64 nDaomen  = m_lUserDaoMenScore[wChairID];
	__int64 nQiaomen = m_lUserQiaoScore[wChairID];
	__int64 nYouJiaomen = m_lUserYouJiaoScore[wChairID];
	__int64 nZuoJiaomen = m_lUserZuoJiaoScore[wChairID];

	m_lShunMenScore -= nShunmen;
	m_lTianMenScore -= nTianmen;
	m_lDaoMenScore	-= nDaomen;
	m_lQiaoScore	-= nQiaomen;
	m_lYouJiaoScore -= nYouJiaomen;
	m_lZuoJiaoScore -= nZuoJiaomen;

	m_lUserDaoMenScore[wChairID] = 0;
	m_lUserShunMenScore[wChairID] = 0;
	m_lUserTianMenScore[wChairID] = 0;
	m_lUserQiaoScore[wChairID] = 0;
	m_lUserYouJiaoScore[wChairID] = 0;
	m_lUserZuoJiaoScore[wChairID] = 0;

	//变量定义
	CMD_S_JettonChange JettonChange;
	ZeroMemory(&JettonChange,sizeof(JettonChange));
	JettonChange.lTianMenScore=nTianmen;
	JettonChange.lDaoMenScore=nDaomen;
	JettonChange.lShunMenScore=nShunmen;
	JettonChange.lQiaomenScore=nQiaomen;
	JettonChange.lYouJiaomenScore=nYouJiaomen;
	JettonChange.lZuoJiaomenScore=nZuoJiaomen;
	JettonChange.lZhuangSocre=m_CurrentBanker.lUserScore; 
	JettonChange.lKexiaSocre=m_CurrentBanker.lUserScore-(m_lTianMenScore+m_lDaoMenScore+m_lShunMenScore);
	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_JETTON_CHANGE,&JettonChange,sizeof(JettonChange));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_JETTON_CHANGE,&JettonChange,sizeof(JettonChange));

}

bool CTableFrameSink::OnEventStartPlaceJetton()
{
	m_pITableFrame->SetGameStatus(GS_FREE + 1);
	m_dwJettonTime=(DWORD)time(NULL);

	//变量定义
	CMD_S_JettonStart JettonStart;
	ZeroMemory(&JettonStart,sizeof(JettonStart));
	//构造数据
	JettonStart.cbTimeLeave=TIME_PLACE_JETTON;	
	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_StartJetton,&JettonStart,sizeof(JettonStart));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_StartJetton,&JettonStart,sizeof(JettonStart));
	return true;
}


//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID==INVALID_CHAIR)
	{
		//游戏玩家
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetServerUserItem(i);
			if (pIServerUserItem==NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_INFO);
		}

		//旁观玩家
		WORD wIndex=0;
		do {
			IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem==NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_INFO);

		}while(true);
	}
	else
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetServerUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_INFO|SMT_EJECT);
	}
}

void CTableFrameSink::ChangeCard( BYTE chCardComp[], BYTE chDescCardComp[] )
{
	for (int i = 0; i < 4; i++)
	{
		BYTE nCardIndex = chDescCardComp[i];
		if (nCardIndex != 255)
		{
			int j = 0;
			for (j = 0; j < 4; j++)
			{
				if (chCardComp[j] == nCardIndex)
					break;
			}
			//i跟j的位置调换
			if (i != j)
			{
				BYTE bFirstCard = m_cbTableCardArray[chCardComp[i]][0];
				BYTE bNextCard = m_cbTableCardArray[chCardComp[i]][1];
				m_cbTableCardArray[chCardComp[i]][0] = m_cbTableCardArray[nCardIndex][0];
				m_cbTableCardArray[chCardComp[i]][1] = m_cbTableCardArray[nCardIndex][1];
				m_cbTableCardArray[nCardIndex][0] = bFirstCard;
				m_cbTableCardArray[nCardIndex][1] = bNextCard;

				chCardComp[j] = chCardComp[i];
				chCardComp[i] = nCardIndex;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

