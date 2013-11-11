/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_CLIENT_EVENT_TIMER_H__ 
#define __CT_LBS_CLIENT_EVENT_TIMER_H__

//  INCLUDES
#include <e32base.h>
#include <test/testexecutelogger.h>

// CONSTANTS

// FORWARD DECLARATIONS
class MCT_LbsRequestObserver;

// CLASS DECLARATION

/**
*  This class is used to time the notification dialog
*
*/
class CT_LbsClientEventTimer : public CActive
    {
    private:  // Constructors

        /**
        * C++ default constructor.
        */
        CT_LbsClientEventTimer();

        /**
        * By default EPOC constructor is private.
        */
		void ConstructL(CTestExecuteLogger& aLogger);

        // By default, prohibit copy constructor
        CT_LbsClientEventTimer( const CT_LbsClientEventTimer& );
        // Prohibit assigment operator
        CT_LbsClientEventTimer& operator= ( const CT_LbsClientEventTimer& );
        // Logger interface used for constructing result strings to be fed for the logger instance
        inline CTestExecuteLogger& Logger() {return iLogger;};

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
		static CT_LbsClientEventTimer* NewL(CTestExecuteLogger& aLogger);

        /**
        * Two-phased constructor.
        */
		static CT_LbsClientEventTimer* NewLC(CTestExecuteLogger& aLogger);

        /**
        * Destructor.
        */
        ~CT_LbsClientEventTimer();

	public: // New functions

		/**
        * Start the timer
        */
		void Start(TInt aTimeout);

		TInt CompletionCode() const;

		void ResetCompletionCode();

        /**
        * Set who is observing timeouts
        * @param aObserver the observing object
        */
		void SetObserver(
        /* IN  */       MCT_LbsRequestObserver* aObserver
        );

    protected:  // Functions from base classes

        /**
        * From CActive
        */
		void RunL();

        /**
        * From CActive
        */
		TInt RunError(TInt aError);

		/**
        * From CActive
        */
		void DoCancel();

    protected: // New functions

        void RequestComplete();

	private: // Data
		RTimer				iTimer;
		MCT_LbsRequestObserver*	iObserver;
		TInt                iCompletionCode;
        
        CTestExecuteLogger	iLogger;
    };

#endif      // __CT_LBS_CLIENT_EVENT_TIMER_H__   
            
// End of File
