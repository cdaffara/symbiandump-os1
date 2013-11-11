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

#ifndef __SUSUTILSERVER_H__
#define __SUSUTILSERVER_H__

#include <e32base.h>
#include <ssm/ssmsuscli.h>

class CSusPluginFrame;

/**
 @internalComponent
 @released
 */
class CSusUtilServer : public CPolicyServer
	{	
public:
	static CSusUtilServer* NewLC();
	static CSusUtilServer* NewLC(const TDesC& aServerName, TSecureId aClientSecureId);
	~CSusUtilServer();
	
	TSecureId ClientSecureId() const;
	
	void LoadUtilityPluginL(TSsmSupInfo& aSupInfo);
	TInt UnLoadUtilityPluginL(TSsmSupInfo& aSupInfo);

#ifdef _DEBUG
	void CompressPluginArray();
#endif
private:
	CSusUtilServer(TSecureId aClientSecureId);
	void ConstructL(const TDesC& aServerName);
	TInt IndexOf(const RLibrary& aLibrary, TInt aNewLOrdinal) const;
	
	//from CServer2
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

private:
	TSecureId iClientSecureId; //the SID of the only allowed client
	RPointerArray<CSusPluginFrame> iLoadedPlugins;
	};

#endif
