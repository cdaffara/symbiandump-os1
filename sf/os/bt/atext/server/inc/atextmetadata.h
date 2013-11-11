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

#ifndef C_CATEXTMETADATA_H
#define C_CATEXTMETADATA_H

#include <e32base.h>
#include <atext.h>
#include "atextpluginobserver.h"

class CATExtPluginBase;
class CATExtListen;

/** Panic categories */
enum TATExtPanicCategories
    {
    EPanicCategoryAddPIsM = 1,  // Master exists while adding Partial
    EPanicCategoryAddMIsP = 2,  // Partial exists while adding Master
    EPanicCategoryAddMIsM = 3,  // Master exists while adding Master
    EPanicCategoryType    = 4   // Wrong plugin type (not M|P|O)
    };

/**  Support types supported by RSS file (M|P|O) */
enum TATExtSupportType
    {
    ESupportTypeUndefined = KErrNotFound,
    ESupportTypeMaster    = 0,
    ESupportTypePartial,
    ESupportTypeObserver
    };

/**  Operation types, either for command handling or for URC receiving */
enum TATExtOperationType
    {
    EOperationUndefined     = KErrNotFound,
    EOperationHandleCommand = 0,
    EOperationReceiveUrc
    };

/**
 *  Completion information for CATExtSession::IpcHandleCommand().
 *  This data is needed to check how the AT command handling succeeded and what
 *  was the type of the completed AT command (in HandleCommand()).
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtCompletionInfo )
    {

public:

    /**
     * Flag to indicate whether or not an AT command was processed by some
     * plugin
     *
     */
    TBool iProcessed;

    /**
     * Flag to indicate whether or not a reply is expected from some of the
     * plugins
     */
    TBool iReplyExpected;

    };

/**
 *  Class to store data needed by each plugin.
 *  Support data TATExtAtCmdSupport creates links to one plugin entry.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtPluginEntry )
    {

public:

    /**
     * UID of plugin with support
     */
    TUid iPluginUid;

    /**
     * Instance of plugin
     */
    CATExtPluginBase* iInstance;

    /**
     * Flag to indicate whether or not the access to plugin data is locked
     */
    TBool iLocked;

    /**
     * Flag to indicate whether or not URC message receiving has started
     */
    TBool iUrcStarted;

    /**
     * Client request message for URC message receiver
     */
    RMessage2 iUrcMessage;

    /**
     * Flag to indicate whether or not the plugin URC handling is owned by the
     * client side user.
     */
    TBool iUrcOwned;

    };

/**
 *  Search helper information for AddNewMetadataEntryLinkL().
 *  The data in this class is changes when the indexes are known.
 *  When known index(es) are found then these indexes are used instead of
 *  searching for the position where to insert the new entry. Index for master
 *  entry is not stored as it is always inserted as the first entry.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtSearchHelper )
    {

public:

    TATExtSearchHelper() : iPartialIndex( KErrNotFound ),
                           iObserverIndex( KErrNotFound ) {}

    /**
     * Index to the partial entry
     */
    TInt iPartialIndex;

    /**
     * Index to the observer entry
     */
    TInt iObserverIndex;

    };

/**
 *  Class to store data needed for one AT command support.
 *  This includes the support's type and the plugin entry.
 *  Thus one plugin's RSS file may contain multiple M|P|O entries.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtOneCmdSupport )
    {

public:

    /**
     * Support's type (M|P|O)
     */
    TATExtSupportType iSupportType;

    /**
     * Plugin entry for support
     */
    TInt iEntryIndex;

    };

/**
 *  Class to store data needed for one AT command.
 *  Links to entry data TATExtPluginEntry are added for each supported AT
 *  command. Each entry of TATExtAtCmdSupport specifies the entry entered to
 *  an RSS file's "default_data" field with the "=" and the substring after it
 *  truncated. The truncation is done by CreateAndFindSupportL().
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtAtCmdSupport )
    {

public:

    /**
     * Supported AT command
     */
    HBufC8* iAtCmdBase;

#if defined( PRJ_OPTIMIZE_FOR_SPEED )

    /**
     * Search helper to iEntries. Needed when creating the database.
     */
    TATExtSearchHelper iSearchHelper;

#endif

    /**
     * Plugin entries for support
     */
    CArrayFixFlat<TATExtOneCmdSupport>* iEntries;

    };

/**
 *  Cleanup information for AddEntryToMetadataL().
 *  This data is needed to clean up partially constructed data in
 *  HandleCommand() when a leave occurs in plugin entry or support entry
 *  addition.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtCleanupInfo )
    {

public:

    /**
     * Flag to indicate whether or not a support entry was created
     */
    TBool iSupportCreated;

    /**
     * Index to the position where the support entry was inserted
     */
    TInt iSupportIndex;

    /**
     * Flag to indicate whether or not a plugin entry was created
     */
    TBool iEntryCreated;

    /**
     * Index to the position where the entry was inserted
     */
    TInt iEntryIndex;

    /**
     * Support information to normal or auxiliary plugin data
     */
    CArrayFixFlat<TATExtAtCmdSupport>* iSupport;

    };

/**
 *  Class to store data related to AT command handling.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtCommandData )
    {

public:

    /**
     * Flag indicating whether or not command handling has started
     */
    TBool iCmdStarted;

    /**
     * Flag indicating whether or not reply expected from plugin to ATEXT.
     * This is used to omit HandleCommandCompleted() from a plugin to ATEXT
     * when ATEXT uses its own reply.
     */
    TBool iReplyExpected;

    /**
     * Client request message for AT command handling
     */
    RMessage2 iCmdMessage;

    /**
     * Buffer for command.
     * Set by HandleCommand().
     */
    RBuf8 iCmdBuffer;

    /**
     * Buffer for reply.
     * Set by CompleteCommandMessage() and GetNextPartOfReply().
     */
    RBuf8 iCmdReplyBuffer;

    /**
     * Plugin entry for the AT command handler
     * Not own.
     */
    TATExtPluginEntry* iHandler;

    /**
     * Plugin entry for the AT command handler (old command handled);
     * set when command handling successfully completed.
     * Not own.
     */
    CATExtPluginBase* iOldHandler;

    /**
     * Pointer to editor handler; set when editor mode started,
     * NULL when editor mode not active.
     * Not own.
     */
    TATExtPluginEntry* iEditorHandler;

    };

/**
 *  Class store data for AT command handling.
 *  This is basically a helper class for internal usage to pass around data
 *  related to AT command handling.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TATExtEntrySupport )
    {

public:

    TATExtEntrySupport( TDesC8& aAtCmdFull,
                        const RMessage2& aMessage,
                        CArrayFixFlat<TATExtOneCmdSupport>* aSupport ) :
                        iAtCmdFull( aAtCmdFull ),
                        iMessage( aMessage ),
                        iSupport( aSupport ),
                        iEntry( NULL ),
                        iStartIndex( KErrNotFound ),
                        iSupportFound( EFalse ){}

    /**
     * Full AT command for which to check the support
     * (base part + parameters)
     */
    TDesC8& iAtCmdFull;

    /**
     * Client request message for reading
     */
    const RMessage2& iMessage;

    /**
     * AT command's plugin entry support data.
     * Used by SendToMultipleObserverL() and FindFirstObserverSupportL().
     */
    CArrayFixFlat<TATExtOneCmdSupport>* iSupport;

    /**
     * Found plugin entry.
     */
    TATExtPluginEntry* iEntry;

    /**
     * Start index for searches with HandlePartialAndMasterPluginSupportL() and
     * HandleObserverPluginSupportL()
     */
    TInt iStartIndex;

    /**
     * Found plugin support for HandlePartialAndMasterPluginSupportL().
     */
    TBool iSupportFound;

    };

/**
 *  Class for handling various operations of the server session which are
 *  related to metadata handling. Also adds methods which don't do metadata
 *  handling but are better to be in this class for abstraction and data hiding
 *  reasons.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CATExtMetadata ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aEComSession ECOM session; used for getting ECOM implementation
     *                     information.
     * @param aListener ECOM plugin notification event listener
     * @param aObserver Plugin observer (needed for instantiation)
     * @return Instance of self
     */
	static CATExtMetadata* NewL( REComSession& aEComSession,
                                 CATExtListen* aListener,
                                 MATExtPluginObserver& aObserver );

    /**
     * Two-phased constructor.
     * @param aEComSession ECOM session; used for getting ECOM implementation
     *                     information.
     * @param aListener ECOM plugin notification event listener
     * @param aObserver Plugin observer (needed for instantiation)
     * @return Instance of self
     */
    static CATExtMetadata* NewLC( REComSession& aEComSession,
                                  CATExtListen* aListener,
	                              MATExtPluginObserver& aObserver );


    /**
    * Destructor.
    */
    virtual ~CATExtMetadata();

    /**
     * Resets data to initial values
     *
     * @since S60 5.0
     * @return None
     */
    void ResetData();

    /**
     * Creates new implementation metadata based on a given interface UID and
     * connection. Uses AddImplementationL() to add new implementations for the
     * interface and sets connection identification name for reporting it to the
     * plugins on instantiation time.
     *
     * @since S60 5.0
     * @param aIfUid Interface UID to support
     * @param aName Connection identification name
     * @return None
     */
    void CreateImplementationMetadataL( TUid& aIfUid, const TDesC8& aName );

    /**
     * Adds an implementation to the metadata based on a given ECOM
     * implementation information. This is used by the function
     * CreateImplementationMetadataL() and also by CATExtSession when its ECOM
     * notification listener detects an addition for a the same ECOM interface.
     * This function uses DoAddImplementation to add the actual AT command set
     * to the metadata.
     *
     * @since S60 5.0
     * @param aImplInfo ECOM implementation information from which to create new
     *                  metadata
     * @return None
     */
    void AddImplementationL( CImplementationInformation* aImplInfo );

    /**
     * Removes an implementation from the support data by a given plugin UID.
     *
     * @since S60 5.0
     * @param aPluginUid Plugin UID to search from plugin data
     * @param aInstanceExists ETrue if instance exists, EFalse otherwise
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt RemoveImplementation( TUid& aPluginUid, TBool aInstanceExists=ETrue );

    /**
     * Handles an AT command
     *
     * @since S60 5.0
     * @param aMessage Client request message for reading
     * @param aComplInfo Completion information for the handled command
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt HandleCommand( const RMessage2& aMessage,
                        TATExtCompletionInfo& aComplInfo );

    /**
     * Cancels an active handle command operation. Uses IsCommandHandling() and
     * CancelCommandOperation().
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CancelHandleCommand();

    /**
     * Gets the next part of a reply for HandleCommand().
     * Length of returned reply must be the same as the one reported
     * from NextReplyPartLength() for the current reply.
     *
     * @since S60 5.0
     * @param aMessage Client request message for reading
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt GetNextPartOfReply( const RMessage2& aMessage );

    /**
     * Completes an URC processing message by a given plugin UID.
     * Completion code for the client message is the return code from a
     * write with WriteReplyBufferToClient()
     *
     * @since S60 5.0
     * @param aAtCmd URC message to write to the client side
     * @param aPlugin Plugin UID to search from plugin data
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CompleteUrcMessage( const TDesC8& aAtCmd,
                             CATExtPluginBase* aPlugin );

    /**
     * Completes an AT command handling message. Also clears internal
     * initialized command hanlder data; see ClearInitializedCmdHandlerData().
     *
     * @since S60 5.0
     * @param aPlugin Plugin's instance to match against the command data
     *                NULL if no check wanted for a match against the command
     *                data
     * @param aError Completion error code for the command message
     * @param aErrorReply ETrue if error reply "ERROR" needed,
     *                    EFalse if empty reply "" needed.
     *                    For "" and "ERROR" replies the reply must not
     *                    contain an existing reply
     * @param aReplyType Reply type for the handled AT command
     * @param aMultiPart ETrue (default behavior) if multipart reply wanted,
     *                   EFalse otherwise.
     *                   For multipart replies the reply may be over
     *                   KDefaultCmdBufLength.
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CompleteCommandMessage( CATExtPluginBase* aPlugin,
                                 TInt aError,
                                 TBool aErrorReply,
                                 TATExtensionReplyType aReplyType,
                                 TBool aMultiPart );

    /**
     * Writes multipart or single part reply buffer to client for handle.
     * Used for creating a reply for HandleCommand().
     *
     * @since S60 5.0
     * @param aMultiPart ETrue (default behavior) if multipart reply wanted,
     *                   EFalse otherwise.
     *                   For multipart replies the reply may be over
     *                   KDefaultCmdBufLength.
     * @param aStartOfEditor ETrue if start of editor mode,
     *                       EFalse otherwise
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt WriteHandleCmdReplyBuffer( TBool aMultiPart,
                                    TBool aStartOfEditor );

    /**
     * Clears internal initialized command handler data. This is currently used
     * only by CompleteCommandMessage() and is called when the data is not
     * needed anymore. It also prepares the internal data for a new
     * HandleCommand() call.
     *
     * @since S60 5.0
     * @param aMultiPart ETrue (default behavior) if multipart reply wanted,
     *                   EFalse otherwise.
     *                   For multipart replies the reply may be over
     *                   KDefaultCmdBufLength.
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ClearInitializedCmdHandlerData( TBool aMultiPart );

    /**
     * Gets the array of supported commands.
     *
     * @since S60 5.0
     * @param aPlugin Plugin UID to search from plugin data
     * @param aCmds Array of supported commands
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt GetSupportedCommands( CATExtPluginBase* aPlugin,
                               RPointerArray<HBufC8>& aCmds );

    /**
     * Starts URC message receiving for plugin. Note that
     * MarkUrcHandlingOwnership() must be called immediately after this in order
     * for the messages to receive their destination.
     *
     * @since S60 5.0
     * @param aMessage Client request message for reading
     * @param aPluginUid Plugin UID to search from plugin data
     * @return None
     */
    void StartUrcReceivingL( const RMessage2& aMessage,
                             TUid& aPluginUid );

    /**
     * Cancels an active URC message receiving operation by a given plugin UID.
     * Uses CancelOneUrcOperation() to cancel if plugin UID found from plugin
     * data.
     *
     * @since S60 5.0
     * @param aPluginUid Plugin UID to search from plugin data
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CancelUrcReceiving( TUid& aPluginUid );

    /**
     * Marks URC handling ownership for a plugin entry. Call to this function
     * must be done immediately after the call to StartUrcReceivingL().
     *
     * @since S60 5.0
     * @param aMessage Client request message for reading
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt MarkUrcHandlingOwnership( const RMessage2& aMessage );

    /**
     * Marks the access to a plugin data as "locked". This blocks all operations
     * where plugin function calls are to be done.
     *
     * @since S60 5.0
     * @param aPluginUid Plugin UID to search from plugin data
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt LockPluginAccess( TUid& aPluginUid );

    /**
     * Marks the access to a plugin data as "unlocked". This enables all
     * operations where plugin function call are to be done.
     *
     * @since S60 5.0
     * @param aPluginUid Plugin UID to search from plugin data
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt UnlockPluginAccess( TUid& aPluginUid );

    /**
     * Searches for an instances from the support data's plugin data link and
     * only marks the instance as uninitialized. Note that this doesn't try to
     * cancel any of the current plugin operations and should be used only when
     * a plugin destroys itself.
     *
     * @since S60 5.0
     * @param aInstance Instance to remove
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt RemoveImplementationInstance( CATExtPluginBase* aInstance );

    /**
     * Checks if support data has been constructed from the plugin data.
     *
     * @since S60 5.0
     * @return ETrue if plugin support data found, EFalse otherwise
     */
    TBool SupportExists();

    /**
     * Number of plugins with AT command handling support based on the
     * information in RSS files' AT command handling entries. This information
     * is needed to instantiate one or more listeners by the user of the client.
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt NumberOfPlugins();

    /**
     * Sets the quiet mode to the required value. After this the mode is
     * reported to the plugins with ReportQuietModeChange().
     *
     * @since S60 5.0
     * @param aMode ETrue if quiet mode on, EFalse if off
     * @return None
     */
    void SetQuietMode( TBool aMode );

    /**
     * Sets the verbose mode to the required value. After this the mode is
     * reported to the plugins with ReportVerboseModeChange().
     *
     * @since S60 5.0
     * @param aMode ETrue if verbose mode on, EFalse if off
     * @return None
     */
    void SetVerboseMode( TBool aMode );

    /**
     * Sets new character value for a carriage return, line feed or backspace
     * character. After this its type and value are reported to the plugins with
     * ReportCharacterChange().
     *
     * @since S60 5.0
     * @param aCharType Character's type (carriage/linefeed/backspace)
     * @param aNewChar New character's value to set
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt SetCharacterValue( TATExtensionCharType aCharType,
                            TInt8 aNewChar );

    /**
     * Extracts the NVRAM settings from a pipe-character delimited NVRAM buffer
     * and sends the subsettings to all of the plugins with
     * ReportNvramStatusChange().
     *
     * @since S60 5.0
     * @param aNvram NVRAM setting to broadcast
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt BroadcastNvramStatusChange( const TDesC8& aNvram );

    /**
     * Reports about external handle command error condition.
     * This is for cases when for example DUN decided the reply contained an
     * error condition but the plugin is still handling the command internally.
     * Example: "AT+TEST;+TEST2" was given in command line; "AT+TEST" returns
     * non-EReplyTypeError condition and "AT+TEST2" returns EReplyTypeError.
     * As the plugin(s) returning the non-EReplyTypeError may still have some
     * ongoing operation then these plugins are notified about the external
     * EReplyTypeError in command line processing. It is to be noted that
     * HandleCommandCancel() is not sufficient to stop the processing as the
     * command handling has already finished.
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ReportExternalHandleCommandError();

    /**
     * Reports about abort condition in command handling.
     * This is for cases when for example DUN decided an abort condition was
     * received from DTE (ITU-T V.250 5.6.1). This API is for notifying the
     * plugin that abort was requested. However the plugin currently handling
     * the command may ignore the request if it doesn't support abort for the
     * command or it may return the changed condition with
     * HandleCommandCompleted()
     *
     * @since S60 5.0
     * @param aMessage Client request message for writing
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ReportHandleCommandAbort( const RMessage2& aMessage );

    /**
     * Gets the next special command RSS file entry. These commands are commands
     * with a matching start but with any ending sequence. Command "ATD*99***1#"
     * is one of these and the number of these commands should be kept minimal.
     *
     * @since S60 5.0
     * @param aMessage Client request message for reading
     * @param aFirstSearch ETrue if first search, EFalse otherwise
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt GetNextSpecialCommand( const RMessage2& aMessage,
                                TBool aFirstSearch );

private:

    CATExtMetadata( REComSession& aEComSession,
                    CATExtListen* aListener,
                    MATExtPluginObserver& aObserver );

    void ConstructL();

    /**
     * Initializes this class
     *
     * @since S60 5.0
     * @return None
     */
    void Initialize();

    /**
     * Removes one plugin data entry by plugin UID
     *
     * @since S60 5.0
     * @param aPluginUid Plugin UID to search from plugin data
     * @param aInstanceExists ETrue if instance exists, EFalse otherwise
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt RemoveOnePlugindata( TUid& aPluginUid,
                              TBool aInstanceExists );

    /**
     * Removes one plugin data entry by plugin UID
     *
     * @since S60 5.0
     * @param aSupport Support information to normal or auxiliary plugin data
     * @param aRemovedIndex Index that was removed
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt UpdateRemovedPluginLinks( CArrayFixFlat<TATExtAtCmdSupport>* aSupport,
                                   TInt aRemovedIndex );

    /**
     * Completes command data
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CompleteCommandData();

    /**
     * Adds AT commands set to metadata. Used by function AddImplementationL().
     *
     * @since S60 5.0
     * @param aImplInfo ECOM implementation information from which to create new
     *                  metadata
     * @param aAtCmdSet AT command set to add to the metadata
     *
     * @return None
     */
    void DoAddImplementationL( CImplementationInformation* aImplInfo,
                               const TDesC8& aAtCmdSet );

    /**
     * Removes an implementation from the support data by a given plugin UID.
     * Used by function RemoveImplementation().
     *
     * @since S60 5.0
     * @param aPluginUid Plugin UID to search from plugin data
     * @param aSupport Support information to normal or auxiliary plugin data
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt DoRemoveImplementation( TUid& aPluginUid,
                                 CArrayFixFlat<TATExtAtCmdSupport>* aSupport );

    /**
     * Checks whether or not AT command handling is active
     *
     * @since S60 5.0
     * @return ETrue if command handling is ongoing, EFalse otherwise
     */
    TBool IsCommandHandling();

    /**
     * Destroys data related to plugins
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt DestroyPlugindata();

    /**
     * Cancels an active AT command handling operation by reinitializing
     * internal data, completing the client request message and calling the
     * plugin's HandleCommandCancel().
     *
     * @since S60 5.0
     * @param aError Completion error code for the command message
     * @param aCheckLock ETrue if plugin lock check needed, EFalse otherwise
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CancelCommandOperation( TInt aError,
                                 TInt aCheckLock=EFalse );

    /**
     * Cancels an active URC message receiving operation by a given plugin
     * entry, completing the client request message and calling the plugin's
     * ReceiveUnsolicitedResultCancel().
     *
     * @since S60 5.0
     * @param aPluginEntry The plugin's entry which is cancelled
     * @param aError Completion error code for the URC message
     * @param aCheckLock ETrue if plugin lock check needed, EFalse otherwise
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CancelOneUrcOperation( TATExtPluginEntry& aPluginEntry,
                                TInt aError,
                                TInt aCheckLock=EFalse );

    /**
     * Destroys data related to AT command support
     *
     * @since S60 5.0
     * @param aSupport Support information to normal or auxiliary plugin data
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt DestroySupportdata( CArrayFixFlat<TATExtAtCmdSupport>*& aSupport );

    /**
     * Extracts one command from a pipe-character delimited command buffer
     * (Note: This function is used to get the settings from a plugin RSS file)
     *
     * @since S60 5.0
     * @param aCommands Command buffer from where to extract the next subcommand
     * @param aStartIndex Start index for the found command
     * @param aEndIndex End index for the found command
     * @param aSupportType Support's type (M|P|O)
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ExtractNextCommand( const TDesC8& aCommands,
                             TInt& aStartIndex,
                             TInt& aEndIndex,
                             TATExtSupportType& aSupportType );

    /**
     * Adds a new plugin entry to support data. If command to be added is not
     * found from the support data then that command is inserted alphabetically
     * to the support data. After this a search for the plugin UID is done; if
     * the UID is not found then it is added to the plugin data and a new
     * initialized data is created for that entry. Finally the added or new
     * plugin entry is linked to the new or existing support entry.
     *
     * @since S60 5.0
     * @param aAtCmdBase Base AT command which to add to support data
     *                   (base part without parameters)
     * @param aPluginUid Plugin's UID to find from plugin data
     * @param aSupportType Support's type (M|P|O)
     * @param aCleanupInfo Cleanup information
     * @return None
     */
    void AddEntryToMetadataL( TDes8& aAtCmdBase,
                              TUid& aPluginUid,
                              TATExtSupportType aSupportType,
                              TATExtCleanupInfo& aCleanupInfo );

    /**
     * Adds new plugin entry link from plugin support entry to plugin entry
     *
     * @since S60 5.0
     * @param aEntries Plugin entries for support
     * @param aEntryIndex Plugin index entry
     * @param aSupportType Support's type (M|P|O)
     * @param aSearchHelper The search helper
     * @return Index to the added entry, KErrNotSupported otherwise
     */
    TInt AddNewMetadataEntryLinkL(
        CArrayFixFlat<TATExtOneCmdSupport>*& aEntries,
        TInt aEntryIndex,
        TATExtSupportType aSupportType,
        TATExtSearchHelper& aSearchHelper );

    /**
     * Adds new master plugin entry link from plugin support entry to plugin
     * entry
     *
     * @since S60 5.0
     * @return Index to the added entry, KErrNotSupported otherwise
     */
    TInt AddNewMasterMetadataEntryLinkL(
        CArrayFixFlat<TATExtOneCmdSupport>* aEntries,
        TATExtSearchHelper& aSearchHelper,
        TATExtOneCmdSupport& aOneCmdSupport );

    /**
     * Adds new partial plugin entry link from plugin support entry to plugin
     * entry
     *
     * @since S60 5.0
     * @return Index to the added entry, KErrNotSupported otherwise
     */
    TInt AddNewPartialMetadataEntryLinkL(
        CArrayFixFlat<TATExtOneCmdSupport>* aEntries,
        TATExtSearchHelper& aSearchHelper,
        TATExtOneCmdSupport& aOneCmdSupport );

    /**
     * Adds new observer plugin entry link from plugin support entry to plugin
     * entry. Search starts from the front as there could be multiple O plugins
     * but only one or two M/P plugins.
     *
     * @since S60 5.0
     * @return Index to the added entry, KErrNotSupported otherwise
     */
    TInt AddNewObserverMetadataEntryLinkL(
        CArrayFixFlat<TATExtOneCmdSupport>* aEntries,
        TATExtSearchHelper& aSearchHelper,
        TATExtOneCmdSupport& aOneCmdSupport );

    /**
     * Cleans partial created metadata based on TATExtCleanupInfo
     *
     * @since S60 5.0
     * @param aCleanupInfo Cleanup information
     * @return None
     */
    void CleanPartialMetadata( TATExtCleanupInfo& aCleanupInfo );

    /**
     * Prints the AT command plugin entries and information about them.
     * For debugging/bug hunting only
     *
     * @since S60 5.0
     * @param aSupport Support information to normal or auxiliary plugin data
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt PrintSupportData( CArrayFixFlat<TATExtAtCmdSupport>* aSupport );

    /**
     * Prints the found AT command plugin entries and information about them.
     * For debugging/bug hunting only
     *
     * @since S60 5.0
     * @param aEntries Plugin entries for support
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt PrintFoundEntries( CArrayFixFlat<TATExtOneCmdSupport>* aEntries );

    /**
     * Does a binary search for an AT command to find the AT command from the
     * support data. Combinations of aFindAuxCmd | aCheckAsterisk:
     * EFalse | EFalse: Used to find normal command from iSupport
     * ETrue  | EFalse: Used to create metadata to iSupportAux
     * EFalse | ETrue : Not possible
     * ETrue  | ETrue : Used to find metadata from iSupportAux
     *
     * @since S60 5.0
     * @param aAtCmdBase Base AT command to find (base part without parameters)
     * @param aFoundIndex If positive (found): the index of the found AT command
     *                    If negative (not found): the index where to add the
     *                    new AT command
     * @param aFindAuxCmd ETrue to find an auxiliary command,
     *                    EFalse to find a normal command
     * @param aCheckAsterisk ETrue if asterisk mark checking required
     *                       (needed for normal finding)
     *                       EFalse if asterisk mark skipping required
     *                       (needed when creating the metadata)
     * @return ETrue if support found, EFalse otherwise
     */
    TBool FindCommandFromMetadata( TDesC8& aAtCmdBase,
                                   TInt& aFoundIndex,
                                   TBool aFindAuxCmd,
                                   TBool aCheckAsterisk );

    /**
     * Finds the first plugin entry from a given plugin UID
     *
     * @since S60 5.0
     * @param aUid UID to find from plugin data
     * @param aFoundIndex Index to the found plugin entry
     * @return Found entry for UID, NULL if UID not found
     */
    TATExtPluginEntry* FindUidFromPlugindata( TUid& aUid,
                                              TInt& aFoundIndex );

    /**
     * Find the first plugin entry from a given plugin instance
     *
     * @since S60 5.0
     * @param aInstance An instance to find from plugin data
     * @return Found entry for instance, NULL if instance not found
     */
    TATExtPluginEntry* FindInstanceFromPlugindata(
        CATExtPluginBase* aInstance );

    /**
     * Finds an Urc message processing plugin. Must be used in synchronization
     * with MarkUrcHandlingOwnership(). More information in aPluginUid
     * parameter's explanation below.
     *
     * @since S60 5.0
     * @param aPluginUid Urc plugin UID to find; if not TUid::Null() then
     *                   aPluginUid must match plugin data's entry and the data
     *                   must be started and owned plugin.
     *                   Otherwise, if aPluginUid is TUid:Null() then plugin
     *                   data's entry must not be started or owned.
     * @return Found URC processing plugin, NULL if not found
     */
    TATExtPluginEntry* FindUrcProcessingPlugin( TUid& aPluginUid );

    /**
     * Extracts the base command from a given AT command to find support with
     * DoCreateAndFindSupportL()
     *
     * @since S60 5.0
     * @param aAtCmdFull Full AT command for which to check the support
     *                   (base part + parameters)
     * @param aMessage Client request message for reading
     * @param aComplInfo Completion information for the handled command
     * @return None
     */
    void CreateAndFindSupportL(
        TDesC8& aAtCmdFull,
        const RMessage2& aMessage,
        TATExtCompletionInfo& aComplInfo );

    /**
     * Creates AT command support and finds the given AT command from the
     * created support data
     *
     * @since S60 5.0
     * @param aAtCmdBase Base AT command to find from support data
     *                   (base part without parameters)
     * @param aAtCmdFull Full AT command for which to check the support
     *                   (base part + parameters)
     * @param aMessage Client request message for reading
     * @param aComplInfo Completion information for the handled command
     * @return None
     */
    void DoCreateAndFindSupportL(
        TDesC8& aAtCmdBase,
        TDesC8& aAtCmdFull,
        const RMessage2& aMessage,
        TATExtCompletionInfo& aComplInfo );

    /**
     * Prepares internal data for completion with "" or "ERROR" messages.
     * More explation in CreateEmptyOrErrorBuffer().
     *
     * @since S60 5.0
     * @param aMessage Client request message for reading
     * @return None
     */
    void CreateSelfReplyData( const RMessage2& aMessage );

    /**
     * Handles support when a master or partial plugin is detected in the plugin
     * data via support data's link. If a partial or master plugin is detected
     * then reply is expected from that plugin. Also if one or more observer
     * plugins are detected then no reply is expected from them.
     *
     * @since S60 5.0
     * @param aEntrySupport Support data to the first found partial or master plugin
     * @param aStartIndex Start index to search the next observer plugin
     * @param aReplyExpected ETrue if reply is expected from the partial or master plugin,
     *                       EFalse if no reply is expected from the partial or master plugin,
     *                       (i.e. no support found)
     * @return ETrue if support found, EFalse otherwise
     */
    TBool HandleMasterAndPartialPluginSupportL(
        TATExtEntrySupport& aEntrySupport,
        TInt aStartIndex,
        TBool& aReplyExpected );

    /**
     * Handles support when a observer plugin is detected in the plugin data
     * via support data's link. If only one observer plugin is detected then
     * reply is expected from that plugin. Instead, if more than one observer
     * plugins are detected then no reply is expected from them.
     *
     * @since S60 5.0
     * @param aEntrySupport Support data to the first found observer plugin
     *                      entry
     * @param aStartIndex Start index to search the next observer plugin
     * @param aReplyExpected ETrue if reply is expected from one or more plugins,
     *                       EFalse if no reply is expected from any of the plugins
     * @return ETrue if support found, EFalse otherwise
     */
    TBool HandleObserverPluginSupportL( TATExtEntrySupport& aEntrySupport,
                                        TInt aStartIndex,
                                        TBool& aReplyExpected );

    /**
     * Finds exclusive partial plugin support
     *
     * @since S60 5.0
     * @param aEntrySupport Support data to the first found partial plugin entry
     * @return ETrue if exclusive support found, EFalse otherwise
     */
    TBool FindExclusivePartialSupportL( TATExtEntrySupport& aEntrySupport );

    /**
     * Finds support entries from support data for a given base AT command
     *
     * @since S60 5.0
     * @param aAtCmdBase Base AT command for which to find the entries
     *                   (base part without parameters)
     * @return Found support entry array, NULL if not found
     */
    CArrayFixFlat<TATExtOneCmdSupport>* FindEntriesForCommandLC(
        TDesC8& aAtCmdBase );

    /**
     * Instantiates plugin support
     *
     * @since S60 5.0
     * @param aEntrySupport Entry support data for which to instantiate the
     *                      plugin
     * @return None
     */
    void InstantiatePluginSupportL( TATExtEntrySupport& aEntrySupport );

    /**
     * Instantiates a plugin
     *
     * @since S60 5.0
     * @param aEntrySupport Plugin entry for which to instantiate
     * @return None
     */
    void InstantiatePluginL( TATExtPluginEntry& aPluginEntry );

    /**
     * Instantiates a plugin
     *
     * @since S60 5.0
     * @param aEntrySupport Plugin entry for which to instantiate
     * @return Leave value from InstantiatePluginL()
     */
    TInt InstantiatePlugin( TATExtPluginEntry& aPluginEntry );

    /**
     * Handles an AT command support request
     *
     * @since S60 5.0
     * @param aEntrySupport Entry support data to a position where to send the
     *                      AT command for support checking
     * @param aAtCmdFull Full AT command for which to check the support
     *                   (base part + parameters)
     * @return ETrue if support found, EFalse otherwise
     */
    TBool HandleCommandSupportL( TATExtEntrySupport& aEntrySupport,
                                 const TDesC8* aAtCmdFull=NULL );

    /**
     * Handles an AT command
     *
     * @since S60 5.0
     * @param aEntrySupport Entry support data to a position where to send the
     *                      AT command for handling
     * @param aReplyNeeded ETrue if reply expected from the plugin,
     *                     EFalse otherwise
     * @param aAtCmdFull Full AT command to handle (base part + parameters)
     * @return None
     */
    void HandleCommandL( TATExtEntrySupport& aEntrySupport,
                         TBool aReplyNeeded,
                         const TDesC8* aAtCmdFull=NULL );

    /**
     * Sends an AT commands to multiple observer plugins, starting from a
     * given position.
     *
     * @since S60 5.0
     * @param aEntrySupport Entry support data to position from where to start
     *                      sending to the found observer plugins
     * @param aAtCmdFull Full AT command to send (base part + parameters)
     * @return None
     */
    void SendToMultipleObserverL( TATExtEntrySupport& aEntrySupport,
                                   const TDesC8* aAtCmdFull );

    /**
     * Finds the first observer plugin support from a given starting position
     *
     * @since S60 5.0
     * @param aEntrySupport Entry support data to the next entry after the found
     *                      observer plugin entry; marks the start for search
     * @return ETrue if support found, EFalse otherwise
     */
    TBool FindFirstObserverSupportL( TATExtEntrySupport& aEntrySupport );

    /**
     * Extracts one NVRAM entry from a pipe-character delimited NVRAM buffer
     *
     * @since S60 5.0
     * @param aNvram NVRAM setting buffer from where to extract the next
     *               subsetting
     * @param aStartIndex Start index for the found NVRAM setting
     * @param aEndIndex End index for the found NVRAM setting
     * @return ETrue if setting found, EFalse otherwise
     */
    TBool ExtractNextNvramSetting( const TDesC8& aNvram,
                                   TInt& aStartIndex,
                                   TInt& aEndIndex );

    /**
     * Gets the next command from support or auxiliary support data.
     *
     * @since S60 5.0
     * @param aFirstSearch If ETrue the search is done from the start,
     *                     If EFalse the search continues from the old position.
     * @param aSupportFind Index for finding support entries
     * @param aFindInAux Flag to indicate whether or not iSupportFind is inside
     *                   iSupportAux
     * @return Found command if not NULL, NULL otherwise
     */
    HBufC8* GetNextSupportedCommand( TBool& aFirstSearch,
                                     TInt& aSupportFind,
                                     TBool& aFindInAux );

    /**
     * Reads an AT command from a client request message and creates buffer
     * locally. For HandleCommand().
     *
     * @since S60 5.0
     * @param aMessage Client request message for reading
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ReadCommandFromMessage( const RMessage2& aMessage );

    /**
     * Creates a buffer with "ERROR" or "" string in it; needed for
     * creating a reply to an unknown command (i.e. when no plugin supports
     * the "base" part of a command) or to the case when plugin support exists
     * but reply is not expected from them.
     *
     * @since S60 5.0
     * @param aBuffer Buffer where to create the error or empty buffer
     * @param aErrorReply ETrue if error reply needed, EFalse if empty buffer
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CreateEmptyOrErrorBuffer( RBuf8& aBuffer,
                                   TBool aErrorReply );

    /**
     * Writes specified input reply buffer to a client request message at
     * given message slot number.
     *
     * @since S60 5.0
     * @param aBuffer Input buffer to write to aDataSlot
     * @param aDataSlot Message slot where to write the buffer
     * @param aMessage Client request message for writing
     * @param aReportNextLength ETrue if length reporting needed,
     *                          EFalse otherwise
     *                          This tells length of the next available
     *                          buffer length for GetNextPartOfReply()
     * @param aLengthSlot Message slot where to write the length if
     *                    aReportNextLength is ETrue. Not used if
     *                    aReportNextLength is EFalse.
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt WriteReplyBufferToClient( const TDesC8& aBuffer,
                                   TInt aDataSlot,
                                   const RMessage2& aMessage,
                                   TBool aReportNextLength=EFalse,
                                   TInt aLengthSlot=KErrNotFound );

private:  // data

    /**
     * ECOM session; used for getting ECOM implementation information.
     */
    REComSession& iEComSession;

    /**
     * ECOM plugin notification event listener.
     * Not own.
     */
    CATExtListen* iListener;

    /**
     * Plugin observer (via the parent).
     * Needed for plugin instantiation.
     */
    MATExtPluginObserver& iObserver;

    /**
     * Required interface UID of a connection (set in Connect()).
     * This UID is used to find the plugin implementation.
     */
    TUid iIfUid;

    /**
     * This connection's identifier name
     */
    RBuf8 iConnectionName;

    /**
     * Support information to plugin data. When supported AT command
     * is found then link from iSupport to iPluginData is created and
     * AT command is stored to iSupport in alphabetical order.
     * Own.
     */
    CArrayFixFlat<TATExtAtCmdSupport>* iSupport;

    /**
     * Support information to auxiliary plugin data (i.e. asterisk mark
     * terminated plugin data). When supported AT command is found then
     * link from iSupport to iPluginData is created and AT command is
     * stored to iSupport in alphabetical order.
     * Own.
     */
    CArrayFixFlat<TATExtAtCmdSupport>* iSupportAux;

    /**
     * Information about plugins and plugin-based operation data.
     * Own.
     */
    CArrayFixFlat<TATExtPluginEntry>* iPluginData;

    /**
     * Data related to AT command handling.
     */
    TATExtCommandData iCmdData;

    /**
     * Carriage return character.
     * Needed for "ERROR" reply when no support found.
     * (i.e. when no plugin supports the "base" part).
     */
    TChar iCarriageReturn;

    /**
     * Line feed character.
     * Needed for "ERROR" reply when no support found.
     * (i.e. when no plugin supports the "base" part).
     */
    TChar iLineFeed;

    /**
     * Backspace character.
     * Needed only for reporting to change to plugins.
     */
    TChar iBackspace;

    /**
     * Flag indicating whether quiet mode is on or off (ATQ)
     */
    TBool iQuietMode;

    /**
     * Flag indicating whether verbose mode is on or off (ATV)
     */
    TBool iVerboseMode;

    /**
     * Index for finding support entries.
     * Used by GetNextSpecialCommand().
     */
    TInt iSupportAuxFind;

    /**
     * Flag indicating whether shutdown is ongoing or not
     */
    TBool iShutdown;

    };

#endif  // C_CATEXTMETADATA_H
