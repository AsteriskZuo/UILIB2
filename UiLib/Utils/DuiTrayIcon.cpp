#include "StdAfx.h"
#include "DuiTrayIcon.h"

namespace UiLib
{

	CDuiTrayIcon::CDuiTrayIcon(void)
	{
		memset(&m_trayData, 0, sizeof(m_trayData));
		m_bEnabled		= false;
		m_bVisible		= false;
		m_bTwinkling	= false;
		m_hWnd			= NULL;
		pIDuiTimer		= NULL;
		m_uMessage		= UIEVENT_TRAYICON;
	}


	CDuiTrayIcon::~CDuiTrayIcon(void)
	{
		DeleteTrayIcon();
	}

	//************************************
	// ��������: CreateTrayIcon
	// ��������: void
	// ������Ϣ: HWND _RecvHwnd
	// ������Ϣ: LPCTSTR _ToolTipText
	// ������Ϣ: UINT _Message
	// ������Ϣ: HICON _DefaultIcon
	// ����˵��: 
	//************************************
	void CDuiTrayIcon::CreateTrayIcon( HWND _RecvHwnd,UINT _IconIDResource,LPCTSTR _ToolTipText /*= NULL*/,UINT _Message /*= UIEVENT_TRAYICON*/)
	{
		if(_Message == NULL)
			_Message = UIEVENT_TRAYICON;

		if(!_RecvHwnd || _IconIDResource <= 0 || _Message < 0)
			return;

		m_hIcon = LoadIcon(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(_IconIDResource));

		m_trayData.cbSize = sizeof(NOTIFYICONDATA);
		m_trayData.hWnd	 = _RecvHwnd;
		m_trayData.uID	 = _IconIDResource;
		m_trayData.hIcon = m_hIcon;
		m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_trayData.uCallbackMessage = _Message;
		if(_ToolTipText)
			wcscpy(m_trayData.szTip,_ToolTipText);

		Shell_NotifyIcon(NIM_ADD,&m_trayData);
		m_bEnabled = true;
	}
	
	//************************************
	// ��������: DeleteTrayIcon
	// ��������: void
	// ����˵��: 
	//************************************
	void CDuiTrayIcon::DeleteTrayIcon()
	{
		if(pIDuiTimer)
			StopTwinkling();

		Shell_NotifyIcon(NIM_DELETE,&m_trayData);
		m_bEnabled		= false;
		m_bVisible		= false;
		m_bTwinkling	= false;
		m_hWnd			= NULL;
		m_uMessage		= UIEVENT_TRAYICON;
	}

	//************************************
	// ��������: SetTooltipText
	// ��������: bool
	// ������Ϣ: LPCTSTR _ToolTipText
	// ����˵��: 
	//************************************
	bool CDuiTrayIcon::SetTooltipText( LPCTSTR _ToolTipText )
	{
		if(_ToolTipText)
			wcscpy(m_trayData.szTip,_ToolTipText);

		if (!m_bEnabled) return FALSE;
		m_trayData.uFlags = NIF_TIP;
		return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;
	}

	//************************************
	// ��������: SetTooltipText
	// ��������: bool
	// ������Ϣ: UINT _IDResource
	// ����˵��: 
	//************************************
	bool CDuiTrayIcon::SetTooltipText( UINT _IDResource )
	{
		wchar_t mbuf[64];
		LoadString(CPaintManagerUI::GetInstance(),_IDResource,mbuf,64);

		return SetTooltipText(mbuf);
	}

	//************************************
	// ��������: GetTooltipText
	// ��������: UiLib::CDuiString
	// ����˵��: 
	//************************************
	UiLib::CDuiString CDuiTrayIcon::GetTooltipText() const
	{
		return m_trayData.szTip;
	}

	//************************************
	// ��������: SetIcon
	// ��������: bool
	// ������Ϣ: HICON _Hicon
	// ����˵��: 
	//************************************
	bool CDuiTrayIcon::SetIcon( HICON _Hicon )
	{
		if(_Hicon)
			m_hIcon = _Hicon;

		m_trayData.uFlags = NIF_ICON;
		m_trayData.hIcon = _Hicon;
		
		if (!m_bEnabled) return FALSE;
		return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;

		return false;
	}

	//************************************
	// ��������: SetIcon
	// ��������: bool
	// ������Ϣ: LPCTSTR _IconFile
	// ����˵��: 
	//************************************
	bool CDuiTrayIcon::SetIcon( LPCTSTR _IconFile )
	{
		HICON hIcon = LoadIcon(CPaintManagerUI::GetInstance(),_IconFile);
		return SetIcon(hIcon);
	}

	//************************************
	// ��������: SetIcon
	// ��������: bool
	// ������Ϣ: UINT _IDResource
	// ����˵��: 
	//************************************
	bool CDuiTrayIcon::SetIcon( UINT _IDResource )
	{
		HICON hIcon = LoadIcon(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(_IDResource));
		return SetIcon(hIcon);
	}

	//************************************
	// ��������: GetIcon
	// ��������: HICON
	// ����˵��: 
	//************************************
	HICON CDuiTrayIcon::GetIcon() const
	{
		HICON hIcon = NULL;
		hIcon = m_trayData.hIcon;
		return hIcon;
	}

	//************************************
	// ��������: SetHideIcon
	// ��������: void
	// ����˵��: 
	//************************************
	void CDuiTrayIcon::SetHideIcon()
	{
		if (IsVisible()) {
			SetIcon((HICON)NULL);
			m_bVisible = TRUE;
		}
	}

	//************************************
	// ��������: SetShowIcon
	// ��������: void
	// ����˵��: 
	//************************************
	void CDuiTrayIcon::SetShowIcon()
	{
		if (!IsVisible()) {
			SetIcon(m_hIcon);
			m_bVisible = FALSE;
		}
	}

	//************************************
	// ��������: RemoveIcon
	// ��������: void
	// ����˵��: 
	//************************************
	void CDuiTrayIcon::RemoveIcon()
	{
		m_trayData.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, &m_trayData);
		m_bEnabled = FALSE;
	}

	//************************************
	// ��������: StartTwinkling
	// ��������: bool
	// ����˵��: 
	//************************************
	bool CDuiTrayIcon::StartTwinkling()
	{
		if(m_bTwinkling || !m_bEnabled || pIDuiTimer)
			return false;

		pIDuiTimer = MakeDuiTimer(this,&CDuiTrayIcon::OnTimer,400);
		pIDuiTimer->SetDuiTimer();
		m_bTwinkling = true;
		return true;
	}

	//************************************
	// ��������: StopTwinkling
	// ��������: void
	// ����˵��: 
	//************************************
	void CDuiTrayIcon::StopTwinkling()
	{
		if(pIDuiTimer){
			pIDuiTimer->KillDuiTimer();
			delete pIDuiTimer;
			pIDuiTimer = NULL;
		}
		m_bTwinkling = false;
		SetShowIcon();
	}

	//************************************
	// ��������: OnTimer
	// ��������: void
	// ������Ϣ: IDuiTimer * pTimer
	// ����˵��: 
	//************************************
	void CDuiTrayIcon::OnTimer( IDuiTimer* pTimer )
	{
		IsVisible()?SetHideIcon():SetShowIcon();
	}
}
