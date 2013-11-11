/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the PhCltExtension interface.
*
*/



#ifndef CPHCLTCOMHANDLIMPLEMENTATION_H
#define CPHCLTCOMHANDLIMPLEMENTATION_H


//  INCLUDES

#include    "RPhCltCommandHandler.h"
#include    "RPhCltCommandHandlerNotify.h"
#include    "RPhCltServer.h"


// CLASS DECLARATION

/**
*  Implementation of the CPhCltCommandHandler interface.
*
*
*  @lib PhoneClient.lib
*  @since 3.2
*/
NONSHARABLE_CLASS( CPhCltCommandHandlerImplementation )
:   public CPhCltCommandHandler
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CPhCltCommandHandlerImplementation* NewL();
        

        /**
        * Destructor.
        */
        virtual ~CPhCltCommandHandlerImplementation();


    public: // Functions from base classes

        /**
        * @see CPhCltCommandHandler.
        */
		void Atd(
            TRequestStatus& aStatus, 
            const TPhCltTelephoneNumber& aTelephoneNumber );
            

        /**
        * @see CPhCltCommandHandler.
        */
        void Ata(
            TRequestStatus& aStatus );


        /**
        * @see CPhCltCommandHandler.
        */
        void Chld(
            TRequestStatus& aStatus, 
            const TPhCltChldCommand aChldCommand,
            const TUint aCallNumber );


        /**
        * @see CPhCltCommandHandler.
        */
        void Chup(
            TRequestStatus& aStatus );


        /**
        * @see CPhCltCommandHandler.
        */
        void Vts(
            TRequestStatus& aStatus, 
            const TPhCltDtmfTone aDtmfTone,
            const TPhCltDtmfAction aAction );


        /**
        * @see CPhCltCommandHandler.
        */
        void Bvra( 
            TRequestStatus& aStatus, 
            const TBool aActivation );


        /**
        * @see CPhCltCommandHandler.
        */
        void CancelAsyncRequest( 
            const TPhCltComHandCommand aReqToCancel );

        /**
        * @see CPhCltCommandHandler.
        */
        void MuteMicrophone( TRequestStatus& aStatus, const TBool aMute );
        
        /**
        * @see CPhCltCommandHandler.
        */
        void MuteRingingTone( TRequestStatus& aStatus );

    private:

        /**
        * C++ constructor.
        */
        CPhCltCommandHandlerImplementation();


        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // Data

        // The command handler to perform requests.
        RPhCltCommandHandler    iCommandHandler;
        
        //RPhCltServer class instance creates phonesession
        RPhCltServer iServer;

    };


#endif      // CPHCLTCOMHANDLIMPLEMENTATION_H

  
// End of File
