/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class deliveres synchronization related operations from 
*				 accessories to client that listens to them. It also provides 
*				 methods for client to respond to synchronization operations.
*
*/



#ifndef CREMCONSYNCHRONIZATIONTARGET_H
#define CREMCONSYNCHRONIZATIONTARGET_H

//  INCLUDES
#include <RemConExtensionApi.h>
#include <e32base.h>
#include <remcon/remconinterfacebase.h>
#include <remcon/remconinterfaceif.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MRemConSynchronizationTargetObserver;
class CRemConInterfaceSelector;

// CLASS DECLARATION

/**
*  Part of Remote Control Extension Api.
*  This class receives synchronization related operations from accessories.
*
*  @lib RemConExtensionApi.dll
*  @since Series 60 3.0
*/
class CRemConSynchronizationTarget : public CRemConInterfaceBase, MRemConInterfaceIf3
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        @param aInterfaceSelector The interface selector.
		@param aObserver The observer of this interface.
		@return A new CCRemConSynchronizationTarget, owned by the interface 
		selector.
        */
        IMPORT_C static CRemConSynchronizationTarget* NewL(
        	CRemConInterfaceSelector& aInterfaceSelector, 
			MRemConSynchronizationTargetObserver& aObserver);
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CRemConSynchronizationTarget();

    public: // New functions
        
        /**
        * Called by client to send a response to remote target's operation after
		* client has processed operation.
        * @since Series 60 3.0
        * @param aStatus	Indicates the completion status of a request made
        * @param aError		An error code.
        * @return 			void		
        */
        IMPORT_C void SynchronizeResponse( TRequestStatus& aStatus, TInt aError );
        
        /**
        * Method to get the bearer source detail for the last command received
        * 
        * @since TB 9.2
        * @param aSource returns the bearer source as TRemConExtCmdSource
        * @return void
        */
        IMPORT_C void GetCommandSourceInfo( TRemConExtCmdSource& aSource );
	                    
    public: // Functions from base classes
        
    protected:  // New functions
    
    protected:  // Functions from base classes
            
        // From CRemConInterfaceBase
    	TAny* GetInterfaceIf( TUid aUid );
        
        // From MRemConInterfaceIf3
    	void MrcibNewMessage(TUint aOperationId, const TDesC8& aData, TRemConMessageSubType aMessageSubType, const TRemConAddress& aRemoteAddr);
    	
    private:

        /** 
		Constructor.
		@param aInterfaceSelector The interface selector.
		@param aObserver The observer of this interface.
		*/
		CRemConSynchronizationTarget(CRemConInterfaceSelector& aInterfaceSelector, 
			MRemConSynchronizationTargetObserver& aObserver);

    public:     // Data
    
    protected:  // Data
   
    private:    // Data
    	// Observer implemented by client of CRemConSynchronizationTarget
    	MRemConSynchronizationTargetObserver& iObserver;
		// Stores any parameters associated with operation response client sends.   
		TBuf8<KRemConExtParamMaxLength> iOutData;
		// Stores the number of remotes the message was sent to.	
		TUint iNumRemotes;
		
		// Stores the UID of the bearer from which the last command was received
		TUint32 iSourceUID;

    public:     // Friend classes
   
    protected:  // Friend classes
   
    private:    // Friend classes
   
    };

#endif // CREMCONSYNCHRONIZATIONTARGET_H 
            
// End of File
