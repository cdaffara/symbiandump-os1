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

#ifndef CMMCALLMESSAGEHANDLER_H
#define CMMCALLMESSAGEHANDLER_H

#include <e32base.h>
#include <etelmm.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "mmockmesshandlerbase.h"

class CMmMessageRouter;

class CMockCallMessHandler : public CBase, public MMockMessHandlerBase
    {
public:
    static CMockCallMessHandler* NewL(CMmMessageRouter* aMessageManager);
    virtual ~CMockCallMessHandler();

public: 
    virtual TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );
	virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);

public: // logging utils
    virtual void LogErrorData(TInt aIpc, const MMockSyEngineData& aData, const TDesC8& aExpectedData);
private:
	void LogEtelCallDialErrorData(const MMockSyEngineData& aData, const TDesC8& aExpectedData);

private:
    CMockCallMessHandler(CMmMessageRouter* aMessageManager);
    void ConstructL();

private: 
    CMmMessageRouter* iMessageRouter;
    };

#endif      // CMockCallMessHandler_H   
            
