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



#ifndef CSIMPSYMULTIREQUESTER_H
#define CSIMPSYMULTIREQUESTER_H

//  INCLUDES
#include <e32base.h>
// CONSTANTS

// FUNCTION TYPES

// FORWARD DECLARATIONS
class TPositionInfoBase;
class TPositionModuleStatus;
class TPositionInfo;
class CPositioner;
class CSimPsyRequester;

// CLASS DECLARATION

/**
*  Active object used to make position requests
*
*/
class CSimPsyMultiRequester : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        static CSimPsyMultiRequester* NewLC(
            TInt aRequestCount = 1
            );

        /**
        * Destructor.
        */
        ~CSimPsyMultiRequester();

    private:
        
        void ConstructL(TInt aRequestCount);

        /**
        * C++ default constructor.
        */
        CSimPsyMultiRequester();

    public: // Functions from base classes
        
        /**
        * Starts a position request
        */
        void MakeRequests();

        void StopRequests();

        void ClosePositioner();

        /**
        * Get result
        */
        void GetResult(
            TInt& aStatus, 
            TPositionInfo& aPosition, 
            TTimeIntervalMicroSeconds& aRequestTime,
            TInt aRequesterIndex = 0
            );

        /**
        * Get module status 
        */
        TBool GetModuleStatus(TInt aRequesterIndex = 0);

        /**
        * @return Number of reported statuses 
        */
        TInt ReportedStatuses(TInt aRequesterIndex = 0);

        /**
        * Resets number of reported statuses 
        */    
        void ResetReportedStatuses(TInt aRequesterIndex = 0);

		/**
        * Module status
        */
        TPositionModuleStatus ModuleStatus(TInt aRequesterIndex = 0);

        void HandleRequestDone();

        void SetRequesterWillCancelL(
            TInt    aIndex = 0,
			TBool   aBool = ETrue
            );

    protected:

        void RunL();

        void DoCancel();

    private:

        RPointerArray<CSimPsyRequester>     iRequesters;                
        TInt                                iNofRequestsDone;
        RTimer                              iTimer;
        RArray<TInt>                        iRequestersWhoWillCancel;
        TBool                               iRequestsActive;
    };

#endif      // CSIMPSYMULTIREQUESTER_H   
            
// End of File
