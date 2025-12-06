#include "pch.h"
#include "SVGImageStatic.h"
#include "BCGPVisualManager.h"


CSVGImageStatic::CSVGImageStatic(int margin, bool border)
	: m_margin(margin), m_border(border)
{
}

/// Load the file from the resources
/// @param[in] res	Resource ID
/// @return					success
BOOL CSVGImageStatic::LoadFromResource(UINT res)
{
	m_SVG.CleanUp();
	if (m_SVG.Load(res))
		{
		RedrawWindow();
		return TRUE;
		}
	else
		return FALSE;
}

void CSVGImageStatic::DoPaint(CDC *pDC)
{
	auto pVisualManager = CBCGPVisualManager::GetInstance();
	if (pDC && pVisualManager)
		{
		const int nMargin = globalUtils.ScaleByDPI(m_margin);
		CRect rectClient;
		GetClientRect(rectClient);
		CRect rectInfo = rectClient;
		pDC->FillRect(rectInfo, &pVisualManager->GetDlgBackBrush(GetParent()));
		rectInfo.DeflateRect(nMargin, nMargin);
		m_SVG.DoDraw(pDC, rectInfo, FALSE, 255, TRUE, TRUE);
		if (m_border)
			pVisualManager->OnDrawControlBorder(pDC, rectClient, this, FALSE);
		}
}


BOOL CSVGImageStatic::LoadFromFile(const CString &filename)
{
	m_SVG.CleanUp();
	if (m_SVG.LoadFromFile(filename))
		{
		RedrawWindow();
		return TRUE;
		}
	else
		return FALSE;
}

BOOL CSVGImageStatic::LoadFromBuffer(const CString &buffer)
{
	m_SVG.CleanUp();
	if (m_SVG.LoadFromBuffer(buffer))
		{
		RedrawWindow();
		return TRUE;
		}
	else
		return FALSE;
}
