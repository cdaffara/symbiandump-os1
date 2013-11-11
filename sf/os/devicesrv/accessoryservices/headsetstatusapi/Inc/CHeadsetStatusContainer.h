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
*     CHeadsetStatusContainer functionality.
*
*/


#ifndef __HEADSETCONTAINER_H__
#define __HEADSETCONTAINER_H__

// INCLUDES
#include <e32base.h>
#ifdef __ACCESSORY_FW
#include <AccessoryServer.h>
#include <AccessoryMode.h>
#endif

// FORWARD DECLARATIONS
class MHeadsetStatusObserver;
class CHeadsetStatusObserver;

// CLASS DECLARATION

/** 
*  This implementation is a wrapper that encapsulates internal interfaces 
*  from extended SDK.
*  Construction will leave with KErrNotSupported unless __ACCESSORY_FW flag
*  is not defined.
*/ 
NONSHARABLE_CLASS( CHeadsetStatusContainer ): public CBase 
    {
    public:     // Constructors & destructors
       
        /**
        * Destructor 
        */
        ~CHeadsetStatusContainer();

        /**
        * Symbian two phased constructor.
        */
        static CHeadsetStatusContainer* NewL();

    public:     // New methods
    
        /**
        * Used to query the headset connectivity status. 
        * @param    aState: Boolean value of headset status
        * @return   KErrNone if query succeeded, otherwise one of the
        *           system wide error codes.
        */
        TInt QueryHeadsetStatus( TBool& aState );
         
        /**
        * Sets observer through headset status notifications are received.
        * @param    aObserver: Observer
        * @return   None
        */
        void SetObserver( MHeadsetStatusObserver* aObserver );
        
        /**
        * Starts observing headset status changes.
        * @param    None
        * @return   None
        */
        void StartObserving();
        
        /**
        * Stops observing.
        * @param    None
        * @return   None
        */
        void StopObserving();
        
        /**
        * Checks whether already observing.
        * @param    None
        * @return   ETrue if observing
        */
        TBool IsObserving() const;
        
    private: 
        
        /**
        * Default Constructor 
        */
        CHeadsetStatusContainer( );
        
        /**
        * Symbian OS 2nd phase constructor 
        */
        void ConstructL();  
               
    private:
    
        #ifdef __ACCESSORY_FW
        RAccessoryServer    iAccSrv;
        RAccessoryMode      iAccMode;
        // headset status observer, owned
        CHeadsetStatusObserver* iObserver;
        #endif
    };

#endif //__HEADSETCONTAINER_H__

// End of File
