/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client side interface of ATEXT common functionality
*
*/


#ifndef R_ATEXTCOMMON_H
#define R_ATEXTCOMMON_H

#include <e32base.h>
#include <atextcommonbase.h>

/**
 *  Client side interface of ATEXT common functionality
 *
 *  @lib atextcommon.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RATExtCommon ) : public RSessionBase
    {

public:

    /**
     * Connects to ATEXT server and creates a new session
     *
     * @since S60 5.0
     * @param aName Connection identifier name
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt Connect( const TDesC8& aName );

    /**
     * Synchronously closes the session
     * Optional: client can do either Close() or SynchronousClose()+Close()
     *
     * @since S60 v5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt SynchronousClose();

    /**
     * Gets current mode status
     *
     * @since S60 5.0
     * @param aMode Current mode (see explanation in atextcommonbase.h)
     * @param aMask Mask for current mode
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt GetMode( TUint& aMask, TUint& aMode );

    /**
     * Receives mode status change
     *
     * @since S60 5.0
     * @param aStatus The request status
     * @param aMode Current mode (see explanation in atextcommonbase.h)
     *              This must be of type TPckgBuf<TUint>
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReceiveModeStatusChange( TRequestStatus& aStatus,
                                           TPckg<TUint>& aMode );

    /**
     * Cancels a pending receiving request
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt CancelReceiveModeStatusChange();

    /**
     * Gets current NVRAM status
     *
     * @since S60 5.0
     * @param aNvram Current NVRAM status. The current settings
     *               (delimited by "|") must differ from the Hayes
     *               defaults.
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt GetNvramStatus( TBuf8<KDefaultNvramBufLength>& aNvram );

    /**
     * Receives NVRAM status change
     *
     * @since S60 5.0
     * @param aStatus The request status
     * @param aNvram New NVRAM status. The new settings (delimited by "|")
     *               must differ from the defaults.
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReceiveNvramStatusChange( TRequestStatus& aStatus,
                                            TBuf8<KDefaultNvramBufLength>& aNvram );

    /**
     * Cancels a pending NVRAM receive request
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt CancelReceiveNvramStatusChange();

    };

#endif  // R_ATEXTCOMMON_H
