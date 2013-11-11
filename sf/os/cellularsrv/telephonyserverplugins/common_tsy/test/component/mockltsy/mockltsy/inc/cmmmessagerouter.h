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
//

#ifndef CMMMESSAGEROUTER_H
#define CMMMESSAGEROUTER_H

#include "mmockmesshandlerbase.h"
#include <ctsy/pluginapi/mmessagerouter.h>

class CMockLtsyEngine;
class CMmPhoNetSender;
class CMmPhoNetReceiver;
class CMockCallMessHandler;
class CMmCustomMessHandlerGsmExt;
class CMmMessageManager;
class CMockPcktContextMessHandler;
class CMockPcktQoSMessHandler;
class CMockPcktServiceMessHandler;
class CMockPhoneBookStoreMessHandler;
class CMockPhoneMessHandler;
class CMockSmsMessHandler;

class MmMessageManagerCallback;

class CMmMessageRouter : public CBase, public MMessageRouter
    {        
public:    
    static CMmMessageRouter* NewL(MmMessageManagerCallback& aMessageManagerCallback);
    virtual ~CMmMessageRouter(void);
    
public:    

	inline MmMessageManagerCallback* MessageManagerCallback();
	inline CMockLtsyEngine* MockLtsyEngine();
	
	virtual MMockMessHandlerBase* RouteRequest(TInt aIpc); 
	
public: // From MMessageRouter

	virtual TInt ExtFuncL(TInt aIpc, const CMmDataPackage* aDataPackage);

private:

    CMmMessageRouter(MmMessageManagerCallback& aMessageManagerCallback);
    void ConstructL();
     
    
private:
    void InitL();
    void InitModulesL();

private:    // Owned
    CMockPhoneMessHandler* iPhoneMessHandler;
    CMockCallMessHandler* iCallMessHandler;
    CMockPhoneBookStoreMessHandler* iPhoneBookStoreMessHandler;
    CMockPcktServiceMessHandler* iPcktServiceMessHandler;
    CMockPcktContextMessHandler* iPcktContextMessHandler;
    CMockPcktQoSMessHandler* iPcktQoSMessHandler;
    CMockSmsMessHandler* iSmsMessHandler;

    CMmCustomMessHandlerGsmExt* iMmCustomMessHandlerGsmExt;

	CMockLtsyEngine* iMockEngine;
	CServer2* iMockServer;

 private:    // Not owned
   
 	MmMessageManagerCallback& iMessageManagerCallback;
    };


inline MmMessageManagerCallback* CMmMessageRouter::MessageManagerCallback()
	{
	return &iMessageManagerCallback;
	}

inline CMockLtsyEngine* CMmMessageRouter::MockLtsyEngine()
	{
	return iMockEngine;
	}

#endif // CMMMESSAGEROUTER_H

