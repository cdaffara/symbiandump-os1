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
* Description:  Implementation of accessory UI note notifier.
*
*/


#ifndef __CACCFWUINOTENOTIFIER_H
#define __CACCFWUINOTENOTIFIER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <eiknotapi.h>

// CONSTANTS
const TUid KAccFwUiNoteNotifierUid={0x10205061};

// FUNCTION PROTOTYPES
CArrayPtr<MEikSrvNotifierBase2>* NotifierArray();

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Displays the accessory unsupported note.
*  @lib -
*  @since S60 3.1
*/
class CAccFwUiNoteNotifierStub : public CBase, public MEikSrvNotifierBase2
    {
    public:  // Constructors and destructor

        static CAccFwUiNoteNotifierStub* NewL();	                    
        
        ~CAccFwUiNoteNotifierStub();
        
    public: // From base class
                
        void Release();

        TNotifierInfo RegisterL();

        TNotifierInfo Info() const;

        TPtrC8 StartL(const TDesC8& aBuffer);

        void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);

        void Cancel();

        TPtrC8 UpdateL(const TDesC8& aBuffer);

                
    private: // Functions

        /**
        * C++ default constructor.
        */
        CAccFwUiNoteNotifierStub();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
	
    private:    // Data
	
				// Notifier info    
        TNotifierInfo iInfo;

    };

#endif      // __CACCFWUINOTENOTIFIER_H

// End of File

