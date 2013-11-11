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

#ifndef HCISYMBIANQDP_H
#define HCISYMBIANQDP_H

#include <bluetooth/hci/hcicmdqueuedecisionplugin.h>
#include <bluetooth/hci/hcicmdqueuedecisioninterface.h>
#include <bluetooth/hci/hcitypes.h>
#include <bluetooth/hci/hciconsts.h>

/**
This class represents a QDP that is relevant for hardware that is perfect,
and so the Command Queue can operate normally without any special considerations.
*/
class CHCISymbianQdp : 	public CHCICmdQueueDecisionPlugin,
						public MHCICmdQueueDecisionInterface,
						public MHCICmdQueueEventModifierInterface,
						public MHCICmdQueueUtilityUser
	{
public:
	static CHCISymbianQdp* NewL();

private: // from CHCICmdQueueDecisionPlugin
	virtual TAny* Interface(TUid aUid);
	
private: // virtuals from MHCICmdQueueDecisionInterface
	TBool MhcqdiDoesCommandRequireWorkaround(const CHCICommandQItem& aParent);
	CHCICommandQItem* MhcqdiGetPreChildCommand(const CHCICommandQItem& aParent,
											   const CHCICommandQItem* const aPreviousWorkaroundCmd,
											   const THCIEventBase* const aPreviousCmdResult);
	CHCICommandQItem* MhcqdiGetPostChildCommand(const CHCICommandQItem& aParent,
												const CHCICommandQItem* const aPreviousPostChild,
												const THCIEventBase* aPreviousCmdResult);
	THCIEventBase* MhcqdiGetFakedUnsolicitedEvent(const CHCICommandQItem& aParent,
												  const THCIEventBase* aPreviousFakedEvent);
	void MhcqdiCommandAboutToBeDeleted(const CHCICommandQItem& aDyingCmd);
	TInt MhcqdiCanSend(CHCICommandQItem& aCommand, const TDblQue<const CHCICommandQItem>& aSentCommands);
	TUint MhcqdiTimeoutRequired(const CHCICommandQItem& aCmdAboutToBeSent);
	void MhcqdiMatchedEventReceived(const THCIEventBase& aEvent, const CHCICommandQItem& aRelatedCommand);
	TCommandErroredAction MhcqdiMatchedErrorEventReceived(const THCIEventBase& aErrorEvent, 
												 		  const CHCICommandQItem& aRelatedCommand);
	void MhcqdiUnmatchedEventReceived(const THCIEventBase& aEvent);
	TCommandTimedOutAction MhcqdiCommandTimedOut(const CHCICommandQItem& aCommand,
												 const TDblQue<const CHCICommandQItem>& aSentCommands,
												 TUint aCurrentCommandCredits,
												 TUint& aCreditsToBeRefunded);
	void MhcqdiSetPhysicalLinksState(const MPhysicalLinksState& aPhysicalLinkState);
	void MhcqdiSetHardResetInitiator(const MHardResetInitiator& aHardResetInitiator);
	void MhcqdiSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue);
	void MhcqdiSetTimeouts(TUint aQueueStarvationTimeout, TUint aMaxHciCommandTimeout);
	TUint MhcqdiReset();

private: // from MHCICmdQueueEventModifierInterface
	void MhcqemiMatchedEventReceived(THCIEventBase& aEvent, const CHCICommandQItem& aRelatedCommand);
	void MhcqemiUnmatchedEventReceived(THCIEventBase& aEvent);

private: // from MHCICmdQueueUtilityUser
	void MhcquuSetUtilitiesProvider(MHCICmdQueueUtilities& aProvider);
	
private:
	// The initial number of command credits for the HCI Command Queue.
	const static TInt KHCIDefaultCmdCredits = 1;

	// Workarounds for controller problems
	void FixIgnoreInvalidHciParameterErrorOnSetEventMaskOnVersion1_1(THCIEventBase& aEvent);
	void FixFakeCompletionEventsOnDisconnection(THCIEventBase& aEvent);
	void FixAddConnectionHandleForTruncatedInvalidConnectionHandleErrorEvents(THCIEventBase& aEvent, const CHCICommandQItem* aRelatedCommand);
	
private:
	CHCISymbianQdp();

private:
	MHCICmdQueueUtilities* iProvider;
	TUint8 iHCIVersion;
	TBuf8<KHCIMaxEventSize> iEventModBuffer; // utility buffer for the backend of modified/injected events
	};

#endif // HCISYMBIANQDP_H
