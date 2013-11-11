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
* Description:  Metadata for ATEXT
*
*/


/*
 * Here is the plugin managing logic for the AT commands and hopefully also for
 * future needs:
 *
 * Three types of support:
 * 1) Master (M): Sends to all O if support found. Panics if > 1 M.
 * 2) Partial (P): Sends to all O if support found. Replies "ERROR" if two P supports.
 * 3) Observer (O): Process the command and give or not give reply, based on
 *    the following logic:
 *
 * => [If] M found, handle command and send reply + send to N O {no reply}, stop, [else]
 * [If] P found, handle command and send reply + send to N O {no reply}, stop, [else]
 * [If] > 1 O found, send to N O {no reply}, stop, [else]
 * [If] only 1 O found, handle command and send reply, stop, [else]
 * Write "ERROR" to client, complete message with KErrNone
 *
 * When incoming reply:
 * => If reply from M, write to client, stop, [else]
 * If reply from P, write to client, stop, [else]
 * If reply from O and M, P nor other O exist, write to client, stop, [else]
 * Complete message with KErrNone and empty string
 *
 * Note: Empty string and "ERROR" string are managed already in HandleCommand()
 */

#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <atextpluginbase.h>
#include "atextclientsrv.h"
#include "atextmetadata.h"
#include "atextlisten.h"
#include "utils.h"
#include "debug.h"

const TInt  KGranularity           = 4;
const TInt8 KDefaultCarriageReturn = 13;
const TInt8 KDefaultLineFeed       = 10;
const TInt  KErrorMsgLen           = 9;  // 2+"ERROR"+2

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CATExtMetadata* CATExtMetadata::NewL( REComSession& aEComSession,
                                      CATExtListen* aListener,
                                      MATExtPluginObserver& aObserver )
    {
    CATExtMetadata* self = NewLC( aEComSession, aListener, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CATExtMetadata* CATExtMetadata::NewLC( REComSession& aEComSession,
                                       CATExtListen* aListener,
                                       MATExtPluginObserver& aObserver )
    {
    CATExtMetadata* self = new (ELeave) CATExtMetadata( aEComSession,
                                                        aListener,
                                                        aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CATExtMetadata::~CATExtMetadata()
    {
    ResetData();
    }

// ---------------------------------------------------------------------------
// Resets data to initial values
// ---------------------------------------------------------------------------
//
void CATExtMetadata::ResetData()
    {
    TRACE_FUNC_ENTRY
    iShutdown = ETrue;
    DestroyPlugindata();
    DestroySupportdata( iSupport );
    DestroySupportdata( iSupportAux );
    iConnectionName.Close();
    iShutdown = EFalse;
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Creates new implementation metadata based on a given interface UID and
// connection. Uses AddImplementationL() to add new implementations for the
// interface and sets connection identification name for reporting it to the
// plugins on instantiation time.
// ---------------------------------------------------------------------------
//
void CATExtMetadata::CreateImplementationMetadataL( TUid& aIfUid,
                                                    const TDesC8& aName )
    {
    TRACE_FUNC_ENTRY
    if ( iPluginData || iSupport || iSupportAux )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    iIfUid = aIfUid;
    iConnectionName.CreateL( aName );
    RImplInfoPtrArray implementations;
    CleanupResetDestroyClosePushL( implementations );
    iEComSession.ListImplementationsL( iIfUid, implementations );
    const TUint implCount = implementations.Count();
    TRACE_INFO((_L("Number of AT Ext: %d"), implCount))
    iSupport = new (ELeave) CArrayFixFlat<TATExtAtCmdSupport>( KGranularity );
    iSupportAux = new (ELeave) CArrayFixFlat<TATExtAtCmdSupport>( KGranularity );
    iPluginData = new (ELeave) CArrayFixFlat<TATExtPluginEntry>( KGranularity );
    // As iSupport, iSupportAux and iPluginData are needed by
    // AddImplementationL(), no ease way to use temporary variables here.
    // If error occurs here then deletion of this class is required.
    for ( TUint i=0; i<implCount; i++ )
        {
        AddImplementationL( implementations[i] );
        }
    CleanupStack::PopAndDestroy( &implementations );
#if defined(_DEBUG) && defined( PRJ_PRINT_SUPPORT_DATA )
    PrintSupportData( iSupport );
    PrintSupportData( iSupportAux );
#endif
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Adds an implementation to the metadata based on a given ECOM implementation
// information. This is used by the function CreateImplementationMetadataL()
// and also by CATExtSession when its ECOM notification listener detects an
// addition for a the same ECOM interface.
// ---------------------------------------------------------------------------
//
void CATExtMetadata::AddImplementationL( CImplementationInformation* aImplInfo )
    {
    TRACE_FUNC_ENTRY
    if ( !aImplInfo )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    DoAddImplementationL( aImplInfo, aImplInfo->DataType() );
    DoAddImplementationL( aImplInfo, aImplInfo->OpaqueData() );
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Removes an implementation from the support data by a given plugin UID.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::RemoveImplementation( TUid& aPluginUid,
                                           TBool aInstanceExists )
    {
    TRACE_FUNC_ENTRY
    if ( iShutdown )
        {
        TRACE_FUNC_EXIT
        return KErrNotReady;
        }
    DoRemoveImplementation( aPluginUid, iSupport );
    DoRemoveImplementation( aPluginUid, iSupportAux );
    TInt retVal = RemoveOnePlugindata( aPluginUid, aInstanceExists );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Handles an AT command
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::HandleCommand( const RMessage2& aMessage,
                                    TATExtCompletionInfo& aComplInfo )
    {
    TRACE_FUNC_ENTRY
    iCmdData.iCmdMessage = aMessage;
    if ( IsCommandHandling() )
        {
        TRACE_FUNC_EXIT
        return KErrInUse;
        }
    // Next pass the entry to command reader
    TInt retTemp = ReadCommandFromMessage( aMessage );
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        return retTemp;
        }
    // First check if in editor mode
    if ( iCmdData.iEditorHandler )
        {
        iCmdData.iReplyExpected = EFalse;
        iCmdData.iHandler = iCmdData.iEditorHandler;
        iCmdData.iHandler->iInstance->HandleCommand( iCmdData.iCmdBuffer,
                                                     iCmdData.iCmdReplyBuffer,
                                                     EFalse );
        aComplInfo.iProcessed = ETrue;
        aComplInfo.iReplyExpected = ETrue;
        // Note: The aComplInfo.iReplyExpected is used only for normal mode and
        // is set to ETrue here to skip a check in CATExtSession::IpcHandleCommand().
        TRACE_FUNC_EXIT
        return KErrNone;
        }
    // Not in editor so handle in normal mode
    TRACE_INFO(( _L8("Received command '%S'"), &iCmdData.iCmdBuffer ));
    // Now the command exists. Load the plugins for a command and check support.
    TRAPD( retTrap, CreateAndFindSupportL(iCmdData.iCmdBuffer,
                                          aMessage,
                                          aComplInfo ) );
    TRACE_FUNC_EXIT
    return retTrap;
    }

// ---------------------------------------------------------------------------
// Cancels an active handle command operation. Uses IsCommandHandling() and
// CancelCommandOperation().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::CancelHandleCommand()
    {
    TRACE_FUNC_ENTRY
    if ( !IsCommandHandling() )
        {
        TRACE_FUNC_EXIT
        return KErrNotReady;
        }
    CancelCommandOperation( KErrCancel, ETrue );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Gets the next part of a reply for HandleCommand(). Length of returned reply
// must be the same as the one reported from NextReplyPartLength() for the
// current reply.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::GetNextPartOfReply( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iCmdData.iHandler || !iCmdData.iHandler->iInstance )
        {
        TRACE_FUNC_EXIT
        return KErrNotReady;
        }
    CATExtPluginBase& plugin = *iCmdData.iHandler->iInstance;
    TInt retTemp = plugin.GetNextPartOfReply( iCmdData.iCmdReplyBuffer );
    if ( retTemp!=KErrNone || iCmdData.iCmdReplyBuffer.Length()<=0 )
        {
        iCmdData.iCmdReplyBuffer.Close();
        TRACE_FUNC_EXIT
        return retTemp;
        }
    retTemp = WriteReplyBufferToClient(
        iCmdData.iCmdReplyBuffer,
        EATExtGetNextPartOfReplyParamReply,
        aMessage,
        ETrue,
        EATExtGetNextPartOfReplyParamLength );
    if ( retTemp != KErrNone )
        {
        iCmdData.iCmdReplyBuffer.Close();
        TRACE_FUNC_EXIT
        return retTemp;
        }
    aMessage.Complete( KErrNone );
    iCmdData.iCmdReplyBuffer.Close();
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Completes an URC processing message by a given plugin UID. Completion code
// for the client message is the return code from a write with
// WriteReplyBufferToClient()
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::CompleteUrcMessage( const TDesC8& aAtCmd,
                                         CATExtPluginBase* aPlugin )
    {
    TRACE_FUNC_ENTRY
    TATExtPluginEntry* foundEntry = FindInstanceFromPlugindata( aPlugin );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( !foundEntry->iUrcMessage.Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = WriteReplyBufferToClient( aAtCmd,
                                            EATExtReceiveUrcCmdParamBuf,
                                            foundEntry->iUrcMessage );
    foundEntry->iUrcMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Completes an AT command handling message. Also clears internal initialized
// command hanlder data; see ClearInitializedCmdHandlerData().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::CompleteCommandMessage( CATExtPluginBase* aPlugin,
                                             TInt aError,
                                             TBool aErrorReply,
                                             TATExtensionReplyType aReplyType,
                                             TBool aMultiPart )
    {
    TRACE_FUNC_ENTRY
    // First check if aPlugin is set (the call comes from a plugin and not from
    // ATEXT) and if it is the instance is different than the currently
    // operating one.
    if ( aPlugin && iCmdData.iHandler && iCmdData.iHandler->iInstance!=aPlugin )
        {
        TRACE_FUNC_EXIT
        return KErrInUse;
        }
    if ( !iCmdData.iCmdMessage.Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TBool startOfEditor = EFalse;
    if ( aReplyType == EReplyTypeEditor)
        {
        // If completion is for editor command then set iCmdData.iEditorHandler
        // for the first time only
        if ( !iCmdData.iEditorHandler )
            {
            iCmdData.iEditorHandler = FindInstanceFromPlugindata( aPlugin );
            iCmdData.iReplyExpected = ETrue;  // reply expected when first reply in editor mode
            startOfEditor = ETrue;
            }
        }
    else
        {
        // If completion was something else than EReplyTypeEditor then just end
        // editor mode (no need to check iEditorHandler)
        if ( iCmdData.iEditorHandler )
            {
            iCmdData.iReplyExpected = ETrue;  // reply expected when last reply in editor mode
            }
        iCmdData.iEditorHandler = NULL;
        }
    // Next check if aPlugin is set (the call comes from a plugin and not from
    // ATEXT) and a reply is not needed. In this case do nothing as it is wrong
    // behavior from the plugin (a plugin must not complete messages where no
    // reply is expected; this is done by ATEXT)
    if ( aPlugin && !iCmdData.iReplyExpected && !iCmdData.iEditorHandler )
        {
        TRACE_FUNC_EXIT
        return KErrAlreadyExists;
        }
    // Finally write the data and complete the message
    TPckg<TATExtensionReplyType> replyType( aReplyType );
    TInt writeError = iCmdData.iCmdMessage.Write( EATExtHandleCmdParamReplyType,
                                                  replyType );
    TRACE_INFO(( _L("Write returned %d"), writeError ));
    if ( aError==KErrNone && writeError==KErrNone )
        {
        if ( iCmdData.iHandler )  // Can be NULL when invoked from inside ATEXT
            {
            iCmdData.iOldHandler = iCmdData.iHandler->iInstance;
            }
        if ( !aPlugin )
            {
            CreateEmptyOrErrorBuffer( iCmdData.iCmdReplyBuffer, aErrorReply );
            }
        WriteHandleCmdReplyBuffer( aMultiPart, startOfEditor );
        }
    iCmdData.iCmdStarted = EFalse;
    iCmdData.iReplyExpected = EFalse;
    iCmdData.iCmdMessage.Complete( aError );
    ClearInitializedCmdHandlerData( aMultiPart );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Writes multipart or single part reply buffer to client for handle.
// Used for creating a reply for HandleCommand().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::WriteHandleCmdReplyBuffer( TBool aMultiPart,
                                                TBool aStartOfEditor )
    {
    TRACE_FUNC_ENTRY
    if ( iCmdData.iEditorHandler && !aStartOfEditor )
        {
        TRACE_FUNC_EXIT
        return KErrNotReady;
        }
    if ( aMultiPart )
        {
        WriteReplyBufferToClient( iCmdData.iCmdReplyBuffer,
                                  EATExtHandleCmdParamReply,
                                  iCmdData.iCmdMessage,
                                  ETrue,
                                  EATExtHandleCmdParamLength );
        }
    else
        {
        WriteReplyBufferToClient( iCmdData.iCmdReplyBuffer,
                                  EATExtHandleCmdParamReply,
                                  iCmdData.iCmdMessage );
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Clears internal initialized command handler data. This is currently used
// only by CompleteCommandMessage() and is called when the data is not needed
// anymore. It also prepares the internal data for a new HandleCommand() call.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::ClearInitializedCmdHandlerData( TBool aMultiPart )
    {
    TRACE_FUNC_ENTRY
    if ( iCmdData.iCmdStarted )
        {
        TRACE_FUNC_EXIT
        return KErrNotReady;
        }
    iCmdData.iCmdBuffer.Close();
    iCmdData.iCmdReplyBuffer.Close();
    if ( !aMultiPart )
        {
        iCmdData.iHandler = NULL;
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Gets the array of supported commands.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::GetSupportedCommands( CATExtPluginBase* aPlugin,
                                           RPointerArray<HBufC8>& aCmds )
    {
    TRACE_FUNC_ENTRY
    if ( !iSupport || !iSupportAux )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TATExtPluginEntry* foundEntry = FindInstanceFromPlugindata( aPlugin );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    // Next create the array of supported commands
    TInt retVal = KErrNone;
    TBool firstSearch = ETrue;
    TInt supportFind = KErrNotFound;
    TBool findInAux = EFalse;
    for ( ;; )
        {
        HBufC8* foundCmd = GetNextSupportedCommand( firstSearch,
                                                    supportFind,
                                                    findInAux );
        if ( !foundCmd )
            {
            break;
            }
        retVal = aCmds.Append( foundCmd );
        if ( retVal != KErrNone )
            {
            break;
            }
        }
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Starts URC message receiving for plugin. Note that
// MarkUrcHandlingOwnership() must be called immediately after this in order
// for the messages to receive their destination.
// ---------------------------------------------------------------------------
//
void CATExtMetadata::StartUrcReceivingL( const RMessage2& aMessage,
                                         TUid& aPluginUid )
    {
    TRACE_FUNC_ENTRY
    TATExtPluginEntry* foundEntry = FindUrcProcessingPlugin( aPluginUid );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrNotFound );
        }
    // Note: Let locked plugin pass here
    InstantiatePluginL( *foundEntry );
    foundEntry->iUrcMessage = aMessage;
    foundEntry->iInstance->ReceiveUnsolicitedResult();
    foundEntry->iUrcStarted = ETrue;
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Cancels an active URC message receiving operation by a given plugin UID.
// Uses CancelOneUrcOperation() to cancel if plugin UID found from plugin
// data.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::CancelUrcReceiving( TUid& aPluginUid )
    {
    TRACE_FUNC_ENTRY
    TInt foundIndex = KErrNotFound;
    TATExtPluginEntry* foundEntry = FindUidFromPlugindata( aPluginUid,
                                                           foundIndex );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    CancelOneUrcOperation( *foundEntry, KErrCancel, ETrue );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Marks URC handling ownership for a plugin entry. Call to this function must
// be done immediately after the call to StartUrcReceivingL().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::MarkUrcHandlingOwnership( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iPluginData )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt i = 0;
    TInt count = iPluginData->Count();
    for ( i=0; i<count; i++ )
        {
        TATExtPluginEntry& entry = (*iPluginData)[i];
        if ( entry.iUrcStarted && !entry.iUrcOwned )
            {
            TPckg<TUid> pluginUid( entry.iPluginUid );
            TInt retTemp = aMessage.Write( EATExtMarkUrcHandlingOwnershipParamUid,
                                           pluginUid );
            TRACE_INFO(( _L("Write returned %d"), retTemp ));
            entry.iUrcOwned = ETrue;
            TRACE_FUNC_EXIT
            return KErrNone;
            }
        }
    TRACE_FUNC_EXIT
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Marks the access to a plugin data as "locked". This blocks all operations
// where plugin function calls are to be done.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::LockPluginAccess( TUid& aPluginUid )
    {
    TRACE_FUNC_ENTRY
    TInt foundIndex = KErrNotFound;
    TATExtPluginEntry* foundEntry = FindUidFromPlugindata( aPluginUid,
                                                           foundIndex );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    if ( foundEntry == iCmdData.iHandler )
        {
        CancelCommandOperation( KErrCancel );
        }
    CancelOneUrcOperation( *foundEntry, KErrCancel );
    foundEntry->iLocked = ETrue;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Marks the access to a plugin data as "unlocked". This enables all
// operations where plugin function call are to be done.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::UnlockPluginAccess( TUid& aPluginUid )
    {
    TRACE_FUNC_ENTRY
    TInt foundIndex = KErrNotFound;
    TATExtPluginEntry* foundEntry = FindUidFromPlugindata( aPluginUid,
                                                           foundIndex );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    foundEntry->iLocked = EFalse;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Searches for an instances from the support data's plugin data link and only
// marks the instance as uninitialized. Note that this doesn't try to cancel
// any of the current plugin operations and should be used only when a plugin
// destroys itself.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::RemoveImplementationInstance( CATExtPluginBase* aInstance )
    {
    TRACE_FUNC_ENTRY
    if ( !aInstance )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TATExtPluginEntry* foundEntry = FindInstanceFromPlugindata( aInstance );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    TInt retVal = RemoveImplementation( foundEntry->iPluginUid, EFalse );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Checks if support data has been constructed from the plugin data.
// ---------------------------------------------------------------------------
//
TBool CATExtMetadata::SupportExists()
    {
    TRACE_FUNC_ENTRY
    if ( (iSupport&&iSupport->Count()>0) ||
         (iSupportAux&&iSupportAux->Count()>0) )
        {
        TRACE_FUNC_EXIT
        return ETrue;
        }
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Number of plugins with AT command handling support based on the information
// in RSS files' AT command handling entries. This information is needed to
// instantiate one or more listeners by the user of the client.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::NumberOfPlugins()
    {
    TRACE_FUNC_ENTRY
    if ( !iPluginData )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt pluginCount = iPluginData->Count();
    TRACE_FUNC_EXIT
    return pluginCount;
    }

// ---------------------------------------------------------------------------
// Sets the quiet mode to the required value. After this the mode is reported
// to the plugins with ReportQuietModeChange().
// ---------------------------------------------------------------------------
//
void CATExtMetadata::SetQuietMode( TBool aMode )
    {
    TRACE_FUNC_ENTRY
    iQuietMode = aMode;
    if ( iPluginData )
        {
        TInt i;
        TInt count = iPluginData->Count();
        for ( i=0; i<count; i++ )
            {
            TATExtPluginEntry& entry = (*iPluginData)[i];
            if ( entry.iLocked )
                {
                continue;
                }
            TInt retTemp = InstantiatePlugin( entry );
            if ( retTemp != KErrNone )
                {
                continue;
                }
            // Note: here two reports will be made if the plugin was not
            // instantiated but as the operatio is fast no new parameter will be
            // done to InstantiatePlugin() for instantiation reporting.
            entry.iInstance->ReportQuietModeChange( aMode );
            }
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Sets the verbose mode to the required value. After this the mode is
// reported to the plugins with ReportVerboseModeChange().
// ---------------------------------------------------------------------------
//
void CATExtMetadata::SetVerboseMode( TBool aMode )
    {
    TRACE_FUNC_ENTRY
    iVerboseMode = aMode;
    if ( iPluginData )
        {
        TInt i;
        TInt count = iPluginData->Count();
        for ( i=0; i<count; i++ )
            {
            TATExtPluginEntry& entry = (*iPluginData)[i];
            if ( entry.iLocked )
                {
                continue;
                }
            TInt retTemp = InstantiatePlugin( entry );
            if ( retTemp != KErrNone )
                {
                continue;
                }
            // Note: here two reports will be made if the plugin was not
            // instantiated but as the operatio is fast no new parameter will be
            // done to InstantiatePlugin() for instantiation reporting.
            entry.iInstance->ReportVerboseModeChange( aMode );
            }
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Sets new character value for a carriage return, line feed or backspace
// character. After this its type and value are report to the plugins with
// ReportCharacterChange().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::SetCharacterValue( TATExtensionCharType aCharType,
                                        TInt8 aNewChar )
    {
    TRACE_FUNC_ENTRY
    // First pick up the needed information for ATEXT
    TInt retVal = KErrNone;
    switch ( aCharType )
        {
        case ECharTypeCarriage:
            iCarriageReturn = aNewChar;
            break;
        case ECharTypeLineFeed:
            iLineFeed = aNewChar;
            break;
        case ECharTypeBackspace:
            iBackspace = aNewChar;
            break;
        default:
            retVal = KErrNotSupported;
            break;
        }
    if ( retVal==KErrNone && iPluginData )
        {
        // Next notify about character change
        TInt i;
        TInt count = iPluginData->Count();
        for ( i=0; i<count; i++ )
            {
            TATExtPluginEntry& entry = (*iPluginData)[i];
            if ( entry.iLocked )
                {
                continue;
                }
            TInt retTemp = InstantiatePlugin( entry );
            if ( retTemp != KErrNone )
                {
                continue;
                }
            // Note: here two reports will be made if the plugin was not
            // instantiated but as the operatio is fast no new parameter will be
            // done to InstantiatePlugin() for instantiation reporting.
            entry.iInstance->ReportCharacterChange( aCharType, aNewChar );
            }
        }
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Extracts the NVRAM settings from a pipe-character delimited NVRAM buffer
// and sends the subsettings to all of the plugins with
// ReportNvramStatusChange().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::BroadcastNvramStatusChange( const TDesC8& aNvram )
    {
    TRACE_FUNC_ENTRY
    if ( !iPluginData )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    HBufC8* nvramEntry = NULL;
    TRAP_IGNORE( nvramEntry=HBufC8::NewMaxL(aNvram.Length()) );
    if ( !nvramEntry )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt i;
    TInt retVal = KErrNotFound;
    TInt count = iPluginData->Count();
    TPtr8 nvramEntryPtr = nvramEntry->Des();
    TInt startIndex = 0;   // Start of found subsetting
    TInt endIndex = 0;     // End of found subsetting
    while ( ExtractNextNvramSetting(aNvram,startIndex,endIndex)  )
        {
        // Report the subsetting to every plugin (and instantiate as every delta
        // change has to be signalled to every plugin).
        nvramEntryPtr.Copy( &aNvram[startIndex], endIndex-startIndex );
        for ( i=0; i<count; i++ )
            {
            TATExtPluginEntry& entry = (*iPluginData)[i];
            if ( entry.iLocked )
                {
                continue;
                }
            TInt retTemp = InstantiatePlugin( entry );
            if ( retTemp != KErrNone )
                {
                continue;
                }
            entry.iInstance->ReportNvramStatusChange( nvramEntryPtr );
            retVal = KErrNone;
            }
        }
    delete nvramEntry;
    TRACE_FUNC_EXIT
    return retVal;
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
TInt CATExtMetadata::ReportExternalHandleCommandError()
    {
    TRACE_FUNC_ENTRY
    if ( !iCmdData.iOldHandler )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    iCmdData.iOldHandler->ReportExternalHandleCommandError();
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Reports about abort condition in command handling.
// This is for cases when for example DUN decided an abort condition was
// received from DTE (ITU-T V.250 5.6.1). This API is for notifying the
// plugin that abort was requested. However the plugin currently handling
// the command may ignore the request if it doesn't support abort for the
// command or it may return the changed condition with
// HandleCommandCompleted()
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::ReportHandleCommandAbort( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iCmdData.iHandler || !iCmdData.iHandler->iInstance )
        {
        TRACE_FUNC_EXIT
        return KErrNotReady;
        }
    // Uncomment the following line for abort API
//    CATExtPluginBase& plugin = *iCmdData.iHandler->iInstance;
    // Remove the following line for abort API
    TInt retTemp = KErrNone;
    TBool stop = EFalse;
    // Uncomment the following line for abort API
//    TInt retTemp = plugin.ReportHandleCommandAbort( stop );
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TPckg<TBool> stopPckg( stop );
    TInt retVal = aMessage.Write( EATExtReportHandleCommandAbortParamStop,
                                  stopPckg );
    TRACE_INFO(( _L("Write returned %d"), retVal ));
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Extracts the NVRAM settings from a pipe-character delimited NVRAM buffer
// and sends the subsettings to all of the plugins with
// ReportNvramStatusChange().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::GetNextSpecialCommand( const RMessage2& aMessage,
                                            TBool aFirstSearch )
    {
    TRACE_FUNC_ENTRY
    if ( !iSupportAux )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    // If first search then start from index 0, othewise start from the
    // next index
    if ( aFirstSearch )
        {
        iSupportAuxFind = -1;
        }
    iSupportAuxFind++;
    TInt i;
    TInt count = iSupportAux->Count();
    TATExtAtCmdSupport* support = NULL;
    for ( i=iSupportAuxFind; i<count; i++ )
        {
        support = &(*iSupportAux)[i];
        if ( support->iEntries )
            {
            break;
            }
        }
    if ( i >= count )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    TInt retTemp = WriteReplyBufferToClient( *support->iAtCmdBase,
                                             EATExtGetNextSpecialCmdParamCmd,
                                             aMessage );
    TRACE_INFO(( _L("First write returned %d"), retTemp ));
    TPckg<TBool> firstSearch( EFalse );
    retTemp = aMessage.Write( EATExtGetNextSpecialCmdParamFirst, firstSearch );
    TRACE_INFO(( _L("Second write returned %d"), retTemp ));
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CATExtMetadata::CATExtMetadata
// ---------------------------------------------------------------------------
//
CATExtMetadata::CATExtMetadata( REComSession& aEComSession,
                                CATExtListen* aListener,
                                MATExtPluginObserver& aObserver ) :
    iEComSession( aEComSession ),
    iListener( aListener ),
    iObserver( aObserver )
    {
    Initialize();
    }

// ---------------------------------------------------------------------------
// CATExtMetadata::ConstructL
// ---------------------------------------------------------------------------
//
void CATExtMetadata::ConstructL()
    {
    if ( !iListener )
        {
        User::Leave( KErrGeneral );
        }
    }

// ---------------------------------------------------------------------------
// Initializes this class
// ---------------------------------------------------------------------------
//
void CATExtMetadata::Initialize()
    {
    // Don't initialize iListener here (it is set through NewL)
    // Don't initialize iCallback here (it is set through NewL)
    iIfUid = TUid::Null();
    iSupport = NULL;
    iSupportAux = NULL;
    iPluginData = NULL;
    iCmdData.iCmdStarted = EFalse;
    iCmdData.iReplyExpected = EFalse;
    iCmdData.iHandler = NULL;
    iCmdData.iOldHandler = NULL;
    iCmdData.iEditorHandler = NULL;
    iCarriageReturn = KDefaultCarriageReturn;
    iLineFeed = KDefaultLineFeed;
    iQuietMode = EFalse;
    iVerboseMode = ETrue;
    iSupportAuxFind = 0;
    iShutdown = EFalse;
    }

// ---------------------------------------------------------------------------
// Removes one plugin data entry by plugin UID
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::RemoveOnePlugindata( TUid& aPluginUid,
                                          TBool aInstanceExists )
    {
    TRACE_FUNC_ENTRY
    TInt foundIndex = KErrNotFound;
    TATExtPluginEntry* foundEntry = FindUidFromPlugindata( aPluginUid,
                                                           foundIndex );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    if ( !aInstanceExists )
        {
        foundEntry->iInstance = NULL;
        }
    if ( foundEntry == iCmdData.iHandler )
        {
        CancelCommandOperation( KErrCompletion );
        }
    CancelOneUrcOperation( *foundEntry, KErrCompletion );
    delete foundEntry->iInstance;
    foundEntry->iInstance = NULL;
    iPluginData->Delete( foundIndex );
    iPluginData->Compress();
    UpdateRemovedPluginLinks( iSupport, foundIndex );
    UpdateRemovedPluginLinks( iSupportAux, foundIndex );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Updates removed plugin links
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::UpdateRemovedPluginLinks(
    CArrayFixFlat<TATExtAtCmdSupport>* aSupport,
    TInt aRemovedIndex )
    {
    TRACE_FUNC_ENTRY
    if ( !aSupport )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt i;
    TInt j;
    TInt supportCount = aSupport->Count();
    for ( i=0; i<supportCount; i++ )
        {
        TATExtAtCmdSupport& support = (*aSupport)[i];
        if ( !support.iEntries )
            {
            continue;
            }
        TInt entryCount = support.iEntries->Count();
        for ( j=entryCount-1; j>=0; j-- )
            {
            TATExtOneCmdSupport& oneCmdSupport = (*support.iEntries)[j];
            TInt entryIndex = oneCmdSupport.iEntryIndex;
            if ( entryIndex > aRemovedIndex )
                {
                oneCmdSupport.iEntryIndex--;
                }
            else if ( entryIndex == aRemovedIndex )
                {
                support.iEntries->Delete( j );
                }
            }
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Completes command data
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::CompleteCommandData()
    {
    TRACE_FUNC_ENTRY
    CompleteCommandMessage( NULL,
                            KErrCompletion,
                            EFalse,
                            EReplyTypeUndefined,
                            EFalse );
    // It is possible that CompleteCommandMessage() didn't complete but there
    // is still data set to non-null values. Force setting now.
    iCmdData.iCmdStarted = EFalse;
    iCmdData.iReplyExpected = EFalse;
    iCmdData.iCmdBuffer.Close();
    iCmdData.iCmdReplyBuffer.Close();
    iCmdData.iHandler = NULL;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Adds AT commands set to metadata. Used by function AddImplementationL().
// ---------------------------------------------------------------------------
//
void CATExtMetadata::DoAddImplementationL(
    CImplementationInformation* aImplInfo,
    const TDesC8& aAtCmdSet )
    {
    TRACE_FUNC_ENTRY
    if ( !aImplInfo || !iPluginData || !iSupport || !iSupportAux )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    TATExtSupportType type = ESupportTypeUndefined;
    TInt start = 0;  // Start of found subcommand
    TInt end = 0;    // End of found subcommand
    HBufC8* parsedCmd = HBufC8::NewMaxLC( aAtCmdSet.Length() );
    TPtr8 parsedCmdPtr = parsedCmd->Des();
    while ( ExtractNextCommand(aAtCmdSet,start,end,type) == KErrNone )
        {
        if ( type == ESupportTypeUndefined )
            {
            continue;
            }
        parsedCmdPtr.Copy( &aAtCmdSet[start], end-start );
        TUid pluginUid = aImplInfo->ImplementationUid();
        TATExtCleanupInfo cleanupInfo;
        TRAPD( retTrap, AddEntryToMetadataL(parsedCmdPtr,pluginUid,type,
                                            cleanupInfo) );
        if ( retTrap != KErrNone )
            {
            CleanPartialMetadata( cleanupInfo );
            TRACE_FUNC_EXIT
            User::Leave( retTrap );
            }
        TInt retTemp = iListener->AddPluginUid( pluginUid );
        if ( retTemp != KErrNone )
            {
            TRACE_FUNC_EXIT
            User::Leave( retTemp );
            }
        }
    CleanupStack::PopAndDestroy( parsedCmd );
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Removes an implementation from the support data by a given plugin UID.
// Used by function RemoveImplementation().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::DoRemoveImplementation(
    TUid& aPluginUid,
    CArrayFixFlat<TATExtAtCmdSupport>* aSupport )
    {
    TRACE_FUNC_ENTRY
    if ( !aSupport )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    // 1. Find the UID from plugindata.
    TInt foundIndex = KErrNotFound;
    TATExtPluginEntry* foundEntry = FindUidFromPlugindata( aPluginUid,
                                                           foundIndex );
    if ( !foundEntry )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    // 2. Delete the entries from metadata which point to the found entry.
    TInt i;
    TInt j;
    TInt supportCount = aSupport->Count();
    for ( i=supportCount-1; i>=0; i-- )
        {
        TATExtAtCmdSupport& support = (*aSupport)[i];
        if ( !support.iEntries )
            {
            continue;
            }
        TInt entryCount = support.iEntries->Count();
        for ( j=entryCount-1; j>=0; j-- )
            {
            TInt entryIndex = (*support.iEntries)[j].iEntryIndex;
            if ( &(*iPluginData)[entryIndex] == foundEntry )
                {
                support.iEntries->Delete( j );
                }
            }
        support.iEntries->Compress();
        // 3. Remove the entries from metadata with zero length.
        if ( support.iEntries->Count() == 0 )
            {
            aSupport->Delete( i );
            }
        }
    aSupport->Compress();
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Checks whether AT command handling is active or not
// ---------------------------------------------------------------------------
//
TBool CATExtMetadata::IsCommandHandling()
    {
    TRACE_FUNC_ENTRY
    if ( iCmdData.iCmdStarted ||
        (iCmdData.iCmdMessage.Handle() && iCmdData.iCmdBuffer.Length()>0) ||
         iCmdData.iCmdReplyBuffer.Length()>0 )
        {
        TRACE_FUNC_EXIT
        return ETrue;
        }
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Destroys data related to plugins
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::DestroyPlugindata()
    {
    TRACE_FUNC_ENTRY
    if ( !iPluginData )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    CancelCommandOperation( KErrServerTerminated );
    // First cancel the plugindata operations before delete
    TInt i;
    TInt pluginCount = iPluginData->Count();
    for ( i=0; i<pluginCount; i++ )
        {
        TATExtPluginEntry& entry = (*iPluginData)[i];
        CancelOneUrcOperation( entry, KErrServerTerminated );
        }
    // Next delete the actual data
    for ( i=0; i<pluginCount; i++ )
        {
        TATExtPluginEntry& entry = (*iPluginData)[i];
        delete entry.iInstance;
        entry.iInstance = NULL;
        }
    iPluginData->Reset();
    delete iPluginData;
    iPluginData = NULL;
    iCmdData.iHandler = NULL;
    iCmdData.iOldHandler = NULL;
    iCmdData.iEditorHandler = NULL;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cancels an active AT command handling operation by reinitializing internal
// data, completing the client request message and calling the plugin's
// HandleCommandCancel().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::CancelCommandOperation( TInt aError, TBool aCheckLock )
    {
    TRACE_FUNC_ENTRY
    if ( aCheckLock && iCmdData.iHandler && iCmdData.iHandler->iLocked )
        {
        TRACE_FUNC_EXIT
        return KErrAccessDenied;
        }
    if ( iCmdData.iHandler && iCmdData.iHandler->iInstance )
        {
        iCmdData.iHandler->iInstance->HandleCommandCancel();
        iCmdData.iEditorHandler = NULL;
        }
    CompleteCommandMessage( NULL,
                            aError,
                            EFalse,
                            EReplyTypeUndefined,
                            EFalse );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cancels an active URC message receiving operation by a given plugin entry,
// completing the client request message and calling the plugin's
// ReceiveUnsolicitedResultCancel().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::CancelOneUrcOperation( TATExtPluginEntry& aPluginEntry,
                                            TInt aError,
                                            TBool aCheckLock )
    {
    TRACE_FUNC_ENTRY
    if ( aCheckLock && aPluginEntry.iLocked )
        {
        TRACE_FUNC_EXIT
        return KErrAccessDenied;
        }
    aPluginEntry.iUrcStarted = EFalse;
    if ( aPluginEntry.iInstance )
        {
        aPluginEntry.iInstance->ReceiveUnsolicitedResultCancel();
        }
    if ( aPluginEntry.iUrcMessage.Handle() )
        {
        aPluginEntry.iUrcMessage.Complete( aError );
        }
    // Note: Don't touch the iUrcOwned here
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Destroys data related to AT command support
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::DestroySupportdata(
    CArrayFixFlat<TATExtAtCmdSupport>*& aSupport )
    {
    TRACE_FUNC_ENTRY
    if ( !aSupport )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    // Next just remove the entries
    TInt i;
    TInt supportCount = aSupport->Count();
    for ( i=0; i<supportCount; i++ )
        {
        TATExtAtCmdSupport& support = (*aSupport)[i];
        delete support.iAtCmdBase;
        support.iAtCmdBase = NULL;
        if ( support.iEntries )
            {
            support.iEntries->Reset();
            delete support.iEntries;
            support.iEntries = NULL;
            }
        }
    aSupport->Reset();
    delete aSupport;
    aSupport = NULL;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Extracts one command from a pipe-character delimited command buffer (Note:
// This function is used to get the settings from a plugin RSS file)
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::ExtractNextCommand( const TDesC8& aCommands,
                                         TInt& aStartIndex,
                                         TInt& aEndIndex,
                                         TATExtSupportType& aSupportType )
    {
    TRACE_FUNC_ENTRY
    aSupportType = ESupportTypeUndefined;
    // Skip data before command
    TInt i;
    TChar character;
    TInt count = aCommands.Length();
    for ( i=aEndIndex; i<count; i++ )
        {
        character = aCommands[i];
        if ( character.IsPrint() && character!='|' )
            {
            break;
            }
        }
    if ( i >= count )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    // Now the plugin type marker is found; check the plugin type
    switch ( character )
        {
        case 'M':  // Master plugin
        case 'm':
            aSupportType = ESupportTypeMaster;
            break;
        case 'P':  // Partial plugin
        case 'p':
            aSupportType = ESupportTypePartial;
            break;
        case 'O':  // Observer plugin
        case 'o':
            aSupportType = ESupportTypeObserver;
            break;
        default:
            _LIT( KPluginType, "PluginType" );
            User::Panic( KPluginType, EPanicCategoryType );
        }
    i++;
    if ( i >= count )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    aStartIndex = i;
    // Start of command found, next mark the end
    while ( i < count )
        {
        character = aCommands[i];
        if ( !character.IsPrint() || character=='|' )
            {
            break;
            }
        i++;
        }
    aEndIndex = i;
    if ( aEndIndex-aStartIndex <= 0 )
        {
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Adds a new plugin entry to support data. If command to be added is not
// found from the support data then that command is inserted alphabetically to
// the support data. After this a search for the plugin UID is done; if the
// UID is not found then it is added to the plugin data and a new initialized
// data is created for that entry. Finally the added or new plugin entry is
// linked to the new or existing support entry.
// ---------------------------------------------------------------------------
//
void CATExtMetadata::AddEntryToMetadataL( TDes8& aAtCmdBase,
                                          TUid& aPluginUid,
                                          TATExtSupportType aSupportType,
                                          TATExtCleanupInfo& aCleanupInfo )
    {
    TRACE_FUNC_ENTRY
    if ( !iSupport || !iSupportAux || !iPluginData )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    // First try to find the string with binary search.
    // If found, add aUid.
    // If not found, add aAtCmdBase, aPluginUid.
    CArrayFixFlat<TATExtAtCmdSupport>* support = iSupport;
    aCleanupInfo.iSupportCreated = EFalse;
    aCleanupInfo.iSupportIndex = KErrNotFound;
    aCleanupInfo.iEntryCreated = EFalse;
    aCleanupInfo.iEntryIndex = KErrNotFound;
    aCleanupInfo.iSupport = iSupport;
    TBool findAux = EFalse;
    TInt strLength = aAtCmdBase.Length();
    if ( strLength>=3 && aAtCmdBase[strLength-1]=='*' )  // 3 for "AT*"
        {
        aAtCmdBase.SetLength( strLength-1 );
        aCleanupInfo.iSupport = iSupportAux;
        support = iSupportAux;
        findAux = ETrue;
        }
    TInt foundPos = KErrNotFound;
    TBool found = FindCommandFromMetadata( aAtCmdBase, foundPos, findAux, EFalse );
    if ( !found )
        {
        // Not found. foundPos is the position where to add the new entry
        TATExtAtCmdSupport newEntry;
        foundPos = -foundPos;  // convert to positive
        newEntry.iAtCmdBase = HBufC8::NewMaxLC( aAtCmdBase.Length() );
        newEntry.iEntries = NULL;
        TPtr8 atCmdPtr = newEntry.iAtCmdBase->Des();
        atCmdPtr.Copy( aAtCmdBase );
        support->InsertL( foundPos, newEntry );
        CleanupStack::Pop( newEntry.iAtCmdBase );
        aCleanupInfo.iSupportCreated = ETrue;
        }
    aCleanupInfo.iSupportIndex = foundPos;
    // Next try to find the plugin UID from plugin data.
    TInt foundIndex = KErrNotFound;
    TATExtPluginEntry* foundEntry = FindUidFromPlugindata( aPluginUid,
                                                           foundIndex );
    if ( !foundEntry )
        {
        // Not found. Append new initialized entry.
        TATExtPluginEntry pluginEntry;
        pluginEntry.iPluginUid = aPluginUid;
        pluginEntry.iInstance = NULL;
        pluginEntry.iLocked = EFalse;
        pluginEntry.iUrcStarted = EFalse;
        pluginEntry.iUrcOwned = EFalse;
        iPluginData->AppendL( pluginEntry );
        aCleanupInfo.iEntryCreated = ETrue;
        aCleanupInfo.iEntryIndex = iPluginData->Count() - 1;
        foundIndex = aCleanupInfo.iEntryIndex;
        foundEntry = &(*iPluginData)[foundIndex];
        }
    // Now foundEntry is either the found entry or a newly added entry.
    // Make the iSupport metadata point to this.
    TATExtAtCmdSupport& foundSupport = (*support)[foundPos];
#if defined( PRJ_OPTIMIZE_FOR_SPEED )
    AddNewMetadataEntryLinkL( foundSupport.iEntries,
                              foundIndex,
                              aSupportType,
                              foundSupport.iSearchHelper );
#else  // PRJ_OPTIMIZE_FOR_SPEED
    TATExtSearchHelper searchHelper;
    AddNewMetadataEntryLinkL( foundSupport.iEntries,
                              foundIndex,
                              aSupportType,
                              searchHelper );
#endif  // PRJ_OPTIMIZE_FOR_SPEED
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Adds new plugin entry link from plugin support entry to plugin entry
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::AddNewMetadataEntryLinkL(
    CArrayFixFlat<TATExtOneCmdSupport>*& aEntries,
    TInt aEntryIndex,
    TATExtSupportType aSupportType,
    TATExtSearchHelper& aSearchHelper )
    {
    TRACE_FUNC_ENTRY
    if ( !iPluginData || aEntryIndex<0 || aEntryIndex>=iPluginData->Count() )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    if ( !aEntries )
        {
        aEntries = new (ELeave) CArrayFixFlat<TATExtOneCmdSupport>( KGranularity );
        }
    TATExtOneCmdSupport oneCmdSupport;
    oneCmdSupport.iSupportType = aSupportType;
    oneCmdSupport.iEntryIndex = aEntryIndex;
    TInt retVal = KErrNotSupported;
    switch ( aSupportType )
        {
        case ESupportTypeMaster:
            {
            retVal = AddNewMasterMetadataEntryLinkL( aEntries,
                                                     aSearchHelper,
                                                     oneCmdSupport );
            }
            break;
        case ESupportTypePartial:
            {
            retVal = AddNewPartialMetadataEntryLinkL( aEntries,
                                                      aSearchHelper,
                                                      oneCmdSupport );
            }
            break;
        case ESupportTypeObserver:
            {
            retVal = AddNewObserverMetadataEntryLinkL( aEntries,
                                                       aSearchHelper,
                                                       oneCmdSupport );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Adds new master plugin entry link from plugin support entry to plugin entry
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::AddNewMasterMetadataEntryLinkL(
    CArrayFixFlat<TATExtOneCmdSupport>* aEntries,
    TATExtSearchHelper& aSearchHelper,
    TATExtOneCmdSupport& aOneCmdSupport )
    {
    TRACE_FUNC_ENTRY
    if ( aEntries->Count() > 0 )
        {
        TATExtOneCmdSupport& oneCmdSupport = (*aEntries)[0];
        if ( oneCmdSupport.iSupportType == ESupportTypePartial )
            {
            _LIT( KAddMIsP, "AddMIsP" );
            User::Panic( KAddMIsP, EPanicCategoryAddMIsP );
            }
        if ( oneCmdSupport.iSupportType == ESupportTypeMaster )
            {
            _LIT( KAddMIsM, "AddMIsM" );
            User::Panic( KAddMIsM, EPanicCategoryAddMIsM );
            }
        }
    aEntries->InsertL( 0, aOneCmdSupport );
    if ( aSearchHelper.iPartialIndex >= 0 )
       {
        aSearchHelper.iPartialIndex++;
        }
    if ( aSearchHelper.iObserverIndex >= 0 )
        {
        aSearchHelper.iObserverIndex++;
        }
    TRACE_FUNC_EXIT
    return 0;
    }

// ---------------------------------------------------------------------------
// Adds new partial plugin entry link from plugin support entry to plugin
// entry
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::AddNewPartialMetadataEntryLinkL(
    CArrayFixFlat<TATExtOneCmdSupport>* aEntries,
    TATExtSearchHelper& aSearchHelper,
    TATExtOneCmdSupport& aOneCmdSupport )
    {
    TRACE_FUNC_ENTRY
    if ( aEntries->Count() > 0 )
        {
        TATExtOneCmdSupport& oneCmdSupport = (*aEntries)[0];
        if ( oneCmdSupport.iSupportType == ESupportTypeMaster )
            {
            _LIT( KAddPIsM, "AddPIsM" );
            User::Panic( KAddPIsM, EPanicCategoryAddPIsM );
            }
        }
    TInt i = aSearchHelper.iPartialIndex;
    if ( i < 0 )
        {
        TInt count = aEntries->Count();
        for ( i=0; i<count; i++ )
            {
            TATExtOneCmdSupport& oneCmdSupport = (*aEntries)[i];
            if ( oneCmdSupport.iSupportType==ESupportTypePartial ||
                 oneCmdSupport.iSupportType==ESupportTypeObserver )
                {
                break;
                }
            }
        aSearchHelper.iPartialIndex = i;
        }
    aEntries->InsertL( i, aOneCmdSupport );
    if ( aSearchHelper.iObserverIndex >= 0 )
        {
        aSearchHelper.iObserverIndex++;
        }
    TRACE_FUNC_EXIT
    return i;
    }

// ---------------------------------------------------------------------------
// Adds new observer plugin entry link from plugin support entry to plugin
// entry. Search starts from the front as there could be multiple O plugins
// but only one or two M/P plugins.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::AddNewObserverMetadataEntryLinkL(
    CArrayFixFlat<TATExtOneCmdSupport>* aEntries,
    TATExtSearchHelper& aSearchHelper,
    TATExtOneCmdSupport& aOneCmdSupport )
    {
    TRACE_FUNC_ENTRY
    TInt i = aSearchHelper.iObserverIndex;
    if ( i < 0 )
        {
        TInt count = aEntries->Count();
        for ( i=0; i<count; i++ )
            {
            TATExtOneCmdSupport& oneCmdSupport = (*aEntries)[i];
            if ( oneCmdSupport.iSupportType == ESupportTypeObserver )
                {
                break;
                }
            }
        aSearchHelper.iObserverIndex = i;
        }
    aEntries->InsertL( i, aOneCmdSupport );
    TRACE_FUNC_EXIT
    return i;
    }

// ---------------------------------------------------------------------------
// Cleans partial created metadata based on TATExtCleanupInfo
// ---------------------------------------------------------------------------
//
void CATExtMetadata::CleanPartialMetadata( TATExtCleanupInfo& aCleanupInfo )
    {
    TRACE_FUNC_ENTRY
    TBool withinEntryLimits = EFalse;
    TBool withinSupportLimits = EFalse;
    CArrayFixFlat<TATExtAtCmdSupport>* support = aCleanupInfo.iSupport;
    if ( aCleanupInfo.iEntryIndex>=0 &&
         aCleanupInfo.iEntryIndex<support->Count() )
        {
        withinEntryLimits = ETrue;
        }
    if ( aCleanupInfo.iSupportIndex>=0 &&
         aCleanupInfo.iSupportIndex<support->Count() )
        {
        withinSupportLimits = ETrue;
        }
    if ( aCleanupInfo.iEntryCreated && withinEntryLimits )
        {
        iPluginData->Delete( aCleanupInfo.iEntryIndex );
        }
    if ( aCleanupInfo.iSupportCreated && withinSupportLimits )
        {
        TInt deleteIndex = aCleanupInfo.iSupportIndex;
        TATExtAtCmdSupport& supportData = (*support)[deleteIndex];
        delete supportData.iAtCmdBase;
        supportData.iAtCmdBase = NULL;
        delete supportData.iEntries;
        supportData.iEntries = NULL;
        support->Delete( deleteIndex );
        support->Compress();
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Prints the AT command plugin entries and information about them.
// For debugging/bug hunting only
// ---------------------------------------------------------------------------
//
#if defined(_DEBUG) && defined( PRJ_PRINT_SUPPORT_DATA )
TInt CATExtMetadata::PrintSupportData(
    CArrayFixFlat<TATExtAtCmdSupport>* aSupport )
    {
    TRACE_FUNC_ENTRY
    if ( !aSupport )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt i;
    TInt j;
    TInt supportCount = aSupport->Count();
    for ( i=0; i<supportCount; i++ )
        {
        TATExtAtCmdSupport& support = (*aSupport)[i];
        if ( !support.iEntries )
            {
            continue;
            }
        TInt entryCount = support.iEntries->Count();
        TRACE_INFO(( _L8("Support entry at index %d = '%S' (%d entries):"), i, &(*support.iAtCmdBase), entryCount ));
        for ( j=0; j<entryCount; j++ )
            {
            TATExtOneCmdSupport& oneCmdSupport = (*support.iEntries)[j];
            TInt entryIndex = oneCmdSupport.iEntryIndex;
            TRACE_INFO(( _L8("\tPlugin entry at index %d = Type:%d, UID:0x%08X"), j, oneCmdSupport.iSupportType, (*iPluginData)[entryIndex].iPluginUid ));
            }
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }
#endif

// ---------------------------------------------------------------------------
// Prints the found AT command plugin entries and information about them.
// For debugging/bug hunting only
// ---------------------------------------------------------------------------
//
#if defined(_DEBUG) && defined( PRJ_PRINT_SUPPORT_DATA )
TInt CATExtMetadata::PrintFoundEntries(
    CArrayFixFlat<TATExtOneCmdSupport>* aEntries )
    {
    TRACE_FUNC_ENTRY
    if ( !aEntries )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt i;
    TInt count = aEntries->Count();
    for ( i=0; i<count; i++ )
        {
        TATExtOneCmdSupport& entry = (*aEntries)[i];
        TInt entryIndex = entry.iEntryIndex;
        TRACE_INFO(( _L8("\tFound entry at index %d = Type:%d, UID:0x%08X"), i, entry.iSupportType, (*iPluginData)[entryIndex].iPluginUid ));
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }
#endif

// ---------------------------------------------------------------------------
// Does a binary search for an AT command to find the AT command from the
// support data
// ---------------------------------------------------------------------------
//
TBool CATExtMetadata::FindCommandFromMetadata( TDesC8& aAtCmdBase,
                                               TInt& aFoundIndex,
                                               TBool aFindAuxCmd,
                                               TBool aCheckAsterisk )
    {
    TRACE_FUNC_ENTRY
    if ( !iSupport || !iSupportAux || (!aFindAuxCmd&&aCheckAsterisk) )
        {
        TRACE_FUNC_EXIT
        return EFalse;
        }
    TInt result = 0;
    TInt middle = 0;
    TInt left = 0;
    CArrayFixFlat<TATExtAtCmdSupport>* support = iSupport;
    if ( aFindAuxCmd )
        {
        support = iSupportAux;
        }
    TInt right = support->Count() - 1;
    while ( left <= right )
        {
        middle = ( left + right ) / 2;
        HBufC8& compareStr = *(*support)[middle].iAtCmdBase;
        TInt strLength = compareStr.Length();
        if ( aCheckAsterisk )
            {
            TPtrC8 leftPart = aAtCmdBase.Left( strLength );
            result = compareStr.Compare( leftPart );
            }
        else
            {
            result = compareStr.Compare( aAtCmdBase );
            }
        if ( result == 0 )  // iSupport == aAtCmd
            {
            aFoundIndex = middle;
            TRACE_FUNC_EXIT
            return ETrue;
            }
        else if ( result < 0 )  // iSupport < aAtCmd
            {
            left = middle + 1;
            }
        else  // iSupport > aAtCmd
            {
            right = middle - 1;
            }
        }
    aFoundIndex = -left;
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Finds the first plugin entry from a given plugin UID
// ---------------------------------------------------------------------------
//
TATExtPluginEntry* CATExtMetadata::FindUidFromPlugindata( TUid& aUid,
                                                          TInt& aFoundIndex )
    {
    TRACE_FUNC_ENTRY
    aFoundIndex = KErrNotFound;
    if ( !iPluginData )
        {
        TRACE_FUNC_EXIT
        return NULL;
        }
    TInt i;
    TInt count = iPluginData->Count();
    for ( i=0; i<count; i++ )
        {
        TATExtPluginEntry& entry = (*iPluginData)[i];
        if ( entry.iPluginUid == aUid )
            {
            TRACE_FUNC_EXIT
            aFoundIndex = i;
            return &entry;
            }
        }
    TRACE_FUNC_EXIT
    return NULL;
    }

// ---------------------------------------------------------------------------
// Find the first plugin entry from a given plugin instance
// ---------------------------------------------------------------------------
//
TATExtPluginEntry* CATExtMetadata::FindInstanceFromPlugindata(
    CATExtPluginBase* aInstance )
    {
    TRACE_FUNC_ENTRY
    if ( !iPluginData )
        {
        TRACE_FUNC_EXIT
        return NULL;
        }
    TInt i;
    TInt count = iPluginData->Count();
    for ( i=0; i<count; i++ )
        {
        TATExtPluginEntry& entry = (*iPluginData)[i];
        if ( entry.iInstance == aInstance )
            {
            TRACE_FUNC_EXIT
            return &entry;
            }
        }
    TRACE_FUNC_EXIT
    return NULL;
    }

// ---------------------------------------------------------------------------
// Finds an Urc message processing plugin. Must be used in synchronization
// with MarkUrcHandlingOwnership(). More information in aPluginUid parameter's
// explanation below.
// ---------------------------------------------------------------------------
//
TATExtPluginEntry* CATExtMetadata::FindUrcProcessingPlugin(
    TUid& aPluginUid )
    {
    TRACE_FUNC_ENTRY
    if ( !iPluginData )
        {
        TRACE_FUNC_EXIT
        return NULL;
        }
    TInt i;
    TInt count = iPluginData->Count();
    for ( i=0; i<count; i++ )
        {
        TATExtPluginEntry& entry = (*iPluginData)[i];
        if ( entry.iUrcMessage.Handle() )
            {
            continue;
            }
        // second run (after MarkUrcHandlingOwnership())
        if ( aPluginUid != TUid::Null() )
            {
            if ( entry.iUrcOwned && entry.iPluginUid==aPluginUid )
                {
                TRACE_FUNC_EXIT
                return &entry;
                }
            }
        // first run (before MarkUrcHandlingOwnership())
        else if ( !entry.iUrcStarted && !entry.iUrcOwned )
            {
            TRACE_FUNC_EXIT
            return &entry;
            }
        }
    TRACE_FUNC_EXIT
    return NULL;
    }

// ---------------------------------------------------------------------------
// Extracts the base command from a given AT command to find support with
// DoCreateAndFindSupportL()
// ---------------------------------------------------------------------------
//
void CATExtMetadata::CreateAndFindSupportL(
    TDesC8& aAtCmdFull,
    const RMessage2& aMessage,
    TATExtCompletionInfo& aComplInfo )
    {
    TRACE_FUNC_ENTRY
    // First extract the base of AT command from aAtCmdFull
    // ('=' and everything after that)
    TInt newLength = aAtCmdFull.Length();
    TInt foundPos = aAtCmdFull.Locate( '=' );
    if ( foundPos >= 2 )  // After "AT"
        {
        newLength = foundPos;
        }
    else if ( newLength >= 1 )
        {
        // There was no '=' so check if the last character is '?'.
        // If it is then remove it.
        if ( aAtCmdFull[newLength-1] == '?' )
            {
            newLength--;
            }
        }
    HBufC8* baseCmd = HBufC8::NewMaxLC( newLength );
    TPtr8 baseCmdPtr = baseCmd->Des();
    baseCmdPtr.Copy( &aAtCmdFull[0], newLength );
    // Now the baseCmd has the base command. Use it to find the support.
    DoCreateAndFindSupportL( *baseCmd,
                             aAtCmdFull,
                             aMessage,
                             aComplInfo );
    CleanupStack::PopAndDestroy( baseCmd );
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Creates AT command support and finds the given AT command from the created
// support data
// ---------------------------------------------------------------------------
//
void CATExtMetadata::DoCreateAndFindSupportL(
    TDesC8& aAtCmdBase,
    TDesC8& aAtCmdFull,
    const RMessage2& aMessage,
    TATExtCompletionInfo& aComplInfo )
    {
    TRACE_FUNC_ENTRY
    // Note: don't set iCmdData.iOldHandler to NULL here as the sequence can
    // invoke this function again before ReportExternalHandleCommandError().
    // Get array of supported plugins for the base command
    CArrayFixFlat<TATExtOneCmdSupport>* support;
    support = FindEntriesForCommandLC( aAtCmdBase );
    if ( !support )
        {
        aComplInfo.iProcessed = EFalse;
        aComplInfo.iReplyExpected = ETrue;
        CreateSelfReplyData( aMessage );
        CleanupStack::PopAndDestroy();
        TRACE_FUNC_EXIT
        return;
        }
    TATExtEntrySupport entrySupport( aAtCmdFull, aMessage, support );
    TInt i;
    aComplInfo.iProcessed = EFalse;
    TInt count = support->Count();
    for ( i=0; i<count; i++ )
        {
        TBool supported = EFalse;
        TATExtOneCmdSupport& oneCmdSupport = (*support)[i];
        entrySupport.iEntry = &(*iPluginData)[oneCmdSupport.iEntryIndex];
        if ( oneCmdSupport.iSupportType == ESupportTypeMaster )
            {
            supported = HandleMasterAndPartialPluginSupportL(
                entrySupport,
                i+1,
                aComplInfo.iReplyExpected );
            }
        else if ( oneCmdSupport.iSupportType == ESupportTypePartial )
            {
            supported = FindExclusivePartialSupportL( entrySupport );
            if ( !supported )
                {
                break;
                }
            supported = HandleMasterAndPartialPluginSupportL(
                entrySupport,
                i+1,
                aComplInfo.iReplyExpected );
            }
        else if ( oneCmdSupport.iSupportType == ESupportTypeObserver )
            {
            supported = HandleObserverPluginSupportL(
                entrySupport,
                i+1,
                aComplInfo.iReplyExpected );
            }
        if ( supported )
            {
            aComplInfo.iProcessed = ETrue;
            break;
            }
        }
    CleanupStack::PopAndDestroy( support );
    if ( !aComplInfo.iProcessed || !aComplInfo.iReplyExpected )
        {
        CreateSelfReplyData( aMessage );
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Prepares internal data for completion with "" or "ERROR" messages.
// More explation in CreateEmptyOrErrorBuffer().
// ---------------------------------------------------------------------------
//
void CATExtMetadata::CreateSelfReplyData( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    // "ERROR" or "" needed; initialize
    iCmdData.iCmdStarted = ETrue;
    iCmdData.iCmdMessage = aMessage;
    iCmdData.iHandler = NULL;
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Handles support when a master or partial plugin is detected in the plugin
// data via support data's link. If a partial or master plugin is detected
// then reply is expected from that plugin. Also if one or more observer
// plugins are detected then no reply is expected from them.
// ---------------------------------------------------------------------------
//
TBool CATExtMetadata::HandleMasterAndPartialPluginSupportL(
    TATExtEntrySupport& aEntrySupport,
    TInt aStartIndex,
    TBool& aReplyExpected )
    {
    TRACE_FUNC_ENTRY
    aReplyExpected = EFalse;
    if ( !aEntrySupport.iSupportFound )
        {
        TBool supported = HandleCommandSupportL( aEntrySupport );
        if ( !supported )
            {
            TRACE_FUNC_EXIT
            return EFalse;
            }
        }
    // If HandleCommand() is implemented synchronously, the command must be
    // saved before executing as CompleteCommandMessage() closes the string
    HBufC8* atCmdFull = HBufC8::NewMaxLC( aEntrySupport.iAtCmdFull.Length() );
    TPtr8 atCmdFullPtr = atCmdFull->Des();
    atCmdFullPtr.Copy( aEntrySupport.iAtCmdFull );
    // Now execute the HandleCommand()
    iCmdData.iReplyExpected = ETrue;  // Set before HandleCommandL()
    HandleCommandL( aEntrySupport, ETrue );
    aEntrySupport.iStartIndex = aStartIndex;
    SendToMultipleObserverL( aEntrySupport, atCmdFull );
    CleanupStack::PopAndDestroy( atCmdFull );
    aReplyExpected = ETrue;
    TRACE_FUNC_EXIT
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Handles support when a observer plugin is detected in the plugin data via
// support data's link. If only one observer plugin is detected then reply is
// expected from that plugin. Instead, if more than one observer plugins are
// detected then no reply is expected from them.
// ---------------------------------------------------------------------------
//
TBool CATExtMetadata::HandleObserverPluginSupportL(
    TATExtEntrySupport& aEntrySupport,
    TInt aStartIndex,
    TBool& aReplyExpected )
    {
    TRACE_FUNC_ENTRY
    aReplyExpected = EFalse;
    if ( !aEntrySupport.iSupportFound )
        {
        TBool supported = HandleCommandSupportL( aEntrySupport );
        if ( !supported )
            {
            TRACE_FUNC_EXIT
            return EFalse;
            }
        }
    TATExtEntrySupport nextSupport = aEntrySupport;
    nextSupport.iStartIndex = aStartIndex;
    TBool entryFound = FindFirstObserverSupportL( nextSupport );
    if ( entryFound )
        {
        // Entry found; send all without reply request
        // If HandleCommand() is implemented synchronously, the command must be
        // saved before executing as CompleteCommandMessage() closes the string
        HBufC8* atCmdFull = HBufC8::NewMaxLC( aEntrySupport.iAtCmdFull.Length() );
        TPtr8 atCmdFullPtr = atCmdFull->Des();
        atCmdFullPtr.Copy( aEntrySupport.iAtCmdFull );
        // Now execute the HandleCommand()
        HandleCommandL( aEntrySupport, EFalse );
        SendToMultipleObserverL( nextSupport, atCmdFull );
        CleanupStack::PopAndDestroy( atCmdFull );
        }
    else
        {
        // No entry found; send one with reply request
        iCmdData.iReplyExpected = ETrue;  // Set before HandleCommandL()
        HandleCommandL( aEntrySupport, ETrue );
        aReplyExpected = ETrue;
        }
    TRACE_FUNC_EXIT
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Finds exclusive partial plugin support
// ---------------------------------------------------------------------------
//
TBool CATExtMetadata::FindExclusivePartialSupportL(
    TATExtEntrySupport& aEntrySupport )
    {
    TRACE_FUNC_ENTRY
    TInt i;
    TInt supports;
    TInt count = aEntrySupport.iSupport->Count();
    for ( i=0,supports=0; i<count; i++ )
        {
        TATExtOneCmdSupport& oneCmdSupport = (*aEntrySupport.iSupport)[i];
        if ( oneCmdSupport.iSupportType == ESupportTypePartial )
            {
            aEntrySupport.iSupportFound = EFalse;
            aEntrySupport.iEntry = &(*iPluginData)[oneCmdSupport.iEntryIndex];
            TBool supported = HandleCommandSupportL( aEntrySupport );
            if ( supported )
                {
                aEntrySupport.iSupportFound = ETrue;
                supports++;
                }
            if ( supports != 1 )
                {
                TRACE_FUNC_EXIT
                return EFalse;
                }
            }
        }
    TRACE_FUNC_EXIT
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Finds support entries from support data for a given base AT command
// ---------------------------------------------------------------------------
//
CArrayFixFlat<TATExtOneCmdSupport>* CATExtMetadata::FindEntriesForCommandLC(
    TDesC8& aAtCmdBase )
    {
    TRACE_FUNC_ENTRY
    TBool found;
    TInt foundPos;
    CArrayFixFlat<TATExtOneCmdSupport>* support =
        new (ELeave) CArrayFixFlat<TATExtOneCmdSupport>( KGranularity );
    CleanupStack::PushL( support );
    // First find normal data to which to add the aux entries
    found = FindCommandFromMetadata( aAtCmdBase, foundPos, EFalse, EFalse );
    if ( found )
        {
        CArrayFixFlat<TATExtOneCmdSupport>* entries =
            (*iSupport)[foundPos].iEntries;
        support->AppendL( &(*entries)[0], entries->Count() );
        }
    // Next add the aux links
    found = FindCommandFromMetadata( aAtCmdBase, foundPos, ETrue, ETrue );
    if ( found )
        {
        CArrayFixFlat<TATExtOneCmdSupport>* entries =
            (*iSupportAux)[foundPos].iEntries;
        TATExtSearchHelper searchHelper;
        TInt i;
        TInt count = entries->Count();
        for ( i=0; i<count; i++ )
            {
            TATExtOneCmdSupport& entry = (*entries)[i];
            AddNewMetadataEntryLinkL( support,
                                      entry.iEntryIndex,
                                      entry.iSupportType,
                                      searchHelper );
            }
        }
    // Now, as the normal data was inserted *before* the auxiliary data, the
    // auxiliary data is in front of the created "support" array for each M, P
    // and O entry. This insertion is faster than the other way around as there
    // can be multiple O plugins for the same command but usually much less
    // auxiliary entries for the same command.
#if defined(_DEBUG) && defined( PRJ_PRINT_SUPPORT_DATA )
    PrintFoundEntries( support );
#endif
    if ( support->Count() > 0 )
        {
        TRACE_FUNC_EXIT
        return support;
        }
    TRACE_FUNC_EXIT
    return NULL;
    }

// ---------------------------------------------------------------------------
// Instantiates plugin support
// ---------------------------------------------------------------------------
//
void CATExtMetadata::InstantiatePluginSupportL(
    TATExtEntrySupport& aEntrySupport )
    {
    TRACE_FUNC_ENTRY
    if ( aEntrySupport.iEntry )
        {
        InstantiatePluginL( *aEntrySupport.iEntry );
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Instantiates a plugin
// ---------------------------------------------------------------------------
//
void CATExtMetadata::InstantiatePluginL( TATExtPluginEntry& aPluginEntry )
    {
    TRACE_FUNC_ENTRY
    if ( !aPluginEntry.iInstance )
         {
         TUid pluginUid = aPluginEntry.iPluginUid;
         CATExtPluginBase* plugin = CATExtPluginBase::NewL( pluginUid,
                                                            iObserver,
                                                            iConnectionName );
         plugin->ReportQuietModeChange( iQuietMode );
         plugin->ReportVerboseModeChange( iVerboseMode );
         plugin->ReportCharacterChange( ECharTypeCarriage, iCarriageReturn );
         plugin->ReportCharacterChange( ECharTypeLineFeed, iLineFeed );
         plugin->ReportCharacterChange( ECharTypeBackspace, iBackspace );
         aPluginEntry.iInstance = plugin;
         }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Instantiates a plugin
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::InstantiatePlugin( TATExtPluginEntry& aPluginEntry )
    {
    TRACE_FUNC_ENTRY
    TRAPD( retTrap, InstantiatePluginL(aPluginEntry) );
    TRACE_FUNC_EXIT
    return retTrap;
    }

// ---------------------------------------------------------------------------
// Handles an AT command support request
// ---------------------------------------------------------------------------
TBool CATExtMetadata::HandleCommandSupportL( TATExtEntrySupport& aEntrySupport,
                                             const TDesC8* aAtCmdFull )
    {
    TRACE_FUNC_ENTRY
    InstantiatePluginSupportL( aEntrySupport );
    TBool supported = EFalse;
    if ( aEntrySupport.iEntry->iLocked )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrAccessDenied );
        }
    if ( !aAtCmdFull )
        {
        supported = aEntrySupport.iEntry->iInstance->IsCommandSupported(
            aEntrySupport.iAtCmdFull );
        }
    else
        {
        supported = aEntrySupport.iEntry->iInstance->IsCommandSupported(
            *aAtCmdFull );
        }
    TRACE_FUNC_EXIT
    return supported;
    }

// ---------------------------------------------------------------------------
// Handles an AT command
// ---------------------------------------------------------------------------
void CATExtMetadata::HandleCommandL( TATExtEntrySupport& aEntrySupport,
                                     TBool aReplyNeeded,
                                     const TDesC8* aAtCmdFull )
    {
    TRACE_FUNC_ENTRY
    if ( !aEntrySupport.iEntry || !aEntrySupport.iEntry->iInstance )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    if ( aEntrySupport.iEntry->iLocked )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrAccessDenied );
        }
    // As HandleCommand() could complete the message synchronously, set the
    // iCmdData before the call
    if ( aReplyNeeded )
        {
        iCmdData.iCmdStarted = ETrue;
        iCmdData.iCmdMessage = aEntrySupport.iMessage;
        iCmdData.iHandler = aEntrySupport.iEntry;
        iCmdData.iEditorHandler = NULL;
        }
    // No "else" here as HandleCommandL() is used also with observer plugins
    if ( !aAtCmdFull )
        {
        TRACE_INFO(( _L8("Handling command '%S' for UID:0x%08X, aReplyNeeded=%d..."), &iCmdData.iCmdBuffer, aEntrySupport.iEntry->iPluginUid, aReplyNeeded ));
        aEntrySupport.iEntry->iInstance->HandleCommand(
            iCmdData.iCmdBuffer,
            iCmdData.iCmdReplyBuffer,
            aReplyNeeded );
        }
    else
        {
        TRACE_INFO(( _L8("Handling command '%S' for UID:0x%08X, aReplyNeeded=%d..."), &(*aAtCmdFull), aEntrySupport.iEntry->iPluginUid, aReplyNeeded ));
        aEntrySupport.iEntry->iInstance->HandleCommand(
            *aAtCmdFull,
            iCmdData.iCmdReplyBuffer,
            aReplyNeeded );
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Sends an AT commands to multiple observer plugins, starting from a given
// position.
// ---------------------------------------------------------------------------
//
void CATExtMetadata::SendToMultipleObserverL(
    TATExtEntrySupport& aEntrySupport,
    const TDesC8* aAtCmdFull )
    {
    TRACE_FUNC_ENTRY
    if ( !aEntrySupport.iSupport )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    if ( !aEntrySupport.iSupportFound )
        {
        // No initial support found -> do nothing
        TRACE_FUNC_EXIT
        return;
        }
    TInt i;
    TInt count = aEntrySupport.iSupport->Count();
    for ( i=aEntrySupport.iStartIndex; i<count; i++ )
        {
        TATExtOneCmdSupport& oneCmdSupport = (*aEntrySupport.iSupport)[i];
        if ( oneCmdSupport.iSupportType != ESupportTypeObserver )
            {
            continue;
            }
        aEntrySupport.iEntry = &(*iPluginData)[oneCmdSupport.iEntryIndex];
        TBool supported = HandleCommandSupportL( aEntrySupport, aAtCmdFull );
        if ( supported )
            {
            HandleCommandL( aEntrySupport, EFalse, aAtCmdFull );
            aEntrySupport.iSupportFound = ETrue;
            }
        if ( i < count-1 )
            {
            aEntrySupport.iSupportFound = EFalse;
            }
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Finds the first observer plugin support from a given starting position
// ---------------------------------------------------------------------------
//
TBool CATExtMetadata::FindFirstObserverSupportL(
    TATExtEntrySupport& aEntrySupport )
    {
    TRACE_FUNC_ENTRY
    if ( !aEntrySupport.iSupport )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    TInt i;
    TInt count = aEntrySupport.iSupport->Count();
    for ( i=aEntrySupport.iStartIndex; i<count; i++ )
        {
        TATExtOneCmdSupport& oneCmdSupport = (*aEntrySupport.iSupport)[i];
        if ( oneCmdSupport.iSupportType != ESupportTypeObserver )
            {
            continue;
            }
        aEntrySupport.iSupportFound = EFalse;
        aEntrySupport.iEntry = &(*iPluginData)[oneCmdSupport.iEntryIndex];
        TBool supported = HandleCommandSupportL( aEntrySupport );
        if ( supported )
            {
            aEntrySupport.iSupportFound = ETrue;
            TRACE_FUNC_EXIT
            return ETrue;
            }
        }
    TRACE_FUNC_EXIT
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Extracts one NVRAM entry from a pipe-character delimited NVRAM buffer
// ---------------------------------------------------------------------------
//
TBool CATExtMetadata::ExtractNextNvramSetting( const TDesC8& aNvram,
                                               TInt& aStartIndex,
                                               TInt& aEndIndex )
    {
    TRACE_FUNC_ENTRY
    // Skip data before command
    TInt i;
    TChar character;
    TInt count = aNvram.Length();
    for ( i=aEndIndex; i<count; i++ )
        {
        character = aNvram[i];
        if ( character.IsPrint() && character!='|' )
            {
            break;
            }
        }
    if ( i >= count )
        {
        TRACE_FUNC_EXIT
        return EFalse;
        }
    aStartIndex = i;
    // Start of command found, next mark the end
    while ( i < count )
        {
        character = aNvram[i];
        if ( !character.IsPrint() || character=='|' )
            {
            break;
            }
        i++;
        }
    aEndIndex = i;
    if ( aEndIndex-aStartIndex <= 0 )
        {
        TRACE_FUNC_EXIT
        return EFalse;
        }
    TRACE_FUNC_EXIT
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Gets the next command from support or auxiliary support data.
// ---------------------------------------------------------------------------
//
HBufC8* CATExtMetadata::GetNextSupportedCommand( TBool& aFirstSearch,
                                                 TInt& aSupportFind,
                                                 TBool& aFindInAux )
    {
    TRACE_FUNC_ENTRY
    // If first search then start from index 0, othewise start from the
    // next index
    if ( aFirstSearch )
        {
        aSupportFind = -1;
        aFindInAux = EFalse;
        }
    aSupportFind++;
    aFirstSearch = EFalse;
    CArrayFixFlat<TATExtAtCmdSupport>* support = iSupport;
    if ( !aFindInAux )
        {
        if ( aSupportFind >= support->Count() )
            {
            aSupportFind = 0;
            aFindInAux = ETrue;
            support = iSupportAux;
            if ( aSupportFind >= support->Count() )
                {
                TRACE_FUNC_EXIT
                return NULL;
                }
            }
        }
    else
        {
        support = iSupportAux;
        if ( aSupportFind >= support->Count() )
            {
            TRACE_FUNC_EXIT
            return NULL;
            }
        }
    TATExtAtCmdSupport& supportEntry = (*support)[aSupportFind];
    HBufC8& atCmdBase = *supportEntry.iAtCmdBase;
    HBufC8* newBuffer = HBufC8::NewMax( atCmdBase.Length() );
    if ( newBuffer )
        {
        newBuffer->Des().Copy( atCmdBase );
        }
    TRACE_FUNC_EXIT
    return newBuffer;
    }

// ---------------------------------------------------------------------------
// Reads an AT command from a client request message and creates buffer
// locally. For HandleCommand().
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::ReadCommandFromMessage( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    // Read AT command from message, stop if error encountered
    TInt desLength = aMessage.GetDesLength( EATExtHandleCmdParamCmd );
    if ( desLength <= 0 )
        {
        TRACE_FUNC_EXIT
        return KErrArgument;
        }
    TInt retTemp = iCmdData.iCmdBuffer.Create( desLength );
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        return retTemp;
        }
    retTemp = aMessage.Read( EATExtHandleCmdParamCmd, iCmdData.iCmdBuffer );
    TRACE_INFO(( _L("Read returned %d"), retTemp ));
    if ( retTemp != KErrNone )
        {
        iCmdData.iCmdBuffer.Close();
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TRACE_FUNC_EXIT
    return retTemp;
    }

// ---------------------------------------------------------------------------
// Creates a buffer with "ERROR" or "" string in it; needed for creating a
// reply to an unknown command (i.e. when no plugin supports the "base" part
// of a command) or to the case when plugin support exists but reply is not
// expected from them.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::CreateEmptyOrErrorBuffer( RBuf8& aBuffer,
                                               TBool aErrorReply )
    {
    TRACE_FUNC_ENTRY
    // Note: The behavior of RBuf8 is awkward. It is not possible to query
    // the "allocation status". Comparing the length to zero always returns
    // true even if RBuf8::Create() was not called. Here we should do the
    // following: If allocated and zero length, return with KErrNone. Otherwise
    // if not allocated create the buffer and return with RBuf::Create()'s
    // return code. We have to hack here so let's expect the buffer is not
    // allocated when calling this function.
    if ( !aErrorReply )
        {
        TInt retTemp = aBuffer.Create( KNullDesC8 );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TInt retVal = KErrNone;
    TBuf8<KErrorMsgLen> errorBuffer;
    if ( !iQuietMode )
        {
        if ( iVerboseMode )
            {
            _LIT8( KVerboseError, "ERROR" );
            errorBuffer.Append( iCarriageReturn );
            errorBuffer.Append( iLineFeed );
            errorBuffer.Append( KVerboseError );
            errorBuffer.Append( iCarriageReturn );
            errorBuffer.Append( iLineFeed );
            }
        else
            {
            _LIT8( KNumericError, "4" );
            errorBuffer.Append( KNumericError );
            errorBuffer.Append( iCarriageReturn );
            }
        }
    retVal = aBuffer.Create( errorBuffer );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Writes specified input reply buffer to a client request message at given
// message slot number.
// ---------------------------------------------------------------------------
//
TInt CATExtMetadata::WriteReplyBufferToClient( const TDesC8& aBuffer,
                                               TInt aDataSlot,
                                               const RMessage2& aMessage,
                                               TBool aReportNextLength,
                                               TInt aLengthSlot )
    {
    TRACE_FUNC_ENTRY
    TInt maxLength = aMessage.GetDesMaxLength( aDataSlot );
    if ( aBuffer.Length() > maxLength )
        {
        TRACE_FUNC_EXIT
        return KErrTooBig;
        }
    TInt retTemp = aMessage.Write( aDataSlot, aBuffer );
    TRACE_INFO(( _L("First write returned %d"), retTemp ));
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        return retTemp;
        }
    if ( aReportNextLength && iCmdData.iHandler &&
         iCmdData.iHandler->iInstance )  // optional
        {
        CATExtPluginBase& plugin = *iCmdData.iHandler->iInstance;
        TInt nextPartLength = plugin.NextReplyPartLength();
        if ( nextPartLength <= 0 )
            {
            iCmdData.iHandler = NULL;
            }
        TPckg<TInt> nextPartLengthPckg( nextPartLength );
        retTemp = aMessage.Write( aLengthSlot, nextPartLengthPckg );
        TRACE_INFO(( _L("Second write returned %d"), retTemp ));
        if ( retTemp != KErrNone )
            {
            TRACE_FUNC_EXIT
            return retTemp;
            }
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }
