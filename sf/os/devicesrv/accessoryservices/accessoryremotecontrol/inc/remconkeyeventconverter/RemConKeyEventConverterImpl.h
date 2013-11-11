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
* Description:  CRemConKeyEventConverter implementation -class.
*                    Defines the Accessory Key event handling API of
*                    Accessory Framework.
*
*/



#ifndef CREMCONKEYEVENTCONVERTERIMPL_H
#define CREMCONKEYEVENTCONVERTERIMPL_H

//  INCLUDES
#include "RemConKeyEventConverter.h"
#include "RemConKeyEventData.h"
#include "RemConKeyEventContainer.h"

// CLASS DECLARATION

/**
*  CRemConKeyEventConverter implementation -class.
*
*  @lib RemConKeyEventConverter.dll (ECom plugin)
*  @since S60 3.0
*/
class CRemConKeyEventConverterImpl : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRemConKeyEventConverterImpl* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRemConKeyEventConverterImpl();

    public: // Functions from base classes

        /**
        * Resolve operation ID based on the given key-event data.
        * The key event data structure is declared at AccKeyEventData.h -file.
        * @since S60 3.0
        * @param aEvent Key event to be handled by Logical ASY 
        *   (see AccKeyEventData.h).
        * @param aOperationID The operation ID to which the found ID is 
        *   assigned.
        * @return KErrNone if operation ID resolved, KErrNotFound if matching 
        * event was not found.
        */
        TInt ResolveOperationID( const TRemConKeyEventData& aEvent, 
            TUid& aInterfaceUID, TUint& aOperationID, TDes8& aData ) const;
    
    private:

        /**
        * C++ default constructor.
        */
        CRemConKeyEventConverterImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
        
        // Container for key events.
        CRemConKeyEventContainer* iKeyEventContainer;

    };

#endif      // CREMCONKEYEVENTCONVERTERIMPL_H   
            
// End of File
