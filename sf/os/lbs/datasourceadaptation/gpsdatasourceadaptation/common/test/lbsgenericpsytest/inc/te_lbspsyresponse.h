// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The class representing a response from the LBS sub-system to the PSY. 
// 
//

#if (!defined __TE_LBSPSYRESPONSE__)
#define __TE_LBSPSYRESPONSE__

#include <e32base.h>
#include <lbs.h>
#include <lbssatellite.h>

#include "te_active.h"
#include "LbsInternalInterface.h"
#include "lbsnetinternalapi.h"
#include "te_lbspsystaticdata.h"
#include "lbsreffnpint.h"

/** Position update data structure */
struct TLbsPositionUpdateData
	{
	/** Position satellie info data */
	TPositionExtendedSatelliteInfo iPosInfo;
	/** Indicates a conflict control message.  iError must not be KErrNone */
	TBool iConflictControl;
	/** Postion update error */
	TInt iError;
	/** Target time for position update */
	TTime iTargetTime;
	/** Actual time for position update */
	TTime iActualTime;
	/** Extra attribute bitfield used to flag extra information */
	TUint iAttributes;
	/** Flag to tell the Location Server the gps Mode that has been 
	sent to the GPS module. At the moment this info is only used by
	the Location Server to spot transitions of mode into Hybrid mode **/
	TUint iGpsMode; 
	};

/**
The class representing a response from the LBS sub-system to the PSY.
*/
class CTe_PsyResponse : public CBase
	{
private:
	enum TType
		{
		EPosUpdate,
		ENetPosUpdate,
		EModuleStatus
		};
	
public:	
	IMPORT_C static CTe_PsyResponse* IssueSimplePosUpdateLC(TPositionModuleInfo::TTechnologyType aPosMode);
	IMPORT_C static CTe_PsyResponse* IssueOutdatedPosUpdateLC();
	IMPORT_C static CTe_PsyResponse* IssuePartialPosUpdateLC();
	IMPORT_C static CTe_PsyResponse* IssueErrPosUpdateLC(TInt aError, 
			TPositionModuleInfo::TTechnologyType aPosMode);
	IMPORT_C static CTe_PsyResponse* IssueRefPosUpdateLC(TPositionModuleInfo::TTechnologyType aPosMode);
	IMPORT_C static CTe_PsyResponse* IssueFinalNetPosUpdateLC(TPositionModuleInfo::TTechnologyType aPosMode);
	IMPORT_C static CTe_PsyResponse* IssueModuleStatusLC(const TPositionModuleStatus& aModuleStatus,
			const TPositionModuleStatusEventBase::TModuleEvent& aOccurredEventsSinceLastSet);
		

private:
	static void MakeDefaultPosUpdate(TLbsPositionUpdateData& aUpdateData);
	static void MakeEmptyPosUpdate(TLbsPositionUpdateData& aUpdateData);
	
public:	
	virtual ~CTe_PsyResponse();
	
	TBool Compare(const TPositionInfo& aSatInfo) const;
	inline const TPositionModuleStatus& PositionModuleStatus() const;
	
	inline CTestExecuteLogger& Logger() const;
	
private:
	static CTe_PsyResponse* NewLC(TType aType, 
			RLbsNetworkPositionUpdates::TLbsNetworkPositionUpdateId aNetPosBusId);
	
	CTe_PsyResponse(TType aType);
	void ConstructL(RLbsNetworkPositionUpdates::TLbsNetworkPositionUpdateId aNetPosBusId);
	
	void SetAndSavePosUpdateL(const TLbsPositionUpdateData& aData);
	void SetAndSaveRefPosUpdateL(const TLbsPositionUpdateData& aData);
	void SetModuleStatusL(const TPositionModuleStatus& aModuleStatus, 
			const TPositionModuleStatusEventBase::TModuleEvent& aOccurredEventsSinceLastSet);
	
	TBool DoubleCompare(TReal64 aFirst, TReal64 aSecond) const;
		
private: 
	TType iType;
	TLbsPositionUpdateData iPosUpdateData;
	RLbsPositionUpdates iPosUpdates;
	TPositionModuleStatus iModStatusData;
	RLbsModuleStatus iModStatus;
	RLbsNetworkPositionUpdates iNetPosUpdates;
	RLbsNetworkPositionUpdates::TLbsNetworkPositionUpdateId iNetPosBusId;
	
	};

inline const TPositionModuleStatus& CTe_PsyResponse::PositionModuleStatus() const
	{
	return iModStatusData;
	}

inline CTestExecuteLogger& CTe_PsyResponse::Logger() const
	{
	return MTe_LbsPsyStaticData::Static().Logger();
	}
		
#endif //__TE_LBSPSYRESPONSE__
