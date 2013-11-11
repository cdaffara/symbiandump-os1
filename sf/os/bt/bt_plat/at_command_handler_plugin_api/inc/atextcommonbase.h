/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AT extension common plugin API
*
*/


#ifndef C_ATEXTCOMMONBASE_H
#define C_ATEXTCOMMONBASE_H

#include <ecom/ecom.h>

class MATExtCommonObserver;

/**
 * The mode notification can specify either multiple modes or one character
 * change, as follows:
 *
 *           KModeStatus  KModeChanged
 *            /------\      /------\
 * (MSB)      XXXXXXXX      XXXXXXXX     XXXXXXXX    (LSB)
 *
 * The 8 LSB bits (KModeChanged-1) specifies either:
 * - 1 or more mode change:
 *       KCommandModeChanged,
 *       KEchoModeChanged,
 *       KQuietModeChanged,
 *       KVerboseModeChanged, OR
 * - exactly 1 character change:
 *       KCarriageChanged,
 *       KLineFeedChanged,
 *       KBackspaceChanged
 * If the change is a mode change: The changed value is in:
 *       KCommandModeBase,
 *       KEchoModeBase,
 *       KQuietModeBase, or
 *       KVerboseModeBase, instead:
 * If the change is a character change: The changed character is in 8 LSB bits.
 *     Note that KCarriageCharBase, KLineFeedCharBase and KBackspaceCharBase
 *     are only for internal use in this case.
 */

/**  Default buffer length for NVRAM status */
const TUint KDefaultNvramBufLength = 512;

/**  Supported mode areas for ReceiveModeStatusChange() */
const TUint KModeChanged        = 0x100;   // Skips the first 0xFF
const TUint KModeStatus         = 0x100;   // Skips the second 0xFF

/**  Different modes for ReceiveModeStatusChange() */
const TUint KCommandModeBase    = 0x01;   // 0000001: For command mode
const TUint KEchoModeBase       = 0x02;   // 0000010: For echo mode (ATE and similar)
const TUint KQuietModeBase      = 0x04;   // 0000100: For quiet mode (ATQ and similar)
const TUint KVerboseModeBase    = 0x08;   // 0001000: For DCE mode (ATV and similar)
const TUint KCarriageCharBase   = 0x10;   // 0010000: For carriage return character
const TUint KLineFeedCharBase   = 0x20;   // 0100000: For line feed character
const TUint KBackspaceCharBase  = 0x40;   // 1000000: For backspace character

/**  Changed mode masks for ReceiveModeStatusChange() */
const TUint KCommandModeChanged = KCommandModeBase   * KModeChanged;
const TUint KEchoModeChanged    = KEchoModeBase      * KModeChanged;
const TUint KQuietModeChanged   = KQuietModeBase     * KModeChanged;
const TUint KVerboseModeChanged = KVerboseModeBase   * KModeChanged;
const TUint KCarriageChanged    = KCarriageCharBase  * KModeChanged;
const TUint KLineFeedChanged    = KLineFeedCharBase  * KModeChanged;
const TUint KBackspaceChanged   = KBackspaceCharBase * KModeChanged;

/**  KModeStatus leaves 0xFF values for mode */
const TUint KModeCommand        = KCommandModeBase   * KModeStatus * KModeChanged;
const TUint KModeEcho           = KEchoModeBase      * KModeStatus * KModeChanged;
const TUint KModeQuiet          = KQuietModeBase     * KModeStatus * KModeChanged;
const TUint KModeVerbose        = KVerboseModeBase   * KModeStatus * KModeChanged;
const TUint KModeCarriage       = KCarriageCharBase  * KModeStatus * KModeChanged;
const TUint KModeLineFeed       = KLineFeedCharBase  * KModeStatus * KModeChanged;
const TUint KModeBackspace      = KBackspaceCharBase * KModeStatus * KModeChanged;
const TUint KSupportedModes     = KModeCommand  | KModeEcho     | KModeQuiet    |
                                  KModeVerbose  | KModeCarriage | KModeLineFeed |
                                  KModeBackspace;

/**
 *  AT extension common plugin API
 *
 *  The base class for implementing AT Extension Plugin.
 *
 *  A concrete AT Extension Plugin must implement this class as an ECom plugin,
 *  and it must be ready once it has been instantiated.
 *
 *  The interface UID of the plugin resource must use the value defined in
 *  atext_interfaceuid.h.
 *
 *  @lib atextcommonbase.lib
 *  @since S60 v5.0
 */
class CATExtCommonBase : public CBase
    {

public:

    /**
     * Instantiate the AT Extension Plugin via ECom framework.
     *
     * @since S60 5.0
     * @param aImplementationUId The implementation UID of the plugin
     * @param aObserver The reference of ATEXT interface.
     * @param aName Connection name to report to extension plugin
     * @return Pointer of the instantiated plugin
     */
	IMPORT_C static CATExtCommonBase* NewL( TUid aImplementationUid,
	                                        MATExtCommonObserver& aObserver,
	                                        const TDesC8& aName );

    /**
    * Destructor
    */
    IMPORT_C virtual ~CATExtCommonBase();

    /**
     * Reports connection identifier name to the extension plugin.
     *
     * @since S60 5.0
     * @param aName Connection identifier name
     * @return None
     */
    virtual void ReportConnectionName( const TDesC8& aName ) = 0;

    /**
     * Gets current mode status. This is a synchronous version of
     * ReceiveModeStatusChange() and the extension plugin must set aMode
     * parameter to the current mode status before this function returns.
     *
     * @since S60 5.0
     * @param aMask Mask for current mode
     * @param aMode Current mode (see explanation in this file)
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt GetMode( TUint aMask, TUint& aMode ) = 0;

    /**
     * Receives mode status change. Cancelling of the pending request is
     * done by CancelReceiveModeStatusChange(). The implementation in the
     * extension plugin should be asynchronous.
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt ReceiveModeStatusChange() = 0;

    /**
     * Cancels a pending mode receiving request.
     *
     * @since S60 5.0
     * @return None
     */
    virtual void CancelReceiveModeStatusChange() = 0;

    /**
     * Gets current NVRAM status. This is a synchronous version of
     * ReceiveNvramStatusChange() and the extension plugin must set aNvram
     * parameter to the current NVRAM status before this function returns.
     *
     * @since S60 5.0
     * @param aNvram Current NVRAM status. The current settings
     *               (delimited by "|") must differ from the Hayes
     *               defaults.
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt GetNvramStatus( RBuf8& aNvram ) = 0;

    /**
     * Receives NVRAM status change. Cancelling of the pending request is done
     * by CancelReceiveNvramStatusChange(). The implementation in the extension
     * plugin should be asynchronous.
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt ReceiveNvramStatusChange() = 0;

    /**
     * Cancels a pending ReceiveNvramStatusChange() NVRAM status change receive
     * request.
     *
     * @since S60 5.0
     * @return None
     */
    virtual void CancelReceiveNvramStatusChange() = 0;

protected:

    /**
     * Called by concrete extension plugin to inform that mode status has
     * changed. Instructs ATEXT to complete client request message for
     * ReceiveModeStatusChange().
     *
     * @since S60 5.0
     * @param aMode New mode
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt SendModeStatusChange( TUint aMode );

    /**
     * Called by concrete extension plugin to inform that NVRAM status has
     * changed. Instructs ATEXT to complete client request message for
     * ReceiveNvramStatusChange().
     *
     * @since S60 5.0
     * @param aNvram New NVRAM status. The new settings (delimited by "|")
     *               must differ from the Hayes defaults.
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt SendNvramStatusChange( const TDesC8& aNvram );

private:

    /**
     * Sets the observer.
     *
     * @since S60 5.0
     * @param aObserver the observer.
     * @return None
     */
    IMPORT_C void SetObserver( MATExtCommonObserver& aObserver );

private:  // data

    /**
     * The reference of ATEXT interface.
     */
    MATExtCommonObserver* iObserver;

    /**
     * UID set by ECOM when the instance is created.
     * Used when the instance is destroyed.
     */
    TUid iInstanceUid;

    };

#endif  // C_ATEXTCOMMONBASE_H
