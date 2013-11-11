// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmcommandlist.h>
#include "ssmcommandlistimpl.h"

/**
 * Constructs and returns a new CSsmCommandList object.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmCommandList* CSsmCommandList::NewL()
	{
	CSsmCommandList* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

/**
 * Constructs and returns a new CSsmCommandList object.
 * 
 * If this function returns successfully then that allocated object
 * is pushed onto the cleanup stack.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmCommandList* CSsmCommandList::NewLC()
	{
	CSsmCommandList* self = new(ELeave) CSsmCommandList;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Used to create an instance of CSsmCommandList class from CSsmCommandList object
Must be used only by CLE
@param aSsmCommandList CSsmCommandList reference 
@param aUtilProvider CSsmCommandUtilProvider pointer 
@return A pointer to an object of type CSsmCommandList.

@internalComponent
*/
EXPORT_C CSsmCommandList* CSsmCommandList::NewLC(const CSsmCommandList* aSsmCommandList, CSsmCommandUtilProvider* aUtilProvider)
    {
    CSsmCommandList* self = new(ELeave) CSsmCommandList;
    CleanupStack::PushL(self);
    self->ConstructL(aSsmCommandList, aUtilProvider);
    return self;
    }
/**
@internalComponent
*/
CSsmCommandList::CSsmCommandList()
	{
	}

/**
@internalComponent
*/
void CSsmCommandList::ConstructL()
	{
	iImpl = CSsmCommandListImpl::NewL();
	}

void CSsmCommandList::ConstructL(const CSsmCommandList* aSsmCommandList, CSsmCommandUtilProvider* aUtilProvider)
    {
    iImpl = CSsmCommandListImpl::NewL(aSsmCommandList->Impl(), aUtilProvider);
    }

CSsmCommandListImpl* CSsmCommandList::Impl() const
    {
    return iImpl;
    }
/**
 * Frees any resources allocated to this object.
 * 
 * @publishedPartner
 * @released
 */
EXPORT_C CSsmCommandList::~CSsmCommandList()
	{
	delete iImpl;
	}

/**
 * Conversion operator to allow CSsmCommandList objects to be used as MSsmCommandLists.
 * 
 * @publishedPartner
 * @released
*/
CSsmCommandList::operator class MSsmCommandList&()
	{
	return *iImpl;
	}
	
/**
 * Appends the provided command to the end of the command list.
 * 
 * If this function does not leave then it takes ownership of the command
 * passed in.
 * 
 * @param aCmd The command object to be appended to the command list.
 * 
 * @publishedPartner
 * @released
 */ 
EXPORT_C void CSsmCommandList::AppendL(MSsmCommand* aCmd)
	{
	iImpl->AppendL(aCmd);
	}

/**
 * Gets the number of Publish System State and SWP commands and a bool value to validate the commandlist.
 * 
 * On return aSystemState number of publish System State commands.
 * On return aSWP number of publish SWP commands.
 * On return aDeferredWait Boolean value, ETrue if commandlist contains a deferred wait command which is not followed by multiple wait, EFalse otherwise. 
 * 
 * @internalComponent
 * @released
 */ 
EXPORT_C void CSsmCommandList::GetDataToValidateCommandlist(TInt& aPublishSystemStateCount , TInt& aPublishSwpCount)  const
	{
	return iImpl->GetDataToValidateCommandlist(aPublishSystemStateCount,aPublishSwpCount);
	}

/**
 * Sets the delay between command execution for this command list.
 * 
 * When this value is set the execution of the command list will pause between each command
 * by the specified number of milliseconds.
 * 
 * Pausing between command execution can be useful 
 * to avoid overloading the system when starting many processes with ESsmFireAndForget due to
 * the processes not rendezvousing.
 * 
 * @param aDelayBetweenCommands The delay between command execution in milliseconds. If this value is 0 then there will be no delay between command execution.
 * 
 * @publishedPartner
 * @released 
 */
EXPORT_C void CSsmCommandList::SetDelayBetweenCommands(TInt aDelayBetweenCommands)
	{
	iImpl->SetDelayBetweenCommands(aDelayBetweenCommands);
	}

/**
@internalComponent
*/
EXPORT_C TInt CSsmCommandList::DelayBetweenCommands() const
	{
	return iImpl->DelayBetweenCommands();
	}

/** Read all commands into the list from a stream. This method is used by cle.
Since cle executes all the commands in the commandlist utilprovider should also be internalized.
@internalComponent
*/
EXPORT_C void CSsmCommandList::InternalizeL(RReadStream& aReadStream, CSsmCommandUtilProvider& aUtilProvider )
	{
	iImpl->InternalizeL(aReadStream, &aUtilProvider);
	}

/** Read all commands into the list from a stream. This method is used only by the ssmswppolicyserver and 
ssmswppolicycli. The ssmswppolicyserver and ssmswppolicycli code internalizes and externalizes 
the commandlist in order to pass the it from server to the client. 
This commandlist is not executed by ssmswppolicycli, it just passes it to the cle where the commandlist is executed 
and hence it does not require the utilprovider to be passed in to this method.
@internalComponent
*/
EXPORT_C void CSsmCommandList::InternalizeL(RReadStream& aReadStream)
	{
	iImpl->InternalizeL(aReadStream, NULL);
	}


/** Write all commands from the list to a stream
@internalComponent
*/
EXPORT_C void CSsmCommandList::ExternalizeL(RWriteStream& aWriteStream) const
	{
	iImpl->ExternalizeL(aWriteStream);
	}


/** Execute a specific command in the list
@internalComponent
*/
EXPORT_C void CSsmCommandList::Execute(TInt aIndex, TRequestStatus& aStatus)
	{
	iImpl->Execute(aIndex, aStatus);
	}


/** Cancel a specific command in the list
@internalComponent
*/
EXPORT_C void CSsmCommandList::ExecuteCancel(TInt aIndex)
	{
	iImpl->ExecuteCancel(aIndex);
	}

/** Count of items in the list
@internalComponent
*/
EXPORT_C TInt CSsmCommandList::Count() const
	{
	return iImpl->Count();
	}

/**
This function should be used only if command list is executed.
It waits for pending deferred commands to finish and then deletes the list.
@internalComponent
*/
EXPORT_C void CSsmCommandList::DeferredDelete()
	{
	if (iImpl)
		{
		iImpl->DeferredDelete();
		iImpl = NULL;
		}
	delete this;
	}

/**
 * Retrieves the command at the position specified position.
 * 
 * @param aIndex The index of the command to retrieve.
 * 
 * @return The command at the specified index.
 * @publishedPartner
 * @released
 */
EXPORT_C const MSsmCommand* CSsmCommandList::operator[](TInt aIndex) const
	{
	return (*iImpl)[aIndex];
	}

/**
 * Retrieves the command's severity
 * 
 * @param aIndex The severity of the command to retrieve.
 * 
 * @return The command's severity.
 * @internalComponent
 * @released
 */
EXPORT_C TCmdErrorSeverity CSsmCommandList::Severity(TInt aCommandIndex) const
	{
	return iImpl->Severity(aCommandIndex);
	}
