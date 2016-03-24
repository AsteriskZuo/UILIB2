#include "StdAfx.h"

namespace UiLib
{

	CDialogBuilder::CDialogBuilder() : m_pCallback(NULL), m_pstrtype(NULL)
	{

	}

	CControlUI* CDialogBuilder::Create(STRINGorID xml, LPCTSTR type, IDialogBuilderCallback* pCallback, 
		CPaintManagerUI* pManager, CControlUI* pParent)
	{
		//��ԴIDΪ0-65535�������ֽڣ��ַ���ָ��Ϊ4���ֽ�
		//�ַ�����<��ͷ��Ϊ��XML�ַ�����������Ϊ��XML�ļ�

		if( HIWORD(xml.m_lpstr) != NULL ) {
			if( *(xml.m_lpstr) == _T('<') ) {
				if( !m_xml.Load(xml.m_lpstr) ) return NULL;
			}
			else {
				if( !m_xml.LoadFromFile(xml.m_lpstr) ) return NULL;
			}
		}
		else {
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);
			if( hResource == NULL ) return NULL;
			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
			if( hGlobal == NULL ) {
				FreeResource(hResource);
				return NULL;
			}

			m_pCallback = pCallback;
			if( !m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource) )) return NULL;
			::FreeResource(hResource);
			m_pstrtype = type;
		}

		return Create(pCallback, pManager, pParent);
	}

	CControlUI* CDialogBuilder::Create(IDialogBuilderCallback* pCallback, CPaintManagerUI* pManager, CControlUI* pParent)
	{
		m_pCallback = pCallback;
		CMarkupNode root = m_xml.GetRoot();
		if( !root.IsValid() ) return NULL;

		if( pManager ) {
			LPCTSTR pstrClass = NULL;
			int nAttributes = 0;
			LPCTSTR pstrName = NULL;
			LPCTSTR pstrValue = NULL;
			LPTSTR pstr = NULL;
			for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
				pstrClass = node.GetName();
				if( _tcscmp(pstrClass, _T("Image")) == 0 ) {
					nAttributes = node.GetAttributeCount();
					LPCTSTR pImageName = NULL;
					LPCTSTR pImageResType = NULL;
					DWORD mask = 0;
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);
						if( _tcscmp(pstrName, _T("name")) == 0 ) {
							pImageName = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("restype")) == 0 ) {
							pImageResType = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("mask")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							mask = _tcstoul(pstrValue, &pstr, 16);
						}
					}
					if( pImageName ) pManager->AddImage(pImageName, pImageResType, mask);
				}
				else if( _tcscmp(pstrClass, _T("Font")) == 0 ) {
					nAttributes = node.GetAttributeCount();
					LPCTSTR pFontName = NULL;
					int size = 12;
					bool bold = false;
					bool underline = false;
					bool italic = false;
					bool defaultfont = false;
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);
						if( _tcscmp(pstrName, _T("name")) == 0 ) {
							pFontName = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("size")) == 0 ) {
							size = _tcstol(pstrValue, &pstr, 10);
						}
						else if( _tcscmp(pstrName, _T("bold")) == 0 ) {
							bold = (_tcscmp(pstrValue, _T("true")) == 0);
						}
						else if( _tcscmp(pstrName, _T("underline")) == 0 ) {
							underline = (_tcscmp(pstrValue, _T("true")) == 0);
						}
						else if( _tcscmp(pstrName, _T("italic")) == 0 ) {
							italic = (_tcscmp(pstrValue, _T("true")) == 0);
						}
						else if( _tcscmp(pstrName, _T("default")) == 0 ) {
							defaultfont = (_tcscmp(pstrValue, _T("true")) == 0);
						}
					}
					if( pFontName ) {
						pManager->AddFont(pFontName, size, bold, underline, italic);
						if( defaultfont ) pManager->SetDefaultFont(pFontName, size, bold, underline, italic);
					}
				}
				else if( _tcscmp(pstrClass, _T("Default")) == 0 ) {
					nAttributes = node.GetAttributeCount();
					LPCTSTR pControlName = NULL;
					LPCTSTR pControlValue = NULL;
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);
						if( _tcscmp(pstrName, _T("name")) == 0 ) {
							pControlName = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("value")) == 0 ) {
							pControlValue = pstrValue;
						}
					}
					if( pControlName ) {
						pManager->AddDefaultAttributeList(pControlName, pControlValue);
					}
				}
				else if( _tcscmp(pstrClass, _T("ActionScript")) == 0) {

					for(CMarkupNode AGroupNode = node.GetChild();AGroupNode.IsValid();AGroupNode = AGroupNode.GetSibling()){
						LPCTSTR pstrClass = AGroupNode.GetName();
						if( _tcscmp(pstrClass, _T("AGroup")) != 0 || !AGroupNode.HasAttribute(_T("name")) || !AGroupNode.HasAttribute(_T("msgtype")) || !AGroupNode.HasAttribute(_T("msgvalue")))
							continue;

						if(_tcscmp(_T("notify"),AGroupNode.GetAttributeValue(_T("msgtype"))) != 0 && 
							_tcscmp(_T("event"),AGroupNode.GetAttributeValue(_T("msgtype"))) != 0)
							continue;

						int  iDefaultInterval	= _ttoi(AGroupNode.GetAttributeValue(_T("defaultinterval")));
						int  iDefaultTimer		= _ttoi(AGroupNode.GetAttributeValue(_T("defaulttimer")));
						bool bDefaultAutoStart	= _tcscmp(_T("true"),AGroupNode.GetAttributeValue(_T("defaultautostart"))) == 0?true:false;
						bool bDefaultLoop		= _tcscmp(_T("true"),AGroupNode.GetAttributeValue(_T("defaultloop"))) == 0?true:false;
						bool bDefaultReverse	= _tcscmp(_T("true"),AGroupNode.GetAttributeValue(_T("defaultreverse"))) == 0?true:false;

						if(_tcscmp(_T("notify"),AGroupNode.GetAttributeValue(_T("msgtype"))) == 0){
							if(!pManager->AddActionScriptGroup(
								AGroupNode.GetAttributeValue(_T("name")),
								AGroupNode.GetAttributeValue(_T("msgvalue")),
								iDefaultInterval,iDefaultTimer,
								bDefaultReverse,bDefaultLoop,bDefaultAutoStart
								))
								continue;
						}
						else if(_tcscmp(_T("event"),AGroupNode.GetAttributeValue(_T("msgtype"))) == 0){
							if(!pManager->AddActionScriptGroup(
								AGroupNode.GetAttributeValue(_T("name")),
								(EVENTTYPE_UI)_ttoi(AGroupNode.GetAttributeValue(_T("msgvalue"))),
								iDefaultInterval,iDefaultTimer,
								bDefaultReverse,bDefaultLoop,bDefaultAutoStart
								))
								continue;
						}

						TAGroup* pTAGroup = pManager->GetActionScriptGroup(AGroupNode.GetAttributeValue(_T("name")));
						if(!pTAGroup)
							continue;

						for(CMarkupNode ActionNode = AGroupNode.GetChild();ActionNode.IsValid();ActionNode = ActionNode.GetSibling()){
							if( _tcscmp(ActionNode.GetName(), _T("Property")) != 0 
								|| !ActionNode.HasAttribute(_T("name")) 
								|| !ActionNode.HasAttribute(_T("type")) 
								|| (!ActionNode.HasAttribute(_T("startvalue")) || !ActionNode.HasAttribute(_T("endvalue")))
								|| _tcscmp(ActionNode.GetAttributeValue(_T("startvalue")),ActionNode.GetAttributeValue(_T("endvalue"))) == 0)
								continue;

							int  iInterval	= iDefaultInterval;
							int	 iTimer		= iDefaultTimer;
							int	 iDelay		= 0;
							bool bAutoStart	= bDefaultAutoStart;
							bool bLoop		= bDefaultLoop;
							bool bReverse	= bDefaultReverse;

							LPCTSTR sName		= ActionNode.GetAttributeValue(_T("name"));
							LPCTSTR sType		= ActionNode.GetAttributeValue(_T("type"));
							LPCTSTR sStartVal	= _T("none");
							LPCTSTR sEndVal		= _T("none");

							if(ActionNode.HasAttribute(_T("startvalue")))
								sStartVal	= ActionNode.GetAttributeValue(_T("startvalue"));
							if(ActionNode.HasAttribute(_T("endvalue")))
								sEndVal		= ActionNode.GetAttributeValue(_T("endvalue"));
							if(ActionNode.HasAttribute(_T("interval")))
								iInterval	= _ttoi(ActionNode.GetAttributeValue(_T("interval"))) <= 0 ?0:_ttoi(ActionNode.GetAttributeValue(_T("interval")));
							if(ActionNode.HasAttribute(_T("timer")))
								iTimer		= _ttoi(ActionNode.GetAttributeValue(_T("timer"))) <= 0 ?0:_ttoi(ActionNode.GetAttributeValue(_T("timer")));
							if(ActionNode.HasAttribute(_T("autostart")))
								bAutoStart	= _tcscmp(_T("true"),ActionNode.GetAttributeValue(_T("autostart"))) == 0?true:false;
							if(ActionNode.HasAttribute(_T("loop")))
								bLoop		= _tcscmp(_T("true"),ActionNode.GetAttributeValue(_T("loop"))) == 0?true:false;
							if(ActionNode.HasAttribute(_T("reverse")))
								bReverse	= _tcscmp(_T("true"),ActionNode.GetAttributeValue(_T("reverse"))) == 0?true:false;

							pManager->AddPropertyAction(*pTAGroup,sName,sType,sStartVal,sEndVal,iInterval,iTimer,iDelay,bReverse,bLoop,bAutoStart);
						}
					}
				}
				else if( _tcscmp(pstrClass, _T("Styles")) == 0) {

					nAttributes = node.GetAttributeCount();
					CDuiString pStrStylesName;
					bool nIsDefaultStyles = false;

					for( int i = 0; i < nAttributes; i++ ) {

						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);

						if( _tcscmp(pstrName, _T("name")) == 0 ){
							pStrStylesName = pstrValue;
						}
						else if( _tcscmp(pstrName, _T("default")) == 0 && _tcscmp(pstrValue, _T("true")) == 0 ){
							nIsDefaultStyles = true;
						}
						else if( _tcscmp(pstrName, _T("inherit")) == 0 && pstrValue ){
							if(CStdStringPtrMap* pStylesMap = pManager->GetControlsStyles(pstrValue))
							{
								for(int nIndex = 0;nIndex < pStylesMap->GetSize();nIndex++)
								{
									CDuiString nKey				= pStylesMap->GetAt(nIndex);
									CStdStringPtrMap* pStyleMap	= static_cast<CStdStringPtrMap*>(pStylesMap->Find(nKey.GetData()));
									if(pStyleMap)
									{
										for(int nSubIndex = 0;nSubIndex < pStyleMap->GetSize();nSubIndex++)
										{
											CDuiString nSubKey	= pStyleMap->GetAt(nSubIndex);
											CDuiString* nSubVal = static_cast<CDuiString*>(pStyleMap->Find(nSubKey.GetData()));
											pManager->SetControlStyle(nKey.GetData(),nSubKey.GetData(),nSubVal->GetData(),pStrStylesName.GetData());
										}
									}
								}
							}
						}
						else if( _tcscmp(pstrName, _T("stylefile")) == 0 ){
							CMarkup nStyleFile;
							if(nStyleFile.LoadFromFile(pstrValue)){
								for(CMarkupNode nStyleNode = nStyleFile.GetRoot().GetChild();nStyleNode.IsValid();nStyleNode = nStyleNode.GetSibling())
								{
									LPCTSTR pstrClass = nStyleNode.GetName();

									if( _tcscmp(pstrClass, _T("Style")) != 0 || !nStyleNode.HasAttribute(_T("stylename")))
										continue;

									CDuiString nControlStyleName = nStyleNode.GetAttributeValue(_T("stylename"));
									if(nControlStyleName.GetLength() <= 0)
										continue;
									
									int nStyleAttributes = nStyleNode.GetAttributeCount();
									for( int nIndex = 0; nIndex < nStyleAttributes; nIndex++ ) {
										pstrName = nStyleNode.GetAttributeName(nIndex);
										pstrValue = nStyleNode.GetAttributeValue(nIndex);
										if( _tcscmp(pstrName, _T("stylename")) == 0 )
											continue;
										pManager->SetControlStyle(nControlStyleName.GetData(),pstrName,pstrValue,pStrStylesName.GetData());
									}
								}
							}
						}
					}

					for(CMarkupNode nStyleNode = node.GetChild();nStyleNode.IsValid();nStyleNode = nStyleNode.GetSibling())
					{
						LPCTSTR pstrClass = nStyleNode.GetName();

						if( _tcscmp(pstrClass, _T("Style")) != 0 || !nStyleNode.HasAttribute(_T("stylename")))
							continue;

						CDuiString nControlStyleName = nStyleNode.GetAttributeValue(_T("stylename"));
						if(nControlStyleName.GetLength() <= 0)
							continue;

						int nStyleAttributes = nStyleNode.GetAttributeCount();
						for( int i = 0; i < nStyleAttributes; i++ ) {
							pstrName = nStyleNode.GetAttributeName(i);
							pstrValue = nStyleNode.GetAttributeValue(i);
							if( _tcscmp(pstrName, _T("stylename")) == 0 )
								continue;

							pManager->SetControlStyle(nControlStyleName.GetData(),pstrName,pstrValue,pStrStylesName.GetData());
						}
					}
					if(nIsDefaultStyles && !pStrStylesName.IsEmpty())
						pManager->SetCurStyles(pStrStylesName.GetData(),false);

					continue;
				}
				else if( _tcscmp(pstrClass, _T("EffectsStyles")) == 0 && node.HasChildren() ) {

					nAttributes = node.GetAttributeCount();

					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = node.GetAttributeName(i);
						pstrValue = node.GetAttributeValue(i);

						if( _tcscmp(pstrName, _T("stylefile")) == 0 ){
							CMarkup mEffectsStyleFile;
							if(mEffectsStyleFile.LoadFromFile(pstrValue))
							{
								for( CMarkupNode nEffectsStyleNode = mEffectsStyleFile.GetRoot().GetChild(); nEffectsStyleNode.IsValid(); nEffectsStyleNode = nEffectsStyleNode.GetSibling() ) {
									LPCTSTR pstrClass = nEffectsStyleNode.GetName();

									if( _tcscmp(pstrClass, _T("EffectsStyle")) != 0)
										continue;

									nAttributes = nEffectsStyleNode.GetAttributeCount();
									LPCTSTR pControlName = NULL;
									LPCTSTR pControlValue = NULL;
									for( int i = 0; i < nAttributes; i++ ) {
										pstrName = nEffectsStyleNode.GetAttributeName(i);
										pstrValue = nEffectsStyleNode.GetAttributeValue(i);
										if( _tcscmp(pstrName, _T("name")) == 0 ) {
											pControlName = pstrValue;
										}
										else if( _tcscmp(pstrName, _T("value")) == 0 ) {
											pControlValue = pstrValue;
										}
									}
									if( pControlName ) {
										pManager->AddEffectsStyle(pControlName, pControlValue);
									}
								}
							}
							break;
						}
					}

					for( CMarkupNode nEffectsStyleNode = node.GetChild() ; nEffectsStyleNode.IsValid(); nEffectsStyleNode = nEffectsStyleNode.GetSibling() ) {
						LPCTSTR pstrClass = nEffectsStyleNode.GetName();

						if( _tcscmp(pstrClass, _T("EffectsStyle")) != 0)
							continue;

						nAttributes = nEffectsStyleNode.GetAttributeCount();
						LPCTSTR pControlName = NULL;
						LPCTSTR pControlValue = NULL;
						for( int i = 0; i < nAttributes; i++ ) {
							pstrName = nEffectsStyleNode.GetAttributeName(i);
							pstrValue = nEffectsStyleNode.GetAttributeValue(i);
							if( _tcscmp(pstrName, _T("name")) == 0 ) {
								pControlName = pstrValue;
							}
							else if( _tcscmp(pstrName, _T("value")) == 0 ) {
								pControlValue = pstrValue;
							}
						}
						if( pControlName ) {
							pManager->AddEffectsStyle(pControlName, pControlValue);
						}
					}
				}
			}

			pstrClass = root.GetName();
			if( _tcscmp(pstrClass, _T("Window")) == 0 ) {
				if( pManager->GetPaintWindow() ) {
					int nAttributes = root.GetAttributeCount();
					for( int i = 0; i < nAttributes; i++ ) {
						pstrName = root.GetAttributeName(i);
						pstrValue = root.GetAttributeValue(i);
						if( _tcscmp(pstrName, _T("size")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetInitSize(cx, cy);
						} 
						else if( _tcscmp(pstrName, _T("sizebox")) == 0 ) {
							RECT rcSizeBox = { 0 };
							LPTSTR pstr = NULL;
							rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
							rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
							rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
							pManager->SetSizeBox(rcSizeBox);
						}
						else if( _tcscmp(pstrName, _T("caption")) == 0 ) {
							RECT rcCaption = { 0 };
							LPTSTR pstr = NULL;
							rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
							rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
							rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
							pManager->SetCaptionRect(rcCaption);
						}
						else if( _tcscmp(pstrName, _T("roundcorner")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetRoundCorner(cx, cy);
						} 
						else if( _tcscmp(pstrName, _T("mininfo")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetMinInfo(cx, cy);
						}
						else if( _tcscmp(pstrName, _T("maxinfo")) == 0 ) {
							LPTSTR pstr = NULL;
							int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
							int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
							pManager->SetMaxInfo(cx, cy);
						}
						else if( _tcscmp(pstrName, _T("showdirty")) == 0 ) {
							pManager->SetShowUpdateRect(_tcscmp(pstrValue, _T("true")) == 0);
						} 
						else if( _tcscmp(pstrName, _T("alpha")) == 0 ) {
							pManager->SetTransparent(_ttoi(pstrValue));
						} 
						else if( _tcscmp(pstrName, _T("bktrans")) == 0 ) {
							pManager->SetBackgroundTransparent(_tcscmp(pstrValue, _T("true")) == 0);
						} 
						else if( _tcscmp(pstrName, _T("disabledfontcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultDisabledColor(clrColor);
						} 
						else if( _tcscmp(pstrName, _T("defaultfontcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultFontColor(clrColor);
						}
						else if( _tcscmp(pstrName, _T("linkfontcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultLinkFontColor(clrColor);
						} 
						else if( _tcscmp(pstrName, _T("linkhoverfontcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultLinkHoverFontColor(clrColor);
						} 
						else if( _tcscmp(pstrName, _T("selectedcolor")) == 0 ) {
							if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
							LPTSTR pstr = NULL;
							DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
							pManager->SetDefaultSelectedBkColor(clrColor);
						}
						else if(_tcscmp(pstrName, _T("trayiconid")) == 0 ) {
							if(_ttoi(pstrValue) > 0)
								pManager->GetTrayObject().CreateTrayIcon(pManager->GetPaintWindow(),_ttoi(pstrValue));
						}
						else if(_tcscmp(pstrName, _T("traytiptext")) == 0 ) {
							pManager->GetTrayObject().SetTooltipText(pstrValue);
						}
					}
				}
			}
		}
		if(pManager->GetCurStylesName().IsEmpty() && pManager->GetStylesCount() > 0)
			pManager->SetCurStyles(0);
		return _Parse(&root, pParent, pManager);
	}

	CMarkup* CDialogBuilder::GetMarkup()
	{
		return &m_xml;
	}

	void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
	{
		return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
	}

	void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
	{
		return m_xml.GetLastErrorLocation(pstrSource, cchMax);
	}

	CControlUI* CDialogBuilder::_Parse(CMarkupNode* pRoot, CControlUI* pParent, CPaintManagerUI* pManager)
	{
		IContainerUI* pContainer = NULL;
		CControlUI* pReturn = NULL;
		for( CMarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() ) {
			LPCTSTR pstrClass = node.GetName();
			if( _tcscmp(pstrClass, _T("Image")) == 0 || _tcscmp(pstrClass, _T("Font")) == 0 \
				|| _tcscmp(pstrClass, _T("Default")) == 0 || _tcscmp(pstrClass, _T("EffectsStyles")) == 0
				|| _tcscmp(pstrClass, _T("Styles")) == 0 || _tcscmp(pstrClass, _T("ActionScript")) == 0) continue;

			CControlUI* pControl = NULL;
			if( _tcscmp(pstrClass, _T("Include")) == 0 ) {
				if( !node.HasAttributes() ) continue;
				int count = 1;
				LPTSTR pstr = NULL;
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				if ( node.GetAttributeValue(_T("count"), szValue, cchLen) )
					count = _tcstol(szValue, &pstr, 10);
				cchLen = lengthof(szValue) - 1;
				if ( !node.GetAttributeValue(_T("source"), szValue, cchLen) ) continue;
				for ( int i = 0; i < count; i++ ) {
					CDialogBuilder builder;
					if( m_pstrtype != NULL ) { // ʹ����Դdll������Դ�ж�ȡ
						WORD id = (WORD)_tcstol(szValue, &pstr, 10); 
						pControl = builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
					}
					else {
						pControl = builder.Create((LPCTSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
					}
				}
				continue;
			}
			//���ؼ�XML����
			else if( _tcscmp(pstrClass, _T("TreeNode")) == 0 && pParent ) {
				CTreeNodeUI* pParentNode	= static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
				CTreeNodeUI* pNode			= new CTreeNodeUI();
				if(pParentNode){
					if(!pParentNode->Add(pNode)){
						delete pNode;
						continue;
					}
				}

				// ���пؼ�Ĭ�������ȳ�ʼ��Ĭ������
				if( pManager ) {
					pNode->SetManager(pManager, NULL, false);
					LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
					if( pDefaultAttributes ) {
						pNode->ApplyAttributeList(pDefaultAttributes);
					}
				}

				// ��������XML���Բ�����Ĭ������
				if( node.HasAttributes() ) {
					int nAttributes = node.GetAttributeCount();
					for( int i = 0; i < nAttributes; i++ ) {
						pNode->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
					}
				}

				//�����ӽڵ㼰���ӿؼ�
				if(node.HasChildren())
					CControlUI* pSubControl = _Parse(&node,pNode,pManager);
				
				if(!pParentNode){
					CTreeViewUI* pTreeView = static_cast<CTreeViewUI*>(pParent->GetInterface(_T("TreeView")));
					ASSERT(pTreeView);
					if( pTreeView == NULL ) return NULL;
					if( !pTreeView->Add(pNode) ) {
						delete pNode;
						continue;
					}
				}
				continue;
			}
			else {
				CDuiString szNodeName;
				szNodeName.Format(_T("C%sUI"),pstrClass);
				pControl = CreateDuiInstance<CControlUI*>(szNodeName.GetData());

				// User-supplied control factory
				if( pControl == NULL ) {
					CStdPtrArray* pPlugins = CPaintManagerUI::GetPlugins();
					LPCREATECONTROL lpCreateControl = NULL;
					for( int i = 0; i < pPlugins->GetSize(); ++i ) {
						lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);
						if( lpCreateControl != NULL ) {
							pControl = lpCreateControl(pstrClass);
							if( pControl != NULL ) break;
						}
					}
				}
				if( pControl == NULL && m_pCallback != NULL ) {
					pControl = m_pCallback->CreateControl(pstrClass);
				}
			}

#ifndef _DEBUG
			ASSERT(pControl);
#endif // _DEBUG
			if( pControl == NULL )
			{
#ifdef _DEBUG
				DUITRACE(_T("δ֪�ؼ�:%s"),pstrClass);
#else
				continue;
#endif
			}

			// Add children
			if( node.HasChildren() ) {
				_Parse(&node, pControl, pManager);
			}
			// Attach to parent
			// ��ΪĳЩ���Ժ͸�������أ�����selected��������Add��������
			if( pParent != NULL ) {
				CTreeNodeUI* pContainerNode = static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
				if(pContainerNode)
					pContainerNode->GetTreeNodeHoriznotal()->Add(pControl);
				else
				{
					if( pContainer == NULL ) pContainer = static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer")));
					ASSERT(pContainer);
					if( pContainer == NULL ) return NULL;
					if( !pContainer->Add(pControl) ) {
						delete pControl;
						continue;
					}
				}
			}
			// Init default attributes
			if( pManager ) {
				pControl->SetManager(pManager, NULL, false);
				LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
				if( pDefaultAttributes ) {
					pControl->ApplyAttributeList(pDefaultAttributes);
				}
			}
			// ������������ģ�棬���ʼ��ģ�����ԣ�������Default�����Ĭ������
			if( node.HasAttributes() ) {
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				
				CDuiString szName = node.GetAttributeValue(_T("name"));

				pControl->SetStyleName(node.GetName());
				
				if(node.HasAttribute(_T("name")) && szName.GetLength() > 0)
					pControl->SetStyleName(szName.GetData());

				for( int i = 0; i < nAttributes; i++ ) {
					if(_tcscmp(node.GetAttributeName(i),_T("style")) == 0){
						pControl->SetStyleName(node.GetAttributeValue(i),pManager);
						break;
					}
				}
			}
			// Process attributes
			if( node.HasAttributes() ) {
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) {
					if(_tcscmp(node.GetAttributeName(i),_T("style")) == 0){
						continue;
					}
					pControl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
				}
			}
			if( pManager ) {
				pControl->SetManager(NULL, NULL, false);
			}
			// Return first item
			if( pReturn == NULL ) pReturn = pControl;
		}
		return pReturn;
	}

} // namespace UiLib
