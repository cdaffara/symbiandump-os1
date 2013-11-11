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
// This is the actual plugin CWsGraphicDrawerCoverage
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test coverage code for GraphicDrawer related code
*/

#ifndef __WSGRAPHICDRAWER_COVERAGE_H__
#define __WSGRAPHICDRAWER_COVERAGE_H__

#include "graphics/WSGRAPHICDRAWER.H"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>

#include "gdcovcommon.h"

NONSHARABLE_CLASS(CWsGraphicDrawerCoverage): public CWsGraphicDrawer
	{
public:
	enum {EImplUid = 0x10285D5B};
		
public:		
	static CWsGraphicDrawerCoverage* NewL();
	virtual ~CWsGraphicDrawerCoverage();

	// override CWsGraphicDrawer
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
	virtual void HandleMessage(const TDesC8& aData);
private:
	// override CWsGraphicDrawer
	virtual void DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const;
	void SendInfo();

	TInt iScreenId;
	};

#endif
