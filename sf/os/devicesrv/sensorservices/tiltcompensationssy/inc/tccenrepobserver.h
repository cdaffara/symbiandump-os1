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
* Description:  CTCCenrepObserver class declaration.
*
*/


#ifndef TCCENREPOBSERVER_H
#define TCCENREPOBSERVER_H

#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include <centralrepository.h>

/**
* Callback interface for cenrep change passing.
*
*  @since S60 5.0
*/
class MTCCenrepObserver
    {
    public:
    
        /**
        * Notifies cenrep value change
        *
        * @param aKey The key which changed.
        */
        virtual void CenrepValueChanged( TUint32 aKey,
            CRepository& aRepository ) = 0;
    };

/**
*  This class observes compensation parameter changes
*
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCCenrepObserver ): public CBase,
                                        public MCenRepNotifyHandlerCallback
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        */
        static CTCCenrepObserver* NewL( MTCCenrepObserver& aCallback );
        
        /**
        * Destructor.
        */
        virtual ~CTCCenrepObserver();
        
    public:
    
        // Returns handle to repository
        CRepository& GetCenrepHandle();

        // From MCenRepNotifyHandlerCallback
        void HandleNotifyGeneric( TUint32 aId );
        void HandleNotifyError( TUint32 aKey, TInt aError, CCenRepNotifyHandler* aHandler );

    private:
    
        /**
        * C++ constructors.
        */
        CTCCenrepObserver( MTCCenrepObserver& aCallback );

        /**
        * 2nd phase of construction
        */
        void ConstructL();        

    private: // Data
    
        // Cenrep client. Owned.
        CRepository*                iRepository;
        
        // Cenrep notifyhandler. Owned.
        CCenRepNotifyHandler*       iNotifyHandler;
        
        // Callback object for passing parameter changes.
        MTCCenrepObserver&          iCallback;
    };

#endif  // TCCENREPOBSERVER_H
            
// End of File
