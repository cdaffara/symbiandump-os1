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
* Description:  Implementation of accessory selection dialog.
*
*/


#ifndef __CACCFWUIDIALOGNOTIFIER_H
#define __CACCFWUIDIALOGNOTIFIER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <eiknotapi.h>
#include <badesca.h>


// CONSTANTS
const TUid KAccFwUiDialogNotifierUid={0x10205062};
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CAccFWUINotifierStubObserver;

// CLASS DECLARATION
/**
*  Implementation of accessory selection dialog.
*  @lib -
*  @since S60 3.1
*/
class CAccFwUiDialogNotifierStub : /*public CActive,*/ public MEikSrvNotifierBase2
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.        
		* @return The created object.
        */
        static CAccFwUiDialogNotifierStub* NewL();	                    
        
        ~CAccFwUiDialogNotifierStub();
        
    public: // From MEikSrvNotifierBase2
                
        void Release();

        TNotifierInfo RegisterL();

        TNotifierInfo Info() const;

        TPtrC8 StartL(const TDesC8& aBuffer);

        void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);

        void Cancel();

        TPtrC8 UpdateL(const TDesC8& aBuffer);
        
        void SetObserverSelection( RArray<TInt> aSelection, TBool aCancelPressed);

		//from CActive
	

	protected:
		//from CActive
		void DoCancel();

                
    private: // Functions

        /**
        * C++ default constructor.
        */
        CAccFwUiDialogNotifierStub();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
	
    private:    // Data
    
    		//Notifier info
        TNotifierInfo iInfo;
        

		//selections 
	//	TUint32 iSelections;		
        TBool iIsCancelled;
		
        //command observer
       // CAccFWUINotifierStubObserver* iObserver;
        
        
        //resource file offset
        TInt iResourceFileOffset;
        
        //item list...
       	CDesCArray *iItemList;
        
		//selections list
		RArray<TInt> iSelections;	
		
		//for coverui
		//CArrayFixFlat<TInt>* iCoverUIITems;			
		
		//message to complete
		RMessagePtr2 iMessage;
        
        //rmessage reply slot
        TInt iReplySlot;
        
        //is notifier cancelled
	    TBool iCancel;
        
        //CCoeEnv* iCoeEnv;  // not owned
        

    };

#endif      // __CACCFWUIDIALOGNOTIFIER_H

// End of File


