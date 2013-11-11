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

#include <c32comm.h>

#include "EtelMngr.h"
#include "RpsSFlogger.h"
#include "rpsasciirqstdefs.h"
#include <e32debug.h>

_LIT(KTSYNAME_Temp, "TRPAT");  			// TODO - get from ini files
//_LIT(KTSYNAME_Temp, "TRPSTUBBED");  	// TODO - get from ini files
//_LIT(KPHONENAME_Temp, "GprsPhone");  	// TODO - get from ini files
	
	
CEtelMngr* CEtelMngr::NewL()
	{
	CEtelMngr* self = new(ELeave)CEtelMngr();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
//	
// Create Etel server connection and initialise telephony components
//
void CEtelMngr::ConstructL()
	{
	
   	LOGLINE1(_L("Initialising Etel...."));
   		
	// Connect to RTelServer
	TInt ret = iEtel.Connect();
	User::LeaveIfError(ret);
	iEtel.ShareAuto();
	
	// Load required TSY
	ret = iEtel.LoadPhoneModule(KTSYNAME_Temp);
	User::LeaveIfError(ret);
	
	// Check there are supported phones by TSY
	TInt noOfPhones(0);
	ret = iEtel.EnumeratePhones(noOfPhones);
	User::LeaveIfError(ret);
	if(noOfPhones == 0)
		User::Leave(KErrNotSupported);

	// Get phone's name
	RTelServer::TPhoneInfo phoneInfo;
	ret = iEtel.GetPhoneInfo(0, phoneInfo);
	User::LeaveIfError(ret);
	TBuf<25> name;
	name.Copy(phoneInfo.iName);
	
	// Establish subsession with a phone (GprsPhone)
	ret = iPhone.Open(iEtel, name);
	User::LeaveIfError(ret);
		
	// Initialise the phone
	ret = iPhone.Initialise();
	User::LeaveIfError(ret);
	
	// Get line info for the phone
	TInt linesCnt;
	iPhone.EnumerateLines(linesCnt);
	
	RPhone::TLineInfo info;
	for(TInt i = 0; i < linesCnt; i++)
		{
		iPhone.GetLineInfo(i, info);
		if(info.iLineCapsFlags & RLine::KCapsData)
			{
			iDataLineName = info.iName;
			}
		else if(info.iLineCapsFlags & RLine::KCapsVoice)
			{
			if(iVoiceLine1Name.Length()<=0)
				iVoiceLine1Name = info.iName;	//voice line 1
			else
				iVoiceLine2Name = info.iName; 	//voice line 2
			}
		else if(info.iLineCapsFlags & RLine::KCapsFax)	//this does not seem to work as there is a defect in TRP that means the KCapsFax bit is not set!
			{
			iFaxLineName = info.iName;
			}
		}
		
	// Open subsessions with the 2 lines & voice calls
	ret = iVoiceLine1.Open(iPhone, iVoiceLine1Name);
	User::LeaveIfError(ret);
	ret = iVoiceLine2.Open(iPhone, iVoiceLine2Name);
	User::LeaveIfError(ret);
	
	ret = iVoiceCall1.OpenNewCall(iVoiceLine1);
	User::LeaveIfError(ret);
	ret = iVoiceCall2.OpenNewCall(iVoiceLine2);
	User::LeaveIfError(ret);
		
	// Forces network registration
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneSubscriberId id;
	iPhone.GetSubscriberId(reqStatus, id);
	User::WaitForAnyRequest();

	iOwnNumber.RetrieveOwnNumbersL(iPhone);
	User::LeaveIfError(ret);
	
	// Open a data line and call after we've established we have data capability
	if(iDataLineName.Length() > 0)
		{
		ret = iDataLine.Open(iPhone, iDataLineName);
		User::LeaveIfError(ret);
		ret = iDataCall.OpenNewCall(iDataLine);
		User::LeaveIfError(ret);
		}
	
	LOGLINE1(_L("Etel Initialisation complete"));
	}
	
CEtelMngr::~CEtelMngr()
	{
	// Close open lines & calls, phone & etel server
	iVoiceCall1.Close();
	iVoiceCall2.Close();
	iVoiceLine1.Close();
	iVoiceLine2.Close();
	iDataCall.Close();
	iDataLine.Close();
	iPhone.Close();
	iEtel.Close();
	}
	

TInt CEtelMngr::CheckCallCapabilities(RMobileCall& aCall, const RMobileCall::TMobileCallControlCaps aExpected)
	{
	RMobileCall::TMobileCallCapsV1 caps;
	RMobileCall::TMobileCallCapsV1Pckg capsPack(caps);
	if(aCall.GetMobileCallCaps(capsPack) == KErrNone)
		if (caps.iCallControlCaps & aExpected)
			return KErrNone;
	return KErrGeneral;
	}

RMobileCall& CEtelMngr::Call(TEtelLine aLine)
	{
	switch(aLine)
		{
	case EDataLine:
		return iDataCall;
	case EVoiceLine:
		return iVoiceCall1;
	case EFaxLine:
	default:
		__ASSERT_ALWAYS(0, User::Panic(KPanicMsgFormat, KUnsupportedCall));
		return iVoiceCall1; //unreachable	
		}
	}
	
RMobileLine& CEtelMngr::Line(TEtelLine aLine)
	{
	switch(aLine)
		{
	case EDataLine:
		return iDataLine;
	case EVoiceLine:
		return iVoiceLine1;
	case EFaxLine:
	default:
		__ASSERT_ALWAYS(0, User::Panic(KPanicMsgFormat, KUnsupportedCall));
		return iVoiceLine1; //unreachable	
		}
	}


// Sends data to the data-port on the Data Line.
// It is assumed the Data Line is already connected by the MO (ie we dont need to dial)
// and that a data call is in progress
TInt CEtelMngr::SendDataL(const TDesC8& aDataToSend)
	{
	LOGLINE1(_L(">>CEtelMngr::SendDataL"));
	if(!DataSupported())
		{
		LOGLINE1(_L("<<CEtelMngr::SendDataL [KErrNotSupported] 1"));
		return KErrNotSupported;
		}
		
	// Check data call is in progress
	TInt ret;
	RMobileCall::TMobileCallStatus callSt;
	ret = iDataCall.GetMobileCallStatus(callSt);
	if(ret != KErrNone)
		{
		LOGLINE2(_L("<<CEtelMngr::SendDataL [%d] 2"),ret);
		return ret;
		}
		
	if(callSt != RMobileCall::EStatusConnected )
		{
		LOGLINE1(_L("<<CEtelMngr::SendDataL [KErrDisconnected] 3"));
		return KErrDisconnected;
		}
		
	// Connect to data port
	RCall::TCommPort commPort;
	ret = iDataCall.LoanDataPort(commPort);
	if(ret != KErrNone)
		{
		LOGLINE2(_L("<<CEtelMngr::SendDataL [%d] 4"),ret);
		return ret;
		}
		
	// Use the data port
	RCommServ serv;
	ret = serv.Connect();
	if(ret != KErrNone)
		{
		LOGLINE2(_L("<<CEtelMngr::SendDataL [%d] 5"),ret);
		return ret;
		}
	CleanupClosePushL(serv);

	/*
	// This was copied from the test-step but seems not to be necessary
	ret = serv.LoadCommModule(commPort.iPort);
	if(ret != KErrNone)
		{
		CleanupStack::PopAndDestroy();  //close serv
		return ret;
		}
	*/
	
	//if port name is new format i.e. <csyname>::<portname> then strip off csy name
	_LIT(KDoubleColon, "::");
	
	TName portname;
	TInt len1 = commPort.iPort.Find(KDoubleColon);
	if (len1 < KErrNone)
		{
		portname = commPort.iPort;
		}
	else
		{
		TUint numPos = len1 + KDoubleColon.iTypeLength;	
		TInt len2 = (commPort.iPort.Mid(numPos)).Find(KDoubleColon);
		
		if (len2 < KErrNone)
			{
			portname = commPort.iPort;
			}
		else
			{
			portname = commPort.iPort.Mid(numPos);
			}
		}
		
	RComm port;
	ret = port.Open(serv, portname, ECommShared);
	if(ret != KErrNone)
		{
		LOGLINE2(_L("<<CEtelMngr::SendDataL [%d] 6"),ret);
		CleanupStack::PopAndDestroy();  //close serv
		return ret;
		}
	CleanupClosePushL(port);

	// Skip the 'CONNECT 9600' stuff and any disconnection stuff sitting 
	// on the port from a previous data call.
	ret = port.ResetBuffers(); // Reset Tx and Rx buffers.
	if(ret != KErrNone)		   // Check with Neil
		{
		LOGLINE2(_L("<<CEtelMngr::SendDataL [%d] 7"),ret);
		CleanupStack::PopAndDestroy();  //close port
		CleanupStack::PopAndDestroy();  //close serv
		return ret;
		}
		
	// Write the data to the port
	TRequestStatus commWriteReqStatus;
	port.Write(commWriteReqStatus, aDataToSend);
	User::WaitForRequest(commWriteReqStatus);
	if(commWriteReqStatus.Int() != KErrNone)
		{
		LOGLINE2(_L("<<CEtelMngr::SendDataL [%d] 8"),ret);
		CleanupStack::PopAndDestroy();  //close port
		CleanupStack::PopAndDestroy();  //close serv
		return commWriteReqStatus.Int();
		}
	
	User::After(1000000);
	
	CleanupStack::PopAndDestroy();  //close port
	CleanupStack::PopAndDestroy();  //close serv
	LOGLINE1(_L("<<CEtelMngr::SendDataL"));
	return KErrNone;
	}



