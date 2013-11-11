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

#ifndef __DISPLAYRENDERSTAGEFACTORY_H__
#define __DISPLAYRENDERSTAGEFACTORY_H__

#include <graphics/wsplugin.h>
#include <graphics/wsrenderstagefactory.h>

class CWsRenderStage;
class MWsGraphicDrawerEnvironment;

/**
This is an implementation of MWsRenderStageFactory, in the form of a CWsPlugin.
The window server will use this to create standard render stages for each screen,
if the ini file specifies them.
*/
class CDisplayRenderStageFactory : public CWsPlugin, public MWsRenderStageFactory
	{
public:
	enum { EImplUid = 0x1028637A };
	
public:
	static CDisplayRenderStageFactory* CreateL();
	~CDisplayRenderStageFactory();

public: //from CWsPlugin
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TDesC8& aData);
	const TDesC& PluginName() const;

public: //from MWsRenderStageFactory
	TAny* ResolveObjectInterface(TUint aTypeId);
	CWsRenderStage* CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, TInt aScreenNumber);
	CWsRenderStage* CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);

private:
	CDisplayRenderStageFactory();
	};

#endif //__DISPLAYRENDERSTAGEFACTORY_H__
