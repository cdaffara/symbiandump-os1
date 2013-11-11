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

#ifndef RATELIMITERRENDERSTAGE_H_
#define RATELIMITERRENDERSTAGE_H_

#include <graphics/wsrenderstage.h>

class CRateLimiterRenderStage : public CWsRenderStage
	{
public:
	static CRateLimiterRenderStage* NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);
	~CRateLimiterRenderStage();

public: // from CWsRenderStage
	TAny* ResolveObjectInterface(TUint aTypeId);
	void Begin(const TRegion* aRegion);
	void End(TRequestStatus* aCompositorReady);	

private:
	CRateLimiterRenderStage(MWsGraphicDrawerEnvironment& aEnv);
	void ConstructL(CWsRenderStage* aNextStage);
	static TBool OnTickCallBack(TAny* aAny);

private:
	MWsGraphicDrawerEnvironment& iEnv;
	CActiveScheduler* iSched;
	CPeriodic* iTick;
	TRequestStatus* iCompositorReady;
	};

#endif /*RATELIMITERRENDERSTAGE_H_*/
