/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CT_LBS_CLIENT_STEP_OBSERVE_INSTALL_H__
#define __CT_LBS_CLIENT_STEP_OBSERVE_INSTALL_H__

//  INCLUDES
#include "ctlbsportedstepbase.h"
#include "epos_mposmodulesobserver.h" 

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Test procedure 300
*
*/
class CT_LbsClientStepObserveInstall : public CT_LbsPortedStepBase, public MPosModulesObserver
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
    	CT_LbsClientStepObserveInstall(CT_LbsServer& aParent);

        /**
        * Destructor.
        */
        ~CT_LbsClientStepObserveInstall();

    public: // Functions from base classes

        /**
        * From CT_LbsPortedStepBase
        * Runs the test in the module
        */
        void StartL();

    public: // New Functions

        void HandleSettingsChangeL(TPosModulesEvent);

        void CheckErrorL();

        void DoEventHandlingL();
        
        TInt SISUninstallPsyL(const TUid& aAppId);
        
        TInt SISInstallPsyL(const TDesC& aSisPath);
        
     protected:

        TBool iError;
        TBuf<200> iErrorText;
        TInt iExpectedEvent; 
        
     private:   
    };


#endif      // __CT_LBS_CLIENT_STEP_OBSERVE_INSTALL_H__

// End of File
