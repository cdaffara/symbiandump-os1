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

#ifndef __CONTAINDRAWER_H__
#define __CONTAINDRAWER_H__

#include <graphics/wsgraphicdrawer.h>
#include <graphics/wsgraphicdrawerinterface.h>

class MWsGraphicDrawerContext;

NONSHARABLE_CLASS(CWsContainGraphicDrawer): public CWsGraphicDrawer
	{
public:
	enum {EImplUid = 0x10283521};	
		
public:		
	static CWsContainGraphicDrawer* NewL();
	virtual ~CWsContainGraphicDrawer();
	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
	virtual void HandleMessage(const TDesC8& aData);
private:
	// override CWsGraphicDrawer
	virtual void DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const;
    virtual TBool HasAsChild(const TArray<TGraphicDrawerId>& aIds) const;

	void DoUpdateColor(TRgb aColor);
private:
	MWsGraphicDrawerContext* iContext;
	TRgb iColor;
};

#endif
