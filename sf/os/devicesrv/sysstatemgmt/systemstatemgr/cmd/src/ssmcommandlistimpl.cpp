// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32strm.h>
#include <ssm/ssmstatemanager.h>
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
#include <e32cmn.h>
#endif
#include <ssm/ssmcommand.h>
#include <ssm/ssmcommandfactory.h>
#include "ssmcommandlistimpl.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include "ssmcommandbase.h"
#include "ssmcommandparameters.h"
#include "ssmdeferreddeleter.h"

#define __IN_RANGE( x, y ) ( (x >= 0) && (x < y) )


//
// Construct a CSsmCommandListImplImpl object
//
CSsmCommandListImpl* CSsmCommandListImpl::NewL()
	{
	CSsmCommandListImpl* self = new(ELeave) CSsmCommandListImpl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Used to create an instance of CSsmCommandListImpl class from CSsmCommandListImpl object
Must be used only by CLE
@param aSsmCommandListImpl CSsmCommandListImpl reference
@param aUtilProvider CSsmCommandUtilProvider pointer
@return A pointer to an object of type CSsmCommandListImpl.
*/
CSsmCommandListImpl* CSsmCommandListImpl::NewL(const CSsmCommandListImpl* aSsmCommandListImpl, CSsmCommandUtilProvider* aUtilProvider)
    {
    CSsmCommandListImpl* self = new(ELeave) CSsmCommandListImpl;
    CleanupStack::PushL(self);
    self->ConstructL(aSsmCommandListImpl, aUtilProvider);
    CleanupStack::Pop(self);
    return self;
    }

CSsmCommandListImpl::CSsmCommandListImpl()
	{
	}

void CSsmCommandListImpl::ConstructL()
	{
//iDeferredDeleter is initialized inside the ConstuctL... when CLE constructs the commandlist
//where as for unit testing we need to construct the iDeferredDeleter here for testing purposes
#ifdef SSM_CMD_TESTFRAMEWORK
	iDeferredDeleter = CSsmDeferredDeleter::NewL();
#endif
	iResourceFileName.CreateL(0);
	}
/**
Used to reconstruct the commandlist from the commandlist recieved from SSM server.
It should be used only by CLE.
@param CSsmCommandListImpl aSsmCommandListImpl pointer 
@param CSsmCommandUtilProvider aUtilProvider pointer
 */
void CSsmCommandListImpl::ConstructL(const CSsmCommandListImpl* aSsmCommandListImpl, CSsmCommandUtilProvider* aUtilProvider)
    {
    SetDelayBetweenCommands(aSsmCommandListImpl->DelayBetweenCommands());

    // It is ok for this pointer to be passed between threads as the policy
    // will remain loaded until CLE returns
    SetConditionalCallback(aSsmCommandListImpl->ConditionalCallback());
    
    iDeferredDeleter = CSsmDeferredDeleter::NewL();
    
#ifndef SYMBIAN_SSM_FLEXIBLE_MERGE
    // Write out the resource filename
    iResourceFileName.ReAllocL(aSsmCommandListImpl->iResourceFileName.Size());
#endif

    const TInt count = aSsmCommandListImpl->iCmdList.Count();
    TArray<MSsmCommand*> deferredList = PendingDeferred();
    for (TInt i = 0; i < count; i++)
        {
        CSsmCommandBase* cmd = SsmCommandFactory::ConstructCommandFromCommandLC(*aSsmCommandListImpl->iCmdList[i], deferredList,aUtilProvider);
        
        const TSsmCommandType cmdType = cmd->Type();
        
        if (ESsmCmdMultipleWait == cmdType)
            {
            iDeferredList.Reset();
            }
        else if (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()) 
            {
            iDeferredList.AppendL(cmd);
            }
        iCmdList.AppendL(cmd);
        CleanupStack::Pop(cmd);
        }
    }

//
// Destructor
//
CSsmCommandListImpl::~CSsmCommandListImpl()
	{
	const TInt count = iCmdList.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iCmdList[i])
			iCmdList[i]->Release();
		}

	iCmdList.Close();
	iDeferredList.Close();
	
	delete iDeferredDeleter;
	delete iResourceFile;
	iResourceFileName.Close();
	}


//
// Read a set of commands into the list from a stream
//
void CSsmCommandListImpl::InternalizeL( RReadStream& aReadStream , CSsmCommandUtilProvider* aUtilProvider )
	{
	if( (0 != iCmdList.Count()) ||  (0 != iDeferredList.Count()) )
		{
		SSMLOGLEAVE( KErrAlreadyExists );
		}

	iDelayBetweenCommands = aReadStream.ReadInt32L();
	// It is ok for this pointer to be passed between threads as the policy
	// will remain loaded until CLE returns
	iConditionalCallback = reinterpret_cast<MSsmConditionalCallback*>(aReadStream.ReadInt32L());
	
	// Read in the resource filename
	iResourceFileName.ReAllocL(aReadStream.ReadInt32L());
	aReadStream.ReadL(iResourceFileName);
	
	TUint32 count = aReadStream.ReadUint32L();
	while (count--)
		{
		TInt conditionalInformation = aReadStream.ReadInt32L();
		const TSsmCommandType type = static_cast<TSsmCommandType>(aReadStream.ReadUint16L());
		TArray<MSsmCommand*> deferredList = PendingDeferred();
		CSsmCommandBase* cmd = SsmCommandFactory::ConstructCommandFromStreamLC(type, aReadStream, deferredList);
		
		// The ssmswppolicyserver and ssmswppolicycli code internalizes and externalizes 
		// the commandlist in order to pass the it from server to the client. 
		// This commandlist is not executed by ssmswppolicycli, it just passes it to the cle where the commandlist is executed 
		// and hence it does not require the utilprovider to be passed in to this method.
		if (aUtilProvider)
			{
			cmd->SetUtilProvider(*aUtilProvider);
			}
		else
			{
			DEBUGPRINT1(_L("CSsmCommandListImpl::InternalizeL used by  ssmswppolicycli code so Utilprovider is not required"));
			}

		cmd->SetConditionalInformation(conditionalInformation);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
		if( cmd->ConditionalInformation() != 0 )
			{
			HBufC* tempFileName = HBufC::NewLC(aReadStream, KMaxFileName);
			cmd->SetCommandResourceFileNameL(*tempFileName);
			CleanupStack::PopAndDestroy(tempFileName);
			}
#endif
		AppendL(cmd);
		CleanupStack::Pop(cmd);
		}
	
	}


//
// Write a set of commands from the list to a stream
//
void CSsmCommandListImpl::ExternalizeL( RWriteStream& aWriteStream ) const
	{
	aWriteStream.WriteInt32L(iDelayBetweenCommands);
	// It is ok for this pointer to be passed between threads as the policy
	// will remain loaded until CLE returns
	aWriteStream.WriteInt32L(reinterpret_cast<TInt>(iConditionalCallback));
	
	// Write out the resource filename
	aWriteStream.WriteInt32L(iResourceFileName.Length());
	aWriteStream.WriteL(iResourceFileName);
	
	const TInt count = iCmdList.Count();
	aWriteStream.WriteUint32L(count);

	for (TInt i = 0; i < count; i++)
		{
		const MSsmCommand* const cmd = iCmdList[i];
		// Write out the conditional information
		aWriteStream.WriteInt32L(cmd->ConditionalInformation());
		// Write out the type
		const TSsmCommandType type = cmd->Type();
		aWriteStream.WriteInt16L(type);
		// Write out the command specific information
		cmd->ExternalizeL(aWriteStream);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
		if (cmd->ConditionalInformation())
			{
			const CSsmCommandBase* cmdBase = static_cast<const CSsmCommandBase*>(cmd);
			if (cmdBase)
				{
				aWriteStream << cmdBase->GetCommandResourceFileName();
				}
			}
#endif
		}
	}


//
// Execute a specific command in the list
//@panic ECmdListBadIdx if the index value is out of range
//@panic ECmdNullPtr if the information used to create command is null
void CSsmCommandListImpl::Execute( const TInt aIndex, TRequestStatus& aStatus )
	{
	__ASSERT_DEBUG( __IN_RANGE(aIndex, iCmdList.Count()), PanicNow(KPanicCmdList, ECmdListBadIdx));
	__ASSERT_DEBUG( iCmdList[ aIndex ], PanicNow(KPanicCmdList, ECmdNullPtr));
	
	MSsmCommand* cmd = iCmdList[ aIndex ];
	if( cmd )
		{
		// Load the resource file if there is conditional information
		if( cmd->ConditionalInformation() != 0 )
			{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			CSsmCommandBase* cmdBase = static_cast<CSsmCommandBase*>(cmd);
			if (cmdBase)
				{
				if (iResourceFileName != cmdBase->GetCommandResourceFileName())
					{
					TInt length = cmdBase->GetCommandResourceFileNameLength();
					if (iResourceFileName.MaxLength() < length)
						{
						TRAPD(err, iResourceFileName.ReAllocL(length));
						//iResourceFileName is set to KNullDesC if there is any error(like KErrNoMemory)
						//InitialiseResourceFileL() leaves in this case and can be handled by evaluate condition
						iResourceFileName = (err == KErrNone) ? cmdBase->GetCommandResourceFileName() : KNullDesC;
						}
					else
						{
						iResourceFileName = cmdBase->GetCommandResourceFileName();
						}
					if (iResourceFile)
						{
						delete iResourceFile;
						iResourceFile = NULL;
						}
					}
				}
#endif
			// TRAP_IGNORE is used here as if this method fails then
			// iResourceFile will be null, but evaluate condition can handle
			// iResourceFile being null.
			TRAP_IGNORE(InitialiseResourceFileL());
			}
		if(iCmdList[ aIndex ]->EvaluateCondition(iConditionalCallback, iResourceFile))
			{
			// Condition is true so execute the command
			iCmdList[ aIndex ]->Execute( aStatus );
			}
		else
			{
			// Condition is false so complete aStatus immediately
			// The EvaluateCondition() method will have set the command
			// correctly to allow completion of deferred wait for signal commands
			TRequestStatus* trs = &aStatus;
			aStatus = KRequestPending;
			User::RequestComplete(trs, KErrNone);
			}
		}
	
	}


//
// Cancel a specific command in the list
//@panic ECmdListBadIdx if the index value is out of range
//@panic ECmdNullPtr if the information used to create command is null
void CSsmCommandListImpl::ExecuteCancel( const TInt aIndex )
	{
	__ASSERT_DEBUG( __IN_RANGE(aIndex, iCmdList.Count()), PanicNow(KPanicCmdList, ECmdListBadIdx));
	__ASSERT_DEBUG( iCmdList[ aIndex ], PanicNow(KPanicCmdList, ECmdNullPtr));
	
	if( iCmdList[aIndex] )
		{
		iCmdList[ aIndex ]->ExecuteCancel();
		}
	}

//
// Count of items in the list
//
TInt CSsmCommandListImpl::Count()const
	{
	return iCmdList.Count();
	}

const MSsmCommand* CSsmCommandListImpl::operator[](TInt aIndex) const
	{
	return iCmdList[aIndex];
	}

void CSsmCommandListImpl::AppendL(MSsmCommand* aCmd)
	{
	if (!aCmd)
		{
		SSMLOGLEAVE(KErrArgument);
		}
	
	const TSsmCommandType cmdType = aCmd->Type();
	DEBUGPRINT2A("Adding command type: %d", cmdType);

    if (ESsmCmdPublishSystemState == cmdType) 
        {
        ++iPublishSysStateCmdCount;
        }
    else if (ESsmCmdPublishSwp == cmdType) 
        {
        ++iPublishSwpCmdCount;
        }

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TLinearOrder<MSsmCommand> order(CSsmCommandListImpl::ComparePriority);
	iCmdList.InsertInOrderAllowRepeatsL(aCmd,order);
#else
	iCmdList.AppendL(aCmd);
#endif  // SYMBIAN_SSM_FLEXIBLE_MERGE	
	}

void CSsmCommandListImpl::GetDataToValidateCommandlist(TInt& aPublishSystemStateCount, TInt& aPublishSwpCount)  const
	{
	aPublishSystemStateCount = iPublishSysStateCmdCount;
	aPublishSwpCount = iPublishSwpCmdCount;
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
TInt CSsmCommandListImpl::ComparePriority(const MSsmCommand& aFirst, const MSsmCommand& aSecond)
	{
	const CSsmCommandBase* const firstCommand = static_cast<const CSsmCommandBase*>(&aFirst);
	const CSsmCommandBase* const secondCommand = static_cast<const CSsmCommandBase*>(&aSecond);
	
	return (secondCommand->Priority() - firstCommand->Priority());
	}
#endif

TArray<MSsmCommand*> CSsmCommandListImpl::PendingDeferred() const
	{
	return iDeferredList.Array();
	}

void CSsmCommandListImpl::SetDelayBetweenCommands(TInt aDelayBetweenCommands)
	{
	iDelayBetweenCommands = aDelayBetweenCommands;
	}

TInt CSsmCommandListImpl::DelayBetweenCommands() const
	{
	return iDelayBetweenCommands;
	}

/**
@panic ECmdNullPtr if the information used to create command is null	
*/
void CSsmCommandListImpl::DeferredDelete()
	{
	__ASSERT_DEBUG(iDeferredDeleter, PanicNow(KPanicCmdList, ECmdNullPtr));
	TInt indexOfPendingCommand = PendingCommandIndex();

	if ((KErrNotFound == indexOfPendingCommand) || !iDeferredDeleter)
		{
		delete this;
		}
	else
		{
		iDeferredDeleter->DeferredDelete(this);
		iDeferredDeleter = NULL;
		}
	}

TInt CSsmCommandListImpl::PendingCommandIndex()
	{
	TInt index = KErrNotFound;
	const TInt count = iCmdList.Count();
	for (TInt i = 0; i < count; i++)
		{
		MSsmCommand* cmd = iCmdList[i];
		// command list can not be deleted if any deferred command is pending
		if (cmd->IsDeferred() && (KRequestPending == cmd->CompletionStatus()))
			{
			index = i;
			break;
			}
		}
	return index;
	}

void CSsmCommandListImpl::InitialiseResourceFileL()
	{
	if(!iResourceFile)
		{
		// Need to load and allocate the resource file
		RFs rfs;
		User::LeaveIfError(rfs.Connect());
		CleanupClosePushL(rfs);
		RFile file;
		User::LeaveIfError(file.Open(rfs, iResourceFileName, EFileRead | EFileShareReadersOnly));
		CleanupClosePushL(file);
		// read entire resource file into a buffer
		TInt fileSize(0);
		User::LeaveIfError(file.Size(fileSize));
		RBuf8 buf;
		buf.CreateL(fileSize);
		CleanupClosePushL(buf);
		User::LeaveIfError(file.Read(buf));
		// Create the resource file
		iResourceFile = CResourceFile::NewL(buf);
		
		// Clean up everything
		CleanupStack::PopAndDestroy(&buf);
		CleanupStack::PopAndDestroy(&file);
		CleanupStack::PopAndDestroy(&rfs);
		}
	}

void CSsmCommandListImpl::SetResourceFileNameL(const TDesC& aFileName)
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	// this is not needed in other case as there is only one resource file per substate
	TInt length = iResourceFileName.MaxLength();
	if (length < aFileName.Length())
		{
		iResourceFileName.ReAllocL(aFileName.Length());		
		}
#else
	iResourceFileName.ReAllocL(aFileName.Length());
#endif
	iResourceFileName.Copy(aFileName);
	// Clear iResourceFile as it is no longer in sync with iResourceFileName
	if(iResourceFile)
		{
		delete iResourceFile;
		iResourceFile = NULL;
		}
	}

void CSsmCommandListImpl::SetConditionalCallback(MSsmConditionalCallback& aCallBack)
	{
	iConditionalCallback = &aCallBack;
	}

MSsmConditionalCallback& CSsmCommandListImpl::ConditionalCallback() const
    {
    return *iConditionalCallback;
    }

/**
  Returns command's severity.
 */		
TCmdErrorSeverity CSsmCommandListImpl::Severity(TInt aCommandIndex)const
	{
	return iCmdList[aCommandIndex]->Severity();	;
	}
