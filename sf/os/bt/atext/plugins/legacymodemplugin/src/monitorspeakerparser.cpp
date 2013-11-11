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
* Description: Handles commands "ATL", "ATL0", "ATL1", "ATL2", "ATL3", "ATM",
*              "ATM0", "ATM1" and "ATM2"
*
*/

#include "monitorspeakerparser.h"
#include "debug.h"

_LIT8( KAtlCmd,  "ATL"  );
_LIT8( KAtl0Cmd, "ATL0" );
_LIT8( KAtl1Cmd, "ATL1" );
_LIT8( KAtl2Cmd, "ATL2" );
_LIT8( KAtl3Cmd, "ATL3" );
_LIT8( KAtmCmd,  "ATM"  );
_LIT8( KAtm0Cmd, "ATM0" );
_LIT8( KAtm1Cmd, "ATM1" );
_LIT8( KAtm2Cmd, "ATM2" );

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMonitorSpeakerParser* CMonitorSpeakerParser::NewL(
    MLegacyModemPlugin* aCallback )
    {
    CMonitorSpeakerParser* self = new (ELeave) CMonitorSpeakerParser( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMonitorSpeakerParser::~CMonitorSpeakerParser()
    {
    }

// ---------------------------------------------------------------------------
// CHfpExtParserMisc::CHfpExtParserMisc
// ---------------------------------------------------------------------------
//
CMonitorSpeakerParser::CMonitorSpeakerParser( MLegacyModemPlugin* aCallback ) :
    iCallback( aCallback )
    {
    iCmdHandlerType = ECmdHandlerTypeUndefined;
    iDetectedCmd = EDetectedCmdUndefined;
    }

// ---------------------------------------------------------------------------
// CHfpExtParserMisc::ConstructL
// ---------------------------------------------------------------------------
//
void CMonitorSpeakerParser::ConstructL()
    {
    if ( !iCallback )
        {
        User::Leave( KErrGeneral );
        }
    }

// ---------------------------------------------------------------------------
// Checks if the command is speaker loudness command
// ---------------------------------------------------------------------------
//
TBool CMonitorSpeakerParser::CheckSpeakerLoudnessCommand( const TDesC8& aCmd )
    {
    TRACE_FUNC_ENTRY
    // The ATL command only supports one type: base
    // The ATL0 command only supports one type: base
    // The ATL1 command only supports one type: base
    // The ATL2 command only supports one type: base
    // The ATL3 command only supports one type: base
    iCmdHandlerType = iCallback->CheckCommandType( KAtlCmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATL;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    iCmdHandlerType = iCallback->CheckCommandType( KAtl0Cmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATL0;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    iCmdHandlerType = iCallback->CheckCommandType( KAtl1Cmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATL1;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    iCmdHandlerType = iCallback->CheckCommandType( KAtl2Cmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATL2;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    iCmdHandlerType = iCallback->CheckCommandType( KAtl3Cmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATL3;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Checks if the command is speaker mode command
// ---------------------------------------------------------------------------
//
TBool CMonitorSpeakerParser::CheckSpeakerModeCommand( const TDesC8& aCmd )
    {
    TRACE_FUNC_ENTRY
    // The ATM command only supports one type: base
    // The ATM0 command only supports one type: base
    // The ATM1 command only supports one type: base
    // The ATM2 command only supports one type: base
    iCmdHandlerType = iCallback->CheckCommandType( KAtmCmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATM;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    iCmdHandlerType = iCallback->CheckCommandType( KAtm0Cmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATM0;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    iCmdHandlerType = iCallback->CheckCommandType( KAtm1Cmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATM1;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    iCmdHandlerType = iCallback->CheckCommandType( KAtm2Cmd, aCmd );
    if ( iCmdHandlerType == ECmdHandlerTypeBase )
        {
        iDetectedCmd = EDetectedCmdATM2;
        TRACE_FUNC_EXIT
        return ETrue;
        }
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Main command handler (leaving version)
// ---------------------------------------------------------------------------
//
TBool CMonitorSpeakerParser::HandleCommandL()
    {
    TRACE_FUNC_ENTRY
    TBool cmdUnderstood = EFalse;
    // Just return "OK" for all detected commands
    if ( iDetectedCmd != EDetectedCmdUndefined )
        {
        iCallback->CreateReplyAndComplete( EReplyTypeOk );
        cmdUnderstood = ETrue;
        }
    TRACE_FUNC_EXIT
    return cmdUnderstood;
    }

// ---------------------------------------------------------------------------
// From CLegacyModemPluginBase.
// Reports the support status of an AT command. This is a synchronous API.
// ---------------------------------------------------------------------------
//
TBool CMonitorSpeakerParser::IsCommandSupported( const TDesC8& aCmd )
    {
    TRACE_FUNC_ENTRY
    TBool cmdUnderstood = EFalse;
    // Set all to undefined if either the command or its type is unknown
    // HandleCommand() should be round only when both are set
    cmdUnderstood = CheckSpeakerLoudnessCommand( aCmd );
    if ( cmdUnderstood )
        {
        TRACE_FUNC_EXIT
        return ETrue;
        }
    cmdUnderstood = CheckSpeakerModeCommand( aCmd );
    if ( cmdUnderstood )
        {
        TRACE_FUNC_EXIT
        return ETrue;
        }
    iCmdHandlerType = ECmdHandlerTypeUndefined;
    iDetectedCmd = EDetectedCmdUndefined;
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From CLegacyModemPluginBase.
// Handles an AT command. Cancelling of the pending request is done by
// HandleCommandCancel(). The implementation in the extension plugin should
// be asynchronous.
// ---------------------------------------------------------------------------
//
void CMonitorSpeakerParser::HandleCommand( const TDesC8& /*aCmd*/,
                                           RBuf8& /*aReply*/,
                                           TBool aReplyNeeded )
    {
    TRACE_FUNC_ENTRY
    if ( !aReplyNeeded )
        {
        TRACE_FUNC_EXIT
        return;
        }
    // Checking the iDetectedCmd here means that the failure is the
    // failing cmd type (set/read/test) for the command, not the command
    // itself (when cmdUnderstood is EFalse)
    TBool cmdUnderstood = EFalse;
    TRAPD( retTrap, cmdUnderstood=HandleCommandL() )
    if ( !cmdUnderstood || retTrap!=KErrNone )
        {
        iCallback->CreateReplyAndComplete( EReplyTypeError );
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// From CLegacyModemPluginBase.
// Cancels a pending HandleCommand request.
// ---------------------------------------------------------------------------
//
void CMonitorSpeakerParser::HandleCommandCancel()
    {
    TRACE_FUNC_ENTRY
    TRACE_FUNC_EXIT
    }
