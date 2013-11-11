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

#ifndef CMockPcktServiceMessHandler_H
#define CMockPcktServiceMessHandler_H

#include "cmockpcktcontextmesshandler.h"
#include "cmockphonemesshandler.h"
#include "mmockmesshandlerbase.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

class CMmMessageRouter;
class CMockPcktContextMessHandler;
class CMockPhoneMessHandler;

class CMockPcktServiceMessHandler : public CBase, public MMockMessHandlerBase 
    {
    public:
        static CMockPcktServiceMessHandler* NewL(CMmMessageRouter* aMessageManager);
        virtual ~CMockPcktServiceMessHandler();

    public:
        virtual TInt ExtFuncL( TInt aIpc,const CMmDataPackage* aDataPackage );
  		virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);

    private: 
        CMockPcktServiceMessHandler(CMmMessageRouter* aMessageManager);
        void ConstructL();

    private: 
        CMmMessageRouter* iMessageRouter;
        RPointerArray<CPcktMbmsMonitoredServiceList> iPcktMbmsMonitoredServiceLists;
    };

#endif // CMockPcktServiceMessHandler_H

