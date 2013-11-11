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
* Description:  This class deliveres PoC related operations from accessories 
*				 to client that listens to them. It also provides methods for 
*				 client to respond to PoC operations.
*
*/



#ifndef CREMCONPOCTARGET_H
#define CREMCONPOCTARGET_H

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
class MRemConPocTargetObserver;
class CRemConInterfaceSelector;

// CLASS DECLARATION

/**
*  Part of Remote Control Extension Api.
*  This class receives PoC related operations from accessories.
*
*  @lib RemConExtensionApi.dll
*  @since Series 60 3.0
*/
class CRemConPocTarget : public CRemConInterfaceBase, MRemConInterfaceIf3
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        @param aInterfaceSelector The interface selector.
		@param aObserver The observer of this interface.
		@return A new CRemConPocTarget, owned by the interface selector.
        */
        IMPORT_C static CRemConPocTarget* NewL(
        	CRemConInterfaceSelector& aInterfaceSelector, 
			MRemConPocTargetObserver& aObserver );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CRemConPocTarget();

    public: // New functions
                
        /**
        * Called by client to send a response to remote target's operation after
		* client has processed operation.
        * @since Series 60 3.0
        * @param aStatus	Indicates the completion status of a request made
        * @param aError		An error code  
        * @return 			void		
        */
        IMPORT_C void APOCButtonUpResponse( TRequestStatus& aStatus, TInt aError );
		
		/**
        * Called by client to send a response to remote target's operation after
		* client has processed operation.
        * @since Series 60 3.0
        * @param aStatus	Indicates the completion status of a request made
        * @param aError		An error code  
        * @return 			void		
        */
		IMPORT_C void APOCButtonDownResponse( TRequestStatus& aStatus, TInt aError );
        
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
          
    private:	
                
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
		CRemConPocTarget( CRemConInterfaceSelector& aInterfaceSelector, 
			MRemConPocTargetObserver& aObserver );
        
        /**
        * Method that sends all reponses.
        */
        void SendResponse(
			TRequestStatus& aStatus,
			TInt aOperationId,
			TInt aError);


    public:     // Data
    
    protected:  // Data
    
    private:    // Data
    	// Observer implemented by client of CRemConPocTarget
    	MRemConPocTargetObserver& iObserver;
		// Stores any parameters associated with operation response client 
		// sends.   
		TBuf8<KRemConExtParamMaxLength> iOutData;
		// Stores the number of remotes the message was sent to.	
		TUint iNumRemotes;
		
		// Stores the UID of the bearer from which the last command was received
		TUint32 iSourceUID;

    public:     // Friend classes
    
    protected:  // Friend classes
    
    private:    // Friend classes
    
    };

#endif // CREMCONPOCTARGET_H 
            
// End of File
