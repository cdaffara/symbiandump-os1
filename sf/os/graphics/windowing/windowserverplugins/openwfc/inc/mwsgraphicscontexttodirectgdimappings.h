// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __MWSGRAPHICSCONTEXTTODIRECTGDIMAPPINGS_H__
#define __MWSGRAPHICSCONTEXTTODIRECTGDIMAPPINGS_H__

#include <graphics/wsgraphicscontext.h>
#include <graphics/directgditypes.h>

class MWsGraphicsContextToDirectGdiMappings
	{
public:
	static DirectGdi::TFillRule Convert(MWsGraphicsContext::TFillRule aFillRule) { return (DirectGdi::TFillRule)aFillRule; }
	static DirectGdi::TPenStyle Convert(MWsGraphicsContext::TPenStyle aPenStyle) { return (DirectGdi::TPenStyle)aPenStyle; }
	static DirectGdi::TBrushStyle Convert(MWsGraphicsContext::TBrushStyle aBrushStyle) { return (DirectGdi::TBrushStyle)aBrushStyle; }
	static DirectGdi::TTextAlign Convert(MWsGraphicsContext::TTextAlign aTextAlign) { return (DirectGdi::TTextAlign)aTextAlign; }
	static DirectGdi::TFontUnderline Convert(MWsGraphicsContext::TFontUnderline aFontUnderline) { return (DirectGdi::TFontUnderline)aFontUnderline; }
	static DirectGdi::TFontStrikethrough Convert(MWsGraphicsContext::TFontStrikethrough aFontStrikethrough) { return (DirectGdi::TFontStrikethrough)aFontStrikethrough; }
	static const DirectGdi::TTextParameters* Convert(const MWsGraphicsContext::TTextParameters* aParam) { return reinterpret_cast<const DirectGdi::TTextParameters*>(aParam); }
	static DirectGdi::TDrawMode LossyConvert(MWsGraphicsContext::TDrawMode aDrawMode)
		{return (aDrawMode==MWsGraphicsContext::EDrawModeWriteAlpha ? DirectGdi::EDrawModeWriteAlpha : DirectGdi::EDrawModePEN);}
	};

#endif	//__MWSGRAPHICSCONTEXTTODIRECTGDIMAPPINGS_H__
