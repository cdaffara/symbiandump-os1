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
* Description:  An interface for issuing Mediator Service commands.
*
*/

#ifndef MEDIATORCOMMANDINITIATOR_H
#define MEDIATORCOMMANDINITIATOR_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class CMediatorCommandInitiatorBody;

// CLASS DECLARATION

/**
*  An observer interface for receiving responses to Mediator Service commands.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class MMediatorCommandResponseObserver
    {
    public: // Receiving responses

        /**
        * A response to a Mediator Service command.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain          
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aStatus    Standard Symbian error code indicating the
        *                   success of the command.
        * @param aData      The parameters of the response.
        * @return None.
        */
        virtual void CommandResponseL( TUid aDomain,
                                       TUid aCategory, 
                                       TInt aCommandId,
                                       TInt aStatus, 
                                       const TDesC8& aData ) = 0;
    };


// CLASS DECLARATION

/**
*  A class used to issue Mediator Service commands.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class CMediatorCommandInitiator : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aObserver  command response observer
        * @return a new CMediatorCommandInitiator instance.
        */
        IMPORT_C static CMediatorCommandInitiator* NewL(
            MMediatorCommandResponseObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CMediatorCommandInitiator();

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
        IMPORT_C TInt IssueCommand( TUid aDomain, 
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
        IMPORT_C void CancelCommand( TUid aDomain, 
                                     TUid aCategory, 
                                     TInt aCommandId );
        
    private:
    
        /**
        * C++ default constructor.
        */
        CMediatorCommandInitiator();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aObserver command response observer
        * @return None.
        */
        void ConstructL( MMediatorCommandResponseObserver* aObserver );
    
    private: // Data

        // Pointer to implementation body
        CMediatorCommandInitiatorBody*  iBody;
        
	};

#endif      // MEDIATORCOMMANDINITIATOR_H
            
// End of File
