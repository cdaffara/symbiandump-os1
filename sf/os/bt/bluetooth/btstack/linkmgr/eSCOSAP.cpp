// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the eSCO SAP class
// 
//

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include <bluetooth/hci/hciframe.h>

#include "SCOSAP.h"
#include "physicallinksmanager.h"
#include "physicallinks.h"
#include "hcifacade.h"
#include "linkutil.h"
#include "linkmuxer.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

// ----------------------------------------------------------------------------
// eSCO link
// ----------------------------------------------------------------------------
CeSCOLink* CeSCOLink::NewLC(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection)
	{
	CeSCOLink* s = new(ELeave) CeSCOLink(aConnectionMan, aConnection);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}


CeSCOLink* CeSCOLink::NewL(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aPhysicalLink)
	{
	CeSCOLink* s = NewLC(aLinksMan, aPhysicalLink);
	CleanupStack::Pop();
	return s;
	}

CeSCOLink::CeSCOLink(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aPhysicalLink)
: CBTSynchronousLink(aLinksMan, aPhysicalLink, EeSCOLink)
	{
	iUserPacketTypes = TBTSyncPackets::ESyncAnyESCOPacket;
	}

void CeSCOLink::ConstructL()
	{
	CBTSynchronousLink::ConstructL();
	}

CeSCOLink::~CeSCOLink()
	{
	}

TInt CeSCOLink::GetOption(TUint aLevel, TUint aName, TDes8& aOption) const
	{
	if (aLevel != KSolBtESCO)
		{
		return KErrNotSupported;
		}
	
	switch (aName)
		{
		case EeSCOExtOptions:
			{
			TBTeSCOLinkParams* options = reinterpret_cast<TBTeSCOLinkParams*>(const_cast<TUint8*>(aOption.Ptr()));
			options->iBandwidth.iTransmit = iTransmitBandwidth;
			options->iBandwidth.iReceive = iReceiveBandwidth;
			options->iLatency = iMaxLatency;
			options->iRetransmissionEffort = iRetransmissionEffort;
			options->iCoding = iVoiceSettings;
			return KErrNone;
			}
		
		default:
			return KErrNotSupported;
		}
	}

TInt CeSCOLink::SAPSetOption(TUint aLevel, TUint aName, const TDesC8& aOption)
/**
	Case ESyncSetUserPacketTypes:
	Store the user specified pakcet types if at least one of the eSCO packet types is set
	The packet types bitmask will be passed as a parameter of CPhysicalLink::Connect(const TBTSyncPackets aUserPacketTypes)
	in DoActiveOpenL()
*/
	{
	switch (aLevel)
		{
		case KSolBtSCO:
			switch (aName)
				{
				case ESyncUserPacketTypes:
					{
					TBTSyncPacketTypes optPacketTypes = *reinterpret_cast<const TBTSyncPacketTypes*>(aOption.Ptr());
					if (TBTSyncPackets::ESyncAnyESCOPacket & optPacketTypes) //make sure at least one packet type is supported
						{
						iUserPacketTypes = static_cast<TBTSyncPacketTypes>(optPacketTypes & (TBTSyncPackets::ESyncAnyESCOPacket));//Just eSCO packet types are stored
						}
					else
						{
						return KErrNotSupported;
						}
					
					break;
					}
				
				default:
					return KErrNotSupported;
				}
			break;
			
		case KSolBtESCO:
			switch (aName)
				{
				case EeSCOExtOptions:
					{
					TBTeSCOLinkParams options = *reinterpret_cast<const TBTeSCOLinkParams*>(aOption.Ptr());
					
					iTransmitBandwidth	= Max(options.iBandwidth.iTransmit, KMinESCOBandwidth);
					iReceiveBandwidth	= Max(options.iBandwidth.iReceive, KMinESCOBandwidth);
					iMaxLatency 		= Max(options.iLatency, KMinESCOLatency);
					iRetransmissionEffort = options.iRetransmissionEffort;
					iVoiceSettings		= options.iCoding;
					
					break;
					}

				default:
					return KErrNotSupported;
				}
			break;
			
		default:
			return KErrNotSupported;
		}
	
	return KErrNone;
	}


TUint CeSCOLink::DoWrite(const TDesC8& aData, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
/**
	Actually do the write - has been checked for length
	In future could write data on a specific SCO "subchannel" if we do that sort of thing
	between devices - possibly then use aAddr or aOptions
**/
	{
	TUint retVal = 0;
	TInt err = KErrNone;
	
#ifdef STACK_SCO_DATA
	// Trim length of data if necessary
	TPtrC8 data = aData.Left(CHctlSynchronousDataFrame::KHCTLMaxSynchronousDataSize);
	if (iLinksMan.LinkManagerProtocol().LinkMuxer().CanWriteSCOData())
		{
		// for now just try to write - if it fails then it doesnt matter - its
		// a lot more *s*ynchronous than sticking it in a queue ;-)
		LOG(_L("CSCOLink: Can write SCO data..."));
		delete iOutboundSCOFrame;
		iOutboundSCOFrame = NULL;
		const CHCIFacade& facade = iLinksMan.HCIFacade(); 
		TRAP(err, iOutboundSCOFrame = facade.NewSynchronousDataFrameL(static_cast<TUint8>(data.Length())));
		if(iOutboundSCOFrame && err == KErrNone)
			{
			// got the frame - so go and use it
			facade.FormatSCOData(*iOutboundSCOFrame, iHandle, data);
			if (facade.WriteSCOData(*iOutboundSCOFrame) == KErrNone)
				{
				retVal = data.Length(); // we sent this many bytes
				}
			}
		}
	else
		{
		LOG(_L("CSCOLink: ERROR Cannot write SCO data."));
		Socket()->Error(((err == KErrNone) ? KErrInUse : err), MSocketNotify::EErrorSend);
		}
#else
	LOG(_L("CSCOLink: ERROR SCO data via stack not supported.")))
	Socket()->Error(KErrNotSupported, MSocketNotify::EErrorSend);
#endif
	return retVal;
	}

void CeSCOLink::PacketTypeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket)
	{
	iState->PacketTypeChange(*this, aErr, aConnH, aNewPacket);
	}

TBool CeSCOLink::IsLinkProbablyPossible()
	{
	TUint minBandwidth = MinBandwidth();
	if ((iTransmitBandwidth < minBandwidth) || (iReceiveBandwidth < minBandwidth))
		{
		return EFalse;
		}
	
	TUint maxBandwidth = MaxBandwidth();
	if ((iTransmitBandwidth != KESCOBandwidthDontCare) && (iTransmitBandwidth > maxBandwidth))
		{
		return EFalse;
		}
	
	if ((iReceiveBandwidth != KESCOBandwidthDontCare) && (iReceiveBandwidth > maxBandwidth))
		{
		return EFalse;
		}
	
	if (iMaxLatency < KMinESCOLatency)
		{
		return EFalse;
		}
	
	TBool airCodingOK = EFalse;
	TBTFeatures airCodings = iLinksMan.LinkManagerProtocol().PacketsSupportedLocally();
	switch (iVoiceSettings & KAirCodingFormatBits)
		{
		case ECVSD:
			airCodingOK = airCodings[ESupportedCVSDBit];
			break;
			
		case EuLawLog:
			airCodingOK = airCodings[ESupportedu_lawLogBit];
			break;
		
		case EALawLog:
			airCodingOK = airCodings[ESupportedA_lawLogBit];
			break;
		
		case ETransparentData:
			airCodingOK = airCodings[ESupportedTransparentSCODataBit];
			break;
		
		default:
			break;
		}

	return airCodingOK;
	}


TUint CeSCOLink::MinBandwidth()
	{
	TReal bandwidth = ((KMinESCOPacketSize *  KMilliSecondsFactor) / iMaxLatency);
	return static_cast<TUint>(bandwidth);
	}


TUint CeSCOLink::MaxBandwidth()
	{
	TInt maxPacketSize = 0;
	TInt interval = 1;
	
	if (iUserPacketTypes & TBTSyncPackets::ESyncPacketsEV5)
		{
		maxPacketSize = KMaxEV5Size;
		interval = KEV5Slots + KEV3Slots;
		}
	else if (iUserPacketTypes & TBTSyncPackets::ESyncPacketsEV4)
		{
		maxPacketSize = KMaxEV4Size;
		interval = KEV4Slots + KEV3Slots;
		}
	else if (iUserPacketTypes & TBTSyncPackets::ESyncPacketsEV3)
		{
		maxPacketSize = KMaxEV3Size;
		interval = KEV3Slots + KEV3Slots;
		}
	
	// Simple, easy to read version...
	//TReal bandwidth = (maxPacketSize / (KBasebandSlotTime * tesco));
	
	// Faster, still correct but harder to understand...  Add 1 to force rounding up, just in case.
	TUint bandwidth = ((KBasebandSlotsPerSecond * maxPacketSize) / interval) + 1;
	return bandwidth;
	}


TInt CeSCOLink::MakeConnection(CPhysicalLink* aPhysLink)
	{
	return aPhysLink->SynchronousConnect(iTransmitBandwidth, iReceiveBandwidth,
		iMaxLatency, iVoiceSettings,
		iRetransmissionEffort, iUserPacketTypes);
	}


TDes8& CeSCOLink::InboundFrame()
	{
	return iInboundSCOFrame;
	}

void CeSCOLink::SetExtOptions(TBTSyncConnectOpts aSyncOpts)
	{
	iVoiceSettings = aSyncOpts.iAirMode & KAirCodingFormatBits;
	
	// Using nasty magic numbers as we have a naming clash...  Responses to a command
	// use a different numbering scheme to setup commands.
	const TInt KCVSDResponse = 2;
	if (iVoiceSettings <= KCVSDResponse)
		{
		if (iVoiceSettings == KCVSDResponse)
			{
			iVoiceSettings = ECVSD;
			}
		else
			{
			iVoiceSettings++;
			}
		}
	
	TInt interval = aSyncOpts.iTransmissionInterval;
	
	TReal latency = interval * KBasebandSlotTime * KMilliSecondsFactor;
	iMaxLatency = static_cast<TInt16>(latency + 1);
	
	iTransmitBandwidth = static_cast<TUint>((KBasebandSlotsPerSecond * aSyncOpts.iTxPacketLength / interval) + 1);
	iReceiveBandwidth = static_cast<TUint>((KBasebandSlotsPerSecond * aSyncOpts.iRxPacketLength / interval) + 1);
	iRetransmissionEffort = aSyncOpts.iRetransmissionWindow;
	}

TUint16 CeSCOLink::GetPacketMask() const
	{
	return iUserPacketTypes;
	}

void CeSCOLink::GetExtOptions(TBTeSCOLinkParams& aOptions) const
	{
	aOptions.iBandwidth.iTransmit = iTransmitBandwidth;
	aOptions.iBandwidth.iReceive = iReceiveBandwidth;
	aOptions.iLatency = iMaxLatency;
	aOptions.iRetransmissionEffort = iRetransmissionEffort;
	aOptions.iCoding = iVoiceSettings;
	}
