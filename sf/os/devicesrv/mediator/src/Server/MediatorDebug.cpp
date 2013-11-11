/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mediator logging services implementation
*
*/



#ifdef _DEBUG

// INCLUDE FILES
#include <barsread.h>
#include <barsc.h>
#include <f32file.h>
#include <flogger.h>
#include <MediatorDebug.rsg>
#include "MediatorServerObjectHandler.h"
#include "MediatorServer.h"
#include "MediatorDebug.h"
#include "MediatorServerCommandHandler.h"
#include "MediatorServerPluginHandler.h"
#include "MediatorServerSession.h"

// CONSTANTS
_LIT( KMediatorDbgResPath,  "z:\\resource\\MediatorDebug.rsc");
_LIT(KMediatorLogFolder, "mediator"); // this will go under c:\logs
_LIT(KMediatorLogFolderFullPath, "c:\\logs\\mediator\\");
_LIT(KMediatorErrorFile, "error.txt");
_LIT(KMediatorPrintTag, "[Mediator] ");

RMediatorDebug* RMediatorDebug::iInstance = NULL;


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// RMediatorDebug::RMediatorDebug
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RMediatorDebug::RMediatorDebug()
    {
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::ConstructL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorDebug::ConstructL(CMediatorServer* aMediator)
    {
    __ASSERT_ALWAYS(aMediator, User::Panic(_L("Invalid mediator"), KErrNotFound));
    
    iMediator = aMediator;
    
    // read settings from resource files
    RResourceFile resourceFile;
    RFs fsSession;
    
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);
    
    TFileName resourceFileName (KMediatorDbgResPath);
    resourceFile.OpenL(fsSession, resourceFileName);
    
    CleanupClosePushL(resourceFile);
    
    // Read the first resource
    HBufC8* res = resourceFile.AllocReadLC(DEBUG_CONF);
    TResourceReader theReader;
    theReader.SetBuffer(res);

       
    iDomainOptions = static_cast<EDebugLogMode>(theReader.ReadUint8());
    
    
    TInt domainCount = theReader.ReadUint16();
    
    // read domain UIDs
    TUid uid;
        
    for (TInt i = 0; i < domainCount; i++)
        {
        uid.iUid = theReader.ReadUint32();
        iDomains.AppendL(uid);
        }    

        
    iCategoryOptions = static_cast<EDebugLogMode>(theReader.ReadUint8());

    TInt categoryCount = theReader.ReadUint16();
    
    // read category UIDs
    for (TInt i = 0; i < categoryCount; i++)
        {
        uid.iUid = theReader.ReadUint32();
        iDomains.AppendL(uid);
        }
       
    // read the generic options
    iOptions = theReader.ReadUint8();
    
    // read output file
    TPtrC outFile = theReader.ReadTPtrC();
    
    // if output file is specified, then file logging must also be done
    if (outFile.Length() != 0)
        {
        // create directory if it does not exist
        TInt err = fsSession.MkDir( KMediatorLogFolderFullPath );
        
        if ( err != KErrNone && err != KErrAlreadyExists )
            {
            User::Leave( err );
            }
        
        // open and configure log
        User::LeaveIfError( iFileLogger.Connect() );
        
        iFileLogger.CreateLog( KMediatorLogFolder, outFile, EFileLoggingModeAppend );
        
        iFileLogger.SetDateAndTime( EFalse, ETrue );
        }
    
    // Make print buffer ready. During normal operation tag is inserted in PrintL.
    iBuf.Append( KMediatorPrintTag );
    
    CleanupStack::PopAndDestroy( res );
    CleanupStack::PopAndDestroy( &resourceFile ); 
    CleanupStack::PopAndDestroy( &fsSession );
 
    PrintConfigurationL();
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::NewL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RMediatorDebug* RMediatorDebug::NewL(CMediatorServer* aMediator)
    {
    RMediatorDebug* self = new( ELeave ) RMediatorDebug;
    
    CleanupStack::PushL( self );
    self->ConstructL(aMediator);
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::~RMediatorDebug
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
RMediatorDebug::~RMediatorDebug()
    {
    iDomains.Close();
    iCategories.Close();
    iFileLogger.CloseLog();
    iFileLogger.Close();
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::ShouldPrintLogL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool RMediatorDebug::ShouldPrintLogL(const RMessage2& aMessage, CMediatorServerSession& aSession)
    {
    if (!(KLogEvents & iOptions) && aMessage.Function() == ERaiseEvent)
        {
        return EFalse;
        }
        
    if (!(KLogCommands & iOptions) && 
         (aMessage.Function() == EIssueCommand || aMessage.Function() == EIssueResponse || aMessage.Function() == ECancelCommand))
        {
        return EFalse;
        }
    
    TBool hasDomain = HasDomain( aMessage );
    
    // check whether this domain should be logged
    if ( hasDomain && (iDomainOptions != ELogAll) )
        {
        TMediatorCategory categ = aSession.ReadCategoryL(aMessage, 0);
        
        TInt idx = iDomains.Find(categ.iDomain);
        
        if ((idx == KErrNotFound && iDomainOptions == ELogInclusive) 
            || (idx != KErrNotFound && iDomainOptions == ELogExclusive))
            {
            return EFalse;
            }
        }
    else if ( !hasDomain && ( iDomainOptions == ELogInclusive ) )
        {
        // If inclusive logging is specified, drop also domainless operations
        return EFalse;
        }
    
    TBool hasCategory = HasCategory( aMessage );
    
    // check whether this category should be logged
    if ( hasCategory && (iCategoryOptions != ELogAll) )
        {
        TMediatorCategory categ = aSession.ReadCategoryL(aMessage, 0);
        
        TInt idx = iCategories.Find(categ.iCategory);
        
        if ((idx == KErrNotFound && iCategoryOptions == ELogInclusive) 
            || (idx != KErrNotFound && iCategoryOptions == ELogExclusive))
            {
            return EFalse;
            }
        }
    else if ( !hasCategory && (iCategoryOptions == ELogInclusive) )
        {
        // If inclusive logging is specified, drop also category-less operations
        return EFalse;
        }        
    
    // all filters passed - log this
    return ETrue;
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::OperationString
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC RMediatorDebug::OperationString(TInt aOperation)
    {
    if (aOperation == ERegisterEventList) return TPtrC(_L("ERegisterEventList"));
    if (aOperation == ERegisterCommandList) return TPtrC(_L("ERegisterCommandList"));
    if (aOperation == EUnregisterEventList) return TPtrC(_L("EUnregisterEventList"));
    if (aOperation == EUnregisterCommandList) return TPtrC(_L("EUnregisterCommandList"));
    if (aOperation == ERaiseEvent) return TPtrC(_L("ERaiseEvent"));
    if (aOperation == ESubscribeEventList) return TPtrC(_L("ESubscribeEventList"));
    if (aOperation == EUnsubscribeEventList) return TPtrC(_L("EUnsubscribeEventList"));
    if (aOperation == EIssueCommand) return TPtrC(_L("EIssueCommand"));
    if (aOperation == ECancelCommand) return TPtrC(_L("ECancelCommand"));
    if (aOperation == EIssueResponse) return TPtrC(_L("EIssueResponse"));
    if (aOperation == EWaitForEvent) return TPtrC(_L("EWaitForEvent"));
    if (aOperation == EWaitForCommand) return TPtrC(_L("EWaitForCommand"));
    if (aOperation == EWaitForCommandResponse) return TPtrC(_L("EWaitForCommandResponse"));
    if (aOperation == EWaitForNotifications) return TPtrC(_L("EWaitForNotifications"));
    if (aOperation == ECancelNotifications) return TPtrC(_L("ECancelNotifications"));
    if (aOperation == EGetDomains) return TPtrC(_L("EGetDomains"));
    if (aOperation == EGetCategories) return TPtrC(_L("EGetCategories"));
    if (aOperation == EGetEvents) return TPtrC(_L("EGetEvents"));
    if (aOperation == EGetCommands) return TPtrC(_L("EGetCommands"));
    if (aOperation == EFetchParameterData) return TPtrC(_L("EFetchParameterData"));
    if (aOperation == ECancelAll) return TPtrC(_L("ECancelAll"));
    if (aOperation == EFetchNotificationEventList) return TPtrC(_L("EFetchNotificationEventList"));
    if (aOperation == EFetchNotificationCommandList) return TPtrC(_L("EFetchNotificationCommandList"));
    
    return TPtrC(_L("Unknown operation"));
    
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::ErrorString
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC RMediatorDebug::ErrorString(TInt aError)
    {
    // Mediator errors
    if (aError == KErrMediatorDomainNotFound) return TPtrC(_L("KErrMediatorDomainNotFound"));
    if (aError == KErrMediatorCategoryNotFound) return TPtrC(_L("KErrMediatorCategoryNotFound"));
    if (aError == KErrMediatorCommandNotFound) return TPtrC(_L("KErrMediatorCommandNotFound"));
    if (aError == KErrMediatorEventNotFound) return TPtrC(_L("KErrMediatorEventNotFound"));
    if (aError == KErrMediatorCommandAlreadyExists) return TPtrC(_L("KErrMediatorCommandAlreadyExists"));
    if (aError == KErrMediatorEventAlreadyExists) return TPtrC(_L("KErrMediatorEventAlreadyExists"));
    if (aError == KErrMediatorAlreadySubscribed) return TPtrC(_L("KErrMediatorAlreadySubscribed"));
    if (aError == KErrMediatorNoSubscription) return TPtrC(_L("KErrMediatorNoSubscription"));
    if (aError == KErrMediatorSecureIdMismatch) return TPtrC(_L("KErrMediatorSecureIdMismatch"));
    if (aError == KErrMediatorVersionMismatch) return TPtrC(_L("KErrMediatorVersionMismatch"));
    if (aError == KErrMediatorTimeout) return TPtrC(_L("KErrMediatorTimeout"));
    if (aError == KErrMediatorCommandRemoved) return TPtrC(_L("KErrMediatorCommandRemoved"));
    
    // Symbian standard errors
    if (aError == KErrNone) return TPtrC(_L("KErrNone"));                   
    if (aError == KErrNotFound) return TPtrC(_L("KErrNotFound"));
    if (aError == KErrGeneral) return TPtrC(_L("KErrGeneral"));
    if (aError == KErrCancel) return TPtrC(_L("KErrCancel"));
    if (aError == KErrNoMemory) return TPtrC(_L("KErrNoMemory"));
    if (aError == KErrNotSupported) return TPtrC(_L("KErrNotSupported"));
    if (aError == KErrArgument) return TPtrC(_L("KErrArgument"));
    if (aError == KErrTotalLossOfPrecision) return TPtrC(_L("KErrTotalLossOfPrecision"));
    if (aError == KErrBadHandle) return TPtrC(_L("KErrBadHandle"));
    if (aError == KErrOverflow) return TPtrC(_L("KErrOverflow"));
    if (aError == KErrUnderflow) return TPtrC(_L("KErrUnderflow"));
    if (aError == KErrAlreadyExists) return TPtrC(_L("KErrAlreadyExists"));
    if (aError == KErrPathNotFound) return TPtrC(_L("KErrPathNotFound"));
    if (aError == KErrDied) return TPtrC(_L("KErrDied"));
    if (aError == KErrInUse) return TPtrC(_L("KErrInUse"));
    if (aError == KErrServerTerminated) return TPtrC(_L("KErrServerTerminated"));
    if (aError == KErrServerBusy) return TPtrC(_L("KErrServerBusy"));
    if (aError == KErrCompletion) return TPtrC(_L("KErrCompletion"));
    if (aError == KErrNotReady) return TPtrC(_L("KErrNotReady"));
    if (aError == KErrUnknown) return TPtrC(_L("KErrMediatorDomainNotFound"));
    if (aError == KErrCorrupt) return TPtrC(_L("KErrCorrupt"));
    if (aError == KErrAccessDenied) return TPtrC(_L("KErrAccessDenied"));
    if (aError == KErrLocked) return TPtrC(_L("KErrLocked"));
    if (aError == KErrWrite) return TPtrC(_L("KErrWrite"));
    if (aError == KErrDisMounted) return TPtrC(_L("KErrMediatorDomainNotFound"));
    if (aError == KErrEof) return TPtrC(_L("KErrEof"));
    if (aError == KErrDiskFull) return TPtrC(_L("KErrDiskFull"));
    if (aError == KErrBadDriver) return TPtrC(_L("KErrBadDriver"));
    if (aError == KErrBadName) return TPtrC(_L("KErrBadName"));
    if (aError == KErrCommsLineFail) return TPtrC(_L("KErrCommsLineFail"));
    if (aError == KErrCommsFrame) return TPtrC(_L("KErrCommsFrame"));
    if (aError == KErrCommsOverrun) return TPtrC(_L("KErrCommsOverrun"));
    if (aError == KErrCommsParity) return TPtrC(_L("KErrCommsParity"));
    if (aError == KErrTimedOut) return TPtrC(_L("KErrTimedOut"));
    if (aError == KErrCouldNotConnect) return TPtrC(_L("KErrCouldNotConnect"));
    if (aError == KErrCouldNotDisconnect) return TPtrC(_L("KErrCouldNotDisconnect"));
    if (aError == KErrDisconnected) return TPtrC(_L("KErrDisconnected"));
    if (aError == KErrBadLibraryEntryPoint) return TPtrC(_L("KErrBadLibraryEntryPoint"));
    if (aError == KErrBadDescriptor) return TPtrC(_L("KErrBadDescr"));
    if (aError == KErrAbort) return TPtrC(_L("KErrAbort"));
    if (aError == KErrTooBig) return TPtrC(_L("KErrTooBig"));
    if (aError == KErrDivideByZero) return TPtrC(_L("KErrDivideByZero"));
    if (aError == KErrBadPower) return TPtrC(_L("KErrBadPower"));
    if (aError == KErrDirFull) return TPtrC(_L("KErrDirFull"));
    if (aError == KErrHardwareNotAvailable) return TPtrC(_L("KErrHardwareNotAvailable"));
    if (aError == KErrSessionClosed) return TPtrC(_L("KErrSessionClosed"));
    if (aError == KErrPermissionDenied) return TPtrC(_L("KErrPermissionDenied"));
    if (aError == KErrExtensionNotSupported) return TPtrC(_L("KErrExtensionNotSupported"));
    if (aError == KErrCommsBreak) return TPtrC(_L("KErrCommsBreak"));
    
    return TPtrC(_L("Unknown error"));
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::CapabilityString
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TPtrC RMediatorDebug::CapabilityString( TCapability aCapability )
    {
    
    if ( aCapability == ECapabilityTCB ) return TPtrC( _L("TCB") );
	if ( aCapability == ECapabilityCommDD ) return TPtrC( _L("CDD") );
	if ( aCapability == ECapabilityPowerMgmt ) return TPtrC( _L("PM") );
	if ( aCapability == ECapabilityMultimediaDD ) return TPtrC( _L("MDD") );
	if ( aCapability == ECapabilityReadDeviceData ) return TPtrC( _L("RDD") );
	if ( aCapability == ECapabilityWriteDeviceData ) return TPtrC( _L("WDD") );
	if ( aCapability == ECapabilityDRM ) return TPtrC( _L("DRM") );
	if ( aCapability == ECapabilityTrustedUI ) return TPtrC( _L("TUI") );
	if ( aCapability == ECapabilityProtServ ) return TPtrC( _L("PS") );
	if ( aCapability == ECapabilityDiskAdmin ) return TPtrC( _L("DA") );
	if ( aCapability == ECapabilityNetworkControl ) return TPtrC( _L("NC") );
	if ( aCapability == ECapabilityAllFiles ) return TPtrC( _L("AF") );
	if ( aCapability == ECapabilitySwEvent ) return TPtrC( _L("SE") );
	if ( aCapability == ECapabilityNetworkServices ) return TPtrC( _L("NS") );
	if ( aCapability == ECapabilityLocalServices ) return TPtrC( _L("LS") );
	if ( aCapability == ECapabilityReadUserData ) return TPtrC( _L("RUD") );
	if ( aCapability == ECapabilityWriteUserData ) return TPtrC( _L("WUD") );
	if ( aCapability == ECapabilityLocation ) return TPtrC( _L("Loc") );
	if ( aCapability == ECapabilitySurroundingsDD ) return TPtrC( _L("SDD") );
	if ( aCapability == ECapabilityUserEnvironment ) return TPtrC( _L("UE") );
	
	return TPtrC( _L("Unknown") ); // new capabilities declared in system
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::HasDomain
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool RMediatorDebug::HasDomain(const RMessage2& aMessage)   
    {
    switch (aMessage.Function())
        {
        case ERegisterEventList:
        case ERegisterCommandList:
        case EUnregisterEventList:
        case EUnregisterCommandList:
        case ERaiseEvent:
        case ESubscribeEventList:
        case EUnsubscribeEventList:
        case EIssueCommand:
        case ECancelCommand:
        case EIssueResponse:
        case EGetEvents:
        case EGetCommands:
        case EGetCategories:
            return ETrue;

        default:
        case EWaitForEvent:
        case EWaitForCommand:
        case EWaitForCommandResponse:
        case EWaitForNotifications:
        case ECancelNotifications:
        case EGetDomains:
        case ECancelAll:
        case EFetchParameterData:
        case EFetchNotificationEventList:
        case EFetchNotificationCommandList:
            return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::HasCategory
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool RMediatorDebug::HasCategory(const RMessage2& aMessage)   
    {
    switch (aMessage.Function())
        {
        case ERegisterEventList:
        case ERegisterCommandList:
        case EUnregisterEventList:
        case EUnregisterCommandList:
        case ERaiseEvent:
        case ESubscribeEventList:
        case EUnsubscribeEventList:
        case EIssueCommand:
        case ECancelCommand:
        case EIssueResponse:
        case EGetEvents:
        case EGetCommands:
            return ETrue;
        
        default:
        case EGetCategories:
        case EWaitForEvent: 
        case EWaitForCommand:
        case EWaitForCommandResponse:
        case EWaitForNotifications:
        case ECancelNotifications:
        case EGetDomains:
        case ECancelAll:
        case EFetchParameterData:
        case EFetchNotificationEventList:
        case EFetchNotificationCommandList:
            return EFalse;
        }
    }
    
// -----------------------------------------------------------------------------
// RMediatorDebug::PrintOperationL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void RMediatorDebug::PrintOperationL(const RMessage2& aMessage, CMediatorServerSession& aSession)
    {
    
    if (!iInstance->ShouldPrintLogL(aMessage, aSession))
        {
        return;
        }
    
    TPtrC opString = OperationString( aMessage.Function() );
    iBuf.AppendFormat( _L("%S"), &iOf, &opString );
        
    iBuf.AppendFormat(_L(" issued by 0x%08X(SID)"), &iOf, (TUint32)aMessage.SecureId());
    
    if (HasDomain(aMessage))
        {
        TMediatorCategory categ = aSession.ReadCategoryL(aMessage, 0);
        iBuf.AppendFormat(_L(" D:0x%08X"), &iOf, categ.iDomain);
        
        if (HasCategory(aMessage))
            {
            iBuf.AppendFormat(_L(" C:0x%08X"), &iOf, categ.iCategory);
            }
        }
    
    PrintL();
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintPluginL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorDebug::PrintPluginL(const CImplementationInformation* aImpl)
    {
    TPtrC implName( aImpl->DisplayName() );
    
    iBuf.AppendFormat( _L("Loading plugin %S, UID=0x%08X"), &iOf,
                                                            &implName,
                                                            aImpl->ImplementationUid() );
    
    PrintL();
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::AppendCapabilitiesL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
void RMediatorDebug::AppendCapabilitiesL( const TCapabilitySet& aCaps )
    {
    TBool capsFound = EFalse;
    
    for ( TInt i = 0; i < ECapability_Limit; i++ )
        {
        if ( aCaps.HasCapability( static_cast<TCapability>( i ) ) )
            {
            TPtrC capName( CapabilityString( static_cast<TCapability>( i ) ) );
            
            iBuf.AppendFormat( _L(" %S"), &iOf, &capName );
            
            capsFound = ETrue;

            }
        }
        
    if ( !capsFound ) // no capabilities specified
        {
        _LIT( KTextNone, "None");
        TPtrC textNone(KTextNone);
        iBuf.AppendFormat( _L("%S"), &iOf, &textNone );
        }
    
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintEventL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void RMediatorDebug::PrintEventL(const RMessage2& aMessage, CMediatorServerSession& aSession, const TEvent& aEvent, TInt aEventNmbr)
    {
    if (!iInstance->ShouldPrintLogL(aMessage, aSession))
        {
        return;
        }
        
    iBuf.AppendFormat(_L("\tEVENT%03d ID:%d VER:%d.%d.%d CAPS:"), &iOf, aEventNmbr, aEvent.iEventId,
                         aEvent.iVersion.iMajor, aEvent.iVersion.iMinor, aEvent.iVersion.iBuild);
                         

    AppendCapabilitiesL( aEvent.iCaps );
    
    PrintL();
    
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintEventListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorDebug::PrintEventListL(const RMessage2& aMessage, CMediatorServerSession& aSession, const REventList& aList)
    {
    if (!iInstance->ShouldPrintLogL(aMessage, aSession))
        {
        return;
        }
        
    for( TInt i = 0; i < aList.Count(); i++)
        {
        PrintEventL(aMessage, aSession, aList[i], i);
        }
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintCommandL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void RMediatorDebug::PrintCommandL(const RMessage2& aMessage, CMediatorServerSession& aSession, const TCommand& aCmd, TInt aCmdNmbr)
    {
    if (!iInstance->ShouldPrintLogL(aMessage, aSession))
        {
        return;
        }
        
    iBuf.AppendFormat(_L("\tCMD%03d ID:%d VER:%d.%d.%d TOUT:%d CAPS:"), &iOf, aCmdNmbr, aCmd.iCommandId,
                         aCmd.iVersion.iMajor, aCmd.iVersion.iMinor, aCmd.iVersion.iBuild, aCmd.iTimeout);

    AppendCapabilitiesL( aCmd.iCaps );
                         
    PrintL();
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintCommandListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void RMediatorDebug::PrintCommandListL(const RMessage2& aMessage, CMediatorServerSession& aSession, const RCommandList& aList)
    {
    if (!iInstance->ShouldPrintLogL(aMessage, aSession))
        {
        return;
        }
        
    
    for( TInt i = 0; i < aList.Count(); i++)
        {
        PrintCommandL(aMessage, aSession, aList[i], i);
        }
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintCategoryListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorDebug::PrintCategoryListL(const RMessage2& aMessage, CMediatorServerSession& aSession, const RCategoryList& aList)
    {
    if (!iInstance->ShouldPrintLogL(aMessage, aSession))
        {
        return;
        }
        
    _LIT(KDomain, " 0x%08X");
    
    // if domain is specified with this request, then it is submitted for querying categories
    if (HasDomain(aMessage))
        {
        iBuf.AppendFormat(_L("Categories(count=%d):"), &iOf, aList.Count());    
        }
    else
        {
        iBuf.AppendFormat(_L("Domains(count=%d):"), &iOf, aList.Count());    
        }
    
    for (TInt i = 0; i < aList.Count(); i++)
        {
        iBuf.AppendFormat(KDomain, &iOf, aList[i].iUid);
        }
    
    PrintL();
    }
    
// -----------------------------------------------------------------------------
// RMediatorDebug::PrintRawL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorDebug::PrintRawL(const RMessage2& aMessage, CMediatorServerSession& aSession, const TDesC8& aData)
    {
    if (!iInstance->ShouldPrintLogL(aMessage, aSession))
        {
        return;
        }
        
    // printing parameters may be flagged out
    if ((iOptions & KLogParameterData))
        {
        const TInt KRowLen = 32;
        
        _LIT(KFormatData, " %02X");
        _LIT(KFormatLine, "\tPARAM%03d :" );
        TInt dataLen = aData.Length();
        const TUint8* data = aData.Ptr();
        
        // print parameter data as an hexdump, 32 bytes each row
        // PARAM000: 00 AA BB CC DD EE ...
        while (dataLen > 0)
            {
            iBuf.AppendFormat(KFormatLine, &iOf, ((data-aData.Ptr())/KRowLen));
            
            for (TInt i = 0; i < KRowLen; i++)
                {
                
                if (--dataLen < 0)
                    {
                    break;
                    }
                    
                iBuf.AppendFormat(KFormatData, &iOf, *data++);
                
                }
            
            PrintL();
            }
        }
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintErrorL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
void RMediatorDebug::PrintErrorL(const RMessage2& aMessage, TInt aError)
    {
    TPtrC opString = OperationString( aMessage.Function() );
    TPtrC errString = ErrorString( aError );
    
    iBuf.AppendFormat( _L("Server responded to operation %S with error %S (%d)"), &iOf,
                                                                                  &opString,
                                                                                  &errString,
                                                                                  aError );

    PrintL();
    }
    
// -----------------------------------------------------------------------------
// RMediatorDebug::PrintServerStatusL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void RMediatorDebug::PrintServerStatusL()    
    {
    
    if (!(iOptions & KLogStatus))
    {
    return;
    }
    
    RThread thread;
    TInt allocSize = 0;
    User::Heap().AllocSize( allocSize );
    
    iBuf.Append( _L("-- MEDIATOR STATUS START --") );
    PrintL();    
    
    iBuf.AppendFormat(_L("\t\tMemory allocated:%d"), &iOf, allocSize);
    PrintL();
    
    iBuf.AppendFormat(_L("\t\tPlugins loaded: %d"), &iOf, iMediator->PluginHandler().PluginCount());
    PrintL();
    
    TInt categCount;
    TInt domainCount;
    TInt observerCount;
    TInt eventCount;
    TInt commandCount;
        
    iMediator->ObjectHandler().GetStatistics(domainCount, categCount, eventCount, commandCount, observerCount);
    
    iBuf.AppendFormat(_L("\t\tDomains: %d\tCategories: %d\tObservers: %d"), &iOf, domainCount, categCount, observerCount);
    PrintL();
    
    iBuf.AppendFormat(_L("\t\tEvents: %d"), &iOf, eventCount);
    PrintL();
    
    iBuf.AppendFormat(_L("\t\tCommands: %d"), &iOf, commandCount);
    PrintL();
    
    TPtrC statusEnd( _L("-- MEDIATOR STATUS END --") );
    iBuf.AppendFormat(_L("\t\tPending commands: %d"), &iOf, 
                                                      iMediator->CommandHandler().CommandPendingCount() );
    PrintL();
    
    
    iBuf.Append( _L("-- MEDIATOR STATUS END --") );
    PrintL(); 
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintStartupConfigurationL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void RMediatorDebug::PrintConfigurationL()
    {
    
    iBuf.Append( _L("Server side logging service initialized.") );
    PrintL();
    
    iBuf.Append( _L("Logging filters status (set in MediatorDebug.rss):") );
    PrintL();
    
    TPtrC filterOff( _L( "Off" ) );
    TPtrC filterOn( _L( "On" ) );
    TPtrC modeInclusive( _L( "Inclusive" ) );
    TPtrC modeExclusive( _L( "Exclusive") );
    TPtrC modeNotFiltered( _L("Not applied") );
    TDesC* currMode;
    
    switch ( iDomainOptions )
        {
        default:
        case ELogAll:
            currMode = &modeNotFiltered;
            break;
        
        case ELogInclusive:
            currMode = &modeInclusive;
            break;
            
        case ELogExclusive:
            currMode = &modeExclusive;
            break;
        }
    
    iBuf.AppendFormat( _L("\tDomain filtering: %S"), &iOf, currMode );
    PrintL();
    
    switch ( iCategoryOptions )
        {
        default:
        case ELogAll:
            currMode = &modeNotFiltered;
            break;
        
        case ELogInclusive:
            currMode = &modeInclusive;
            break;
            
        case ELogExclusive:
            currMode = &modeExclusive;
            break;
        }
        
    iBuf.AppendFormat( _L("\tCategory filtering: %S"), &iOf, currMode );
    PrintL();
    
    currMode = ( iOptions & KLogEvents ) ? &filterOn : &filterOff;
    iBuf.AppendFormat( _L("\tEvent logging: %S") , &iOf, currMode);
    PrintL();
    
    currMode = ( iOptions & KLogCommands ) ? &filterOn : &filterOff;
    iBuf.AppendFormat( _L("\tCommand logging: %S") , &iOf, currMode);
    PrintL();
    
    currMode = ( iOptions & KLogStatus ) ? &filterOn : &filterOff;
    iBuf.AppendFormat( _L("\tStatus report: %S") , &iOf, currMode);
    PrintL();
    
    currMode = ( iOptions & KLogParameterData ) ? &filterOn : &filterOff;
    iBuf.AppendFormat( _L("\tParameter data logging: %S") , &iOf, currMode);
    PrintL();
    
    iBuf.Append( _L("In order to enable function tracing define flag _DEBUG_MEDIATOR in debug.h") );
    PrintL();
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::PrintL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void RMediatorDebug::PrintL()
    {
    // check that zero termination can be done
    if (iBuf.Length() >= iBuf.MaxLength())
        {
        User::Leave(KErrOverflow);
        }
    
    iBuf.ZeroTerminate();
    
    Print(iBuf);
    
    iBuf.Zero(); // so that next caller can append text to this buffer
    iBuf.Append( KMediatorPrintTag ); // ready for next print
    }
    
// -----------------------------------------------------------------------------
// RMediatorDebug::Print
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void RMediatorDebug::Print(const TDesC& aText)
    {
    // print this text to com port and/or to a file
    iFileLogger.Write( aText ); // safe to call even if session is not opened
    
    RDebug::Print( aText );
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::HandleInternalError
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorDebug::HandleInternalError(TInt aError)
    {
        if (aError != KErrNone)
        {
        const TInt KMsgLen = 100;
        TBuf<KMsgLen> errMsg;
        errMsg.Format(_L("Log writing failed with error %d"), aError);
        errMsg.ZeroTerminate();
        Print(errMsg);
        }
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::HandleInitError
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorDebug::HandleInitError( TInt aError )
    {
    const TInt KMsgLen = 100;
    TBuf<KMsgLen> errorMsg;
    errorMsg.AppendFormat(_L("[Mediator] RMediatorDebug::Initialize failed with code %d. Debug services not available."), aError );
    errorMsg.ZeroTerminate();
    RDebug::Print(errorMsg);
    RFileLogger::WriteFormat(KMediatorLogFolder, KMediatorErrorFile, EFileLoggingModeAppend, errorMsg);
    }
#endif // _DEBUG

//  End of File
