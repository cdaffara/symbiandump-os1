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

#ifndef __CTESTSERVICEPLUGIN_H__
#define __CTESTSERVICEPLUGIN_H__

#include <graphics/wsplugin.h>
#include "t_logfile.h"
  
class MWsGraphicDrawerEnvironment;

class CTestServicePlugin : public CWsPlugin
  {
public:
	enum { EImplUid = 0x10285D5d };

public:
	static CTestServicePlugin* CreateL();
	~CTestServicePlugin();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData);
	const TDesC& PluginName() const;
	void SetFadingParamsL(TUint8 aWhiteMap, TUint8 aBlackMap);
	TUint8 GetBlackMapL();
	TUint8 GetWhiteMapL();

public: // from MWsObjectProvider
	TAny* ResolveObjectInterface(TUint aTypeId);

private:
	CTestServicePlugin();

private:
	TUint8 iBlackMap;
 	TUint8 iWhiteMap;
  };

#endif //__CTESTSERVICEPLUGIN_H__
