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
* Description:  SUPL PSY singleton class.
*
*/



#ifndef SUPLPSYREQUESTMANAGER_H
#define SUPLPSYREQUESTMANAGER_H

// INCLUDES
#include "suplpsycellidhandler.h"
// FORWARD DECLARATIONS

// CLASS DECLARATION
class MSuplPsyRequestCompleteListener;
class CSuplPsyRequestor;
class CEnvironmentChangeNotifier;
class CSuplPsyInfoStoreManager;

/**
*  This class implements singleton class of SUPL PSY
*  
*  This class provides function to instance a singleton object of SUPL PSY. 
*  All client connections and location requests are managed by this class.
*
*  @lib nlaSsuplpsy.dll
*  @since S60 3.1
*/
class CSuplPsyRequestManager : public CBase, public MBasicCellIDListener
    {
    public:  // Constructors and destructor
        
        /**
        * Register the positioner and get singleton instance of the object.
        * @param aPositioner The reference to the positioner object that 
        *                    will register to request manager.
        */
        static CSuplPsyRequestManager* RegisterL( 
            MSuplPsyRequestCompleteListener& aPositioner );

        /**
        * Unregister the positioner. The singleton object will be self - destructed
        * if not positioner is registered
        * @param aPositioner reference to the positioner that unregister to
        *        request manager.
        */
        void Unregister( 
            MSuplPsyRequestCompleteListener& aPositioner );
        /**
        * Recieves max age set by client 
        * @param aMaxAge Max age value
        *        
        */
        void RecieveMaxAge( TTime aMaxAge );    
        

    public: // Functions from base classes

        /**
        * Make location request to request manager
        * @param aPositioner The requestor that makes location request
        * @since S60 3.1
        */
        void NotifyPositionUpdate( 
            MSuplPsyRequestCompleteListener&    aPositioner );


        /**
        * Cancel location request from request manager
        * @param aPositioner Reference to the requestor that will cancel the on - going
        *        location request.
        * @since S60 3.1
        */
        void CancelNotifyPositionUpdate( 
            MSuplPsyRequestCompleteListener& aPositioner );
            
        /**
        * Check if the last position is still valid
        * @param aMaxAge The max age
        * @return ETrue if last position valid. Otherwise, return EFalse
        */
        TBool IsLastPostionValid( TTime aMaxAge ) const;
        
        /**
        * Get position info
        * @return Position info. The value is valid only if there has been location
        * received
        */
        const HPositionGenericInfo& GetPosition();
       /** Callback for completion of cell id request
        * 
        * @return Position info. The value is valid only if there has been location
        * received
        */
        void BasicCellIDRequestCompletedL(TInt aErrorCode);
        
        
    private:

        /**
        * C++default constructor.
        */
        CSuplPsyRequestManager();

        /**
        * Destructor.
        */
        ~CSuplPsyRequestManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Register a positioner
        */
        void RegisterPositionerL( 
            MSuplPsyRequestCompleteListener& aPositioner );

        /**
        * Find the index of positioner
        */
        TInt FindPositioner( 
            MSuplPsyRequestCompleteListener& aPositioner ) const;

        /**
        * Check if there is location request on going
        */
        TBool IsActiveLocationRequest() const;
        
        /**
        * Location request complete call back function
        */
        void LocationRequestComplete();
        
        /**
        * Location request complete static call back
        */
        static TInt LocationRequestCompleteStatic( TAny* aAny );

        /**
        * Callback function for environment change notifier.
        */
        static TInt NotifierCallBack( TAny* aPtr );

        /**
        * This function is called whenever environment change notifier
        * reports that something has changed. The only change we are
        * interested of is the system time change.
        */
        void HandleEnvironmentChange();

    private:    
        //TPositioner connection
        struct TPositionerStatus
            {
            //Reference to connected positioner
            MSuplPsyRequestCompleteListener* iPositioner;
            
            //Positioner has active location request
            TBool iRequested;
            };

    private:
        //Positioner array
        RArray < TPositionerStatus > iPositionerArray;
        
        //Position requestor
        CSuplPsyRequestor* iRequestor;

        //Environment change notifier
        CEnvironmentChangeNotifier* iEnvChangeNotifier;
        
        // Returns Cell info 
        CSuplPsyCellIdHandler* iSuplPsyCellIdHandler;
        
        CSuplPsyInfoStoreManager* iSuplPsyInfoStoreManager;
        //Cell Id info
        TCellIdInfo iCellIdInfo;
        
        TTime iMaxAge;
    };

#endif      // SUPLPSYREQUESTMANAGER_H   
            
// End of File
