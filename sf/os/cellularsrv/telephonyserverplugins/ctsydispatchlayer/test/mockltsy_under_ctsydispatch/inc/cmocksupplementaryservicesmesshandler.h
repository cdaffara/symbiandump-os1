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
 @internalAll 
*/

#ifndef CMOCKSUPPLEMENTARYSERVICESMESSHANDLER_H
#define CMOCKSUPPLEMENTARYSERVICESMESSHANDLER_H

#include "mmockmesshandlerbase.h"
#include "config.h"

class CCtsyDispatcherCallback;
class CMockLtsyEngine;

class CMockSupplementaryServicesMessHandler : public CBase, public MMockMessHandlerBase
    {
public: 
	static CMockSupplementaryServicesMessHandler* NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback);
	virtual ~CMockSupplementaryServicesMessHandler();

public: //from MMmMessHandlerBase
  	TInt ExtFuncL(TInt aInterfaceId,VA_LIST& aList);
  	
public: //from MMockMessHandlerBase
	  virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);
	  virtual TBool IsHandlerForApi(TInt aApiId);
     
private: 
 	CMockSupplementaryServicesMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback);
	void ConstructL();
        
private:
	CMockLtsyEngine& iMockLtsyEngine;
    CCtsyDispatcherCallback& iCompletionCallback;
    TConfig iConfig;
    }; // class CMockSupplementaryServicesMessHandler

#endif // CMOCKSUPPLEMENTARYSERVICESMESSHANDLER_H

