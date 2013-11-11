// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "containdrawer.h"
#include "wsgraphicdrawercontext.h"
#include <s32mem.h>
#include <s32strm.h>

const TGraphicDrawerId KChildInterfaceId1={0x1028353A, ETrue};
const TGraphicDrawerId KChildInterfaceId2={0x1028353C, ETrue};


// CWsGraphicDrawer	
CWsContainGraphicDrawer* CWsContainGraphicDrawer::NewL()
	{
	return new(ELeave) CWsContainGraphicDrawer;	
	}
	
CWsContainGraphicDrawer::~CWsContainGraphicDrawer()
	{
	if (iContext)
		{
		iContext->Destroy();
		iContext = NULL;
		}
	}

void CWsContainGraphicDrawer::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv, aId, aOwner);
	iColor = KRgbWhite;
	if (!(aEnv.Screen(0)->ResolveObjectInterface(KMWsCompositionContext) || aEnv.Screen(0)->ResolveObjectInterface(KMWsScene)))
		{
		iContext = CWsGraphicDrawerNonNgaContext::NewL();
		}
	else
		{
		iContext = CWsGraphicDrawerNgaContext::NewL();
		}
	}

void CWsContainGraphicDrawer::HandleMessage(const TDesC8& aData)
	{
	TInt red = aData[0];
	TInt green = aData[1];
	TInt blue = aData[2];
	TRgb color(red, green, blue);
	DoUpdateColor(color);
	}
	
void CWsContainGraphicDrawer::DoUpdateColor(TRgb aColor)
	{
    iColor = aColor;
    // Invalidate the redrawing
    Invalidate();
 	}

void CWsContainGraphicDrawer::DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const
	{
	iContext->DrawEllipse(aGc, aRect, iColor);

	// invoke another CRPs to draw their artwork
    TRect rect1 = TRect(TPoint(100,150),TSize(50,50));
	const CWsGraphicDrawer* child1 = Env().ResolveGraphic(KChildInterfaceId1);
   	if (child1)
   		child1->Draw(aGc, rect1, aData);	
	TRect rect2 = TRect(TPoint(200,150),TSize(50,50));
   	const CWsGraphicDrawer* child2 = Env().ResolveGraphic(KChildInterfaceId2);
   	if (child2)
        child2->Draw(aGc, rect2, aData);		
	}
	
// inherited API to indicate there are contained drawers	
TBool CWsContainGraphicDrawer::HasAsChild(const TArray<TGraphicDrawerId>& aIds) const
	{
	// Two child CRPs
	const CWsGraphicDrawer* child1 = Env().ResolveGraphic(KChildInterfaceId1);
   	const CWsGraphicDrawer* child2 = Env().ResolveGraphic(KChildInterfaceId2);
   	if (child1 && child1->Contains(aIds))
   		return ETrue;
   	else if (child2 && child2->Contains(aIds))
   		return ETrue;
   	else
   		return EFalse;
	}
