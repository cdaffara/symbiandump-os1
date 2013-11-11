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

#ifndef RATELIMITERRENDERSTAGEFACTORY_H_
#define RATELIMITERRENDERSTAGEFACTORY_H_

#include <graphics/wsplugin.h>
#include <graphics/wsrenderstagefactory.h>

/**
This is an implementation of MWsRenderStageFactory, in the form of a CWsPlugin.
The window server will use this to create the rate limiter render stage,
if the ini file specifies them.
*/
class CRateLimiterRenderStageFactory : public CWsPlugin, public MWsRenderStageFactory
	{
public:
	enum { EImplUid = 0x102866C7 };

public:
	static CRateLimiterRenderStageFactory* CreateL();
	~CRateLimiterRenderStageFactory();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TDesC8& aData);
	const TDesC& PluginName() const;

public: // MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);

public: // MWsRenderStageFactory
	CWsRenderStage* CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, TInt aScreenNumber);
	CWsRenderStage* CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);

private:
	CRateLimiterRenderStageFactory();
	};

#endif /*RATELIMITERRENDERSTAGEFACTORY_H_*/
