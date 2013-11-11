/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* DataMonitoringProvider.h
* 
*
*/



/**
 @file
 @internalComponent 
*/

#if !defined(DATAMONITORINGPROVIDER_H_INCLUDED)
#define DATAMONITORINGPROVIDER_H_INCLUDED

#include <e32const.h>
#include <comms-infras/ss_datamon_apiext.h>
#include <comms-infras/ss_nodemessages.h>

namespace ESock
{

class TCFDataMonitoringNotification
	{
private:
	enum
	{
    ECFDataMonitoringNotification   = 1
	};
public:
    enum { ERealmId = 0x10285A78 }; //UID allocated on 21/11/2007
    /*==========================================================================
    [Name]:      TDataMonitoringNotification
    [Semantics]: [399TODO] RZ: Simon please finish.

    [Type]:
    [Fallible]:

    [Structure]:
    ===========================================================================*/
	typedef Messages::TMessageSigNumberNumber<ECFDataMonitoringNotification, TCFDataMonitoringNotification::ERealmId> TDataMonitoringNotification;
	};


/*
	Enumerations for typeids
*/
enum 
    {
    EDataMonitoringProvisioningInfoBase,
    EDataMonitoringConnProvisioningInfo,
    EDataMonitoringSubConnProvisioningInfo
    };
    
/**
	@internalTechnology
*/
enum TDataMonitoringDirection
	{
	EReceived,
	ESent
	};

/**
	@internalTechnology
*/
struct TDataVolumes
	{
	volatile TUint32 iReceivedBytes;
	volatile TUint32 iSentBytes;
	};
	
/**
	@internalTechnology
*/
struct TNotificationThresholds
	{
	volatile TUint32 iReceivedThreshold;
	volatile TUint32 iSentThreshold;
	};


const TUint32 KDataMonitoringProvisioningUid = 0x10281E0D;

/**
	Base class for flow provisioning data

	@internalTechnology
*/
class TDataMonitoringProvisioningInfoBase : public Meta::SMetaData
	{
public:
    enum 
        { 
        iId = EDataMonitoringProvisioningInfoBase,
        iUid = KDataMonitoringProvisioningUid
        };

protected:
	TDataMonitoringProvisioningInfoBase(TDataVolumes* aDataVolumesPtr, TNotificationThresholds* aThresholdsPtr) :
		iDataVolumesPtr(aDataVolumesPtr), iThresholdsPtr(aThresholdsPtr)
		{
		}

private:
	TDataMonitoringProvisioningInfoBase();
	
public:        
	TDataVolumes* iDataVolumesPtr;
	TNotificationThresholds* iThresholdsPtr;

	EXPORT_DATA_VTABLE_AND_FN
	};

/**
	@internalTechnology
*/
class TDataMonitoringConnProvisioningInfo : public TDataMonitoringProvisioningInfoBase
	{
public:
    enum 
        { 
        iId = EDataMonitoringConnProvisioningInfo
        };
        
	IMPORT_C TDataMonitoringConnProvisioningInfo(TDataVolumes* aDataVolumesPtr, TNotificationThresholds* aThresholdsPtr);
		
	EXPORT_DATA_VTABLE_AND_FN
		
private:
	TDataMonitoringConnProvisioningInfo();
	};

/**
	@internalTechnology
*/
class TDataMonitoringSubConnProvisioningInfo : public TDataMonitoringProvisioningInfoBase
	{
public:
    enum 
        { 
        iId = EDataMonitoringSubConnProvisioningInfo
        };

	IMPORT_C TDataMonitoringSubConnProvisioningInfo(TDataVolumes* aDataVolumesPtr, TNotificationThresholds* aThresholdsPtr);

	EXPORT_DATA_VTABLE_AND_FN

private:
	TDataMonitoringSubConnProvisioningInfo();		
	};

/**
	@internalTechnology
*/
class TNotificationRequest
	{
public:
	TNotificationRequest(CDataMonitoringResponder* aResponder, TSubSessionUniqueId aClientId) 
		: iThreshold(0), iResponder(aResponder), iClientId(aClientId), iCancelled(EFalse), iWrapped(EFalse)
		{
		}
	
	TUint32 Threshold() const
		{
		return iThreshold;
		}
		
	void SetThreshold(TUint32 aThreshold)
		{
		iThreshold = aThreshold;
		}
		
	void SetThresholdFromDelta(TUint32 aVolume, TUint32 aDelta);		
	

	CDataMonitoringResponder*& Responder()
		{
		return iResponder;
		}
		
	TSubSessionUniqueId ClientId() const
		{
		return iClientId;
		}
	
	TBool Cancelled() const
		{
		return iCancelled;
		}
		
	void SetCancelled()
		{
	    iCancelled = ETrue;
		}
	
	TBool Wrapped() const
		{
		return iWrapped;
		}

	void SetWrapped(TBool aWrapped)
		{
		iWrapped = aWrapped;
		}

	static TInt CompareThresholds(const TNotificationRequest& aFirst, const TNotificationRequest& aSecond);
	static TBool CompareClientIds(const TNotificationRequest& aFirst, const TNotificationRequest& aSecond);
	static TBool CompareClientIdsAndCancelledFlag(const TNotificationRequest& aFirst, const TNotificationRequest& aSecond);
	
private:
	TUint32 iThreshold;	
	CDataMonitoringResponder* iResponder;
	TSubSessionUniqueId iClientId;
	TBool iCancelled;
	TBool iWrapped;
	};
	
typedef RPointerArray<TNotificationRequest> RNotificationRequestArray;

/**
	@internalTechnology
*/
class ADataMonitoringProvider : public ADataMonitoringProtocolReq
	{
public:
	IMPORT_C virtual ~ADataMonitoringProvider();	
	
	// MExtensionInterfaceBase virtuals
	IMPORT_C virtual void CancelClientExtItfRequests(TSubSessionUniqueId aClientId);
	
	// ADataMonitoringProtocolReq virtuals
	IMPORT_C virtual void RequestDataTransferred(CDataMonitoringResponder*& aResponder, TSubSessionUniqueId aClientId);
	IMPORT_C virtual void CancelDataTransferredRequest(TSubSessionUniqueId aClientId);
	IMPORT_C virtual void RequestDataReceivedNotification(CDataMonitoringResponder*& aResponder, TUint32 aDelta, TUint32 aReceivedBytes, TSubSessionUniqueId aClientId);
	IMPORT_C virtual void CancelDataReceivedNotificationRequest(TSubSessionUniqueId aClientId);
	IMPORT_C virtual void RequestDataSentNotification(CDataMonitoringResponder*& aResponder, TUint32 aDelta, TUint32 aSentBytes, TSubSessionUniqueId aClientId);
	IMPORT_C virtual void CancelDataSentNotificationRequest(TSubSessionUniqueId aClientId);
    	
  	IMPORT_C void DataReceivedNotificationL(TUint32 aNotificationValue);
  	IMPORT_C void DataSentNotificationL(TUint32 aNotificationValue);
  	IMPORT_C void DataNotificationL(const TCFDataMonitoringNotification::TDataMonitoringNotification& aNotification);

	TDataVolumes* DataVolumesPtr() { return &iDataVolumes; }
	TNotificationThresholds* NotificationThresholdsPtr() { return &iThresholds; }
	
protected:
	IMPORT_C ADataMonitoringProvider();
	IMPORT_C void ConstructL();
		
private:
	RNotificationRequestArray* NotificationRequestArray(TDataMonitoringDirection aDirection);
	void InsertWrapMarkerL(TDataMonitoringDirection aDirection);	
	TInt InsertNotificationRequest(TNotificationRequest* aRequest, TDataMonitoringDirection aDirection);
	void SetNextNotificationThreshold(TDataMonitoringDirection aDirection);

private:	
	// Notification request arrays
	RNotificationRequestArray iReceivedNotificationRequests;
	RNotificationRequestArray iSentNotificationRequests;
	
protected:
	// Shared memory
	TDataVolumes iDataVolumes;
	TNotificationThresholds iThresholds;
	};

} //namespace ESock

#endif


