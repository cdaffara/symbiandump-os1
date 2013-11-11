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

#ifndef CMOCKCALLCONTROLMULTIPARTYMESSHANDLER_H
#define CMOCKCALLCONTROLMULTIPARTYMESSHANDLER_H

#include "mmockmesshandlerbase.h"
#include "config.h"

class CCtsyDispatcherCallback;
class CMockLtsyEngine;

class CMockCallControlMultipartyMessHandler : public CBase, public MMockMessHandlerBase
    {
public: 
	static CMockCallControlMultipartyMessHandler* NewL(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback);
	virtual ~CMockCallControlMultipartyMessHandler();

public: //from MMmMessHandlerBase
  	TInt ExtFuncL(TInt aInterfaceId,VA_LIST& aList);
  	
public: //from MMockMessHandlerBase
	  virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);
	  virtual TBool IsHandlerForApi(TInt aApiId);
     
private: 
 	CMockCallControlMultipartyMessHandler(CMockLtsyEngine& aEngine, CCtsyDispatcherCallback& aCallback);
	void ConstructL();
        
private:
	CMockLtsyEngine& iMockLtsyEngine;
    CCtsyDispatcherCallback& iCompletionCallback;
    TConfig iConfig;
    }; // class CMockCallControlMultipartyMessHandler

#endif // CMOCKCALLCONTROLMULTIPARTYMESSHANDLER_H

