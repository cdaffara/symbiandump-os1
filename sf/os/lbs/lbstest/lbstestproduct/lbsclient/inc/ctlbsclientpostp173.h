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



#ifndef __CT_LBS_CLIENT_POS_TP173_H__
#define __CT_LBS_CLIENT_POS_TP173_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"
#include "epos_mposmodulesobserver.h" 

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 173
*
*/
class CT_LbsClientPosTp173 : public CT_LbsPortedStepBase, public MPosModulesObserver
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
    	CT_LbsClientPosTp173(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientPosTp173();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

    public: // New Functions
    
        /**
		* Timeout request if the no notification has been sent
		* by the server.
		*/
		void NotifyTimeout();

        void HandleSettingsChangeL(TPosModulesEvent);

        /**
        * Leaves if aCondition is false.
        * @param aCondition something that should be true
        */
        void AssertTrueL(TBool aCondition, const TDesC& errorMsg);

		/**
        * Leaves if aCondition is false.
        * @param aCondition something that should be true
        */
		void AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);

        void CheckErrorL();

        void DoEventHandlingL();
        
     protected:

        TBool iNoEvents;
        TBool iError;
        TBuf<200> iErrorText;
     
     private: 

        TInt iEvent;   
        
    };

//// CLASS DECLARATION

/**
 *
 */
class CMyNotificationTimer : public CTimer 
	{
	public:
	
		/**
        * Destructor.
        */
		virtual ~CMyNotificationTimer();

	public:

		/**
        * Two-phased constructor.
        */
		static CMyNotificationTimer* NewLC(CT_LbsClientPosTp173* aMainControl);

		/**
        * Two-phased constructor.
        */
		static CMyNotificationTimer* NewL(CT_LbsClientPosTp173* aMainControl);

	public:

		/**
		* Starts the power checking timer
		*/
		void StartNotificationTimer(); 

		/** 
		* Cancel the power checking timer.
		*/
		void DoCancel();

	private:

		/**
        * EPOC constructor.
        */
		void ConstructL();

		/**
        * C++ default constructor.
        */
		CMyNotificationTimer(CT_LbsClientPosTp173* aMainControl);

        /**
        * From CActive. Handles an active object’s request completion
        * event.
        */
        void RunL();

		//Prohibit copy constructor
        CMyNotificationTimer( const CMyNotificationTimer& );
        // Prohibit assigment operator
        CMyNotificationTimer& operator= ( const CMyNotificationTimer& );

	private:
		// a handler to the communicator
		CT_LbsClientPosTp173*	iMainControl;
		TInt		iNotificationTimout;
	};

#endif      // __CT_LBS_CLIENT_POS_TP173_H__

// End of File
