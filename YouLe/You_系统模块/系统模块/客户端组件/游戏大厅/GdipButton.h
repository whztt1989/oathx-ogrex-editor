//
// GdipButton.h : Version 1.0 - see article at CodeProject.com
//
// Author:  Darren Sessions
//          
//
// Description:
//     GdipButton is a CButton derived control that uses GDI+ 
//     to support alternate image formats
//
// History
//     Version 1.0 - 2008 June 10
//     - Initial public release
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// GdipButton.h : header file
//

class CGdiPlusBitmapResource;
/////////////////////////////////////////////////////////////////////////////
// CGdipButton window

class CGdipButton : public CButton
{
public:

	CGdipButton();
	virtual ~CGdipButton();

	// image types
	enum	{
				STD_TYPE	= 0,
				ALT_TYPE,
				DIS_TYPE
			};


	// create button
	BOOL CreateButton(CWnd* pParentWnd,LPCTSTR ResName, LPCTSTR pImageName,
		int dx,int dy,UINT WondowsID,UINT lStatusNum = 1,HINSTANCE hInst=NULL);

	// create button
	BOOL CreateButton(CWnd* pParentWnd,UINT ResourceID, LPCTSTR pImageName,
		int dx,int dy,UINT WondowsID,UINT lStatusNum = 1,HINSTANCE hInst=NULL);

	// sets the image type
	void SetImage(int type);

	BOOL LoadAltImage(LPCTSTR ResName, LPCTSTR pType,UINT lStatusNum=1,HINSTANCE hInst=NULL);
	BOOL LoadStdImage(LPCTSTR ResName, LPCTSTR pType,UINT lStatusNum=1,HINSTANCE hInst=NULL);

	BOOL LoadAltImage(UINT id, LPCTSTR pType,UINT lStatusNum=1,HINSTANCE hInst=NULL);
	BOOL LoadStdImage(UINT id, LPCTSTR pType,UINT lStatusNum=1,HINSTANCE hInst=NULL);
	// if false, disables the press state and uses grayscale image if it exists
	void EnableButton(BOOL bEnable = TRUE) { m_bIsDisabled = !bEnable; }

	// in toggle mode each press toggles between std and alt images
	void EnableToggle(BOOL bEnable = TRUE);

	// return the enable/disable state
	BOOL IsDisabled(void) {return (m_bIsDisabled == TRUE); }

	void SetBkGnd(CDC* pDC);

	void SetToolTipText(CString spText, BOOL bActivate = TRUE);
	void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
	void SetHorizontal(bool ImagesAreLaidOutHorizontally = FALSE);
	void DeleteToolTip();
	//重新设置AltImage
	void ResetAltImage(LPCTSTR ResName, LPCTSTR pImageName);

protected:

	void PaintBk(CDC* pDC);
	void PaintBtn(CDC* pDC);

	BOOL	m_bHaveAltImage;
	BOOL	m_bHaveBitmaps;

	BOOL	m_bIsDisabled;
	BOOL	m_bIsToggle;
	BOOL	m_bIsHovering;	//悬挂状态
	BOOL	m_bIsTracking;


	int		m_nCurType;			//当前模式，可用，改变，禁用
	int		m_nStatusNum;		//状态数量

	CGdiPlusBitmapResource* m_pAltImage;
	CGdiPlusBitmapResource* m_pStdImage;

	CString			m_tooltext;
	CToolTipCtrl*	m_pToolTip;
	
	void	InitToolTip();

	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//{{AFX_MSG(CGdipButton)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	CDC		m_dcBk;			// 按钮背景
	
	CDC		m_dcStd;		// 正常状态
	CDC		m_dcStdP;		// 按下状态
	CDC		m_dcStdH;		// 悬挂状态

	CDC		m_dcAlt;		// 正常状态
	CDC		m_dcAltP;		// 按下状态
	CDC		m_dcAltH;		// 悬挂状态

	CDC		m_dcGS;			// 不可用状态

	CDC*	m_pCurBtn;		// 当前使用

};
