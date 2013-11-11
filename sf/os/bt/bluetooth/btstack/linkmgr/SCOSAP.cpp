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
// Implements the SCO SAP class
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
_LIT8(KLogComponent, LOG_COMPONENT_HCI_CSCOLINK);
#endif

CSCOLink* CSCOLink::NewLC(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection)
	{
	LOG_STATIC_FUNC
	CSCOLink* s = new(ELeave) CSCOLink(aConnectionMan, aConnection);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}


CSCOLink* CSCOLink::NewL(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aPhysicalLink)
	{
	LOG_STATIC_FUNC
	CSCOLink* s = NewLC(aLinksMan, aPhysicalLink);
	CleanupStack::Pop();
	return s;
	}

CSCOLink::CSCOLink(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aPhysicalLink)
: CBTSynchronousLink(aLinksMan, aPhysicalLink, ESCOLink)
	{
	LOG_FUNC
	iUserHVPacketTypes = EAnySCOPacket;
	}

void CSCOLink::ConstructL()
	{
	LOG_FUNC
	CBTSynchronousLink::ConstructL();
	}

CSCOLink::~CSCOLink()
	{
	LOG_FUNC
	}

TInt CSCOLink::GetOption(TUint /*aLevel*/, TUint /*aName*/, TDes8& /*aOption*/) const
	{
	LOG_FUNC
	return KErrNotSupported;
	}

TInt CSCOLink::SAPSetOption(TUint aLevel,TUint aName,const TDesC8 &aOption)
/**
	Case ESyncSetUserPacketTypes:
	Store the user specified pakcet types if at least one of the SCO packet types is set
	The packet types bitmask will be passed as a parameter of CPhysicalLink::Connect(const TBTSyncPackets aUserPacketTypes)
	in DoActiveOpenL()
*/
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	if(aLevel != KSolBtSCO)
		{
		rerr = KErrNotSupported;
		}
	if(rerr == KErrNone)
		{
		switch (aName)
			{
			case ESCOSetUserPacketTypes:
				{
				TUint16 optHVPacketTypes = *reinterpret_cast<const TUint16*>(aOption.Ptr());
				if (EAnySCOPacket & optHVPacketTypes) //make sure at least one HV type is supported
					{
					iUserHVPacketTypes = static_cast<TUint16>(optHVPacketTypes & (EAnySCOPacket));//Just SCO packet types are stored
					iUserHVPacketTypes /= EPacketsHV1;
					}
				else
					{
					rerr = KErrNotSupported;
					}
				break;
				}

			case ESyncUserPacketTypes:
				{
				TUint16 optHVPacketTypes = *reinterpret_cast<const TUint16*>(aOption.Ptr());
				if(TBTSyncPackets::ESyncAnySCOPacket & optHVPacketTypes) //make sure at least one HV type is supported
					{
					iUserHVPacketTypes = static_cast<TUint16>(optHVPacketTypes & (TBTSyncPackets::ESyncAnySCOPacket));//Just SCO packet types are stored
					}
				else
					{
					rerr = KErrNotSupported;
					}
				break;
				}
			
			default:
				rerr = KErrNotSupported;
			}
		}
	return rerr;
	}


TUint CSCOLink::DoWrite(const TDesC8& aData, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
/**
	Actually do the write - has been checked for length
	In future could write data on a specific SCO "subchannel" if we do that sort of thing
	between devices - possibly then use aAddr or aOptions
**/
	{
	LOG_FUNC
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
	LOG(_L("CSCOLink: ERROR SCO data via stack not supported."))
	Socket()->Error(KErrNotSupported, MSocketNotify::EErrorSend);
#endif
	return retVal;
	}

void CSCOLink::PacketTypeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket)
	{
	LOG_FUNC
	iState->PacketTypeChange(*this, aErr, aConnH, aNewPacket);
	}

TBool CSCOLink::IsLinkProbablyPossible()
	{
	LOG_FUNC
	return Baseband().IsSCOPossible();
	}

TInt CSCOLink::MakeConnection(CPhysicalLink* aPhysLink)
	{
	LOG_FUNC
	// Stores packets as sync packet types, uses packets as SCO types.
	return aPhysLink->SCOConnect(iUserHVPacketTypes << KSCOvsSyncHVOffset);
	}

TDes8& CSCOLink::InboundFrame()
	{
	LOG_FUNC
	return iInboundSCOFrame;
	}


void CSCOLink::SetExtOptions(TBTSyncConnectOpts /*aSyncOpts*/)
	{
	LOG_FUNC
	}
