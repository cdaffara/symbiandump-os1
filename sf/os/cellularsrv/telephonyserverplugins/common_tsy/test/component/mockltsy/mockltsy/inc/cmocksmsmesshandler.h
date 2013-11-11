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

#ifndef CMockSmsMessHandler_H
#define CMockSmsMessHandler_H

#include <etelmm.h>
#include <etelmmcs.h>
#include "cmmmessagerouter.h"
#include "mmockmesshandlerbase.h"

class CMmMessageRouter;

typedef CArrayPtrFlat<TWcdmaCbsMsg> WcdmaCbsMsgArray;
typedef CArrayPtrFlat<TSmsParameters> SmsParametersArray;
typedef CArrayPtrFlat<TSmsMsg> SmsMsgArray;

class CMockSmsMessHandler : public CBase, public MMockMessHandlerBase
    { 
    public: 
        static CMockSmsMessHandler* NewL(CMmMessageRouter* aMessageRouter);
        virtual ~CMockSmsMessHandler(); 

     public: 
       virtual TInt ExtFuncL(TInt aIpc, const CMmDataPackage* aDataPackage); 
  		virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);

    private:
        CMockSmsMessHandler(CMmMessageRouter* aMessageRouter);
        void ConstructL(); 

    private:
        CMmMessageRouter* iMessageRouter;
        RPointerArray<WcdmaCbsMsgArray> iWcdmaBroadcastMessages;
        RPointerArray<SmsParametersArray> iSmsParametersList;
        RPointerArray<SmsMsgArray> iSmsMsgList;
        RPointerArray<CMobilePhoneBroadcastIdList> iBroadcastIdLists;
    };

#endif // CMockSmsMessHandler_H

// End of File
