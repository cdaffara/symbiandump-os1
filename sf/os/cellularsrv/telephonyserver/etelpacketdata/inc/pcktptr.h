// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ETel Multimode Packet Data API 
// MBMS Broadcast,GPRS Rel97/98, CDMAOne, CDMA2000, Release 99 and Release 4.
// 
//

/**
 @file
 @internalAll 
*/

#if !defined(__PCKTPTR_H__)
#define __PCKTPTR_H__

#if !defined(__ETELPCKT_H__)
#include "etelpckt.h"
#endif


class CEtelPacketPtrHolder : public CBase
/**
Holder class. 
Non-descriptor parameters in API methods will be copied into a 
descriptor and held in a descriptor array iPtrArray. 
This class type also stores copies of API parameters that are passed in by value as opposed
to by reference. This keeps the size of the R-classes constant as the API evolves, thus
maintaining Binary Compatibility (BC).
@internalAll
*/
	{
public:
	virtual ~CEtelPacketPtrHolder();

	// The ptr holder slot numbers used by RPacketService asynchronous requests
	enum TPacketPtrHolderSlots
	{
		ESlotPacketStatus=0,
		ESlotRegStatus,
		ESlotGetNtwkReg,
		ESlotProtocolType,
		ESlotPpdAddress,
		ESlotNrcaApn,
		ESlotDynamicCaps,
		ESlotEnumerateCount,
		ESlotEnumerateMaxCount,
		ESlotContextInfoIndex,
		ESlotContextInfo,
		ESlotSetMsClass,
		ESlotNtfMsClass,
		ESlotCurrentMsClass,
		ESlotMaxMsClass,
		ESlotSetPrefBearer,
		ESlotSetAttachMode,
		ESlotGetAttachMode,
		ESlotGetCurrentReleaseMode,
		ESlotNotifyModeChange,
		ESlotEnumerateNifs,
		ESlotGetNifInfo,
		ESlotEnumerateContextsInNif,
		ESlotGetContextNameInNif,
		ESlotNotifyAttachModeChange,
		ESlotGetPrefBearer,
		ESlotGetMbmsNetworkServiceStatus,
		ESlotGetMbmsNetworkServiceStatus2,
		ESlotNotifyMbmsNetworkServiceStatusChange,
		ESlotEnumerateMbmsMonitorCount,
		ESlotEnumerateMbmsMonitorMaxCount,
		ESlotEnumerateMbmsActiveCount,
		ESlotEnumerateMbmsActiveMaxCount,
		EMaxNumPacketPtrSlots
	};
	
	// The ptrC holder slot numbers used by RPacketService asynchronous requests
	enum TPacketPtrCHolderSlots
		{
		ESlotUpdateMbmsMonitorServiceListL=0,
		EMaxNumPacketPtrCSlots
		};

	// The ptr holder slot numbers used by RPacketContext asynchronous requests
	enum TPacketContextPtrHolderSlots
	{
		ESlotDataPort=0,
		ESlotContextStatus,
		ESlotDataVolume,
		ESlotGranularity,
		ESlotGetSpeed,
		ESlotNotifySpeed,
		ESlotGetDataVolumeTransferred,
		ESlotEnumeratePacketFilters,
		ESlotGetPacketFilterInfo,
		ESlotRemovePacketFilter,
		ESlotCreateTFT,
		ESlotRemoveMediaAuthorization,
		EMaxNumPacketContextPtrSlots
	};
	
	// The ptrC holder slot numbers used by RPacketMbmsContext asynchronous requests
	enum TPacketMbmsContextPtrCHolderSlots
		{
		ESlotUpdateMbmsSessionList=0,
		ESlotUpdateMbmsSessionList2,
		EMaxNumPacketContextPtrCSlots
		};

	// The ptr holder slot numbers used by RPacketQoS asynchronous requests
	enum TPacketQoSPtrHolderSlots
		{
		EMaxNumPacketQoSPtrSlots
		};
	
	// The ptrC holder slot numbers used by RPacketQoS asynchronous requests
	enum TPacketQoSPtrCHolderSlots
		{
		EMaxNumPacketQoSPtrCSlots
		};	

public:
	template <typename T> inline TPtr8& Set(TInt aSlot,T& aObject)
		{
		TPtr8& ptr=Ptr(aSlot);
		ptr.Set(reinterpret_cast<TText8*>(&aObject),sizeof(T),sizeof(T));
		return ptr;
		};
		
	template <typename T> inline TPtrC8& SetC(TInt aSlot, const T& aObject)
		{
		TPtrC8& ptr=PtrC(aSlot);
		ptr.Set(REINTERPRET_CAST(const TText8*,(&aObject)),sizeof(T));
		return ptr;
		};

protected:
	virtual void ConstructL(const TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray);	
	CEtelPacketPtrHolder();
private:
	TPtr8& Ptr(const TInt aIndex);
	TPtrC8& PtrC(const TInt aIndex);

protected:
	RArray<TPtr8> iPtrArray;
	RArray<TPtrC8> iPtrCArray;
	};



class CPacketQoSPtrHolder : public CEtelPacketPtrHolder
/**
QoS profile ptr holder class.
This class inherits from the CEtelPacketPtrHolder class.
Currently, there are no asynch. requests in RPacketQoS that pass argument by value.
@internalAll
*/
	{
public:
	static CPacketQoSPtrHolder* NewL(const TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray);
	~CPacketQoSPtrHolder();
protected:
	CPacketQoSPtrHolder();
	};


class CPacketContextPtrHolder : public CEtelPacketPtrHolder
/**
Packet Context ptr holder class.
This class inherits from the CEtelPacketPtrHolder class.
@internalAll
*/
	{
public:
	static CPacketContextPtrHolder* NewL(const TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray);
	~CPacketContextPtrHolder();
protected:
	CPacketContextPtrHolder();
public:
	RPacketContext::TNotifyDataTransferredRequest iNotifyDataTransferRequest;	//< used in RPacketContext::NotifyDataTransferred
	TInt iGetPacketFilterInfoIndex;												//< used in RPacketContext::GetPacketFilterInfo(*,*,*)
	TInt iRemovePacketFilterId;													//< used in RPacketContext::RemovePacketFilter(*,*)
	TInt iCreateTFTSize;														//< used in RPacketContext::CreateNewTFT
	HBufC8* iStoreNetworkBuf;													//< used in RPacketContext::AddMediaAuthorizationL
	TPtr8 iStoreNetworkPtr;														//< used in RPacketContext::AddMediaAuthorizationL
	TMbmsAction iMbmsAction;													//< used in RPacketContext::UpdateMbmsSessionList
	TMbmsSessionId	iSessionId;													//< used in RPacketContext::UpdateMbmsSessionList
	};	

/**
Packet Service ptr holder class.
This class inherits from the CEtelPacketPtrHolder class.
@internalAll
*/
class CPacketPtrHolder : public CEtelPacketPtrHolder
	{
public:
	static CPacketPtrHolder* NewL(const TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray);
	~CPacketPtrHolder();
protected:
	CPacketPtrHolder();
public:
	TInt iGetContextInfoIndex;								//< used in RPacketService::GetContextInfo() async. version
	RPacketService::TMSClass iMSClass;						//< used in RPacketService::SetMSClass() async. version
	RPacketService::TPreferredBearer iPrefBearer;			//< used in RPacketService::SetPreferredBearer() async. version
	RPacketService::TAttachMode iAttachMode;				//< used in RPacketService::SetAttachMode(), async. version
	TInt iGetNifInfoIndex;									//< used in RPacketService::GetNifInfo(*,*,*)
	RPacketService::TContextNameInNif iGetContextNameInNif; //< used in RPacketService::GetContextNameInNif() 
	CBufBase* iStoreMbmsBuf;								//< used in RPacketService::UpdateMbmsMonitorServiceListL
	TPtr8 iStoreMbmsPtr;									//< used in RPacketService::UpdateMbmsMonitorServiceListL
	TBool iMbmsAttemptAttach;								//< used in RPacketService::GetMbmsNetworkServiceStatus
	TMbmsAction iMbmsAction;								//< used in RPacketService::UpdateMbmsMonitorServiceListL
	};	

#endif
