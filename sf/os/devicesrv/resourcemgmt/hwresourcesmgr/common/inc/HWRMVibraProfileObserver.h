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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMVIBRAPROFILEOBSERVER_H
#define HWRMVIBRAPROFILEOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include "cenrepobserver.h"
#include "HWRMVibra.h"


/**
* Callback interface for vibra status change passing.
*/
class MHWRMVibraProfileObserver
    {
    public:
    
        /**
        * Notifies vibra mode setting changes in profile
        *
        * @param aNewState the new state of setting.
        */
        virtual void VibraModeChange(CHWRMVibra::TVibraModeState aNewState)=0;
        
		/**
        * Notifies feedback vibration mode setting changes in profile
        *
        * @param aNewState the new state of setting.
        */
        virtual void VibraFeedbackModeChange(CHWRMVibra::TVibraFeedbackModeState aNewState)=0;

    };

/**
*  This class observes vibration feature setting in device profile
*
*/
class CHWRMVibraProfileObserver : public CBase,
                                  public MCenRepObserver
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CHWRMVibraProfileObserver* NewL(MHWRMVibraProfileObserver*  aCallback);
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CHWRMVibraProfileObserver();
        
    public: // New functions
    
        // Get the current vibra mode state
        IMPORT_C CHWRMVibra::TVibraModeState GetVibraModeState();
		
		// Get the current vibra feedback-mode state
		IMPORT_C CHWRMVibra::TVibraFeedbackModeState GetVibraFeedbackModeState();
    
    public: // Functions from base classes

        // From MCenRepObserver
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);
        void HandleNotifyError(TUint32 aId, TInt error, CCenRepListener* aHandler);

    private:
    
        /**
        * C++ default constructor.
        */
        CHWRMVibraProfileObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MHWRMVibraProfileObserver*  aCallback);        

    private:
    
        CRepository*                iRepository;          	// Cenrep client
        CCenRepListener*       		iNotifyHandler;       	// Cenrep notifyhandler
        CCenRepListener*       		iFeedbackNotifyHandler; // Cenrep notifyhandler
        MHWRMVibraProfileObserver*  iCallback;            	// Callback object for passing vibra mode changes. Not owned
    };



#endif  // HWRMVIBRAPROFILEOBSERVER_H
            
// End of File
