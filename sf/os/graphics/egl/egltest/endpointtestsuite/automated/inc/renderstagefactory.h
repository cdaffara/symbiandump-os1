// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __RENDERSTAGEFACTORY_H__
#define __RENDERSTAGEFACTORY_H__


#include <graphics/wsplugin.h>
#include <graphics/wsrenderstagefactory.h>


//This provides the wserv interface for creating instances of CRenderStage
class CRenderStageFactory : public CWsPlugin, public MWsRenderStageFactory
	{
public:
	enum { EImplUid = 0xA000E082 };

public:
	static CRenderStageFactory* CreateL();
	~CRenderStageFactory();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TDesC8& aData);
	const TDesC& PluginName() const;

public: // MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);

public: // MWsRenderStageFactory
	CWsRenderStage* CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, TInt aScreenNumber);
	CWsRenderStage* CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);

private:
    CRenderStageFactory();
	};


#endif
