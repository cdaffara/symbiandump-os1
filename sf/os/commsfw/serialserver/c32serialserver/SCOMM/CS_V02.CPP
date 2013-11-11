// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/** @file
 *
 * Implements the v2 extensions of CPort
 */

#include "CS_STD.H"
#include "C32LOG.H"

void CPort::CommNotifySignalChange(const RMessage2& aMessage, CCommSession* aClient)
/**
 * Set DCE/DTE signal notifying on
 *
 * @param aMessage handle to the IPC message from the client
 * @param aClient pointer to the Comm session
 */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommNotifySignalChange(), Client 0x%x"), aClient);
	if (TakeOwnershipForSignals(aMessage,aClient))
		{
		iBlockedSignalChange=aMessage;
		if (aMessage.Int1()>0)
			{
			// We give signal mask as a parameter
			NotifySignalChange(aMessage.Int1());
			}
		else
			{
			// Bad signal mask, we give up.
			SafeComplete(aMessage, KErrArgument);
			}
		}	
	else if (aClient!=iSignalOwner)
		{
		SafeComplete(aMessage, KErrInUse);
		}
	}


EXPORT_C void CPort::SignalChangeCompleted(const TUint& aSignals, TInt aError)
/** Tells the comms server that a request initiated through NotifySignalChange() 
is complete. 

The comms server will then notify the client that its request is complete. 

Called by the CSY module.
	
@param aSignals The new signals value to pass to the client. 
@param anError An error code. */
	{
	C32LOG1(KC32Player, _L8("CPort::SignalChangeCompleted()"));
	if (iSignalOwner)
		{
		if (aError!=KErrCancel)
			{
			TPtr8 sigDes((TUint8*)&aSignals,sizeof(TUint),sizeof(TUint));
			TInt ret = iSignalOwner->Write(0,iBlockedSignalChange,sigDes);
			if (ret!=KErrNone)
				{
				aError=ret;
				}
			}
		SafeComplete(iBlockedSignalChange, aError);
		}
	iSignalOwner = 0;
	}


TBool CPort::TakeOwnershipForSignals(const RMessage2& aMessage, CCommSession* aClient)
/**
 * We check that signal notifying is not called twice at the same time
 *
 * @param aClient pointer to the Comm session
 * @return ETrue if not owner, EFalse if owner
 */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForSignals(), Client 0x%x"), aClient);
	if (!iSignalOwner)
		{
		iSignalOwner=aClient;
		iSignalOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iSignalOwner)
			{
			PanicClient(ENotifySignalTwice,aMessage);
			}
		return EFalse;
		}
	}


void CPort::CommNotifySignalChangeCancel(TInt aHandle, CCommSession* aClient)
/**
 * Cancel outstanding signal notification
 *
 * @param aHandle Handle to the session
 * @param aClient pointer to the Comm session
 */
	{
	C32LOG2(KC32Player, _L8("CPort::CommNotifySignalChangeCancel(), Client 0x%x"), aClient);
	if (iSignalOwner==aClient && (!aHandle || aHandle==iSignalOwnerHandle))
		{
		NotifySignalChangeCancel();
		SignalChangeCompleted(0,KErrCancel);
		}
	}


void CPort::CommNotifyFlowControlChange(const RMessage2& aMessage, CCommSession* aClient)
/**
 * Set flow control notifying on
 *
 * @param aMessage handle to the IPC message from the client
 * @param aClient pointer to the Comm session
 */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommNotifyFlowControlChange(), Client 0x%x"), aClient);
	if (TakeOwnershipForFlowControl(aMessage,aClient))
		{
		iBlockedFlowControlChange = aMessage;
		NotifyFlowControlChange();
		}	
	else if(aClient!= iFlowControlOwner)
		{
		SafeComplete(aMessage, KErrInUse);
		}
	}


EXPORT_C void CPort::FlowControlChangeCompleted(const TFlowControl& aFlowControl, TInt aError)
/** Tells the comms server that a request initiated through NotifyFlowControlChange() 
is complete. 

The comms server will then notify the client that its request is complete. 

Called by the CSY module.
	
@param aFlowControl Flow control to pass to client 
@param anError Error code */
	{
	C32LOG1(KC32Player, _L8("CPort::FlowControlChangeCompleted()"));
	if (iFlowControlOwner)
		{
		if (aError!=KErrCancel)
			{
			TPckg<TFlowControl> len(aFlowControl);
			TInt ret = iFlowControlOwner->Write(0,iBlockedFlowControlChange,len,0);
			if (ret!=KErrNone)
				{
				aError=ret;
				}
			}
		SafeComplete(iBlockedFlowControlChange, aError);
		}		
	iFlowControlOwner = 0;
	}


TBool CPort::TakeOwnershipForFlowControl(const RMessage2& aMessage,CCommSession* aClient)
/**
 * We check that flowcontrol notifying is not called twice at the same time
 *
 * @param aClient pointer to the Comm session
 * @return ETrue if not owner, EFalse if owner
 */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForFlowControl(), Client 0x%x"), aClient);
	if (!iFlowControlOwner)
		{
		iFlowControlOwner=aClient;
		iFlowControlOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iFlowControlOwner)
			{
			PanicClient(ENotifyFlowControlTwice,aMessage);
			}
		return EFalse;
		}
	}


void CPort::CommNotifyFlowControlChangeCancel(TInt aHandle, CCommSession* aClient)
/**
 * Cancel outstanding flow control notification
 *
 * @param aHandle Handle to the session
 * @param aClient pointer to the Comm session
 */
	{
	C32LOG2(KC32Player, _L8("CPort::CommNotifyFlowControlChangeCancel(), Client 0x%x"), aClient);
	if (iFlowControlOwner==aClient && (!aHandle || aHandle==iFlowControlOwnerHandle))
		{
		NotifyFlowControlChangeCancel();
		FlowControlChangeCompleted((TFlowControl)0,KErrCancel);
		}
	}


void CPort::CommNotifyConfigChange(const RMessage2& aMessage, CCommSession* aClient)
/**
 * Set configuration change (data format, speed, ...) notifying on
 *
 * @param aMessage handle to the IPC message from the client
 * @param aClient pointer to the Comm session
 */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommNotifyConfigChange(), Client 0x%x"), aClient);
	if (TakeOwnershipForConfig(aMessage,aClient))
		{
		iBlockedConfigChange=aMessage;
		NotifyConfigChange();
		}	
	else if(aClient!=iConfigOwner)
		SafeComplete(aMessage, KErrInUse);
	}


EXPORT_C void CPort::ConfigChangeCompleted(const TDesC8& aNewConfig, TInt aError)
/** Tells the comms server that a request initiated through NotifyConfigChange() 
is complete. 

The comms server will then notify the client that its request is complete. 
	
Called by the CSY module.
	
@param aNewConfig Configuration value to pass to client 
@param anError Error code */
	{
	C32LOG1(KC32Player, _L8("CPort::ConfigChangeCompleted()"));
	if (iConfigOwner)
		{
		if (aError!=KErrCancel)
			{
			TInt ret = iConfigOwner->Write(0,iBlockedConfigChange,aNewConfig);
			if (ret!=KErrNone)
				aError=ret;
			}
		SafeComplete(iBlockedConfigChange, aError);
		}
	iConfigOwner = 0;
	}


TBool CPort::TakeOwnershipForConfig(const RMessage2& aMessage,CCommSession* aClient)
/**
 * We check that config change notifying is not called twice at the same time
 *
 * @param aClient pointer to the Comm session
 * @return ETrue if not owner, EFalse if owner
 */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForConfig(), Client 0x%x"), aClient);
	if (!iConfigOwner)
		{
		iConfigOwner=aClient;
		iConfigOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iConfigOwner)
			PanicClient(ENotifyConfigTwice,aMessage);
		return EFalse;
		}
	}


void CPort::CommNotifyConfigChangeCancel(TInt aHandle, CCommSession* aClient)
/**
 * Cancel outstanding configuration change notification
 *
 * @param aHandle Handle to the session
 * @param aClient pointer to the Comm session
 */
	{
	C32LOG2(KC32Player, _L8("CPort::CommNotifyConfigChangeCancel(), Client 0x%x"), aClient);
	if (iConfigOwner==aClient && (!aHandle || aHandle==iConfigOwnerHandle))
		{
		NotifyConfigChangeCancel();
		TPtr8 dummy(NULL,0,0);
		ConfigChangeCompleted(dummy,KErrCancel);
		}
	}


void CPort::CommNotifyBreak(const RMessage2& aMessage, CCommSession* aClient)
/**
 * Set break signal notifying on
 *
 * @param aMessage handle to the IPC message from the client
 * @param aClient pointer to the Comm session
 */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommNotifyBreak(), Client 0x%x"), aClient);
	if (TakeOwnershipForBreak(aMessage,aClient))
		{
		iBlockedBreakNotify=aMessage;
		NotifyBreak();
		}
	else if(aClient!=iBreakNotifyOwner)
		SafeComplete(aMessage, KErrInUse);		
	}


EXPORT_C void CPort::BreakNotifyCompleted(TInt aError)
/** Tells the comms server that a request initiated through NotifyBreak()
is complete. 

The comms server will then notify the client that its request is complete. 
	
Called by the CSY module.
	
@param anError Error code */
	{
	C32LOG1(KC32Player, _L8("CPort::BreakNotifyCompleted()"));
	if (iBreakNotifyOwner)
		SafeComplete(iBlockedBreakNotify, aError);		
	iBreakNotifyOwner = 0;
	}


TBool CPort::TakeOwnershipForBreak(const RMessage2& aMessage,CCommSession* aClient)
/**
 * We check that break notifying is not called twice at the same time
 *
 * @param aClient pointer to the Comm session
 * @return ETrue if not owner, EFalse if owner
 */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForBreak(), Client 0x%x"), aClient);
	if (!iBreakNotifyOwner)
		{
		iBreakNotifyOwner=aClient;
		iBreakNotifyOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iBreakNotifyOwner)
			PanicClient(ENotifyBreakTwice,aMessage);
		return EFalse;
		}
	}


void CPort::CommNotifyBreakCancel(TInt aHandle, CCommSession* aClient)
/**
 * Cancel outstanding break notification
 *
 * @param aHandle Handle to the session
 * @param aClient pointer to the Comm session
 */
	{
	C32LOG2(KC32Player, _L8("CPort::CommNotifyBreakCancel(), Client 0x%x"), aClient);
	if (iBreakNotifyOwner==aClient && (!aHandle || aHandle==iBreakNotifyOwnerHandle))
		{
		NotifyBreakCancel();
		BreakNotifyCompleted(KErrCancel);
		}
	}


void CPort::CommNotifyDataAvailable(const RMessage2& aMessage, CCommSession* aClient)
/**
 * Set input buffer data notifying on
 *
 * @param aMessage handle to the IPC message from the client
 * @param aClient pointer to the Comm session
 */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommNotifyDataAvailable(), Client 0x%x"), aClient);
	if (TakeOwnershipForNotifyDataAvailable(aMessage,aClient))
		{
		iBlockedNotifyDataAvailable=aMessage;
		NotifyDataAvailable();
		}
	else if(aClient!=iNotifyDataAvailableOwner)
		SafeComplete(aMessage, KErrInUse);		
	}


EXPORT_C void CPort::NotifyDataAvailableCompleted(TInt aError)
/** Tells the comms server that a request initiated through NotifyDataAvailable() 
is complete. 

The comms server will then notify the client that its request is complete. 

Called by the CSY module.
	
@param anError Error code */
	{
	C32LOG1(KC32Player, _L8("CPort::NotifyDataAvailableCompleted()"));
	if (iNotifyDataAvailableOwner)
		SafeComplete(iBlockedNotifyDataAvailable, aError);
	iNotifyDataAvailableOwner = 0;
	}


TBool CPort::TakeOwnershipForNotifyDataAvailable(const RMessage2& aMessage,CCommSession* aClient)
/**
 * We check that input buffer data notifying is not called twice at the same time
 *
 * @param aClient pointer to the Comm session
 * @return ETrue if not owner, EFalse if owner
 */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForNotifyDataAvailable(), Client 0x%x"), aClient);
	if (!iNotifyDataAvailableOwner)
		{
		iNotifyDataAvailableOwner=aClient;
		iNotifyDataAvailableOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iNotifyDataAvailableOwner)
			PanicClient(ENotifyDataAvailableTwice,aMessage);
		return EFalse;
		}
	}


void CPort::CommNotifyDataAvailableCancel(TInt aHandle, CCommSession* aClient)
/**
 * Cancel outstanding input buffer data notification
 *
 * @param aHandle Handle to the session
 * @param aClient pointer to the Comm session
 */
	{
	C32LOG2(KC32Player, _L8("CPort::CommNotifyDataAvailableCancel(), Client 0x%x"), aClient);
	if (iNotifyDataAvailableOwner==aClient && (!aHandle || aHandle==iNotifyDataAvailableOwnerHandle))
		{
		NotifyDataAvailableCancel();
		NotifyDataAvailableCompleted(KErrCancel);
		}
	}


void CPort::CommNotifyOutputEmpty(const RMessage2& aMessage, CCommSession* aClient)
/**
 * Set output buffer empty notifying on
 *
 * @param aMessage handle to the IPC message from the client
 * @param aClient pointer to the Comm session
 */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommNotifyOutputEmpty(), Client 0x%x"), aClient);
	if (TakeOwnershipForNotifyOutputEmpty(aMessage,aClient))
		{
		iBlockedNotifyOutputEmpty=aMessage;
		NotifyOutputEmpty();
		}
	else if(aClient!=iNotifyOutputEmptyOwner)
		SafeComplete(aMessage, KErrInUse);
	}


EXPORT_C void CPort::NotifyOutputEmptyCompleted(TInt aError)
/** Tells the comms server that a request initiated through NotifyOutputEmpty() 
is complete. 

The comms server will then notify the client that its request is complete. 

Called by the CSY module.
	
@param anError Error code */
	{
	C32LOG1(KC32Player, _L8("CPort::NotifyOutputEmptyCompleted()"));
	if (iNotifyOutputEmptyOwner)
		SafeComplete(iBlockedNotifyOutputEmpty, aError);
	iNotifyOutputEmptyOwner = 0;
	}


TBool CPort::TakeOwnershipForNotifyOutputEmpty(const RMessage2& aMessage,CCommSession* aClient)
/**
 * We check that output buffer empty notifying is not called twice at the same time
 *
 * @param aClient pointer to the Comm session
 * @return ETrue if not owner, EFalse if owner
 */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForNotifyOutputEmpty(), Client 0x%x"), aClient);
	if (!iNotifyOutputEmptyOwner)
		{
		iNotifyOutputEmptyOwner=aClient;
		iNotifyOutputEmptyOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iNotifyOutputEmptyOwner)
			PanicClient(ENotifyOutputTwice,aMessage);
		return EFalse;
		}
	}


void CPort::CommNotifyOutputEmptyCancel(TInt aHandle, CCommSession* aClient)
/**
 * Cancel outstanding output buffer empty notification
 *
 * @param aHandle Handle to the session
 * @param aClient pointer to the Comm session
 */
	{
	C32LOG2(KC32Player, _L8("CPort::CommNotifyOutputEmptyCancel(), Client 0x%x"), aClient);
	if (iNotifyOutputEmptyOwner==aClient && (!aHandle || aHandle==iNotifyOutputEmptyOwnerHandle))
		{
		NotifyOutputEmptyCancel();
		NotifyOutputEmptyCompleted(KErrCancel);
		}
	}


void CPort::CommGetRole(const RMessage2& aMessage, CCommSession* aClient)
/**
 * Get role DCE/DTE from CSY module
 *
 * @param aMessage handle to the IPC message from the client
 * @param aClient pointer to the Comm session
 */
	{
	(void)aClient;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommGetRole(), Client 0x%x"), aClient);
	TCommRole tempRole;
	TInt ret=GetRole(tempRole);
	TPckg<TCommRole> len(tempRole);
	if (ret==KErrNone)
		{
		ret = aMessage.Write(0, len, 0);
		if (ret!=KErrNone)
			{
			PanicClient(EBadDescriptor,aMessage);
			}
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommGetFlowControlStatus(const RMessage2& aMessage, CCommSession* aClient)
/**
 * Get flow control state ON/OFF from CSY module
 *
 * @param aMessage handle to the IPC message from the client
 * @param aClient pointer to the Comm session
 */
	{
	(void)aClient;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommGetFlowControlStatus(), Client 0x%x"), aClient);
	TFlowControl tempFlow;
	TPckg<TFlowControl> len(tempFlow);
	TInt ret=GetFlowControlStatus(tempFlow);
	if (ret==KErrNone)
		{
		ret = aMessage.Write(0, len, 0);
		if (ret!=KErrNone)
			{
			PanicClient(EBadDescriptor,aMessage);
			}
		}
	SafeComplete(aMessage, ret);
	}


// EOF - CS_V02.CPP
