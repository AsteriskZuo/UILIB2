#include "stdafx.h"
#include "UIButton.h"

namespace UiLib
{
	REGIST_DUICLASS(CButtonUI);

	CButtonUI::CButtonUI() : m_uButtonState(0), m_dwHotTextColor(0), m_dwPushedTextColor(0), m_dwFocusedTextColor(0),m_dwHotBkColor(0)

	{
		m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	}

	LPCTSTR CButtonUI::GetClass() const
	{
		return _T("ButtonUI");
	}

	LPVOID CButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("Button")) == 0 ) return static_cast<CButtonUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CButtonUI::GetControlFlags() const
	{
		return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	void CButtonUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KEYDOWN )
		{
			if (IsKeyboardEnabled()) {
				if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
					Activate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() ) {
				m_pManager->SendNotify(this, _T("menu"), event.wParam, event.lParam);
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_SETCURSOR ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CLabelUI::DoEvent(event);
	}

	bool CButtonUI::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		if( m_pManager != NULL ){
			m_pManager->SendNotify(this, _T("click"));
			TriggerEffects(GetClickInEffect());
			if(!m_sCalendarName.IsEmpty()){
				CCalendarUI::ShowCalendarDlg(this);
			}
			BindTriggerTabSel();
		}
		return true;
	}

	void CButtonUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	//************************************
	// Method:    SetHotBkColor
	// FullName:  CButtonUI::SetHotBkColor
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: DWORD dwColor
	// Note:	  
	//************************************
	void CButtonUI::SetHotBkColor( DWORD dwColor )
	{
		try
		{
			m_dwHotBkColor = dwColor;
		}
		catch (...)
		{
			throw "CButtonUI::SetHotBkColor";
		}
	}

	//************************************
	// Method:    GetHotBkColor
	// FullName:  CButtonUI::GetHotBkColor
	// Access:    public 
	// Returns:   DWORD
	// Qualifier: const
	// Note:	  
	//************************************
	DWORD CButtonUI::GetHotBkColor() const
	{
		try
		{
			return m_dwHotBkColor;
		}
		catch (...)
		{
			throw "CButtonUI::GetHotBkColor";
		}
	}

	void CButtonUI::SetHotTextColor(DWORD dwColor)
	{
		m_dwHotTextColor = dwColor;
	}

	DWORD CButtonUI::GetHotTextColor() const
	{
		return m_dwHotTextColor;
	}

	void CButtonUI::SetPushedTextColor(DWORD dwColor)
	{
		m_dwPushedTextColor = dwColor;
	}

	DWORD CButtonUI::GetPushedTextColor() const
	{
		return m_dwPushedTextColor;
	}

	void CButtonUI::SetFocusedTextColor(DWORD dwColor)
	{
		m_dwFocusedTextColor = dwColor;
	}

	DWORD CButtonUI::GetFocusedTextColor() const
	{
		return m_dwFocusedTextColor;
	}

	LPCTSTR CButtonUI::GetNormalImage()
	{
		return m_sNormalImage;
	}

	void CButtonUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetHotImage()
	{
		return m_sHotImage;
	}

	void CButtonUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetPushedImage()
	{
		return m_sPushedImage;
	}

	void CButtonUI::SetPushedImage(LPCTSTR pStrImage)
	{
		m_sPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetFocusedImage()
	{
		return m_sFocusedImage;
	}

	void CButtonUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonUI::GetDisabledImage()
	{
		return m_sDisabledImage;
	}

	void CButtonUI::SetDisabledImage(LPCTSTR pStrImage)
	{
		m_sDisabledImage = pStrImage;
		Invalidate();
	}

	SIZE CButtonUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotforeimage")) == 0 ) SetHotForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("calendardestname")) == 0 ) SetCalendarValDest(pstrValue);
		else if( _tcscmp(pstrName, _T("calendarname")) == 0 ) SetCalendarName(pstrValue);
		else if( _tcscmp(pstrName, _T("calendarstyle")) == 0 ) SetCalendarStyle(pstrValue);
		else if( _tcscmp(pstrName, _T("calendarprofile")) == 0 ) SetCalendarProfile(pstrValue);
		else if( _tcscmp(pstrName, _T("bindtabindex")) == 0 ) BindTabIndex(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("bindtablayoutname")) == 0 ) BindTabLayoutName(pstrValue);
		else if( _tcscmp(pstrName, _T("hotbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("hottextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("pushedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetPushedTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("focusedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetFocusedTextColor(clrColor);
		}
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CButtonUI::PaintText(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		DWORD clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;

		if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0) )
			clrColor = GetPushedTextColor();
		else if( ((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0) )
			clrColor = GetHotTextColor();
		else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0) )
			clrColor = GetFocusedTextColor();

		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, clrColor, \
			NULL, NULL, nLinks, m_uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, clrColor, \
			m_iFont, m_uTextStyle);
	}

	void CButtonUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
				else goto Label_ForeImage;
			}
		}
		else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if( !m_sPushedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sPushedImage) ){
					m_sPushedImage.Empty();
				}
				if( !m_sPushedForeImage.IsEmpty() ) {
					if( !DrawImage(hDC, (LPCTSTR)m_sPushedForeImage) )
						m_sPushedForeImage.Empty();
					return;
				}
				else goto Label_ForeImage;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ){
					m_sHotImage.Empty();
				}
				if( !m_sHotForeImage.IsEmpty() ) {
					if( !DrawImage(hDC, (LPCTSTR)m_sHotForeImage) )
						m_sHotForeImage.Empty();
					return;
				}
				else goto Label_ForeImage;
			}
			else if(m_dwHotBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
				return;
			}
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else goto Label_ForeImage;
			}
		}
		if( !m_sNormalImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
			else goto Label_ForeImage;
		}
		if(!m_sForeImage.IsEmpty() )
			goto Label_ForeImage;
		return;

Label_ForeImage:
		if(!m_sForeImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sForeImage) ) m_sForeImage.Empty();
		}
	}

	//************************************
	// Method:    GetForeImage
	// FullName:  CButtonUI::GetForeImage
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Note:	  
	//************************************
	LPCTSTR CButtonUI::GetForeImage()
	{
		try
		{
			return m_sForeImage;
		}
		catch(...)
		{
			throw "CButtonUI::GetForeImage";
		}
	}

	//************************************
	// Method:    SetForeImage
	// FullName:  CButtonUI::SetForeImage
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pStrImage
	// Note:	  
	//************************************
	void CButtonUI::SetForeImage( LPCTSTR pStrImage )
	{
		try
		{
			m_sForeImage = pStrImage;
			Invalidate();
		}
		catch(...)
		{
			throw "CButtonUI::SetForeImage";
		}
	}

	//************************************
	// Method:    GetHotForeImage
	// FullName:  CButtonUI::GetHotForeImage
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Note:	  
	//************************************
	LPCTSTR CButtonUI::GetHotForeImage()
	{
		try
		{
			return m_sHotForeImage;
		}
		catch(...)
		{
			throw "CButtonUI::GetHotForeImage";
		}
	}

	//************************************
	// Method:    SetHotForeImage
	// FullName:  CButtonUI::SetHotForeImage
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pStrImage
	// Note:	  
	//************************************
	void CButtonUI::SetHotForeImage( LPCTSTR pStrImage )
	{
		try
		{
			m_sHotForeImage = pStrImage;
			Invalidate();
		}
		catch(...)
		{
			throw "CButtonUI::SetHotForeImage";
		}
	}
	
	//************************************
	// Method:    SetCalendarName
	// FullName:  CButtonUI::SetCalendarName
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pStrCalendarName
	// Note:	  
	//************************************
	void CButtonUI::SetCalendarName( LPCTSTR pStrCalendarName )
	{
		try
		{
			m_sCalendarName = pStrCalendarName;
		}
		catch (...)
		{
			throw "CButtonUI::SetCalendarName";
		}
	}

	//************************************
	// Method:    GetCalendarName
	// FullName:  CButtonUI::GetCalendarName
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Note:	  
	//************************************
	LPCTSTR CButtonUI::GetCalendarName()
	{
		try
		{
			return m_sCalendarName;
		}
		catch (...)
		{
			throw "CButtonUI::GetCalendarName";
		}
	}

	//************************************
	// Method:    SetCalendarStyle
	// FullName:  CButtonUI::SetCalendarStyle
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pStrCalendarStyle
	// Note:	  
	//************************************
	void CButtonUI::SetCalendarStyle( LPCTSTR pStrCalendarStyle )
	{
		try
		{
			m_sCalendarStyle = pStrCalendarStyle;
		}
		catch (...)
		{
			throw "CButtonUI::SetCalendarStyle";
		}
	}

	//************************************
	// Method:    GetCalendarStyle
	// FullName:  CButtonUI::GetCalendarStyle
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Note:	  
	//************************************
	LPCTSTR CButtonUI::GetCalendarStyle()
	{
		try
		{
			return m_sCalendarStyle;
		}
		catch (...)
		{
			throw "CButtonUI::GetCalendarStyle";
		}
	}

	//************************************
	// Method:    SetCalendarProfile
	// FullName:  CButtonUI::SetCalendarProfile
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pStrCalendarProfile
	// Note:	  
	//************************************
	void CButtonUI::SetCalendarProfile( LPCTSTR pStrCalendarProfile )
	{
		try
		{
			m_sCalendarProfile = pStrCalendarProfile;
		}
		catch (...)
		{
			throw "CButtonUI::SetCalendarProfile";
		}
	}

	//************************************
	// Method:    GetCalendarProfile
	// FullName:  CButtonUI::GetCalendarProfile
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Note:	  
	//************************************
	LPCTSTR CButtonUI::GetCalendarProfile()
	{
		try
		{
			return m_sCalendarProfile;
		}
		catch (...)
		{
			throw "CButtonUI::GetCalendarProfile";
		}
	}

	//************************************
	// Method:    BindTabIndex
	// FullName:  CButtonUI::BindTabIndex
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _BindTabIndex
	// Note:	  
	//************************************
	void CButtonUI::BindTabIndex(int _BindTabIndex )
	{
		try
		{
			if( _BindTabIndex >= 0)
				m_iBindTabIndex			= _BindTabIndex;
		}
		catch (...)
		{
			throw "CButtonUI::BindTabIndex";
		}
	}
	
	//************************************
	// Method:    BindTabLayoutName
	// FullName:  CButtonUI::BindTabLayoutName
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR _TabLayoutName
	// Note:	  
	//************************************
	void CButtonUI::BindTabLayoutName( LPCTSTR _TabLayoutName )
	{
		try
		{
			if(_TabLayoutName)
				m_sBindTabLayoutName = _TabLayoutName;
		}
		catch (...)
		{
			throw "CButtonUI::BindTabLayoutName";
		}
	}

	//************************************
	// Method:    BindTriggerTabSel
	// FullName:  CButtonUI::BindTriggerTabSel
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _SetSelectIndex
	// Note:	  
	//************************************
	void CButtonUI::BindTriggerTabSel( int _SetSelectIndex /*= -1*/ )
	{
		try
		{
			if(GetBindTabLayoutName().IsEmpty() || (GetBindTabLayoutIndex() < 0 && _SetSelectIndex < 0))
				return;

			CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(GetManager()->FindControl(GetBindTabLayoutName()));
			if(!pTabLayout)
				return;

			pTabLayout->SelectItem(_SetSelectIndex >=0?_SetSelectIndex:GetBindTabLayoutIndex());
		}
		catch (...)
		{
			throw "CButtonUI::BindTriggerTabSel";
		}
	}

	//************************************
	// Method:    RemoveBindTabIndex
	// FullName:  CButtonUI::RemoveBindTabIndex
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Note:	  
	//************************************
	void CButtonUI::RemoveBindTabIndex()
	{
		try
		{
			m_iBindTabIndex	= -1;
			m_sBindTabLayoutName.Empty();
		}
		catch (...)
		{
			throw "CButtonUI::RemoveBindTabIndex";
		}
	}
	
	//************************************
	// Method:    GetBindTabLayoutIndex
	// FullName:  CButtonUI::GetBindTabLayoutIndex
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Note:	  
	//************************************
	int CButtonUI::GetBindTabLayoutIndex()
	{
		try
		{
			return m_iBindTabIndex;
		}
		catch (...)
		{
			throw "CButtonUI::GetBindTabLayoutIndex";
		}
	}

	//************************************
	// Method:    GetBindTabLayoutName
	// FullName:  CButtonUI::GetBindTabLayoutName
	// Access:    public 
	// Returns:   UiLib::CDuiString
	// Qualifier:
	// Note:	  
	//************************************
	UiLib::CDuiString CButtonUI::GetBindTabLayoutName()
	{
		try
		{
			return m_sBindTabLayoutName;
		}
		catch (...)
		{
			throw "CButtonUI::GetBindTabLayoutName";
		}
	}

	//************************************
	// 函数名称: OnPropertyActionTimer
	// 返回类型: void
	// 参数信息: IDuiTimer * pTimer
	// 参数信息: TProperty * pTProperty
	// 函数说明: 
	//************************************
	void CButtonUI::OnPropertyActionTimer( IDuiTimer* pTimer,TProperty* pTProperty )
	{
		if(!pTProperty)
			return;

		int nDiffTime	= pTimer->GetTotalTimer() - pTimer->GetCurTimer();
		int nTotalFrame	= (int)(pTimer->GetTotalTimer()/pTimer->GetInterval());
		int nCurFrame	= (int)((pTimer->GetTotalTimer() - nDiffTime) / pTimer->GetInterval());
		bool bStartNone	= pTProperty->IsStartNull();
		bool bEndNone	= pTProperty->IsEndNull();
		CDuiString nPropertyName = pTProperty->sName;

		DUITRACE(_T("===========%s============"),nPropertyName.GetData());
		DUITRACE(_T("nDiffTime:%d ,%d - %d"),nDiffTime,pTimer->GetTotalTimer(),pTimer->GetCurTimer());
		DUITRACE(_T("nTotalFrame:%d ,%d / %d"),nTotalFrame,pTimer->GetTotalTimer(),pTimer->GetInterval());
		DUITRACE(_T("nCurFrame:%d ,%d / %d"),nCurFrame,(pTimer->GetTotalTimer() - nDiffTime),pTimer->GetInterval());

		if(_tcscmp(pTProperty->sType.GetData(),_T("image.source")) == 0){
			if( nPropertyName == _T("normalimage") )
				return SetNormalImage(pTProperty->CalCurImageSource(GetNormalImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("hotimage") )
				return SetHotImage(pTProperty->CalCurImageSource(GetHotImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("pushedimage") )
				return SetPushedImage(pTProperty->CalCurImageSource(GetPushedImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("focusedimage") )
				return SetForeImage(pTProperty->CalCurImageSource(GetForeImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("disabledimage") )
				return SetDisabledImage(pTProperty->CalCurImageSource(GetDisabledImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else {
				goto GoPropertyActionTimer;
			}
		}
		else if(_tcscmp(pTProperty->sType.GetData(),_T("image.mask")) == 0){
			if( nPropertyName == _T("normalimage") )
				return SetNormalImage(pTProperty->CalCurImageMask(GetNormalImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("hotimage") )
				return SetHotImage(pTProperty->CalCurImageMask(GetHotImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("pushedimage") )
				return SetPushedImage(pTProperty->CalCurImageMask(GetPushedImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("focusedimage") )
				return SetForeImage(pTProperty->CalCurImageMask(GetForeImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("disabledimage") )
				return SetDisabledImage(pTProperty->CalCurImageMask(GetDisabledImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else
				goto GoPropertyActionTimer;
		}
		else if(_tcscmp(pTProperty->sType.GetData(),_T("image.corner")) == 0){
			if( nPropertyName == _T("normalimage") )
				return SetNormalImage(pTProperty->CalCurImageCorner(GetNormalImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("hotimage") )
				return SetHotImage(pTProperty->CalCurImageCorner(GetHotImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("pushedimage") )
				return SetPushedImage(pTProperty->CalCurImageCorner(GetPushedImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("focusedimage") )
				return SetForeImage(pTProperty->CalCurImageCorner(GetForeImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("disabledimage") )
				return SetDisabledImage(pTProperty->CalCurImageCorner(GetDisabledImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else
				goto GoPropertyActionTimer;
		}
		else if(_tcscmp(pTProperty->sType.GetData(),_T("image.fade")) == 0){
			if( nPropertyName == _T("normalimage") )
				return SetNormalImage(pTProperty->CalCurImageFade(GetNormalImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("hotimage") )
				return SetHotImage(pTProperty->CalCurImageFade(GetHotImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("pushedimage") )
				return SetPushedImage(pTProperty->CalCurImageFade(GetPushedImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("focusedimage") )
				return SetForeImage(pTProperty->CalCurImageFade(GetForeImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("disabledimage") )
				return SetDisabledImage(pTProperty->CalCurImageFade(GetDisabledImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else
				goto GoPropertyActionTimer;
		}
		else if(_tcscmp(pTProperty->sType.GetData(),_T("image.dest")) == 0){
			if( nPropertyName == _T("normalimage") )
				return SetNormalImage(pTProperty->CalCurImageDest(GetNormalImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("hotimage") )
				return SetHotImage(pTProperty->CalCurImageDest(GetHotImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("pushedimage") )
				return SetPushedImage(pTProperty->CalCurImageDest(GetPushedImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("focusedimage") )
				return SetForeImage(pTProperty->CalCurImageDest(GetForeImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else if( nPropertyName == _T("disabledimage") )
				return SetDisabledImage(pTProperty->CalCurImageDest(GetDisabledImage(),nTotalFrame,nCurFrame,bStartNone,bEndNone));
			else
				goto GoPropertyActionTimer;
		}
		else goto GoPropertyActionTimer;

GoPropertyActionTimer:
		{
			CControlUI::OnPropertyActionTimer(pTimer,pTProperty);
		}
	}

	//************************************
	// 函数名称: GetAttribute
	// 返回类型: UiLib::unUserData
	// 参数信息: LPCTSTR pstrName
	// 函数说明: 
	//************************************
	UiLib::unUserData CButtonUI::GetAttribute( LPCTSTR pstrName )
	{
		unUserData nRetData = {{0,0,0,0}};

		if(!pstrName)
			return nRetData;

		if( _tcscmp(pstrName, _T("normalimage")) == 0 )
			nRetData.pcwchar = GetNormalImage();
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 )
			nRetData.pcwchar = GetHotImage();
		else if( _tcscmp(pstrName, _T("pushedimage")) == 0 )
			nRetData.pcwchar = GetPushedImage();
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 )
			nRetData.pcwchar = GetPushedImage();
		else if( _tcscmp(pstrName, _T("disabledimage")) == 0 )
			nRetData.pcwchar = GetDisabledImage();
		else
			return CControlUI::GetAttribute(pstrName);

		return nRetData;
	}

	//************************************
	// 函数名称: SetCalendarValDest
	// 返回类型: void
	// 参数信息: LPCTSTR pstrValue
	// 函数说明: 
	//************************************
	void CButtonUI::SetCalendarValDest( LPCTSTR pstrValue )
	{
		m_sSalendarValDest = pstrValue;
	}

	//************************************
	// 函数名称: GetCalendarValDest
	// 返回类型: LPCTSTR
	// 函数说明: 
	//************************************
	LPCTSTR CButtonUI::GetCalendarValDest()
	{
		return m_sSalendarValDest.GetData();
	}

}