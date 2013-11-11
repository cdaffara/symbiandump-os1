// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
*/

#include "ss_commsdataobject.h"

#include <comms-infras/ss_log.h>

#include <comms-infras/api_ext_list.h>
#include <es_sock.h>
#include <comms-infras/es_parameterbundle.h>
#include <comms-infras/es_commsdataobject.h>

using namespace ESock;
using namespace Messages;

TBool TGetOrSetParameters::CanProcess(MeshMachine::TNodeContextBase& aContext)
	{
	TUint interfaceId = (*iDataObject)->RequiredItfExtId();
	return HasInterface(aContext, (TSupportedCommsApiExt)interfaceId);
	}

void TGetOrSetParameters::ForwardL(MeshMachine::TNodeContextBase& aContext)
	{
	TUint interfaceId = (*iDataObject)->RequiredItfExtId();
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	TRAPD(err, ForwardRequestL(aContext));
	if(err == KErrNotReady)
		{
		// We've reached the bottom of the stack, so complete the message
		if ((*iDataObject)->IsProgressive())
			{
			// If the data object was progressive (meaning it traverses the entire
			// stack progressively building up information using all nodes that
			// support its required interface) then we complete with KErrNone at
			// the bottom...
			__CFLOG_VAR((KESockComponentTag, KESockMessage,
					_L8("TGetOrSetParameters::ForwardL() Node %08x implementing interface id 0x%08x completing message (%08X) with %d"),
					&aContext.Node(), interfaceId, iMessage.Handle(), err));
			responseMsg.Complete(KErrNone);
			}
		else
			{
			// ...if on the other hand the data object was not progressive
			// hitting the bottom of the stack means it didn't find any
			// nodes that supporting its required interface. In thay case
			// we complete with KErrNotFound
			User::Leave(KErrNotFound);
			}
		return;
		}
	User::LeaveIfError(err);
	}

void TGetOrSetParameters::ProcessL(MeshMachine::TNodeContextBase& aContext)
	{
	TAny* interface = NULL;
	TUint interfaceId = (*iDataObject)->RequiredItfExtId();
	TRAPD(err, interface = FetchInterfaceL(aContext.Node(), (TSupportedCommsApiExt)interfaceId));
	ASSERT(interface);
	
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	User::LeaveIfError(err);

    if (interface)
    	{
    	XCommsDataObject::TProgressAction action(XCommsDataObject::EComplete);
    	TRAP(err, action = (*iDataObject)->DispatchL(interface, this));
		if (err != KErrNone)
			{
            NM_LOG_START_BLOCK(KESockMessage, _L8("TGetOrSetParameters::ProcessL()"));
            NM_LOG((KESockMessage, _L8("Node implementing interface id 0x%08x failed with %d"), interfaceId, err));
            NM_LOG_NODE(KESockMessage, aContext.Node().Id().Node());
            NM_LOG_END_BLOCK(KESockMessage, _L8("TGetOrSetParameters::ProcessL()"));
                
			Complete(aContext, err);
			return;
			}

		if ((action == XCommsDataObject::EComplete)
			|| !(*iDataObject)->IsProgressive())
			{
			Complete(aContext, KErrNone);
			return;
			}

	    // On successful handling of a progressive data object
	    // forward the data object down to the next node
	    TRAP(err, ForwardRequestL(aContext));

		if(err == KErrNotReady)
			{
			// We've reached the bottom of the stack, so complete the message
			Complete(aContext, KErrNone);
			return;
			}
		User::LeaveIfError(err);
    	}
	}

TInt TGetOrSetParameters::CheckError(MeshMachine::TNodeContextBase& /*aContext*/, TInt aError)
	{
	// The query set container is owned by the CConnection that originated
    // this message - if an error occurs we need to delete it and NULL the
    // pointer
	if (aError != KErrNone)
		{
		delete (*iDataObject);
		*iDataObject = NULL;
		}

	return aError;
	}

void TGetOrSetParameters::Complete(MeshMachine::TNodeContextBase& aContext, TInt aError)
	{
	RLegacyResponseMsg responseMsg(aContext, iMessage, iMessage.Int0());
	
	__CFLOG_VAR((KESockComponentTag, KESockMessage,
		_L8("TGetOrSetParameters::Complete() completing message (%08X) with %d"),
		iMessage.Handle(), aError));

	if ((*iDataObject)->OperationMode() == XCommsDataObject::EOperationSet)
		{
		// Done with the object
		delete (*iDataObject);
		*iDataObject = NULL;
		}

	responseMsg.Complete(aError);
	}


