/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Key event class used internally by the converter. Used to map
*                operation IDs, key actions etc. with key event data passed from
*                bearer plugins. 
*
*/



#ifndef CREMCONKEYEVENT_H
#define CREMCONKEYEVENT_H

//  INCLUDES
#include    "RemConKeyEventData.h"
#include    <remconcoreapi.h>
#include    <e32std.h>
#include    <e32cmn.h>
#include    <e32base.h>

// CONSTANTS

// CLASS DECLARATION

/**
*  Key event class used internally by the converter.
*
*  @lib RemConKeyEventConverter.dll (ECom plugin)
*  @since S60 3.0
*/
class CRemConKeyEvent : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRemConKeyEvent* NewL( const TRemConKeyEventData& aKeyEvent, 
            const TUid& aInterfaceUid, 
            const TUint& aOperationID, 
            const TRemConCoreApiButtonAction& aButtonAction );
        
        /**
        * Two-phased constructor.
        */    
        static CRemConKeyEvent* NewL( const TRemConKeyEventData& aKeyEvent, 
    		const TUid& aInterfaceUid, 
    		const TUint& aOperationID,
    		TDesC8& aParameter );
        
        /**
        * Destructor.
        */
        virtual ~CRemConKeyEvent();

    public: // New functions
        
        /**
        * Compares two TRemConKeyEventData instances for equality.
        * @since S60 3.0
        * @param TRemConKeyEventData The key event against which this event is
        * compared to.
        * @return ETrue if the events match, otherwise EFalse.
        */
        TBool Compare( const TRemConKeyEventData& aKeyEvent ) const;
		        
        /**
        * Return the interface UID of this key event.
        * @since S60 3.0
        * @param none
        * @return The interface UID of this key event
        */
        TUid InterfaceUID() const;
        
        /**
        * Return the operation ID of this key event.
        * @since S60 3.0
        * @param none
        * @return The operation ID of this key event
        */
        TUint OperationID() const;

        /**
        * Returns parameter of this key event.
        * @since S60 3.1
        * @param none
        * @return The parameter of this key event
        */
        TDesC8& Parameter() const;
        
    private:

        /**
        * C++ default constructor.
        */
        CRemConKeyEvent();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TRemConKeyEventData& aKeyEvent, 
            const TUid& aInterfaceUid, 
            const TUint& aOperationID, 
            TDesC8& aParameter );

    private:    // Data
    
        // The key event data of this key event.
        TRemConKeyEventData iKeyEvent;

        // The interface UID of this key event.
        TUid iInterfaceUID;
        
        // The operation ID of this key event.
        TUint iOperationID;
       	
		// The parameter of this key event.
        HBufC8* iParameter;
    
    };

#endif      // CREMCONKEYEVENT_H   
            
// End of File
