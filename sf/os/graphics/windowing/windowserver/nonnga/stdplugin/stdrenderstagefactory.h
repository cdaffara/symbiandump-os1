// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// STDRENDERSTAGE.CPP
// 
//

#ifndef __STDRENDERSTAGEFACTORY_H__
#define __STDRENDERSTAGEFACTORY_H__

#include "Graphics/WSPLUGIN.H"
#include "Graphics/WsRenderStageFactory.h"

class CWsRenderStage;
class MWsGraphicDrawerEnvironment;

/**
This is an implementation of MWsRenderStageFactory, in the form of a CWsPlugin.
The window server will use this to create standard render stages for each screen,
if the ini file specifies them.
*/
class CStdRenderStageFactory : public CWsPlugin, public MWsRenderStageFactory
	{
public:
	enum { EImplUid = 0x2001B70A };
	
public:
	static CStdRenderStageFactory* CreateL();
	~CStdRenderStageFactory();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
	virtual const TDesC& PluginName() const;

public: // implementation of MWsObjectProvider (but which one? we inherit from 2!)
	TAny* ResolveObjectInterface(TUint aTypeId);

public: // implementation of MWsRenderStageFactory
	virtual CWsRenderStage * CreateStageL(MWsScreen * aScreen, MWsScreenRedraw * aScreenRedraw);

private:
	CStdRenderStageFactory();
	};

#endif //__STDRENDERSTAGEFACTORY_H__
