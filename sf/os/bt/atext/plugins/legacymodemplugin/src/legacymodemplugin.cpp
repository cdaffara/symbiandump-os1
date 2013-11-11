/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main handler for incoming requests
*
*/

#include "legacymodemplugin.h"
#include "monitorspeakerparser.h"
#include "debug.h"

const TInt KErrorReplyLength = 9;  // CR+LF+"ERROR"+CR+LF

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CLegacyModemPlugin* CLegacyModemPlugin::NewL()
    {
    CLegacyModemPlugin* self = new (ELeave) CLegacyModemPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CLegacyModemPlugin::~CLegacyModemPlugin()
	{
	iHandlers.ResetAndDestroy();
	iHandlers.Close();
    iReplyBuffer.Close();
	}

// ---------------------------------------------------------------------------
// CLegacyModemPlugin::CLegacyModemPlugin
// ---------------------------------------------------------------------------
//
CLegacyModemPlugin::CLegacyModemPlugin() : CATExtPluginBase()
    {
    iHandler = NULL;
    iHcCmd = NULL;
    iHcReply = NULL;
    }

// ---------------------------------------------------------------------------
// CLegacyModemPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CLegacyModemPlugin::ConstructL()
    {
    CLegacyModemPluginBase* handler = NULL;
    handler = CMonitorSpeakerParser::NewL( this );
    CleanupStack::PushL( handler );
    iHandlers.AppendL( handler );
    CleanupStack::Pop( handler );
   	}

// ---------------------------------------------------------------------------
// Reports connection identifier name to the extension plugin.
// ---------------------------------------------------------------------------
//
void CLegacyModemPlugin::ReportConnectionName( const TDesC8& /*aName*/ )
    {
    }

// ---------------------------------------------------------------------------
// Reports the support status of an AT command. This is a synchronous API.
// ---------------------------------------------------------------------------
//
TBool CLegacyModemPlugin::IsCommandSupported( const TDesC8& aCmd )
    {
    TRACE_FUNC_ENTRY
    iHcCmd = NULL;
    iHcReply = NULL;
    TInt i;
    TInt count = iHandlers.Count();
    for ( i=0; i<count; i++ )
        {
        CLegacyModemPluginBase* handler = iHandlers[i];
        TBool supported = handler->IsCommandSupported( aCmd );
        if ( supported )
            {
            iHandler = handler;
            TRACE_FUNC_EXIT
            return ETrue;
            }
        }
    iHandler = NULL;
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Handles an AT command. Cancelling of the pending request is done by
// HandleCommandCancel(). The implementation in the extension plugin should
// be asynchronous.
// ---------------------------------------------------------------------------
//
void CLegacyModemPlugin::HandleCommand( const TDesC8& aCmd,
                                        RBuf8& aReply,
                                        TBool aReplyNeeded )
	{
	TRACE_FUNC_ENTRY
	if ( iHandler )
	    {
	    iHcCmd = &aCmd;
	    iHcReply = &aReply;
	    iHandler->HandleCommand( aCmd, aReply, aReplyNeeded );
	    }
	TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Cancels a pending HandleCommand request.
// ---------------------------------------------------------------------------
//
void CLegacyModemPlugin::HandleCommandCancel()
    {
    TRACE_FUNC_ENTRY
	if ( iHandler )
	    {
	    iHandler->HandleCommandCancel();
	    }
	TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Next reply part's length.
// The value must be equal or less than KDefaultCmdBufLength.
// When the reply from this method is zero, ATEXT stops calling
// GetNextPartOfReply().
// ---------------------------------------------------------------------------
//
TInt CLegacyModemPlugin::NextReplyPartLength()
    {
    TRACE_FUNC_ENTRY
    if ( iReplyBuffer.Length() < KDefaultCmdBufLength )
        {
        TRACE_FUNC_EXIT
        return iReplyBuffer.Length();
        }
    TRACE_FUNC_EXIT
    return KDefaultCmdBufLength;
    }

// ---------------------------------------------------------------------------
// Gets the next part of reply initially set by HandleCommandComplete().
// Length of aNextReply must be equal or less than KDefaultCmdBufLength.
// ---------------------------------------------------------------------------
//
TInt CLegacyModemPlugin::GetNextPartOfReply( RBuf8& aNextReply )
    {
    TRACE_FUNC_ENTRY
    TInt retVal = CreatePartOfReply( aNextReply );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Receives unsolicited results. Cancelling of the pending request is done by
// by ReceiveUnsolicitedResultCancel(). The implementation in the extension
// plugin should be asynchronous.
// ---------------------------------------------------------------------------
//
void CLegacyModemPlugin::ReceiveUnsolicitedResult()
    {
    TRACE_FUNC_ENTRY
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Cancels a pending ReceiveUnsolicitedResult request.
// ---------------------------------------------------------------------------
//
void CLegacyModemPlugin::ReceiveUnsolicitedResultCancel()
    {
    TRACE_FUNC_ENTRY
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Reports NVRAM status change to the plugins.
// ---------------------------------------------------------------------------
//
void CLegacyModemPlugin::ReportNvramStatusChange( const TDesC8& /*aNvram*/ )
    {
    TRACE_FUNC_ENTRY
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Reports about external handle command error condition.
// This is for cases when for example DUN decided the reply contained an
// error condition but the plugin is still handling the command internally.
// Example: "AT+TEST;+TEST2" was given in command line; "AT+TEST" returns
// non-EReplyTypeError condition and "AT+TEST2" returns EReplyTypeError.
// As the plugin(s) returning the non-EReplyTypeError may still have some
// ongoing operation then these plugins are notified about the external
// EReplyTypeError in command line processing. It is to be noted that
// HandleCommandCancel() is not sufficient to stop the processing as the
// command handling has already finished.
// ---------------------------------------------------------------------------
//
void CLegacyModemPlugin::ReportExternalHandleCommandError()
    {
    TRACE_FUNC_ENTRY
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Creates part of reply from the global reply buffer to the destination
// buffer. Used with APIs which need the next part of reply in multipart reply
// requests.
// ---------------------------------------------------------------------------
//
TInt CLegacyModemPlugin::CreatePartOfReply( RBuf8& aDstBuffer )
    {
    TRACE_FUNC_ENTRY
    if ( iReplyBuffer.Length() <= 0 )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt partLength = NextReplyPartLength();
    if ( iReplyBuffer.Length() < partLength )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    aDstBuffer.Create( iReplyBuffer, partLength );
    iReplyBuffer.Delete( 0, partLength );
    if ( iReplyBuffer.Length() == 0 )
        {
        iReplyBuffer.Close();
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Creates an AT command reply based on the reply type and completes the
// request to ATEXT. Uses iReplyBuffer for reply storage.
// ---------------------------------------------------------------------------
//
TInt CLegacyModemPlugin::CreateReplyAndComplete(
    TATExtensionReplyType aReplyType,
    const TDesC8& aSrcBuffer,
    TInt aError )
    {
    TRACE_FUNC_ENTRY
    iReplyBuffer.Close();
    if ( aError != KErrNone )
        {
        HandleCommandCompleted( aError, EReplyTypeUndefined );
        iHcCmd = NULL;
        iHcReply = NULL;
        TRACE_FUNC_EXIT
        return KErrNone;
        }
    if ( !iHcReply )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    switch ( aReplyType )
        {
        case EReplyTypeOther:
            if ( iQuietMode || !iVerboseMode )
                {
                iReplyBuffer.Create( KNullDesC8 );
                }
            else
                {
                iReplyBuffer.Create( aSrcBuffer );
                }
            break;
        case EReplyTypeOk:
            CreateOkOrErrorReply( iReplyBuffer, ETrue );
            break;
        case EReplyTypeError:
            CreateOkOrErrorReply( iReplyBuffer, EFalse );
            break;
        default:
            TRACE_FUNC_EXIT
            return KErrGeneral;
        }
    CreatePartOfReply( *iHcReply );
    HandleCommandCompleted( KErrNone, aReplyType );
    iHcCmd = NULL;
    iHcReply = NULL;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Creates a buffer for "OK" or "ERROR" reply based on the line settings
// ---------------------------------------------------------------------------
//
TInt CLegacyModemPlugin::CreateOkOrErrorReply( RBuf8& aReplyBuffer,
                                               TBool aOkReply )
    {
    TRACE_FUNC_ENTRY
    if ( iQuietMode )
        {
        TRACE_FUNC_EXIT
        return iReplyBuffer.Create( KNullDesC8 );
        }
    _LIT8( KErrorReplyVerbose, "ERROR" );
    _LIT8( KOkReplyVerbose,    "OK" );
    _LIT8( KErrorReplyNumeric, "4" );
    _LIT8( KOkReplyNumeric,    "0" );
    TBuf8<KErrorReplyLength> replyBuffer;
    if ( iVerboseMode )
        {
        replyBuffer.Append( iCarriageReturn );
        replyBuffer.Append( iLineFeed );
        if ( aOkReply )
            {
            replyBuffer.Append( KOkReplyVerbose );
            }
        else
            {
            replyBuffer.Append( KErrorReplyVerbose );
            }
        replyBuffer.Append( iCarriageReturn );
        replyBuffer.Append( iLineFeed );
        }
    else
        {
        if ( aOkReply )
            {
            replyBuffer.Append( KOkReplyNumeric );
            }
        else
            {
            replyBuffer.Append( KErrorReplyNumeric );
            }
        replyBuffer.Append( iCarriageReturn );
        }
    TInt retVal = aReplyBuffer.Create( replyBuffer );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// From MHfpExtPlugin.
// Checks if the command is a base, set, read or test type of command
// ---------------------------------------------------------------------------
//
TCmdHandlerType CLegacyModemPlugin::CheckCommandType( const TDesC8& aCmdBase,
                                                      const TDesC8& aCmdFull )
    {
    TRACE_FUNC_ENTRY
    TInt retTemp = KErrNone;
    TBuf8<KDefaultCmdBufLength> atCmdBuffer;
    atCmdBuffer.Copy( aCmdBase );
    // Check "base" command ("AT+COMMAND")
    retTemp = aCmdFull.Compare( atCmdBuffer );
    if ( retTemp == 0 )
        {
        TRACE_FUNC_EXIT
        return ECmdHandlerTypeBase;
        }
    // Check "read" command ("AT+COMMAND?")
    // Add last question mark
    atCmdBuffer.Append( '?' );
    retTemp = aCmdFull.Compare( atCmdBuffer );
    if ( retTemp == 0 )
        {
        TRACE_FUNC_EXIT
        return ECmdHandlerTypeRead;
        }
    // Check "test" command ("AT+COMMAND=?")
    // Add "=" before the question mark
    _LIT8( KAssignmentMark, "=" );
    atCmdBuffer.Insert( atCmdBuffer.Length()-1, KAssignmentMark );
    retTemp = aCmdFull.Compare( atCmdBuffer );
    if ( retTemp == 0 )
        {
        TRACE_FUNC_EXIT
        return ECmdHandlerTypeTest;
        }
    // Check "set" command ("AT+COMMAND=")
    // Remove last question mark
    atCmdBuffer.SetLength( atCmdBuffer.Length() - 1 );
    retTemp = aCmdFull.Compare( atCmdBuffer );
    if ( retTemp == 0 )
        {
        TRACE_FUNC_EXIT
        return ECmdHandlerTypeSet;
        }
    TRACE_FUNC_EXIT
    return ECmdHandlerTypeUndefined;
    }
