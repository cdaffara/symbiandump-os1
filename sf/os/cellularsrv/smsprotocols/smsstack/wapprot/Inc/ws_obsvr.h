// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * @internalAll 
*/

#ifndef __WS_OBSVR_H__
#define __WS_OBSVR_H__

#include <e32base.h>

#include "smsuaddr.H"
#include "smspcomm.h"

class CWapSmsProvider;
class CSmsMessage;
class CWapDatagram;
class CSmsProtocol;
class CWapSmsProtocol;

/**
 *  @internalComponent
 */
class CWapSapMessageSender : public CBase, public MSmsMessageObserver
	{
public:
	static CWapSapMessageSender* NewL(CSmsProtocol* aSmsProtocol, CWapSmsProvider* aWapSap);
	~CWapSapMessageSender();
	void SendDatagramL(CWapDatagram* aMsg);
public:
// From MSmsMessageObserver
	virtual const TSmsAddr& GetLocalAddress() const;
	virtual void SetLocalAddress(const TSmsAddr& aSmsAddr);
	virtual void ModemNotificationCompleted(TInt aStatus);
	virtual void MessageSendCompleted(TInt aStatus);
	virtual TInt MessageReceived(const CSmsMessage& aSmsMessage,TDes& aDes);
	virtual TBool ClientConfirmsMessage()const;
	virtual TInt SmsAddrIsDuplicate(const MSmsMessageObserver* aObserver,const TSmsAddr& aAddr)const;
	virtual void EnumeratePhoneCompleted(TInt aStatus);
	virtual void MessageWriteCompleted(TInt aStatus, const CSmsMessage* aSmsMessage = NULL);
	virtual void MessageDeleteCompleted(TInt aStatus);
    virtual void ReadSmsParamsCompleted(TInt aStatus,CMobilePhoneSmspList* aSmspList);
    virtual void WriteSmsParamsCompleted(TInt aStatus);
protected:
	CWapSapMessageSender(CSmsProtocol* aSmsProtocol, CWapSmsProvider* aWapSap);
private:
	void SendNextSms();
	void SetSmsMessageSettingsL(CSmsMessage* aSmsMessage);
private:

	TSmsAddr iSmsAddr;
	CSmsProtocol* iSmsProtocol;
	CWapSmsProvider* iProvider;
	CArrayPtrFlat<CSmsMessage>* iSmsMsgArray;
	};


/**
 *  @internalComponent
 */
class CWapProtocolObserver : public CBase, public MSmsMessageObserver
	{
public:
	static CWapProtocolObserver* NewL(CWapSmsProtocol* aProtocol);
public:
// From MSmsMessageObserver
	virtual const TSmsAddr& GetLocalAddress() const;
	virtual void SetLocalAddress(const TSmsAddr& aSmsAddr);
	virtual void ModemNotificationCompleted(TInt aStatus);
	virtual void MessageSendCompleted(TInt aStatus);
	virtual TInt MessageReceived(const CSmsMessage& aSmsMessage,TDes& aDes);
	virtual TBool ClientConfirmsMessage()const;
	virtual TInt SmsAddrIsDuplicate(const MSmsMessageObserver* aObserver,const TSmsAddr& aAddr)const;
	virtual void EnumeratePhoneCompleted(TInt aStatus);
	virtual void MessageWriteCompleted(TInt aStatus, const CSmsMessage* aSmsMessage = NULL);
	virtual void MessageDeleteCompleted(TInt aStatus);
    virtual void ReadSmsParamsCompleted(TInt aStatus,CMobilePhoneSmspList* aSmspList);
    virtual void WriteSmsParamsCompleted(TInt aStatus);
private:

	TSmsAddr iSmsAddr;
	CWapSmsProtocol* iWapSmsProtocol;
	};

#endif
