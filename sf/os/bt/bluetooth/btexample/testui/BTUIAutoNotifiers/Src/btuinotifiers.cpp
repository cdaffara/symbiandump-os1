// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <btextnotifiers.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <btextnotifierspartner.h>
#endif

#include <e32cons.h>
#include <es_prot.h>
#include <networking/panuiinterfaces.h>
#include <e32debug.h>
#include "btuinotifiers.h"
#include <es_sock.h>
#include <e32msgqueue.h>

#define PIN_DEFAULT  "1234"
_LIT8(KDefaultPinCode, PIN_DEFAULT);

const TUint KDefaultAuthorisation = 1; // Accept

/*There can be any number of keypress notifications, I am taking 20 just for testing purpose*/
const TUint KMaxNumberOfKeyPressNotifications = 20;

const TUint KDefaultSSPAuthorisation = 1;

#define DEFAULT_PBAP_AUTH_PASSWORD "password"

_LIT8(KNotUsed, "Not used");

const TUid KTechViewScreenOutputChannel={0x10009D48};

const TInt KBTNotifierArrayIncrement = 2;

CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
	{
	CArrayPtrFlat<MEikSrvNotifierBase2>* subjects=NULL;
	TRAPD( err, subjects=new (ELeave)CArrayPtrFlat<MEikSrvNotifierBase2>(KBTNotifierArrayIncrement) );
	if( err == KErrNone )
		{
		TRAP( err, subjects->AppendL( CBTUIAutoPasskeyNotifier::NewL() ) );
		TRAP( err, subjects->AppendL( CBTUIAutoAuthNotifier::NewL() ) );
		TRAP( err, subjects->AppendL( CPbapUIAutoAuthNotifier::NewL() ) );
		TRAP( err, subjects->AppendL( CPanNapAuthAutoNotifier::NewL() ) );
		TRAP( err, subjects->AppendL( CSSPNumericComparisionAutoNotifier::NewL() ) );
		TRAP( err, subjects->AppendL( CSSPPassKeyAutoNotifier::NewL() ) );
		TRAP( err, subjects->AppendL( CBTUIAutoPinCodeNotifier::NewL() ));				
		return(subjects);
		}
	else
		{
		return NULL;
		}
	}

//Adding ECOM Support
#include <ecom/implementationproxy.h>
const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(0x10201D09, NotifierArray)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
}

//Update the P&S key for the notifier type so test code can check which type of notifier has fired
//This is not critical so work on a best effort approach. If it fails then we don't care unless it's
//not defined
void UpdateLastNotifierType(TAutoNotifierType aType)
	{
	TInt err = RProperty::Set(KUidSystemCategory, KUidLastNotifierTypeRun, aType);
	if(err == KErrNotFound)
		{
		_LIT_SECURITY_POLICY_PASS(KPassPolicy);
		err = RProperty::Define(KUidSystemCategory, KUidLastNotifierTypeRun, RProperty::EInt, KPassPolicy, KPassPolicy);
		if(err == KErrNone || err == KErrAlreadyExists)
			{
			RProperty::Set(KUidSystemCategory, KUidLastNotifierTypeRun, ESspNumericComparisonNotifier);
			}
		}
	}


//  * * *         CBTUIAutoPasskeyNotifier * * *
CBTUIAutoPasskeyNotifier* CBTUIAutoPasskeyNotifier::NewL()
	{
	CBTUIAutoPasskeyNotifier* self=new (ELeave) CBTUIAutoPasskeyNotifier();	
	CleanupStack::PushL(self);
	self->ConstructL();	
	CleanupStack::Pop();
	return self;
	}

CBTUIAutoPasskeyNotifier::CBTUIAutoPasskeyNotifier() 
	{
	}

void CBTUIAutoPasskeyNotifier::ConstructL()
	{
	}

CBTUIAutoPasskeyNotifier::~CBTUIAutoPasskeyNotifier()
	{

	}

void CBTUIAutoPasskeyNotifier::Release()
	{
	delete this;
	}

CBTUIAutoPasskeyNotifier::TNotifierInfo CBTUIAutoPasskeyNotifier::RegisterL()
	{
	iInfo.iUid=KBTManPinNotifierUid;
	iInfo.iChannel=KTechViewScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityLow;
	return iInfo;
	}

CBTUIAutoPasskeyNotifier::TNotifierInfo CBTUIAutoPasskeyNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CBTUIAutoPasskeyNotifier::StartL(const TDesC8&)
	{
	return TPtrC8(KNotUsed);
	}

void CBTUIAutoPasskeyNotifier::StartL(const TDesC8&, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	iMessage = aMessage;
    iReplySlot = aReplySlot;	
	// read properties	
	TBTPinCode pinCode;
	
	TBuf<16> buf;
	TInt err = RProperty::Get(KUidSystemCategory, KUidAutoPropertyPinCode, buf);
	
	if (err == KErrNotFound)
		{
		// p+s key not defined, use default pin

		buf.Copy(KDefaultPinCode);
		}
	else
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}
	
	for (TUint i=0; i<buf.Length(); i++)
		{
		pinCode().iPIN[i] = buf[i];
		}
	pinCode().iLength = buf.Length();
	
	TInt notifierError = KErrNone;	
	/* Get the value of the P&S key 'KUidErrorNotifiers' to see if need to complete the notifiers with an error
	 * If P&S key is not defined then we will proceed normally otherwise we will complete the notifiers 
	 * the specified error*/	
	err = RProperty::Get(KUidSystemCategory, KUidErrorNotifiers, notifierError);
	
	if (err == KErrNotFound)
		{
		PinInputComplete(pinCode, KErrNone);
		}
	else
		{
		/*If error is defined then complete the notifiers with an error*/
		PinInputComplete(pinCode, notifierError);
		}

	return;	
	}


void CBTUIAutoPasskeyNotifier::Cancel()
	{
	}
	
void CBTUIAutoPasskeyNotifier::UpdateL(const TDesC8& /*aBuffer*/, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	{
	aMessage.Complete(KErrNone);
	}

TPtrC8 CBTUIAutoPasskeyNotifier::UpdateL(const TDesC8&)
	{
	TPtrC8 ret(KNullDesC8);
	return (ret);
	}
	
void CBTUIAutoPasskeyNotifier::PinInputComplete(const TBTPinCode& aPin, TInt aReason)
	{	
	// update executed status, ignore error, what can we do !?!
	RProperty::Set(KUidSystemCategory, KUidAuthenticationExecuted, 1);
	
	if (aReason == KErrNone)
		{		
		TInt err = iMessage.Write(iReplySlot, aPin);
		
		iMessage.Complete(err);
		}
	else
		{
		iMessage.Complete(aReason);
		}
	}


// CBTUIAutoAuthNotifier 
CBTUIAutoAuthNotifier* CBTUIAutoAuthNotifier::NewL()
	{
	CBTUIAutoAuthNotifier* self=new (ELeave) CBTUIAutoAuthNotifier();	
	CleanupStack::PushL(self);
	self->ConstructL();	
	CleanupStack::Pop();
	return self;
	}

CBTUIAutoAuthNotifier::CBTUIAutoAuthNotifier() 
	{
	}

void CBTUIAutoAuthNotifier::ConstructL()
	{
	}

CBTUIAutoAuthNotifier::~CBTUIAutoAuthNotifier()
	{
	
	}

void CBTUIAutoAuthNotifier::Release()
	{
	delete this;
	}

CBTUIAutoAuthNotifier::TNotifierInfo CBTUIAutoAuthNotifier::RegisterL()
	{
	iInfo.iUid=KBTManAuthNotifierUid;
	iInfo.iChannel=KTechViewScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityLow;
	return iInfo;
	}

CBTUIAutoAuthNotifier::TNotifierInfo CBTUIAutoAuthNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CBTUIAutoAuthNotifier::StartL(const TDesC8&)
	{
	return TPtrC8(KNotUsed);
	}

void CBTUIAutoAuthNotifier::StartL(const TDesC8&, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	iMessage = aMessage;
    iReplySlot = aReplySlot;
	// read properties
	TInt authoriseAccept;
		
	TInt err = RProperty::Get(KUidSystemCategory, KUidAutoPropertyAuthoriseAccept, authoriseAccept);
	
	if (err == KErrNotFound)
		{
		// p+s key not defined, use default (Yes)
		authoriseAccept = KDefaultAuthorisation;
		}
	else
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}
	
	AuthorisationComplete(authoriseAccept);
	return;
	}

void CBTUIAutoAuthNotifier::Cancel()
	{
	}

void CBTUIAutoAuthNotifier::UpdateL(const TDesC8& /*aBuffer*/, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	{
	aMessage.Complete(KErrNone);
	}

TPtrC8 CBTUIAutoAuthNotifier::UpdateL(const TDesC8&)
	{
	TPtrC8 ret(KNullDesC8);
	return (ret);
	}
	
void CBTUIAutoAuthNotifier::AuthorisationComplete(TInt aAuthorised)
	{

		TInt err = iMessage.Write(iReplySlot, TPckgBuf<TBool>(aAuthorised));
		// update executed status
		RProperty::Set(KUidSystemCategory, KUidAuthorisationExecuted, 1);		
		iMessage.Complete(err);
	}

//
// CPbapUIAutoAuthNotifier
//
CPbapUIAutoAuthNotifier* CPbapUIAutoAuthNotifier::NewL()
	{
	CPbapUIAutoAuthNotifier* self = new (ELeave) CPbapUIAutoAuthNotifier();
	return self;
	}

CPbapUIAutoAuthNotifier::CPbapUIAutoAuthNotifier()
	{
	}

void CPbapUIAutoAuthNotifier::Release()
	{
	delete this;
	}

void CPbapUIAutoAuthNotifier::Cancel()
	{
	}

CPbapUIAutoAuthNotifier::~CPbapUIAutoAuthNotifier()
	{
	iProperty.Close();
	}

CPbapUIAutoAuthNotifier::TNotifierInfo CPbapUIAutoAuthNotifier::RegisterL()
	{
	iInfo.iUid=KPbapAuthNotifierUid;
	iInfo.iChannel=KTechViewScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CPbapUIAutoAuthNotifier::TNotifierInfo CPbapUIAutoAuthNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CPbapUIAutoAuthNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CPbapUIAutoAuthNotifier::StartL(const TDesC8& /*aBuffer*/, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);

	TBuf<16> buf;
	TInt err = iProperty.Get(KUidSystemCategory, KUidPbapAutoNotifierPassword, buf);
	
	if (err == KErrNotFound)
		{
		// p+s key not defined, use default password
		_LIT8(KDefaultPbapAuthPassword, DEFAULT_PBAP_AUTH_PASSWORD);
		
		buf.Copy(KDefaultPbapAuthPassword);
		}
	else	
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}
	
	PasswordInputComplete(buf, KErrNone);
	}

void CPbapUIAutoAuthNotifier::UpdateL(const TDesC8& /*aBuffer*/, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	{
	aMessage.Complete(KErrNone);
	}

TPtrC8 CPbapUIAutoAuthNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CPbapUIAutoAuthNotifier::PasswordInputComplete(const TDesC& aPassword, TInt aReason)
	{
	// update executed status, ignore error, what can we do !?!
	RProperty::Set(KUidSystemCategory, KUidPbapAutoNotifierExecuted, 1);	
	if (aReason == KErrNone)
		{
		TPbapAuthNotifierResponsePckg pckg;
		pckg().SetPassword(aPassword);		
		TInt err = iMessage.Write(iReplySlot, pckg);
		iMessage.Complete(err);
		}
	else
		{
		iMessage.Complete(aReason);
		}
	}


//
// CPanNapAuthAutoNotifier
//
CPanNapAuthAutoNotifier* CPanNapAuthAutoNotifier::NewL()
	{
	CPanNapAuthAutoNotifier* self = new (ELeave) CPanNapAuthAutoNotifier();
	return self;
	}

CPanNapAuthAutoNotifier::CPanNapAuthAutoNotifier()
	{
	}

void CPanNapAuthAutoNotifier::Release()
	{
	delete this;
	}

void CPanNapAuthAutoNotifier::Cancel()
	{
	}

CPanNapAuthAutoNotifier::~CPanNapAuthAutoNotifier()
	{
	iProperty.Close();
	}

CPanNapAuthAutoNotifier::TNotifierInfo CPanNapAuthAutoNotifier::RegisterL()
	{
	iInfo.iUid=KBTPanNapUplinkAuthorisationNotifierUid;
	iInfo.iChannel=KTechViewScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CPanNapAuthAutoNotifier::TNotifierInfo CPanNapAuthAutoNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CPanNapAuthAutoNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CPanNapAuthAutoNotifier::StartL(const TDesC8& /*aBuffer*/, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);

	TInt psResult;
	TNapConnectionResult result = EAcceptNapConnectionAllowUplinkAccess;	// Default value if P&S key is not setup
	TInt err = iProperty.Get(KUidSystemCategory, KUidPanNapAutoNotifierResult, psResult);
	if(err == KErrNone)
		{
		result = static_cast<TNapConnectionResult>(psResult);
		if(result != EDisallowNewNapConnection &&
		   result != EAcceptNapConnectionAllowUplinkAccess &&
		   result != EAcceptNapConnectionDisallowUplinkAccess)
			{
			User::Leave(KErrArgument);
			}
		}
	else	
		{
		// Ensure no unexpected error occurred (KErrNotFound will be received if the key has not been
		// setup)
		if(err != KErrNotFound)
			{
			User::LeaveIfError(err);
			}
		}
	
	PanNapNotifierComplete(result, KErrNone);
	}

TPtrC8 CPanNapAuthAutoNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CPanNapAuthAutoNotifier::PanNapNotifierComplete(TNapConnectionResult aResult, TInt aReason)
	{
	if(aReason == KErrNone)
		{
		TNapConnectionResultPckg pckg;
		pckg = aResult;		
		TInt err = iMessage.Write(iReplySlot, pckg);
		iMessage.Complete(err);
		}
	else
		{
		iMessage.Complete(aReason);
		}

	// update executed status, ignore error, what can we do !?!
	iProperty.Set(KUidSystemCategory, KUidPanNapAutoNotifierExecuted, 1);
	}



//
// CSSPNumericComparisionAutoNotifier
//

CSSPNumericComparisionAutoNotifier* CSSPNumericComparisionAutoNotifier::NewL()
	{
	CSSPNumericComparisionAutoNotifier* self = new(ELeave) CSSPNumericComparisionAutoNotifier();
	return self;
	}

CSSPNumericComparisionAutoNotifier::CSSPNumericComparisionAutoNotifier()
	{
	}

void CSSPNumericComparisionAutoNotifier::Release()
	{
	delete this;
	}

void CSSPNumericComparisionAutoNotifier::Cancel()
	{
	}

CSSPNumericComparisionAutoNotifier::~CSSPNumericComparisionAutoNotifier()
	{
	}

CSSPNumericComparisionAutoNotifier::TNotifierInfo CSSPNumericComparisionAutoNotifier::RegisterL()
	{
	iInfo.iUid=KBTNumericComparisonNotifierUid;
	iInfo.iChannel=KTechViewScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CSSPNumericComparisionAutoNotifier::TNotifierInfo CSSPNumericComparisionAutoNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CSSPNumericComparisionAutoNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	User::Panic(KPanicCat, ESynchronousStartWithNumericComparison);
	return KNullDesC8();
	}

void CSSPNumericComparisionAutoNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	UpdateLastNotifierType(ESspNumericComparisonNotifier);
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
	TBTNumericComparisonParamsPckg pckg;
	pckg.Copy(aBuffer);
	
	/*Update a P&S key for identifying who initiated the authentication*/
	TInt err = RProperty::Set(KUidSystemCategory, KUidLocallyInitiated, pckg().LocallyInitiated());
	
	if (err == KErrNotFound)
		{
		/* p+s key not defined by the test
		 * May be autonotifiers are not called for testing purpose (i.e. just running epoc in startupmode 4) 
		 * so we should not panic here and hence do nothing
		 */		
		}
	else
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}
	
	/*Read the Numeric Value*/
	TInt numericValue;
	
	numericValue = pckg().NumericalValue();
	
	/*Now I need to update a P&S key which will have the NumericValue*/
	err = RProperty::Set(KUidSystemCategory, KUidSspNumericValue, numericValue);
	
	if (err == KErrNotFound)
		{
		/* p+s key not defined by the test
		 * May be autonotifiers are not called for testing purpose (i.e. just running epoc in startupmode 4) 
		 * so we should not panic here and hence do nothing
		 */		
		}
	else
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}
  
	/* Complete the Numeric comparision*/
	TInt authoriseAccept;
	err = RProperty::Get(KUidSystemCategory, KUidSspAuthorisation, authoriseAccept);
	
	if (err == KErrNotFound)
		{
		// p+s key not defined, use default (Yes)
		authoriseAccept = KDefaultSSPAuthorisation;
		}
	else
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}
	
	SSPNumericComparisonComplete(authoriseAccept);
	}

void CSSPNumericComparisionAutoNotifier::UpdateL(const TDesC8& /*aBuffer*/, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	{
	aMessage.Complete(KErrNone);
	}

TPtrC8 CSSPNumericComparisionAutoNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CSSPNumericComparisionAutoNotifier::SSPNumericComparisonComplete(TInt aAuthoriseAccept)
	{	
	TInt err = iMessage.Write(iReplySlot, TPckgC<TBool>(aAuthoriseAccept));
	
	if(err != KErrNone)
		{
		iMessage.Complete(err);	
		}
	else
		{	
		TInt notifierError = KErrNone;	
		/* Get the value of the P&S key 'KErrorNotifiers' to see if need to complete the notifiers with an error
		 * If P&S key is not defined then we will proceed normally otherwise we will complete the notifiers 
		 * the specified error*/	
		err = RProperty::Get(KUidSystemCategory, KUidErrorNotifiers, notifierError);
		
		if (err == KErrNotFound)
			{
			iMessage.Complete(KErrNone);	
			}
		else
			{
			/*If error is defined then complete the notifiers with an error*/
			iMessage.Complete(notifierError);	
			}
		}
	}



//
// CSSPPassKeyAutoNotifier
//

CSSPPassKeyAutoNotifier* CSSPPassKeyAutoNotifier::NewL()
	{
	CSSPPassKeyAutoNotifier* self = new(ELeave) CSSPPassKeyAutoNotifier();
	return self;
	}

CSSPPassKeyAutoNotifier::CSSPPassKeyAutoNotifier()
	{
	}

void CSSPPassKeyAutoNotifier::Release()
	{
	delete this;
	}

void CSSPPassKeyAutoNotifier::Cancel()
	{	
	if (iMessage.Handle())
		{
		iMessage.Complete(KErrCancel);
		}	
	}

CSSPPassKeyAutoNotifier::~CSSPPassKeyAutoNotifier()
	{
	}

CSSPPassKeyAutoNotifier::TNotifierInfo CSSPPassKeyAutoNotifier::RegisterL()
	{
	iInfo.iUid=KBTPasskeyDisplayNotifierUid;
	iInfo.iChannel=KTechViewScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CSSPPassKeyAutoNotifier::TNotifierInfo CSSPPassKeyAutoNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CSSPPassKeyAutoNotifier::StartL(const TDesC8& )
	{
	User::Panic(KPanicCat, ESynchronousStartWithPasskeyDisplay);
	return KNullDesC8();
	}

void CSSPPassKeyAutoNotifier::StartL(const TDesC8& aBuffer, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	{	
	UpdateLastNotifierType(ESspPassKeyNotifier);
	TBTPasskeyDisplayParamsPckg pckg;
	pckg.Copy(aBuffer);
	
	/*Update a P&S key for identifying who initiated the authentication*/
	TInt err = RProperty::Set(KUidSystemCategory, KUidLocallyInitiated, pckg().LocallyInitiated());
	
	if (err == KErrNotFound)
		{
		/* p+s key not defined by the test
		 * May be autonotifiers are not called for testing purpose (i.e. just running epoc in startupmode 4) 
		 * so we should not panic here and hence do nothing
		 */		
		}
	else
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}	
	
	/*Read the Numeric Value*/
	TInt passKeyValue;
	
	passKeyValue = pckg().NumericalValue();
	
	/*Now I need to update a P&S key which will have the Pass key Value*/
	err = RProperty::Set(KUidSystemCategory, KUidSspPassKey, passKeyValue);
	
	if (err == KErrNotFound)
		{
		/* p+s key not defined by the test
		 * May be autonotifiers are not called for testing purpose (i.e. just running epoc in startupmode 4) 
		 * so we should not panic here and hence do nothing
		 */		
		}
	else
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}	
	
	iMessage = RMessage2(aMessage);
	
	TInt notifierError = KErrNone;	
	/* Get the value of the P&S key 'KUidErrorNotifiers' to see if need to complete the notifiers with an error
	 * If P&S key is not defined then we will proceed normally otherwise we will complete the notifiers 
	 * the specified error*/	
	err = RProperty::Get(KUidSystemCategory, KUidErrorNotifiers, notifierError);
	
	if (err != KErrNotFound)
		{
		//If error is defined then complete the notifiers with an error
		iMessage.Complete(notifierError);	
		}	
	else
		{
		//Do nothing. We do not have to complete this msg here as stack will cancel it when it gets a 
		//simple pairing complete event. 
		}
	}

TPtrC8 CSSPPassKeyAutoNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CSSPPassKeyAutoNotifier::UpdateL(const TDesC8& aBuffer, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	{		
	TBTNotifierUpdateParamsPckg2 pckgRaw;
	pckgRaw.Copy(aBuffer.Left(pckgRaw.MaxLength()));
	
	RMsgQueue<THCIPasskeyEntryNotificationType> keyPressNotificationQueue;
	_LIT(KKeypressNotificationQueue, "KeypressNotificationQueue");
	
	TInt err;
	
	err = keyPressNotificationQueue.OpenGlobal(KKeypressNotificationQueue);
	if(err == KErrNotFound)
		{
		err = keyPressNotificationQueue.CreateGlobal(KKeypressNotificationQueue,KMaxNumberOfKeyPressNotifications);
		User::LeaveIfError(err);		
		}
	else 
		{
		User::LeaveIfError(err);
		}
	
	/*Notify the test code for the passkey press*/
	if (pckgRaw().Type() == TBTNotifierUpdateParams2::EPasskeyDisplay)
		{
		TBTPasskeyDisplayUpdateParamsPckg pckg;
		pckg.Copy(aBuffer);
		
		THCIPasskeyEntryNotificationType keypressNotification = pckg().KeypressNotification();
		
		keyPressNotificationQueue.SendBlocking(keypressNotification);
		
		switch (keypressNotification)
			{
			case EPasskeyEntryStarted:
			case EPasskeyDigitEntered:
			case EPasskeyDigitDeleted:
			case EPasskeyCleared:			
			case EPasskeyEntryCompleted:
				break;
			}
		}
	
	TInt notifierError = KErrNone;	
	/* Get the value of the P&S key 'KUidErrorNotifiers' to see if need to complete the notifiers with an error
	 * If P&S key is not defined then we will proceed normally otherwise we will complete the notifiers 
	 * the specified error*/	
	err = RProperty::Get(KUidSystemCategory, KUidErrorNotifiers, notifierError);
	
	if (err == KErrNotFound)
		{
		aMessage.Complete(KErrNone);	
		}
	else
		{
		/*If error is defined then complete the notifiers with an error*/
		iMessage.Complete(notifierError);
		aMessage.Complete(KErrNone);
		}		
	}

//
// CBTUIAutoPinCodeNotifier
//

CBTUIAutoPinCodeNotifier* CBTUIAutoPinCodeNotifier::NewL()
	{
	CBTUIAutoPinCodeNotifier* self = new(ELeave) CBTUIAutoPinCodeNotifier();	
	return self;
	}

CBTUIAutoPinCodeNotifier::CBTUIAutoPinCodeNotifier() 
	{
	}

CBTUIAutoPinCodeNotifier::~CBTUIAutoPinCodeNotifier()
	{
	}

void CBTUIAutoPinCodeNotifier::Release()
	{
	delete this;
	}

CBTUIAutoPinCodeNotifier::TNotifierInfo CBTUIAutoPinCodeNotifier::RegisterL()
	{
	iInfo.iUid=KBTPinCodeEntryNotifierUid;
	iInfo.iChannel=KTechViewScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityLow;
	return iInfo;
	}

CBTUIAutoPinCodeNotifier::TNotifierInfo CBTUIAutoPinCodeNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CBTUIAutoPinCodeNotifier::StartL(const TDesC8&)
	{
	User::Panic(KPanicCat, ESynchronousStartWithPinCodeEntry);
	return KNullDesC8();
	}

void CBTUIAutoPinCodeNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	UpdateLastNotifierType(ELegacyPinCodeNotifier);
	iMessage = aMessage;
    iReplySlot = aReplySlot;	
	// read properties	
	TBTPinCode pinCode;
	
	TBTPinCodeEntryNotifierParamsPckg pckg;
	if(aBuffer.Length() != sizeof(TBTPinCodeEntryNotifierParams))
		{
		User::Leave(KErrArgument);
		}
	pckg.Copy(aBuffer);
	TUint passkeyMinLength;
	passkeyMinLength = pckg().PinCodeMinLength();
	
	
	TBuf<16> buf;
	TInt err = RProperty::Get(KUidSystemCategory, KUidAutoPropertyPinCode, buf);
	
	if (err == KErrNotFound)
		{
		// p+s key not defined, use default pin	
		buf.Copy(KDefaultPinCode);
		}
	else
		{
		// make sure no other error occurred
		User::LeaveIfError(err);
		}
	
	for (TUint i=0; i<buf.Length(); i++)
		{
		pinCode().iPIN[i] = buf[i];
		}
	pinCode().iLength = buf.Length();
	
	/* Passkey length should not be less then the expected length.
	 * If it is smaller than complete the notifiers with an error
	 */  
	if(buf.Length() < passkeyMinLength )
		{
		PinInputComplete(pinCode, KErrArgument);
		}
	else
		{			
		TInt notifierError = KErrNone;	
		/* Get the value of the P&S key 'KUidErrorNotifiers' to see if need to complete the notifiers with an error
		 * If P&S key is not defined then we will proceed normally otherwise we will complete the notifiers 
		 * the specified error*/	
		err = RProperty::Get(KUidSystemCategory, KUidErrorNotifiers, notifierError);
		
		if (err == KErrNotFound)
			{		
			PinInputComplete(pinCode, KErrNone);
			}
		else
			{
			/*If error is defined then complete the notifiers with an error*/
			PinInputComplete(pinCode, notifierError);
			}
		}
	}


void CBTUIAutoPinCodeNotifier::Cancel()
	{
	}
	
void CBTUIAutoPinCodeNotifier::UpdateL(const TDesC8& /*aBuffer*/, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	{
	aMessage.Complete(KErrNone);
	}

TPtrC8 CBTUIAutoPinCodeNotifier::UpdateL(const TDesC8&)
	{
	return KNullDesC8();
	}
	
void CBTUIAutoPinCodeNotifier::PinInputComplete(const TBTPinCode& aPin, TInt aReason)
	{
	
	// update executed status, ignore error, what can we do !?!
	RProperty::Set(KUidSystemCategory, KUidAuthenticationExecuted, 1);
	
	if (aReason == KErrNone)
		{
		TInt err = iMessage.Write(iReplySlot, aPin);
		iMessage.Complete(err);
		}
	else
		{
		iMessage.Complete(aReason);
		}
	}

