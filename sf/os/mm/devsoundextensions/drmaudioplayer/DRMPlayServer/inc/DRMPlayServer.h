/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   DRM Play Server
*
*/


#ifndef __CDRMPLAYSERVER_H
#define __CDRMPLAYSERVER_H

#include <e32base.h>
#include "DRMPlayClientServer.h"
#include "DRMPlaySession.h"


// reasons for server panic
enum TDRMPlayServPanic
	{
	EBadRequest,
	EBadDescriptor,
	EMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ECreateTrapCleanup,
	ENotImplementedYet
	};


class CDRMPlayServer : public CServer2
	{
public:
	enum {EPriority=950}; 
public:
	static CDRMPlayServer* New();
	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
    ~CDRMPlayServer();
    TUint	iClients;
protected:
	CDRMPlayServer(TInt aPriority);
private:
	TInt				iActive;
	};
	

GLREF_C void PanicServer(TDRMPlayServPanic aPanic);
	
#endif
	
	
	

