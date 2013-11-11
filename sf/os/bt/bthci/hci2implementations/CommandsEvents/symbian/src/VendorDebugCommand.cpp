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

#include <bluetooth/hci/vendordebugcommand.h>
#include "symbiancommandseventsutils.h"
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/commandbaseextension.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

EXPORT_C CVendorDebugCommand* CVendorDebugCommand::NewL(TUint16 aOpcode)
	{
	CVendorDebugCommand* self = new(ELeave) CVendorDebugCommand(aOpcode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CVendorDebugCommand::CVendorDebugCommand(TUint16 aOpcode)
  :	CHCICommandBase(aOpcode)
	{
	__ASSERT_DEBUG((aOpcode & KOGFMask) == KVendorDebugOGF, PANIC(KSymbianCommandsEventsPanicCat, EInvalidVendorDebugCommandOpcode));
	}

EXPORT_C void CVendorDebugCommand::ConstructL()
	{
	CHCICommandBase::BaseConstructL();
	}

void CVendorDebugCommand::Format(CHctlCommandFrame& aCommandFrame) const
	{
	aCommandFrame.PutString(iVendorSpecificData);
	}

EXPORT_C void CVendorDebugCommand::Reset(TUint16 aOpcode)
	{
	__ASSERT_DEBUG((aOpcode & KOGFMask) == KVendorDebugOGF, PANIC(KSymbianCommandsEventsPanicCat, EInvalidVendorDebugCommandOpcode));
	iOpcode = aOpcode;
	iVendorSpecificData.Zero();
	}

EXPORT_C TDes8& CVendorDebugCommand::Command()
	{
	return iVendorSpecificData;
	}

EXPORT_C void CVendorDebugCommand::SetCreditsConsumed(TUint aConsumedCredits)
	{
	CHCICommandBase::SetCreditsConsumed(aConsumedCredits);
	}

EXPORT_C void CVendorDebugCommand::SetExpectsCommandStatusEvent(TBool aExpectsCmdStatus)
	{
	return CHCICommandBase::SetExpectsCommandStatusEvent(aExpectsCmdStatus);
	}

EXPORT_C void CVendorDebugCommand::SetExpectsCommandCompleteEvent(TBool aExpectsCmdComplete)
	{
	return CHCICommandBase::SetExpectsCommandCompleteEvent(aExpectsCmdComplete);
	}

EXPORT_C TInt CVendorDebugCommand::SetExpectsCompletingEvent(TBool aExpectsCompletingEvent)
	{
	TInt err = KErrNone;
	
	if(CHCICommandBase::iCommandBaseExtension->CompletingEventQueryHelper())
	    {
	    CHCICommandBase::iCommandBaseExtension->CompletingEventQueryHelper()->SetExpectsCompletingEvent(aExpectsCompletingEvent);
	    }
	else
	    {
	    CHCICompletingEventQueryHelper* completingEventQueryHelper = NULL;
	    TRAP(err, completingEventQueryHelper = CHCICompletingEventQueryHelper::NewL(aExpectsCompletingEvent));
	    if(err == KErrNone)
	        {
	        CHCICommandBase::iCommandBaseExtension->SetCompletingEventQueryHelper(completingEventQueryHelper);
	        }
	    }
	return err;	
	}

/*virtual*/ EXPORT_C TInt CVendorDebugCommand::Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData)
	{
	return CHCICommandBase::Extension_(aExtensionId, aInterface, aData);
	}

void CVendorDebugCommand::Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const
	{
	if (iMatchImpl)
		{
		iMatchImpl->MvdcmMatch(*this, aEvent, aMatchesCmd, aConcludesCmd, aContinueMatching);
		}
	else
		{
		CHCICommandBase::Match(aEvent, aMatchesCmd, aConcludesCmd, aContinueMatching);
		}
	}

EXPORT_C void CVendorDebugCommand::SetMatcher(MVendorDebugCommandMatcher* aMatcher)
	{
	iMatchImpl = aMatcher;
	}

