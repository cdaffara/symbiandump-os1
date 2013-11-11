// CTestTelephonyPktSharedData.h
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyPktSharedData

#ifndef TESTTELEPHONYPKTSHAREDDATA_H_
#define TESTTELEPHONYPKTSHAREDDATA_H_

// system include
#include <etelqos.h>
#include <pcktcs.h>

// const define
const TInt KTFTFiltersGranularity 	= 1;
const TInt KMaxServiceNum 			= 1;
const TInt KMaxContextNum 			= 11;

/**
 * This class defines the shared data of PACKET test category between 
 * different test steps.
 */
class CTestLtsyPktSharedData : public CBase
/**
 * @internalAll
 * @test
 */
	{
public: 
	virtual ~CTestLtsyPktSharedData();
	static CTestLtsyPktSharedData* NewL();	
private:
	CTestLtsyPktSharedData();
	void ConstructL();

public:
	RPacketService                      iPacket;
	RPacketService::TStatus             iPacketStatus;
	RPacketService::TRegistrationStatus iRegistrationStatus;
	RPacketService::TMSClass            iCurrentMobileStationClass;
	RPacketService::TMSClass            iMaxMobileStationClass;
	TUint                               iStaticCaps;
	RPacketService::TDynamicCapsFlags   iDynamicCaps;
	RPacketService::TAttachMode         iCurrentAttachMode;
	TInt                                iNumContexts;
	TInt                                iMaxContextsAllowed;
	RPacketContext                      iContext1;
	RPacketContext                      iContext2;
	RPacketContext                      iContext3;
	RPacketService::TContextInfo        iContextInfo;
	RPacketService::TContextInfo        iContextInfo2;
	TName                               iContextName;
	TName                               iContextName1;
	RPacketContext::TContextStatus      iContextStatus;
	RPacketQoS                          iQoS;
	TName                               iQoSName;
	RPacketQoS::TQoSCapsGPRS        iQoSCapabilities;
	RPacketQoS::TQoSGPRSRequested   iQoSRequested;
	RPacketQoS::TQoSGPRSNegotiated  iQoSNegotiated;
	};

#endif // TESTTELEPHONYPKTSHAREDDATA_H_
