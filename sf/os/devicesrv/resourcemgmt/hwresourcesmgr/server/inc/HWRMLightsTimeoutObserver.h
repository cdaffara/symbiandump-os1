/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HWRMLLIGHTSTIMEOUTOBSERVER_H
#define HWRMLLIGHTSTIMEOUTOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include "cenrepobserver.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

/**
* Callback interface for lights timeout change passing.
*/
class MHWRMLightsTimeoutObserver
    {
    public:
    
        /**
        * Notifies lights timeout changes in cenrep
        *
        * @param aTimeout the new timeout value
        */
        virtual void LightsTimeoutChange(TInt aTimeout)=0;
    };

/**
*  This class observes lights timeout cenrep key in lights settings
*
*/
class CHWRMLightsTimeoutObserver : public CBase,
                                   public MCenRepObserver
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CHWRMLightsTimeoutObserver* NewL(MHWRMLightsTimeoutObserver*  aCallback);
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CHWRMLightsTimeoutObserver();
        
    public: // New functions
    
    public: // Functions from base classes

        // From MCenRepObserver
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);
        void HandleNotifyError(TUint32 aId, TInt error, CCenRepListener* aHandler);

    private:
    
        /**
        * C++ default constructor.
        */
        CHWRMLightsTimeoutObserver();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MHWRMLightsTimeoutObserver*  aCallback);        

    private:
    
        CRepository*                 iRepository;          // Cenrep client
        CCenRepListener*        iNotifyHandler;       // Cenrep notifyhandler
        MHWRMLightsTimeoutObserver*  iCallback;            // Callback object for passing value changes. Not owned
    };



#endif  // HWRMLLIGHTSTIMEOUTOBSERVER_H
            
// End of File
