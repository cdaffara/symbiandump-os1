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

#ifndef __SIMPLEDRAWER_H__
#define __SIMPLEDRAWER_H__

#include <graphics/wsgraphicdrawer.h>
#include <graphics/wsgraphicdrawerinterface.h>

class MWsGraphicDrawerContext;

// A simple drawer
NONSHARABLE_CLASS(CWsSimpleGraphicDrawer): public CWsGraphicDrawer//, public MWsEventHandler
	{
public:
	enum {EImplUid = 0x1028351C};	
		
public:		
	static CWsSimpleGraphicDrawer* NewL();
	virtual ~CWsSimpleGraphicDrawer();
 	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
	virtual void HandleMessage(const TDesC8& aData);
protected:
	// override CWsGraphicDrawer
	void BaseConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner);
	virtual void DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const;
 	void DoUpdateColor(TRgb aColor);
protected:
	MWsGraphicDrawerContext* iContext;
	TRgb iColor;
};

// First Contained (Child) drawer
NONSHARABLE_CLASS(CWsInvisibleGraphicDrawer1): public CWsSimpleGraphicDrawer//, public MWsEventHandler
	{
public:
	enum {EImplUid = 0x10283539};	
		
public:		
	static CWsInvisibleGraphicDrawer1* NewL();
	virtual ~CWsInvisibleGraphicDrawer1();
	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
};

// Second contained (child) drawer
NONSHARABLE_CLASS(CWsInvisibleGraphicDrawer2): public CWsSimpleGraphicDrawer//, public MWsEventHandler
	{
public:
	enum {EImplUid = 0x1028353B};	
		
public:		
	static CWsInvisibleGraphicDrawer2* NewL();
	virtual ~CWsInvisibleGraphicDrawer2();
	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
};

#endif
