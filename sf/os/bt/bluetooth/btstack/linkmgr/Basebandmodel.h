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
//

#ifndef BASEBANDMODEL_H_
#define BASEBANDMODEL_H_

class CLinkMgrProtocol;
// this will be owned by the linkmgrprotocol 

static const TInt KBTBasebandModelLinkArrayGranularity = 2;
static const TInt KMaxPhysicalLinksWithSCO=2;
static const TInt KMaxActivePhysicalLinks=7;

enum TSCOLinkType
/**
	The value order in this enum must be maintained
*/
	{
	ENoSCOLink = 0,
	ESCOHV1Packets = 0x20,
	ESCOHV2Packets = 0x40,
	ESCOHV3Packets = 0x80,

	//uses all b/w based on 80 bits per slot at 64 kbits/sec
	ESCODVPackets = 0x100, /*no associated command mask value*/
	
	EIseSCOLink = 0x40000000	// Don't use top bit as signed, but ensure invalid.
	};

enum TACLLinkType
/**
	The value order in this enum must be maintained
*/

	{
	ENoACLLink = 0,
	EOneSlotPackets = 1,
	EThreeSlotPackets = 3,
	EFiveSlotPackets = 5,
	};

NONSHARABLE_CLASS(TPhysicalLinkData)
    {
	public:
	TPhysicalLinkData();
	TACLLinkType ACLActualMinSlots() const;
	void Park();
	void UnPark();
	TBool IsParked() const;

	public:
    TBTDevAddr		iBdaddr;
    THCIConnHandle	iConnH;
    THCIConnHandle	iSCOConnH;
    THCIConnHandle	ieSCOConnH;
	TACLLinkType	iACLMinSlotsAllowed;
	TACLLinkType	iACLMaxSlotsAllowed;
	TSCOLinkType	iSCOMinRequirement; //HV3 < HV2 < HV1
	TBool			iParked;
    };


enum TBandwidth
	{
	//HV1 packets use all available slots and completely hogs the baseband capacity (T_SCO=2)
	//HV2 packets use 2 out of every 4 slots (T_SCO=4), 
	//HV3 packets use 2 out of 6 slots (T_SCO=6)
	EBandwidthNoneUsed = 0,
	EBandwidthOneTwelfthUsed = 1,
	EBandwidthOneSixthUsed = 2,
	EBandwidthOneQuarterUsed = 3,
	EBandwidthOneThirdUsed = 4,
	EBandwidthFiveTwelfthsUsed = 5,
	EBandwidthOneHalfUsed = 6,
	EBandwidthSevenTwelfthsUsed = 7,
	EBandwidthTwoThirdsUsed = 8,
	EBandwidthThreeQuartersUsed = 9,
	EBandwidthFiveSixthsUsed = 10,
	EBandwidthElevenTwelfthsUsed = 11,
	EBandwidthAllUsed = 12,
	};
	
NONSHARABLE_CLASS(CBTBasebandModel) : public CBase
/**
	This looks at the Baseband in an orthogonal manner to the ConnectionsManager.
	The connectionManager thinks in terms of logical links to other devices.
	The BasebandModel thinks in terms of radio activity. If 25% of the slots are
	pre-booked by SCO commitments, then this knowledge is in the BasebandModel.
*/
	{
public:
	static CBTBasebandModel* NewL(CLinkMgrProtocol& aLMProt);
	~CBTBasebandModel();

	TInt UpdateModel(const TBTDevAddr& aAddr, TUint16 aPacketMask, TLinkType aLinkTypeForPacketMask);
	TInt UpdateModel(THCIConnHandle aConnH, TLinkType aLinkTypeForHandle, TUint16 aPacketMask, TLinkType aLinkTypeForPacketMask);
	TInt UpdateModel(const TBTConnect& aConn);
	TInt UpdateModel(const TBTConnect& aConn, TUint16 aPacketMask);
	TInt UpdateModel(THCIConnHandle aConnH, TUint8 aMaxSlots);
	void UpdateModelIfRecordExists(THCIConnHandle aConnH, TUint16 aPacketMask);
	void UpdateModelIfNoRecord(THCIConnHandle aConnH, TUint16 aPacketMask, TLinkType aLinkType);
	void UpdateModelForDisconnection(THCIConnHandle aConnH, TLinkType aLinkType);
	void UpdateModelForConnectionError(TBTDevAddr aAddr, TLinkType aLinkType);
	void ParkLink(THCIConnHandle aConnH);
	void UnParkLink(THCIConnHandle aConnH);
	
	TBool IsACLPossible() const;
	TBool IsSCOPossible() const;
	
private:
	CBTBasebandModel(CLinkMgrProtocol& aLMProt); //constructor
	void ConstructL();

	TBool IsACLPossible(TACLLinkType aACLType) const;
	TBool IsSCOPossible(TSCOLinkType aSCOType) const;
	TBool IsCombinationAllowed(TInt aSCOBandwidth, TACLLinkType aACLType) const;
	TBandwidth CostOfACLLink(TACLLinkType aACLType) const;
	TACLLinkType ACLRequirement(TUint16 aPacketMask) const;
	TSCOLinkType SCORequirement(TUint16 aPacketMask) const;
	TBandwidth Bandwidth(TSCOLinkType aLinkData) const;

	void GarnerInfo(TInt& aActivePhyCount, TInt& aSCOCount, TACLLinkType& aGreatestMinSlotRequirement, TInt& aSCOBandwidth) const;
	void CheckInfo(TInt aActivePhyCount, TInt aSCOCount, TACLLinkType aGreatestMinSlotRequirement, TInt aSCOBandwidth) const;

	TInt FindByAddress(const TBTDevAddr& aAddr) const;
	TInt FindByHandle(THCIConnHandle aHandle) const;
	TInt FindByACLHandle(THCIConnHandle aHandle) const;
	TInt FindBySCOHandle(THCIConnHandle aHandle) const;
	TInt FindByeSCOHandle(THCIConnHandle aHandle) const;
	TInt DoUpdateModel(const TBTConnect& aConn, TUint16 aPacketMask, TBool iIgnorePacketMask);
	void UpdateLink(TPhysicalLinkData& aLinkData, const TBTConnect& aConn);
	void UpdateLink(TPhysicalLinkData& aLinkData, TLinkType aLinkType, TUint16 aPacketMask);
	void SetupConnectRecord(TBTConnect& aConn, THCIConnHandle aConnH, 
								  const TBTDevAddr& aAddr,
								  TUint aCoD,
								  TLinkType aLinkType,
								  TEncryptMode aEncryptMode) const;



private:
	RArray<TPhysicalLinkData> iLinkData;
	CLinkMgrProtocol& iLMProt;
	};

#endif //BASEBANDMODEL_H_

