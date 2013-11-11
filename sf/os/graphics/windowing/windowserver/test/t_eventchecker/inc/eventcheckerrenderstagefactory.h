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

#ifndef __EVENTCHECKERRENDERSTAGEFACTORY_H__
#define __EVENTCHECKERRENDERSTAGEFACTORY_H__

#include <graphics/wsplugin.h>
#include <graphics/wsrenderstagefactory.h>

/**
This is an implementation of MWsRenderStageFactory, in the form of a CWsPlugin.
The window server will use this to create the event checker render stage,
if the ini file specifies them.
*/
class CEventCheckerRenderStageFactory : public CWsPlugin, public MWsRenderStageFactory
	{
public:
	enum { EImplUid = 0x1028653d };
	
public:
	static CEventCheckerRenderStageFactory* CreateL();
	~CEventCheckerRenderStageFactory();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TDesC8& aData);
	const TDesC& PluginName() const;

public: // MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);

public: // MWsRenderStageFactory
	CWsRenderStage* CreateFinalRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, TInt aScreenNumber);
	CWsRenderStage* CreateRenderStageL(MWsScreen* aScreen, MWsScreenRedraw* aScreenRedraw, CWsRenderStage* aNextStage);

private:
	CEventCheckerRenderStageFactory();
	};

#endif // __EVENTCHECKERRENDERSTAGEFACTORY_H__
