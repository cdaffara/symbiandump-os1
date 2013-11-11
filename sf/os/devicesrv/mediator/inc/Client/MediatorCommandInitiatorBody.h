/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An implementation class for issuing Mediator Service commands.
*
*/

#ifndef MEDIATORCOMMANDINITIATORBODY_H
#define MEDIATORCOMMANDINITIATORBODY_H

// INCLUDES
#include <e32base.h>

#include "MediatorCommandInitiator.h"
#include "MediatorServerClient.h"

// CLASS DECLARATION

/**
*  A class used to issue Mediator Service commands.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS(CMediatorCommandInitiatorBody) : public CActive
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param aObserver  command response observer
        * @return a new MMediatorCommandResponseObserver instance.
        */
        static CMediatorCommandInitiatorBody* NewL(
            MMediatorCommandResponseObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CMediatorCommandInitiatorBody();

    public: // Command initiating
        
        /**
        * Issue a Mediator Service command.
        * 
        * @since S60 3.1   
        * @param aDomain    The identifier of the domain     
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aVersion   The version information for the command.
        * @param aData      The parameters of the command.
        * @return TInt      an error code
        */
        TInt IssueCommand( TUid aDomain, 
                           TUid aCategory, 
                           TInt aCommandId,
                           TVersion aVersion, 
                           const TDesC8& aData );

        /**
        * Cancel a Mediator Service command.
        * 
        * @since S60 3.1 
        * @param aDomain    The identifier of the domain         
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @return None.
        */
        void CancelCommand( TUid aDomain, 
                            TUid aCategory, 
                            TInt aCommandId );
    
    protected:  // Functions from base classes
        
        /**
        * From CActive::RunL
        */
        void RunL();
        
        /**
        * From CActive::DoCancel()
        */
        void DoCancel();
		
		/**
		* From CActive::RunError()
		*/
		TInt RunError( TInt aError );
    
    private:    // new functions
        /**
        * Start to wait command response asynchronously
        *
        * @since S60 3.1
        * @return None.
        */
        void WaitForCommandResponse();
        
        /**
        * Re-init return data buffer
        *
        * @since S60 3.1
        * @param aSize size of the buffer
        * @return None.
        */
        void ResetDataBufferL( TInt aSize );
        
    private:
    
        /**
        * C++ default constructor.
        *
        * @param aObserver  command response observer
        */
        CMediatorCommandInitiatorBody( 
                    MMediatorCommandResponseObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private: // Data

        // A session class to mediator server
        RMediatorServer     iMediatorServer;
        
        // Command response observer
        MMediatorCommandResponseObserver* iObserver;
        
        // Buffer for Command parameter data
        HBufC8*     iCommandData;
        TPtr8       iCommandDataPtr;
        
        // For category information 
        TMediatorCategory           iCategory;
        TMediatorCategoryRetBuffer  iCategoryBuffer;
        
        // For status information
        TInt                        iResponseStatus;
        TPckg<TInt>                 iStatusBuffer;
        
        // For command information
        MediatorService::TCommand   iCommand;
        TCommandRetBuffer           iCommandBuffer;

	};

#endif      // MEDIATORCOMMANDINITIATORBODY_H
            
// End of File
