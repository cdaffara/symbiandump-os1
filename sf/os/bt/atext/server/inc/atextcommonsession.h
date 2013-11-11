/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The session object for ATEXT common server
*
*/


#ifndef C_ATEXTCOMMONSESSION_H
#define C_ATEXTCOMMONSESSION_H

#include <e32svr.h>
#include "atextcommonobserver.h"

class CATExtSrv;
class CDesC8ArraySeg;

/**
 *  Manages separate ATEXT common server sessions for client(s)
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CATExtCommonSession ) : public CSession2,
                                           public MATExtCommonObserver
    {

public:

    /**
     * Two-phased constructor.
     * @param aServer
     * @return Instance of self
     */
    static CATExtCommonSession* NewL( CATExtSrvCommon& aServer,
                                      const TVersion& aVersion );

    /**
    * Destructor.
    */
    ~CATExtCommonSession();

private:

// from base class CSession2

    /**
     * From CSession2.
     * Handles the servicing of client requests passed to the server
     *
     * @since S60 5.0
     * @param aMessage Message containing the client request
     * @return None
     */
    void ServiceL( const RMessage2& aMessage );

    CATExtCommonSession( CATExtSrvCommon& aServer, const TVersion& aVersion );

    /**
     * Handles the servicing of client requests passed to the server
     *
     * @since S60 5.0
     * @param aMessage Message containing the client request
     * @return Symbian error code on error, KErrNone otherwise
     */
    void DoServiceCommonL( const RMessage2& aMessage );

    /**
     * Handles the servicing of setting up interface
     *
     * @since S60 5.0
     * @param aMessage Message for Connect() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    void IpcSetCommonInterfaceL( const RMessage2& aMessage );

    /**
     * Synchronously closes the session
     * Optional: client can do either Close() or SynchronousClose()+Close()
     *
     * @since S60 5.0
     * @param aMessage Message for SynchronousClose() client call
     * @return None
     */
    TInt IpcSynchronousClose( const RMessage2& aMessage );

    /**
     * Handles the servicing of mode status reporting
     *
     * @since S60 5.0
     * @param aMessage Message for GetMode() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcGetMode( const RMessage2& aMessage );

    /**
     * Handles the servicing of mode status change receiving
     *
     * @since S60 5.0
     * @param aMessage Message for ReceiveModeStatusChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReceiveModeStatusChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of mode status change receive cancel
     *
     * @since S60 5.0
     * @param aMessage Message for CancelReceiveModeStatusChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcCancelReceiveModeStatusChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of NVRAM status reporting
     *
     * @since S60 5.0
     * @param aMessage Message for GetNvramStatus() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcGetNvramStatus( const RMessage2& aMessage );

    /**
     * Handles the servicing of NVRAM status receiving
     *
     * @since S60 5.0
     * @param aMessage Message for ReceiveNvramStatusChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReceiveNvramStatusChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of pending NVRAM status receive cancel
     *
     * @since S60 5.0
     * @param aMessage Message for CancelReceiveNvramStatusChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcCancelReceiveNvramStatusChange( const RMessage2& aMessage );

    /**
     * Writes NVRAM status to client with WriteReplyBufferToClient() and
     * completes the client request message
     *
     * @since S60 5.0
     * @param aNvram
     * @param aNvramSlot
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt WriteNvramStatusToClient( const TDesC8& aNvram,
                                   TInt aNvramSlot );

    /**
     * Reads a string from a message
     *
     * @since S60 5.0
     * @param aBuffer Destination buffer to which to read the string
     * @param aDataSlot Slot for the aMessage from where to read the string
     * @param aMessage Message from where the string is to be read
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ReadStringFromMessage( RBuf8& aBuffer,
                                TInt aDataSlot,
                                const RMessage2& aMessage );

    /**
     * Reads a struct from a message
     *
     * @since S60 5.0
     * @param aBuffer Destination buffer to which to read the struct
     * @param aDataSlot Slot for the aMessage from where to read the struct
     * @param aMessage Message from where the struct is to be read
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ReadStructFromMessage( TDes8& aBuffer,
                                TInt aDataSlot,
                                const RMessage2& aMessage );

    /*
     * Writes specified input reply buffer to a client request message at
     * given message slot number.
     *
     * @since S60 5.0
     * @param aBuffer Input buffer to write to aDataSlot
     * @param aDataSlot Message slot where to write the buffer
     * @param aMessage Client request message for writing
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt WriteReplyBufferToClient( const TDesC8& aBuffer,
                                   TInt aDataSlot,
                                   const RMessage2& aMessage );

    /**
     * Destructs objects/associations for the current session
     *
     * @since S60 5.0
     * @param aSyncClose ETrue if synchronous close, EFalse if Close()
     * @return None
     */
    void Destruct( TBool aSyncClose );

// from base class MATExtCommonObserver

    /**
     * From MATExtCommonObserver.
     * Called by concrete extension Plugin to inform that mode status has
     * changed.
     *
     * @since S60 5.0
     * @param aMode New mode
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt SendModeStatusChange( TUint aMode );

    /**
     * From MATExtCommonObserver.
     * Called by concrete extension plugin to inform that NVRAM status has
     * changed.
     *
     * @since S60 5.0
     * @param aNvram New NVRAM status. The new settings (delimited by "|")
     *               must differ from the Hayes defaults.
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt SendNvramStatusChange( const TDesC8& aNvram );

    /**
     * From MATExtCommonObserver.
     * Called by the destructor of CATExtPluginBase.
     * A concrete service provider implementation should not touch this.
     *
     * @param aPlugin Plugin that was closed
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ATExtPluginClosed( CATExtCommonBase* aPlugin );

private:  // data

    /**
     * The server
     */
    CATExtSrvCommon& iServer;

    /**
     * Session to ECOM
     */
    REComSession* iEComSession;

    /**
     * Instance of plugin
     */
    CATExtCommonBase* iCommonBase;

    /**
     * Version of server
     */
    TVersion iVersion;

    /**
     * Mode status message
     */
    RMessage2 iModeStatusMessage;

    /**
     * NVRAM status message
     */
    RMessage2 iNvramStatusMessage;

    };

#endif  // C_ATEXTCOMMONSESSION_H
