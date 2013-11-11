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

#ifndef CMockPcktQoSMessHandler_H
#define CMockPcktQoSMessHandler_H

#include "cmmmessagerouter.h"
#include "cmockpcktcontextmesshandler.h"
#include "mmockmesshandlerbase.h"

class CMmMessageRouter;
class CMockPcktContextMessHandler;

class CMockPcktQoSMessHandler : public CBase, public MMockMessHandlerBase 
    {
    public:
        static CMockPcktQoSMessHandler* NewL(CMmMessageRouter* aMessageRouter);
        virtual ~CMockPcktQoSMessHandler();

    public:
        virtual TInt ExtFuncL( TInt aIpc, const CMmDataPackage* );
  		virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);
  
    private:
        CMockPcktQoSMessHandler(CMmMessageRouter* aMessageRouter);
        void ConstructL();

    private: 
        CMmMessageRouter* iMessageRouter;
    };

#endif // CMockPcktQoSMessHandler_H

