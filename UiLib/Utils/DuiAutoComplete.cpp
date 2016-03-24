#include "stdafx.h"
#include <share.h>
#include "DuiAutoComplete.h"

#define ARRSIZE(x)	(sizeof(x)/sizeof(x[0]))

namespace UiLib
{

	//************************************
	// ��������: CDuiAutoComplete
	// ��������: 
	// ����˵��: 
	//************************************
	CDuiAutoComplete::CDuiAutoComplete()
	{
		InternalInit();
	}

	//************************************
	// ��������: ~CDuiAutoComplete
	// ��������: 
	// ����˵��: 
	//************************************
	CDuiAutoComplete::~CDuiAutoComplete()
	{
		Clear();
		if (m_fBound && GetIAutoComplete())
			GetIAutoComplete().Release();
		m_fBound = FALSE;
	}

	//************************************
	// ��������: Bind
	// ��������: BOOL
	// ������Ϣ: HWND p_hWndEdit
	// ������Ϣ: DWORD p_dwOptions
	// ������Ϣ: LPCTSTR p_lpszFormatString
	// ����˵��: 
	//************************************
	BOOL CDuiAutoComplete::Bind(HWND p_hWndEdit, DWORD p_dwOptions, LPCTSTR p_lpszFormatString)
	{
		ATLASSERT(::IsWindow(p_hWndEdit));
		if ((m_fBound) || (GetIAutoComplete()))
			return FALSE;

		HRESULT hr = GetIAutoComplete().CoCreateInstance(CLSID_AutoComplete);
		if (SUCCEEDED(hr))
		{
			if (p_dwOptions){
				CComQIPtr<IAutoComplete2> pAC2(GetIAutoComplete());
				if (pAC2){
					pAC2->SetOptions(p_dwOptions);
					pAC2.Release();
				}
			}

			USES_CONVERSION;
			if (SUCCEEDED(hr = GetIAutoComplete()->Init(p_hWndEdit, this, NULL, T2CW(p_lpszFormatString))))
			{
				m_fBound = TRUE;
				return TRUE;
			}
		}
		return FALSE;
	}

	//************************************
	// ��������: Unbind
	// ��������: VOID
	// ����˵��: 
	//************************************
	VOID CDuiAutoComplete::Unbind()
	{
		if (!m_fBound)
			return;
		if (m_fBound && GetIAutoComplete()){
			GetIAutoComplete().Release();
			m_fBound = FALSE;
		}
	}

	//************************************
	// ��������: FindItem
	// ��������: int
	// ������Ϣ: const CDuiString & rstr
	// ����˵��: 
	//************************************
	int CDuiAutoComplete::FindItem(const CDuiString& rstr)
	{
		for (int i = 0; i < m_sStringArrayMap.GetSize(); i++)
			if (m_sStringArrayMap.GetAt(i)->Compare(rstr.GetData()) == 0)
				return i;

		return -1;
	}

	//************************************
	// ��������: AddItem
	// ��������: BOOL
	// ������Ϣ: const CDuiString & p_sItem
	// ������Ϣ: int iPos
	// ����˵��: 
	//************************************
	BOOL CDuiAutoComplete::AddItem(const CDuiString& p_sItem, int iPos)
	{
		if (p_sItem.GetLength() != 0)
		{
			int oldpos=FindItem(p_sItem);
			if (oldpos == -1)
			{
				if (iPos == -1)
					m_sStringArrayMap.Add(new CDuiString(p_sItem));
				else
					m_sStringArrayMap.InsertAt(iPos,new CDuiString(p_sItem));

				while (m_sStringArrayMap.GetSize() > m_iMaxItemCount)
					m_sStringArrayMap.Remove(m_sStringArrayMap.GetSize() - 1,true);
				return TRUE;
			} else if (iPos!=-1) {
				m_sStringArrayMap.Remove(oldpos,true);
				if (oldpos<iPos) --iPos;
				m_sStringArrayMap.InsertAt(iPos,new CDuiString(p_sItem));

				while (m_sStringArrayMap.GetSize() > m_iMaxItemCount)
					m_sStringArrayMap.Remove(m_sStringArrayMap.GetSize() - 1,true);
				return TRUE;
			}
		}
		return FALSE;
	}

	//************************************
	// ��������: GetItemCount
	// ��������: int
	// ����˵��: 
	//************************************
	int CDuiAutoComplete::GetItemCount()
	{
		return (int)m_sStringArrayMap.GetSize();
	}

	//************************************
	// ��������: RemoveItem
	// ��������: BOOL
	// ������Ϣ: const CDuiString & p_sItem
	// ����˵��: 
	//************************************
	BOOL CDuiAutoComplete::RemoveItem(const CDuiString& p_sItem)
	{
		if (p_sItem.GetLength() != 0)
		{
			int iPos = FindItem(p_sItem);
			if (iPos != -1)
			{
				m_sStringArrayMap.Remove(iPos,true);
				return TRUE;
			}
		}
		return FALSE;
	}


	//************************************
	// ��������: Clear
	// ��������: BOOL
	// ����˵��: 
	//************************************
	BOOL CDuiAutoComplete::Clear()
	{
		if (m_sStringArrayMap.GetSize() != 0)
		{
			while (m_sStringArrayMap.GetSize() > 0)
				m_sStringArrayMap.Remove(m_sStringArrayMap.GetSize()-1,true);

			return TRUE;
		}
		return FALSE;
	}

	//************************************
	// ��������: Disable
	// ��������: BOOL
	// ����˵��: 
	//************************************
	BOOL CDuiAutoComplete::Disable()
	{
		if ((!GetIAutoComplete()) || (!m_fBound))
			return FALSE;
		return SUCCEEDED(EnDisable(FALSE));
	}

	//************************************
	// ��������: Enable
	// ��������: BOOL
	// ������Ϣ: VOID
	// ����˵��: 
	//************************************
	BOOL CDuiAutoComplete::Enable(VOID)
	{
		if ((!GetIAutoComplete()) || (m_fBound))
			return FALSE;
		return SUCCEEDED(EnDisable(TRUE));
	}

	//************************************
	// ��������: GetStringArray
	// ��������: const TStdPtrArray<CDuiString*>&
	// ����˵��: 
	//************************************
	const TStdPtrArray<CDuiString*>& CDuiAutoComplete::GetStringArray() const
	{
		return m_sStringArrayMap;
	}

	//************************************
	// ��������: AddRef
	// ��������: STDMETHODIMP_(ULONG)
	// ����˵��: 
	//************************************
	STDMETHODIMP_(ULONG) CDuiAutoComplete::AddRef()
	{
		ULONG nCount = ::InterlockedIncrement(reinterpret_cast<LONG*>(&m_nRefCount));
		return nCount;
	}

	//************************************
	// ��������: Release
	// ��������: STDMETHODIMP_(ULONG)
	// ����˵��: 
	//************************************
	STDMETHODIMP_(ULONG) CDuiAutoComplete::Release()
	{
		ULONG nCount = 0;
		nCount = (ULONG) ::InterlockedDecrement(reinterpret_cast<LONG*>(&m_nRefCount));
		return nCount;
	}

	//************************************
	// ��������: QueryInterface
	// ��������: STDMETHODIMP
	// ������Ϣ: REFIID riid
	// ������Ϣ: void * * ppvObject
	// ����˵��: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::QueryInterface(REFIID riid, void** ppvObject)
	{
		HRESULT hr = E_NOINTERFACE;
		if (ppvObject != NULL)
		{
			*ppvObject = NULL;

			if (IID_IUnknown == riid)
				*ppvObject = static_cast<IUnknown*>(this);
			else if (IID_IEnumString == riid)
				*ppvObject = static_cast<IEnumString*>(this);
			if (*ppvObject != NULL)
			{
				hr = S_OK;
				((LPUNKNOWN)*ppvObject)->AddRef();
			}
		}
		else
			hr = E_POINTER;
		return hr;
	}

	//************************************
	// ��������: Next
	// ��������: STDMETHODIMP
	// ������Ϣ: ULONG celt
	// ������Ϣ: LPOLESTR * rgelt
	// ������Ϣ: ULONG * pceltFetched
	// ����˵��: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched)
	{
		USES_CONVERSION;
		HRESULT hr = S_FALSE;

		if (!celt)
			celt = 1;
		ULONG i;
		for (i = 0; i < celt; i++)
		{
			if (m_nCurrentElement == (ULONG)m_sStringArrayMap.GetSize())
				break;

			rgelt[i] = (LPWSTR)::CoTaskMemAlloc((ULONG) sizeof(WCHAR) * (m_sStringArrayMap.GetAt(m_nCurrentElement)->GetLength() + 1));
			wcscpy(rgelt[i], T2CW(m_sStringArrayMap.GetAt(m_nCurrentElement)->GetData()));

			if (pceltFetched)
				*pceltFetched++;

			m_nCurrentElement++;
		}

		if (i == celt)
			hr = S_OK;

		return hr;
	}

	//************************************
	// ��������: Skip
	// ��������: STDMETHODIMP
	// ������Ϣ: ULONG celt
	// ����˵��: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::Skip(ULONG celt)
	{
		m_nCurrentElement += celt;
		if (m_nCurrentElement > (ULONG)m_sStringArrayMap.GetSize())
			m_nCurrentElement = 0;

		return S_OK;
	}

	//************************************
	// ��������: Reset
	// ��������: STDMETHODIMP
	// ������Ϣ: void
	// ����˵��: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::Reset(void)
	{
		m_nCurrentElement = 0;
		return S_OK;
	}

	//************************************
	// ��������: Clone
	// ��������: STDMETHODIMP
	// ������Ϣ: IEnumString * * ppenum
	// ����˵��: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::Clone(IEnumString** ppenum)
	{
		if (!ppenum)
			return E_POINTER;

		CDuiAutoComplete* pnew = new CDuiAutoComplete();
		pnew->AddRef();
		*ppenum = pnew;
		return S_OK;
	}

	//************************************
	// ��������: InternalInit
	// ��������: void
	// ����˵��: 
	//************************************
	void CDuiAutoComplete::InternalInit()
	{
		m_nCurrentElement = 0;
		m_nRefCount = 0;
		m_fBound = FALSE;
		m_iMaxItemCount = 30;
	}

	//************************************
	// ��������: EnDisable
	// ��������: HRESULT
	// ������Ϣ: BOOL p_fEnable
	// ����˵��: 
	//************************************
	HRESULT CDuiAutoComplete::EnDisable(BOOL p_fEnable)
	{
		HRESULT hr = GetIAutoComplete()->Enable(p_fEnable);
		if (SUCCEEDED(hr))
			m_fBound = p_fEnable;
		return hr;
	}

	//************************************
	// ��������: LoadList
	// ��������: BOOL
	// ������Ϣ: LPCTSTR pszFileName
	// ����˵��: 
	//************************************
	BOOL CDuiAutoComplete::LoadList(LPCTSTR pszFileName)
	{
		FILE* fp = _tfsopen(pszFileName, _T("rb"), _SH_DENYWR);
		if (fp == NULL)
			return FALSE;

		// У�� Unicode �����ֽ��� mark 0xFEFF
		WORD wBOM = fgetwc(fp);
		if (wBOM != 0xFEFF){
			fclose(fp);
			return FALSE;
		}

		TCHAR szItem[256];
		while (_fgetts(szItem, ARRSIZE(szItem), fp) != NULL){
			CDuiString strItem(szItem);
			strItem.Replace(_T("\r"),_T(""));
			strItem.Replace(_T("\n"),_T(""));
			AddItem(strItem, -1);
		}
		fclose(fp);
		return TRUE;
	}

	//************************************
	// ��������: SaveList
	// ��������: BOOL
	// ������Ϣ: LPCTSTR pszFileName
	// ����˵��: 
	//************************************
	BOOL CDuiAutoComplete::SaveList(LPCTSTR pszFileName)
	{
		FILE* fp = _tfsopen(pszFileName, _T("wb"), _SH_DENYWR);
		if (fp == NULL)
			return FALSE;

		// д Unicode �����ֽ��� mark 0xFEFF
		fputwc(0xFEFF, fp);

		for (int i = 0; i < m_sStringArrayMap.GetSize(); i++)
			_ftprintf(fp, _T("%s\r\n"), m_sStringArrayMap[i]);
		fclose(fp);
		return !ferror(fp);
	}

	//************************************
	// ��������: GetItem
	// ��������: UiLib::CDuiString
	// ������Ϣ: int pos
	// ����˵��: 
	//************************************
	CDuiString CDuiAutoComplete::GetItem(int pos){
		if (pos>=m_sStringArrayMap.GetSize())
			return CDuiString(); 
		else
			return m_sStringArrayMap.GetAt(pos)->GetData();
	}

	//************************************
	// ��������: GetIAutoComplete
	// ��������: CComPtr<IAutoComplete>&
	// ����˵��: 
	//************************************
	CComPtr<IAutoComplete>& CDuiAutoComplete::GetIAutoComplete()
	{
		static CComPtr<IAutoComplete> m_pac;
		return m_pac;
	}

}