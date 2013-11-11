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
 @internalComponent
*/

#include "ControllerInitialisationPluginImpl.h"
#include "SymbianControllerInitialisor.h"

#include <bluetooth/hci/controllerinitialisationobserver.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_INITIALISOR_SYMBIAN);
#endif

/*static*/ CControllerInitialisationPluginImpl* CControllerInitialisationPluginImpl::NewL()
	{
	LOG_STATIC_FUNC

	CControllerInitialisationPluginImpl* self = new (ELeave) CControllerInitialisationPluginImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CControllerInitialisationPluginImpl::~CControllerInitialisationPluginImpl()
	{
	LOG_FUNC

	}
	
void CControllerInitialisationPluginImpl::ConstructL()
	{
	LOG_FUNC

	}

/*virtual*/ TAny* CControllerInitialisationPluginImpl::Interface(TUid aUid)
	{
	LOG_FUNC

	TAny* ret = NULL;
	switch(aUid.iUid)
		{
		case KControllerInitialisationInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MControllerInitialisationInterface*>(this));
			break;

		case KControllerInitialisationAbortExtenstionInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MControllerInitialisationAbortExtensionInterface*>(this));
			break;

		default:
			break;
		};

	return ret;
	}


// Protected constructor.
CControllerInitialisationPluginImpl::CControllerInitialisationPluginImpl()
	{
	LOG_FUNC

	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiPreResetCommand()
	{
	LOG_FUNC

	__ASSERT_DEBUG(iControllerInitialisationObserver, PANIC(KSymbianControllerInitialisorPanic, ENullInitialisationObserver));

	// Do any pre-reset initialisation...
	
	// Inform stack that we have finished
	iControllerInitialisationObserver->McioPreResetCommandComplete(KErrNone);
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiPostResetCommand()
	{
	LOG_FUNC

	__ASSERT_DEBUG(iControllerInitialisationObserver, PANIC(KSymbianControllerInitialisorPanic, ENullInitialisationObserver));

	// Do any post-reset initialisation...

	// Inform stack that we have finished
	iControllerInitialisationObserver->McioPostResetCommandComplete(KErrNone);
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiSetHCICommandQueue(MHCICommandQueue& /*aHCICommandQueue*/)
	{
	LOG_FUNC

	// This initialisor doesn't send any commands.  Therefore it will not need a reference
	// to the command queue.
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiSetControllerInitialisationObserver(MControllerInitialisationObserver& aObserver)
	{
	LOG_FUNC

	__ASSERT_DEBUG(iControllerInitialisationObserver == 0, PANIC(KSymbianControllerInitialisorPanic, EInitialisationObserverOverwritten));
	iControllerInitialisationObserver = &aObserver;
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiSetCoreHci(MCoreHci& /*aCoreHci*/)
	{
	LOG_FUNC

	// This initialisor doesn't need to communicate with the
	// Core HCI.
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciaeiAbortInitialisation()
	{
	LOG_FUNC
	
	// As this initialisor calls back synchronously from MciiPreResetCommand and MciiPostResetCommand
	// nothing needs to be aborted. This does however show a problem in the Stack logic as this should
	// only be called if a callback is outstanding.
	PANIC(KSymbianControllerInitialisorPanic, EUnexpectedAbortInitialisation);
	}


