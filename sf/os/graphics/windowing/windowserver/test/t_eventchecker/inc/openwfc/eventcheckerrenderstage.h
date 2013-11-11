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

#ifndef __EVENTCHECKERRENDERSTAGE_H__
#define __EVENTCHECKERRENDERSTAGE_H__

#include <graphics/wsrenderstage.h>

class CGraphicsContextChecker;
class CSceneChecker;

class CEventCheckerRenderStage: public CWsRenderStage
	{
public:
	static CEventCheckerRenderStage* NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);
	~CEventCheckerRenderStage();

public: // from CWsRenderStage
	TAny* ResolveObjectInterface(TUint aTypeId);
	void Begin(const TRegion* aRegion);
	void End(TRequestStatus* aCompositorReady);	

private:
	CEventCheckerRenderStage(MWsGraphicDrawerEnvironment& aEnv, MWsScreen& aScreen);
	void ConstructL(CWsRenderStage* aNextStage);

private:
	MWsGraphicDrawerEnvironment& iEnv;
	MWsScreen& iScreen;
	CGraphicsContextChecker* iGraphicsContextChecker;
	CSceneChecker* iSceneChecker;
	TBool iWithinBeginAndEnd;
	};

#endif // __EVENTCHECKERRENDERSTAGE_H__
