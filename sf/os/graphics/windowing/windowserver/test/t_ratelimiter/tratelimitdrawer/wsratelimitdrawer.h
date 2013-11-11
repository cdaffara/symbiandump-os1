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
// Test GraphicTestFrameRate plugins
// 
//

/**
 @file
 @test - Test plug-in to test wsgraphic animation artwork framerate with rate limiting renterstage enabled
 @internalComponent - Internal Symbian test code
*/

#ifndef WSRATELIMITDRAWER_H_
#define WSRATELIMITDRAWER_H_

#include <w32std.h>

const TUid KRateLimitDrawerInterfaceId = {0x102869F5};
const TUid KRateLimitDrawerImplId = {0x102869F6};

/**
Holds draw count values of two different animations
*/
NONSHARABLE_STRUCT(TAnimData)
	{
	TReal iFrameRate;
	};
	
/** Client representation of a test anim that displays its frame-rate

@publishedPartner
@released
*/	
NONSHARABLE_CLASS(CWsRateLimitGraphic): public CWsGraphic

	{
public:
	IMPORT_C static CWsRateLimitGraphic* NewL(TUid aUid);
	IMPORT_C static CWsRateLimitGraphic* NewL();
	IMPORT_C ~CWsRateLimitGraphic();
	IMPORT_C TInt QueryPluginForFrameRate(TAnimData& aCount);
private:
	CWsRateLimitGraphic();
	void HandleMessage(const TDesC8& aData);
	void OnReplace();
private:
	TAnimData* iAnimData;
	};

#endif /*WSRATELIMITDRAWER_H_*/
