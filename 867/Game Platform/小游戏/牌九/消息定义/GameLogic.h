#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//扑克类型    CARDTYPE
#define CT_SINGLE					0	//点数
#define CT_SPECIAL					1	//特殊牌型
#define CT_DOUBLE					2   //对子

typedef struct 
{
	int nLevel;
	int nCardType;
}stCardType;

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//游戏逻辑
class CGameLogic
{
	//变量定义
private:
	static const BYTE	m_cbCardListData[32];		//扑克定义

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//逻辑函数
public:
	//获取牌点
	BYTE GetCardPip(BYTE cbCardData);
	//获取牌点
	BYTE GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount);


	//--
	//判断类型
	BYTE GetCardType(BYTE bCardList[], BYTE bCardCount);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE bCardData)
	{
		//扑克属性
		BYTE bCardColor=GetCardColor(bCardData);
		BYTE bCardValue=GetCardValue(bCardData);

		//转换数值
		if(bCardValue==1)return 6;

		return bCardValue;
	}
	//排列扑克
	void SortCard(BYTE bCardList[], BYTE bCardCount);
	bool CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bCardCount);
	BYTE GetMaxColor(BYTE bCardList[]);
	int GetMaxSingleValue(BYTE bCardList[]);
	CString GetCardNick(BYTE bCardList[]);
	stCardType GetCardLevel(BYTE bCardList[]);
	int GetSingleTypeLevel(BYTE bSingleCard);
};

#endif