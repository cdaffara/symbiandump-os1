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
// Captures the current slot commitments of our device
// so this can be viewed of as representing (partially) the
// Basic Piconet Physical *Channel*
// 
//

#include <bt_sock.h>
#include <bluetooth/logger.h>
#include "linkconsts.h"
#include "linkutil.h"
#include "basebandsap.h"
#include "Basebandmodel.h"
#include "hcifacade.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

#ifdef _DEBUG
PANICCATEGORY("basebndm");
#endif

TACLLinkType TPhysicalLinkData::ACLActualMinSlots() const
	{
	LOG_FUNC
	return (iACLMaxSlotsAllowed < iACLMinSlotsAllowed) ? ENoACLLink : iACLMinSlotsAllowed;
	}

TPhysicalLinkData::TPhysicalLinkData()
: iBdaddr(0), iConnH(0), iSCOConnH(0), ieSCOConnH(0), iACLMinSlotsAllowed(ENoACLLink), iACLMaxSlotsAllowed(EOneSlotPackets), iSCOMinRequirement(ENoSCOLink), iParked(EFalse)
	{
	LOG_FUNC
	}

void TPhysicalLinkData::Park()
	{
	LOG_FUNC
	iParked = ETrue;
	}

void TPhysicalLinkData::UnPark()
	{
	LOG_FUNC
	iParked = EFalse;
	}

TBool TPhysicalLinkData::IsParked() const
	{
	LOG_FUNC
	return iParked;
	}


//PUBLIC FUNCTIONS
CBTBasebandModel* CBTBasebandModel::NewL(CLinkMgrProtocol& aLMProt)
	{
	LOG_STATIC_FUNC
	CBTBasebandModel* self = new(ELeave) CBTBasebandModel(aLMProt);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CBTBasebandModel::~CBTBasebandModel()
	{
	LOG_FUNC
	iLinkData.Reset();
	iLinkData.Close();
	}

TInt CBTBasebandModel::UpdateModel(const TBTDevAddr& aAddr, TUint16 aPacketMask, TLinkType aLinkTypeForPacketMask)
	{
	LOG_FUNC
	TBTConnect conn;
	SetupConnectRecord(conn, 0, aAddr, 0, aLinkTypeForPacketMask, EDisabled);
	return UpdateModel(conn, aPacketMask);
	}

TInt CBTBasebandModel::UpdateModel(THCIConnHandle aConnH, TLinkType aLinkTypeForHandle, TUint16 aPacketMask, TLinkType aLinkTypeForPacketMask)
	{
	LOG_FUNC
	//See if link record already exists
	TInt found = FindByHandle(aConnH);
	//If so, update
	if(found != KErrNotFound)
		{
		if (!iLinkData[found].IsParked())
			{
			UpdateLink(iLinkData[found], aLinkTypeForPacketMask, aPacketMask);
			}
		return KErrNone;
		}

	TPhysicalLinkData ld;
	TBTConnect conn;
	SetupConnectRecord(conn, aConnH, TBTDevAddr(0), 0, aLinkTypeForHandle, EDisabled);
	UpdateLink(ld, conn);
	UpdateLink(ld, aLinkTypeForPacketMask, aPacketMask);
	iLinkData.Append(ld); //NB failure => we become too tolerant => unnecessary HCI traffic
	return KErrNone;
	}

TInt CBTBasebandModel::UpdateModel(const TBTConnect& aConn)
/**
	Tries to find the record containing either the BD address, or, if not, the handle
	supplied in 'aConn'. 
	If successful, updates record with those values in 'aConn' that are not already 
	in the record, and a value deduced 'aPacketMask'.
	Whether all these values are used for ACL or SCO is decided by 'aConn.iLinkType'.
	If unsuccessful, creates a new record with 'aConn', and 'aPacketMask'.
*/
	{
	LOG_FUNC
	return DoUpdateModel(aConn, 0, ETrue);
	}

TInt CBTBasebandModel::UpdateModel(const TBTConnect& aConn, TUint16 aPacketMask)
/**
	Tries to find the record containing either the BD address, or, if not, the handle
	supplied in 'aConn'. 
	If successful, updates record with those values in 'aConn' that are not already 
	in the record, and a value deduced 'aPacketMask'.
	Whether all these values are used for ACL or SCO is decided by 'aConn.iLinkType'.
	If unsuccessful, creates a new record with 'aConn', and 'aPacketMask'.
*/
	{
	LOG_FUNC
	return DoUpdateModel(aConn, aPacketMask, EFalse);
	}

TInt CBTBasebandModel::UpdateModel(THCIConnHandle aConnH, TUint8 aMaxSlots)
	{
	LOG_FUNC
	TInt found = FindByACLHandle(aConnH);
	if(found==KErrNotFound)
		{
		return KErrArgument;
		}
	//convert param 2 to contraints of CBTBasebandModel
	TACLLinkType maxSlots;
	switch(aMaxSlots)
		{
		case 1:
			maxSlots = EOneSlotPackets;
			break;
		case 3:
			maxSlots = EThreeSlotPackets;
			break;
		case 5:
			maxSlots = EFiveSlotPackets;
			break;
		default:
			maxSlots = ENoACLLink; //not sure - make most likely to pass
			break;
		}

	iLinkData[found].iACLMaxSlotsAllowed = maxSlots;
	return KErrNone;
	}

void CBTBasebandModel::UpdateModelIfRecordExists(THCIConnHandle aConnH, TUint16 aPacketMask)
   	{
	LOG_FUNC
   	#pragma message("eSCO ignored in baseband model")
   	// Updates link bandwidth consumption statistics
   	
   	TInt found = FindByACLHandle(aConnH);
  	if(found != KErrNotFound)
  		{
  		UpdateLink(iLinkData[found], EACLLink, aPacketMask);
  		return;
   		}
   		
	found = FindBySCOHandle(aConnH);
	if(found != KErrNotFound)
		{
		UpdateLink(iLinkData[found], ESCOLink, aPacketMask);
		return;
		}
   	}
   
void CBTBasebandModel::UpdateModelIfNoRecord(THCIConnHandle aConnH, TUint16 aPacketMask, TLinkType aLinkType)
	{
	LOG_FUNC
	if(FindByHandle(aConnH) == KErrNotFound)
		{
		TBTConnect conn;
		SetupConnectRecord(conn, aConnH, TBTDevAddr(0), 0, aLinkType, EDisabled);
		TPhysicalLinkData ld;
		UpdateLink(ld, conn);
		UpdateLink(ld, aLinkType, aPacketMask);
		iLinkData.Append(ld); //NB failure => we become too tolerant => unnecessary HCI traffic
		}
	}

void CBTBasebandModel::UpdateModelForDisconnection(THCIConnHandle aConnH, TLinkType aLinkType)
	{
	LOG_FUNC
	TInt found = KErrNotFound;
	
	LOG2(_L("Baseband model: updating for disconnect. Handle %d, link type %d"), aConnH, aLinkType);
	
	switch (aLinkType)
		{
		case EACLLink:
			found = FindByACLHandle(aConnH);
			if (found != KErrNotFound)
				{
				iLinkData.Remove(found); //remove record if ACL
				}
			break;
		
		case ESCOLink:
			found = FindBySCOHandle(aConnH);
			if(found != KErrNotFound)
				{
				iLinkData[found].iSCOConnH = 0; //zero SCO handle if SCO 
				iLinkData[found].iSCOMinRequirement = ENoSCOLink;//set SCO requirements to 0
				}
				
		#ifdef _DEBUG
			found = FindBySCOHandle(aConnH);
			ASSERT_DEBUG(found == KErrNotFound);
		#endif
			
			break;
		
		case EeSCOLink:
			found = FindByeSCOHandle(aConnH);
			if (found != KErrNotFound)
				{
				LOG1(_L("Baseband model: Found eSCO link at index %d"), found);
				iLinkData[found].ieSCOConnH = 0; // zero eSCO handle
				}
				
		#ifdef _DEBUG
			found = FindByeSCOHandle(aConnH);
			ASSERT_DEBUG(found == KErrNotFound);
		#endif
			
			break;
		
		// Explicitly ignore 'default' in the hope of compiler warnings if link types
		// grow again.
		}
	}

void CBTBasebandModel::UpdateModelForConnectionError(TBTDevAddr aAddr, TLinkType aLinkType)
	{
	LOG_FUNC
	TInt found = KErrNotFound;
	
	LOG1(_L("Baseband model: updating for connection error. link type %d"), aLinkType);
	LOG(_L("  BDADDR: "));
	LOGBTDEVADDR(aAddr);
	
	found = FindByAddress(aAddr);

	if (found != KErrNotFound)
		{
		switch(aLinkType)
			{
			case (EACLLink):
				iLinkData.Remove(found); //remove record if ACL
				break;
				
			case (ESCOLink):
				iLinkData[found].iSCOConnH = 0; //zero SCO handle 
				iLinkData[found].iSCOMinRequirement = ENoSCOLink;//set SCO requirements to 0
				break;
				
			case (EeSCOLink):
				LOG1(_L("Baseband model: Found eSCO link at index %d"), found);
				iLinkData[found].ieSCOConnH = 0; // zero eSCO handle
				break;
			}
		}
	}
	
void CBTBasebandModel::ParkLink(THCIConnHandle aConnH)
	{
	LOG_FUNC
	TInt found = FindByACLHandle(aConnH);
	if (found != KErrNotFound)
		{
		iLinkData[found].Park();
		}
	}


void CBTBasebandModel::UnParkLink(THCIConnHandle aConnH)
	{
	LOG_FUNC
	TInt found = FindByACLHandle(aConnH);
	if (found != KErrNotFound)
		{
		iLinkData[found].UnPark();
		}
	}


TBool CBTBasebandModel::IsACLPossible() const
/**
	Decide if ACL link with default packet type is possible
	If error, or not sure, return ETrue and let hardware sort it out.
*/
	{
	LOG_FUNC
	return IsACLPossible(ACLRequirement(KHCIDefaultPacketType));
	}


TBool CBTBasebandModel::IsSCOPossible() const
/**
	Decide if SCO link with default packet type is possible
	If error, or not sure, return ETrue and let hardware sort it out.
*/
	{
	LOG_FUNC
	return IsSCOPossible(SCORequirement(KHCIDefaultSCOPacketType));
	}



//PRIVATE FUNCTIONS
CBTBasebandModel::CBTBasebandModel(CLinkMgrProtocol& aLMProt) 
: iLinkData(KBTBasebandModelLinkArrayGranularity),
iLMProt(aLMProt)
	{
	LOG_FUNC
	}

void CBTBasebandModel::ConstructL()
	{
	LOG_FUNC
	}


TBool CBTBasebandModel::IsACLPossible(TACLLinkType aACLType) const
/**
	Decide if another ACL link allowing minimum of 'aACLType' packets
	will be allowed by hardware LMP.
	If error, or not sure, return ETrue and let hardware sort it out.
*/
	{
	LOG_FUNC
	TACLLinkType aclLinkType = ENoACLLink;
	TInt scoBandwidth = 0;
	TInt activePhyCount = 0;
	TInt scoCount = 0;
	GarnerInfo(activePhyCount, scoCount, aclLinkType, scoBandwidth);
	CheckInfo(activePhyCount, scoCount, aclLinkType, scoBandwidth);

	// Check that the stack could accommodate another ACL link
	if (activePhyCount > KMaxActivePhysicalLinks)
		{
		return EFalse;
		}
	
	return IsCombinationAllowed(scoBandwidth, aACLType);
	}

TBool CBTBasebandModel::IsSCOPossible(TSCOLinkType aSCOType) const
/**
	Decide if SCO link allowing minimum of 'aSCOType' packets
	will be allowed by hardware LMP.
	If error, or not sure, return ETrue and let hardware sort it out.
*/
	{
	LOG_FUNC
	TACLLinkType aclLinkType = ENoACLLink;
	TInt scoBandwidth = 0;
	TInt scoCount = 0;
	TInt activePhyCount = 0;
	GarnerInfo(activePhyCount, scoCount, aclLinkType, scoBandwidth);
	CheckInfo(activePhyCount, scoCount, aclLinkType, scoBandwidth);
	
	
	#ifdef _DEBUG
	TInt bw = Bandwidth(aSCOType);
	LOG(_L("IsSCOLinkPossible: Pre-existing:-"));
	LOG2(_L("IsSCOLinkPossible: PHY count = %d, scoCount = %d, ACL link type = %d, "), activePhyCount, scoCount);
	LOG2(_L("IsSCOLinkPossible: ACL link type = %d, scoBandwidth = %d"), aclLinkType, scoBandwidth);
	LOG(_L("IsSCOLinkPossible: New requirement:-"));
	LOG2(_L("IsSCOLinkPossible: SCO type required = %d, => bandwidth needed = %d"), aSCOType, bw);
	#endif

  	
  	//Deal with too much SCO
  	if((scoBandwidth + Bandwidth(aSCOType))>EBandwidthAllUsed)
   		{
   		return EFalse;
   		}
  	//Deal with too many PHYs using SCOs
  	if(scoCount>=KMaxPhysicalLinksWithSCO&&aSCOType!=ENoSCOLink)
   		{
  		return EFalse;
  		}
  	return IsCombinationAllowed(scoBandwidth + Bandwidth(aSCOType), aclLinkType);
	}

TBool CBTBasebandModel::IsCombinationAllowed(TInt aSCOBandwidth, TACLLinkType aACLType) const
	{
	LOG_FUNC
	//Now go through each ACL/SCOcombination carefully!
	switch(aSCOBandwidth)
		{
		case EBandwidthNoneUsed:
			return ETrue;
		case EBandwidthOneThirdUsed:
			return (aACLType<=EThreeSlotPackets);
		case EBandwidthOneHalfUsed:
		case EBandwidthTwoThirdsUsed:
			return (aACLType<=EOneSlotPackets);
		default:
			return (aSCOBandwidth <= EBandwidthAllUsed);
		}
	}

TBandwidth CBTBasebandModel::Bandwidth(TSCOLinkType aLinkType) const
	{
	LOG_FUNC
	switch (aLinkType)
		{
		case ENoSCOLink:
			return EBandwidthNoneUsed;
			
		case ESCODVPackets:
			return EBandwidthAllUsed;
			
		case ESCOHV1Packets:
			return EBandwidthAllUsed;
			
		case ESCOHV2Packets:
			return EBandwidthOneHalfUsed;
			
		case ESCOHV3Packets:
			return EBandwidthOneThirdUsed;
			
		default:
			__ASSERT_DEBUG(ETrue, Panic(EBTBasebandModelBadRecord));
			return EBandwidthNoneUsed;
		}
	}

void CBTBasebandModel::GarnerInfo(TInt& aActivePhyCount, TInt& aSCOCount, TACLLinkType& aGreatestMinSlotRequirement, TInt& aSCOBandwidth) const
	{
	LOG_FUNC
	aSCOCount = 0;
	aGreatestMinSlotRequirement = ENoACLLink;
	aSCOBandwidth = 0;

	TInt totalPhyCount = iLinkData.Count();
	aActivePhyCount = 0;
	for (TInt i=(totalPhyCount-1); i>=0; i--)
		{
		if (!iLinkData[i].IsParked())
			{
			aActivePhyCount++;
			if (iLinkData[i].iSCOMinRequirement != ENoSCOLink)
				{
				aSCOCount++; //only one SCO link per PHY for the time being
				aSCOBandwidth += Bandwidth(iLinkData[i].iSCOMinRequirement);
				}
			aGreatestMinSlotRequirement = aGreatestMinSlotRequirement<iLinkData[i].ACLActualMinSlots()?
								iLinkData[i].iACLMinSlotsAllowed:aGreatestMinSlotRequirement;
			}
		}
	__ASSERT_DEBUG(aSCOBandwidth <= EBandwidthAllUsed,
				   Panic(EBTBasebandModelBadRecord));
	}

void CBTBasebandModel::CheckInfo(TInt __DEBUG_ONLY(aActivePhyCount),
								 TInt __DEBUG_ONLY(aSCOCount),
								 TACLLinkType __DEBUG_ONLY(aGreatestMinSlotRequirement),
								 TInt __DEBUG_ONLY(aSCOBandwidth)) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aActivePhyCount <= KMaxActivePhysicalLinks,
				   Panic(EBTBasebandModelBadRecord));
	__ASSERT_DEBUG(aSCOCount <= KMaxPhysicalLinksWithSCO,
				   Panic(EBTBasebandModelBadRecord));
	__ASSERT_DEBUG((aSCOBandwidth == EBandwidthNoneUsed ||
				   aSCOBandwidth == EBandwidthOneThirdUsed ||
				   aSCOBandwidth == EBandwidthOneHalfUsed ||
				   aSCOBandwidth == EBandwidthTwoThirdsUsed ||
				   aSCOBandwidth == EBandwidthAllUsed),
				   Panic(EBTBasebandModelBadRecord));
	__ASSERT_DEBUG((aGreatestMinSlotRequirement == ENoACLLink ||
				   aGreatestMinSlotRequirement == EOneSlotPackets ||
				   aGreatestMinSlotRequirement == EThreeSlotPackets ||
				   aGreatestMinSlotRequirement == EFiveSlotPackets),
				   Panic(EBTBasebandModelBadRecord));
	}

TBandwidth CBTBasebandModel::CostOfACLLink(TACLLinkType aACLType) const
	{
	LOG_FUNC
	switch(aACLType)
		{
		case EOneSlotPackets:
		return EBandwidthOneTwelfthUsed;
		case EThreeSlotPackets:
		return EBandwidthFiveSixthsUsed;
		case EFiveSlotPackets:
		return EBandwidthElevenTwelfthsUsed;
		default:
		return EBandwidthNoneUsed;
		}
	}

TACLLinkType CBTBasebandModel::ACLRequirement(TUint16 aPacketMask) const
	{
	LOG_FUNC
	if(aPacketMask & (KDM1Packet | KDH1Packet | KAUX1Packet))
		{
		return EOneSlotPackets;
		}
	if(aPacketMask & (KDM3Packet | KDH3Packet))
		{
		return EThreeSlotPackets;
		}
	if(aPacketMask & (KDM5Packet | KDH5Packet))
		{
		return EFiveSlotPackets;
		}
	//No ACL link type given so assume...
//	__DEBUGGER();
	return ENoACLLink;
	}

TSCOLinkType CBTBasebandModel::SCORequirement(TUint16 aPacketMask) const
	{
	LOG_FUNC
	//Baseband needs to err towards the least bandwidth used,
	//so start checking mask for HV3 packets
	if(aPacketMask & ESCOHV3Packets)
		{
		return ESCOHV3Packets;
		}
	if(aPacketMask & ESCOHV2Packets)
		{
		return ESCOHV2Packets;
		}
	if(aPacketMask & ESCOHV1Packets)
		{
		return ESCOHV1Packets;
		}
	//DV as expensive as HV1 but do allow ACL data transmission over
	//the physical link on which the SCO link has been created
	if(aPacketMask & ESCODVPackets)
		{
		return ESCODVPackets;
		}
	//No SCO link type given so assume...
	__DEBUGGER();
	return ENoSCOLink;
	}

TInt CBTBasebandModel::FindByAddress(const TBTDevAddr& aAddr) const
	{
	LOG_FUNC
	if(aAddr == TBTDevAddr(0))
		//Do not search on zero.
		{
		return KErrNotFound;
		}

	TInt count = iLinkData.Count();
	for (TInt i=(count-1); i>=0; i--)
		{
		if (iLinkData[i].iBdaddr == aAddr && (!iLinkData[i].IsParked()))
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt CBTBasebandModel::FindByHandle(THCIConnHandle aHandle) const
	{
	LOG_FUNC
	TInt ret = FindByACLHandle(aHandle);
	if(ret == KErrNotFound)
		{
		ret = FindBySCOHandle(aHandle);
		}
	if(ret == KErrNotFound)
		{
		ret = FindByeSCOHandle(aHandle);
		}
	return ret;
	}	

TInt CBTBasebandModel::FindByACLHandle(THCIConnHandle aHandle) const
	{
	LOG_FUNC
	if(!aHandle)
		//Do not search on zero.
		{
		return KErrNotFound;
		}

	for (TInt i=(iLinkData.Count()-1); i>=0; i--)
		{
		const TPhysicalLinkData& handle = iLinkData[i];
		if ((handle.iConnH == aHandle))
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt CBTBasebandModel::FindBySCOHandle(THCIConnHandle aHandle) const
	{
	LOG_FUNC
	if(!aHandle)
		//Do not search on zero.
		{
		return KErrNotFound;
		}

	TInt count = iLinkData.Count();
	for (TInt i=(count-1); i>=0; i--)
		{
		if (iLinkData[i].iSCOConnH == aHandle)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt CBTBasebandModel::FindByeSCOHandle(THCIConnHandle aHandle) const
	{
	LOG_FUNC
	if(!aHandle)
		//Do not search on zero.
		{
		return KErrNotFound;
		}

	TInt count = iLinkData.Count();
	for (TInt i=(count-1); i>=0; i--)
		{
		if (iLinkData[i].ieSCOConnH == aHandle)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt CBTBasebandModel::DoUpdateModel(const TBTConnect& aConn, TUint16 aPacketMask, TBool iIgnorePacketMask)
/**
	Tries to find the record containing either the BD address, or, if not, the handle
	supplied in 'aConn'. 
	If successful, updates record with those values in 'aConn' that are not already 
	in the record, and a value deduced 'aPacketMask'.
	Whether all these values are used for ACL or SCO is decided by 'aConn.iLinkType'.
	If unsuccessful, creates a new record with 'aConn', and 'aPacketMask'.
*/
	{
	LOG_FUNC
	//See if link record already exists
	TInt found = FindByAddress(aConn.iBdaddr);
	if(found == KErrNotFound)
		{
		//try finding by handle
		found = FindByHandle(aConn.iConnH);
		}
	//If so, update
	if(found != KErrNotFound)
		{
		if (!iLinkData[found].IsParked())
			{
			UpdateLink(iLinkData[found], aConn);
			if(!iIgnorePacketMask)
				{
				UpdateLink(iLinkData[found], aConn.iLinkType, aPacketMask);
				}
			}
		return KErrNone;
		}

	//Got to here - this is a new PHY
	__ASSERT_DEBUG(aConn.iLinkType==EACLLink, Panic(EBTBasebandModelBadRecord));
	if(aConn.iLinkType!=EACLLink)
	{
	return KErrArgument;
	}

	TPhysicalLinkData ld;
	UpdateLink(ld, aConn);
	if(!iIgnorePacketMask)
		{
		UpdateLink(ld, aConn.iLinkType, aPacketMask);
		}
	(void)iLinkData.Append(ld); //NB failure => we become too tolerant => unnecessary HCI traffic
	return KErrNone;
	}

void CBTBasebandModel::UpdateLink(TPhysicalLinkData& aLinkData, const TBTConnect& aConn)
/**
	Use aPacketMask together those values aConn not already in 
	aLinkData to update aLinkData.
	NB An ACL handle may have been supplied in aConn, 
	   but the update is for a SCO Link
*/
	{
	LOG_FUNC
	LOG5(_L("Baseband model: updating record %d,%d,%d, to %d, type %d"), aLinkData.iConnH, aLinkData.iSCOConnH, aLinkData.ieSCOConnH, aConn.iConnH, aConn.iLinkType);
	__ASSERT_DEBUG((aLinkData.iBdaddr==TBTDevAddr(0) || aLinkData.iBdaddr==aConn.iBdaddr),
					Panic(EBTBasebandModelBadRecord));
	aLinkData.iBdaddr = aConn.iBdaddr;
	
	switch (aConn.iLinkType)
		{
		case ESCOLink:
			__ASSERT_DEBUG((aLinkData.iSCOConnH==0 || aLinkData.iSCOConnH==aConn.iConnH),
							Panic(EBTBasebandModelBadRecord));
			aLinkData.iSCOConnH = aConn.iConnH;
			break;

		case EACLLink:
			__ASSERT_DEBUG((aLinkData.iConnH==0 || aLinkData.iConnH==aConn.iConnH),
							Panic(EBTBasebandModelBadRecord));
			aLinkData.iConnH = aConn.iConnH;
			break;
		
		case EeSCOLink:
			__ASSERT_DEBUG((aLinkData.ieSCOConnH==0 || aLinkData.ieSCOConnH==aConn.iConnH),
							Panic(EBTBasebandModelBadRecord));
			aLinkData.ieSCOConnH = aConn.iConnH;
			break;
		
		default:
			Panic(EBTUnknownLogicalLink);
		}
	}

void CBTBasebandModel::UpdateLink(TPhysicalLinkData& aLinkData, TLinkType aLinkType, TUint16 aPacketMask)
	{
	LOG_FUNC
	if(aLinkType==ESCOLink)
		{
		aLinkData.iSCOMinRequirement = SCORequirement(aPacketMask);
		}
	else if (aLinkType == EACLLink)
		{
		aLinkData.iACLMinSlotsAllowed = ACLRequirement(aPacketMask);
		}
	}

void CBTBasebandModel::SetupConnectRecord(TBTConnect& aConn, THCIConnHandle aConnH, 
												const TBTDevAddr& aAddr,
												TUint aCoD,
												TLinkType aLinkType,
												TEncryptMode aEncryptMode) const
/**
	Helper class to fill out a TBTConnect
*/
	{
	LOG_FUNC
    aConn.iConnH = aConnH;
    aConn.iBdaddr = aAddr;
    aConn.iCoD = aCoD;
    aConn.iLinkType = aLinkType;
    aConn.iEncryptMode = aEncryptMode;
	}
