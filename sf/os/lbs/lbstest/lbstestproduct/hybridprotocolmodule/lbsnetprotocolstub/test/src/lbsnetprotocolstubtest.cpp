// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file lbsnetprotocolstubtest.cpp
*/

#include "lbsnetprotocolstubtest.h"

//-------------------------------------------------------------------------------------------------

CTResponseChannel::CTResponseChannel(CConsoleBase* aConsole, CLbsNetworkProtocolBase* aNetProtocol)
	: CActive(EPriorityStandard), iConsole(aConsole), iProtocol(aNetProtocol), iKeyCodes(), iNextResponse(ENetMsgRespondPrivacyRequest)
	{
	CActiveScheduler::Add(this);
	}

CTResponseChannel::~CTResponseChannel()
	{
	Cancel();
	iKeyCodes.Close();
	}

void CTResponseChannel::ConstructL()
	{
	}

void CTResponseChannel::StartFetch()
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(_L("Basic"), KErrInUse));
	iConsole->Read(iStatus);
	SetActive();
	}

void CTResponseChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	iConsole->Printf(_L("Start of CTResponseChannel::RunL()\n"));
	HandleKeyPressL(iConsole->KeyCode());

	//RespondPrivacyRequest
	TLbsNetSessionId sessionId(TUid::Uid(0x88888888), 0x2020);
	CLbsNetworkProtocolBase::TLbsPrivacyResponse response = CLbsNetworkProtocolBase::EPrivacyResponseAccepted;

	//RespondLocationRequest
	TInt reason = KErrNotFound;

	TCoordinate coordinate(3.14159265359, 2.71828183);
	TLocality locality(coordinate, 99.99);
	TTime time(101010101);
	TPosition position(locality, time);
	TPositionInfo positionInfo;
	positionInfo.SetPosition(position);
	positionInfo.SetModuleId(TUid::Uid(0x0118));
	positionInfo.SetPositionMode(0x04);
	positionInfo.SetPositionModeReason(KErrNotFound);

	//RequestTransmitLocation
	const TInt  KX3pPriorityPush = 0x02;

	//RequestAssistanceData
	TLbsAssistanceDataItem dataRequestMask = EAssistanceDataIonosphericModel;

	//RequestNetworkLocation
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0x1010);

	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	
	TLbsNetPosRequestQuality quality1;
	quality1.SetMaxFixTime(424242);
	quality1.SetMaxFixAge(252525);
	quality1.SetMinHorizontalAccuracy(118.118);
	quality1.SetMinVerticalAccuracy(118.247);
	options1.SetRequestQuality(quality1);
	
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataBadSatList;
	options1.SetDataRequestMask(dataRequestMask1);
	options1.SetPosMode(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);

	//CancelNetworkLocation
	//RequestSelfLocation
	//CancelSelfLocation

	//AdviceSystemStatus
	CLbsNetworkProtocolBase::TLbsSystemStatus status = CLbsNetworkProtocolBase::ESystemStatusClientTracking;
	_LIT(KDestination,"x3pstep1@there.com");

	iConsole->Printf(_L("Test Response %d\n"), iNextResponse);
	switch (iNextResponse++)
		{
		case ENetMsgRespondPrivacyRequest :
			iProtocol->RespondPrivacyRequest(sessionId, response);
			break;
		case ENetMsgRespondLocationRequest :
			iProtocol->RespondLocationRequest(sessionId, reason, positionInfo);
			break;
		case ENetMsgRequestTransmitLocation :
			iProtocol->RequestTransmitLocation(sessionId, KDestination, KX3pPriorityPush);
			break;
		case ENetMsgCancelTransmitLocation :
			iProtocol->CancelTransmitLocation(sessionId, reason);
			break;
		case ENetMsgRequestAssistanceData :
			iProtocol->RequestAssistanceData(dataRequestMask);
			break;
		case ENetMsgRequestSelfLocation :
			iProtocol->RequestSelfLocation(sessionId1, options1);
			break;
		case ENetMsgCancelSelfLocation :
			iProtocol->CancelSelfLocation(sessionId1, reason);
			break;
		case ENetMsgRequestNetworkLocation :
			iProtocol->RequestNetworkLocation(sessionId1, options1);
			break;
		case ENetMsgCancelNetworkLocation :
			iProtocol->CancelNetworkLocation(sessionId1, reason);
			break;
		case ENetMsgGetCurrentCapabilitiesResponse :	//assumed to map onto 	ENetMsgAdviceSystemStatus
			iProtocol->AdviceSystemStatus(status);
			iNextResponse = ENetMsgRespondPrivacyRequest;
			break;
		default :
			iNextResponse = ENetMsgRespondPrivacyRequest;
			break;	
		}

	iConsole->Printf(_L("End of CTResponseChannel::RunL()\n"));
	}

void CTResponseChannel::DoCancel()
	{
	iConsole->ReadCancel();
	}

void CTResponseChannel::DisplayKeyPresses()
	{
	iConsole->Printf(_L("Displaying last 100 keypresses\n"));
	for (TInt index = 0; index < iKeyCodes.Count(); index++)
		{
		iConsole->Printf(_L("keypress %d was %d\n"), index, iKeyCodes[index]);
		}
	}

void CTResponseChannel::HandleKeyPressL(TKeyCode aCode)
	{
	iLastCode = aCode;
	iKeyCodes.Append(iLastCode);
	if (iKeyCodes.Count() > 100)
	{
		iKeyCodes.Remove(0);
	}
	if (49 >= aCode && aCode <=57)
		{
		TInt failureRate = aCode - 48;
		__UHEAP_SETFAIL(RAllocator::EFailNext,failureRate);
		}
	switch(aCode)
		{
		case 17: CActiveScheduler::Stop(); return;
			break;
		case 16: DisplayKeyPresses();
			break;
		case 48: __UHEAP_RESET;
			break;
		default:
			{
//			iConsole->Printf(_L("lastKey = %d\n"), iLastCode);
			break;
			}
		}
	StartFetch();
	}

CTResponseChannel* CTResponseChannel::NewL(CConsoleBase* aConsole, CLbsNetworkProtocolBase* aNetProtocol)
	{
	CTResponseChannel* self = new (ELeave) CTResponseChannel(aConsole, aNetProtocol);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


//-------------------------------------------------------------------------------------------------

CNetworkProtocolObserverStub* CNetworkProtocolObserverStub::NewL()
	{
	CNetworkProtocolObserverStub* self = new (ELeave) CNetworkProtocolObserverStub();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CNetworkProtocolObserverStub::ConstructL()
	{
	
	}
CNetworkProtocolObserverStub::CNetworkProtocolObserverStub()
	{
	
	}

void CNetworkProtocolObserverStub::GetCurrentCapabilities(
							TLbsNetPosCapabilities& aCapabilities) const
	{
	LBSLOG(ELogP2, "CNetworkProtocolObserverStub::GetCurrentCapabilities");	
	}

void CNetworkProtocolObserverStub::ProcessStatusUpdate(
							TLbsNetProtocolServiceMask aActiveServiceMask)
	{
	LBSLOG(ELogP2, "CNetworkProtocolObserverStub::ProcessStatusUpdate");
	}

void CNetworkProtocolObserverStub::ProcessPrivacyRequest(
							const TLbsNetSessionId& aSessionId,
							TBool aEmergency, 
							const TLbsNetPosRequestPrivacy& aPrivacy,
							const TLbsExternalRequestInfo& aRequestInfo)
	{
	LBSLOG(ELogP2, "CNetworkProtocolObserverStub::ProcessPrivacyRequest");
	}

void CNetworkProtocolObserverStub::ProcessLocationRequest(
							const TLbsNetSessionId& aSessionId,
							TBool aEmergency,
							TLbsNetProtocolService aService, 
							const TLbsNetPosRequestQuality& aQuality,
							const TLbsNetPosRequestMethod& aMethod)
	{
	LBSLOG(ELogP2, "CNetworkProtocolObserverStub::ProcessLocationRequest");	
	}

void CNetworkProtocolObserverStub::ProcessSessionComplete(
							const TLbsNetSessionId& aSessionId,
							TInt  aReason)
	{
	LBSLOG(ELogP2, "CNetworkProtocolObserverStub::ProcessSessionComplete");		
	}
	
void CNetworkProtocolObserverStub::ProcessAssistanceData(
							TLbsAsistanceDataGroup aDataMask,
							const RLbsAssistanceDataBuilderSet& aData,
							TInt aReason)
	{
	LBSLOG(ELogP2, "CNetworkProtocolObserverStub::ProcessAssistanceData");

	RLbsAssistanceDataBuilderSet& data = const_cast<RLbsAssistanceDataBuilderSet&>(aData);
	RUEPositioningGpsReferenceTimeBuilder* refTimeBuilder = NULL;
	
	data.GetDataBuilder(refTimeBuilder);

	// Create a reader from the builder's data to allow us to verify the actual
	// assistance data.
	RUEPositioningGpsReferenceTimeReader refTimeReader;
		
	TRAPD(err, refTimeReader.OpenL());
	if (err == KErrNone)
		{
		refTimeReader.DataBuffer() = refTimeBuilder->DataBuffer();
		
		TUint week;
		TUint tow1MSec;	                                            

		// Access the reference time values.
		refTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsWeek, week);
		refTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, tow1MSec);


		refTimeReader.Close();
		}
			
	}

void CNetworkProtocolObserverStub::ProcessLocationUpdate(
							const TLbsNetSessionId& aSessionId,
							const TPositionInfoBase& aPosInfo)
	{
	LBSLOG(ELogP2, "CNetworkProtocolObserverStub::ProcessLocationUpdate");
	const TPositionInfo& info = static_cast<const TPositionInfo&>(aPosInfo);		
	}

//-------------------------------------------------------------------------------------------------

LOCAL_D CConsoleBase* console;

_LIT(KConsoleTitle, "lbsnetprotocolstubtest");

LOCAL_C void startTestL()
	{
	console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
    console->Printf(_L(">>>>>>>>>>>>>>>>>LbsNetProtocolStubTest Start >>>>>>>>>>>>>>>>>\n"));
    console->Printf(_L("Press CTRL-Q to terminate test\n"));
    console->Printf(_L("Press any key to trigger Response Channel\n"));

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	LBSLOG_BEGIN();
	LBSNEWLOG(">>>>>>>>>>>>>>>>> LbsNetProtocolStub test code started >>>>>>>>>>>>>>>>>");
	
	MLbsNetworkProtocolObserver* observer = CNetworkProtocolObserverStub::NewL();
	CleanupStack::PushL(observer);	
	TLbsNetProtocolModuleParams params(*observer);
	
	CNetProtocolStub* stub = CNetProtocolStub::NewL(&params);
	CleanupStack::PushL(stub);
	
	CTResponseChannel* main = CTResponseChannel::NewL(console, stub);
	main->StartFetch();
	CleanupStack::PushL(main);
	CActiveScheduler::Start();

	console->Printf(_L(">>>>>>>>>>>>>>>>>LbsNetProtocolStubTest End >>>>>>>>>>>>>>>>>\n"));
    console->Printf(_L("Press any key terminate\n"));
	console->Getch();
	CleanupStack::PopAndDestroy(5, console);
	}


GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,startTestL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(_L("lbsnetprotocolstubtest"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }

