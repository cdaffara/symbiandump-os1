/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_CLIENT_PERIODIC_TESTER_H__
#define __CT_LBS_CLIENT_PERIODIC_TESTER_H__

//  INCLUDES
#include <e32base.h>
#include <e32std.h>

#include "lbs.h"

// CONSTANTS

#ifdef __WINS__
const TInt KToleratedEarliness  = 400000; //0.4 s
#endif

const TInt KTracePeriod = 10;

// FORWARD DECLARATIONS
class CT_LbsClientLog;

// CLASS DECLARATION

/**
*  Helper class for periodic update tests
*
*/
class CT_LbsClientPeriodicTester : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CT_LbsClientPeriodicTester* NewL(
		/* IN */	TTimeIntervalMicroSeconds aInterval, 
		/* IN */	const TDesC& aSericeName,
		/* IN */	CT_LbsClientLog* aLog,
		/* IN */	TUid aUid, 
		/* IN */	TInt aNumberOfRuns
		);
        
        /**
        * Destructor.
        */
        virtual ~CT_LbsClientPeriodicTester();

    public: // New functions
        
        /**
        * Requests notification from positioner and set the active flag true.
        */
		void RequestNotification();

		/**
		* Changes current test parameters. Can only be called when previous test is completed
		*/
		void ChangeTestParametersL(
		/* IN */	TTimeIntervalMicroSeconds aInterval, 
		/* IN */	TInt aNumberOfRuns
		);

		/**
		* Changes name in requestor information.
		*/
		void ChangeRequestorInformation(
		/* IN */	const TDesC& aServiceName
		);

		/**
		* Closes the positioner
		*/
		void ClosePositioner();

		/**
		* Opens the positioner
		*/
		void OpenPositionerL(
		/* IN */	const TDesC& aServiceName
		);

		/**
		* Sets the maxage parameter in position update options
		*/
		void SetMaxAgeL(
		/* IN */	TTimeIntervalMicroSeconds aInterval
		);
		
		/**
		* Sets the partial update parameter in position update options
		*/
		void SetPartialUpdateL(
		/* IN */	TBool aUpdateOption
		);
	
		/**
		* Sets the timeout parameter in position update options
		*/
		void SetTimeOutL(
		/* IN */	TTimeIntervalMicroSeconds aInterval
		);

		/**
		* Changes the update interval before the previous interval has completed one time
		*/
		void SetChangeParametersPrematurely(
		/* IN */	TBool aCondition
		);

        /**
		* A way to determine if any error has happened.
		*/
		TBool IsCompletedOK();

        /**
		* Get errorcode if error has occurred.
		*/
		TInt GetError();

		/**
		* Cancels periodic requests from EPos Server
		*/
		void CancelRequest();

        /**
		* Returns the position from the latest notift position request
		*/
		void GetPosition(TPositionInfo& aPosInfo);


    public: // Functions from CActive

        /**
        * From CActive Service completed request.
		* Defined as pure virtual by CActive;
		* implementation provided by this class,
        */
        void RunL();

        /**
        * From CActive Cancel request.
		* Defined as pure virtual by CActive;
		* implementation provided by this class.
        */
        TInt RunError(TInt aError);

		/**
        * From CActive Cancel request.
		* Defined as pure virtual by CActive;
		* implementation provided by this class.
        */
		void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
    	CT_LbsClientPeriodicTester();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(
		/* IN */	TTimeIntervalMicroSeconds aInterval, 
		/* IN */	const TDesC& aServiceName,
		/* IN */	CT_LbsClientLog* aLog,
		/* IN */	TUid aUid, 
		/* IN */	TInt aNumberOfRuns
		);

		/**
        * Evaluates condition, logs and leaves if false.
        */
		void AssertTrueL(
		/* IN */	TBool aCondition, 
		/* IN */	const TDesC& aErrorMsg,
		/* IN */	TInt aErrorCode
		);
 

    private:    // Data
        // EPos server instance
		RPositionServer	iPosServer;
        
		// Positioner instance
		RPositioner iPositioner;
		
		// Position information to be sent with NotifyPositionUpdate
		TPositionInfo iPositionInfo;
		
		// Number of periodic updates to request
		TInt iNumberOfUpdates;
		
		// Number of periodic updates received
		TInt iNumberOfRuns;
		
		// Tells if the test is going to end prematurely with a chage of update interval
		TBool iPrematureChangeOfParameters;

		// Tells if the outstanding request is going to be canceled
		TBool iCancelRequest;

        // Tells if the outstanding request has been canceled
        TBool iCanceledRequest; 
        
        // Tells if to write messages to log
        TBool iWriteToLog;
		
		// The time for when the most recent position update was received
		TTime iRecentTime;
		
		// The interval to request position updates between
		TTimeIntervalMicroSeconds iInterval;
		
		// Test log
		CT_LbsClientLog* iLog;
		
		// The name of the positioner
		TBuf<100> iName;
		
		// The uid of the positioner
		TUid iUid;

        //Error indication
        TBool iError;
        
        //Error indication
        TInt iErrorCode;

    };

#endif      // __CT_LBS_CLIENT_PERIODIC_TESTER_H__
            
// End of File
