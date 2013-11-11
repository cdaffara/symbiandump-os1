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
 @internalComponent
*/

#ifndef __TESTRENDERSTAGE_H__
#define __TESTRENDERSTAGE_H__

#include <graphics/wsrenderstage.h>
#include <graphics/wsgraphicdrawer.h>
#include "t_testserviceplugin.h"

class MWsObjectProvider;
class MWsGraphicDrawerEnvironment;
class MWsFrontBuffer;
class CFbsBitGc;

/**
This is the implementation of CWsRenderStage which is created by a CTestRenderStageFactory
*/
class CTestRenderStage : public CWsRenderStage , public MWsEventHandler
	{
public:
	static CTestRenderStage* NewL(MWsGraphicDrawerEnvironment* aEnv, MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw);
	~CTestRenderStage();

public: // implementation of MWsRenderStage
	CFbsBitGc* Begin();
	void End();	
	void DoHandleEvent(const TWservCrEvent& aEvent);

private:
	CTestRenderStage();
	void ConstructL(MWsGraphicDrawerEnvironment* aEnv,MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw);
	void OnWindowCloseL(MWsWindow* aWindow);
private:
	MWsGraphicDrawerEnvironment* iEnv;
	MWsScreen* iScreen;
	MWsBackBuffer* iBackBuffer;
	MWsScreenRedraw* iScreenRedraw;
	MWsScreenConfig* iScreenConfig;
	};

#endif //__TESTRENDERSTAGE_H__
