// smswatch.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __SMS_WATCH__
#define __SMS_WATCH__

#include <smsuaddr.h>
class CSmsMessage;
#include "smsevent.h"

//
// CSmsWatcher
// listen for incoming sms's from smsstack (via esock)
// interpret & execute the command contained therein
//
NONSHARABLE_CLASS(CSmsWatcher) : public CActive
	{
public:
	static CSmsWatcher* NewL(CCmdSms& aParent);
	~CSmsWatcher();
	void WaitForMessage();
	const TDesC& Message();
private:
	CSmsWatcher(CCmdSms& aParent);
	void ConstructL();
	void ReadSmsAndRespondToStackL();
	void PrepBuffersL();
	void ProcessCommandL();
	void ParseBufferL(const TDesC& aInstruction, const TDesC& aSenderAddress);
	
	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
private:
	CCmdSms& iParent;
	RSocketServ iSocketServer;
	RSocket iSocket;
	TSmsAddr iSmsAddr;
	TPckgBuf<TUint>	iIoctlResult;
	CSmsMessage* iMessage;
	RBuf iCommand;
		
	enum TSmsListenState
		{
		ESmsListenIdle,
		ESmsListenWaitForSms,
		ESmsListenReadSms,
		ESmsListenProcessSms,
		ESmsListenTestMode
		};
	TSmsListenState iState;
	TBuf<0x100> iErrorMessage;
	};


#endif // __SMS_WATCH__
