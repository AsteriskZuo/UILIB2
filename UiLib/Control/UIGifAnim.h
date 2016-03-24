#ifndef GifAnimUI_h__
#define GifAnimUI_h__

#include <gdiplus.h>
using namespace Gdiplus;
#pragma once

namespace UiLib
{
	class CLabelUI;

#define EVENT_TIEM_ID	100

	class CGifAnimUI : public CLabelUI
	{
	public:
		CGifAnimUI(void);
		~CGifAnimUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	DoPaint(HDC hDC, const RECT& rcPaint);
		void	DoEvent(TEventUI& event);
	private:
		void	InitGifImage();
		void	DeleteGif();
		void    OnTimer( UINT_PTR idEvent );
		void	DrawFrame( HDC hDC );		// ����GIFÿ֡
	private:
		Image	*m_pGifImage;
		UINT	m_nFrameCount;				// gifͼƬ��֡��
		UINT	m_nFramePosition;			// ��ǰ�ŵ��ڼ�֡
		PropertyItem*	m_pPropertyItem;	// ֡��֮֡����ʱ��
	private:
		ULONG_PTR				m_gdiplusToken;
		GdiplusStartupInput		m_gdiplusStartupInput;
	};
}

#endif // GifAnimUI_h__
