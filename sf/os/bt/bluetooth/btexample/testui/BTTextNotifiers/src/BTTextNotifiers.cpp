// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "BTTextNotifiers.h"
#include <btdevice.h>
#include <e32cons.h>
#include <es_prot.h>
#include <bluetooth/logger.h>
#include "pannapuplinkauthorisationnotifier.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, "TextNotifiers");
#endif


void CreateNotifiersL(CArrayPtrFlat<MNotifierBase2>& aNotifiers)
	{
	LOG_STATIC_FUNC
// Utility macro to make this function simpler.
#define ADD_BT_NOTIFIER(XType) \
	{\
	XType* notifier = XType::NewLC(); \
	aNotifiers.AppendL(notifier); \
	CleanupStack::Pop(notifier); \
	}

	ADD_BT_NOTIFIER(CBTManPinNotifier);	
	ADD_BT_NOTIFIER(CBTManAuthNotifier);
	ADD_BT_NOTIFIER(CBTManDeviceSelectionNotifier);
	ADD_BT_NOTIFIER(CBTPanDeviceSelectionNotifier);
	ADD_BT_NOTIFIER(CPbapAuthNotifier);
	ADD_BT_NOTIFIER(CPanNapUplinkAuthorisationNotifier);
	
#ifndef __BT_TEXT_NOTIFIERS_NO_SSP__
	ADD_BT_NOTIFIER(CBTUINumericComparison);
	ADD_BT_NOTIFIER(CBTUIPasskeyEntry);
	ADD_BT_NOTIFIER(CBTUIPinCodeEntry);	
#endif // __BT_TEXT_NOTIFIERS_NO_SSP__
	
#undef ADD_BT_NOTIFIER // This macro is local to this function
	}


/**
The main library entry point for a notifier implementation.
*/
EXPORT_C CArrayPtr<MNotifierBase2>* NotifierArray()
	{
	LOG_STATIC_FUNC
	FLOG(_L("RNot\tExported entry point"));
	CArrayPtrFlat<MNotifierBase2>* notifiers = new CArrayPtrFlat<MNotifierBase2>(5);
    if (notifiers)
       {
       TRAPD(err, CreateNotifiersL(*notifiers));
       if(err)
           {
			FLOG(_L("RNot\tError Creating notifier"));
           TInt count = notifiers->Count();
           while(count--)
               {
               (*notifiers)[count]->Release();
               }
           delete notifiers, notifiers = NULL;
           }
       }
    return notifiers;
	}


//
// BTTextNotifiersConsole
//

CConsoleBase* BTTextNotifiersConsole::AutoSizeNewL(const TDesC& aTitle, TSize aSize)
	{
	LOG_STATIC_FUNC
	// Try to create a console of our preferred size, otherwise we
	// guess the screen is too small and use its entirety.
	CConsoleBase* console = NULL;
	TRAPD(err, console = Console::NewL(aTitle, aSize));
	if (err != KErrNone)
		{
		// If we leave now it is not because of offscreen drawing.
		console = Console::NewL(aTitle, TSize(KConsFullScreen, KConsFullScreen));
		}
	return console;
	}


//
// CBTManPinNotifier
//

CBTManPinNotifier* CBTManPinNotifier::NewLC()
	{
	LOG_STATIC_FUNC
	CBTManPinNotifier* self = new(ELeave) CBTManPinNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBTManPinNotifier::CBTManPinNotifier()
	{
	LOG_FUNC
	}

void CBTManPinNotifier::ConstructL()
	{
	LOG_FUNC
	iEngine = CBTManPinNotifierEngine::NewL(*this);
	}

void CBTManPinNotifier::Release()
	{
	LOG_FUNC
	Cancel();
	delete this;
	}

void CBTManPinNotifier::Cancel()
	{
	LOG_FUNC
	iEngine->Stop();

	if (iNeedToCompleteMessage)
		{
		iMessage.Complete(KErrCancel);
		iNeedToCompleteMessage = EFalse;
		}
	}


CBTManPinNotifier::~CBTManPinNotifier()
	{
	LOG_FUNC
	delete iEngine;
	}


CBTManPinNotifier::TNotifierInfo CBTManPinNotifier::RegisterL()
	{
	LOG_FUNC
	iInfo.iUid=KBTManPinNotifierUid;
	iInfo.iChannel=KScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CBTManPinNotifier::TNotifierInfo CBTManPinNotifier::Info() const
	{
	LOG_FUNC
	return iInfo;
	}
	
TPtrC8 CBTManPinNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	LOG(_L("The synchronous StartL overload is not used."))
	LEAVEL(KErrNotSupported);
	return KNullDesC8();
	}
	
void CBTManPinNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	LOG_FUNC
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
	iNeedToCompleteMessage = ETrue;

	TRAPD(err, iEngine->StartLegacyPinEntryL(aBuffer));
	LOG1(_L("Pin Notifier started with result %d"), err);
	if(err != KErrNone)
		{
		aMessage.Complete(err);
		iNeedToCompleteMessage = EFalse;
		LEAVEL(err);
		}
	}



TPtrC8 CBTManPinNotifier::UpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	return iEngine->LegacyUpdateL(aBuffer);
	}

void CBTManPinNotifier::MbpceoPinInputComplete(const TBTPinCode& aPin, TInt aReason)
	{
	LOG_FUNC
	LOG1(_L("\taReason = %d"), aReason);
	if (aReason == KErrNone)
		{
		TInt err = iMessage.Write(iReplySlot, aPin);
		iMessage.Complete(err);
		}
	else
		{
		iMessage.Complete(aReason);
		}
	iNeedToCompleteMessage = EFalse;
	}


#ifndef __BT_TEXT_NOTIFIERS_NO_SSP__

//
// CBTUIPinCodeEntry
//

CBTUIPinCodeEntry* CBTUIPinCodeEntry::NewLC()
	{
	LOG_STATIC_FUNC
	CBTUIPinCodeEntry* self = new(ELeave) CBTUIPinCodeEntry();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBTUIPinCodeEntry::CBTUIPinCodeEntry()
	{
	LOG_FUNC
	}

void CBTUIPinCodeEntry::ConstructL()
	{
	LOG_FUNC
	iEngine = CBTManPinNotifierEngine::NewL(*this);
	}

void CBTUIPinCodeEntry::Release()
	{
	LOG_FUNC
	Cancel();
	delete this;
	}

void CBTUIPinCodeEntry::Cancel()
	{
	LOG_FUNC
	iEngine->Stop();

	if (iNeedToCompleteMessage)
		{
		iMessage.Complete(KErrCancel);
		iNeedToCompleteMessage = EFalse;
		}
	}

CBTUIPinCodeEntry::~CBTUIPinCodeEntry()
	{
	LOG_FUNC
	delete iEngine;
	}


CBTManPinNotifier::TNotifierInfo CBTUIPinCodeEntry::RegisterL()
	{
	LOG_FUNC
	iInfo.iUid = KBTPinCodeEntryNotifierUid;
	iInfo.iChannel = KScreenOutputChannel;
	iInfo.iPriority = ENotifierPriorityVHigh;
	return iInfo;
	}

CBTManPinNotifier::TNotifierInfo CBTUIPinCodeEntry::Info() const
	{
	LOG_FUNC
	return iInfo;
	}
	
TPtrC8 CBTUIPinCodeEntry::StartL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	LOG(_L("The synchronous StartL overload is not used."))
	LEAVEL(KErrNotSupported);
	return KNullDesC8();
	}
	
void CBTUIPinCodeEntry::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	LOG_FUNC
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
	iNeedToCompleteMessage = ETrue;

	TRAPD(err, iEngine->StartPinEntryL(aBuffer));
	LOG1(_L("Pin Notifier started with result %d"), err);
	if(err != KErrNone)
		{
		aMessage.Complete(err);
		iNeedToCompleteMessage = EFalse;
		LEAVEL(err);
		}
	}

TPtrC8 CBTUIPinCodeEntry::UpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	return iEngine->UpdateL(aBuffer);
	}

void CBTUIPinCodeEntry::MbpceoPinInputComplete(const TBTPinCode& aPin, TInt aReason)
	{
	LOG_FUNC
	LOG1(_L("\taReason = %d"), aReason);
	if (aReason == KErrNone)
		{
		TInt err = iMessage.Write(iReplySlot, aPin);
		iMessage.Complete(err);
		}
	else
		{
		iMessage.Complete(aReason);
		}
	iNeedToCompleteMessage = EFalse;
	}

#endif // __BT_TEXT_NOTIFIERS_NO_SSP__




CBTManPinNotifierEngine* CBTManPinNotifierEngine::NewL(MBTPinCodeEntryObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CBTManPinNotifierEngine* c = new (ELeave) CBTManPinNotifierEngine(aObserver);
	CleanupStack::PushL(c);
	c->ConstructL();
	CleanupStack::Pop(c);
	return c;
	}
	
void CBTManPinNotifierEngine::ConstructL()
	{
	LOG_FUNC
	}

void CBTManPinNotifierEngine::StartPinEntryL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	__ASSERT_DEBUG(((iHostResolverCacheProdder==NULL)&&(iGetPinFromConsole==NULL)), User::Panic(_L("BTPinProvider"), KErrGeneral));

	__ASSERT_DEBUG(!IsActive(), User::Panic(_L("BTPinProvider"), KErrGeneral));

	TBTPinCodeEntryNotifierParamsPckg pckg;
	if(aBuffer.Length() != sizeof(TBTPinCodeEntryNotifierParams))
		{
		User::Leave(KErrArgument);
		}
	pckg.Copy(aBuffer);

	iDeviceAddress		= pckg().DeviceAddress();
	iDeviceName			= pckg().DeviceName();
	iPasskeyMinLength	= pckg().PinCodeMinLength();
	iLocallyInitiated	= pckg().LocallyInitiated();
	iStrongKeyRequired	= pckg().StrongPinCodeRequired();
	iRecommendedPasskeyMinLength = pckg().RecommendedPinCodeMinLength();
	
	DoStartPinEntryL();
	}

void CBTManPinNotifierEngine::StartLegacyPinEntryL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	__ASSERT_DEBUG(((iHostResolverCacheProdder==NULL)&&(iGetPinFromConsole==NULL)), User::Panic(_L("BTPinProvider"), KErrGeneral));

	__ASSERT_DEBUG(!IsActive(), User::Panic(_L("BTPinProvider"), KErrGeneral));

	TBTPasskeyNotifierParamsPckg pckg;
	if(aBuffer.Length() != sizeof(TBTPasskeyNotifierParams))
		{
		User::Leave(KErrArgument);
		}
	pckg.Copy(aBuffer);

	iDeviceAddress		= pckg().iBDAddr;
	iDeviceName			= pckg().iName;
	iPasskeyMinLength	= pckg().iPasskeyMinLength;
	iLocallyInitiated	= pckg().iLocallyInitiated;
	iStrongKeyRequired	= EFalse;  // Cannot be signalled through legacy notifier.
	
	DoStartPinEntryL();
	}

void CBTManPinNotifierEngine::DoStartPinEntryL()
	{
	LOG_FUNC
	if(iDevice == NULL)
		{
		iDevice = CBTDevice::NewL(iDeviceAddress);
		FLOG(_L("RNot\tCBTManPinNotifierEngine::StartPinEntryL() - Create new CDevice"));
		}

	if (iDeviceName.Length()==0)
		{
		User::LeaveIfError(iRegistry.Connect());
		User::LeaveIfError(iRegistryView.Open(iRegistry));
		FLOG(_L("RNot\tPin Notifier looking for device in Registry"));
		// let's try the Registry for a name
		iRegistrySearch.FindAddress(iDeviceAddress);
		iRegistryView.CreateView(iRegistrySearch, iStatus);
		iRegistryState = EFinding;
		SetActive();
		}
	else
		{
		//Just work with addresses and names
		iDevice->SetDeviceNameL(BTDeviceNameConverter::ToUTF8L(iDeviceName));
		
		//Ask Host Resolver Inquiry Cache for CoD
		//DEF039276 Altered code - always try and get CoD
		iHostResolverCacheProdder = CHostResolverCacheProdder::NewL(this);
		iHostResolverCacheProdder->Start(iDeviceAddress);

		iGetPinFromConsole = CBTGetPinFromConsole::NewL(this, *iDevice, iPasskeyMinLength, iLocallyInitiated, iStrongKeyRequired, iRecommendedPasskeyMinLength);
		iGetPinFromConsole->GetPinL();
		}
	}


void CBTManPinNotifierEngine::RunL()
	{
	LOG_FUNC
	
	if(iStatus == KErrNotFound)
		{
		FLOG(_L("Device not found in Registry (OK)"))
		__ASSERT_DEBUG(iRegistryState == EFinding, User::Invariant());

		// not to worry - just no name available for the UI
		iDevice->SetDeviceAddress(iDeviceAddress);
		iGetPinFromConsole = CBTGetPinFromConsole::NewL(this, *iDevice, iPasskeyMinLength, iLocallyInitiated, iStrongKeyRequired, iRecommendedPasskeyMinLength);
		iGetPinFromConsole->GetPinL();
		}
	else if(iStatus >= KErrNone)
		{
		switch (iRegistryState)
			{
			case EFinding:
				{
				FLOG(_L("Device found in Registry - getting details"))
				iResponse = CBTRegistryResponse::NewL(iRegistryView);
				iResponse->Start(iStatus);
				SetActive();
				iRegistryState = EGetting;
				}
				break;
			case EGetting:
				{		
				FLOG(_L("Got device details, launching PIN console"))
				iGetPinFromConsole = CBTGetPinFromConsole::NewL(this, *(iResponse->Results()[0]), iPasskeyMinLength, iLocallyInitiated, iStrongKeyRequired, iRecommendedPasskeyMinLength);
				
				PerformNameUpdateL(KErrNone);
					
				iGetPinFromConsole->GetPinL();
				}
				break;
			}
		}
	}

void CBTManPinNotifierEngine::DoCancel()
	{
	LOG_FUNC
	if (iHostResolverCacheProdder)
 		{
 		iHostResolverCacheProdder->Cancel();
 		delete iHostResolverCacheProdder;
 		iHostResolverCacheProdder = NULL;
 		}
	iRegistryView.CancelRequest(iStatus);
	if (iResponse)
		{
		iResponse->Cancel();
		}
	iGetPinFromConsole->Cancel();
	}

CBTManPinNotifierEngine::~CBTManPinNotifierEngine()
	{
	LOG_FUNC
	Cancel();

	delete iHostResolverCacheProdder;
	delete iGetPinFromConsole;
	delete iDevice;

	iRegistryView.Close();
	iRegistry.Close();
	}

void CBTManPinNotifierEngine::Stop()
	{
	LOG_FUNC
	Cancel();

	if (iGetPinFromConsole)
		{
		iGetPinFromConsole->Cancel();
		delete iGetPinFromConsole;
		iGetPinFromConsole = NULL;
		}

	if (iHostResolverCacheProdder)
 		{
 		iHostResolverCacheProdder->Cancel();
 		delete iHostResolverCacheProdder;
 		iHostResolverCacheProdder = NULL;
 		}
				
	//clean up iDevice...
	delete iDevice;
	iDevice = NULL;		
	}

TPtrC8 CBTManPinNotifierEngine::LegacyUpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	
	TBTNotifierUpdateParamsPckg pckg;
	pckg.Copy(aBuffer);
	
	if(iDevice)
		{	
		//we only want to do this if our device doesn't have a valid device name...
		if (!iDevice->IsValidDeviceName())
			{
			iDeviceName = pckg().iName;
			if(iGetPinFromConsole)
				{
				PerformNameUpdateL(pckg().iResult);
				}
			}
		}
	else
		{
		FLOG(_L("RNot\tCBTManPinNotifierEngine::UpdateL() called with iDevice == NULL"));
		}
		
	return KNullDesC8();
	}

TPtrC8 CBTManPinNotifierEngine::UpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	
	TBTNotifierUpdateParamsPckg2 pckgRaw;
	pckgRaw.Copy(aBuffer.Left(pckgRaw.MaxLength()));
	
	// We only expect device name updates
	if(pckgRaw().Type() != TBTNotifierUpdateParams2::EDeviceName)
		{
		User::Leave(KErrArgument);
		}
	
	TBTDeviceNameUpdateParamsPckg pckg;
	pckg.Copy(aBuffer);
	if(iDevice)
		{
		//we only want to do this if our device doesn't have a valid device name...
		if(!iDevice->IsValidDeviceName())
			{
			iDeviceName = pckg().DeviceName();
			if(iGetPinFromConsole)
				{
				PerformNameUpdateL(pckg().Result());
				}
			}
		}
	else
		{
		FLOG(_L("RNot\tCBTManPinNotifierEngine::UpdateL() called with iDevice == NULL"));
		}
		
	return KNullDesC8();
	}

void CBTManPinNotifierEngine::PerformNameUpdateL(TInt aResult)
	{
	LOG_FUNC
	iGetPinFromConsole->DeviceNameRetrieved(iDeviceName, aResult);
	if(iDevice)
		{
		iDevice->SetDeviceNameL(BTDeviceNameConverter::ToUTF8L(iDeviceName));
		}
	else
		{
		FLOG(_L("RNot\tCBTManPinNotifierEngine::PerformNameUpdateL() called with iDevice == NULL"));
		}
	}

CBTManPinNotifierEngine::CBTManPinNotifierEngine(MBTPinCodeEntryObserver& aObserver)
	: CActive(EPriorityStandard)
	, iObserver(aObserver)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}


void CBTManPinNotifierEngine::PinInputComplete(const TBTPinCode& aPin, TInt aReason)
	{
	LOG_FUNC
	iObserver.MbpceoPinInputComplete(aPin, aReason);
	delete iHostResolverCacheProdder;
	iHostResolverCacheProdder = NULL;
	delete iDevice;
	iDevice = NULL;
	delete iGetPinFromConsole;
	iGetPinFromConsole = NULL;
	}

void CBTManPinNotifierEngine::HostResolverCacheInfoReceived(const TNameEntry& aResult)
	{
	LOG_FUNC
	__ASSERT_DEBUG(((iGetPinFromConsole!=NULL)), User::Panic(_L("BTPinProvider"), KErrGeneral));

	TInquirySockAddr& sa = TInquirySockAddr::Cast(aResult().iAddr);
	TBTDeviceClass deviceClass(sa.MajorServiceClass(), sa.MajorClassOfDevice(), sa.MinorClassOfDevice());
	iGetPinFromConsole->CoDRetrieved(deviceClass);
	}
	


//------------------------------------------------------------------------//
//Get CoD from HostResolver class
//------------------------------------------------------------------------//
CHostResolverCacheProdder* CHostResolverCacheProdder::NewL(CBTManPinNotifierEngine* aParent)
	{
	LOG_STATIC_FUNC
	CHostResolverCacheProdder* self = CHostResolverCacheProdder::NewLC(aParent);
	CleanupStack::Pop();
	return self;	
	}
	
CHostResolverCacheProdder* CHostResolverCacheProdder::NewLC(CBTManPinNotifierEngine* aParent)
	{
	LOG_STATIC_FUNC
	CHostResolverCacheProdder* self = new (ELeave) CHostResolverCacheProdder(aParent);
	CleanupStack::PushL(self);
	return self;
	}
	
CHostResolverCacheProdder::~CHostResolverCacheProdder() 
	{
	LOG_FUNC
	Cancel();
	}

CHostResolverCacheProdder::CHostResolverCacheProdder(CBTManPinNotifierEngine* aParent)
: CActive(EPriorityStandard), iParent(aParent)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}
	
	

TInt CHostResolverCacheProdder::Start(const TBTDevAddr& aBDAddr)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!IsActive(), User::Panic(_L("HostResolverCacheProdder"), KErrGeneral));
	
	FLOG(_L("RNot\tCHostResolverCacheProdder::Start"));
	RSocketServ sockServer;
	sockServer.Connect();
	TProtocolDesc info;
	/**/
 	TInt ret1 = sockServer.FindProtocol(_L("BTLinkManager"),info);
 	if(ret1!=KErrNone)
	 	{
		FTRACE(FPrint(_L("RNot\tFailure to find btlinkmanager - error %d"), ret1));
		TInquirySockAddr& sa = TInquirySockAddr::Cast(iResult().iAddr);
		sa.SetMajorServiceClass(0);
		sa.SetMajorClassOfDevice(0);
		sa.SetMinorClassOfDevice(0);
		iParent->HostResolverCacheInfoReceived(iResult);
		return ret1;
	 	}
	 /**/
	TInt ret = iHostResolver.Open(sockServer, KBTAddrFamily, KBTLinkManager);
	iAddr.SetBTAddr(aBDAddr);
	iAddr.SetIAC(KGIAC);
	iAddr.SetAction(KHostResCache);
	if(ret==KErrNone)
		{
		iHostResolver.GetByAddress(iAddr, iResult, iStatus); 
		SetActive();
		}
	return ret;
	}

void CHostResolverCacheProdder::RunL()
	{
	LOG_FUNC
	if (iStatus.Int() == KErrNone)
		{
		iHostResolver.Close();
		iParent->HostResolverCacheInfoReceived(iResult);
		}
	else
		{
		FTRACE(FPrint(_L("RNot\tNo CoD returned from Host Resolver - error %d"), iStatus.Int()));
		iHostResolver.Close();
		}
	}



//------------------------------------------------------------------------//
//Get pin from a console window class
//------------------------------------------------------------------------//


CBTGetPinFromConsole* CBTGetPinFromConsole::NewL(CBTManPinNotifierEngine* aParent, const CBTDevice& aDevice, TUint aPasskeyMinLength, TBool aInternallyInitiated, TBool aStrongKeyRequired, TUint aRecommendedPasskeyMinLength)
	{
	LOG_STATIC_FUNC
	CBTGetPinFromConsole* self = new(ELeave) CBTGetPinFromConsole(aParent, aPasskeyMinLength, aInternallyInitiated, aStrongKeyRequired, aRecommendedPasskeyMinLength);
	CleanupStack::PushL(self);
	self->ConstructL(aDevice);
	CleanupStack::Pop(self);
	return self;
	}

CBTGetPinFromConsole::CBTGetPinFromConsole(CBTManPinNotifierEngine* aParent, TUint aPasskeyMinLength, TBool aInternallyInitiated, TBool aStrongKeyRequired, TUint aRecommendedPasskeyMinLength)
	: CActive(EPriorityStandard)
	, iParent(aParent)
	, iPasskeyMinLength(aPasskeyMinLength)
	, iInternallyInitiated(aInternallyInitiated)
	, iStrongKeyRequired(aStrongKeyRequired)
	, iRecommendedPasskeyMinLength(aRecommendedPasskeyMinLength)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CBTGetPinFromConsole::ConstructL(const CBTDevice& aDevice)
	{
	LOG_FUNC
	iConsole = BTTextNotifiersConsole::AutoSizeNewL(_L("PIN Input"), TSize(KConsFullScreen,KConsFullScreen));
	iDevice = &aDevice;//.CopyL();

#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	TCallBack autoNotifierCB(AutoNotifierCallBack, this);
	iAutoNotifierCallback = new (ELeave)CAsyncCallBack(autoNotifierCB, EActiveMedPriority);
#endif // __BT_TEXT_NOTIFIERS_AUTO__
	}

CBTGetPinFromConsole::~CBTGetPinFromConsole()
	{
	LOG_FUNC
	Cancel();
	delete iConsole;
	
#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	delete iAutoNotifierCallback;
#endif
	}

void CBTGetPinFromConsole::RunL()
	{
	LOG_FUNC
#ifdef __BT_TEXT_NOTIFIERS_AUTO__

	// Set pin to 1234
	iPin().iPIN[0] = 0x31;
	iPin().iPIN[1] = 0x32;
	iPin().iPIN[2] = 0x33;
	iPin().iPIN[3] = 0x34;
	
	iPin().iLength = 4;
	
	iParent->PinInputComplete(iPin, KErrNone);
	
#else // __BT_TEXT_NOTIFIERS_AUTO__

	TKeyCode key = iConsole->KeyCode();
	if (key == EKeyEnter)
		{
		if (iPin().iLength >= iPasskeyMinLength) 
			{
			iParent->PinInputComplete(iPin, KErrNone);
			}
		else 
			{
			iConsole->Printf(_L("\nToo short!...\nPress any key to continue\n"));
			TRequestStatus stat;
			iConsole->Read(stat);
			User::WaitForRequest(stat);
			GetPinL();
			return;	
			}	
		}
	else if (key == EKeyEscape)
		{
		iParent->PinInputComplete(iPin, KErrCancel);
		}
	else
		{
		if (iInsert < KHCIPINCodeSize)
			{
			TBuf<1> b;
			b.Append(key);
			iPin().iPIN[iInsert] = (TUint8)key;
			iConsole->Printf(b);
			iPin().iLength = (TUint8)++iInsert;
			}
		IssueRequestL();
/*
		else
			{
			iConsole->Printf(_L("\nToo many characters...\nPress any key to continue\n"));
			TRequestStatus stat;
			iConsole->Read(stat);
			User::WaitForRequest(stat);
			GetPinL();
			}
*/
		}
#endif // __BT_TEXT_NOTIFIERS_AUTO__
	}

void CBTGetPinFromConsole::DoCancel()
	{
	LOG_FUNC
#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	iConsole->ReadCancel();
#endif
	iConsole->Printf(_L("\nPin cancelled.\nPress any key to continue...\n"));
	iPin.Zero();
	}

void CBTGetPinFromConsole::GetPinL()
	{
	LOG_FUNC
	iInsert = 0;
	iConsole->ClearScreen();
	iConsole->Printf(_L("Please enter pin for device:\nAddress: "));
	TBTDevAddr a = iDevice->BDAddr();
	iConsole->Printf(_L("0x%02x%02x%02x%02x%02x%02x"), a[0], a[1], a[2], a[3], a[4], a[5]);
	if (iDevice->IsValidDeviceName())
		{
		TBTDeviceName dispBuf;
		dispBuf.Copy(iDevice->DeviceName());
		iConsole->Printf(_L("\nName: %S"), &dispBuf);
		}

  	// check if PINCodeLength is set
    if (0<iPasskeyMinLength && iPasskeyMinLength<=KHCIPINCodeSize)
    	{
		iConsole->Printf(_L("\nRequired Minimal PIN code length: %d"), iPasskeyMinLength);  	
    	}
    else 
    	{
    	// set to 0 if we got some rubbish value...
    	iPasskeyMinLength=0;
    	}

    	//check if RecommendedPinCodeMinLength is set
	if (0<iRecommendedPasskeyMinLength && iRecommendedPasskeyMinLength<=KHCIPINCodeSize)
		{
		iConsole->Printf(_L("\nRecommended Minimal PIN code length: %d"), iRecommendedPasskeyMinLength);
		}
	else
		{
		// set to 0 if we got some rubbish value...
		iRecommendedPasskeyMinLength=0;
		}
    	
    if(iInternallyInitiated)
    	{
		iConsole->Printf(_L("\nInternally initiated authentication.")); 
		}
    else
    	{
		iConsole->Printf(_L("\nRemote side initiated the authentication."));  	
    	}
    
    if(iStrongKeyRequired)
    	{
    	iConsole->Printf(_L("\n!!! Strong PIN Code Required !!!")); 
    	}
    	
	iConsole->Printf(_L("\nand press [return] to accept or [esc] to cancel\n"));		
	IssueRequestL();
	}

void CBTGetPinFromConsole::IssueRequestL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!IsActive(), User::Panic(_L("BTGetPinFromConsole"), KErrGeneral));

#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	iAutoNotifierCallback->CallBack();
	iStatus = KRequestPending;
#else //__BT_TEXT_NOTIFIERS_AUTO__
	iConsole->Read(iStatus);
#endif // __BT_TEXT_NOTIFIERS_AUTO__

	SetActive();
	}

#ifdef __BT_TEXT_NOTIFIERS_AUTO__
/*static*/ TInt CBTGetPinFromConsole::AutoNotifierCallBack(TAny *aConsolePin)
	{
	LOG_STATIC_FUNC
	CBTGetAuthFromConsole* consolePin = static_cast<CBTGetAuthFromConsole*>(aConsolePin);
	if(consolePin->IsActive())
		{
		TRequestStatus* stat = &(consolePin->iStatus);
		User::RequestComplete(stat, KErrNone);
		}
	return EFalse;
	}
#endif // __BT_TEXT_NOTIFIERS_AUTO__

void CBTGetPinFromConsole::DeviceNameRetrieved(const TPtrC aName, TInt aResult)
	{
	LOG_FUNC
	iConsole->Printf(_L("\nDevice Name Retrieved (code %d): \n"), aResult);
	iConsole->Printf(aName);
#ifdef _DEBUG
	iConsole->Printf(_L("\n"));
#endif
	}

void CBTGetPinFromConsole::CoDRetrieved(TBTDeviceClass& aCoD)
	{
	LOG_FUNC
	iConsole->Printf(_L("CoD Retrieved (MajorS MajorD MinorD):"));
	iConsole->Printf(_L("\t0x%04x 0x%02x 0x%02x\n"),
		aCoD.MajorServiceClass(),
		aCoD.MajorDeviceClass(),
		aCoD.MinorDeviceClass()
		);
	}



//
// CBTManAuthNotifier
//

void CBTManAuthNotifier::Release()
	{
	LOG_FUNC
	Cancel();
	delete this;
	}

CBTManAuthNotifier::TNotifierInfo CBTManAuthNotifier::RegisterL()
	{
	LOG_FUNC
	iInfo.iUid=KBTManAuthNotifierUid;
	iInfo.iChannel=KScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CBTManAuthNotifier::TNotifierInfo CBTManAuthNotifier::Info() const
	{
	LOG_FUNC
	return iInfo;
	}


TPtrC8 CBTManAuthNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	LOG(_L("The synchronous StartL overload is not used."))
	LEAVEL(KErrNotSupported);
	return KNullDesC8();
	}

void CBTManAuthNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	LOG_FUNC
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
	iNeedToCompleteMessage = ETrue;

	TRAPD(err, iEngine->StartAuthorisationL(aBuffer));
	if (err)
		{
		FLOG(_L("RNot\t** could not StartAuthorisation **"));
		aMessage.Complete(err);
		iNeedToCompleteMessage = EFalse;
		User::Leave(err);
		}
	else
		{
		FLOG(_L("RNot\tCBTManAuthNotifier - Started authorisation OK"));
		}
	}



CBTManAuthNotifier::~CBTManAuthNotifier()
	{
	LOG_FUNC
	delete iEngine;
	}

void CBTManAuthNotifier::Cancel()
	{
	LOG_FUNC

	iEngine->Stop();

	if (iNeedToCompleteMessage)
		{
		iMessage.Complete(KErrCancel);
		iNeedToCompleteMessage = EFalse;
		}	
	}

TPtrC8 CBTManAuthNotifier::UpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	return iEngine->UpdateL(aBuffer);
	}

void CBTManAuthNotifier::AuthorisationComplete(TBool aDecision, TInt aReason)
	{
	LOG_FUNC
	if (aReason == KErrNone)
		{
		FTRACE(FPrint(_L("RNot\tCBTManAuthNotifier::AuthorisationComplete(TBool=%d)"), aDecision));

		TInt err = iMessage.Write(iReplySlot, TPckgC<TBool>(aDecision));
		
		iMessage.Complete(err);
		}
	else
		{
		FTRACE(FPrint(_L("RNot\tCBTManAuthNotifier::AuthorisationComplete(ERROR=%d)"), aReason));
		iMessage.Complete(aReason);
		}
	iNeedToCompleteMessage = EFalse;
	}


CBTManAuthNotifier* CBTManAuthNotifier::NewLC()
	{
	LOG_STATIC_FUNC
	CBTManAuthNotifier* self=new (ELeave) CBTManAuthNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CBTManAuthNotifier::ConstructL()
	{
	LOG_FUNC
	iEngine = CBTManAuthNotifierEngine::NewL(*this);
	}

CBTManAuthNotifierEngine::CBTManAuthNotifierEngine(CBTManAuthNotifier& aParent)
: CActive(EPriorityStandard), iParent(aParent)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}


//
// CBTManAuthNotifierEngine
//

CBTManAuthNotifierEngine* CBTManAuthNotifierEngine::NewL(CBTManAuthNotifier& aParent)
	{
	LOG_STATIC_FUNC
	CBTManAuthNotifierEngine* self=new (ELeave) CBTManAuthNotifierEngine(aParent);
	return self;
	}

void CBTManAuthNotifierEngine::Stop()
	{
	LOG_FUNC
	Cancel();
	if (iGetAuthFromConsole)
		{
		FLOG(_L("RNot\tCancelling authorisation console..."))
		iGetAuthFromConsole->Cancel();
		delete iGetAuthFromConsole;
		iGetAuthFromConsole = NULL;
		}

	//clean up iDevice...
	delete iDevice;
	iDevice = NULL;
	}

void CBTManAuthNotifierEngine::DoCancel()
	{
	LOG_FUNC

	iRegistryView.CancelRequest(iStatus);
	if (iResponse)
		{
		iResponse->Cancel();
		}
	}

CBTManAuthNotifierEngine::~CBTManAuthNotifierEngine()
	{
	LOG_FUNC
	Cancel();

	delete iGetAuthFromConsole;

	iRegistryView.Close();
	iRegistry.Close();
	}

void CBTManAuthNotifierEngine::AuthorisationComplete(TBool aDecision, TInt aReason)
	{
	LOG_FUNC
	iParent.AuthorisationComplete(aDecision, aReason);
	delete iGetAuthFromConsole;
	iGetAuthFromConsole = NULL;
	delete iDevice;
	iDevice = NULL;
	}

TPtrC8 CBTManAuthNotifierEngine::UpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	if (iGetAuthFromConsole)
		{
		if (!iDevice)
			{
			iDevice = CBTDevice::NewL();
			}
		//we only want to do this if our device doesn't have a valid device name...
		if (!iDevice->IsValidDeviceName())
			{
			TBTNotifierUpdateParams params;
			TPckgC<TBTNotifierUpdateParams> pckg(params);
			pckg.Set(aBuffer);
			iGetAuthFromConsole->DeviceNameRetrieved(pckg().iName, pckg().iResult);
			iDevice->SetDeviceNameL(BTDeviceNameConverter::ToUTF8L(pckg().iName));
			}
		}
	return KNullDesC8();
	}

void CBTManAuthNotifierEngine::StartAuthorisationL(const TDesC8& aBuffer)
	{
	LOG_FUNC
//	__ASSERT_DEBUG(((iGetAuthFromConsole==NULL)&&(iDevice==NULL)&&(iDeviceFromRegistry==NULL)), User::Panic(_L("BTAuthProvider"), KErrGeneral));
	__ASSERT_DEBUG(!IsActive(), User::Panic(_L("BTAuthProvider"), KErrGeneral));

	iAuthPckg.Copy(aBuffer);

	TBTDeviceName name = iAuthPckg().iName;

	if (name.Length()==0)
		{
		// Find the device - need to get the full details for the name, so do a search
		FTRACE(FPrint(_L("RNot\tStarting authorisation...")));
		User::LeaveIfError(iRegistry.Connect());
		User::LeaveIfError(iRegistryView.Open(iRegistry));
		iRegistrySearch.FindAddress(iAuthPckg().iBDAddr);
		iRegistryView.CreateView(iRegistrySearch, iStatus);
		iRegistryState = EFinding;
		SetActive();
		}
	else
		{
		// create our dummy device (didn't really need this could just work with addresses and names!)
		delete iDevice;
		iDevice = NULL;
		iDevice = CBTDevice::NewL(iAuthPckg().iBDAddr);
		iDevice->SetDeviceNameL(BTDeviceNameConverter::ToUTF8L(name));
		
		iGetAuthFromConsole = CBTGetAuthFromConsole::NewL(this, *iDevice, iAuthPckg().iUid);
		iGetAuthFromConsole->AuthoriseL();
		}
	}

void CBTManAuthNotifierEngine::RunL()
	{
	LOG_FUNC
	if (iStatus >= KErrNone)
		{
		switch (iRegistryState)
			{
			case EFinding:
				{
				// search has completed, get the resultset
				FLOG(_L("RNot\tCBTManAuthNotifier: Found device"))
				iResponse = CBTRegistryResponse::NewL(iRegistryView);
				iResponse->Start(iStatus);
				iRegistryState = EGetting;
				SetActive();
				}
				break;
			case EGetting:
				{
				// got the details, now launch the authorisor console
				FLOG(_L("RNot\tCBTManAuthNotifier: Got device details"))
				iGetAuthFromConsole = CBTGetAuthFromConsole::NewL(this, *(iResponse->Results()[0]), iAuthPckg().iUid);
				iGetAuthFromConsole->AuthoriseL();
				delete iResponse;
				iResponse = NULL;
				}
				break;
			}
		}
	else if (iStatus == KErrNotFound)
		{
		__ASSERT_DEBUG(iRegistryState == EFinding, User::Invariant());
		FLOG(_L("RNot\tCBTManAuthNotifier: Device not in Registry (OK)"))
		// not to worry - the device isn't in the Registry
		// although strictly speaking we should only Authorise an Authenticated device
		// which means it *should* be in the Registry.
		// however we do allow authorisation of unauthenticated devices at BT level
		
		// so we don't have a name at this point, but not to worry
		CBTDevice* device = CBTDevice::NewLC(iAuthPckg().iBDAddr);
		iGetAuthFromConsole = CBTGetAuthFromConsole::NewL(this, *device, iAuthPckg().iUid);
		iGetAuthFromConsole->AuthoriseL();
		CleanupStack::PopAndDestroy(device);
		}
	else
		{
		User::Invariant();
		}
	}


//------------------------------------------------------------------------//
//Get Auth from a console window class
//------------------------------------------------------------------------//


CBTGetAuthFromConsole* CBTGetAuthFromConsole::NewL(CBTManAuthNotifierEngine* aParent, 
												   const CBTDevice& aDevice, TUid aUid)
	{
	LOG_STATIC_FUNC
	CBTGetAuthFromConsole* s = CBTGetAuthFromConsole::NewLC(aParent, aDevice, aUid);
	CleanupStack::Pop();
	return s;
	}

CBTGetAuthFromConsole* CBTGetAuthFromConsole::NewLC(CBTManAuthNotifierEngine* aParent, 
													const CBTDevice& aDevice, TUid aUid)
	{
	LOG_STATIC_FUNC
	CBTGetAuthFromConsole* s = new(ELeave) CBTGetAuthFromConsole(aParent);
	CleanupStack::PushL(s);
	s->ConstructL(aDevice, aUid);
	return s;
	}

CBTGetAuthFromConsole::~CBTGetAuthFromConsole()
	{
	LOG_FUNC
	Cancel();
	delete iConsole;
#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	delete iAutoNotifierCallback;
#endif
	}

void CBTGetAuthFromConsole::RunL()
	{
	LOG_FUNC
#ifdef __BT_TEXT_NOTIFIERS_AUTO__

	iParent->AuthorisationComplete(ETrue, KErrNone);

#else // __BT_TEXT_NOTIFIERS_AUTO__

	TKeyCode key = iConsole->KeyCode();
	if (key == 'y')
		{
		iParent->AuthorisationComplete(ETrue, KErrNone);
		}
	else if (key == 'n')
		{
		iParent->AuthorisationComplete(EFalse, KErrNone);
		}
	else
		{
		iConsole->Printf(_L("\nStop being a muppet and press 'y' or 'n'\n"));
		IssueRequest();
		}
#endif // __BT_TEXT_NOTIFIERS_AUTO__
	}

void CBTGetAuthFromConsole::DoCancel()
	{
	LOG_FUNC
#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	FLOG(_L("RNot\tCBTGetAuthFromConsole::DoCancel()"))
	iConsole->ReadCancel();
#endif
	}

CBTGetAuthFromConsole::CBTGetAuthFromConsole(CBTManAuthNotifierEngine* aParent) :
	CActive(EPriorityStandard), iParent(aParent)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CBTGetAuthFromConsole::ConstructL(const CBTDevice& aDevice, TUid aUid)
	{
	LOG_FUNC
	iConsole = BTTextNotifiersConsole::AutoSizeNewL(_L("Authorisor"), TSize(KConsFullScreen,KConsFullScreen));
	iDevice = &aDevice;
	iServiceUid = aUid;

#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	TCallBack autoNotifierCB(AutoNotifierCallBack, this);
	iAutoNotifierCallback = new (ELeave)CAsyncCallBack(autoNotifierCB, EActiveMedPriority);
#endif // __BT_TEXT_NOTIFIERS_AUTO__
	}

void CBTGetAuthFromConsole::AuthoriseL()
	{
	LOG_FUNC
	iConsole->ClearScreen();
	iConsole->Printf(_L("Can the following device:\nAddress: "));
	TBTDevAddr a = iDevice->BDAddr();
	iConsole->Printf(_L("0x%02x%02x%02x%02x%02x%02x"), a[0], a[1], a[2], a[3], a[4], a[5]);
	if (iDevice->IsValidDeviceName())
		{
		iConsole->Printf(_L("\nName: "));
		THostName dispBuf;
		dispBuf.Copy(iDevice->DeviceName());
		iConsole->Printf(dispBuf);
		}
	else
		{
		iConsole->Printf(_L("\n<No name>"));
		}
	iConsole->Printf(_L("\nuse the following service\nService Uid = 0x%08x\n[y/n]"), iServiceUid.iUid);
	IssueRequest();
	}

void CBTGetAuthFromConsole::IssueRequest()
	{
	LOG_FUNC
#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	iAutoNotifierCallback->CallBack();
	iStatus = KRequestPending;
#else //__BT_TEXT_NOTIFIERS_AUTO__
	iConsole->Read(iStatus);
#endif // __BT_TEXT_NOTIFIERS_AUTO__

	SetActive();
	}

#ifdef __BT_TEXT_NOTIFIERS_AUTO__
/*static*/ TInt CBTGetAuthFromConsole::AutoNotifierCallBack(TAny *aConsoleAuth)
	{
	LOG_STATIC_FUNC
	CBTGetAuthFromConsole* consoleAuth = static_cast<CBTGetAuthFromConsole*>(aConsoleAuth);
	if(consoleAuth->IsActive())
		{
		TRequestStatus* stat = &(consoleAuth->iStatus);
		User::RequestComplete(stat, KErrNone);
		}
	return EFalse;
	}
#endif // __BT_TEXT_NOTIFIERS_AUTO__

void CBTGetAuthFromConsole::DeviceNameRetrieved(const TDesC& aName, TInt aResult)
	{
	LOG_FUNC
	iConsole->Printf(_L("\nDevice Name Retrieved (code %d): "), aResult);
	iConsole->Printf(aName);
	iConsole->Printf(_L("\n"));
	}



//
// CBTManDeviceSelectionNotifier
//

void CBTManDeviceSelectionNotifier::Release()
	{
	LOG_FUNC
	delete this;
	}

CBTManDeviceSelectionNotifier::TNotifierInfo CBTManDeviceSelectionNotifier::RegisterL()
	{
	LOG_FUNC
	iInfo.iUid=KDeviceSelectionNotifierUid;
	iInfo.iChannel=KScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVLow;

	return iInfo;
	}

CBTManDeviceSelectionNotifier::TNotifierInfo CBTManDeviceSelectionNotifier::Info() const
	{
	LOG_FUNC
	return iInfo;
	}
	

TPtrC8 CBTManDeviceSelectionNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	LOG(_L("The synchronous StartL overload is not used."))
	LEAVEL(KErrNotSupported);
	return KNullDesC8();
	}

void CBTManDeviceSelectionNotifier::StartL(const TDesC8& /*aBuffer*/, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	LOG_FUNC

	CBTManDeviceScanner* scanner = CBTManDeviceScanner::NewL();
	CleanupStack::PushL(scanner);
	TKeyCode choice = GetResponseL(_L("Press 'm' to enter a devaddr,\nANY OTHER KEY for general inquiry \nfor discoverable devices."), KConsFullScreen, 3);
	TInt err = 0;
	TBTDeviceResponseParamsPckg pckg;

	if(choice=='m'||choice=='M')
		{
		TRAP(err, GetAddressL(iBDAddr));
		}
	else
		{
		TRAP(err, scanner->GetByScanL(iBDAddr, iName, iClass));
		}

	if(err)
		{
		DisplayErrorMessageL();
		}
	else
		{
		pckg().SetDeviceAddress(iBDAddr);
//		pckg().SetDeviceName(iName);
		pckg().SetDeviceClass(iClass);
		/*TInt err = */aMessage.Write(aReplySlot, pckg);
		}
	aMessage.Complete(err);
	CleanupStack::PopAndDestroy();
	return;
	}
	


CBTManDeviceSelectionNotifier::~CBTManDeviceSelectionNotifier()
	{
	LOG_FUNC
	}

void CBTManDeviceSelectionNotifier::Cancel()
	{
	LOG_FUNC
	}

TPtrC8 CBTManDeviceSelectionNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	LOG(_L("The UpdateL is not supported."))
	LEAVEL(KErrNotSupported);
	return KNullDesC8();
	}

CBTManDeviceSelectionNotifier* CBTManDeviceSelectionNotifier::NewLC()
	{
	LOG_STATIC_FUNC
	CBTManDeviceSelectionNotifier* self=new (ELeave) CBTManDeviceSelectionNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBTManDeviceSelectionNotifier::CBTManDeviceSelectionNotifier()
	{
	LOG_FUNC
	}

void CBTManDeviceSelectionNotifier::ConstructL()
	{
	LOG_FUNC
	}

TKeyCode CBTManDeviceSelectionNotifier::GetResponseL(TRefByValue<const TDesC> aPrompt, TInt aWidth, TInt aHeight)
	{
	LOG_FUNC
	CConsoleBase* console = BTTextNotifiersConsole::AutoSizeNewL(_L("Device Selector"), TSize(aWidth,aHeight));
	CleanupStack::PushL(console);
	console->Printf(aPrompt);
	TKeyCode ret = console->Getch();
	CleanupStack::PopAndDestroy();
	return ret;
	}


void CBTManDeviceSelectionNotifier::GetAddressL(TBTDevAddr& aBDAddr)
    {
    LOG_FUNC
	TBuf<12> addrAsText;
	addrAsText.Zero();
	if (aBDAddr != TBTDevAddr(0))
		addrAsText.Format(_L("%02x%02x%02x%02x%02x%02x"), 
			aBDAddr[0], aBDAddr[1], aBDAddr[2], aBDAddr[3], aBDAddr[4], aBDAddr[5]);
	CConsoleBase* console = BTTextNotifiersConsole::AutoSizeNewL(_L("Device Selector"), TSize(KConsFullScreen,6));
	CleanupStack::PushL(console);

	TKeyCode code;
	TBuf<1> character;
	console->Printf(_L("Enter a device address:\n\n        0x"));
	if (addrAsText.Length()>0)
		console->Printf(addrAsText);
	FOREVER
		{
		code = console->Getch();
		character.SetLength(0);
		character.Append(code);
	
		// If <CR> finish editing string
		if (code == 0x0d)
			break;
		
		// if <BS> remove last character
		if ((code == 0x08)&&(addrAsText.Length() != 0))
			{
			console->Printf(_L("%S"),&character);
			addrAsText.SetLength((addrAsText.Length()-1));
			}
		else
			{
			if (addrAsText.Length() < addrAsText.MaxLength())
				{
				console->Printf(_L("%S"),&character);
				addrAsText.Append(code);
				}
			}
		}
	//now extract the new address from the string...
	if(!(addrAsText.Length()))
		addrAsText.Append('0'); //null string causes TLex::Val to return an error
	TLex lex(addrAsText);
	TInt64 addrAsInt64 = 0;
	TInt err = lex.Val(addrAsInt64, EHex);
	User::LeaveIfError(err);
	aBDAddr = TBTDevAddr(addrAsInt64);
	CleanupStack::PopAndDestroy();//console
    }

void CBTManDeviceSelectionNotifier::GetNameL(TBTDeviceName& aName)
    {
    LOG_FUNC
	CConsoleBase* console = BTTextNotifiersConsole::AutoSizeNewL(_L("Device Selector"), TSize(KConsFullScreen,6));
	CleanupStack::PushL(console);

	TKeyCode code;
	TBuf<1> character;
	console->Printf(_L("Enter a device name:\n\n        "));
	if (aName.Length()>0)
		{
		THostName dispBuf;
		dispBuf.Copy(aName);
		console->Printf(dispBuf);
		}
	FOREVER
		{
		code = console->Getch();
		character.SetLength(0);
		character.Append(code);
	
		// If <CR> finish editing string
		if (code == 0x0d)
			break;
		
		// if <BS> remove last character
		if ((code == 0x08)&&(aName.Length() != 0))
			{
			console->Printf(_L("%S"),&character);
			aName.SetLength((aName.Length()-1));
			}
		else
			{
			if (aName.Length() < aName.MaxLength())
				{
				console->Printf(_L("%S"),&character);
				aName.Append(code);
				}
			}
		}
	CleanupStack::PopAndDestroy();//console
    }

void CBTManDeviceSelectionNotifier::DisplayErrorMessageL()
	{
	LOG_FUNC
	CConsoleBase* console = 0;
	TRAPD(err, console = BTTextNotifiersConsole::AutoSizeNewL(_L("Error!"), TSize(KConsFullScreen,4)));
	if(err)
		return;
	CleanupStack::PushL(console);

//	console->Printf(_L("Sorry! An error has occurred whilst the last\npiece of data was being entered.\n\nPress any key, and then re-enter ALL data."));
	console->Printf(_L("Sorry! An error has occurred. No new device\nselection has been recorded.\n\nPress any key."));
//	TKeyCode code;
	(void) console->Getch();
	CleanupStack::PopAndDestroy();//console
	}




//    |                                 |
//    | Secure Simple Pairing Notifiers |
//    v                                 v

#ifndef __BT_TEXT_NOTIFIERS_NO_SSP__

	
CBTUINumericComparison* CBTUINumericComparison::NewLC()
	{
	LOG_STATIC_FUNC
	CBTUINumericComparison* self=new (ELeave) CBTUINumericComparison();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CBTUINumericComparison::ConstructL()
	{
	LOG_FUNC
	iEngine = CBTUINumericComparisonEngine::NewL(*this);
	}

CBTUINumericComparison::~CBTUINumericComparison()
	{
	LOG_FUNC
	delete iEngine;
	}


void CBTUINumericComparison::Release()
	{
	LOG_FUNC
	Cancel();
	delete this;
	}

CBTUINumericComparison::TNotifierInfo CBTUINumericComparison::RegisterL()
	{
	LOG_FUNC
	iInfo.iUid=KBTNumericComparisonNotifierUid;
	iInfo.iChannel=KScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CBTUINumericComparison::TNotifierInfo CBTUINumericComparison::Info() const
	{
	LOG_FUNC
	return iInfo;
	}


void CBTUINumericComparison::Cancel()
	{
	LOG_FUNC

	iEngine->Stop();

	if (iNeedToCompleteMessage)
		{
		iMessage.Complete(KErrCancel);
		iNeedToCompleteMessage = EFalse;
		}	
	}

TPtrC8 CBTUINumericComparison::UpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	return iEngine->UpdateL(aBuffer);
	}

TPtrC8 CBTUINumericComparison::StartL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	LOG(_L("The synchronous StartL overload is not used."))
	LEAVEL(KErrNotSupported);
	return KNullDesC8();
	}

void CBTUINumericComparison::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	LOG_FUNC
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
	iNumericComparisonParamsPckg.Copy(aBuffer);
	    	
	iNeedToCompleteMessage = ETrue;

	TRAPD(err, iEngine->DoComparisonL(iNumericComparisonParamsPckg()));
	if (err)
		{
		FLOG(_L("\t** TRAPD error - DoComparisonL() **"));
		aMessage.Complete(err);
		iNeedToCompleteMessage = EFalse;
		User::Leave(err);
		}
	}

void CBTUINumericComparison::NumericComparisonComplete(TBool aDecision, TInt aReason)
	{
	LOG_FUNC
	if (aReason == KErrNone)
		{
		FTRACE(FPrint(_L("\tCBTUINumericComparison::NumericComparisonComplete(TBool=%d)"), aDecision));

		TInt err = iMessage.Write(iReplySlot, TPckgC<TBool>(aDecision));
		
		iMessage.Complete(err);
		}
	else
		{
		FTRACE(FPrint(_L("\tCBTUINumericComparison::NumericComparisonComplete(ERROR=%d)"), aReason));
		iMessage.Complete(aReason);
		}
	iNeedToCompleteMessage = EFalse;
	}	

//------------------------------------------------------------------------//
// Do the numeric comparison...
//------------------------------------------------------------------------//
CBTUINumericComparisonEngine* CBTUINumericComparisonEngine::NewL(MBTGetNCResultFromConsoleObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CBTUINumericComparisonEngine* s = CBTUINumericComparisonEngine::NewLC(aObserver);
	CleanupStack::Pop();
	return s;
	}

CBTUINumericComparisonEngine* CBTUINumericComparisonEngine::NewLC(MBTGetNCResultFromConsoleObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CBTUINumericComparisonEngine* s = new(ELeave) CBTUINumericComparisonEngine(aObserver);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

CBTUINumericComparisonEngine::CBTUINumericComparisonEngine(MBTGetNCResultFromConsoleObserver& aObserver) :
	CActive(EPriorityStandard), iObserver(aObserver), iConsole(NULL)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CBTUINumericComparisonEngine::ConstructL()
	{
	LOG_FUNC
	}

CBTUINumericComparisonEngine::~CBTUINumericComparisonEngine()
	{
	LOG_FUNC
	Cancel();
	delete iConsole;
	}

void CBTUINumericComparisonEngine::DoComparisonL(TBTNumericComparisonParams& aNumericComparisonParams)
	{
	LOG_FUNC
	if(iConsole==NULL)
		{
		iConsole = BTTextNotifiersConsole::AutoSizeNewL(_L("Numeric Comparison"), TSize(KConsFullScreen,KConsFullScreen));
		}
	iConsole->ClearScreen();
	if(aNumericComparisonParams.LocallyInitiated())
		{
		iConsole->Printf(_L("\nLocally initiated Numeric Comparision \n"));
		}
	else
		{
		iConsole->Printf(_L("\nRemote initiated the Numeric Comparision \n"));
		}		
	iConsole->Printf(_L("\nNumeric value to compare: %06d\n\n"), aNumericComparisonParams.NumericalValue());
	iConsole->Printf(_L("Numbers should be displayed on local and remote device.\n"));
	switch(aNumericComparisonParams.ComparisonScenario())
		{
		case TBTNumericComparisonParams::ERemoteCannotConfirm:
			iConsole->Printf(_L("N.B. The remote device HOWEVER cannot receive input.\n"));
			break;
		case TBTNumericComparisonParams::ERemoteCanConfirm:
			iConsole->Printf(_L("N.B. The remote device will ALSO expect input.\n"));
			break;
		default:
			__ASSERT_DEBUG(EFalse, User::Panic(_L("CBTUINumericComparisonEngine"), KErrArgument));
			break;
		}
	iConsole->Printf(_L("Please compare the numbers now.\n"));
	iConsole->Printf(_L("\n\nDo the numbers match?[y/n]"));
	IssueRequest();
	}

TPtrC8 CBTUINumericComparisonEngine::UpdateL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	// need something to update the name
	return KNullDesC8();
	}

void CBTUINumericComparisonEngine::Stop()
	{
	LOG_FUNC
	Cancel();
	delete iConsole;
	iConsole = NULL;
	}

void CBTUINumericComparisonEngine::IssueRequest()
	{
	LOG_FUNC
	iConsole->Read(iStatus);
	SetActive();
	}

void CBTUINumericComparisonEngine::RunL()
	{
	LOG_FUNC
	TKeyCode key = iConsole->KeyCode();
	if (key == 'y')
		{
		iObserver.NumericComparisonComplete(ETrue, KErrNone);
		delete iConsole;
		iConsole=NULL;
		}
	else if (key == 'n')
		{
		iObserver.NumericComparisonComplete(EFalse, KErrNone);
		delete iConsole;
		iConsole=NULL;
		}
	else
		{
		iConsole->Printf(_L("\nInvalid key pressed! Press 'y' or 'n'\n"));
		IssueRequest();
		}
	}

void CBTUINumericComparisonEngine::DoCancel()
	{
	LOG_FUNC
	if (iConsole)
		{
		iConsole->ReadCancel();
		}
	}


//
// CBTUIPasskeyEntry
//

CBTUIPasskeyEntry* CBTUIPasskeyEntry::NewLC()
	{
	LOG_STATIC_FUNC
	CBTUIPasskeyEntry* self = new (ELeave) CBTUIPasskeyEntry();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CBTUIPasskeyEntry::ConstructL()
	{
	LOG_FUNC
	iEngine = CBTUIPasskeyEntryEngine::NewL(*this);
	}

CBTUIPasskeyEntry::~CBTUIPasskeyEntry()
	{
	LOG_FUNC
	delete iEngine;
	}


void CBTUIPasskeyEntry::Release()
	{
	LOG_FUNC
	Cancel();
	delete this;
	}

CBTUIPasskeyEntry::TNotifierInfo CBTUIPasskeyEntry::RegisterL()
	{
	LOG_FUNC
	iInfo.iUid=KBTPasskeyDisplayNotifierUid;
	iInfo.iChannel=KScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CBTUIPasskeyEntry::TNotifierInfo CBTUIPasskeyEntry::Info() const
	{
	LOG_FUNC
	return iInfo;
	}


void CBTUIPasskeyEntry::Cancel()
	{
	LOG_FUNC

	iEngine->Stop();

	if (iNeedToCompleteMessage)
		{
		iMessage.Complete(KErrCancel);
		iNeedToCompleteMessage = EFalse;
		}	
	}

TPtrC8 CBTUIPasskeyEntry::UpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	return iEngine->UpdateL(aBuffer);
	}

TPtrC8 CBTUIPasskeyEntry::StartL(const TDesC8& /*aBuffer*/)
	{
	LOG_FUNC
	LOG(_L("The synchronous StartL overload is not used."))
	LEAVEL(KErrNotSupported);
	return KNullDesC8();
	}

void CBTUIPasskeyEntry::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	LOG_FUNC
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
		
	iPasskeyDisplayParamsPckg.Copy(aBuffer);
	iAddr			= iPasskeyDisplayParamsPckg().DeviceAddress();
	iName	    	= iPasskeyDisplayParamsPckg().DeviceName();
	iNumericValue	= iPasskeyDisplayParamsPckg().NumericalValue();

	iNeedToCompleteMessage = ETrue;

	TRAPD(err, iEngine->DoPasskeyL(iPasskeyDisplayParamsPckg()));
	if (err)
		{
		FLOG(_L("\t** TRAPD error - DoComparisonL() **"));
		aMessage.Complete(err);
		iNeedToCompleteMessage = EFalse;
		User::Leave(err);
		}
	}

void CBTUIPasskeyEntry::PasskeyComplete(TInt aReason)
	{
	LOG_FUNC
	LOG1(_L("\taReason = %d"), aReason)
	iMessage.Complete(aReason);
	iNeedToCompleteMessage = EFalse;
	}	


//
// CBTUIPasskeyEntryEngine
//

CBTUIPasskeyEntryEngine* CBTUIPasskeyEntryEngine::NewL(MBTGetPasskeyResultFromConsoleObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CBTUIPasskeyEntryEngine* s = CBTUIPasskeyEntryEngine::NewLC(aObserver);
	CleanupStack::Pop();
	return s;
	}

CBTUIPasskeyEntryEngine* CBTUIPasskeyEntryEngine::NewLC(MBTGetPasskeyResultFromConsoleObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CBTUIPasskeyEntryEngine* s = new(ELeave) CBTUIPasskeyEntryEngine(aObserver);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

CBTUIPasskeyEntryEngine::CBTUIPasskeyEntryEngine(MBTGetPasskeyResultFromConsoleObserver& aObserver) :
	CActive(EPriorityStandard), iObserver(aObserver), iConsole(NULL)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CBTUIPasskeyEntryEngine::ConstructL()
	{
	LOG_FUNC
	}

CBTUIPasskeyEntryEngine::~CBTUIPasskeyEntryEngine()
	{
	LOG_FUNC
	Cancel();
	delete iConsole;
	}

void CBTUIPasskeyEntryEngine::DoPasskeyL(TBTPasskeyDisplayParams& aPasskeyParams)
	{
	LOG_FUNC
	if(iConsole==NULL)
		{
		iConsole = BTTextNotifiersConsole::AutoSizeNewL(_L("Numeric Comparison"), TSize(KConsFullScreen,KConsFullScreen));
		}
	
	if(aPasskeyParams.LocallyInitiated())
		{
		iConsole->Printf(_L("\nLocally initiated Authentication \n"));
		}
	else
		{
		iConsole->Printf(_L("\nRemote initiated the Authentication \n"));
		}		
	iConsole->ClearScreen();
	iConsole->Printf(_L("\nEnter the following number on the remote device: %06d\n\n"), aPasskeyParams.NumericalValue());
	iConsole->Printf(_L("Press [ESC] to cancel.\n"));
	IssueRequest();
	}

TPtrC8 CBTUIPasskeyEntryEngine::UpdateL(const TDesC8& aBuffer)
	{
	LOG_FUNC
	
	TBTNotifierUpdateParamsPckg2 pckgRaw;
	pckgRaw.Copy(aBuffer.Left(pckgRaw.MaxLength()));
	
	if (iConsole)
		{
		if (pckgRaw().Type() == TBTNotifierUpdateParams2::EPasskeyDisplay)
			{
			TBTPasskeyDisplayUpdateParamsPckg pckg;
			pckg.Copy(aBuffer);
			THCIPasskeyEntryNotificationType keypressNotification = pckg().KeypressNotification();
			switch (keypressNotification)
				{
				case EPasskeyEntryStarted:
					{
					break;
					}
				case EPasskeyDigitEntered:
					{
					iConsole->Write(_L("*"));
					break;
					}
				case EPasskeyDigitDeleted:
					{
					TInt xPos = iConsole->WhereX(); 
					if (xPos > 0)
						{
						iConsole->SetPos(xPos - 1);
						}
					iConsole->ClearToEndOfLine();
					break;
					}
				case EPasskeyCleared:
					{
					iConsole->SetPos(0);
					iConsole->ClearToEndOfLine();
					break;
					}
				case EPasskeyEntryCompleted:
					{
					Cancel();
					iObserver.PasskeyComplete(KErrNone);
					delete iConsole;
					iConsole=NULL;
					break;
					}
				}
			}
		else if(pckgRaw().Type() == TBTNotifierUpdateParams2::EDeviceName)
			{
			// handle the name update
			}
		}
	return KNullDesC8();
	}

void CBTUIPasskeyEntryEngine::Stop()
	{
	LOG_FUNC
	Cancel();
	delete iConsole;
	iConsole=NULL;
	}

void CBTUIPasskeyEntryEngine::IssueRequest()
	{
	LOG_FUNC
	iConsole->Read(iStatus);
	SetActive();
	}

void CBTUIPasskeyEntryEngine::RunL()
	{
	LOG_FUNC
	TKeyCode key = iConsole->KeyCode();
	if (key == EKeyEscape)
		{
		iObserver.PasskeyComplete(KErrCancel);
		delete iConsole;
		iConsole=NULL;
		}
	else
		{
		IssueRequest();
		}
	}

void CBTUIPasskeyEntryEngine::DoCancel()
	{
	LOG_FUNC
	if (iConsole)
		{
		iConsole->ReadCancel();
		}
	}


#endif // __BT_TEXT_NOTIFIERS_NO_SSP__
