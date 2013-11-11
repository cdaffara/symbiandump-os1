/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*     CHeadsetStatus functionality.
*
*/


#ifndef __HEADSET_H__
#define __HEADSET_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CHeadsetStatusContainer;

// CLASS DECLARATION

/**
* Observer interface through headset status change events are notified.
*/
class MHeadsetStatusObserver
    {
    public:     // New methods
    
        /**
        * Callback for headset status change notifications.
        * @param    aHeadsetConnected: ETrue if connected
        * @return   None
        */
        virtual void HeadsetStatusChangedL( TBool aHeadsetConnected ) = 0;
        
        /**
        * Callback for error situations.
        * @param    aError: Error code
        * @return   None
        */
        virtual void HeadsetStatusError( TInt aError ) = 0;
    
    protected:    // Deny deletion through this class
    
        /**
        * C++ destructor
        */
        virtual ~MHeadsetStatusObserver() {};
    };

// CLASS DECLARATION

/**
*  API for checking the headset status.
*  This implementation is a wrapper that encapsulates internal interface 
*  for extended SDK
*  
*  Usage:
*  Include HeadsetStatus.h and link against HeadsetStatus.lib
*
*  Note:
*  If you intend to call HeadsetConnectedL more than once it is more efficient
*  to declare the instance from CHeadsetStatus as a member variable and create it
*  in ConstructL and delete it in destructor.
*
*  Observing is only enbaled in Series 60 3.1 with __ACCESSORY_FW flag enabled!
*
*  @code
*  TBool connected( EFalse );
*  CHeadsetStatus* wrapper = CHeadsetStatus::NewL();
*  CleanupStack::PushL( wrapper );
*  TInt err( wrapper->HeadsetConnectedL( connected ) );
*  CleanupStack::PopAndDestroy( wrapper );
*  @endcode
*/
class CHeadsetStatus : public CBase 
    {
    public: // Constructors & destructors
       
        /**
        * Destructor 
        */
        IMPORT_C ~CHeadsetStatus();

        /**
        * Symbian two phased constructor.
        */
        IMPORT_C static CHeadsetStatus* NewL();
        
    public: // New methods
    
        /**
        * Used to query the headset connectivity status. 
        * @param    aState: Boolean value of headset status
        * @return   KErrNone if query succeeded, otherwise one of the
        *           system wide error codes.
        */
        IMPORT_C TInt HeadsetConnectedL( TBool& aState );
        
        /**
        * Sets observer through headset status notifications are received.
        * NULL pointer will lead to panic.
        * @param    aObserver: Observer
        * @return   None
        */
        IMPORT_C void SetObserver( MHeadsetStatusObserver* aObserver );
        
        /**
        * Starts observing headset status changes.
        * @param    None
        * @return   None
        */
        IMPORT_C void StartObserving();
        
        /**
        * Stops observing headset status changes.
        * This is not nessecary to be called in destructor.
        * Listening will be automatically stopped when CHeadsetStatus
        * object is deleted.
        * @param    None
        * @return   None
        */
        IMPORT_C void StopObserving();
        
        /**
        * Checks whether already observing.
        * @param    None
        * @return   ETrue if observing
        */
        IMPORT_C TBool IsObserving() const;
         
    private: 
        
        /**
        * Default Constructor 
        */
        CHeadsetStatus( );
        
        /**
        * Symbian OS 2nd phase constructor 
        */
        void ConstructL();  
         
    private:
    
        // headset container, owned
        CHeadsetStatusContainer* iContainer;
    };

#endif //__HEADSET_H__

// End of File
