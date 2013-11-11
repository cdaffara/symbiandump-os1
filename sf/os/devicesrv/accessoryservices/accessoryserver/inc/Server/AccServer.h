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
* Description:  Server's side server class
*
*/


#ifndef CACCSERVER_H
#define CACCSERVER_H

// INCLUDES
#include "AccClientServerMessages.h"
#include "AsyProxyAPI.h"
#include <e32base.h>

// CONSTANTS
// ----------------------------------------------------------------------------------------
// Accessory Server's policy
// ----------------------------------------------------------------------------------------

//Total number of ranges
const TUint KAccessoryServerRangeCount = 7;

//Definition of the ranges of IPC numbers
const TInt accessoryServerRanges[KAccessoryServerRangeCount] =
    {
    EAccSrvCreateSubSessionAccessoryMode,        //0th range. Accessory API;               EAccSrvCreateSubSessionAccessoryMode -> EAccSrvGetValueTDes8
    EAccSrvCreateSubSessionAccessoryConnection,  //1st range. Accessory & Accessory Monitoring API; EAccSrvCreateSubSessionAccessoryConnection -> EAccSrvGetValueTInt
    EAccSrvCreateSubSessionAccessoryControl,     //2nd range, Accessory Control API;       EAccSrvCreateSubSessionAccessoryControl -> EAccSrvSetAccessoryModeSettings
    EAccSrvCreateSubSessionAccessoryBTControl,   //3rd range, Accessory BT Control API ;   EAccSrvCreateSubSessionAccessoryBTControl -> EAccSrvCancelNotifyBluetoothAudioLinkCloseReq
    EAccSrvCreateSubSessionAccessoryAudioControl,//4th range, Accessory Audio Control API; EAccSrvCreateSubSessionAccessoryAudioControl->EAccSrvCancelNotifyAccessoryAudioLinkClosed
    EAccSrvCreateSubSessionAccessoryAsyComms,    //5th range, Accessory Asy Comms API;     EAccSrvCreateSubSessionAccessoryAsyComms->EAccSrvProcessResponseTDes    
    EAccSrvUnknown                               //6th range,                              non implemented function end of range check ; ENotSupported
    };

//Policy to implement for each of the above ranges
const TUint8 accessoryServerElementsIndex[KAccessoryServerRangeCount] =
    {
    0,                           //applies to 0th range
    CPolicyServer::EAlwaysPass,  //applies to 1st range
    1,                           //applies to 2nd range
    2,                           //applies to 3rd range
    2,                           //applies to 4th range
    3,                           //applies to 5th range    
    CPolicyServer::ENotSupported //applies to 6th range (out of range IPC)
    //ECustomCheck
    //EAlwaysPass
    };

//Specific capability checks
const CPolicyServer::TPolicyElement accessoryServerElements[] =
    {
    { _INIT_SECURITY_POLICY_C1( ECapabilityReadUserData ), CPolicyServer::EFailClient },     //policy "0"
    { _INIT_SECURITY_POLICY_C3( ECapabilityReadUserData,
                                ECapabilityReadDeviceData,
                                ECapabilityWriteDeviceData
                             /* ECapabilityDRM */
                             /* ECapabilityLocalServices */), CPolicyServer::EFailClient },//policy "1"
    { _INIT_SECURITY_POLICY_C4( ECapabilityReadUserData,
                                ECapabilityReadDeviceData,
                                ECapabilityWriteDeviceData,
                             /* ECapabilityDRM */
                             /* ECapabilityLocalServices */
                                ECapabilitySurroundingsDD ), CPolicyServer::EFailClient },   //policy "2"
    { _INIT_SECURITY_POLICY_S0( 0x10205030 ), CPolicyServer::EPanicClient },                 //policy "3", i.e. access is allowed only for AccServer.exe
    };

//Package all the above together into a policy
const CPolicyServer::TPolicy accessoryServerPolicy =
    {
    CPolicyServer::EAlwaysPass,  //all connect attempts to Accessory Server pass
    KAccessoryServerRangeCount,  //number of ranges
    accessoryServerRanges,       //ranges array
    accessoryServerElementsIndex,//elements<->ranges index
    accessoryServerElements,     //array of elements
    };

// MACROS
_LIT( KASYProxyNotFound, "ASYproxy not found" );

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYProxyStarter;
class CAccSrvConnectionController;
class CAccSrvServerModel;

// CLASS DECLARATION
/**
*  Server's side server class
*
*  @lib AccServer.exe
*  @since S60 3.1
*/

NONSHARABLE_CLASS( CAccServer ) : public CPolicyServer
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccServer* NewLC();

        /**
        * Destructor.
        */
        ~CAccServer();

    public: // New functions

        /**
        * Main function for the server thread.
        * @since S60 3.1
        * @param void
        * @return Error code
        */
        static TInt ThreadFunction();

        /**
        * Creates one CObjectCon object for every session.
        * @since S60 3.1
        * @param void
        * @return a pointer to a CObjectCon
        */
        CObjectCon* NewContainerL();

        /**
        * Removes CObjectCon object from the container index and deletes the CObjectCon object.
        * @since S60 3.2
        * @param aObjectCon Object container to delete
        */
        void RemoveContainer( CObjectCon* aObjectCon );

    public:    // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

        /**
        * From CActive Process any errors
        */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CAccServer(TInt aPriority, TServerType aType);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Creates instance of ASYProxy
        */
        void CreateASYProxyL();

        /**
        * Raises panic for this server
        */
        static void PanicServer( TAccServerError aReason );

        /**
        * Constructs the server.
        * Creates, initializes and starts the ActiveScheduler.
        */
        static void ThreadFunctionL();

        /**
        * Creates main session for server
        */
        CSession2* NewSessionL( const TVersion &aVersion,
                                const RMessage2& aMessage ) const;

    public:     // Data

        // pointer to a CObjectConIx
        CObjectConIx* iObjectConIx; // Owned

    protected:  // Data

    private:    // Data

        CASYProxyAPI*                iProxy; // Owned
        CASYProxyStarter*            iProxyStarter; // Owned

        // Connection Controller is treated as singleton in Accessory Server.
        // This ensures that only one Connection Controller object exist and
        // its destructed properly.
        CAccSrvConnectionController* iConnectionController; // Owned

        // Server Model is treated as singleton in Accessory Server.
        // This ensures that only one Server Model object exist and
        // its destructed properly.
        CAccSrvServerModel*          iServerModel; // Owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSERVER_H

// End of File
