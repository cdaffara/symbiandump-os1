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

#ifndef _LCF_CLcfPrivacyDialogPlugin_H
#define _LCF_CLcfPrivacyDialogPlugin_H

//  INCLUDES
#include <e32base.h>
#include <techview/eikdialg.h>
#include <EPos_CPosPrivacyNotifier.h>

#include "LcfFIPluginController.h"


// CONSTANTS

// FORWARD DECLARATIONS
class CPeriodic;
class CEikInfoDialog;


// CLASS DECLARATION

/**
*  This class controls the notification dialog
*
*/
class CLcfPrivacyDialogPlugin : public CPosPrivacyNotifier
    {
	private: // Constructors

		/**
    * private constructor. May not leave
    */
    /*IMPORT_C*/ CLcfPrivacyDialogPlugin();

		/**
    * private constructor. May leave
    */
		void ConstructL();

    public:  // Constructors and destructor
        
    /**
    * Two-phased constructor.
    */
		/*IMPORT_C*/ static CLcfPrivacyDialogPlugin* NewL();

    /**
    * Destructor.
    */
    ~CLcfPrivacyDialogPlugin();

    public: // Functions from base classes

        /** 
        * From EPos_CPosPrivacyNotifier.h
        * This method is called when a new privacy query or notification has
        * been requested.
        *
        * @param aRequestId The id of the new query or notification request.
        */
        void HandleNewRequestL(/*IN*/ TPosQNRequestId aRequestId);


        /**
        * From EPos_CPosPrivacyNotifier.h
        * This method is called when a privacy query or notification request is
        * cancelled.
        *
        * It is not necessary to call @ref CompleteRequest() for a cancelled
        * request.
        *
        * @param aRequestId The id of the cancelled query or notification
        *   request.
        */
        void HandleRequestCancelled(/*IN*/ TPosQNRequestId aRequestId);

        /**
        * From EPos_CPosPrivacyNotifier.h
        * This method is called to cancel all outstanding requests.
        *
        * It is not necessary to call @ref CompleteRequest() for a cancelled
        * request.
        */
        void HandleAllRequestCancelled();

    protected: // Functions from base classes


	private:

        // By default, prohibit copy constructor
        CLcfPrivacyDialogPlugin( const CLcfPrivacyDialogPlugin& );
        // Prohibit assigment operator
        CLcfPrivacyDialogPlugin& operator= ( const CLcfPrivacyDialogPlugin& );

        void Dismiss();

        void HandleNextRequestL(/*IN*/ TPosQNRequestId aRequestId);

        void HandleNextRequestAutomatisedL(/*IN*/ TPosQNRequestId aRequestId);

    private: // new functions by Lcf
        static TInt TimerCallBack(TAny* aPtr);
        void DismissDialog();

	private: // Data

		TBool               iDialogLaunched;
        TBool               iDialogComplete;

        CLcfNotifierManager*   iControlFilesManager;

        CLcfPluginDiagnostics* iDiagnostics;
        TLcfPluginAnswer       iAnswer;


        CPeriodic*           iTimer;
        TInt                 iVerificationCount;
        TInt                 iNotificationCount;

    private: // for new functionality

        TLcfCompletionStyle iStyleToComplete;
        TInt                iRequestIdToComplete;
        TInt                iCompletionCode;
        CEikInfoDialog*     iInfoDialog;
    };

#endif      
            
// End of File
