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

#include "graphicscontextstate.h"
#include "cliwin.h"

/*------------------------------------------------------------------------------
  Description: Resets internal status of the graphics context to pre-defined values.
 -----------------------------------------------------------------------------*/
void TInternalGcStatus::ResetInternalStatus(CWsClientWindow* aWin)
	{
	iDrawMode = CGraphicsContext::EDrawModePEN;
	iPenColor = KRgbBlack;
	iBrushColor = aWin ? aWin->BackColor() : KRgbWhite;
	iPenStyle = CGraphicsContext::ESolidPen;
	iBrushStyle = CGraphicsContext::ENullBrush;
	iPenSize = TSize(1,1);
	iFontHandle = NULL;
	iUnderline = EUnderlineOff;
	iStrikethrough = EStrikethroughOff;
	iBrushPatternHandle = NULL;
	iBrushOrigin = TPoint(0,0);
	iCharExcessWidth = 0;
	iCharNumChars = 0;
	iWordExcessWidth = 0;
	iWordNumChars = 0;
	iOrigin = TPoint(0,0);
	iShadowColor = KRgbGray; 
	}

/*------------------------------------------------------------------------------
  Description: Helper function to retrieve current data from the given buffer and 
  set graphics context.
 -----------------------------------------------------------------------------*/
void TInternalGcStatus::InternalizeGcAttributesL(MWsGraphicsContext* aGc, RReadStream& aReadStream)
	{
	aGc->SetDrawMode((MWsGraphicsContext::TDrawMode)(aReadStream.ReadUint8L()));
	
	MWsGraphicsContext::TBrushStyle brushStyle = (MWsGraphicsContext::TBrushStyle) (aReadStream.ReadUint8L());
	aGc->SetPenStyle((MWsGraphicsContext::TPenStyle) (aReadStream.ReadUint8L()));

	TRgb penColor;
	penColor.InternalizeL(aReadStream); 
	aGc->SetPenColor(penColor);

	TRgb brushColor;
	brushColor.InternalizeL(aReadStream);
	aGc->SetBrushColor(brushColor);
	
	TSize size;
	aReadStream >> size;
	aGc->SetPenSize(size);

	aGc->ResetBrushPattern();
	TInt brushHandle = aReadStream.ReadInt32L();
	if(brushHandle)
		{
		aGc->SetBrushPattern(brushHandle);
		if (!aGc->HasBrushPattern() && brushStyle == MWsGraphicsContext::EPatternedBrush)
			{
			// Panic may occur if trying to set EPatternedBrush when a bitmap has not been
			// successfully set, so revert to null brush.
			brushStyle = MWsGraphicsContext::ENullBrush;
			}
		}
	// Wait until any brush pattern has been set before setting the brush style.
	aGc->SetBrushStyle(brushStyle);
	
	TInt wordExcessWidth = aReadStream.ReadUint32L();
	TInt wordNumChars = aReadStream.ReadUint32L();
	aGc->SetWordJustification(wordExcessWidth, wordNumChars);

	TInt charExcessWidth = aReadStream.ReadUint32L();
	TInt charNumChars = aReadStream.ReadUint32L();
	aGc->SetCharJustification(charExcessWidth, charNumChars);
	
	TRgb shadowColor;
	shadowColor.InternalizeL(aReadStream);
	aGc->SetTextShadowColor(shadowColor);
	
	TPoint origin;
	aReadStream >> origin;
	aGc->SetOrigin(origin);

	TPoint brushOrigin;
	aReadStream >> brushOrigin;
	aGc->SetBrushOrigin(brushOrigin);
	
	aGc->SetUnderlineStyle((MWsGraphicsContext::TFontUnderline) (aReadStream.ReadUint8L()));
 	aGc->SetStrikethroughStyle((MWsGraphicsContext::TFontStrikethrough) (aReadStream.ReadUint8L()));
	
	aGc->ResetFont();
	TInt fontHandle = aReadStream.ReadInt32L();
	if(fontHandle)
		{
		CFbsBitGcFont font;
		TInt res = font.Duplicate(fontHandle);
		if(res == KErrNone)
			{
			aGc->SetFont(&font);
			font.Reset();
			}
		}
	}

/*------------------------------------------------------------------------------
  Description: Helper function to save graphics context information into a given buffer.
 -----------------------------------------------------------------------------*/
void TInternalGcStatus::ExternalizeGcAttributesL(RWriteStream& aWriteStream)
	{
	aWriteStream.WriteUint8L(iDrawMode);
	aWriteStream.WriteUint8L(iBrushStyle);
	aWriteStream.WriteUint8L(iPenStyle);
	iPenColor.ExternalizeL(aWriteStream); 
	iBrushColor.ExternalizeL(aWriteStream);
	aWriteStream << iPenSize;
	aWriteStream.WriteInt32L(iBrushPatternHandle);
	aWriteStream.WriteUint32L(iWordExcessWidth);
	aWriteStream.WriteUint32L(iWordNumChars);
	aWriteStream.WriteUint32L(iCharExcessWidth);
	aWriteStream.WriteUint32L(iCharNumChars);
	iShadowColor.ExternalizeL(aWriteStream);
	aWriteStream << iOrigin;
	aWriteStream << iBrushOrigin;
	aWriteStream.WriteUint8L(iUnderline);
	aWriteStream.WriteUint8L(iStrikethrough);
	aWriteStream.WriteInt32L(iFontHandle);
	}

