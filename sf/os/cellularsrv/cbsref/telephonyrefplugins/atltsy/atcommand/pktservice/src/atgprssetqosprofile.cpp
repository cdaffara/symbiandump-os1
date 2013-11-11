// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements the CATGprsSetProfile class and the CATGprsGetProfile.
// This command allows the TE to specify a Quality of Service Profile that is 
// used when the MT sends an Activate PDP Context Request message to the network.
// The set command specifies a profile for the context identified by the local context
// identification parameter, <cid>. Since this is the same parameter that is used in the
// +CGDCONT command, the +CGQREQ command is effectively an extension to the
// +CGDCONT command. The QoS profile consists of a number of parameters, each of
// which may be set to a separate value.
// A special form of the set command, +CGQREQ= <cid> causes the requested profile for
// context number <cid> to become undefined.
// The read command returns the current settings for each defined context.
// The test command returns values supported as a compound value. If the MT supports
// several PDP types, the parameter value ranges for each PDP type are returned on a
// separate line.
// Defined values
// <cid>: a numeric parameter which specifies a particular PDP context definition.
// <precedence>: a numeric parameter which specifies the precedence class
// <delay>: a numeric parameter which specifies the delay class
// <reliability>: a numeric parameter which specifies the reliability class
// <peak>: a numeric parameter which specifies the peak throughput class
// <mean>: a numeric parameter which specifies the mean throughput class
// If a value is omitted for a particular class then the value is considered to be 
// unspecified.
// AT+CGQREQ=[<cid> [,<precedence >[,<delay> [,<reliability.> [,<peak>[,<mean>]]]]]]
// other items were commented in a header

#include <pcktcs.h>
#include "atgprsqosprofile.h"
#include <etelpckt.h>
#include "mslogger.h"

_LIT8(KCGQreq, "AT+CGQREQ=%d,%d,%d,%d,%d,%d\r");

// CATGprsSetProfileReq::NewL()
CATGprsSetProfileReq* CATGprsSetProfileReq::NewL(CGlobalPhonemanager& aGloblePhone, 
                                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsSetProfileReq* self = new(ELeave) CATGprsSetProfileReq(aGloblePhone,aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGprsSetProfileReq::CATGprsSetProfileReq()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsSetProfileReq::CATGprsSetProfileReq(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGprsSetProfileReq::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsSetProfileReq::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CATGprsSetProfileReq::~CATGprsSetProfileReq()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsSetProfileReq::~CATGprsSetProfileReq()
	{
	}

// ---------------------------------------------------------------------------
// CATGprsSetProfileReq::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsSetProfileReq::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsSetProfileReq::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsSetProfileReq::ExecuteCommand()
	{
	MakeupCGQREQ();
	Write();
	}

// ---------------------------------------------------------------------------
// CATGprsSetProfileReq::SetContext()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsSetProfileReq::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	TInt count = iPhoneGlobals.iContextList.Count();
	TInt i;
	//Get the context from context list
	for(i = 0; i < count;i++)
		{
		if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
			{
			break;
			}
		}
	iQoSConfig = iPhoneGlobals.iContextList[i]->iQoSConfig;
	}

// ---------------------------------------------------------------------------
// CATGprsSetProfileReq::ParseResponseL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsSetProfileReq::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) == 0)
		{
		iError = KErrNone;
		}
	else
		{
		iError = KErrGeneral;
		}
	}

// ---------------------------------------------------------------------------
// CATGprsSetProfileReq::EventSignal()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsSetProfileReq::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if(KErrNone ==aStatus)
		{
		if(aEventSource == EReadCompletion)
			{
			aStatus = iError;
			}
		else
			return;
		}
	TInt count = iPhoneGlobals.iContextList.Count();
	TInt i;
	//Get the context from context list
	for(i = 0; i < count;i++)
		{
		if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
			{
			break;
			}
		}

	CAtCommandBase::Complete();
	if(KErrNone ==aStatus)
		{
		RPacketQoS::TQoSGPRSNegotiated QoSGPRSNegotiated;
		QoSGPRSNegotiated.iDelay = iPhoneGlobals.iContextList[i]->iQoSConfig.iReqDelay;
		QoSGPRSNegotiated.iMeanThroughput = iPhoneGlobals.iContextList[i]->iQoSConfig.iReqMeanThroughput;
		QoSGPRSNegotiated.iPeakThroughput = iPhoneGlobals.iContextList[i]->iQoSConfig.iReqPeakThroughput;
		QoSGPRSNegotiated.iPrecedence = iPhoneGlobals.iContextList[i]->iQoSConfig.iReqPrecedence;
		QoSGPRSNegotiated.iReliability = iPhoneGlobals.iContextList[i]->iQoSConfig.iReqReliability;
		iCtsyDispatcherCallback.CallbackPacketServicesNotifyQosProfileChangedGPRSInd(aStatus,
				                iPhoneGlobals.iContextList[i]->iInfo.iName,
				                QoSGPRSNegotiated);
		}
	iCtsyDispatcherCallback.CallbackPacketServicesSetPdpContextQosComp(aStatus,
						    iPhoneGlobals.iContextList[i]->iInfo.iName);
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

/**
 * This Function creates the at set string for the AT+CGQREQ command.
 */
void CATGprsSetProfileReq::MakeupCGQREQ()
	{
	TInt precedence, delay, reliability, peakthru, meanthru = 0;

	switch (iQoSConfig.iReqPrecedence)
		{
		case RPacketQoS::EPriorityHighPrecedence:
			precedence = 1; // high precedence
		break;

		case RPacketQoS::EPriorityMediumPrecedence:
			precedence = 2; // normal precedence
		break;
		
		case RPacketQoS::EPriorityLowPrecedence:
			precedence = 3; // low precedence
		break;
		
		default:
			precedence = 0; // unspecified or unknown precedence
		break;
		};

	switch (iQoSConfig.iReqDelay)
		{
		case RPacketQoS::EDelayClass1:
			delay = 1;
		break;

		case RPacketQoS::EDelayClass2:
			delay = 2;
		break;

		case RPacketQoS::EDelayClass3:
			delay = 3;
		break;

		case RPacketQoS::EDelayClass4:
			delay = 4;
		break;

		default:
			delay = 0;
		break;
		};

	switch (iQoSConfig.iReqReliability)
		{
		case RPacketQoS::EReliabilityClass1:
			reliability = 1;
		break;

		case RPacketQoS::EReliabilityClass2:
			reliability = 2;
		break;

		case RPacketQoS::EReliabilityClass3:
			reliability = 3;
		break;

		case RPacketQoS::EReliabilityClass4:
			reliability = 4;
		break;

		case RPacketQoS::EReliabilityClass5:
			reliability = 5;
		break;

		default:
			reliability = 0; // unspecified or unknown reliability
		break;
		};

	switch (iQoSConfig.iReqPeakThroughput)
		{
		case RPacketQoS::EPeakThroughput1000:
			peakthru = 1;
		break;

		case RPacketQoS::EPeakThroughput2000:
			peakthru = 2;
		break;

		case RPacketQoS::EPeakThroughput4000:
			peakthru = 3;
		break;

		case RPacketQoS::EPeakThroughput8000:
			peakthru = 4;
		break;

		case RPacketQoS::EPeakThroughput16000:
			peakthru = 5;
		break;

		case RPacketQoS::EPeakThroughput32000:
			peakthru = 6;
		break;

		case RPacketQoS::EPeakThroughput64000:
			peakthru = 7;
		break;

		case RPacketQoS::EPeakThroughput128000:
			peakthru = 8;
		break;

		case RPacketQoS::EPeakThroughput256000:
			peakthru = 9;
		break;
		
		default:
			peakthru = 0; // unspecified or unknown peak throughput
		break;
		};

	switch (iQoSConfig.iReqMeanThroughput)
		{
		case RPacketQoS::EMeanThroughput100:
			meanthru = 2;
		break;

		case RPacketQoS::EMeanThroughput200:
			meanthru = 3;
		break;

		case RPacketQoS::EMeanThroughput500:
			meanthru = 4;
		break;

		case RPacketQoS::EMeanThroughput1000:
			meanthru = 5;
		break;

		case RPacketQoS::EMeanThroughput2000:
			meanthru = 6;
		break;

		case RPacketQoS::EMeanThroughput5000:
			meanthru = 7;
		break;

		case RPacketQoS::EMeanThroughput10000:
			meanthru = 8;
		break;

		case RPacketQoS::EMeanThroughput20000:
			meanthru = 9;
		break;
		
		case RPacketQoS::EMeanThroughput50000:
			meanthru = 10;
		break;

		case RPacketQoS::EMeanThroughput100000:
			meanthru = 11;
		break;

		case RPacketQoS::EMeanThroughput200000:
			meanthru = 12;
		break;
		case RPacketQoS::EMeanThroughput500000:
			meanthru = 13;
		break;

		case RPacketQoS::EMeanThroughput1000000:
			meanthru = 14;
		break;

		case RPacketQoS::EMeanThroughput2000000:
			meanthru = 15;
		break;

		case RPacketQoS::EMeanThroughput5000000:
			meanthru = 16;
		break;

		case RPacketQoS::EMeanThroughput10000000:
			meanthru = 17;
		break;

		case RPacketQoS::EMeanThroughput20000000:
			meanthru = 18;
		break;

		case RPacketQoS::EUnspecifiedMeanThroughput:
		case RPacketQoS::EMeanThroughput50000000:
			meanthru = 31;
		break;

		default:
			meanthru = 0; // unspecified or unknown mean throughput
		break;
		};

	iTxBuffer.Format(KCGQreq,iContextId,			
			precedence,
			delay,
			reliability,
			peakthru,
			meanthru);
	}

// End of file
