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
// TESTRENDERSTAGEFACTORY.H
// 
//

/**
 @file
 @test
 @internalComponent
*/

#ifndef __TESTRENDERSTAGEFACTORY_H__
#define __TESTRENDERSTAGEFACTORY_H__

#include "graphics/wsplugin.h"
#include "graphics/wsrenderstagefactory.h"
class CWsRenderStage;
class MWsGraphicDrawerEnvironment;

/**
This is an implementation of MWsRenderStageFactory, in the form of a CWsPlugin.
The window server will use this to create test render stages for each screen,
if the ini file specifies them.
*/
class CTestRenderStageFactory : public CWsPlugin, public MWsRenderStageFactory
	{
public:
	enum { EImplUid = 0x10285C5E };
	
public:
	static CTestRenderStageFactory* CreateL();
	~CTestRenderStageFactory();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
	const TDesC& PluginName() const;

public: // implementation of MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);

public: // implementation of MWsRenderStageFactory
	CWsRenderStage* CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, TInt aScreenNumber);
	CWsRenderStage* CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw*aScreenRedraw, CWsRenderStage* aNextStage);
	

protected:
	CTestRenderStageFactory();
	};
	
#endif //__TESTRENDERSTAGEFACTORY_H__
