/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSPSYMULTIREQUESTER_H
#define CPOSPSYMULTIREQUESTER_H

//  INCLUDES
#include <e32base.h>
#include "epos_mposrequestobserver.h"

// FORWARD DECLARATIONS
class TPositionInfoBase;
class TPositionInfo;
class CPositioner;
class CPosPSYRequester;

// CLASS DECLARATION
/**
*  Active object used to make position requests
*
*/
class CPosPSYMultiRequester : public CActive, public MPosRequestObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CPosPSYMultiRequester* NewL(
        /* IN */  MPosRequestObserver* aParent
        );

        /**
        * Destructor.
        */
        ~CPosPSYMultiRequester();

    public: // New functions
        
        /**
        * Creates positioner instance
        */
        void OpenPositionerL(
        /* IN */ const TUid aImplementationUid,
        /* IN */ const TInt aRequestCount = 1
        );

		/**
        * Closes all positioners and resets requester data.
        */
        void ClosePositioner();

        /**
        * Starts a position request
        */
        void MakeRequestsL();

        /**
        * GetResult. Gets the result of a location request
        * @param aStatus the complete status of the request
        * @param aRequestTime time  the request took
        * @param aInfo the position information from the psy
        * @param aRequesterIndex the positioner index to get the result from
        */
        void GetResult(
        /* OUT */ TInt& aStatus, 
        /* OUT */ TTimeIntervalMicroSeconds& aRequestTime,
        /* OUT */ TPositionInfo& aInfo,
        /* IN */  TInt aRequesterIndex = 0  
        );
        
        /**
        * SetRequesterWillCancelL. Sets if the requester should cancel or not
        * @param aIndex the requester to set cancel mode on
        * @param aBool the state the set cancel mode to
        */
        void SetRequesterWillCancelL(
        /* IN */ TInt aIndex = 0,
		/* IN */ TBool aCancel = ETrue
        );

        /**
        * Returns the number of requests that has been succesfully canceled
        * @return TInt the number of succesfully canceled requests
        */
        TInt GetNumberOfCanceledRequests();


    public: // Functions from base classes

		/**
		* From MPosRequestObserver.
        * HandlePositionCompleteL. Handels completed requests.
        */
        void HandlePositionCompleteL(
        /* IN */ TInt aCompleteCode,
        /* IN */ TTimeIntervalMicroSeconds aRequestTime
        );

		/**
        * Called when a request went wrong.
        * @param aErrorCode The error code for which type of error that occured.
        */
        void HandleRequestError( 
		/* IN */	TInt aErrorCode
		);

		void AddTestResultL(
        /* IN */   const TDesC& aMessage, 
        /* IN */   const TInt aResultType
        );


    public: // Functions from base classes

        /**
        * From CActive RunL.
        */
        void RunL();

        /**
        * From CActive DoCancel.
        */
        void DoCancel();

    private:
              
        /**
        * C++ default constructor.
        */
        CPosPSYMultiRequester();
        
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(
        /* IN */  MPosRequestObserver* aParent = NULL
        );

    private:

        RPointerArray<CPosPSYRequester>     iRequesters;             
        RPointerArray<TPositionInfo>        iResults;
        RArray<TInt>                        iRequestersWhoWillCancel;
        RTimer                              iTimer;
        TInt                                iNofRequestsDone;
        TInt                                iCancelWasCompleted;
		MPosRequestObserver*				iParent;

    };

#endif      // CPOSPSYMULTIREQUESTER_H   
            
// End of File
