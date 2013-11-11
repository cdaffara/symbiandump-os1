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

#include "debugbardrawer.h"

CDebugBarDrawer::~CDebugBarDrawer()
	{
	if(iFontStore && iFont)
		iFontStore->ReleaseFont(iFont);
	delete iFontStore;
	iGc=NULL;
	}

CDebugBarDrawer::CDebugBarDrawer(CWsRenderStage* aRenderStage, TInt aScreenWidth):
	iRenderStage(aRenderStage), iScreenWidth(aScreenWidth)
	{}

void CDebugBarDrawer::ConstructL()
	{
	iGc = iRenderStage->ObjectInterface<MWsGraphicsContext>();
	TFontSpec fspec(_L("DejaVu Sans Condensed"), KFontHeightInPixel);		
	iFontStore = CFbsTypefaceStore::NewL(NULL);
	User::LeaveIfError(iFontStore->GetNearestFontToDesignHeightInPixels(iFont, fspec));		
	iBaseline = (iFont->BaselineOffsetInPixels() + iFont->AscentInPixels());
	}

CDebugBarDrawer* CDebugBarDrawer::NewL(CWsRenderStage* aRenderStage, TInt aScreenWidth)
	{
	CDebugBarDrawer* self = new(ELeave) CDebugBarDrawer(aRenderStage, aScreenWidth);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CDebugBarDrawer::DrawDebugBar(const TArray<TPtrC>& aDebugText)
	{
	if (iDebugBarRect.IsEmpty())
		{
		iDebugBarRect=TRect(TSize(iScreenWidth, KFontHeightInPixel*aDebugText.Count()));
		}
	TRegionFix<1> iBarRegion=TRegionFix<1>(iDebugBarRect);
	iRenderStage->Begin(&iBarRegion);
	iGc->Reset();
	iGc->SetDrawMode(MWsGraphicsContext::EDrawModePEN);
	iGc->SetPenColor(KRgbWhite);
	iGc->SetPenStyle(MWsGraphicsContext::ESolidPen);
	iGc->SetBrushStyle(MWsGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbBlack);
	iGc->DrawRect(iDebugBarRect);
	iGc->SetFont(iFont);		
	for (TInt k = 0; k < aDebugText.Count(); k++)
		{
		iGc->DrawText(aDebugText[k], NULL, TPoint(0, iBaseline*(k+1)+2));
		}	
	iRenderStage->End(NULL);	
	}


