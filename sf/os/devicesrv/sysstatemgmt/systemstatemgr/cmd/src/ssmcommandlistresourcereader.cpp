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

#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmconditionalcallback.h>
#include "ssmpanic.h"
#include "ssmcommandlistresourcereaderimpl.h"
#include "ssmcommandlistimpl.h"
#include "ssmdebug.h"

/**
Constructs a new command list resource reader object.

This object must be successfully initialised before it can
be used to create command lists.

@param aFs					A connected file server session.
@param aCommandListPath		Path identifying a directory containing command list resource files.
@param aConditionalCallback	A callback used to determine whether an individual command should be
							included in the command list or not.
@return A fully constructed, uninitialised command list resource reader.
@leave KErrArgument If @c aCommandListPath is zero length.
*/	
EXPORT_C CSsmCommandListResourceReader* CSsmCommandListResourceReader::NewL(RFs& aFs, const TDesC& aCommandListPath, MSsmConditionalCallback& aConditionalCallback)
	{
	CSsmCommandListResourceReader* self = new (ELeave) CSsmCommandListResourceReader();
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aCommandListPath, aConditionalCallback);
	CleanupStack::Pop(self);
	return self;
	}

CSsmCommandListResourceReader::CSsmCommandListResourceReader()
	{
	}

/**
Destructor.

This frees all resources owned by the object, prior to its destruction.
*/
EXPORT_C CSsmCommandListResourceReader::~CSsmCommandListResourceReader()
	{
	delete iImpl;
	delete iCommandList;
	}

void CSsmCommandListResourceReader::ConstructL(RFs& aFs, const TDesC& aCommandListPath, MSsmConditionalCallback& aConditionalCallback)
	{
	iImpl = CSsmCommandListResourceReaderImpl::NewL(aFs, aCommandListPath, aConditionalCallback);
	}

/**
Initialises a command list resource reader.

During initialisation, the reader builds a list of mappings between command list ids
and associated @c SSM_COMMAND_LIST resources. This is done by reading the
@c SSM_COMMAND_LIST_MAPPING resource for each of the command list resource files
located in the command list path provided on construction of this object.

This object must be initialised before calling @c PrepareCommandList or @c GetCommandList.

Initialising an already initialised object just completes @c aStatus with KErrNone.

Initialisation is performed asynchronously in the calling thread, so the Active Scheduler
must be allowed to run while waiting for @c aStatus to complete.
This means code such as @c User::WaitForRequest(aStatus) should not be used to wait for
initialisation to complete, as this would cause the current thread to block indefinately.

@param aStatus	Completes with KErrNone if successful, KErrNotFound if no command list
				resource files can be found, or a resource file contains no
				command list id mappings, KErrNotSupported if the @c SSM_COMMAND_LIST_ROOT resource
				contains an invalid version number, otherwise a system wide error code.
@panic CmdResourceRead EInUse1 if currently initialising or preparing a command list.
*/	
EXPORT_C void CSsmCommandListResourceReader::Initialise(TRequestStatus& aStatus)
	{
	iImpl->Initialise(aStatus);
	}

/**
Cancels an outstanding @c Initialise request by completing the reqeust with KErrCancel.
*/	
EXPORT_C void CSsmCommandListResourceReader::InitialiseCancel()
	{
	iImpl->InitialiseCancel();
	}

/**
Initiates preparation of a command list used to transition to a new state.

If command list preparation is successfull, the command list may be obtained by calling
GetCommandList.

Preparation is performed asynchronously in the calling thread, so the Active Scheduler
must be allowed to run while waiting for @c aStatus to complete.
This means code such as @c User::WaitForRequest(aStatus) should not be used to wait for
preparation to complete, as this would cause the current thread to block indefinately.

@pre Object must have been successfully initialised via a call to @c Initialise().
@param aCommandListId	An integer that uniquely identifies a @c SSM_COMMAND_LIST resource
						containing a list of commands.
@param aState			State used to construct the Publish System State command.
						The command list @c aCommandListId should contain exactly one
						Publish System State command.
@param aStatus			Completes with KErrNone if successful, KErrNotFound if the
						@c SSM_COMMAND_LIST resource corresponding to @c aCommandListId
						cannot be found, otherwise a system wide error code.
@panic CmdResourceRead EInUse2 if currently initialising or preparing a command list.
@panic CmdResourceRead ENotInitialized if not successfully initialised.
*/	
EXPORT_C void CSsmCommandListResourceReader::PrepareCommandList(TInt aCommandListId, const TSsmState& aState, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(!iImpl->Busy(), PanicNow(KPanicCmdResourceReader, EInUse2));
	delete iCommandList;
	iCommandList = NULL;
	TRAPD(err, iCommandList = CSsmCommandList::NewL());
	if (err != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	else
		{
		iImpl->PrepareCommandList(*iCommandList, aCommandListId, aState, aStatus);
		}
	}

/**
Initiates preparation of a command list used to change the value of a system wide property.

This object must be initialised before calling @c PrepareCommandList or @c GetCommandList.

If command list preparation is successfull, the command list may be obtained by calling
GetCommandList.

Preparation is performed asynchronously in the calling thread, so the Active Scheduler
must be allowed to run while waiting for @c aStatus to complete.
This means code such as @c User::WaitForRequest(aStatus) should not be used to wait for
preparation to complete, as this would cause the current thread to block indefinately.

@pre Object must have been successfully initialised via a call to @c Initialise().
@param aCommandListId	An integer that uniquely identifies a @c SSM_COMMAND_LIST resource
						containing a list of commands.
@param aSwp				System wide property used to construct the Publish System Wide Property command.
						The command list @c aCommandListId should contain exactly one
						Publish System Wide Property command.
@param aStatus			Completes with KErrNone if successful, KErrNotFound if the
						@c SSM_COMMAND_LIST resource corresponding to @c aCommandListId
						cannot be found, otherwise a system wide error code.
@panic CmdResourceRead EInUse3 if currently initialising or preparing a command list.
@panic CmdResourceRead ENotInitialized2 if not successfully initialised.
*/
EXPORT_C void CSsmCommandListResourceReader::PrepareCommandList(TInt aCommandListId, const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(!iImpl->Busy(), PanicNow(KPanicCmdResourceReader, EInUse3));
	delete iCommandList;
	iCommandList = NULL;
	TRAPD(err, iCommandList = CSsmCommandList::NewL());
	if (err != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	else
		{
		iImpl->PrepareCommandList(*iCommandList, aCommandListId, aSwp, aStatus);
		}
	}

/**
Cancels an outstanding @c PrepareCommandList request by completing the request with KErrCancel.
*/	
EXPORT_C void CSsmCommandListResourceReader::PrepareCommandListCancel()
	{
	iImpl->PrepareCommandListCancel();
	delete iCommandList;
	iCommandList = NULL;
	}

/**
Following successful completion of a @c PrepareCommandList request,
returns the prepared command list.

Only the most recently prepared command list may be returned by this method and
may only be returned once, because ownership of the returned command list is
transferred to the caller.
Subsequent calls to this method with no prepare inbetween will return NULL.

@pre A command list must have been successfully prepared via a call to @c PrepareCommandList().
@return		A new command list object populated with commands read from resource.
			Ownership of the returned command list is transferred to the caller.
@panic CmdResourceRead ENoPreparedCommandList if a prepared command list is not available.
*/
EXPORT_C CSsmCommandList* CSsmCommandListResourceReader::GetCommandList()
	{
	__ASSERT_DEBUG(iImpl->IsCommandListReady(), PanicNow(KPanicCmdResourceReader, ENoPreparedCommandList));
	DEBUGPRINT2A("CSsmCommandListResourceReader::GetCommandList - List Count() = %d", iCommandList->Count());
	CSsmCommandList* const commandList = iCommandList;
	iCommandList = NULL; // return ownership of command list to the caller
	return commandList;
	}

/**
Fills the provided array with command list ids.
On return, aArray contains valid command list ids that may be passed to @c PrepareCommandList.

@pre Object must have been successfully initialised via a call to @c Initialise().
@param aArray	An empty array to be populated with command list ids.
@leave KErrArgument If @c aArray is non-empty.
@panic CmdResourceRead ENotInitialized3 if not successfully initialised.
*/
EXPORT_C void CSsmCommandListResourceReader::GetCommandListIdsL(RArray<TInt>& aArray) const
	{
	iImpl->GetCommandListIdsL(aArray);
	}

