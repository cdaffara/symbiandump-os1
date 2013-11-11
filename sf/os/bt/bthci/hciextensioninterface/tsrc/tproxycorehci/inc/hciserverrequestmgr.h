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

#ifndef HCISERVERREQUESTMGR_H
#define HCISERVERREQUESTMGR_H

#include <e32base.h>
#include <bluetooth/hcicommandqueueclient.h>

class CHCISession;
class MHCICommandQueue;
class CCoreHCIPluginImpl;

NONSHARABLE_CLASS(CHCIServerRequestManager) : public CBase
	{
public:
	enum TCategoryMask
		{
		EBTPowerControlMaskBit = 0x01,
		EHCIDirectAccessMaskBit = 0x02
		};

	static CHCIServerRequestManager* NewL(CCoreHCIPluginImpl& aHCI);

	void BlockAsync(CHCISession& aSession, const RMessage2& aMessage);
	TBool IsAsyncBlocked();
	void ServiceL(CHCISession& aSession, const RMessage2& aMessage);

protected:
	CHCIServerRequestManager(CCoreHCIPluginImpl& aHCI);
	void ConstructL();
	
	virtual void CompleteAsyncTask(TInt aError);

private:
	virtual TUint MaskBit() const = 0;
	virtual void DoServiceL(CHCISession& aSession, const RMessage2& aMessage) = 0;
	virtual TBool HasCapabilities(const RMessage2& aMessage) = 0;

protected:
	CCoreHCIPluginImpl& iHCI;
	CHCISession* iAsyncTaskSession; // unowned

private:
	TUint iBlockMask;
	RMessage2 iAsyncTaskMessage;
	};

NONSHARABLE_CLASS(CHCIDirectAccessManager) : public CHCIServerRequestManager,
											 public MHCICommandQueueClient
	{
public:
	static CHCIDirectAccessManager* NewL(CCoreHCIPluginImpl& aHCI);
	void SetHCICommandQueue(MHCICommandQueue& aHCICommandQueue);

private:
	virtual void DoServiceL(CHCISession& aSession, const RMessage2& aMessage);
	virtual TBool HasCapabilities(const RMessage2& aMessage);
	virtual void CompleteAsyncTask(TInt aError);

	virtual void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aCommand);

	CHCIDirectAccessManager(CCoreHCIPluginImpl& aHCI);
	TUint MaskBit() const;

private:
	MHCICommandQueue* iHCICommandQueue; // un-owned

	// This implementation restricts the number of server requests to
	// one in total (not per session) - this includes both direct
	// access requests and power management requests.
	const RMessage2* iMessage;
	};

NONSHARABLE_CLASS(CBTPowerControlManager) : public CHCIServerRequestManager
	{
public:
	static CBTPowerControlManager* NewL(CCoreHCIPluginImpl& aHCI);
	void CompleteRequest(TInt aError);

private:
	virtual void DoServiceL(CHCISession& aSession, const RMessage2& aMessage);
	virtual TBool HasCapabilities(const RMessage2& aMessage);
	virtual void CompleteAsyncTask(TInt aError);
	
private:
	CBTPowerControlManager(CCoreHCIPluginImpl& aHCI);
	TUint MaskBit() const;
	};

#endif // HCISERVERREQUESTMGR_H
