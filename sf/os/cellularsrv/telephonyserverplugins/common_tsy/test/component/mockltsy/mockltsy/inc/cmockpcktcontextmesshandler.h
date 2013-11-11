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

#ifndef CMockPcktContextMessHandler_H
#define CMockPcktContextMessHandler_H

#include <etelpckt.h>
#include "cmockpcktqosmesshandler.h"
#include "mmockmesshandlerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

class CMmMessageRouter;

class CMockPcktContextMessHandler : public CBase, public MMockMessHandlerBase
    {
    public: 
        static CMockPcktContextMessHandler* NewL(CMmMessageRouter* aMessageRouter);
        virtual ~CMockPcktContextMessHandler();

    public: 
        virtual TInt ExtFuncL(TInt aIpc, const CMmDataPackage* aDataPackage);
  		virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);

    private:
        CMockPcktContextMessHandler(CMmMessageRouter* aMessageRouter);
        void ConstructL();

    private:
		static void CleanupArrayPtrFlat(TAny* self);

    private: // Data
        /*
         * Pointer to the message router
         * Own.
         */                  
        CMmMessageRouter* iMessageRouter;
        RPointerArray<RPacketMbmsContext::CMbmsSession> iMbmsSessionLists;
};

#endif // CMockPcktContextMessHandler_H

