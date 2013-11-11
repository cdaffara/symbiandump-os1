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

#include "simpledrawer.h"
#include "wsgraphicdrawercontext.h"
#include <s32mem.h>
#include <s32strm.h>

// CWsGraphicDrawer	
CWsSimpleGraphicDrawer* CWsSimpleGraphicDrawer::NewL()
	{
	return new(ELeave) CWsSimpleGraphicDrawer;	
	}
	
CWsSimpleGraphicDrawer::~CWsSimpleGraphicDrawer()
	{
	if (iContext)
		{
		iContext->Destroy();
		iContext = NULL;
		}
	}

void CWsSimpleGraphicDrawer::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv, aId, aOwner);
 	// default color
	iColor = KRgbBlue;
 	}

void CWsSimpleGraphicDrawer::HandleMessage(const TDesC8& aData)
	{
	TInt red = aData[0];
	TInt green = aData[1];
	TInt blue = aData[2];
	TRgb color(red, green, blue);
	DoUpdateColor(color);
	}

void CWsSimpleGraphicDrawer::BaseConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner)
	{
	CWsGraphicDrawer::BaseConstructL(aEnv, aId, aOwner);
	if (!(aEnv.Screen(0)->ResolveObjectInterface(KMWsCompositionContext) || aEnv.Screen(0)->ResolveObjectInterface(KMWsScene)))
		{
		iContext = CWsGraphicDrawerNonNgaContext::NewL();
		}
	else
		{
		iContext = CWsGraphicDrawerNgaContext::NewL();
		}
	}

void CWsSimpleGraphicDrawer::DoUpdateColor(TRgb aColor)
	{
	iColor = aColor;
	// Invalidate the redrawing
	Invalidate();
	}

void CWsSimpleGraphicDrawer::DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& /*aData*/) const
	{
	iContext->DrawEllipse(aGc, aRect, iColor);
	}
	
// First contained (child) drawer
CWsInvisibleGraphicDrawer1* CWsInvisibleGraphicDrawer1::NewL()
	{
	return new(ELeave) CWsInvisibleGraphicDrawer1;	
	}
	
CWsInvisibleGraphicDrawer1::~CWsInvisibleGraphicDrawer1()
	{
	}

void CWsInvisibleGraphicDrawer1::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv, aId, aOwner);
	// default color
	iColor = KRgbYellow;
	}

// Second contained (child) drawer
CWsInvisibleGraphicDrawer2* CWsInvisibleGraphicDrawer2::NewL()
	{
	return new(ELeave) CWsInvisibleGraphicDrawer2;	
	}
	
CWsInvisibleGraphicDrawer2::~CWsInvisibleGraphicDrawer2()
	{
	}

void CWsInvisibleGraphicDrawer2::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/)
	{
	BaseConstructL(aEnv, aId, aOwner);
	// default color
	iColor = KRgbCyan;
	}
