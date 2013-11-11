// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DRAWSECTION_H__
#define __DRAWSECTION_H__

#include <w32std.h>
#include <s32mem.h>

class TWsGraphicMsgBufParser;

/**
CDrawSection contains information about one recorded redraw in the CCommandBuffer.
It has a segmented buffer containing all drawcommands that belongs to this drawsection.
*/
NONSHARABLE_CLASS(CDrawSection) : public CBase
	{
public:
	static CDrawSection* NewL();		
	static CDrawSection* NewL(const TRect& aDrawRect, const TRect& aBoundingRect, TBool aHasBitmapCommand);
	~CDrawSection();
	
	void SetBuffer(CBufBase* aSegBuf);
	CBufBase* Buffer() const;	
	void SetHasBeenExternalized(TBool aHasBeenExternalized);
	TBool HasBeenExternalized() const;
	TBool IsIdentical(const CDrawSection& aDrawSection) const;
	void ExternalizeL(RWsGraphicMsgBuf& aMsgBuf);
	TInt LoadL(const TWsGraphicMsgBufParser& aData, TInt aIndex);
	TRect DrawRect() const;
	
private:
	CDrawSection();
	CDrawSection(const TRect& aDrawRect, const TRect& aBoundingRect, TBool aHasBitmapCommand);
	
private:
	TRect iDrawRect;
	TRect iBoundingRect;
	TBool iHasBitmapCommand;
	TBool iHasBeenExternalized;	
	CBufBase* iSectionSegBuf;
	};
	
#endif	// __DRAWSECTION_H__
