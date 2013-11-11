/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef __CT_LBS_CLIENT_POS_HOLDER_H__
#define __CT_LBS_CLIENT_POS_HOLDER_H__

//  INCLUDES
#include <e32base.h>
#include "ctlbsportedstepbase.h"

// CONSTANTS

// FUNCTION TYPES

// FORWARD DECLARATIONS
class CPositionInfo;
class CPositioner;
class CT_LbsPosClient;

// CLASS DECLARATION

/**
*  Active object used to make position requests
*
*/
class CT_LbsPosClientHolder : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        static CT_LbsPosClientHolder* NewLC(
            TInt aRequestCount = 1, TUid aPsy = TUid::Uid(KUidDynDbTestPsy), TBool aSameServer = EFalse);

        /**
        * Destructor.
        */
        ~CT_LbsPosClientHolder();

    private:
        
        void ConstructL(TInt aRequestCount, TUid aPsy, TBool aSameServer);

        /**
        * C++ default constructor.
        */
        CT_LbsPosClientHolder();

    public: // Functions from base classes
        
        /**
        * Starts a position request
        */
        void MakeRequests();

        void StopRequests();

		void SetCancelTime(TTimeIntervalMicroSeconds32 aTimeInterval);

        /**
        * Get result
        */
        void GetResult(
            TInt& aStatus,  
            TPositionInfo& aModuleInfo,
            TTimeIntervalMicroSeconds& aRequestTime,
            TInt aRequesterIndex = 0
            );

        void HandleRequestDone();

        void SetRequesterWillCancelL(
            TInt    aIndex = 0
            );

    protected:

        void RunL();

        void DoCancel();

    private:

        RPointerArray<CT_LbsPosClient>       iRequesters;
        RPositionServer				    iPosServer;
        TInt                            iNofRequestsDone;
        RTimer                          iTimer;
        RArray<TInt>                    iRequestersWhoWillCancel;
        TBool                           iRequestsActive;
		TTimeIntervalMicroSeconds32     iCancelTime;
    };

#endif      // __CT_LBS_CLIENT_POS_HOLDER_H__
            
// End of File
