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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef BUFFERDRAWER_H_
#define BUFFERDRAWER_H_

#include <bitstd.h>
#include <graphics/wsgraphicdrawer.h>
#include <graphics/wsgraphicdrawerinterface.h>

// A buffer drawer 
NONSHARABLE_CLASS(CWsBufferGraphicDrawer): public CWsGraphicDrawer
	{
public:
	enum {EImplUid = 0x10285D54};	
		
public:		
	static CWsBufferGraphicDrawer* NewL();
	virtual ~CWsBufferGraphicDrawer();
 	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
	virtual void HandleMessage(const TDesC8& aData);
	
protected:
	// override CWsGraphicDrawer
	virtual void DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const;
	
private:
	void DoUpdateWhiteLinePos(TInt aWhiteLinePos);
	
private:
	TInt iWhiteLinePos;
};


#endif /*BUFFERDRAWER_H_*/
