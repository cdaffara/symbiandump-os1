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

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_SS_TIER_MANAGER_UTILS_H
#define SYMBIAN_SS_TIER_MANAGER_UTILS_H

#include <metadatabase.h>		// for TMDBElementId
#include <cdbcols.h>		// for TCommDbConnectionDirection
#include <es_sock.h>		// for TAccessPointInfo
#include <e32cmn.h>		// for RArray

class TConnIdList;
class TIp6Addr;
class TCommDbConnPref;
class TConnectionPrefs;

namespace CommsDat
{
	class CMDBSession;
	class CCDTierRecord;
	class CCDAccessPointRecord;
	class CCDMCprRecord;
	class CCDCprRecord;
	class CCDSCprRecord;
	class CCDProtocolRecord;
	class CCDGlobalSettingsRecord;
	class CCDConfigAccessPointRecord;
	class CCDIAPRecord;
	template<class TYPE> class CMDBRecordSet;
	class CMDBElement;
	template<class TYPE> class CMDBField;
	class CCDAPPrioritySelectionPolicyRecord;
}

namespace ESock
{

//
//CTierManagerUtils - for commsdat manipulation, etc so that can be used in several places
class TProviderInfo;
class SAccessPointConfig;
class RTierThreadMap;
class RConnPrefList;
class TierManagerUtils
/** Base class for selection resolver factories. Specifies the interface
    that selection resolver factories must implement.

@internalTechnology
*/
	{
	friend class TCFImplicitFlowRequest;

private:
	//private, not exported, used with legacy implicit flows only
	static TUid MapTierIdsForLegacyImplicitFlowsL(TUid aTierId, TUint aProtocol);
	
	static TInt ResolveDefAPtoTagL(TInt aElementId, CommsDat::CMDBSession& aDbs );
	
public:
	IMPORT_C static TUid MapTierIdsL(TUid aTierId, TUint aProtocol);
	IMPORT_C static CommsDat::CCDTierRecord* LoadTierRecordL(TUid aTierId, CommsDat::CMDBSession& aDbs);
	
	IMPORT_C static CommsDat::CCDTierRecord* LoadTierRecordL(CommsDat::TMDBElementId aTierId, CommsDat::CMDBSession& aDbs);
    IMPORT_C static CommsDat::CCDAPPrioritySelectionPolicyRecord* LoadAPPrioritySelRecordL(CommsDat::TMDBElementId aApSelId, CommsDat::CMDBSession& aDbs);
    IMPORT_C static CommsDat::CCDAccessPointRecord* LoadAccessPointRecordL(TUid aAccessPointId, CommsDat::CMDBSession& aDbs);
    
    
	IMPORT_C static CommsDat::CCDAccessPointRecord* LoadAccessPointRecordL(TUint aAccessPointId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CCDAccessPointRecord* LoadAccessPointRecordL(const TProviderInfo& aProviderInfo, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CMDBRecordSet<CommsDat::CCDAccessPointRecord>* LoadAccessPointRecordsL(TUid aTierId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CMDBRecordSet<CommsDat::CCDAccessPointRecord>* LoadAccessPointRecordsL(TUid aTierId, const RArray<TAccessPointInfo>& aApIdsToMatch, CommsDat::CMDBSession& aDbs);

	IMPORT_C static CommsDat::CCDMCprRecord* LoadMCprRecordL(TUint aMCprId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CCDMCprRecord* LoadMCprRecordL(TUid aMCprFactoryUid, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CCDCprRecord* LoadCprRecordL(TUint aCprId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CCDSCprRecord* LoadSCprRecordL(TUint aSCprId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CCDProtocolRecord* LoadProtocolRecordL(TUint aProtocolId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CCDGlobalSettingsRecord* LoadGlobalSettingsRecordL(CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CCDIAPRecord* LoadIapRecordL(TUint aIapId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static CommsDat::CMDBRecordSet<CommsDat::CCDIAPRecord>* LoadIapRecordsL(CommsDat::CMDBSession& aDbs);

	IMPORT_C static TUint ReadDefaultAccessPointL(TUid aTierId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static TUid ReadTierIdL(TUint aAccessPointId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static TUid ReadMCprUidL(TUint aAccessPointId, CommsDat::CMDBSession& aDbs);
	
	IMPORT_C static TUint ReadSelectionPolicyIdL(TUint aAccessPointId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static TUint ReadCustomSelectionPolicyIdL(TUint aAccessPointId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static TUint ReadCprConfigL(TUint aAccessPointId, CommsDat::CMDBSession& aDbs);
	IMPORT_C static void MapRankingToPrefsL(TUint aRanking, TCommDbConnPref& aPrefs, CommsDat::CMDBSession& aDbs);
	IMPORT_C static void ParseTLConfigAccessPointIdsL(RArray<TUint> &aIds, const TDesC& aIdList);
	IMPORT_C static CommsDat::CCDConfigAccessPointRecord* LoadConfigAccessPointRecordL(TUint aId, CommsDat::CMDBSession& aDBs);
	IMPORT_C static TUint GetConnectionAttemptsL(CommsDat::CMDBSession& aDbs);
	IMPORT_C static void MapRankingAndDirectionToPrefsL(TUint aRanking, TCommDbConnectionDirection aDirection, TCommDbConnPref& aPrefs, CommsDat::CMDBSession& aDbs);

	//This function reads the AP priority based selection policy table (CCDAPPrioritySelectionPolicyRecord)
	//and fills the array provided using the specified selectionPolicy id (record id within the policy table).
	IMPORT_C static void FillListL(TConnIdList& aList, TUint selectionPolicy, CommsDat::CMDBSession& aDbs);
	IMPORT_C static void FillListL(RConnPrefList& aList, TUint selectionPolicy, CommsDat::CMDBSession& aDbs);

	IMPORT_C static RTierThreadMap* BuildTierThreadMappingL(CommsDat::CMDBSession& aDbs);

	IMPORT_C static TUid MapElementIdToTagId(TUint aElementId, CommsDat::CMDBSession& aDbs);
	
	IMPORT_C static void GetPrefsFromConnPrefRecL(TUint aConnPrefRecElemId, 
												  CommsDat::CMDBSession& aDbs,
												  TConnectionPrefs& aConnPrefs);
	//This function is called from the Network level selector when a SNAP
	//preference is received from the client.
	IMPORT_C static TInt ConvertSNAPPrefToTagIdL(TUint aSNAPPref, CommsDat::CMDBSession& aDbs);
	IMPORT_C static TUid MapTierIdtoTierImplIdL(TUid aTierUid, CommsDat::CMDBSession& aDbs );
	};

class CCommsDatIapView : public CBase
/**
Class used for reading CommsDat Service and Bearer tables according to a particular IAP view.

@internalTechnology
*/
	{
public:
	enum TFieldId
		{
		EIfNetworks = 1,
		EConfigDaemonName,
		EConfigDaemonManagerName
		};

	IMPORT_C static CCommsDatIapView* NewL(TUint aIapId);
	IMPORT_C static CCommsDatIapView* NewLC(TUint aIapId);
	IMPORT_C ~CCommsDatIapView();

	IMPORT_C TUint IapId();
	IMPORT_C CommsDat::TMDBElementId GetServiceTableType() const;
	IMPORT_C CommsDat::TMDBElementId GetBearerTableType() const;

	IMPORT_C void GetIntL(CommsDat::TMDBElementId aElementId, TUint32& aValue);
	IMPORT_C void GetIntL(CommsDat::TMDBElementId aElementId, TInt& aValue);
	IMPORT_C void GetBoolL(CommsDat::TMDBElementId aElementId, TBool& aValue);
	IMPORT_C void GetTextL(CommsDat::TMDBElementId aElementId, HBufC8*& aValue);
	IMPORT_C void GetTextL(CommsDat::TMDBElementId aElementId, HBufC16*& aValue);

	IMPORT_C TInt GetInt(CommsDat::TMDBElementId aElementId, TUint32& aValue);
	IMPORT_C TInt GetInt(CommsDat::TMDBElementId aElementId, TInt& aValue);
	IMPORT_C TInt GetBool(CommsDat::TMDBElementId aElementId, TBool& aValue);
	IMPORT_C TInt GetText(CommsDat::TMDBElementId aElementId, HBufC8*& aValue);
	IMPORT_C TInt GetText(CommsDat::TMDBElementId aElementId, HBufC16*& aValue);

	IMPORT_C void GetTableCommonTextFieldL(CCommsDatIapView::TFieldId aFieldId, HBufC*& aValue);

	IMPORT_C void GetTimeoutValuesL(TUint32& aShortTimeout, TUint32& aMediumTimeout, TUint32& aLongTimeout);

private:
	void SetElementAndRecordIdL(CommsDat::CMDBElement* aElement, CommsDat::TMDBElementId aElementId);
    static TInt ConvertBearerTypeToTableId(const TDesC& aBearerType, CommsDat::TMDBElementId& aElementId);
    static TInt ConvertServiceTypeToTableId(const TDesC& aBearerType, CommsDat::TMDBElementId& aElementId);
    void GetIapRecordIntL(CommsDat::TMDBElementId aElementId, TUint32& aValue);
    void GetIapRecordIntL(CommsDat::TMDBElementId aElementId, TInt& aValue);
	void GetIapRecordTextL(CommsDat::TMDBElementId aElementId, HBufC16*& aValue);
	void IntFieldTypeCheckL(TInt aElementId);
	void BoolFieldTypeCheckL(TInt aElementId);
	void TextOrBinFieldTypeCheckL(TInt aElementId);
	//void BinFieldTypeCheckL(TInt aElementId);
private:
	CommsDat::CMDBSession*          iDbs;
	CommsDat::CCDIAPRecord*         iIapRec;
	TInt                            iUmtsR99QoSAndOnRecId;

	CommsDat::CMDBField<TBool>*     iBool;
	CommsDat::CMDBField<TUint32>*   iUint;
	CommsDat::CMDBField<TInt>*      iInt;
	CommsDat::CMDBField<TDesC8>*    iText8;
	CommsDat::CMDBField<TDesC16>*   iText16;
	};

} //namespace ESock


#endif
// SYMBIAN_SS_TIER_MANAGER_UTILS_H


