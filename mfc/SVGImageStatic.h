#pragma once

#include "BCGPStatic.h"
#include "BCGPSVGImage.h"

/// Use this control to display a *.SVG in dialog. The SVG file will be loaded from the respurces
class CSVGImageStatic : public CBCGPStatic
{
public:
	explicit CSVGImageStatic(int margin = 0, bool border = false);

	/// Load the file from the resources
	BOOL LoadFromResource(UINT res);
	/// Load the file from a file
	BOOL LoadFromFile(const CString &filename);
	/// Load the file from a string buffer
	BOOL LoadFromBuffer(const CString &buffer);

protected:
	void DoPaint(CDC* pDC) override;
	
private:
	int m_margin = 0;
	bool m_border = false;
	CBCGPSVGImage m_SVG;
};

