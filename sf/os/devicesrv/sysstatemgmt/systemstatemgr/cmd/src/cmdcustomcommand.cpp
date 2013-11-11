// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmcustomcommandinfo.h>
#include <f32file.h>
#include <s32file.h>

#include "cmdcustomcommand.h"
#include "ssmcommandparameters.h"
#include "ssmcommandutilprovider.h"

#include "ssmdebug.h"
#include "ssmpanic.h"

// This file is used to store the handles of the libraries which has unload option as ENeverUnload.
// This is also defined in the CleSrv and these handles are used to release those libraries from
// CleSrv destrcutor.
_LIT(KNeverUnloadLibHandleFile, ":\\private\\2000d75b\\temp\\unloadlibhandles.bin");

/**
Used to create an instance of CCmdCustomCommand class from a read stream.
CSsmCommandList::InternalizeL() uses this method to construct a command from stream.

@param aReadStream 	Read stream containing data through which object can be created
@return				A pointer to an object of type CCmdCustomCommand.
*/
CCmdCustomCommand* CCmdCustomCommand::NewL(RReadStream& aReadStream)
	{
	CCmdCustomCommand* self = new (ELeave) CCmdCustomCommand();
	CleanupStack::PushL(self);
	self->ConstructL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Used to create an instance of CCmdCustomCommand class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity 			The severity of the command
@param aExecutionBehaviour 	Execution behaviour of this command
@param aInfo 				CustomCommand-specific info
@param aPriority 			The priority of the command in the list
@return						A pointer to an object of type CCmdCustomCommand.
*/
CCmdCustomCommand* CCmdCustomCommand::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, 
		const CSsmCustomCommandInfo& aInfo, const TUint16 aPriority)
	{
	CCmdCustomCommand* self = new (ELeave) CCmdCustomCommand(aSeverity, aExecutionBehaviour, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aInfo);
	CleanupStack::Pop(self);
	return self;
	}
#endif

/**
Used to create an instance of CCmdCustomCommand class from given parameters.
This method is used by SsmCommandFactory to create a command.

@param aSeverity 			The severity of the command
@param aExecutionBehaviour 	Execution behaviour of this command
@param aInfo 				CustomCommand-specific info
@return						A pointer to an object of type CCmdCustomCommand.
*/
CCmdCustomCommand* CCmdCustomCommand::NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo)
	{
	CCmdCustomCommand* self = new (ELeave) CCmdCustomCommand(aSeverity, aExecutionBehaviour);
	CleanupStack::PushL(self);
	self->ConstructL(aInfo);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdCustomCommand class from resource.

@param aCommandParameters 	Object data from a resource file
@return						A pointer to an object of type CCmdCustomCommand.
*/
CCmdCustomCommand* CCmdCustomCommand::NewL(TSsmCommandParameters& aCommandParameters)
	{
	CCmdCustomCommand* self = new (ELeave) CCmdCustomCommand();
	CleanupStack::PushL(self);
	self->ConstructL(aCommandParameters);
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CCmdCustomCommand class from CCmdCustomCommand object
Must be used only by CLE
@param aCmdCustomCommand CCmdCustomCommand reference 
@param aUtilProvider CSsmCommandUtilProvider reference 
@return A pointer to an object of type CCmdCustomCommand.
*/
CCmdCustomCommand* CCmdCustomCommand::NewLC(const CCmdCustomCommand& aCmdCustomCommand, CSsmCommandUtilProvider* aUtilProvider)
    {
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
    CCmdCustomCommand* self = new (ELeave) CCmdCustomCommand(aCmdCustomCommand.Severity(),aCmdCustomCommand.ExecutionBehaviour(),
                                                             aCmdCustomCommand.Priority());
#else
    CCmdCustomCommand* self = new (ELeave) CCmdCustomCommand(aCmdCustomCommand.Severity(),aCmdCustomCommand.ExecutionBehaviour());
#endif
    CleanupStack::PushL(self);
    self->ConstructL(aCmdCustomCommand, aUtilProvider);
    return self;
    }

void CCmdCustomCommand::ConstructL(const CCmdCustomCommand& aCmdCustomCommand, CSsmCommandUtilProvider* aUtilProvider)
    {
    iConditionalResourceId = aCmdCustomCommand.ConditionalInformation();
	SetUtilProvider(*aUtilProvider);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (iConditionalResourceId != 0)
        {
        SetCommandResourceFileNameL(aCmdCustomCommand.GetCommandResourceFileName());
        }
#endif
    iInfo = new (ELeave) CSsmCustomCommandInfo();
    (*iInfo) = aCmdCustomCommand.SsmCustomCommandInfo();
	}

/**
Destructor
*/	
CCmdCustomCommand::~CCmdCustomCommand()
	{
	Cancel();
	
	if (iCustomCommand)	// we requested it so we are responsible for deleting it
		{
		iCustomCommand->Release();
		}
		
	if (iInfo)
		{
		// release the DLL if instructed, DLL's with ENeverUnload option needs unloading atleast when the server dies.
		if (iInfo->Unloading() == EUnloadOnCommandCompletion)
			{
			iLibrary.Close();
			}

		delete iInfo;
		}
	if(iCmdEnv)
		{
		delete iCmdEnv;
		}
	}

/**
Initialises the BIC's active object and initiates the command
 
@param aStatus 	the TRequestStatus of the active object calling this BIC
*/	
void CCmdCustomCommand::Execute(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;
	iAction = EPrepareDll;

	if (iExecutionBehaviour != ESsmWaitForSignal)
		{
		CompleteExecuteRequest(KErrNone);
		}

	CompleteRequest(iStatus, KErrNone);
	SetActive();
	}

/**
Releases resources associated with this BIC
*/	
void CCmdCustomCommand::Release()
	{
	delete this;
	}

/**
Initiates a Cancel on the object. 
*/	
void CCmdCustomCommand::ExecuteCancel()
	{
	Cancel();
	CompleteDeferredExecuteRequest(KErrCancel);	
	}

/**
Returns the type of the BIC
 
@return The type of the BIC
*/	
TSsmCommandType CCmdCustomCommand::Type() const
	{
	return (ESsmCmdCustomCommand);
	}

/**
Returns the Version of the BIC
 
@return	The maximum supported version of the BIC
*/	
TInt CCmdCustomCommand::MaxSupportedVersion()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	return (static_cast<TInt>(ECmdCustomCommandVersionWithPriority));
#else
	return (static_cast<TInt>(ECmdCustomCommandInitialVersion));
#endif
	}

/**
Configures the BIC using data contained in a ReadStream
 
@param aReadStream 	A read stream containing BIC data
*/
void CCmdCustomCommand::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(aReadStream.ReadUint8L());

	iInfo = new (ELeave) CSsmCustomCommandInfo();
	iInfo->InternalizeL(aReadStream);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif	
	}

/**
Externalises the configuration of the BIC
 
@param aWriteStream A write stream to write BIC data to
@leave	One of the system-wide error codes.
*/
void CCmdCustomCommand::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	aWriteStream.WriteUint8L(iExecutionBehaviour);
	iInfo->ExternalizeL(aWriteStream);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
	}

/**
Performs the tasks of the BIC

@leave	One of the system-wide error codes.
*/
void CCmdCustomCommand::RunL()
	{
	switch(iAction)
		{
		case EPrepareDll:
			{
			PrepareCustomCmdL();
			break;
			}
		case EInitialiseDll:
			{
			InitialiseCmdL();
			break;
			}
		case EExecuteMethod:
			{
			SSMLOGLEAVEIFNULL(iCustomCommand);
			iCustomCommand->Execute(iInfo->Params(), iStatus);
			MoveToNextStateL(KErrNone, EClose, EFalse);	// custom command will complete this request
			break;
			}
		case EClose:
			{
			TInt err = iStatus.Int();
			if (err != KErrNone)
				{
				// custom cmd call failed so try it again
				MoveToNextStateL(err, EExecuteMethod);
				}
			else
				{
				SSMLOGLEAVEIFNULL(iCustomCommand);
				iCustomCommand->Close();
				CompleteDeferredExecuteRequest(KErrNone);		// success
				}
			break;
			}
		default:
			{
			PanicNow(KPanicCmdCustomCommand, EInvalidRunLAction);
			break;		
			}
		};
	}

/**
Called to handle any cleanup if RunL leaves
 
@param aError 	The error to finish with
@return			KErrNone
*/
TInt CCmdCustomCommand::RunError(TInt aError)
	{
	CompleteDeferredExecuteRequest(aError);
	return KErrNone;
	}

/**
Called during cancellation of the active BIC
*/
void CCmdCustomCommand::DoCancel()
	{
	if (iCustomCommand)	// only cancel if Execute has been called
		{
		iCustomCommand->ExecuteCancel();
		}
	}

/**
 Default Constructor.
 */
CCmdCustomCommand::CCmdCustomCommand()
	{
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
 Overloaded constructor.
 @param aSeverity 			The severity of the command
 @param aExecutionBehaviour The execution behaviour of the command
 @param aPriority 			The priority of the command in the list
 */
CCmdCustomCommand::CCmdCustomCommand(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour,const TUint16 aPriority)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour, aPriority)
	{
	}
#endif

/**
 Overloaded constructor.
 @param aSeverity 			The severity of the command
 @param aExecutionBehaviour The execution behaviour of the command
 */
CCmdCustomCommand::CCmdCustomCommand(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour)
	: CSsmDeferrableCommand(aSeverity, aExecutionBehaviour)
	{
	}

/**
 Constructs the object from custom command info.
 @param aInfo A custom command info object containing BIC data
*/
void CCmdCustomCommand::ConstructL(const CSsmCustomCommandInfo& aInfo)
	{
	iInfo = new (ELeave) CSsmCustomCommandInfo();
	(*iInfo) = aInfo;
	ValidateL();
	}

/**
 Constructs the object through read stream.
 @param aReadStream A read stream containing BIC data
*/
void CCmdCustomCommand::ConstructL(RReadStream& aReadStream)
	{
	InternalizeL(aReadStream);
	ValidateL();
	}

/**
 Constructs an object from resource file.
 @param aCommandParameters Object data from a resource file
*/
void CCmdCustomCommand::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	const TSsmCommandType type = static_cast<TSsmCommandType>(reader.ReadInt16L());
	SSMLOGLEAVEIFFALSE(type == Type(), KErrNotSupported);
	const TInt version = reader.ReadInt16L();
	SSMLOGLEAVEIFFALSE(__COMPARE_VERSION(version, CCmdCustomCommand::MaxSupportedVersion()), KErrNotSupported);
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iExecutionBehaviour = static_cast<TSsmExecutionBehaviour>(reader.ReadUint8L());
	const TPtrC name = reader.ReadTPtrCL();
	const TInt32 ordinal = reader.ReadInt32L();
	TCmdCustomCommandLibUnloading unloading = static_cast<TCmdCustomCommandLibUnloading>(reader.ReadUint8L());
	TInt16 retries = reader.ReadInt16L();

	// read dll data
	const TInt32 paramDataId = reader.ReadUint32L();
	HBufC8* params = NULL;
	if (paramDataId > 0)
		{
		params = aCommandParameters.AllocBufferForResourceIdL(paramDataId);
		}
	else
		{
		params = HBufC8::NewL(0);
		}
	CleanupStack::PushL(params);

	iInfo = new (ELeave) CSsmCustomCommandInfo();
	iInfo->SetL(name, ordinal, unloading, retries, *params);

	ValidateL();
	CleanupStack::PopAndDestroy(params);	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = (version > ECmdCustomCommandInitialVersion) ? reader.ReadUint16L() : KDefaultCommandPriority;
#endif	
	}

/**
 Loads and sets up the custom command dll
*/
void CCmdCustomCommand::PrepareCustomCmdL()
	{
	TInt err = KErrNone;
	if (!iLoaded)
		{
		// load the dll
		DEBUGPRINT2(_L("Loading library %S..."), &iInfo->FileName());
		err = iLibrary.Load(iInfo->FileName());
		if (err == KErrNone)
			{
			iLoaded = ETrue;
			if(iInfo->Unloading() == ENeverUnload)
				{
				//Record the handle into the file.
				//If it fails to write the handle, ignore it.
				//We don't want to stop the processing of the command due to this failure. 
				TRAP_IGNORE(WriteHandleToFileL(iLibrary.Handle()));
				}
			}
		}

	if (iLoaded)
		{
		err = KErrNotFound;

		// get a pointer to the function
		DEBUGPRINT2A("Looking up ordinal %d...", iInfo->Ordinal());
		TLibraryFunction function = iLibrary.Lookup(iInfo->Ordinal());
		if (function)
			{	
			// cast the pointer to our custom cmd type
			CustomCmdFunctionType customCmdFunction = reinterpret_cast<CustomCmdFunctionType>(function);
			iCustomCommand = (*customCmdFunction)();
			if (iCustomCommand)
				{
				err = KErrNone;
				}
			}
		}

	MoveToNextStateL(err, EInitialiseDll);
	}

void CCmdCustomCommand::WriteHandleToFileL(TInt aHandle)
	{
	const TChar sysDrive = RFs::GetSystemDriveChar();
	RBuf filename;
	filename.CreateL(KNeverUnloadLibHandleFile().Length() + 1);
	filename.Append(sysDrive);
	filename.Append(KNeverUnloadLibHandleFile());
	filename.CleanupClosePushL();
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	fs.MkDirAll(filename); // ignore any error
	RFile file;
	CleanupClosePushL(file);
	TInt err=KErrNone;
	TInt pos = 0;
	if(KErrNotFound == (err = file.Open(fs, filename, EFileShareExclusive|EFileStream|EFileWrite)))
		{
		User::LeaveIfError(file.Replace(fs, filename, EFileShareExclusive|EFileStream|EFileWrite));
		}
	else
		{
		User::LeaveIfError(err);
		file.Seek(ESeekEnd, pos);
		}
	RFileWriteStream targetStream;
	targetStream.Attach(file, pos); // file gets closed by this call, but that's okay, we don't need it any more (targetStream has its own copy of this RFile object that it owns)
	CleanupClosePushL(targetStream);
	targetStream.WriteInt32L(aHandle);
	targetStream.CommitL();
	CleanupStack::PopAndDestroy(4); 
	}

/**
 Calls the Initialize() method on the DLL
*/
void CCmdCustomCommand::InitialiseCmdL()
	{
	SSMLOGLEAVEIFNULL(iCustomCommand);
	SSMLOGLEAVEIFNULL(iUtilProvider);
	iCmdEnv = CSsmCustomCommandEnv::NewL(iUtilProvider->RfsL());
	TInt err = iCustomCommand->Initialize(iCmdEnv);
	if (err!=KErrNone)
		{
		iCustomCommand->Close();	
		delete iCmdEnv;
		iCmdEnv=NULL;
		}
	MoveToNextStateL(err, EExecuteMethod);
	}

/**
 Sets the command up to run for another iteration
*/
void CCmdCustomCommand::MoveToNextStateL(TInt aError, TCustomCmdAction aNextState, TBool aCompleteRequest)
	{
	if (aError == KErrNone)
		{
		iAction = aNextState;
		}
	else
		{
		if (++iAttempts >= iInfo->Retries())
			{
			DEBUGPRINT4A("Error encountered %d attempt %d retries %d", aError, iAttempts, iInfo->Retries());
			SSMLOGLEAVE(aError);
			}
		}

	if (aCompleteRequest)
		{
		CompleteRequest(iStatus, aError);
		}

	SetActive();
	}

/**
 Performs command-specific validation
*/
void CCmdCustomCommand::ValidateL()
	{
	CSsmDeferrableCommand::ValidateL();

	SSMLOGLEAVEIFFALSE(iInfo->FileName().Length()>0, KErrArgument);
	SSMLOGLEAVEIFFALSE(iInfo->Ordinal()>0, KErrArgument);
	SSMLOGLEAVEIFFALSE(iInfo->Unloading()>=EUnloadOnCommandCompletion, KErrArgument);
	SSMLOGLEAVEIFFALSE(iInfo->Unloading()<=ENeverUnload, KErrArgument);
	SSMLOGLEAVEIFFALSE(iInfo->Retries()>=0, KErrArgument);
	}

