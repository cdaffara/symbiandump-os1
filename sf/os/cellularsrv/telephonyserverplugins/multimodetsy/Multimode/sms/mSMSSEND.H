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
// Sms Messaging Send Message
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __MSMSSEND_H__
#define __MSMSSEND_H__

#include "smsbase.H"


class CATSmsMessagingSend : public CATSmsCommands
/**
 */
	{
public:
	static CATSmsMessagingSend* NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aGsmStatus);
	~CATSmsMessagingSend();

	// from CATCommands
	virtual void Start(TTsyReqHandle aTsyReqHandle,TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);

	// from CATBase
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void Complete(TInt aError,TEventSource aSource=EReadCompletion);

	// Used by client to give us access to their message data
	void SetMsgRef(TUint16* aMsgRef);
	void SetMsgAttributes(RMobileSmsMessaging::TMobileSmsSendAttributesV1* aMsgAttributes); // ::SetMsgAttributes must be called before ::Start is called

private:
	enum TState {ENotInProgress,
	             ESetPhoneToPDUMode,
				 ESendPDULengthToPhone,
				 ESendPDUToPhone,
				 ESendEscapeCharToPhone};

	void ConstructL();
	CATSmsMessagingSend(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aGsmStatus);

	// Major functionality
	void StartFindSCA();
	void DoneFindSCA();
	void SendMessageToOldPhone();
	void SendMessageToOldPhone_Stage2();
	void SendMessageToNewPhone();
	void SendPDUToPhone();
	void SendPDUToPhone_Stage2();

	// Utility functions
	void WriteTxBufferToPhone(TState aNewState);
	TInt ParseCMGSResponse();
	TBool PhoneUsesNewPDUStandard();
	void TogglePhonePDUStandard();

private:
	CCommChatString* iExpectString;
		
	TState iState;
	TBuf8<KMaxAsciiPduLength> iTxBuffer;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1* iMsgAttributes; // Message attributes passed down by client
	HBufC8* iMsgData;		 // Copy of SMS PDU passed down by client in binary format
	TBuf8<KMaxAsciiPduLength> iMsgDataAscii;  // PDU in ascii format
	RMobilePhone::TMobileAddress iMsgSCA;			// SCA to be used when sending message
	TBool iHaveRetriedWithOtherPduStd;
	TBool iStop;		// ETrue when client has requested we cancel
	};

#endif
