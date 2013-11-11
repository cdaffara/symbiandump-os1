/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It is the client side remote class of phone server session.
*
*/


#ifndef RPHCLTSERVER_H
#define RPHCLTSERVER_H


//  INCLUDES
#include <e32std.h>
#include    <PhCltTypes.h>

// CONSTANTS
// A version must be specified when creating a session with the server
// These are the version numbers for PhoneServer.
const TUint KPhoneServerMajorVersionNumber = 0;
const TUint KPhoneServerMinorVersionNumber = 1;
const TUint KPhoneServerBuildVersionNumber = 1;

// Default amount of message slots.
const TInt KPhCltDefaultMessageSlots = 4;

// DATA TYPES

// PhoneClient panic codes visible to the user.
enum TPhCltClientSidePanics
    {
    EPhCltClientSidePanicNullHandle = 0, // Handle was NULL
    //
    EPhCltClientSidePanicLast            // Keep as last enum value.
    };


// CLASS DECLARATION

/**
*  It is the client side remote class of phone server session.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class RPhCltServer : public RSessionBase
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor.
        */
        IMPORT_C RPhCltServer();
       
    public:  // New functions

        /**
        * Returns the version of the API.
        *
        * @return Returns the version encapsulated in TVersion object.
        */
        TVersion Version() const;

        /**
        * Creates connection to the server.
        * 
        * @param aMessageSlots It tells the amount of message slots to be used
        *                      for the connection.
        * @return Returns error code.
        */
        IMPORT_C TInt Connect( TInt aMessageSlots = KPhCltDefaultMessageSlots );

        /**
        * Creates all submodules in Phone Server side.
        *
        * @param aStatus It is used to notify the completion of the request. 
        *                Then it indicates the success/failure of the operation.
        * @return error code.
        */
        IMPORT_C void CreateAll( TRequestStatus& aStatus );


    private: // Panic

        // Panic types to be used in this class.
        enum TPhCltPanicTypes
            {
            // Connection failed.
            EPhCltPanicServerConnectionFailed = 0,
            // Server was not found in starting phase.
            EPhCltPanicServerUnexpectedServerNotFound,
            // Server startup error.
            EPhCltPanicServerStartupObservationResultedInError,
            //
            EPhCltPanicLast  // Keep as last value.
            };
        
        // Panic function.
        static void Panic( TPhCltPanicTypes aPanic );


    private: // Internal methods

        // Starts the Phone Server.
        TInt StartServer();

    private:
   
        // By default, prohibit copy constructor
        RPhCltServer( const RPhCltServer& );
        // Prohibit assigment operator
        RPhCltServer& operator= ( const RPhCltServer& );

    };

#endif      // RPHCLTSERVER_H   
            
// End of File
