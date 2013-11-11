// smssend.h
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

#ifndef __SMS_SEND_H__
#define __SMS_SEND_H__

#include <gsmubuf.h>
#include <gsmumsg.h>
#include <es_sock.h>
#include <smsustrm.h>
#include <smsuaddr.h>
#include <mmretrieve.h>

class MSmsSendObserver
	{
public:
	virtual void SendComplete(const TInt aError) = 0;
	};

NONSHARABLE_CLASS(CSmsSender) : public CActive
	{
public:
	static CSmsSender* NewL(RFs& aFs, MSmsSendObserver& aParent, const TDesC& aMsgToSend, const TDesC& aDestination);
	~CSmsSender();
	void StartL();
private:
	CSmsSender(MSmsSendObserver& aParent, RFs& aFs, const TDesC& aMsgToSend, const TDesC& aDestination);
	void ConstructL();
	TInt GetServiceCentreAndSendSms();
	void RetrieveServiceCentreL();
	void CreateAndSendSmsL();
	
	// From CActive
	void RunL();
	void DoCancel();
private:
	MSmsSendObserver& iParent;
	RFs& iFs;
	const TDesC& iMsgContent;
	const TDesC& iDestination;
	CSmsBuffer* iMsgBuffer;
	CSmsMessage* iMsgToSend;
	RMobilePhone::TMobileAddress iServiceCentre;
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RMobileSmsMessaging iMobileMessaging;
	CRetrieveMobilePhoneSmspList* iSmspList;
	RSocketServ iSocketServer;
	RSocket iSocket;
	TSmsAddr iSmsAddr;
	
	enum TSmsSenderState
		{
		ESmsSenderIdle,
		ESmsSenderGetServiceCentre,
		ESmsSenderSendMessage
		};
	TSmsSenderState iState;
	};

#endif // __SMS_SEND_H__
