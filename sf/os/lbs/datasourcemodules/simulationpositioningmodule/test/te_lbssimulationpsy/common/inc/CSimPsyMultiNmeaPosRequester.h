/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CSIMPSYMULTINMEAPOSREQUESTER_H
#define CSIMPSYMULTINMEAPOSREQUESTER_H

//  INCLUDES
#include <e32base.h>
#include <lbspositioninfo.h>

// CONSTANTS

// FUNCTION TYPES

// FORWARD DECLARATIONS
class TPositionInfo;
class CSimPsyNmeaPosRequester;

// CLASS DECLARATION

/**
*  Active object used to make position requests
*
*/
class CSimPsyMultiNmeaPosRequester : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        static CSimPsyMultiNmeaPosRequester* NewLC(            
            TInt aRequestCount = 1
            );

        /**
        * Destructor.
        */
        ~CSimPsyMultiNmeaPosRequester();

    private:
        
        void ConstructL(TInt aRequestCount);

        /**
        * C++ default constructor.
        */
        CSimPsyMultiNmeaPosRequester();

    public: // Functions from base classes
        
        /**
        * Starts a position request
        */
        void MakeRequests(TPositionInfo* aPositionInfo);

        void StopRequests();

        /**
        * Get result
        */
        void GetResult(
            TInt& aStatus,             
            TTimeIntervalMicroSeconds& aRequestTime,
            TInt aRequesterIndex = 0
            );

        void HandleRequestDone();

        void SetPartialUpdateRestriction(
            TBool aPartialUpdateRestriction);

        void SetPartialUpdateRestriction(
            TBool aPartialUpdateRestriction,
            TInt aRequesterIndex);
        
        void SetRequesterWillCancelL(
            TInt    aIndex = 0,
            TTimeIntervalMicroSeconds32 aMicroSeconds = 10
            );

    protected:

        void RunL();

        void DoCancel();

    private:

        RPointerArray<CSimPsyNmeaPosRequester> iRequesters;
        TInt                                iNofRequestsDone;
        RTimer                              iTimer;
        RArray<TInt>                        iRequestersWhoWillCancel;
        TTimeIntervalMicroSeconds32         iMicroSecondsToCancel;
        TBool                               iRequestsActive;
    };

#endif      // CSIMPSYMULTINMEAREQUESTER_H   
            
// End of File
