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



#ifndef TAUTOMATISATION_H
#define TAUTOMATISATION_H

//  INCLUDES
#include <e32base.h>
#include <eikenv.h>
#include <EPos_CPosPrivacyNotifier.h>
#include "TPrivacyDialogCtrl.h"
#include "Epos_PrivacyQN.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Handles automating verification and notfication dialog. 
* Automating means that no dialog is launched. 
* Instead the response is read from a test file.
*/
class TAutomatisation
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        TAutomatisation();

    public: // New functions
        
        TBool Enabled();

        /**
        * Starts automated manual input.
        */
        void StartL( 
        /* IN  */       TInt aEntryCount,
        /* IN  */       TInt aType,
        /* IN  */       TInt aServiceIdFormat,
        /* IN  */       TInt aContactIdFormat,
        /* IN  */       CPosPrivacyNotifier::TRequestType aNotOrVer,
        /* IN  */       const TDesC& aServiceStringId,
        /* IN  */       const TDesC& aContactStringId,
        /* IN  */       TBool aServiceMultipleMarches,
        /* IN  */       TBool aContactMultipleMarches,
        /* IN  */       TBool aServiceResolved,
        /* IN  */       TBool aContactResolved,
        /* IN  */       TPosQNRequestId aRequestId, 
        /* IN/OUT */    CPosPrivacyNotifier* aPrivPlugin);

        /**
        * Cancels automated manual input.
        */
        void Cancel();

    private:

        void Complete(
        /* IN  */       TInt aCompletionCode
        );

        void CompleteAllRequests(TInt aCompletionCode);

    private:    // Data
        TBool iActive;
        
//        RMessage iMessage;
        CEikonEnv* iEnv;

        CPosPrivacyNotifier* iPrivPlugin;
        TPosQNRequestId iRequestId;
        TPrivacyDialogCtrl iPrivCtrl;
        TPrivDlgDiagnostics iDiagnostics;
        TPrivDlgControlData iPrivCtrlData;
        CPosPrivacyNotifier::TRequestType iNotOrVer;
    };

#endif      // TAUTOMATISATION_H  
            
// End of File
