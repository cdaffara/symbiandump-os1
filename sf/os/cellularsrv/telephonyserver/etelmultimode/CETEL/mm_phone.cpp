// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains all the method definitions for RMobilePhone.  It also contains the
// entry point for the Etelmm Dll.
// 
//

/**
 @file
*/

// From core API
#include <etelext.h>

// Multimode header files
#include <etelmm.h>
#include <mmlist.h>
#include "mm_hold.h"


/************************************************************************/
//
//  TMobileAddress
//
/************************************************************************/

EXPORT_C RMobilePhone::TMobileAddress::TMobileAddress() 
	: iTypeOfNumber(EUnknownNumber),
	  iNumberPlan(EUnknownNumberingPlan)
	{
	}

void RMobilePhone::TMobileAddress::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the mobile address from a stream.
 *
 * @param aStream The read stream containing the mobile address.
 */
	{
	iTypeOfNumber=STATIC_CAST(TMobileTON, aStream.ReadUint32L());
	iNumberPlan=STATIC_CAST(TMobileNPI, aStream.ReadUint32L());
	aStream >> iTelNumber;
	}

void RMobilePhone::TMobileAddress::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the mobile address into a stream. 
 *
 * @param aStream The write stream that will contain the mobile address.
 */
	{
	aStream.WriteUint32L(iTypeOfNumber);
	aStream.WriteUint32L(iNumberPlan);
	aStream << iTelNumber;
	}


/************************************************************************/
//
//  RMobilePhone
//
/************************************************************************/

EXPORT_C RMobilePhone::RMobilePhone()
	: iMmPtrHolder(NULL)
/** Constructor. */
	{
	}

EXPORT_C void RMobilePhone::ConstructL()
/** Allocates and Initializes the members of the RMobilePhone object.

@leave KErrNoMemory Out of memory. 
@capability None
*/
	{
	RPhone::ConstructL();
	__ASSERT_ALWAYS(iMmPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iMmPtrHolder = CMobilePhonePtrHolder::NewL(CMobilePhonePtrHolder::EMaxNumberPhonePtrSlots,CMobilePhonePtrHolder::EMaxNumberPhonePtrCSlots);
	}

EXPORT_C void RMobilePhone::Destruct()
/** Destructor. 
@capability None
*/
	{
	RPhone::Destruct();
	delete iMmPtrHolder;
	iMmPtrHolder = NULL;
	}

EXPORT_C void RMobilePhone::InitialiseMM(TRequestStatus& aReqStatus, TDes8& aTableSettings) const
/**
 * This method explicitly initialises the phone.
 *
 * @param aReqStatus returns the result code after the asynchronous call completes.
 * @param aIAP contains the IAP location to use for this phone.
 * @capability NetworkControl
 * @capability WriteDeviceData
*/
	{
	Set(EMobilePhoneMMInitialise, aReqStatus, aTableSettings);
	}

/************************************************************************/
//
//  TMultimodeType
//
/************************************************************************/

RMobilePhone::TMultimodeType::TMultimodeType()
	{}

EXPORT_C TInt RMobilePhone::TMultimodeType::ExtensionId() const
/**
 * This method returns the multimode API extension number of the class
 *
 * @return TInt An integer that will indicate the version of the type
 * @capability None
*/
	{
	return iExtensionId;
	}

void RMobilePhone::TMultimodeType::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the multimode type from a stream 
 *
 * @param aStream The read stream containing the multimode type
 */
	{
	iExtensionId=aStream.ReadInt32L();	
	}

void RMobilePhone::TMultimodeType::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the multimode type into a stream 
 *
 * @param aStream The write stream that will contain the multimode type
 */
	{
	aStream.WriteInt32L(iExtensionId);
	}

/************************************************************************/
//
//  Methods to retrieve basic multimode API information & capabilities
//
/************************************************************************/

EXPORT_C TInt RMobilePhone::GetMultimodeAPIVersion(TInt& aVersion) const
/** Gets the current version of the multimode ETel API.

@param aVersion On return, the current version of the multimode ETel API
@return KErrNone 
@capability None
*/
	{
	aVersion = KEtelExtMultimodeV6;
	return KErrNone;
	}

EXPORT_C TInt RMobilePhone::GetMultimodeCaps(TUint32& aCaps) const
/** Gets the multimode capabilities of the phone.

@param aCaps On return, contain the bit-wise sum of the multimode capabilities 
from TMobilePhoneModeCaps
@return KErrNone 
@capability None
@see RMobilePhone::TMobilePhoneModeCaps
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetMultimodeCaps, ptr1);
	}

EXPORT_C void RMobilePhone::GetPhoneStoreInfo(TRequestStatus& aReqStatus, TDes8& aInfo, const TDesC& aStoreName) const

/** Gets information about a phone store.

The method returns information about the phone store specified by aStoreName. 
Valid values of aStoreName are the various store name literals whose values are defined
as S1, S2, S3... S28.

The phone store information includes the store's name, the type of thing 
it stores, the total number of storage slots and the number of used slots 
if known. It does not give information about which slots are filled and which 
are empty. The information is returned in the aInfo structure. The RMobilePhoneStore::TMobilePhoneStoreInfoV1 
type is defined in more detail in section Common Storage functionality. Valid 
storage specific aInfo types are listed in sections SMS Message Storage (RMobileSmsStore) 
to Own Number Storage (RMobileONStore).

@param aReqStatus On return, KErrNone if successful, an error code if not.
@param aInfo On completion, a descriptor of type 
RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg, or one of the 
TMobilePhoneStoreInfoV1 derived classes' package types, that contains the phone 
store information.
@param aStoreName The name of the store, for which information is required
@see RMobilePhoneStore::TMobilePhoneStoreInfoV1 
@capability ReadDeviceData
*/
// Note for TSY developers: The RMobilePhone::TPhoneStoreNameAndIccType structure 
// is used internally (API/TSY) to package the iStoreName and iIccType and pass 
// them to the TSY.
	{
	SetAndGet(EMobilePhoneGetPhoneStoreInfo,aReqStatus, aInfo, aStoreName);
	}

EXPORT_C void RMobilePhone::GetPhoneStoreInfo(TRequestStatus& aReqStatus, TDes8& aInfo, const TDesC& aStoreName, const TDesC& aMode) const
/** This method returns the information related to a particular phone store on 
	a specific Icc.
	
The method returns information about the phone store specified by aStoreName 
and aMode. Valid values of aStoreName are the various store name literals whose values 
are defined as "S1", "S2", "S3"... "S28". Valid values of aMode are specified 
in the EtelMM API Design Document under the section "Phone Book Storage 
(RMobilePhoneBookStore)"

The phone store information includes the store’s name, the type of thing 
it stores, the total number of storage slots and the number of used slots 
if known. It does not give information about which slots are filled and which 
are empty. The information is returned in the aInfo structure. The RMobilePhoneStore::TMobilePhoneStoreInfoV1 
type is defined in more detail in section Common Storage functionality. Valid 
storage specific aInfo types are listed in sections SMS Message Storage (RMobileSmsStore) 
to Own Number Storage (RMobileONStore).

Note for TSY developers: The RMobilePhone::TPhoneStoreNameAndIccType structure 
is used internally (API/TSY) to package the iStoreName and iIccType and pass 
them to the TSY.

@param aStatus On return, KErrNone if successful, an error code if not.
@param aInfo On completion, a descriptor of type 
RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg, or one of the 
TMobilePhoneStoreInfoV1 derived classes' package types, that contains the phone 
store information.
@param aStoreName Specifies the name of the store, for which information is 
required.
@param aMode Specifies the type of the ICC that the phonebook belongs to.
@see RMobilePhoneStore::TMobilePhoneStoreInfoV1
@capability ReadDeviceData
*/
	{
	
	iMmPtrHolder->iPhoneStoreNameAndIccType.iStoreName = aStoreName;
	iMmPtrHolder->iPhoneStoreNameAndIccType.iMode = aMode;

	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetPhoneStoreInfo, iMmPtrHolder->iPhoneStoreNameAndIccType);


	SetAndGet(EMobilePhoneGetPhoneStoreInfo,aReqStatus, ptr1, aInfo);
	}

/***********************************************************************************/
//
// MobileIccAccess functional unit
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetIccAccessCaps(TUint32& aCaps) const

/** This function member gets the current ICC access capabilities of the phone.

ICC access, as meant by this API, consists of any read/write or command transaction 
that the phone may execute with the ICC. ICC access is always initiated by 
the phone (even for SIM toolkit), however in a multimode phone it may not 
be possible to access the ICC at all times - perhaps only when the phone 
is in a cellular mode that includes a ICC as part of its standard.

@param aCaps On completion, the sum of RMobilePhone::TMobilePhoneIccCaps
constants that is currently supported by the TSY.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not
support any ICC access.
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetIccAccessCaps, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyIccAccessCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const

/** This function member notifies the client if the current ICC access capabilities 
change, probably due to a change of network mode.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyIccAccessCapsChange) 
to cancel a previously placed asynchronous NotifyIccAccessCapsChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aCaps On completion, the sum of RMobilePhone::TMobilePhoneIccCaps
constants that is currently supported by the TSY.
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyIccAccessCapsChange,aCaps);

	Get(EMobilePhoneNotifyIccAccessCapsChange,aReqStatus,ptr1);
	}
	
EXPORT_C void RMobilePhone::GetIccIdentity(TRequestStatus& aReqStatus, TIccIdentity& aIdentity) const

/** 
Gets the ICC identity. 

This is the unique identifier for the ICC as defined in EF_ICCID.

@param aReqStatus On return, KErrNone if successful.
param aIdentity TIccIdentity buffer which will contain the ICC ID on completion.
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetIccIdentity, aIdentity);

	Get(EMobilePhoneGetIccIdentity,aReqStatus, ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneCspFileV1::TMobilePhoneCspFileV1()
	: iCallOfferingServices(0),iCallRestrictionServices(0),iOtherSuppServices(0),
	iCallCompletionServices(0),iTeleservices(0),iCphsTeleservices(0),iCphsFeatures(0),
	iNumberIdentServices(0),iPhase2PlusServices(0),iValueAddedServices(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::GetCustomerServiceProfile(TRequestStatus& aReqStatus, TDes8& aCsp) const

/** Gets the whole Customer Service Profile file that is stored on the SIM to the 
client.

Use: RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetCustomerServiceProfile) 
to cancel a previously placed asynchronous GetCustomerServiceProfile()() request.

@param aReqStatus On return, KErrNone if successful, KErrMMETelWrongMode if 
the phone does not support ICC access during the current mode, KErrNotFound 
if there is no SIM inserted or the CSP file does not exist on the current 
SIM, KErrAccessDenied if the CSP read access condition has not been fulfilled 
(i.e. PIN1 has not been verified).
@param aCsp On completion, a TMobilePhoneCspFileV1Pckg with the current Service 
Profile.
@see TMobilePhoneCspFileV1 
@capability None
*/
	{
	Get(EMobilePhoneGetCustomerServiceProfile, aReqStatus, aCsp);
	}

EXPORT_C RMobilePhone::TMobilePhoneServiceTableV1::TMobilePhoneServiceTableV1()
	: iServices1To8(0),iServices9To16(0),iServices17To24(0),iServices25To32(0),
	iServices33To40(0), iServices41To48(0), iServices49To56(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

/**
Default constructor.

Initialises iExtensionId to KETelExtMultimodeV2.
*/
EXPORT_C RMobilePhone::TMobilePhoneServiceTableV2::TMobilePhoneServiceTableV2()
	: TMobilePhoneServiceTableV1()
	{
	iExtensionId=KETelExtMultimodeV2;
	}

/**
Default constructor.

Initialises iExtensionId to KETelExtMultimodeV8.
*/
EXPORT_C RMobilePhone::TMobilePhoneServiceTableV8::TMobilePhoneServiceTableV8()
	: TMobilePhoneServiceTableV2(),iServices57To64(0),iServices65To72(0)
	{
	iExtensionId=KEtelExtMultimodeV8;
	}


EXPORT_C void RMobilePhone::GetServiceTable(TRequestStatus& aReqStatus, TMobilePhoneServiceTable aTable, TDes8& aSst) const
 /** Gets the whole contents of a service table file (SIM or CDMA) that is stored on the ICC to 
	the client.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetServiceTable) to 
	cancel a previously placed asynchronous GetServiceTable()() request.
	
	@param aReqStatus On return, KErrNone if successful, KErrMMETelWrongMode if 
	the phone does not support ICC access during the current mode, KErrNotFound 
	if there is no ICC inserted or the specified service table file does not exist 
	on the current ICC, KErrAccessDenied if the service table's read access condition 
	has not been fulfilled (i.e. PIN1 has not been verified).
	@param aTable Specifies whether the SIM or CDMA service table is to be retrieved
	@param aSst On completion, a TMobilePhoneServiceTableV1Pckg containing 
	the service table.
	@see TMobilePhoneServiceTableV1 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iServiceTable = aTable;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetServiceTable, iMmPtrHolder->iServiceTable);

	SetAndGet(EMobilePhoneGetServiceTable, aReqStatus, ptr1, aSst);
	}

/***********************************************************************************/
//
// MobilePower functional unit
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetBatteryCaps(TUint32& aCaps) const
/** Gets the battery information capabilities of the phone.

@param aCaps On return, the bit-wise sum of RMobilePhone::TMobilePhoneBatteryCaps
constants.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not support 
any battery functionality.
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetBatteryCaps, ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneBatteryInfoV1::TMobilePhoneBatteryInfoV1() :
		iStatus(EPowerStatusUnknown), iChargeLevel(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::GetBatteryInfo(TRequestStatus& aReqStatus, TMobilePhoneBatteryInfoV1& aInfo) const

/** This function member returns a snapshot of the current battery information 
of the phone.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetBatteryInfo) to 
cancel a previously placed asynchronous GetBatteryInfo() request.

@param aReqStatus On return, KErrNone if successful.
@param aInfo On completion, the mobile phone battery status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetBatteryInfo,aInfo);

	Get(EMobilePhoneGetBatteryInfo,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyBatteryInfoChange(TRequestStatus& aReqStatus, TMobilePhoneBatteryInfoV1& aInfo) const

/** This function member posts an asynchronous notification request.

The notification will trigger when the phone's battery information changes 
in any way.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyBatteryInfoChange) 
to cancel a previously placed asynchronous NotifyBatteryInfoChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aInfo On completion, the mobile phone battery status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyBatteryInfoChange,aInfo);

	Get(EMobilePhoneNotifyBatteryInfoChange,aReqStatus,ptr1);
	}

/***********************************************************************************/
//
// MobileSignal functional unit
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetSignalCaps(TUint32& aCaps) const

/** This function member gets the signal strength capabilities of the phone.

@param aCaps On return, the sum of RMobilePhone::TMobilePhoneSignalCaps
constants that this phone supports.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not
support any signal strength functionality.
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetSignalCaps, ptr1);
	}

EXPORT_C void RMobilePhone::GetSignalStrength(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) const

/** This function member gets the phone's current signal strength.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetSignalStrength) 
to cancel a previously placed asynchronous GetSignalStrength() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if signal 
strength information is not supported by the phone, and KErrNotFound if no 
signal strength information is currently available.
@param aSignalStrength On completion, the signal strength in dBm, or 0 if the 
TSY does not wish to provide this information.
@param aBar On completion, the number of bars the phone should display, or 
-1 if the TSY does not wish to provide this information.
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetSignalStrength,aSignalStrength);
	TPtr8& ptr2=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2GetSignalStrength,aBar);

	Get(EMobilePhoneGetSignalStrength,aReqStatus,ptr1,ptr2);
	}

EXPORT_C void RMobilePhone::NotifySignalStrengthChange(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) const

/** This function member posts an asynchronous notification request. The notification 
will trigger when the phone's signal strength changes.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange) 
to cancel a previously placed asynchronous NotifySignalStrengthChange() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if signal 
strength information is not supported by the phone, and KErrNotFound if no 
signal strength information is currently available.
@param aSignalStrength On completion, the signal strength in dBm, or 0 if the 
TSY does not wish to provide this information.
@param aBar On completion, the number of bars the phone should display, or 
-1 if the TSY does not wish to provide this information. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifySignalStrengthChange,aSignalStrength);
	TPtr8& ptr2=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2NotifySignalStrengthChange,aBar);

	Get(EMobilePhoneNotifySignalStrengthChange, aReqStatus, ptr1, ptr2);
	}

/***********************************************************************************/
//
// MobileIndicator functional unit
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetIndicatorCaps(TUint32& aActionCaps, TUint32& aIndCaps) const

/** This function member gets the indicator capabilities of the phone.

@param aActionCaps On completion, the sum of RMobilePhone::TMobilePhoneIndicatorCaps
constants which indicate the API function members that are supported by t
he TSY.
@param aIndCaps On completion, the sum of RMobilePhone::TMobilePhoneIndicators
constants that are supported by the TSY.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not
support any indicator functionality.
@capability None
*/
	{
	TPckg<TUint32> ptr1(aActionCaps);
	TPckg<TUint32> ptr2(aIndCaps);
	return Get(EMobilePhoneGetIndicatorCaps, ptr1, ptr2);
	}

EXPORT_C void RMobilePhone::GetIndicator(TRequestStatus& aReqStatus, TUint32& aIndicator) const

/** This function member gets the phone's current values for all supported indicators.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetIndicator) to cancel 
a previously placed asynchronous GetIndicator() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if indicator 
information is not supported by the phone, KErrNotFound if no indicator information 
is currently available.
@param aIndicator On completion, the sum of the TMobilePhoneIndicators flags. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetIndicator,aIndicator);

	Get(EMobilePhoneGetIndicator,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyIndicatorChange(TRequestStatus& aReqStatus, TUint32& aIndicator) const

/** This function member posts an asynchronous notification request

The notification will trigger when one or more of the phone's supported indicators 
changes. When the TSY completes the notification request, it will return the 
new set of indicator values within the aIndicator parameter.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyIndicatorChange) 
to cancel a previously placed asynchronous NotifyIndicatorChange() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if indicator 
information is not supported by the phone, KErrNotFound if no indicator information 
is currently available.
@param aIndicator On completion, the sum of the TMobilePhoneIndicators. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyIndicatorChange,aIndicator);

	Get(EMobilePhoneNotifyIndicatorChange, aReqStatus, ptr1);
	}

/***********************************************************************************/
//
// MobileIdentity functional unit
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetIdentityCaps(TUint32& aCaps) const

/** This function member gets the identity information capabilities of the phone.

@param aCaps On return, the sum of RMobilePhone::TMobilePhoneIdentityCaps
constants.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not
support any identity information functionality.
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetIdentityCaps, ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneIdentityV1::TMobilePhoneIdentityV1()
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::GetPhoneId(TRequestStatus& aReqStatus, TMobilePhoneIdentityV1& aId) const

/** This function member gets the phone's identity as described by its manufacturer, 
model, revision and serial number. The TSY may not support all these elements
of a phone's identity and a client can discover which ones are available using 
the GetIdentityCaps() request.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetPhoneId) to cancel 
a previously placed asynchronous GetPhoneId() request.

@param aReqStatus On return, KErrNone if successful.
@param aId On completion, the mobile phone identity. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetPhoneId,aId);

	Get(EMobilePhoneGetPhoneId, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::GetSubscriberId(TRequestStatus& aReqStatus, TMobilePhoneSubscriberId& aId) const

/** This function member returns the subscriber's identity as described by its 
IMSI.

The TSY may not support client retrieval of the IMSI and a client can discover 
whether it is available using the GetIdentityCaps() request.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetSubscriberId) to 
cancel a previously placed asynchronous GetSubscriberId() request.

@param aReqStatus On return, KErrNone if successful.
@param aId On completion, the mobile phone subscriber id. 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetSubscriberId,aId);

	Get(EMobilePhoneGetSubscriberId, aReqStatus, ptr1);
	}

/***********************************************************************************/
//
// MobileDTMF functional unit
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetDTMFCaps(TUint32& aCaps) const

/** This function member gets the current instance of the dynamic DTMF capabilities 
of the phone.

DTMF tones can only be sent or received during an active voice call so the 
TSY can reflect when DTMF is possible by changing the dynamic DTMF capabilities 
as calls start and end.

@param aCaps On return, the sum of RMobilePhone::TMobilePhoneDTMFCaps
constants.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not
support any DTMF functionality.
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetDTMFCaps, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyDTMFCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const

/** This function member notifies the clients of changes to the dynamic DTMF capabilities 
of the phone.

The DTMF capabilities are most likely to change when the phone starts and 
ends voice calls.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyDTMFCapsChange) 
to cancel a previously placed asynchronous NotifyDTMFCapsChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aCaps On completion, the sum of RMobilePhone::TMobilePhoneDTMFCaps
constants.
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyDTMFCapsChange, aCaps);

	Get(EMobilePhoneNotifyDTMFCapsChange, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::SendDTMFTones(TRequestStatus& aReqStatus, const TDesC& aTones) const

/** Allows a client to transmit DTMF tones across all the current active voice 
calls

The request completes when the tones have been sent. It is the responsibility 
of the TSY or phone to check that the characters in the aTones parameter are 
valid DTMF characters for the serving network.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSendDTMFTones) to cancel 
a previously placed asynchronous SendDTMFTones() request.

@param aReqStatus On return, KErrNone if successful, KErrArgument if the specified
string contains illegal DTMF characters, KErrEtelCallNotActive if no voice 
calls are in the active and connected state.
@param aTones The string of valid DTMF characters to send. Can contain one 
or more DTMF tone(s). In GSM mode valid DTMF tone characters are contained 
in the set 0-9, A,B,C,D, #,*, p, w 
@capability NetworkServices
*/
	{
	Set(EMobilePhoneSendDTMFTones, aReqStatus, aTones);
	}

EXPORT_C void RMobilePhone::ReadDTMFTones(TRequestStatus& aReqStatus, TDes& aTones) const
/**
 * This method reads a series of DTMF tones from a connected and active voice call
 *
 * @param aReqStatus returns the result code after the asynchronous call completes
 * @param aTones Will contain the tones that have been read
 
@capability NetworkServices
*/
	{
	Get(EMobilePhoneReadDTMFTones, aReqStatus, aTones);
	}

EXPORT_C TInt RMobilePhone::StartDTMFTone(TChar aTone) const

/** Starts the transmission of a single DTMF tone across a connected 
and active voice call. 

This corresponds to the depression of a key on the pad of the ME.

@param aTone The DTMF tone to send.
@return KErrNone if successful 
@capability NetworkServices
*/
	{
	TPckgC<TChar> ptr1(aTone);
	return Set(EMobilePhoneStartDTMFTone,ptr1);
	}

EXPORT_C TInt RMobilePhone::StopDTMFTone() const

/** Stops the transmission of a single DTMF tone, which had previously 
been started by use of the StartDTMFTone function member, across a 
connected and active voice call.

This corresponds to the release of a key on the pad of the ME.

@return KErrNone if successful 
@capability NetworkServices
*/
	{
	return Blank(EMobilePhoneStopDTMFTone);
	}

EXPORT_C void RMobilePhone::NotifyStopInDTMFString(TRequestStatus& aRequestStatus) const

/** This function member allows a client to be notified when the phone has encountered 
a 'w' char during the sending of a DTMF string.

The sending would have been previously started by use of the SendDTMFTones 
function member. The 'w' char stops the DTMF string sending and gives control 
back to the client so that it can decide to continue or end the sending. For 
example, string "p123w555" means that the phone should first wait for 2.5 
seconds before starting to send the DTMF string (the 'p' char) and then send 
tones 123. After that, the phone should stop the sending of DTMF tones and 
give control back to the client ('w' char). The client can then decide to 
continue to send the rest of the tones 555 or discard them and stop the sending
entirely. 

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyStopInDTMFString) 
to cancel a previously placed asynchronous NotifyStopInDTMFString() request.

@param aRequestStatus On return, KErrNone if successful. 
@capability NetworkServices
*/
	{
	Blank(EMobilePhoneNotifyStopInDTMFString,aRequestStatus);
	}

EXPORT_C TInt RMobilePhone::ContinueDTMFStringSending(TBool aContinue) const

/** Continue or cancels the sending of a DTMF string when it 
was stopped by the use of 'w' char in the string.

@param aContinue ETrue if sending of the DTMF string should continue, EFalse 
if the rest of the DTMF string is to be discarded. 
@return KErrNone if successful. 
@capability NetworkServices
*/
	{
	TPckg<TBool> ptr1(aContinue);
	return Set(EMobilePhoneContinueDTMFStringSending,ptr1);
	}
	
 EXPORT_C void RMobilePhone::NotifyDTMFEvent(TRequestStatus& aReqStatus, TMobilePhoneDTMFEvent& aEvent) const
 /**
 Notifies the client of dtmf tone events. These events are generated when a Start/Stop Dtmf 
 operation has been acknowledged from the MSC and the tone has been played remotely.
 
 @param aReqStatus returns the result code after the asynchronous call completes.
 @param aEvent On completion contains the DTMF event.
 
@capability None
*/
 	{
 	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyDTMFEvent, aEvent);

	Get(EMobilePhoneNotifyDTMFEvent, aReqStatus, ptr1);
 	}

/***********************************************************************************/
//
// MobileNetwork functional unit
//
/***********************************************************************************/

EXPORT_C RMobilePhone::TMobilePhoneNetworkInfoV1::TMobilePhoneNetworkInfoV1()
:	iMode(ENetworkModeUnknown),
	iStatus(ENetworkStatusUnknown),
	iBandInfo(EBandUnknown)
	{
	iExtensionId=KETelExtMultimodeV1;
	}
/**
* This is the initialization of the TMobilePhoneNetworkInfoV2 structured type
*
*/
EXPORT_C RMobilePhone::TMobilePhoneNetworkInfoV2::TMobilePhoneNetworkInfoV2()
:	TMobilePhoneNetworkInfoV1(), iAccess(ENetworkAccessUnknown)
	{
	iExtensionId=KETelExtMultimodeV2;
	}

/**
* This is the initialization of the TMobilePhoneNetworkInfoV5 structured type
*
*/
EXPORT_C RMobilePhone::TMobilePhoneNetworkInfoV5::TMobilePhoneNetworkInfoV5()
:	TMobilePhoneNetworkInfoV2(),
	iHsdpaAvailableIndicator(EFalse),
	iEgprsAvailableIndicator(EFalse)
	{
	iExtensionId=KEtelExtMultimodeV5;
	}

/**
* This is the initialization of the TMobilePhoneNetworkInfoV8 structured type
*
*/
EXPORT_C RMobilePhone::TMobilePhoneNetworkInfoV8::TMobilePhoneNetworkInfoV8()
:	TMobilePhoneNetworkInfoV5(),
	iHsupaAvailableIndicator(EFalse)
	{
	iExtensionId=KEtelExtMultimodeV8;
	}	
	
void RMobilePhone::TMobilePhoneNetworkInfoV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the mobile network information from a stream 
 *
 * @param aStream The read stream containing the mobile network information
 */
	{
	TMultimodeType::InternalizeL(aStream);
	iMode=STATIC_CAST(TMobilePhoneNetworkMode, aStream.ReadUint32L());
	iStatus=STATIC_CAST(TMobilePhoneNetworkStatus, aStream.ReadUint32L());
	iBandInfo=STATIC_CAST(TMobilePhoneNetworkBandInfo, aStream.ReadUint32L());
	aStream >> iCountryCode;
	aStream >> iCdmaSID;
	aStream >> iAnalogSID;
	aStream >> iNetworkId;
	aStream >> iDisplayTag;
	aStream >> iShortName;
	aStream >> iLongName;
	}

void RMobilePhone::TMobilePhoneNetworkInfoV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the mobile network information into a stream 
 *
 * @param aStream The write stream that will contain the mobile network information
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream.WriteUint32L(iMode);
	aStream.WriteUint32L(iStatus);
	aStream.WriteUint32L(iBandInfo);
	aStream << iCountryCode;
	aStream << iCdmaSID;
	aStream << iAnalogSID;
	aStream << iNetworkId;
	aStream << iDisplayTag;
	aStream << iShortName;
	aStream << iLongName;
	}

void RMobilePhone::TMobilePhoneNetworkInfoV2::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the mobile network information from a stream 
 *
 * @param aStream The read stream containing the mobile network information
 */
	{
	TMobilePhoneNetworkInfoV1::InternalizeL(aStream);
	iAccess = STATIC_CAST(TMobilePhoneNetworkAccess, aStream.ReadUint32L());
	}

void RMobilePhone::TMobilePhoneNetworkInfoV2::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the mobile network information into a stream 
 *
 * @param aStream The write stream that will contain the mobile network information
 */
	{
	TMobilePhoneNetworkInfoV1::ExternalizeL(aStream);
	aStream.WriteUint32L(iAccess);
	}

void RMobilePhone::TMobilePhoneNetworkInfoV5::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the mobile network information from a stream 
 *
 * @param aStream The read stream containing the mobile network information
 */
	{
	TMobilePhoneNetworkInfoV2::InternalizeL(aStream);
	iHsdpaAvailableIndicator = aStream.ReadUint32L();
	iEgprsAvailableIndicator = aStream.ReadUint32L();
	}

void RMobilePhone::TMobilePhoneNetworkInfoV5::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the mobile network information into a stream 
 *
 * @param aStream The write stream that will contain the mobile network information
 */
	{
	TMobilePhoneNetworkInfoV2::ExternalizeL(aStream);
	aStream.WriteUint32L(iHsdpaAvailableIndicator);
	aStream.WriteUint32L(iEgprsAvailableIndicator);
	}

void RMobilePhone::TMobilePhoneNetworkInfoV8::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the mobile network information from a stream 
 *
 * @param aStream The read stream containing the mobile network information
 */
	{
	TMobilePhoneNetworkInfoV5::InternalizeL(aStream);
	iHsupaAvailableIndicator = aStream.ReadUint32L();
	}

void RMobilePhone::TMobilePhoneNetworkInfoV8::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the mobile network information into a stream 
 *
 * @param aStream The write stream that will contain the mobile network information
 */
	{
	TMobilePhoneNetworkInfoV5::ExternalizeL(aStream);
	aStream.WriteUint32L(iHsupaAvailableIndicator);
	}
	
EXPORT_C TInt RMobilePhone::GetNetworkCaps(TUint32& aCaps) const

/** This function member gets the static network capabilities of the phone.

@param aCaps On return, is the sum of RMobilePhone::TMobilePhoneNetworkCaps
constants.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not
support any client access to network functionality.
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetNetworkCaps, ptr1);
	}

EXPORT_C TInt RMobilePhone::GetCurrentMode(TMobilePhoneNetworkMode& aNetworkMode) const

/** This function member gets information about the current network mode.

@param aNetworkMode On return, the network mode, e.g. GSM, WCDMA, CDMA, TDMA, AMPS.
@return KErrNone if successful, an error code if not. 
@capability None
*/
	{
	TPckg<TMobilePhoneNetworkMode> ptr1(aNetworkMode);
	return Get(EMobilePhoneGetCurrentMode, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyModeChange(TRequestStatus& aReqStatus, TMobilePhoneNetworkMode& aNetworkMode) const

/** This function member allows the client to be notified of changes to the current 
network mode.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyModeChange) to 
cancel a previously placed asynchronous NotifyModeChange() request.

@param aReqStatus On return, KErrNone if successful
@param aNetworkMode On completion, the new network mode, e.g. GSM, WCDMA, CDMA, TDMA, AMPS 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyModeChange, aNetworkMode);

	Get(EMobilePhoneNotifyModeChange,aReqStatus, ptr1);
	}

/**
This function member provides the client with information on the home network 
of this subscriber.

This allows the client to compare detected and/or current network information 
with the home network information in order to determine if the current or 
detected networks are also the home network.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetHomeNetwork) to 
cancel a previously placed asynchronous GetHomeNetwork() request.

@param aReqStatus On return, KErrNone if successful.
@param aNetworkInfo On completion, the home network information.  The data returned is dependent on the supported versions from the TSY. 
This parameter should be a packaged RMobilePhone::TMobilePhoneNetworkInfoV1 (or derived class).
@see TMobilePhoneNetworkInfoV1 , TMobilePhoneNetworkV2, TMobilePhoneNetworkV5
@capability None
*/
EXPORT_C void RMobilePhone::GetHomeNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const
	{
	Get(EMobilePhoneGetHomeNetwork, aReqStatus, aNetworkInfo);
	}
	
EXPORT_C void RMobilePhone::GetHomeNetworkSearchPeriod(TRequestStatus& aReqStatus, TInt& aSearchIntv) const
/**
Provides the client with the time interval used between two searches for detecting the home network. 
The interval is returned to the client via the aSearchIntv parameter as minutes. 
A value of '0' indicates that no attempts are made to search for a home network.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aSearchIntv On completon contains the search interval in minutes.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetHomeNetworkSearchPeriod, aSearchIntv);

	Get(EMobilePhoneGetHomeNetworkSearchPeriod,aReqStatus, ptr1);	
	}
	
EXPORT_C void RMobilePhone::GetLastUsedAccessTechnology(TRequestStatus& aReqStatus, TMobilePhoneNetworkAccess& aAccessTech) const
/**
Provides the client with the last used access technology with the currently registered mobile network operator (PLMN).

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aAccessTech On completon contains the access technology.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetLastUsedAccessTechnology, aAccessTech);

	Get(EMobilePhoneGetLastUsedAccessTechnology,aReqStatus, ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneLocationAreaV1::TMobilePhoneLocationAreaV1() 
:	iAreaKnown(EFalse), iLocationAreaCode(0), iCellId(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::GetCurrentNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo, TMobilePhoneLocationAreaV1& aArea) const

/** This function member gets information about the network that the phone is currently 
registered to.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetCurrentNetwork) 
to cancel a previously placed asynchronous GetCurrentNetwork() request.

@param aReqStatus On return, KErrNone if successful.
@param aNetworkInfo On completion, contains the network information.
The data returned is dependent on the supported versions from the TSY.
This parameter should be a packaged RMobilePhone::TMobilePhoneNetworkInfoV1 (or derived class).

@param aArea On completion, information on the current location of the phone 
(only for GSM/WCDMA mode).
@see TMobilePhoneNetworkInfoV1 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr2=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetCurrentNetwork, aArea);

	Get(EMobilePhoneGetCurrentNetwork, aReqStatus, aNetworkInfo, ptr2);
	}
	
EXPORT_C void RMobilePhone::GetCurrentNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const

/** This overloaded function is a less sensitive version of RMobilePhone::GetCurrentNetwork(TRequestStatus&, TDes8&, TMobilePhoneLocationAreaV1&). 
It has been created for use by application that do not have the required capabilities to call the other version of GetCurrentNetwork which contains location information.

The function member gets information about the network that the phone is currently 
registered to.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetCurrentNetworkNoLocation) 
to cancel a previously placed asynchronous GetCurrentNetwork() request.

@param aReqStatus On return, KErrNone if successful.
@param aNetworkInfo On completion, contains the network information.
The data returned is dependent on the supported versions from the TSY.
This parameter should be a packaged RMobilePhone::TMobilePhoneNetworkInfoV2 (or derived class).
 
@see TMobilePhoneNetworkInfoV2 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	Get(EMobilePhoneGetCurrentNetworkNoLocation, aReqStatus, aNetworkInfo);	
	}	

EXPORT_C void RMobilePhone::NotifyCurrentNetworkChange(TRequestStatus& aReqStatus, TDes8& aNetworkInfo, TMobilePhoneLocationAreaV1& aArea) const

/** This function member allows the client to be notified of changes to the current 
network.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange) 
to cancel a previously placed asynchronous NotifyCurrentNetworkChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aNetworkInfo On completion, contains the network information.
The data returned is dependent on the supported versions from the TSY.
This parameter should be a packaged RMobilePhone::TMobilePhoneNetworkInfoV1 (or derived class).
 
@param aArea On completion, information on the current location of the phone 
(only for GSM/WCDMA mode).
@see TMobilePhoneNetworkInfoV1 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr2=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyCurrentNetworkChange, aArea);

	Get(EMobilePhoneNotifyCurrentNetworkChange, aReqStatus, aNetworkInfo, ptr2);
	}

EXPORT_C void RMobilePhone::NotifyCurrentNetworkChange(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const

/** This function is a less sensitive version of RMobilePhone::NotifyCurrentNetworkChange(TRequestStatus& aReqStatus, TDes8& aNetworkInfo, TMobilePhoneLocationAreaV1& aArea). 
It has been created for use by application that do not have the required capabilities to call the other version of NotifyCurrentNetwork which contains location information.

The function member allows the client to be notified of changes to the current 
network.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange) 
to cancel a previously placed asynchronous NotifyCurrentNetworkChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aNetworkInfo On completion, contains the network information.
The data returned is dependent on the supported versions from the TSY.
This parameter should be a packaged RMobilePhone::TMobilePhoneNetworkInfoV2 (or derived class).

@see TMobilePhoneNetworkInfoV2 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	Get(EMobilePhoneNotifyCurrentNetworkNoLocationChange, aReqStatus, aNetworkInfo);
	}	

/** Default constructor

@publishedPartner 
@released 
*/
EXPORT_C RMobilePhone::TMobilePhoneCellInfoV9::TMobilePhoneCellInfoV9()
	: iMode(ECellModeUnknown), iCellId(0), iTimingAdvance(-1), iStatus(ECellInfoStatusUnknown)
	{
	iExtensionId = KEtelExtMultimodeV9;
	}

/** This function gets information about the cell that the device is connected to.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetCellInfo) to cancel a 
previously placed asynchronous GetCellInfo() request.

@param aReqStatus On return, KErrNone if successful.
@param aCellInfo On completion, the cell information packaged in a TMobilePhoneCellInfoV9Pckg.
@see TMobilePhoneCellInfoV9
@see TMobilePhoneCellInfoV9Pckg

@capability ReadDeviceData

@publishedPartner
@released
*/
EXPORT_C void RMobilePhone::GetCellInfo(TRequestStatus& aReqStatus, TDes8& aCellInfo) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	Get(EMobilePhoneGetCellInfo, aReqStatus, aCellInfo);	
	}	

/** This function member allows the client to be notified of changes to the current 
network.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCellInfoChange) to cancel a 
previously placed asynchronous NotifyCellInfoChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aCellInfo On completion, the cell information packaged in a TMobilePhoneCellInfoV9Pckg.
@see TMobilePhoneCellInfoV9
@see TMobilePhoneCellInfoV9Pckg

@capability ReadDeviceData

@publishedPartner
@released
*/
EXPORT_C void RMobilePhone::NotifyCellInfoChange(TRequestStatus& aReqStatus, TDes8& aCellInfo) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	Get(EMobilePhoneNotifyCellInfoChange, aReqStatus, aCellInfo);
	}	

EXPORT_C void RMobilePhone::GetNetworkRegistrationStatus(TRequestStatus& aReqStatus, TMobilePhoneRegistrationStatus& aStatus) const

/** This function member gets information about the current network registration 
status.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetNetworkRegistrationStatus) 
to cancel a previously placed asynchronous GetNetworkRegistrationStatus() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aStatus On completion, the registration status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetNetworkRegistrationStatus, aStatus);

	Get(EMobilePhoneGetNetworkRegistrationStatus, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyNetworkRegistrationStatusChange(TRequestStatus& aReqStatus, TMobilePhoneRegistrationStatus& aStatus) const

/** This function member allows the client to be notified of changes to the current 
network registration status.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange) 
to cancel a previously placed asynchronous NotifyNetworkRegistrationStatusChange() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aStatus On completion, the new registration status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyNetworkRegistrationStatusChange, aStatus);

	Get(EMobilePhoneNotifyNetworkRegistrationStatusChange, aReqStatus, ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneNetworkSelectionV1::TMobilePhoneNetworkSelectionV1()
	: iMethod(ENetworkSelectionUnknown), 
	  iBandClass(ENetworkBandClassUnknown), 
	  iOperationMode(ENetworkOperationUnknown)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C TInt RMobilePhone::GetNetworkSelectionSetting(TDes8& aSetting) const

/** This function member gets the current value of the network selection setting.

@param aSetting On return, a TMobilePhoneNetworkSelectionV1Pckg class holding 
the network selection settings.
@return KErrNone
@see TMobilePhoneNetworkSelectionV1 
@capability ReadDeviceData
*/
	{
	return Get(EMobilePhoneGetNetworkSelectionSetting, aSetting);
	}

EXPORT_C void RMobilePhone::SetNetworkSelectionSetting(TRequestStatus& aReqStatus, const TDes8& aSetting) const

/** This function member is used by client application to set the way a phone will 
select a network

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetNetworkSelectionSetting) 
to cancel a previously placed asynchronous SetNetworkSelectionSetting() request.

@param aReqStatus On return, KErrNone if successful.
@param aSetting A TMobilePhoneNetworkSelectionV1Pckg holding the new network 
selection settings.
@see TMobilePhoneNetworkSelectionV1 
@capability WriteDeviceData
*/
	{
	Set(EMobilePhoneSetNetworkSelectionSetting,aReqStatus,aSetting);
	}

EXPORT_C void RMobilePhone::NotifyNetworkSelectionSettingChange(TRequestStatus& aReqStatus, TDes8& aSetting) const

/** This function member allows the client to be notified of changes to the network 
selection setting.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange) 
to cancel a previously placed asynchronous NotifyNetworkSelectionSettingChange() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aSetting On completion, a TMobilePhoneNetworkSelectionV1Pckg class holding 
the new network selection settings.
@see TMobilePhoneNetworkSelectionV1 
@capability ReadDeviceData
*/
	{
	Get(EMobilePhoneNotifyNetworkSelectionSettingChange, aReqStatus, aSetting);
	}

EXPORT_C void RMobilePhone::SelectNetwork(TRequestStatus& aReqStatus, TBool aIsManual, const TMobilePhoneNetworkManualSelection& aManualSelection) const

/** This function member allows the client to instruct the phone to attempt network 
re-selection.

If the client has chosen manual network selection, then it should have previously 
asked the user which network to manually select. The chosen network should 
be included in the detected networks list, i.e. the list that is received 
by using the CRetrieveMobilePhoneDetectedNetworks class.

It is highly recommended to search detected networks just before trying to 
select a new network. It is possible that the client has an old "out-of-date" 
network list, that does not correspond to the present detected networks. Thus 
the user could select a network that is no longer available.

If the client has chosen automatic network selection, then the phone will 
choose the new network based upon its automatic network selection criteria.

It is only possible to manually select a network in GSM or WCDMA mode.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSelectNetwork) to cancel 
a previously placed asynchronous SelectNetwork() request.

@param aReqStatus On return, KErrNone if successful.
@param aIsManual ETrue if the network selection is to be done manually, EFalse 
if the network selection is to be done automatically.
@param aManualSelection If manual selection is requested, the network to select. 
This parameter is ignored when automatic network selection is requested. 
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iIsManual = aIsManual;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SelectNetwork, iMmPtrHolder->iIsManual);
	TPtrC8& ptr2=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SelectNetwork, aManualSelection);

	Set(EMobilePhoneSelectNetwork,aReqStatus, ptr1, ptr2);
	}

EXPORT_C RMobilePhone::TMobilePhoneNITZ::TMobilePhoneNITZ() 
	: iNitzFieldsUsed(0), iTimeZone(0), iDST(0)
	/** Creates an uninitialized TMobilePhoneNITZ */
	{
	}

EXPORT_C RMobilePhone::TMobilePhoneNITZ::TMobilePhoneNITZ(TInt aYear, TMonth aMonth, TInt aDay, TInt aHour, TInt aMinute, TInt aSecond, TInt aMicroSecond) 
	: TDateTime(aYear,aMonth,aDay,aHour,aMinute,aSecond,aMicroSecond), iNitzFieldsUsed(0), iTimeZone(0), iDST(0)
	/** Creates a TMobilePhoneNITZ whose date and time attributes are set to the constructor's 
	parameters. The other TMobilePhoneNITZ attributes must be set explicitly.
	
	@param aYear Year.
	@param aMonth Month.
	@param aDay Day.
	@param aHour Hour.
	@param aMinute Minute.
	@param aSecond Second.
	@param aMicroSecond Micro second. */
	{
	}

EXPORT_C TInt RMobilePhone::GetNITZInfo(TMobilePhoneNITZ& aNITZInfo) const
/** Gets the current Network Information and Time Zone (NITZ) 
from the network.

@param aNITZInfo On return the NITZ information.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not support 
access to network time and date information, KErrNotFound if the TSY has not 
received any network time and date information. 
@capability ReadDeviceData
*/
	{
	TPckg<TMobilePhoneNITZ> ptr1(aNITZInfo);
	return Get(EMobilePhoneGetNITZInfo,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyNITZInfoChange(TRequestStatus& aReqStatus, TMobilePhoneNITZ& aNITZInfo) const
/** Notifies the client of changes to the Network Information and Time Zone (NITZ).

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyNITZInfoChange) 
to cancel a previously placed asynchronous NotifyNITZInfoChange() request.

@param aReqStatus On completion, KErrNone if successful.
@param aNITZInfo On completion, the new NITZ. 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyNITZInfoChange,aNITZInfo);
	Get(EMobilePhoneNotifyNITZInfoChange,aReqStatus,ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneServiceProviderNameV2::TMobilePhoneServiceProviderNameV2() 
	: iDisplayReq(KDisplayPLMNNotRequired)
/** Default constructor. Initialises iDisplayReq to be false. */
	{
	iExtensionId=KETelExtMultimodeV2;
	}

/**
Returns the service provider name.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aName Will contain the service provider name and a "display-required" 
boolean. 
@capability None
*/
EXPORT_C void RMobilePhone::GetServiceProviderName(TRequestStatus& aReqStatus, TDes8& aName) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	Get(EMobilePhoneGetServiceProviderName,aReqStatus,aName);
	}

EXPORT_C TInt RMobilePhone::GetNetworkInvScanSetting(TMobilePhoneInvestigationScan & aSetting) const
/**
Returns the current value of the network investigation scan setting. 
It is a synchronous method because it is assumed that the TSY will have previously retrieved and 
locally stored this system setting. 

@param aSetting Returns the current setting of investigation scan.
@return KErrNone

@capability None
*/
	{
	TPckg<TMobilePhoneInvestigationScan> ptr1(aSetting);
	return Get(EMobilePhoneGetNetworkInvScanSetting, ptr1);
	}
	
EXPORT_C void RMobilePhone::NotifyNetworkInvScanChange(TRequestStatus& aReqStatus, TMobilePhoneInvestigationScan& aSetting) const
/**
Notifies the client of changes to the network investigation scan setting.
 
@param aReqStatus returns the result code after the asynchronous call completes.
@param aSetting On completion returns the current setting of investigation scan.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyNetworkInvScanChange, aSetting);

	Get(EMobilePhoneNotifyNetworkInvScanChange, aReqStatus, ptr1);
	}
	
EXPORT_C void RMobilePhone::NotifyNetworkInvScanEvent(TRequestStatus& aReqStatus, TMobilePhoneInvestigationScanEvent& aEvent) const
/**
Notifies the client when a higher priority network has been detected that does not offer voice. The mobile user can manually select the network if this is desirable.
 
@param aReqStatus returns the result code after the asynchronous call completes.
@param aEvent Returns the current setting of investigation scan.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyNetworkInvScanEvent, aEvent);

	Get(EMobilePhoneNotifyNetworkInvScanEvent, aReqStatus, ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneNetworkNameV3::TMobilePhoneNetworkNameV3() 
	
/**
Default constructor.
*/
	{
	iExtensionId=KETelExtMultimodeV3;
	}
	
EXPORT_C RMobilePhone::TMobilePhoneOPlmnV3::TMobilePhoneOPlmnV3() 
	: iPNNid(0), iFirstLocationAreaCode(0), iLastLocationAreaCode(0)
/**
Default constructor.
*/
	{
	iExtensionId=KETelExtMultimodeV3;
	}

/**
Retrieves a set of short, long network names and location information for the PLMN that the phone is currently registered to. 
This information is extracted from the EF_PNN and EF_OPL files in the UICC. 

@param aReqStatus returns the result code after the asynchronous call completes.

@param aNetworkName On completion contains the current network name information.
This parameter should be a packaged RMobilePhone::TMobilePhoneNetworkNameV3 (or derived class).

@param aLocInfo On completion  contains the PLMN information.
This parameter should be a packaged RMobilePhone::TMobilePhoneOPlmnV3 (or derived class).

@capability None

@see TMobilePhoneNetworkNameV3Pckg
@see TMobilePhoneOPlmnV3Pckg
*/
EXPORT_C void RMobilePhone::GetCurrentNetworkName(TRequestStatus& aReqStatus, TDes8& aNetworkName, TDes8& aLocInfo) const
	{
	Get(EMobilePhoneGetNetworkName,aReqStatus,aNetworkName, aLocInfo);	
	}
	
EXPORT_C RMobilePhone::TMobilePreferredNetworkEntryV3::TMobilePreferredNetworkEntryV3() 
	: iAccess(KNetworkAccessUnknown), iUserDefined(ETrue)
/**
Default constructor.
*/	
	{
	iExtensionId=KETelExtMultimodeV3;	
	}

void RMobilePhone::TMobilePreferredNetworkEntryV3::InternalizeL(RReadStream& aStream)
/**
Internalizes the preferred network entry from a stream.

@param aStream The read stream containing the preferred network entry.
*/
	{
	TMultimodeType::InternalizeL(aStream);
	iAccess = aStream.ReadUint32L();
	iUserDefined = static_cast<TBool>(aStream.ReadInt32L());
	aStream >> iCountryCode;
	aStream >> iNetworkId;
	}

void RMobilePhone::TMobilePreferredNetworkEntryV3::ExternalizeL(RWriteStream& aStream) const
/**
This method externalizes the preferred network entry into a stream.

@param aStream The write stream that will contain the preferred network entry.
*/
	{
	TMultimodeType::ExternalizeL(aStream);	
	aStream.WriteUint32L(iAccess);
	aStream.WriteInt32L(iUserDefined); 
	aStream << iCountryCode;
	aStream << iNetworkId;
	}
	
	
EXPORT_C void RMobilePhone::StorePreferredNetworksListL(TRequestStatus& aReqStatus, CMobilePhoneStoredNetworkList* aList) const
/**
Stores a new version of the entire list of preferred network entries. The list, which is supplied by the client in 
an instance of CMobilePhoneStoredNetworkList, should not contain more entries than can be stored phone-side.

The list of the entries refer to below context.
  (i)   Home Networks List with Access Technology (EFHPLMNwAcT user read-only), 
  (ii)  Operator Networks List with Access Technology (EFOPLMNwACT user read-only), 
  (iii) User-controlled Networks List with Access Technology (EFPLMNwAcT user editable),
  (iv)  Operator controlled PLMN selector for WLAN Access (EFOPLMNWLAN user read-only), and
  (v)	User controlled PLMN selector for WLAN Access (EFUPLMNWLAN user editable). 

@param aReqStatus returns the result code after the asynchronous call completes.
@param aList List of preferred networks to be stored phone-side.

@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(aList!=NULL,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	delete iMmPtrHolder->iStoreNetworkBuf;
	iMmPtrHolder->iStoreNetworkBuf = NULL;

	iMmPtrHolder->iStoreNetworkBuf=aList->StoreLC();
	CleanupStack::Pop();

	(iMmPtrHolder->iStoreNetworkPtr).Set((iMmPtrHolder->iStoreNetworkBuf)->Ptr(0));

	Set(EMobilePhoneStorePreferredNetworksList,aReqStatus,iMmPtrHolder->iStoreNetworkPtr);	
	}
	
EXPORT_C void RMobilePhone::NotifyPreferredNetworksListChange(TRequestStatus& aReqStatus) const
/**
Notifies the client that changes have been made to the stored preferred networks list. 
When it completes, it does not return the new version of the list so the client will have to retrieve the list again if it is interested in the new list.

@param aReqStatus returns the result code after the asynchronous call completes.

@capability None
*/
	{
	Blank(EMobilePhoneNotifyStorePreferredNetworksListChange, aReqStatus);
	}
	
/***********************************************************************************/
//
// MobileMultimediaCallSettings functional unit
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetMultimediaCallPreference(TMobilePhoneMultimediaSettings& aMmSettings) const
/** Returns the phone's multimedia call preferences. It is synchronous 
because the TSY keeps a cached version.

@param aMmSettings An enum that will contain the current multimedia call preferences.
@return The error code that will be KErrNone or, if user network access is 
not supported then KErrNotSupported. 
@capability None
*/
	{
	TPckg<TMobilePhoneMultimediaSettings> ptr1(aMmSettings);
	return Get(EMobilePhoneGetMultimediaCallPreference, ptr1);
	}

EXPORT_C void RMobilePhone::SetMultimediaCallPreference(TRequestStatus& aReqStatus, TMobilePhoneMultimediaSettings aMmSettings) const
/** Sets the phone's multimedia call preferences. It is asynchronous.

@param aReqStatus The result code after the asynchronous call completes.
@param aMmSettings The new multimedia call preferences. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iMmSettings = aMmSettings;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetMultimediaCallPreference,iMmPtrHolder->iMmSettings);

	Set(EMobilePhoneSetMultimediaCallPreference,aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyMultimediaCallPreferenceChange(TRequestStatus& aReqStatus, TMobilePhoneMultimediaSettings& aMmSettings)
/** Notifies the client when the phone's Multimedia Call 
Preferences mode changes. Upon completion, the new mode will be specified 
by aMmSettings.

@param aReqStatus On completion this contains the asynchronous call result 
code.
@param aMmSettings An enum that will contain the new multimedia call preferences. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyMultimediaCallPreferenceChange, aMmSettings);

	Get(EMobilePhoneNotifyMultimediaCallPreferenceChange, aReqStatus, ptr1);
	}


/***********************************************************************************/
//
// MobilePrivacy functional unit
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetDefaultPrivacy(TMobilePhonePrivacy& aSetting) const

/** Gets the current setting of the voice privacy service.

This setting dictates whether privacy will be requested or not for the next 
MO or MT call. If the phone is in GSM or WCDMA mode (but also supports CDMA 
mode), then the returned aSetting will not be applicable in the current mode 
but will be applicable if the MS roams to CDMA mode.

@param aSetting On return, the default privacy setting of the phone.
@return KErrNone if successful, KErrNotSupported if the phone does not support 
voice privacy. 
@capability ReadDeviceData
@capability NetworkControl
*/
	{
	TPckg<TMobilePhonePrivacy> ptr1(aSetting);
	return Get(EMobilePhoneGetDefaultPrivacy, ptr1);
	}

EXPORT_C void RMobilePhone::SetDefaultPrivacy(TRequestStatus& aReqStatus, TMobilePhonePrivacy aSetting) const

/** This function member allows a client to set the default setting for the voice 
privacy service.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetDefaultPrivacy) 
to cancel a previously placed asynchronous SetDefaultPrivacy() request.

@param aReqStatus On completion, KErrNone if successful, KErrNotSupported 
if the phone does not support voice privacy.
@param aSetting The new default privacy setting. 
@capability WriteDeviceData
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iPrivacySetting = aSetting;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetDefaultPrivacy,iMmPtrHolder->iPrivacySetting);

	Set(EMobilePhoneSetDefaultPrivacy,aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyDefaultPrivacyChange(TRequestStatus& aReqStatus, TMobilePhonePrivacy& aSetting) const

/** This function member allows a client to be notified if there is a change in 
the default setting for the voice privacy service.

Use RTelSubSessionBase::CancelAsyncRequest(aREMobilePhoneNotifyDefaultPrivacyChangeeqToCancel) 
to cancel a previously placed asynchronous NotifyDefaultPrivacyChange() request.

@param aReqStatus On completion, KErrNone if successful, KErrNotSupported 
if the phone does not support voice privacy.
@param aSetting On return, the new default privacy setting of the phone. 
@capability ReadDeviceData
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyDefaultPrivacyChange, aSetting);

	Get(EMobilePhoneNotifyDefaultPrivacyChange, aReqStatus, ptr1);
	}

/***********************************************************************************/
//
// TSY Capabilities for supplementary call services
//
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetCallServiceCaps(TUint32& aCaps) const

/** This function member gets the combination of the static and dynamic call service 
capabilities of the phone.

@param aCaps On return, the sum of RMobilePhone::TMobilePhoneCallServiceCaps
constants.
@return KErrNone 
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetCallServiceCaps, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyCallServiceCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const

/** This request allows a client to be notified when the phone's call
service capabilities changes - probably in response to a change of
network mode.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCallServiceCapsChange) 
to cancel a previously placed asynchronous NotifyCallServiceCapsChange() request.

@param aReqStatus On completion, KErrNone if successful
@param aCaps On return, the sum of RMobilePhone::TMobilePhoneCallServiceCaps
constants.
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyCallServiceCapsChange, aCaps);

	Get(EMobilePhoneNotifyCallServiceCapsChange, aReqStatus, ptr1);
	}

/***********************************************************************************/
//
// MobileUserNetworkAccess functional unit
//
/***********************************************************************************/

EXPORT_C RMobilePhone::TMobilePhoneSendSSRequestV3::TMobilePhoneSendSSRequestV3() 
:	iOpCode(0)
/** 
Default constructor for TMobilePhoneSendSSRequestV3.
*/
	{
	iExtensionId=KETelExtMultimodeV3;
	}

EXPORT_C void RMobilePhone::ProgramFeatureCode(TRequestStatus& aReqStatus, const TDesC& aFCString, TMobilePhoneNetworkService aService, TMobilePhoneServiceAction aAction) const

/** This function member programs the feature code string against the network service 
and the action that is being taken upon this service.

The feature code string programmed must be applicable to the network represented 
by the currently active NAM.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneProgramFeatureCode) 
to cancel a previously placed asynchronous ProgramFeatureCode() request.

@param aReqStatus On completion, KErrNone if successful, KErrNotSupported 
if the phone does not support feature code programming or does not support 
programming a code for the specified network service.
@param aFCString The feature code string.
@param aService The network service
@param aAction Action that is being taken. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iProgramFeatureCode.iService = aService;
	iMmPtrHolder->iProgramFeatureCode.iAction = aAction;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1ProgramFeatureCode,iMmPtrHolder->iProgramFeatureCode);

	Set(EMobilePhoneProgramFeatureCode,aReqStatus, ptr1, aFCString);
	}

EXPORT_C void RMobilePhone::GetFeatureCode(TRequestStatus& aReqStatus, TDes& aFCString, TMobilePhoneNetworkService aService, TMobilePhoneServiceAction aAction) const

/** This function member gets the feature code string programmed against the network 
service programmed for the service provider represented by the currently active 
NAM.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetFeatureCode) to 
cancel a previously placed asynchronous GetFeatureCode() request.

@param aReqStatus On completion, KErrNone if successful, KErrNotSupported 
if the phone does not support feature code programming or does not support 
programming a code for the specified network service, KErrNotFound if the 
feature code has not been programmed for the specified network service.
@param aFCString On return, the programmed feature code string.
@param aService The service specified.
@param aAction The action specified. 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iGetFeatureCode.iService = aService;
	iMmPtrHolder->iGetFeatureCode.iAction = aAction;
	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetFeatureCode,iMmPtrHolder->iGetFeatureCode);

	Get(EMobilePhoneGetFeatureCode,aReqStatus, ptr1, aFCString);
	}

EXPORT_C void RMobilePhone::SendNetworkServiceRequest(TRequestStatus& aReqStatus, const TDesC& aServiceString) const

/** This function member sends a network service request string (e.g. a SS string 
entered by the user) directly to the serving network

The request will complete after the phone has sent the string and, if it is 
expecting one, received an acknowledgment from the network.

In CDMA mode, feature code strings are sent to the network within a flash 
message. They are not acknowledged by a signalling layer message by the network, 
so the TSY completes SendNetworkServiceRequest() request as soon as it has 
sent the string.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSendNetworkServiceRequest) 
to cancel a previously placed asynchronous SendNetworkServiceRequest() request.

@param aReqStatus On return, KErrNone if successful.
@param aServiceString The service string to send. 
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	Set(EMobilePhoneSendNetworkServiceRequest, aReqStatus, aServiceString);
	}

EXPORT_C void RMobilePhone::SendNetworkServiceRequestNoFdnCheck(TRequestStatus& aReqStatus, const TDesC& aServiceString) const
/**
 * This method sends a request to the network in the form of a character and digit string
 * It will complete either when the network has confirmed the request or after the phone has sent it
 * depending upon whether network confirmation is supported by serving network. The number used for   
 * sending the network request is not checked against those in the Fixed Dialling Number list even 
 * if the FDN service is enabled
 *
 * @param aServiceString Supplies the supplementary service or feature code string to be sent
 
@capability NetworkControl
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	Set(EMobilePhoneSendNetworkServiceRequestNoFdnCheck, aReqStatus, aServiceString);
	}
	
EXPORT_C void RMobilePhone::NotifySendNetworkServiceRequest(TRequestStatus& aReqStatus, const TMobilePhoneNotifySendSSOperation aOperation, TDes8& aRequestComplete) const
/**
Notifies the client for send network service request invocation and request completion events.

@param aReqStatus returns the result code after the asynchronous call completes.
@param aOperation Supplies the SS operation to be notified about.
@param aRequestComplete On completion will contain the operation code and any additional information parameters for the SS operation.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSendSSOperation = aOperation;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SendSSOperation,iMmPtrHolder->iSendSSOperation);
	
	SetAndGet(EMobilePhoneNotifySendNetworkServiceRequest, aReqStatus, ptr1, aRequestComplete);	
	}

EXPORT_C void RMobilePhone::NotifyAllSendNetworkServiceRequest(TRequestStatus& aReqStatus, TMobilePhoneNotifySendSSOperation& aOperation, TDes8& aRequestComplete) const
/**
Notifies the client for all send network service request invocation and request completion events.

@param aReqStatus returns the result code after the asynchronous call completes.
@param aOperation On return, will contain the type of operation i.e. Send SS request invokation, return result/error or reject.
@param aRequestComplete On completion will contain the operation code and any additional information parameters for the SS operation.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1SendSSOperation, aOperation);
	
	Get(EMobilePhoneNotifyAllSendNetworkServiceRequest, aReqStatus, ptr1, aRequestComplete);	
	}

/***********************************************************************************/
//
// MobileCallForwarding functional unit
// 
/***********************************************************************************/

EXPORT_C RMobilePhone::TMobilePhoneCFInfoEntryV1::TMobilePhoneCFInfoEntryV1() 
:	iCondition(ECallForwardingUnspecified), 
	iServiceGroup(EServiceUnspecified),
	iStatus(ECallForwardingStatusUnknown),
	iTimeout(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

void RMobilePhone::TMobilePhoneCFInfoEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the call forwarding information from a stream 
 *
 * @param aStream The read stream containing the call forwarding information
 */
	{
	TMultimodeType::InternalizeL(aStream);
	iCondition=STATIC_CAST(TMobilePhoneCFCondition, aStream.ReadUint32L());
	iServiceGroup=STATIC_CAST(TMobileService, aStream.ReadUint32L());
	iStatus=STATIC_CAST(TMobilePhoneCFStatus, aStream.ReadUint32L());
	aStream >> iNumber;
	iTimeout=aStream.ReadInt32L();
	}

void RMobilePhone::TMobilePhoneCFInfoEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the call forwarding information into a stream 
 *
 * @param aStream The write stream that will contain the call forwarding information
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream.WriteUint32L(iCondition);
	aStream.WriteUint32L(iServiceGroup);
	aStream.WriteUint32L(iStatus);
	aStream << iNumber;
	aStream.WriteInt32L(iTimeout);
	}


EXPORT_C void RMobilePhone::NotifyCallForwardingStatusChange(TRequestStatus& aReqStatus, TMobilePhoneCFCondition& aCondition) const

/** This request allows a client to be noftified when the status or associated 
information changes for any of the call forwarding conditions as applied to 
any of the basic service groups.

The request will be completed when either the status, "forward-to" number, 
or "no-reply" time-out changes for any of the CF conditions applied to any 
of the basic service groups. Upon completion, the call forwarding condition(s) 
that have changed will be specified by aCondition. It will then be up to the 
client to update its knowledge of the CF condition(s) by performing the "get" 
call forwarding status operation again and asking for the relevant CF condition(s).

For example, CFB condition is registered and active for voice calls. Client 
A decides to register and activate CFU condition for all calls. This will 
force CFB condition to become quiescent while CFU is active. Client B has 
an outstanding NotifyCallForwardingStatusChange. The TSY needs to tell client 
B that the status of both CFU and CFB has changed. It will complete NotifyCallForwardingStatusChange 
and set the returned aCondition to ECallForwardingAllCases. 

Client B can then choose to interrogate current status of all call forwarding 
conditions. In response to this client B will receive a list with up to 16 
entries.

These entries will contain the following:

One CFU entry, set for "all call services" and with status "active" and registered 
information. 

Up to 4 CFB entries, one for each supported basic service group. The one for 
voice will have status "not active" and registered information and the other 
entries will have status "not-registered". 

One CFNRy entry, set for "all call services" and with status "not-registered".

One CFNRc entry, set for "all call services" and with status "not-registered".

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCallForwardingStatusChange) 
to cancel a previously placed asynchronous NotifyCallForwardingStatusChange() 
request.

@param aReqStatus On completion, KErrNone if successful.
@param aCondition On return, the new call forwarding flags. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyCallForwardingStatusChange,aCondition);

	Get(EMobilePhoneNotifyCallForwardingStatusChange,aReqStatus, ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneCFChangeV1::TMobilePhoneCFChangeV1() 
:	iServiceGroup(EServiceUnspecified),
	iAction(EServiceActionUnspecified), 
	iTimeout(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::SetCallForwardingStatus(TRequestStatus& aReqStatus, TMobilePhoneCFCondition aCondition, const TMobilePhoneCFChangeV1& aInfo) const

/** This function member sets a new status and/or registered information for the 
call forwarding condition.

If a client activates a call forwarding service for "all call services", then 
this will have the effect of replacing any individual basic service group 
settings.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetCallForwardingStatus) 
to cancel a previously placed asynchronous SetCallForwardingStatus() request.

@param aReqStatus On completion, KErrNone if successful.
@param aCondition The call forwarding condition.
@param aInfo The iAction attribute indicates the action it is taking. The status 
and information change will be applied to the basic service group(s) indicated 
by the iServiceGroup attribute. 
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iSetCFCondition = aCondition;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetCallForwardingStatus,iMmPtrHolder->iSetCFCondition);

	TPtrC8& ptr2=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SetCallForwardingStatus,aInfo);

	Set(EMobilePhoneSetCallForwardingStatus, aReqStatus, ptr1, ptr2);
	}

EXPORT_C void RMobilePhone::NotifyCallForwardingActive(TRequestStatus& aReqStatus, TMobileService& aServiceGroup, TMobilePhoneCFActive& aActiveType) const

/** This request allows a client to be notified when a call is made on a line and 
a call forwarding service is active on this line at the time.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCallForwardingActive) 
to cancel a previously placed asynchronous NotifyCallForwardingActive() request.

@param aReqStatus On completion, KErrNone if successful.
@param aServiceGroup On return, the basic service group(s) the call forwarding 
is active.
@param aActiveType On return, specifies whether CF Unconditional or one of 
the conditional CF services is active 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1PhoneNotifyCallForwardingActive, aServiceGroup);
	TPtr8& ptr2=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2PhoneNotifyCallForwardingActive, aActiveType);

	Get(EMobilePhoneNotifyCallForwardingActive, aReqStatus, ptr1, ptr2);
	}
	
EXPORT_C RMobilePhone::TMobilePhoneCFUIndicatorV3::TMobilePhoneCFUIndicatorV3() 
	: iIndicator(KCFUIndicatorUnknown)
/**
Default constructor.
*/
	{
	iExtensionId=KETelExtMultimodeV3;
	}

EXPORT_C RMobilePhone::TMobilePhoneCFUIndicatorV6::TMobilePhoneCFUIndicatorV6() 
	:	TMobilePhoneCFUIndicatorV3(),
		iMultipleSubscriberProfileID(KProfileIdentityUnknown)
/**
Default constructor.
This is the initialization of the TMobilePhoneCFUIndicatorV6 structured type
*/
	{
	iExtensionId = KEtelExtMultimodeV6;
	}

EXPORT_C void RMobilePhone::GetCallForwardingIndicator(TRequestStatus& aReqStatus, TDes8& aIndicator) const
/**
In GSM or WCDMA mode, if unconditional call forwarding is active an indicator can display this current status. 
This API, allows clients to retrieve the current setting of the indicator status. The indicator can be available for Voice, Fax and Data Service groups.

@param aReqStatus returns the result code after the asynchronous call completes.
@param aIndicator On completion contains the status information in a RMobilePhone::TMobilePhoneCFUIndicatorV3Pckg containing a RMobilePhone::TMobilePhoneCFUIndicatorV3.

@capability None
*/
	{
	Get(EMobilePhoneGetCallForwardingIndicator, aReqStatus, aIndicator);
	};

/***********************************************************************************/
//
// MobileIdentityService functional unit
// 
/***********************************************************************************/

EXPORT_C void RMobilePhone::GetIdentityServiceStatus(TRequestStatus& aReqStatus, const TMobilePhoneIdService aService, TMobilePhoneIdServiceStatus& aStatus, TMobileInfoLocation aLocation) const
 /** This function member retrieves the current status of the identity service specified.
	
	The phone will retrieve this service status information from the location 
	specified by the client.
	
	If the client has requested EInfoLocationCache but there is no valid status 
	information in the cache then the request will return KErrNotFound.
	
	If the client has requested EInfoLocationCachePreferred, the network will 
	be interrogated if the cache is empty. Whenever the network is interrogated, 
	the cache will be refreshed with the information from the network.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetIdentityServiceStatus) 
	to cancel a previously placed asynchronous GetIdentityServiceStatus() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aService The identity service (CLIP, CLIR, COLP, COLR etc.) to be queried.
	@param aStatus On return, the service status.
	@param aLocation Specifies whether the information should be retrieved from phone cache or network. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iGetIdentityServiceStatus.iLocation = aLocation;
	iMmPtrHolder->iGetIdentityServiceStatus.iService = aService;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetIdentityServiceStatus,iMmPtrHolder->iGetIdentityServiceStatus);
	TPtr8& ptr2 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2GetIdentityServiceStatus,aStatus);

	SetAndGet(EMobilePhoneGetIdentityServiceStatus,aReqStatus,ptr1,ptr2);
	}
	
EXPORT_C void RMobilePhone::SetIdentityServiceStatus(TRequestStatus& aReqStatus, const TMobilePhoneIdService aService, const TMobilePhoneIdServiceSetting aSetting) const
/**
Sets the default service status for provisioned identity services. If the selected identity service is not provisioned then KErrNotSupported is returned back to the client.

@param aReqStatus returns the result code after the asynchronous call completes.
@param aService Specifies the identity service (CLIP, CLIR, COLP, COLR etc.).
@param aSetting The preferred new setting.

@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetIdService = aService;
	iMmPtrHolder->iSetIdServiceSetting = aSetting;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetIdentityServiceStatus,iMmPtrHolder->iSetIdService);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SetIdentityServiceStatus,iMmPtrHolder->iSetIdServiceSetting);

	Set(EMobilePhoneSetIdentityServiceStatus,aReqStatus,ptr1,ptr2);
	}
	
EXPORT_C void RMobilePhone::NotifyIdentityServiceStatus(TRequestStatus& aReqStatus, const TMobilePhoneIdService aService, TMobilePhoneIdServiceStatus& aStatus) const
/**
Notifies a client when the status changes for any of the identity services.

@param aReqStatus returns the result code after the asynchronous call completes.
@param aService Specifies the identity service (CLIP, CLIR, COLP, COLR etc.).
@param aSetting The new status setting

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iNotifyIdService = aService;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1NotifyIdentityServiceStatus,iMmPtrHolder->iNotifyIdService);
	TPtr8& ptr2 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2NotifyIdentityServiceStatus,aStatus);

	SetAndGet(EMobilePhoneNotifyIdentityServiceStatus, aReqStatus,ptr1,ptr2);
	}
	
EXPORT_C void RMobilePhone::NotifyIdentitySuppressionRejected(TRequestStatus& aReqStatus) const
/**
Notifies the client when a suppression request (either permanent default or temporary) has been rejected for the CLIR identity
service. The request will be completed when a request for suppressing presentation of the call currently being "setup" is rejected. 

@param aReqStatus returns the result code after the asynchronous call completes.

@capability None
*/
	{
	Blank(EMobilePhoneNotifyIdentitySuppressionRejected,aReqStatus);
	}

/***********************************************************************************/
//
// MobileCallBarring functional unit
// 
/***********************************************************************************/

EXPORT_C RMobilePhone::TMobilePhoneCBInfoEntryV1::TMobilePhoneCBInfoEntryV1() 
:	iCondition(EBarUnspecified), 
	iServiceGroup(EServiceUnspecified), 
	iStatus(ECallBarringStatusUnknown)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

void RMobilePhone::TMobilePhoneCBInfoEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the call barring inforamation from a stream 
 *
 * @param aStream The read stream containing the call barring inforamation
 */
	{
	TMultimodeType::InternalizeL(aStream);
	iCondition = STATIC_CAST(TMobilePhoneCBCondition,aStream.ReadUint32L());
	iServiceGroup = STATIC_CAST(TMobileService,aStream.ReadUint32L());
	iStatus = STATIC_CAST(TMobilePhoneCBStatus,aStream.ReadUint32L());
	}

void RMobilePhone::TMobilePhoneCBInfoEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the call barring information into a stream 
 *
 * @param aStream The write stream that will contain the call barring information
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream.WriteUint32L(iCondition);
	aStream.WriteUint32L(iServiceGroup);
	aStream.WriteUint32L(iStatus);
	}
	
EXPORT_C RMobilePhone::TMobilePhoneCBInfoEntryV3::TMobilePhoneCBInfoEntryV3()
:	iCompMethod(0)
/**
Default constructor.
*/
	{
	iExtensionId=KETelExtMultimodeV3;
	}
	
void RMobilePhone::TMobilePhoneCBInfoEntryV3::InternalizeL(RReadStream& aStream)
/**
This method internalizes the call barring information from a stream 

@param aStream The read stream containing the call barring inforamation
*/
	{
	TMobilePhoneCBInfoEntryV1::InternalizeL(aStream);
	iCompMethod = aStream.ReadUint8L();	
	}

void RMobilePhone::TMobilePhoneCBInfoEntryV3::ExternalizeL(RWriteStream& aStream) const
/**
This method externalizes the call barring information into a stream 

@param aStream The write stream that will contain the call barring information
*/
	{
	TMobilePhoneCBInfoEntryV1::ExternalizeL(aStream);
	aStream.WriteUint8L(iCompMethod);
	}

EXPORT_C RMobilePhone::TMobilePhoneCBChangeV1::TMobilePhoneCBChangeV1() 
:	iServiceGroup(EServiceUnspecified),
	iAction(EServiceActionUnspecified)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::SetCallBarringStatus(TRequestStatus& aReqStatus, TMobilePhoneCBCondition aCondition, const TMobilePhoneCBChangeV1& aInfo) const

/** This function member sets the call barring status for the call barring condition.


If a client activates a call barring service for "all call services" then 
this will have the effect of replacing any individual basic service group 
settings.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetCallBarringStatus) 
to cancel a previously placed asynchronous SetCallBarringStatus() request.

Note: Completes either when the network has confirmed the request or after the phone has 
sent it depending upon whether network confirmation is supported by the serving network.

@param aReqStatus On return, KErrNone if successful.
@param aCondition The Call Barring type to set.
@param aInfo The iAction attribute of this object reflects the action to be 
taken. The client will also have to supply the barring password in order to 
change the setting of the specified barring program. The status change will 
be applied to the basic service group(s) indicated by the iServiceGroup attribute. 
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetCBStatusCondition = aCondition;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetCallBarringStatus,iMmPtrHolder->iSetCBStatusCondition);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SetCallBarringStatus,aInfo);

	Set(EMobilePhoneSetCallBarringStatus,aReqStatus,ptr1,ptr2);
	}

EXPORT_C void RMobilePhone::NotifyCallBarringStatusChange(TRequestStatus& aReqStatus, TMobilePhoneCBCondition& aCondition) const

/** This request allows a client to be notified when the status or associated information 
changes for any of the call barring conditions as applied to any of the basic 
service groups.

The request will be completed when the status changes for any of the CB conditions 
applied to any of the call services groups. It will then be up to the client 
to update its knowledge of the CB condition(s) by retrieving call barring 
status again for the relevant CB condition(s). 

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCallBarringStatusChange) 
to cancel a previously placed asynchronous NotifyCallBarringStatusChange() 
request.

@param aReqStatus On completion, KErrNone if successful.
@param aCondition On return, the call barring condition(s) that have changed. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyCallBarringStatusChange,aCondition);

	Get(EMobilePhoneNotifyCallBarringStatusChange,aReqStatus,ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhonePasswordChangeV1::TMobilePhonePasswordChangeV1()
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C RMobilePhone::TMobilePhonePasswordChangeV2::TMobilePhonePasswordChangeV2()
	{
	iExtensionId=KETelExtMultimodeV2;
	}

EXPORT_C void RMobilePhone::SetCallBarringPassword(TRequestStatus& aReqStatus, const TMobilePhonePasswordChangeV1& aPassword) const

/** This function member allows the client to change the call barring password 
for the call barring service.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetCallBarringPassword) 
to cancel a previously placed asynchronous SetCallBarringPassword() request.

Note: Completes either when the network has confirmed the request or after the phone has 
sent it depending upon whether network confirmation is supported by the serving network.

@param aReqStatus On return, KErrNone if successful, KErrAccessDenied if the 
old password was incorrect.
@param aPassword The old and new password. 
@capability WriteDeviceData
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetCallBarringPassword,aPassword);

	Set(EMobilePhoneSetCallBarringPassword,aReqStatus,ptr1);
	}


EXPORT_C void RMobilePhone::SetSSPassword(TRequestStatus& aReqStatus, const TDesC8& aPassword, const TInt aService) const
/**
 * This method allows a call barring or common password to be changed on a given service.
 * Verification of the password change is returned by the network and should be passed to
 * the MS for correct interpretation of the result.
 *
 * @param aReqStatus returns the result code after the asynchronous call completes
 * @param aPassword Supplies the old, new & confirmation call barring passwords
 * @param aService The service that the password is being changed on (0=All SS services)
 * @capability WriteDeviceData 
 * @capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iBarringService=aService;
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetService, iMmPtrHolder->iBarringService);

	Set(EMobilePhoneSetSSPassword,aReqStatus,aPassword,ptr2);
	}
	
EXPORT_C void RMobilePhone::GetCompMethodName(TRequestStatus& aReqStatus, TDes& aAlphaTag, const TUint8 aCmi) const
/**
Retrieves comparison methods alpha-tags as specified by UICC card issuers and stored in the EF_CMI file.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aAlphaTag On completion contains the alpha-tags.
@param aCmi Comparison method identifier.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iCompMethodIdentifier=aCmi;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetCompMethodName, iMmPtrHolder->iCompMethodIdentifier);

	SetAndGet(EMobilePhoneGetCompMethodName,aReqStatus,ptr1,aAlphaTag);
	}

/***********************************************************************************/
//
// MobileCallWaiting functional unit
// 
/***********************************************************************************/

void RMobilePhone::TMobilePhoneCWInfoEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the call waiting inforamation from a stream 
 *
 * @param aStream The read stream containing the call waiting inforamation
 */
	{
	TMultimodeType::InternalizeL(aStream);
	iServiceGroup = STATIC_CAST(TMobileService,aStream.ReadUint32L());
	iStatus = STATIC_CAST(TMobilePhoneCWStatus,aStream.ReadUint32L());
	}

void RMobilePhone::TMobilePhoneCWInfoEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the call waiting information into a stream 
 *
 * @param aStream The write stream that will contain the call waiting information
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream.WriteUint32L(iServiceGroup);
	aStream.WriteUint32L(iStatus);
	}

EXPORT_C RMobilePhone::TMobilePhoneCWInfoEntryV1::TMobilePhoneCWInfoEntryV1() 
:	iServiceGroup(EServiceUnspecified), 
	iStatus(ECallWaitingStatusUnknown)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::SetCallWaitingStatus(TRequestStatus& aReqStatus, TMobileService aServiceGroup, TMobilePhoneServiceAction aAction) const

/** This function member sets the call waiting status.

If a client activates a call waiting service for "all call services" then 
this will have the effect of replacing any individual basic service group 
settings. 

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetCallWaitingStatus) 
to cancel a previously placed asynchronous SetCallWaitingStatus() request.

Note: Completes either when the network has confirmed the request or after 
the phone has sent it depending upon whether network confirmation is supported 
by the serving network.

@param aReqStatus On return, KErrNone if successful.
@param aServiceGroup The service group affected.
@param aAction Set to activate or deactivate the CW service. 
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetCallWaitingStatusGroup = aServiceGroup;
	iMmPtrHolder->iSetCallWaitingStatusAction = aAction;
	
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetCallWaitingStatus,iMmPtrHolder->iSetCallWaitingStatusGroup);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SetCallWaitingStatus,iMmPtrHolder->iSetCallWaitingStatusAction);

	Set(EMobilePhoneSetCallWaitingStatus,aReqStatus,ptr1,ptr2);
	}

EXPORT_C void RMobilePhone::NotifyCallWaitingStatusChange(TRequestStatus& aReqStatus, TDes8& aCWStatus) const

/** This request allows a client to be notified when the status changes for the 
call waiting service as applied to any of the basic service groups.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCallWaitingStatusChange) 
to cancel a previously placed asynchronous NotifyCallWaitingStatusChange() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aCWStatus On return, a TMobilePhoneCWInfoEntryV1Pckg class describing 
the changes.
@see TMobilePhoneCWInfoEntryV1 
@capability None
*/
	{
	Get(EMobilePhoneNotifyCallWaitingStatusChange,aReqStatus,aCWStatus);
	}


/***********************************************************************************/
//
// MobileCallCompletion functional unit
// 
/***********************************************************************************/
	
EXPORT_C void RMobilePhone::GetCCBSStatus(TRequestStatus& aReqStatus, TMobilePhoneCCBSStatus& aCcbsStatus, TMobileInfoLocation aLocation) const

/** This function member retrieves the current status of the CCBS service as applied 
to the MS subscriber.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetCCBSStatus) to cancel 
a previously placed asynchronous GetCCBSStatus() request.

@param aReqStatus On return, KErrNone if successful.
@param aCcbsStatus On return, the current CCBS status.
@param aLocation The location from which the status is to be retrieved. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iGetCCBSStatusLocation = aLocation;
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetCCBSStatus,aCcbsStatus);
	TPtr8& ptr2 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2GetCCBSStatus,iMmPtrHolder->iGetCCBSStatusLocation);

	Get(EMobilePhoneGetCCBSStatus,aReqStatus,ptr1,ptr2);
	}

EXPORT_C void RMobilePhone::NotifyCCBSStatusChange(TRequestStatus& aReqStatus, TMobilePhoneCCBSStatus& aCcbsStatus) const

/** This function member allows the client to be notified if the status of the 
CCBS service changes

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCCBSStatusChange) 
to cancel a previously placed asynchronous GetBNotifyCCBSStatusChangeatteryInfo() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aCcbsStatus On return, the new CCBS status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyCCBSStatusChange,aCcbsStatus);
	Get(EMobilePhoneNotifyCCBSStatusChange,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyCCBSRecall(TRequestStatus& aReqStatus, TDes8& aCCBSEntry) const
/**
 * This notification completes after a previously busy remote party becomes idle 
 * \retval aCCBSEntry a structure that contains the details of the ccbs call
 
@capability None
*/
	{
	Get(EMobilePhoneNotifyCCBSRecall,aReqStatus,aCCBSEntry);	
	}


EXPORT_C void RMobilePhone::DeactivateCCBS(TRequestStatus& aReqStatus, TInt aIndex) const
/**
 * This method deactivates a CCBS request that is still waiting for the remote party to become free
 * It will complete once the deactivate request has been confirmed by the network
 * 
 * @param aIndex specifies the index of the active CCBS request, if set to 1 all the requests will be cancelled
 
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iDeactivateCCBSIndex=aIndex;
 	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1DeactivateCCBS,iMmPtrHolder->iDeactivateCCBSIndex);

	Set(EMobilePhoneDeactivateCCBS,aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::AcceptCCBSRecall(TRequestStatus& aReqStatus, TInt aIndex, TName& aCallName) const
/**
 * This method accepts the CCBS recall so that the MO call is set-up to the remote party
 * It will complete once the call is connected to the remote party
 * @param aIndex The index of the call that the client wishes to set the recall on
 * \retval aName The name of the call created by the tsy and that the client will have to open
 
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iAcceptCCBSRecallIndex=aIndex;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1AcceptCCBSRecall,iMmPtrHolder->iAcceptCCBSRecallIndex);
	SetAndGet(EMobilePhoneAcceptCCBSRecall,aReqStatus, ptr1, aCallName);
	}

EXPORT_C TInt RMobilePhone::RefuseCCBSRecall(TInt aIndex) const
/**
 * This method refuses the CCBS recall after Notify ccbs recall hads completed.
 * @param aIndex The index of the call to refuse.
 * @return KErrNone
 
@capability NetworkServices
*/
	{
	TPckg<TInt> ptr1(aIndex);
	return Set(EMobilePhoneRefuseCCBSRecall, ptr1);
	}

void RMobilePhone::TMobilePhoneCCBSEntryV1::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the CCBS inforamation from a stream 
 *
 * @param aStream The read stream containing the CCBS inforamation
 */
	{
	TMultimodeType::InternalizeL(aStream);
	iServiceGroup = STATIC_CAST(TMobileService,aStream.ReadUint32L());
	iCcbsIndex = STATIC_CAST(TInt,aStream.ReadUint32L());
	aStream >> iDestination;
	}

void RMobilePhone::TMobilePhoneCCBSEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the CCBS information into a stream 
 *
 * @param aStream The write stream that will contain the CCBS information
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream.WriteInt32L(iServiceGroup);
	aStream.WriteInt32L(iCcbsIndex);
	aStream << iDestination;
	}

/**
 * Version KETelExtMultimodeV1 constructor
 *
 */
EXPORT_C RMobilePhone::TMobilePhoneCCBSEntryV1::TMobilePhoneCCBSEntryV1() 
:	iCcbsIndex(0), iServiceGroup(EServiceUnspecified)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

/**
 * Version KETelExtMultimodeV2 constructor
 *
 */
EXPORT_C RMobilePhone::TMobilePhoneCCBSEntryV2::TMobilePhoneCCBSEntryV2() 
:	iEvent(ECcbsRecallUnspecified) 
	{
	iExtensionId=KETelExtMultimodeV2;
	}

/***********************************************************************************/
//
// MobileAlternatingCall functional unit
// 
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetAlternatingCallCaps(TUint32& aCaps) const

/** This function member gets a snapshot of the combination of the static and dynamic 
alternating call capabilities of the phone.

@param aCaps On return, the sum of
RMobilePhone::TMobilePhoneAlternatingCallCaps constants.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not support 
any client access to alternating call functionality. 
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetAlternatingCallCaps,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyAlternatingCallCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const

/** This request allows a client to be notified when the phone's alternating call 
capabilities change - probably in response to a change of network mode.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyAlternatingCallCapsChange) 
to cancel a previously placed asynchronous NotifyAlternatingCallCapsChange() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aCaps On return, the new sum of
RMobilePhone::TMobilePhoneAlternatingCallCaps constants.
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyAlternatingCallCapsChange,aCaps);

	Get(EMobilePhoneNotifyAlternatingCallCapsChange,aReqStatus,ptr1);
	}

EXPORT_C TInt RMobilePhone::GetAlternatingCallMode(TMobilePhoneAlternatingCallMode& aMode, TMobileService& aFirstService) const

/** This function member gets the current setting of alternating call mode. This 
setting dictates the type of call that will be requested for the next call.

The alternating call mode is returned in the aMode parameter and the aFirstService 
parameter specifies which bearer service (voice, data or fax) will be the 
first mode of the call.

@param aMode On return, the alternating call mode.
@param aFirstService On return, specifies which bearer service will be the 
first mode of the call.
@return KErrNone if successful, KErrNotSupported if the phone does not support 
alternating calls. 
@capability None
*/
	{
	TPckg<TMobilePhoneAlternatingCallMode> ptr1(aMode);
	TPckg<TMobileService> ptr2(aFirstService);

	return Get(EMobilePhoneGetAlternatingCallMode,ptr1,ptr2);
	}

EXPORT_C void RMobilePhone::SetAlternatingCallMode(TRequestStatus& aReqStatus, TMobilePhoneAlternatingCallMode aMode, TMobileService aFirstService) const

/** This function member sets the alternating call mode.

An alternating call can only be initiated or received if the alternating call 
mode setting equals one of the non-default values of TMobilePhoneAlternatingCallMode. 
The default setting is EAlternatingModeSingle and the alternating call mode 
setting will return to the default after an alternating call has finished.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetAlternatingCallMode) 
to cancel a previously placed asynchronous SetAlternatingCallMode() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if the 
phone does not support alternating calls.
@param aMode The alternating call mode to set
@param aFirstService The call bearer to set. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetAlternatingCallModeMode = aMode;
	iMmPtrHolder->iSetAlternatingCallModeService = aFirstService;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetAlternatingCallMode,iMmPtrHolder->iSetAlternatingCallModeMode);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SetAlternatingCallMode,iMmPtrHolder->iSetAlternatingCallModeService);
	
	Set(EMobilePhoneSetAlternatingCallMode,aReqStatus,ptr1,ptr2);
	}

EXPORT_C void RMobilePhone::NotifyAlternatingCallModeChange(TRequestStatus& aReqStatus, TMobilePhoneAlternatingCallMode& aMode,TMobileService& aFirstService) const

/** This request allows a client to be notified when the phone's alternating call 
mode changes.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyAlternatingCallModeChange) 
to cancel a previously placed asynchronous NotifyAlternatingCallModeChange() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aMode On return, the new alternating call mode.
@param aFirstService On return, the new call bearer setting. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyAlternatingCallModeChange,aMode);
	TPtr8& ptr2 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2NotifyAlternatingCallModeChange,aFirstService);

	Get(EMobilePhoneNotifyAlternatingCallModeChange,aReqStatus,ptr1,ptr2);
	}

/***********************************************************************************/
//
// MobileAlternateLineService functional unit
// 
/***********************************************************************************/


EXPORT_C TInt RMobilePhone::GetALSLine(TMobilePhoneALSLine& aALSLine) const

/** This function member allows a client to find out which voice line is currently 
selected as the active ALS line for outgoing voice calls.

@param aALSLine On return, the currently selected ALS line
@return KErrNone if successful, KErrNotSupported if the phone does not support 
access to ALS functionality. 
@capability None
*/
	{
	TPckg<TMobilePhoneALSLine> ptr1(aALSLine);
	return Get(EMobilePhoneGetALSLine,ptr1);
	}

EXPORT_C void RMobilePhone::SetALSLine(TRequestStatus& aReqStatus, TMobilePhoneALSLine aALSLine) const

/** This function member allows a client to set which voice line is currently selected 
as the active ALS line for outgoing voice calls. 

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetALSLine) to cancel 
a previously placed asynchronous SetALSLine() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if the 
phone does not support access to ALS functionality.
@param aALSLine The new ALS setting 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetALSLine = aALSLine;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetALSLine,iMmPtrHolder->iSetALSLine);

	Set(EMobilePhoneSetALSLine,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyALSLineChange(TRequestStatus& aReqStatus, TMobilePhoneALSLine& aALSLine) const

/** This function member allows a client to be notified if the currently selected 
ALS line changes.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyALSLineChange) 
to cancel a previously placed asynchronous NotifyALSLineChange() request.

@param aReqStatus On return, KErrNone if successful KErrNotSupported if the 
phone does not support access to ALS functionality.
@param aALSLine On return, the new ALS setting. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyALSLineChange,aALSLine);

	Get(EMobilePhoneNotifyALSLineChange,aReqStatus,ptr1);
	}


/***********************************************************************************/
//
// Mobile Network Security functional unit
// 
/***********************************************************************************/


EXPORT_C void RMobilePhone::GetNetworkSecurityLevel(TRequestStatus& aReqStatus, TMobilePhoneNetworkSecurity& aSecurity) const

/** This method returns the encryption level that is currently offered for the 
next MO or MT call by the network. The result is returned in the aSecurity 
parameter and will correspond to one of the values enumerated in the table 
above.

This method will return KErrNotSupported if the phone does not support network 
security level retrieval. 

@param aReqStatus Has the result code after the asynchronous call completes.
@param aSecurity Will contain the current Network Security Level. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetNetworkSecurityLevel,aSecurity);
	Get(EMobilePhoneGetNetworkSecurityLevel,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyNetworkSecurityLevelChange(TRequestStatus& aReqStatus, TMobilePhoneNetworkSecurity& aSecurity) const

/** This method allows a client to be notified if there is a change in the network 
security level. The new setting will be placed in the aSetting parameter and 
must take one of the values enumerated in section Network Security.

This method will return KErrNotSupported if the TSY does not support access 
to network security level functionality.

@param aReqStatus Has the result code after the asynchronous call completes.
@param aSecurity Will contain the new Network Security Level. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyNetworkSecurityLevelChange,aSecurity);

	Get(EMobilePhoneNotifyNetworkSecurityLevelChange,aReqStatus,ptr1);
	}
	
EXPORT_C void RMobilePhone::GetCipheringIndicatorStatus(TRequestStatus& aReqStatus, TMobileCallCipheringIndicator& aIndicatorStatus) const
/**
Retrieves the current setting of the ciphering indicator status. The indicator status is used to determine whether 
the ME indication for displaying ciphering status (encrypter/non-encrypted) of calls should be on or off.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aIndicatorStatus On completion will contain the ciphering status indicator.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetCipheringIndicatorStatus,aIndicatorStatus);

	Get(EMobilePhoneGetCipheringIndicatorStatus,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyCipheringIndicatorStatus(TRequestStatus& aReqStatus, TMobileCallCipheringIndicator& aIndicatorStatus) const
/**
Notifies the client of any change in the ciphering indication status.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyCipheringIndicatorStatus,aIndicatorStatus);

	Get(EMobilePhoneNotifyCipheringIndicatorStatus,aReqStatus,ptr1);	
	}

/***********************************************************************************/
//
// MobileCost functional unit
// 
/***********************************************************************************/


EXPORT_C TInt RMobilePhone::GetCostCaps(TUint32& aCaps) const
/** This function member gets a snapshot of the combination of the static and dynamic 
call cost capabilities of the phone.

@param aCaps On return, the sum of RMobilePhone::TMobilePhoneCostCaps constants.
@return On return, KErrNone if successful KErrNotSupported if the phone does 
not support access to any call cost functionality 
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetCostCaps,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyCostCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const
/** This request allows a client to be notified when the phone's call cost capabilities 
change - probably in response to a change of network mode.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCostCapsChange) 
to cancel a previously placed asynchronous NotifyCostCapsChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aCaps On return, the sum of RMobilePhone::TMobilePhoneCostCaps constants
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyCostCapsChange,aCaps);

	Get(EMobilePhoneNotifyCostCapsChange,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::ClearCostMeter(TRequestStatus& aReqStatus, TMobilePhoneCostMeters aMeter) const
/** This function member clears the cost meter specified by the aMeter parameter.

If PIN2 still requires verification before the ACM or CCM can be cleared, 
then the TSY will take the following approach.

TSY keeps ClearCostMeter request uncompleted while it looks for an outstanding 
NotifySecurityEvent() request. If it can not find one, then the ClearCostMeter 
request will be completed with the extended error KErrGsm0707SimPin2Required 
and core error KErrAccessDenied.

If TSY does find a NotifySecurityEvent request, it will trigger it and return 
the EPin2Required security event. The user should then be prompted to enter 
PIN2 and when he/she has done so, the VerifySecurityCode() function member 
will be used to send the PIN2 digits to the SIM via the TSY. Assuming the 
user has entered the correct digits, the TSY can successfully complete the 
VerifySecurityCode() request.

The TSY can then return to the ClearCostMeter request and send this to the 
SIM too. Once the SIM (or phone) has confirmed that the resetting of ACM (or 
CCM) was successful, the TSY will successfully complete the ClearCostMeter() 
request.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneClearCostMeter) to 
cancel a previously placed asynchronous ClearCostMeter() request.

@param aReqStatus On return, KErrNone if successful.
@param aMeter The cost meter to clear. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iClearCostMeter = aMeter;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1ClearCostMeter,iMmPtrHolder->iClearCostMeter);

	Set(EMobilePhoneClearCostMeter,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::SetMaxCostMeter(TRequestStatus& aReqStatus, TUint aUnits) const
/** This function member sets the maximum value that the call cost meter will be 
allowed to rise to.

If PIN2 still requires verification, then the TSY will follow the same approach 
as outlined for clearing of ACM.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetMaxCostMeter) to 
cancel a previously placed asynchronous SetMaxCostMeter() request.

@see ClearCostMeter()
@param aReqStatus On return, KErrNone if successful.
@param aUnits An integer value that represents the maximum number of charging 
units 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetMaxCostMeterUnits = aUnits;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetMaxCostMeter,iMmPtrHolder->iSetMaxCostMeterUnits);

	Set(EMobilePhoneSetMaxCostMeter,aReqStatus,ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhonePuctV1::TMobilePhonePuctV1() 
: iPricePerUnit(0.0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

	
EXPORT_C void RMobilePhone::SetPuct(TRequestStatus& aReqStatus, const TDesC8& aPuct) const
/** This function member sets the Price per Unit and Currency Table to the specified 
values.

If PIN2 is required before PUCT can be set, then the TSY will follow the same 
approach as outlined for clearing of ACM.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetPuct) to cancel 
a previously placed asynchronous SetPuct() request.

@see ClearCostMeter()
@param aReqStatus On return, KErrNone if successful.
@param aPuct The TMobilePhonePuctV1Pckg holding the pricing parameters.
@see TMobilePhonePuctV1 
@capability WriteDeviceData
*/
	{
	Set(EMobilePhoneSetPuct,aReqStatus,aPuct);
	}

EXPORT_C RMobilePhone::TMobilePhoneCostInfoV1::TMobilePhoneCostInfoV1() 
:	iService(ECostServiceUnknown), 
	iCCM(0), 
	iACM(0), 
	iACMmax(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}
	
EXPORT_C void RMobilePhone::GetCostInfo(TRequestStatus& aReqStatus, TDes8& aCostInfo) const
/** This function member retrieves a snapshot of the information associated with 
the call cost services.

This information includes current values of all the cost meters (CCM, ACM, 
ACMmax), the current currency settings and the current status of the call 
cost service ( i.e. which type, if any, is currently active).

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetCostInfo) to cancel 
a previously placed asynchronous GetCostInfo() request.

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the TSY 
is unable to return this information in the current mode.
@param aCostInfo On return, a TMobilePhoneCostInfoV1Pckg with the current cost 
information.
@see TMobilePhoneCostInfoV1 
@capability ReadUserData
*/
	{
	Get(EMobilePhoneGetCostInfo,aReqStatus,aCostInfo);
	}

EXPORT_C void RMobilePhone::NotifyCostInfoChange(TRequestStatus& aReqStatus, TDes8& aCostInfo) const
/** This function member allows a client to be notified when any part of the call 
cost information changes.

The CCM and ACM information will change most regularly, but it is also possible 
for the ACMmax or PUCT to change as a result of a client action or a network 
initiated update (e.g. data download via SMS to update the ACMmax).

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyCostInfoChange) 
to cancel a previously placed asynchronous NotifyCostInfoChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aCostInfo On return, a TMobilePhoneCostInfoV1Pckg with the current cost 
information.
@see TMobilePhoneCostInfoV1 
@capability ReadUserData
*/
	{
	Get(EMobilePhoneNotifyCostInfoChange,aReqStatus,aCostInfo);
	}


/***********************************************************************************/
//
// MobileSecurity functional unit
// 
/***********************************************************************************/

EXPORT_C TInt RMobilePhone::GetSecurityCaps(TUint32& aCaps) const
/** This function member gets a snapshot of the combination of the static and dynamic 
security capabilities of the phone.

@param aCaps On return, the sum of RMobilePhone::TMobilePhoneSecurityCaps constants.
@return KErrNone if successful, KErrNotSupported if the TSY/ME does not support 
client access to any security functionality. 
@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetSecurityCaps,ptr1);
	}

EXPORT_C void RMobilePhone::NotifySecurityCapsChange(TRequestStatus& aReqStatus, TUint32& aCaps) const
/** This request allows a client to be notified when the phone's security capabilities 
change - probably in response to a change of network mode.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifySecurityCapsChange) 
to cancel a previously placed asynchronous NotifySecurityCapsChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aCaps On return, the sum of RMobilePhone::TMobilePhoneSecurityCaps
constants.
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifySecurityCapsChange,aCaps);

	Get(EMobilePhoneNotifySecurityCapsChange,aReqStatus,ptr1);
	}

EXPORT_C RMobilePhone::TMobilePhoneLockInfoV1::TMobilePhoneLockInfoV1() 
:	iStatus(EStatusLocked), 
	iSetting(ELockSetEnabled)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::GetLockInfo(TRequestStatus& aReqStatus, TMobilePhoneLock aLock, TDes8& aLockInfo) const
/** This function member retrieves the current status and setting of a lock.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetLockInfo) to cancel 
a previously placed asynchronous GetLockInfo() request.

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the phone 
does not support the lock type requested, KErrNotSupported if the phone does 
not support access to information for any locks.
@param aLock The lock type whose information is to be retrieved.
@param aLockInfo On return, a TMobilePhoneLockInfoV1Pckg with the lock information.
@see TMobilePhoneLockInfoV1 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iLock = aLock;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetLockInfo,iMmPtrHolder->iLock);

	SetAndGet(EMobilePhoneGetLockInfo,aReqStatus,ptr1,aLockInfo);
	}

EXPORT_C void RMobilePhone::NotifyLockInfoChange(TRequestStatus& aReqStatus, TMobilePhoneLock& aLock, TDes8& aLockInfo) const
/** This function member allows a client to be notified when any part of the lock 
information changes

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyLockInfoChange) 
to cancel a previously placed asynchronous NotifyLockInfoChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aLock On return, the lock that changed.
@param aLockInfo On return, a TMobilePhoneLockInfoV1Pckg with the lock information.
@see TMobilePhoneLockInfoV1 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyLockInfoChange,aLock);
	
	Get(EMobilePhoneNotifyLockInfoChange,aReqStatus,ptr1,aLockInfo);
	}

EXPORT_C void RMobilePhone::SetLockSetting(TRequestStatus& aReqStatus, TMobilePhoneLock aLock, TMobilePhoneLockSetting aSetting) const
/** This function member allows a client to change the current setting of a lock.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetLockSetting) to 
cancel a previously placed asynchronous SetLockSetting() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if the 
phone does not support changing the setting for the lock requested, KErrNotSupported 
if the phone does not support changing the setting for any lock
@param aLock The lock to change
@param aSetting The new settings for the lock 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetLockSettingLock = aLock;
	iMmPtrHolder->iSetLockSettingSetting = aSetting;

	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetLockSetting,iMmPtrHolder->iSetLockSettingLock);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SetLockSetting,iMmPtrHolder->iSetLockSettingSetting);

	Set(EMobilePhoneSetLockSetting,aReqStatus,ptr1,ptr2);
	}

EXPORT_C void RMobilePhone::ChangeSecurityCode(TRequestStatus& aReqStatus, TMobilePhoneSecurityCode aType, const TMobilePhonePasswordChangeV1& aChange) const
/** This function member allows a client to change a security code. 

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneChangeSecurityCode) 
to cancel a previously placed asynchronous ChangeSecurityCode() request.

@param aReqStatus On return, KErrNone if successful, KErrAccessDenied if the 
old security code specified is incorrect, KErrNotSupported if the phone does 
not support changing the code requested.
@param aType The security code to be changed.
@param aChange The old and the new values of the security code. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iChangeSecurityCodeType = aType;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1ChangeSecurityCode,iMmPtrHolder->iChangeSecurityCodeType);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2ChangeSecurityCode,aChange);

	Set(EMobilePhoneChangeSecurityCode,aReqStatus,ptr1,ptr2);
	}

EXPORT_C RMobilePhone::TMobilePhoneSecurityCodeInfoV5::TMobilePhoneSecurityCodeInfoV5() 
	{
	iExtensionId=KEtelExtMultimodeV5;
	}


EXPORT_C void RMobilePhone::GetSecurityCodeInfo(TRequestStatus& aReqStatus, TMobilePhoneSecurityCode aSecurityCode, TDes8& aSecurityCodeInfo) const
/** This function member allows a client to retrieve the current number or remaining entry attempts of a
security code. 

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetSecurityCodeInfo) 
to cancel a previously placed asynchronous GetSecurityCodeInfo() request.

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the phone
does not support the security code type requested, KErrNotSupported if the phone
does not support access to information for any security codes.
@param aSecurityCode The security code whose information is to be retrieved.
@param aSecurityCodeInfo On return, a TMobilePhoneSecurityCodeInfoV5Pckg with the security code information.
@see TMobilePhoneSecurityCodeInfoV5
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iGetSecurityCodeInfoType = aSecurityCode;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetSecurityCodeInfo,iMmPtrHolder->iGetSecurityCodeInfoType);

	SetAndGet(EMobilePhoneGetSecurityCodeInfo,aReqStatus,ptr1,aSecurityCodeInfo);
	}
	
EXPORT_C void RMobilePhone::NotifySecurityCodeInfoChange(TRequestStatus& aReqStatus, TMobilePhoneSecurityCode& aSecurityCode, TDes8& aSecurityCodeInfo) const
/** This function member allows a client to be notified when the security code 
information changes

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifySecurityCodeInfoChange) 
to cancel a previously placed asynchronous NotifySecurityCodeInfoChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aSecurityCode On return, the security code that changed.
@param aSecurityCodeInfo On return, a TMobilePhoneSecurityCodeInfoV5Pckg with the security code information.
@see TMobilePhoneSecurityCodeInfoV5 
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifySecurityCodeInfoChange,aSecurityCode);
	
	Get(EMobilePhoneNotifySecurityCodeInfoChange,aReqStatus,ptr1,aSecurityCodeInfo);
	}


EXPORT_C void RMobilePhone::NotifySecurityEvent(TRequestStatus& aReqStatus, TMobilePhoneSecurityEvent& aEvent) const
/** This function member allows a client to be notified when the phone has detected 
the occurrence of one of the security related events.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifySecurityEvent) 
to cancel a previously placed asynchronous NotifySecurityEvent() request.

@param aReqStatus On return, KErrNone if successful.
@param aEvent On return, the security event detected. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifySecurityEvent,aEvent);

	Get(EMobilePhoneNotifySecurityEvent,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::VerifySecurityCode(TRequestStatus& aReqStatus, TMobilePhoneSecurityCode aType, const TMobilePassword& aCode,
		const TMobilePassword& aUnblockCode) const
/** This function member sends a security code requiring verification to the phone.

If the security code required is one of the unblock codes (either PUK1 or 
PUK2), then the user must supply a new PIN1 or PIN2 code as well as the associated 
unblocking code. In this case, aUnblockCode will contain the unblocking code 
and aCode will contain the new PIN1 or PIN2 value. If the security code required 
is NOT one of the unblock codes, then the user only has to supply the one 
security code. In this case, aUnblockCode will be invalid and aCode will contain 
the required security code (PIN1, PIN2 or Phone Password). 

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneVerifySecurityCode) 
to cancel a previously placed asynchronous VerifySecurityCode() request.

@param aReqStatus On return, KErrNone if successful, KErrAccessDenied if the 
code is incorrect, KErrNotReady if the phone is not waiting for a code, KErrArgument
if the code is not well formatted.
@param aType The type of code that is to be verified.
@param aCode The actual code.
@param aUnblockCode If a PUK1 or PUK2 is specified, the PUK code. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iVerifySecurityCodeType = aType;
	iMmPtrHolder->iVerifySecurityCodeData.iCode = aCode;
	iMmPtrHolder->iVerifySecurityCodeData.iUnblockCode = aUnblockCode;
	
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1VerifySecurityCode,iMmPtrHolder->iVerifySecurityCodeType);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2VerifySecurityCode,iMmPtrHolder->iVerifySecurityCodeData);

	Set(EMobilePhoneVerifySecurityCode,aReqStatus,ptr1,ptr2);	
	}

EXPORT_C TInt RMobilePhone::AbortSecurityCode(TMobilePhoneSecurityCode aType) const
/** This function informs the phone that the user has cancelled an outstanding 
"get security code" request.

@param aType Specifies which code request has been cancelled.
@return KErrNone if successful. 
@capability WriteDeviceData
*/
	{
	TPckg<TMobilePhoneSecurityCode> ptr1(aType);
	return Set(EMobilePhoneAbortSecurityCode,ptr1);
	}

/***********************************************************************************/
//
// MobileMulticall functional unit
// 
/***********************************************************************************/

EXPORT_C RMobilePhone::TMobilePhoneMulticallSettingsV1::TMobilePhoneMulticallSettingsV1()
	: iUserMaxBearers(-1),iServiceProviderMaxBearers(-1),iNetworkSupportedMaxBearers(-1),
	iUESupportedMaxBearers(-1)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobilePhone::GetMulticallParams(TRequestStatus& aReqStatus, TDes8& aMulticallParams) const
/** This function member retrieves the maximum number of simultaneous CS service 
bearers defined by the user and the service provider, and supported by the 
network and the phone.

The maximum number defined by 3GPP Release '99 of simultaneous mobile originating 
or mobile terminating CS calls is seven.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetMulticallParams) 
to cancel a previously placed asynchronous GetMulticallParams() request.

@param aReqStatus On return, KErrNone if successful.
@param aMulticallParams On return, a TMobilePhoneMulticallSettingsV1Pckg containing 
the multicall parameters.
@see TMobilePhoneMulticallSettingsV1 
@capability None
*/
	{
	Get(EMobilePhoneGetMulticallParams, aReqStatus, aMulticallParams);	
	}

EXPORT_C void RMobilePhone::SetMulticallParams(TRequestStatus& aReqStatus, TInt aUserMaxBearers) const
/** This function member is used by client application to set the maximum number 
of simultaneous CS bearers specified by the user. 

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetMulticallParams) 
to cancel a previously placed asynchronous SetMulticallParams() request.

@param aReqStatus On return, KErrNone if successful, KErrMMEtelUserMaxBearersNotAllowed 
if the number requested is not allowed.
@param aUserMaxBearers The number of maximum simultaneous CS bearers, this 
has to be less than or equal to the maximum number of bearers supported by 
the MS/service provider/serving network. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iUserMaxBearers = aUserMaxBearers;
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetMulticallParams, iMmPtrHolder->iUserMaxBearers);

	Set(EMobilePhoneSetMulticallParams,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyMulticallParamsChange(TRequestStatus& aReqStatus, TDes8& aMulticallParams) const
/** This function member allows the client to be notified of changes to any of 
the multicall parameter settings.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyMulticallParamsChange) 
to cancel a previously placed asynchronous NotifyMulticallParamsChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aMulticallParams On return, a TMobilePhoneMulticallSettingsV1Pckg containing 
the multicall parameters.
@see TMobilePhoneMulticallSettingsV1 
@capability None
*/
	{
	Get(EMobilePhoneNotifyMulticallParamsChange, aReqStatus, aMulticallParams);
	}
	
EXPORT_C void RMobilePhone::NotifyMulticallIndicatorChange(TRequestStatus& aReqStatus, TMobilePhoneMulticallIndicator& aMulticallIndicator) const
/**
Notifies the client of changes to the multicall indicator.

@param aReqStatus returns the result code after the asynchronous call completes.
@param aMulticallIndicator On completion contains the new TMobilePhoneMulticallIndicator value.

@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyMulticallIndicatorChange, aMulticallIndicator);
	Get(EMobilePhoneNotifyMulticallIndicatorChange, aReqStatus, ptr1);
	}

/***********************************************************************************/
//
// MobileMessageWaiting functional unit
// 
/***********************************************************************************/

EXPORT_C RMobilePhone::TMobilePhoneMessageWaitingV1::TMobilePhoneMessageWaitingV1()
: iDisplayStatus(0), iVoiceMsgs(0), iAuxVoiceMsgs(0), iDataMsgs(0), iFaxMsgs(0), 
  iEmailMsgs(0), iOtherMsgs(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}
	
EXPORT_C RMobilePhone::TMobilePhoneMessageWaitingV8::TMobilePhoneMessageWaitingV8()
:  TMobilePhoneMessageWaitingV1(), iVideoMsgs(0)
	{
	iExtensionId=KEtelExtMultimodeV8;
	}
	
EXPORT_C void RMobilePhone::NotifyMessageWaiting(TRequestStatus& aReqStatus, TInt& aCount) const
/** This function member allows a client to be notified when the phone receives 
a message waiting indication from the network. The request will be completed 
when the TSY receives the indication from the phone.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyMessageWaiting) 
to cancel a previously placed asynchronous NotifyMessageWaiting() request.

@param aReqStatus On return, KErrNone if successful.
@param aCount On return, the number of voice mail messages waiting in the network. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyMessageWaiting,aCount);

	Get(EMobilePhoneNotifyMessageWaiting,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::GetIccMessageWaitingIndicators(TRequestStatus& aReqStatus, TDes8& aMsgIndicators) const
/** This function member retrieves the set of message waiting indicators from the 
current ICC.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetIccMessageWaitingIndicators) 
to cancel a previously placed asynchronous GetIccMessageWaitingIndicators() 
request.

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the indicators 
are not present on the ICC.
@param aMsgIndicators On return, a TMobilePhoneMessageWaitingV1Pckg containing 
the ICC message waiting indicators.
@see TMobilePhoneMessageWaitingV1 
@capability ReadDeviceData
*/
	{
	Get(EMobilePhoneGetIccMessageWaitingIndicators,aReqStatus,aMsgIndicators);
	}

EXPORT_C void RMobilePhone::SetIccMessageWaitingIndicators(TRequestStatus& aReqStatus, const TDesC8& aMsgIndicators) const
/** This member function sets the message waiting indicators on the current ICC.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetIccMessageWaitingIndicators) 
to cancel a previously placed asynchronous SetIccMessageWaitingIndicators() 
request.

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the indicators 
are not present on the ICC.
@param aMsgIndicators An instance of TMobilePhoneMessageWaitingV1Pckg containing 
the message waiting indicators to set.
@see TMobilePhoneMessageWaitingV1 
@capability WriteDeviceData
*/
	{
	Set(EMobilePhoneSetIccMessageWaitingIndicators,aReqStatus,aMsgIndicators);
	}

EXPORT_C void RMobilePhone::NotifyIccMessageWaitingIndicatorsChange(TRequestStatus& aReqStatus, TDes8& aMsgIndicators) const
/** This function member notifies the client if the message waiting indicators 
on the current ICC change.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange) 
to cancel a previously placed asynchronous NotifyIccMessageWaitingIndicatorsChange() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aMsgIndicators On return, a TMobilePhoneMessageWaitingV1Pckg containing 
the ICC message waiting indicators.
@see TMobilePhoneMessageWaitingV1 
@capability ReadDeviceData
*/
	{
	Get(EMobilePhoneNotifyIccMessageWaitingIndicatorsChange,aReqStatus,aMsgIndicators);
	}

/***********************************************************************************/
//
// MobileFixedDiallingNumbers functional unit
// 
/***********************************************************************************/


EXPORT_C TInt RMobilePhone::GetFdnStatus(TMobilePhoneFdnStatus& aFdnStatus) const
/** This function member retrieves the current status of the Fixed Dialling Numbers 
service.

@param aFdnStatus On return, the fixed dialling number status.
@return KErrNone if successful. 
@capability None
*/
	{
	TPckg<TMobilePhoneFdnStatus> ptr1(aFdnStatus);

	return Get(EMobilePhoneGetFdnStatus,ptr1);
	}

EXPORT_C void RMobilePhone::GetFdnStatus(TRequestStatus& aReqStatus, TMobilePhoneFdnStatus& aFdnStatus) const
/**
 * This new overloaded asynchronous method returns the status of the Fixed Dialling Number (FDN) service
 * This allows the FDN status to be retrieved asynchronously in case (as it is with 3G cards) it is PIN1
 * protected and can only be determined after the pin has been successfully entered.
 *
 * @param aReqStatus returns the result code after the asynchronous call completes
 * @param aFdnStatus Will contain the current status of FDN
 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetFdnStatus, aFdnStatus);

	Get(EMobilePhoneGetFdnStatus, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::SetFdnSetting(TRequestStatus& aReqStatus, TMobilePhoneFdnSetting aFdnSetting) const
/** This function member activates (ON) or deactivates (OFF) the Fixed Dialling 
Numbers (FDN) service.

PIN2 must have been verified prior to changing the Fixed Dialling Numbers 
setting. If PIN2 has not been verified when this function member is called, 
then the TSY should follow the same approach as in ClearCostMeter().

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetFdnSetting) to cancel 
a previously placed asynchronous SetFdnSetting() request.

@param aReqStatus On return, KErrNone if successful.
@param aFdnSetting The new setting of the FDN service. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetFdnSetting = aFdnSetting;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetFdnSetting,iMmPtrHolder->iSetFdnSetting);
	
	Set(EMobilePhoneSetFdnSetting,aReqStatus,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyFdnStatusChange(TRequestStatus& aReqStatus, TMobilePhoneFdnStatus& aFdnStatus) const
/** This request allows a client to be notified when the status of the FDN service 
has changed.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyFdnStatusChange) 
to cancel a previously placed asynchronous NotifyFdnStatusChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aFdnStatus On return, the new FDN setting. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyFdnStatusChange,aFdnStatus);

	Get(EMobilePhoneNotifyFdnStatusChange,aReqStatus,ptr1);
	}

/************************************************************************************/
//
// MobileNextIncomingCall functional unit
//
/************************************************************************************/


EXPORT_C void RMobilePhone::GetIncomingCallType(TRequestStatus& aReqStatus, TMobilePhoneIncomingCallType& aCallType, TDes8& aDataParams) const
/** This function member retrieves the current setting of the incoming call type.

This setting is over-ridden by the alternating call mode setting such that, 
if the alternating call mode setting specifies an alternating mode, then the 
MS will request an alternating call for the next incoming call. If the alternating 
call mode setting specifies single mode, then this incoming call type setting 
applies.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetIncomingCallType) 
to cancel a previously placed asynchronous GetIncomingCallType() request.

@param aReqStatus On return, KErrNone if successful.
@param aCallType On return, the incoming call type.
@param aDataParams If aCallType equals EIncomingData then, on return, the aDataParams 
parameter will contain the data bearer service settings as a RMobileCall::TMobileDataCallParamsV1Pckg. 

@see RMobileCall::TMobileDataCallParamsV1 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetIncomingCallType, aCallType);

	Get(EMobilePhoneGetIncomingCallType, aReqStatus, ptr1, aDataParams);
	}

EXPORT_C void RMobilePhone::SetIncomingCallType(TRequestStatus& aReqStatus, TMobilePhoneIncomingCallType aCallType, TDes8& aDataParams) const
/** This function member sets the desired incoming call type.

This setting is over-ridden by the alternating call mode setting such that, 
if the alternating call mode setting specifies an alternating mode, then the 
MS will request an alternating call for the next incoming call. If the alternating 
call mode setting specifies single mode, then this incoming call type setting 
applies.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetIncomingCallType) 
to cancel a previously placed asynchronous SetIncomingCallType() request.

@param aReqStatus On return, KErrNone if successful.
@param aCallType The call type.
@param aDataParams If aCallType equals EIncomingData then the aDataParams parameter 
will contain the data bearer service settings as an instance of RMobileCall::TMobileDataCallParamsV1Pckg. 

@see RMobileCall::TMobileDataCallParamsV1 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iCallType = aCallType;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetIncomingCallType, iMmPtrHolder->iCallType);

	Set(EMobilePhoneSetIncomingCallType, aReqStatus, ptr1, aDataParams);
	}

EXPORT_C void RMobilePhone::NotifyIncomingCallTypeChange(TRequestStatus& aReqStatus, TMobilePhoneIncomingCallType& aCallType, TDes8& aDataParams) const

/** This function member notifies the client if the setting of the incoming call 
type changes.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyIncomingCallTypeChange) 
to cancel a previously placed asynchronous NotifyIncomingCallTypeChange() 
request.

@param aReqStatus On return, KErrNone if successful.
@param aCallType On return, the call type.
@param aDataParams If aCallType equals EIncomingData then, on return, the aDataParams 
parameter will contain the data bearer service settings RMobileCall::TMobileDataCallParamsV1Pckg. 

@see RMobileCall::TMobileDataCallParamsV1 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyIncomingCallTypeChange, aCallType);

	Get(EMobilePhoneNotifyIncomingCallTypeChange, aReqStatus, ptr1, aDataParams);
	}

/************************************************************************************/
//
// MobileUserSignalling functional unit
//
/************************************************************************************/


EXPORT_C void RMobilePhone::GetUUSSetting(TRequestStatus& aReqStatus, TMobilePhoneUUSSetting& aSetting) const
/** This function member retrieves the current UUS settings of the phone. The UUS 
settings enable a user to accept or reject all incoming UUS requests.

These are phone level settings and the user can not modify them on a per-call 
basis.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetUUSSetting) to cancel 
a previously placed asynchronous GetUUSSetting() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if he 
TSY/ME does not support any UUS functionality
@param aSetting On return, the UUS settings. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetUUSSetting, aSetting);
	Get(EMobilePhoneGetUUSSetting, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::SetUUSSetting(TRequestStatus& aReqStatus, TMobilePhoneUUSSetting aSetting) const
/** This function member sets the UUS settings of the phone.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetUUSSetting) to cancel 
a previously placed asynchronous SetUUSSetting() request.

@param aReqStatus On return, KErrNone if successful, KErrNotSupported if the 
TSY/ME does not support any UUS functionality.
@param aSetting The new UUS setting 
@capability NetworkServices
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetUUSSetting = aSetting;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetUUSSetting, iMmPtrHolder->iSetUUSSetting);
	
	Set(EMobilePhoneSetUUSSetting, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyUUSSettingChange(TRequestStatus& aReqStatus, TMobilePhoneUUSSetting& aSetting) const
/** This function member notifies the clients of changes to the UUS settings of 
the phone.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyUUSSettingChange) 
to cancel a previously placed asynchronous NotifyUUSSettingChange() request.

@param aReqStatus On return, KErrNone if successful.
@param aSetting On return, the new UUS setting. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyUUSSettingChange,aSetting);

	Get(EMobilePhoneNotifyUUSSettingChange, aReqStatus, ptr1);
	}

/************************************************************************************/
//
// Mobile USIM Applications functional unit
//
/************************************************************************************/

EXPORT_C void RMobilePhone::EnumerateUSimApplications(TRequestStatus& aReqStatus, TInt& aCount, TAID& aActiveAID) const
/** This method enumerates the USIM applications available on the UICC and returns 
the information in aCount parameter. This method also return the AID of the 
currently active USIM Application in the aActiveAID parameter (returns NULL 
if there is no active USIM Application).

To retrieve information about a specific USIM Application, the client should 
call GetUSimApplicationInfo().

This method is asynchronous because it is assumed that the TSY needs to access 
the ICC to retrieve this information.

@param aReqStatus On completion this contains the result code of the asynchronous 
call.
@param aCount Will contain the total number of USIM applications.
@param aActiveAID Will contain the AID of the currently active USIM application 
(NULL if there is no active USIM Application). 
@capability None
@deprecated 9.1 Replaced with @see RMobilePhone::EnumerateSmartCardApplications
*/
	{
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1EnumerateUSimApplications, aCount);
	TPtr8& ptr2 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2EnumerateUSimApplications, aActiveAID);

	Get(EMobilePhoneEnumerateUSimApplications,aReqStatus, ptr1, ptr2);
	}

EXPORT_C void RMobilePhone::SetUSimApplicationStatus(TRequestStatus& aReqStatus, const TAID aAID, TUSimAppAction aUSimAppAction) const
 /** This API method sets the USIM Application (identified by its AID) status, by 
	performing the action passed in aUsimAppAction parameter.
	
	Only one USIM Application can be active at any one time. Calling this method 
	to activate a USIM Application when another one is already active would result 
	in terminating the currently active USIM Application and activating the new 
	one.
	
	Once the USIM Application is active, the retrieval of information in USIM 
	specific files using ETel MM API methods will by default retrieve the information 
	in the currently active USIM Application.
	
	Activation of a USIM Application implies that the GSM session is no longer 
	active. To revive the GSM session, the currently acitve USIM Application should 
	be terminated.
	
	This method is asynchronous because the TSY requires to access the UICC.
	
	@param aReqStatus Upon completion this contains the result code of the asynchronous 
	call.
	@param aAID Supplies the new settings for the USIM Application identifier.
	@param aUsimAppAction Supplies the action for this USIM Application. 
@capability WriteDeviceData
@deprecated 9.1 Replaced with @see RMobilePhone::SetSmartCardApplicationStatus
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iUSimAppAction = aUSimAppAction;
	iMmPtrHolder->iAID = aAID;

	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetUSimApplicationStatus, iMmPtrHolder->iUSimAppAction);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SetUSimApplicationStatus, iMmPtrHolder->iAID);

	Set(EMobilePhoneSetUSimApplicationStatus,aReqStatus,ptr2,ptr1);
	}


EXPORT_C RMobilePhone::TUSimApplicationInfoV2::TUSimApplicationInfoV2()
	: iLabel(0)
/** Default constructor. This sets the USIM Application label to NULL. */
	{
	iExtensionId=KETelExtMultimodeV2;
	}

EXPORT_C void RMobilePhone::GetUSimApplicationInfo(TRequestStatus& aReqStatus, TInt aUSimAppIndex, TDes8& aInfo) const
/** This method retrieves information about the different USIM Applications on 
the UICC. This information is extracted from EF_DIR.

This method is asynchronous because the TSY requires to access the UICC.

@param aReqStatus On completion this contains the result code of the asynchronous 
call.
@param aUSimAppIndex Specifies which USIM Application to get the info for and 
is relative to the application count returned in EnumerateUSimApplications().
@param aInfo A packaged instance of TUSimApplicationInfoV2. 
@capability None
@deprecated 9.1 Replaced with @see RMobilePhone::GetSmartCardApplicationInfo
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iUSimAppIndex = aUSimAppIndex;
	
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetUSimApplicationInfo, iMmPtrHolder->iUSimAppIndex);

	SetAndGet(EMobilePhoneGetUSimApplicationsInfo, aReqStatus, ptr1,aInfo);
	}


EXPORT_C void RMobilePhone::NotifyUSimApplicationsInfoChange(TRequestStatus& aReqStatus, TInt& aCount, TAID& aActiveAID) const 
/** This method notifies the clients of changes to the USIM Application(s). Upon 
completion of this notification, the total number of USIM Applications on 
the UICC is returned as well as the AID of the currently active USIM Application.

@param aReqStatus Upon completion this contains the result code of the asynchronous 
call.
@param aCount Upon completion this is the total number of USIM Applications 
on the UICC
@param aActiveAID Upon completion this is the AID of the currently active USIM 
Application. 
@capability None
@deprecated 9.1 Replaced with @see RMobilePhone::NotifySmartCardApplicationChange
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyUSimApplicationsInfoChange, aCount);
	TPtr8& ptr2 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2NotifyUSimApplicationsInfoChange, aActiveAID);

	Get(EMobilePhoneNotifyUSimApplicationsInfoChange, aReqStatus,ptr1,ptr2);
	}

EXPORT_C TInt RMobilePhone::GetUSimAppsSelectionMode(TUSimSelectionMode& aUSimSelectionMode) const
/** This method allows the client to retrieve the USIM Application selection mode.

It is synchronous because the TSY keeps a cached version.

@param aUSimSelectionMode Upon completion this will be the USIM Application 
selection mode.
@return An error code that will be KErrNotSupported if USIM Application selection 
functionality is not supported by the phone/TSY. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPckg<TUSimSelectionMode> ptr1(aUSimSelectionMode);

	return Get(EMobilePhoneGetUSimAppsSelectionMode,ptr1);
	}

EXPORT_C TInt RMobilePhone::SetUSimAppsSelectionMode(TUSimSelectionMode aUSimSelectionMode) const

/** This API method allows the client to set the USIM Application selection mode. 
When the selection mode is automatic, the ME shall use the "last selected 
USIM Application" stored in the UICC or in licensee stack to select the 
USIM Application at device start up.

@param aUSimSelectionMode The new settings for the USIM Application selection 
mode.
@return An error code that will be KErrNotSupported if USIM Application selection 
functionality is not supported by the phone/TSY. 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPckg<TUSimSelectionMode> ptr1(aUSimSelectionMode);

	return Set(EMobilePhoneSetUSimAppsSelectionMode,ptr1);
	}

EXPORT_C void RMobilePhone::NotifyUSimAppsSelectionModeChange(TRequestStatus& aReqStatus, TUSimSelectionMode& aUSimSelectionMode) const
/** The method allows a client to be notified of changes to the USIM Application 
selection mode.

@param aReqStatus Upon completion this contains the result code of the asynchronous 
call.
@param aUSimSelectionMode Upon completion of this notification, this will be 
the new USIM Application selection mode. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyUSimAppsSelectionModeChange, aUSimSelectionMode);

	Get(EMobilePhoneNotifyUSimAppsSelectionModeChange, aReqStatus, ptr1);
	}
	
EXPORT_C void RMobilePhone::GetCurrentActiveUSimApplication(TRequestStatus& aReqStatus, TAID& aAID) const
/**
Returns the active USIM application.  

There is only ever at most one USIM application active at a time. The AID of the active USIM is returned in aAID.  

@param aReqStatus Upon completion this contains the result code of the asynchronous call.
@param aAID Upon completion this is the AID of the currently active USIM Application. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetCurrentActiveUSimApplication, aAID);
	
	Get(EMobilePhoneGetCurrentActiveUSimApplication, aReqStatus, ptr1);	
	}
	
/************************************************************************************/
//
// Mobile Smart Card Applications Functional Unit
//
/************************************************************************************/

EXPORT_C RMobilePhone::TSmartCardApplicationInfoV5::TSmartCardApplicationInfoV5() :
	iAID(0), iLabel(0)
/** Default constructor. */
	{
	iExtensionId=KEtelExtMultimodeV5;
	}

/**
Default constructor for info with EAP support.
*/
EXPORT_C RMobilePhone::TUiccApplicationEapInfoV6::TUiccApplicationEapInfoV6()
	{
	iExtensionId = KEtelExtMultimodeV6;
	}

EXPORT_C RMobilePhone::TScFileInfoV5::TScFileInfoV5() :
	iFileID(0), iType(EUnknown), iTotalLength(0), iRecordLength(0), iNumberRecords(0)
/** Default constructor. */
	{
	iExtensionId=KEtelExtMultimodeV5;
	}
	
EXPORT_C RMobilePhone::TScFilePathWithAccessOffsets::TScFilePathWithAccessOffsets() :
	TScFilePath(), iType(RMobilePhone::EUnknown), iStartOffset(0), iCount(0)
/** Default constructor. */
	{
	//Empty	
	}
	
EXPORT_C RMobilePhone::TScFilePath::TScFilePath() :
	iAID(0), iPath(0)
/** Default constructor. */
	{
	//Empty	
	}	

/**
Enumerates the smart card applications available on the smart card and returns 
the information in aCount parameter. 

To retrieve information about a specific smart card application the client should 
call GetSmartCardApplicationInfo().

This method is asynchronous because it is assumed that the TSY needs to access 
the smart card to retrieve this information.

@param aReqStatus On completion this contains the result code of the asynchronous call.
@param aCount On completion this contains the total number of smart card applications.

@capability None
*/
EXPORT_C void RMobilePhone::EnumerateSmartCardApplications(TRequestStatus& aReqStatus, TUint& aCount) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
		
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1EnumerateSmartCardApplications, aCount);
	
	Get(EMobilePhoneEnumerateSmartCardApplications,aReqStatus, ptr1);
	}
	
/**
Retrieves information about the different smart card applications on
the smart card.

TSmartCardApplicationInfoV5 provides generic information for all smart
card applications, whereas TEapUiccApplicationInfoV6 is a specialised
data structure, which as well as providing a generic smart card
information, provides further information about the EAP support in the
particular UICC application. Please note that only the clients wanting
to gather the EAP capabilities of an UICC application should pass in a
packaged instance of TEapUiccApplicationInfoV6, otherwise
TSmartCardApplicationInfoV5 is sufficient.

This information is extracted from EF_DIR.

This method is asynchronous because the TSY requires access to the smart card.

@param aReqStatus On completion this contains the result code of the
                  asynchronous call.
@param aAppIndex Specifies which application to get the info for and is
                 relative to the application count returned in
                 RMobilePhone::EnumerateSmartCardApplications().
@param aInfo A packaged instance of either TSmartCardApplicationInfoV5
             or TEapUiccApplicationInfoV6.

@see RMobilePhone::EnumerateSmartCardApplications()

@capability None
*/
EXPORT_C void RMobilePhone::GetSmartCardApplicationInfo(TRequestStatus& aReqStatus, TUint aAppIndex, TDes8& aInfo) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iScAppIndex = aAppIndex;
	
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetSmartCardApplicationInfo, iMmPtrHolder->iScAppIndex);

	SetAndGet(EMobilePhoneGetSmartCardApplicationInfo, aReqStatus, ptr1,aInfo);
	}

EXPORT_C void RMobilePhone::NotifySmartCardApplicationInfoChange(TRequestStatus& aReqStatus, TAID& aAID, TSmartCardApplicationEvent& aEvent) const
/**
Notifies the clients of changes to the smart card application(s).
Upon completion of this notification, the event and the AID to which it applies is returned.

@param aReqStatus Upon completion this contains the result code of the asynchronous call.
@param aAID On completion this is the AID to which the aEvent applies.
@param aEvent On completion provides the TSmartCardApplicationEvent event that caused the notification.
@capability None
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot2NotifySmartCardApplicationInfoChange, aAID);
	TPtr8& ptr2 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifySmartCardApplicationInfoChange, aEvent);
		
	Get(EMobilePhoneNotifySmartCardApplicationInfoChange, aReqStatus,ptr1,ptr2);
	}
	


EXPORT_C void RMobilePhone::SetSmartCardApplicationStatus(TRequestStatus& aReqStatus, const TAID& aAID, TSmartCardApplicationAction aAppAction) const
 /** 
Sets the smart card application (identified by its AID) status, by performing the 
action passed in aAppAction parameter.

In WCDMA mode there may be at most four applications active at any one time.  Activation of an application 
on a logical channel that has already got an active application will result in the previous application being deactivated.

A further restriction is that there may be only one USIM Application active at any one time. 
Calling this method to activate a USIM Application when another one is already active would result in 
terminating the currently active USIM Application and activating the new one.

Once the USIM Application is active, the retrieval of information in USIM specific files 
using ETel MM API methods will by default retrieve the information in the currently active USIM Application.

Activation of a USIM Application implies that the GSM session is no longer active. To revive the GSM session, the currently active USIM Application should be terminated.
	
@param aReqStatus Upon completion this contains the result code of the asynchronous call. KErrMMEtelScMaxApplicationsActive is returned if the maximum number of applications are already active. 
@param aAID AID of the smart card application.
@param aAppAction Supplies the action to be applied to this smart card application. 
@capability WriteDeviceData
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iScAppAction = aAppAction;

	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot2SetSmartCardApplicationStatus, aAID);
	TPtrC8& ptr2 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetSmartCardApplicationStatus, iMmPtrHolder->iScAppAction);
	
	Set(EMobilePhoneSetSmartCardApplicationStatus,aReqStatus,ptr1,ptr2);
	}
	
EXPORT_C void RMobilePhone::GetScFileInfo(TRequestStatus& aReqStatus, const TScFilePath& aFilePathId, TDes8& aInfo) const
/** 
Returns information relating to the smart card file.  The file is identified by specifying a TScFilePath containing
the parent application AID (passed by the iAID attribute) and its file path (passed by the iPath attribute). 

The iAID parameter is required to distinguish the same named files under different parent applications.

Returned is a packaged TScFileInfoV5 parameter class which details the 2 byte file ID, file type, total file size,  
number of records and record size (if file is record based).

The parent application as defined in iAID must be active prior to calling GetScFileInfo.  An application can be 
activated by calling RMobilePhone::SetSmartCardApplicationStatus.  If the application is not active the call will complete with KErrMMEtelScApplicationNotActive.

@param aReqStatus Upon completion this contains the result code of the asynchronous call. If the application is not active KErrMMEtelScApplicationNotActive will be returned.
@param aFilePathId TScFilePath containing the AID of the smart card application and the path of the file on which to retrieve information.
@param aInfo Packaged TScFileInfoV5 containing file information.
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetScFileInfo, aFilePathId);

	SetAndGet(EMobilePhoneGetScFileInfo,aReqStatus, ptr1, aInfo);		
	}
	
EXPORT_C void RMobilePhone::ReadScFile(TRequestStatus& aReqStatus, const TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8& aReadBuffer) const
/**
Reads data from a specified file in a smart card application. 

It is expected that a prior call to RMobilePhone::GetScFile is made in order to determine the file type, file size and (if applicable) 
number of records and record size which may then be used to correctly determine a suitable buffer size.  

The parent application as defined by the AID in aFilePathOffsets must be active prior to calling ReadScFile. An application can 
be activated by calling RMobilePhone::SetSmartCardApplicationStatus. If the application is not active the call will complete with KErrMMEtelScApplicationNotActive.

The following scenarios apply to reading data from a smart card file:
If file type is ELinearFixed or ECyclic then this will use one or more calls to the smart card READ RECORD command to read 
iScCount consecutive records starting at iStartOffset.       

If file type is ETransparent then this will use the smart card READ BINARY command to read iScCount bytes starting at 
iStartOffset.

It is the responsibility of the client to set up the correct size of the buffer (e.g. no records to
read * record length) corresponding to the number of records or bytes they wish to read.  If they have 
requested to read multiple records the records are appended into the buffer and should be extracted copying 
iRecordLength bytes at a time.

This API does not cover decoding the data elements from a record.  This is specific to the file that has been read and its byte format is defined in the 3GPP specs.

@param aReqStatus Upon completion this contains the result code of the asynchronous call.  If the application is not active KErrMMEtelScApplicationNotActive will be returned.
@param aFilePathOffsets TScFilePathWithAccessOffsets which identfies the AID, full path reference and access offsets of the file to be read.
@param aReadBuffer On completion will contain the data read from the file.
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1ReadScFile, aFilePathOffsets);

	SetAndGet(EMobilePhoneReadScFile,aReqStatus, ptr1, aReadBuffer);
	}
	
EXPORT_C void RMobilePhone::UpdateScFile(TRequestStatus& aReqStatus, const TScFilePathWithAccessOffsets& aFilePathOffsets, TDes8& aUpdateBuffer) const
/**
Updates data in a specified file in a smart card application. 

It is expected that a prior call to RMobilePhone::GetScFile is made in order to determine the file type, file size and (if applicable) 
number of records and record size which may then be used to correctly determine a suitable buffer size.  

The parent application as defined by the AID in aFilePathOffsets must be active prior to calling ReadScFile. An application can 
be activated by calling RMobilePhone::SetSmartCardApplicationStatus.  If the application is not active the call will complete with KErrMMEtelScApplicationNotActive.

The following scenarios apply to updating data in a smart card file:
If file type is ELinearFixed or ECyclic then this will use one or more calls to UPDATE RECORD command to write 
iScCount consecutive records starting at iStartOffset.       

If file type is ETransparent then this will use the UPDATE BINARY command to write iScCount bytes starting at 
iStartOffset.

It is the responsibility of the client to set up the correct size of the buffer (e.g. no records to
update * record length) corresponding to the number of records or bytes they wish to update.  If they have 
requested to update multiple records the records are appended into the buffer and should be extracted copying 
iRecordLength bytes at a time.

This API does not cover encoding the data elements into a record.  This is specific to the file that is being updated and its byte format is defined in the 3GPP specs.

@param aReqStatus Upon completion this contains the result code of the asynchronous call.  If the application is not active KErrMMEtelScApplicationNotActive will be returned.
@param aFilePathOffsets TScFilePathWithAccessOffsets which identfies the AID, full path reference and access offsets of the file to be read.
@param aUpdateBuffer Contains data to be written to the file.
@capability WriteDeviceData
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1UpdateScFile, aFilePathOffsets);

	SetAndGet(EMobilePhoneUpdateScFile,aReqStatus, ptr1, aUpdateBuffer);
	}

/***********************************************************************************/
//
//	MobileMMSInformation  Functional Unit.
// 		
/***********************************************************************************/


EXPORT_C RMobilePhone::TMmsNotificationV3::TMmsNotificationV3()
	: iSpace(EFalse), iNotification(EFalse), iMmsStatus(EMmsNotRetrieved), iImplementation(KMmsImplUnknwown)
/**
Default Constructor.
*/
	{
	iExtensionId=KETelExtMultimodeV3;
	}


EXPORT_C void RMobilePhone::NotifyMmsUpdate(TRequestStatus& aReqStatus, TDes8& aNotification) const
/**
Notifies the client of changes to the MMS notification file (EF_MMSN) in the UICC.
 
@param aReqStatus Returns the result code after the asynchronous call completes.
@param aNotification Will contain MMS notification change details. 
@capability ReadDeviceData
*/
	{	
	Get(EMobilePhoneNotifyMmsUpdate, aReqStatus, aNotification);	
	}
	
	
EXPORT_C void RMobilePhone::SetMmsUserConnParams(TRequestStatus& aReqStatus, TDes8& aConnectivity) const
/**
Sets the values available for MMS Connectivity parameters as determined by the user and stored in the (EF_MMSUCP) file in the UICC card.
 
@param aReqStatus Returns the result code after the asynchronous call completes.
@param aConnectivity Will contain MMS user connection parameters coded as a TLV object. 

@capability WriteDeviceData
*/
	{	
	Set(EMobilePhoneSetMmsUserConnParams, aReqStatus, aConnectivity);	
	}
	
EXPORT_C void RMobilePhone::SetMmsUserPreferences(TRequestStatus& aReqStatus, TDes8& aPreferences) const
/**
Sets the values available for MMS preference parameters as determined by the user and stored in the (EF_MMSUP) file in the UICC card.
 
@param aReqStatus Returns the result code after the asynchronous call completes.
@param  aPreferences Contains the MMS user preference parameters coded as a TLV object.

@capability WriteDeviceData
*/
	{
	Set(EMobilePhoneSetMmsUserPreferences, aReqStatus, aPreferences);
	}	
	
EXPORT_C void RMobilePhone::GetMmsConfig(TRequestStatus& aReqStatus, const TMmsConnParams aType, TDes8& aConnectivity) const
/**
Retrieves values available for MMS User preferences stored in EF_MMSUP file and MMS Connectivity parameters stored
in the EF_MMSICP and EF_MMSUCP files in the UICC card.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aType Contains the connectivity parametrs.
@param aConnectivity Contains the parameters that are returned coded as a TLV object.
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iGetMmsConfig = aType;	
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetMmsConfig, iMmPtrHolder->iGetMmsConfig);
	
	SetAndGet(EMobilePhoneGetMmsConfig, aReqStatus, ptr1, aConnectivity);
	}


EXPORT_C void RMobilePhone::NotifyMmsConfig(TRequestStatus& aReqStatus,const TMmsConnParams aType, TDes8& aConnectivity) const
/**
This API allows retrieval of the values available for MMS Connectivity parameters as determined by the issuer and stored in the EF_MMSICP and EF_MMSUCP files in the UICC card. 

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aType Contains the connectivity parametrs.
@param aConnectivity Contains the parameters that are returned coded as a TLV object.
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iMmPtrHolder->iNotifyMmsConfig = aType;	
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1NotifyMmsConfig, iMmPtrHolder->iNotifyMmsConfig);
	
	SetAndGet(EMobilePhoneNotifyMmsConfig, aReqStatus, ptr1, aConnectivity);
	}

/***********************************************************************************/
//
//	MobileMailBoxNumbers  functional unit.
// 		
/***********************************************************************************/

EXPORT_C RMobilePhone::TMobilePhoneVoicemailIdsV3::TMobilePhoneVoicemailIdsV3()
	: iVoice(0),iData(0), iFax(0), iOther(0)
/**
Default constructor.
*/
	{
	iExtensionId=KETelExtMultimodeV3;
	}

EXPORT_C RMobilePhone::TMobilePhoneVoicemailIdsV8::TMobilePhoneVoicemailIdsV8()
:TMobilePhoneVoicemailIdsV3(), iVideo(0)
/**
Default constructor.
*/
	{
	iExtensionId=KEtelExtMultimodeV8;
	}
	
EXPORT_C void RMobilePhone::GetMailboxNumbers(TRequestStatus& aReqStatus, TDes8& aMailBox) const
/**
Retrieves the Mailbox numbers identifier information from the EF_MBI file in the USIM.

Cancel this request using RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetMailboxNumbers)

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aMailBox Contains mailbox identifiers. This should be a packaged RMobilePhone::TMobilePhoneVoicemailIdsV3 or derived class.
@capability ReadUserData

@see TMobilePhoneVoicemailIdsV3Pckg
@see TMobilePhoneVoicemailIdsV3
*/
	{	
	Get(EMobilePhoneGetMailboxNumbers, aReqStatus, aMailBox);
	}
	
	
EXPORT_C void RMobilePhone::NotifyMailboxNumbersChange(TRequestStatus& aReqStatus, TDes8& aMailBox) const
/**
Notifies the client of changes to any of the Mailbox numbers identifiers.

Cancel this request using RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyMailboxNumbersChange)

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aMailBox Contains new mailbox numbers identifiers. This should be a packaged RMobilePhone::TMobilePhoneVoicemailIdsV3 or derived class.
@capability ReadUserData

@see TMobilePhoneVoicemailIdsV3Pckg
@see TMobilePhoneVoicemailIdsV3
*/
	{	
	Get(EMobilePhoneNotifyMailboxNumbersChange, aReqStatus, aMailBox);	 
	}

/***********************************************************************************/
//
// Mobile APN Control List Functional Unit
// 
/***********************************************************************************/

EXPORT_C RMobilePhone::TAPNEntryV3::TAPNEntryV3()
/**
Default constructor.
*/
	{
	iExtensionId=KETelExtMultimodeV3;
	}

EXPORT_C void RMobilePhone::EnumerateAPNEntries(TRequestStatus& aReqStatus, TUint32 &aIndex) const
/**
Enumerates the available APN names in the Access Control list EF_ACL file in the UICC.
If this file or service is not available, then KErrNotFound is returned.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aIndex  Returns the total number of available entries.
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1EnumerateAPNEntries, aIndex);

	Get(EMobilePhoneEnumerateAPNEntries, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::GetAPNname(TRequestStatus& aReqStatus, const TUint32 aIndex, TDes8& aAPN) const
/**
Retrieves the APN names stored in the Access Control list EF_ACL file in the UICC. 

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aIndex Index of APN entry to retrieve.
@param aAPN Packaged instance of TAPNEntryV3.
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iGetAPNnameIndex = aIndex;	
	TPtrC8& ptr1=iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1GetAPNnameIndex, iMmPtrHolder->iGetAPNnameIndex);
	
	SetAndGet(EMobilePhoneGetAPNname, aReqStatus, ptr1, aAPN);
	}

EXPORT_C void RMobilePhone::AppendAPNName(TRequestStatus& aReqStatus, const TDes8& aAPN) const
/**
Appends an entry to the end of the APN control list stored in the 
EF_ACL file on the USIM.  After calling this function, the client 
should re-enumerate the whole list.  The client should not assume 
that any entry found at a particular index before this function 
was called will still be at the same index after the call.

Editing an entry in the APN control list can be achieved first by 
deleting an entry from the list by calling DeleteAPNName() and 
then appending a new entry using this function.  

After this process, the client should re-enumerate the whole list.  
The client should not assume that any entry found at a particular 
index before this function was called will still be at the same 
index after the call.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneAppendAPNName)
to cancel a previously placed asynchronous AppendAPNName() request.

@param aReqStatus Request status. On completion contains KErrNone if successful. Otherwise, another error code depending on failure.\n

@param aAPN The APN name to append to the list.

@post
On completion aReqStatus contains KErrNone if successful. Otherwise, another error code depending on failure.\n

Error codes returned:

If extended error codes are supported:\n
	- KErrMMEtelAPNNameACLNotFound if the EF_ACL file cannot be found on the USIM.\n
	- KErrMMEtelAPNNameInvalidAPNName if the supplied APN name to append is invalid. E.g. it is too long.\n
	- KErrMMEtelAPNNameAPNListFull if the entry cannot be added to the list because the list is full or there is no space left for an additional entry.\n
	- KErrGsm0707SimPin2Required if PIN2, which is required to update EF_ACL has not been supplied or was incorrect.\n

If basic error codes are supported:\n
	- KErrNotFound if the EF_ACL file cannot be found on the USIM.\n
	- KErrBadName if the supplied APN name to append is invalid. E.g. it is too long.\n
	- KErrOverflow if the entry cannot be added to the list because the list is full or there is no space left for an additional entry.\n
	- KErrAccessDenied if PIN2, which is required to update EF_ACL has not been supplied or was incorrect.

@capability WriteDeviceData
*/
	{
	Set(EMobilePhoneAppendAPNName, aReqStatus, aAPN);
	}

EXPORT_C void RMobilePhone::DeleteAPNName(TRequestStatus& aReqStatus, TUint32 aIndex) const
/**
Deletes an entry from the APN control list stored in the EF_ACL file 
on the USIM.  After calling this function, the client should 
re-enumerate the list.  The client should not assume that any entry 
found at a particular index before the function was called will 
still be at the same index after the call. 

Editing an entry in the APN control list can be achieved first by 
deleting an entry from the list (by calling this function) and 
then appending a new entry using AppendAPNName().  

After this process, the client should re-enumerate the whole list.  
The client should not assume that any entry found at a particular 
index before this function was called will still be at the same 
index after the call.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneDeleteAPNName)
to cancel a previously placed asynchronous DeleteAPNName() request.

@param aReqStatus Request status.

@param aIndex Index of APN entry to delete.

@post
On completion aReqStatus contains KErrNone if successful. Otherwise, another error code depending on failure.\n

Error codes returned:

If extended error codes are supported.\n
	- KErrMMEtelAPNNameACLNotFound if the EF_ACL file cannot be found on the USIM.\n
	- KErrMMEtelAPNNameInvalidIndex if aIndex does not refer to a valid index in the APN control list.\n
	- KErrGsm0707SimPin2Required if PIN2, which is required to update EF_ACL has not been supplied or was incorrect.\n

If basic error codes are supported:\n
	- KErrNotFound if the EF_ACL file cannot be found on the USIM.\n
	- KErrArgument if aIndex does not refer to a valid index in the APN control list.\n
	- KErrAccessDenied if PIN2, which is required to update EF_ACL has not been supplied or was incorrect.\n

@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
   
	iMmPtrHolder->iDeleteAPNNameIndex = aIndex;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1DeleteAPNNameIndex, iMmPtrHolder->iDeleteAPNNameIndex);

	Set(EMobilePhoneDeleteAPNName, aReqStatus, ptr1);
	}
	
EXPORT_C void RMobilePhone::NotifyAPNListChanged(TRequestStatus& aReqStatus) const
/**
Allows a client to request notification of when the APN control list 
changes.

This function only tells the client that the list has changed but 
not what operation has been done on the list.  Therefore, the client 
should re-enumerate and refresh the list after this operation has 
completed.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyAPNListChanged)
to cancel a previously placed asynchronous NotifyAPNListChanged() request.

@param aReqStatus Request status. On completion contains KErrNone if
successful, otherwise one of the other system-wide error codes.

@capability ReadDeviceData
*/
	{
	Blank(EMobilePhoneNotifyAPNListChanged, aReqStatus);
	}	

EXPORT_C void RMobilePhone::SetAPNControlListServiceStatus(TRequestStatus& aReqStatus, TAPNControlListServiceStatus aAPNControlListServiceStatus) const
/**
Sets the status of the APN Control List Service.  
The APN Control List service is stored as service number 3 in the
Enabled Services Table (EF_EST) on the USIM.
See 3GPP 31102.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneSetAPNControlListServiceStatus)
to cancel a previously placed asynchronous SetAPNControlListServiceStatus() request.

@param aReqStatus Request status.
@param aAPNControlListServiceStatus The service status to set.

@post
On completion aReqStatus contains KErrNone if successful. Otherwise, another error code depending on failure.\n

Error codes returned:

If extended error codes are supported:\n
	- KErrMMEtelAPNEnabledServicesTableNotFound if the EF_EST file could not be found on the USIM.\n
	- KErrGsm0707SimPin2Required if PIN2, which is required to update EF_EST has not been supplied or was incorrect.\n

Basic error codes used in additional to extended error codes if supported by the TSY:\n
	- KErrArgument if an invalid service status was supplied (i.e. one that is not part of RMobilePhone::TAPNControlListServiceStatus).\n
	- KErrNotSupported if the service status supplied in aAPNControlListServiceStatus is not supported.\n

If basic error codes are supported:\n
	- KErrNotFound if the EF_EST file could not be found on the USIM.\n
	- KErrAccessDenied if PIN2, which is required to update EF_EST has not been supplied or was incorrect.\n

@capability NetworkServices WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetAPNControlListServiceStatus = aAPNControlListServiceStatus;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetAPNControlListServiceStatus, iMmPtrHolder->iSetAPNControlListServiceStatus);

	Set(EMobilePhoneSetAPNControlListServiceStatus, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::GetAPNControlListServiceStatus(TRequestStatus& aReqStatus, TAPNControlListServiceStatus& aAPNControlListServiceStatus) const
/**
Retrieves the APN Control List Service status which is stored as
service number 35 in the Enabled Services Table (EF_EST) on the USIM.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneGetAPNControlListServiceStatus)
to cancel a previously placed asynchronous GetAPNControlListServiceStatus() request.

@param aReqStatus Request status.

@param aAPNControlListServiceStatus The retrieved service status.

@post
On completion aReqStatus contains KErrNone if successful. Otherwise, another error code depending on failure.\n

Error codes returned:

If extended error codes are supported:\n
	- KErrMMEtelAPNEnabledServicesTableNotFound if the EF_EST file is not present on the USIM.\n
	- KErrGsm0707SimPin1Required if PIN1, which is required to read the EF_EST file has not been supplied or was incorrect.\n

If basic error codes are supported:\n
	- KErrNotFound if the EF_EST file is not present on the USIM.\n
	- KErrAccessDenied if PIN1, which is required to read the EF_EST file has not been supplied or was incorrect.\n

@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetAPNControlListServiceStatus, aAPNControlListServiceStatus);
	
	Get(EMobilePhoneGetAPNControlListServiceStatus, aReqStatus, ptr1);
	}

EXPORT_C void RMobilePhone::NotifyAPNControlListServiceStatusChange(TRequestStatus& aReqStatus, TAPNControlListServiceStatus& aAPNControlListServiceStatus) const
/**
Called by the client to request notification of when the APN Control 
List service status changes.  The APN Control List service status 
is stored as service number 35 in the Enabled Services Table (EF_EST) 
on the USIM.  The resulting new service status is returned via the 
aAPNControlListServiceStatus parameter.

Use RTelSubSessionBase::CancelAsyncRequest(EMobilePhoneNotifyAPNControlListServiceStatusChange)
to cancel a previously placed asynchronous NotifyAPNControlListServiceStatusChange() request.

@param aReqStatus Request status.
@param aAPNControlListServiceStatus The new service status.

@post
On completion aReqStatus contains KErrNone if successful. Otherwise, another error code depending on failure.\n

Error codes returned:

If extended error codes are supported.\n
	- KErrMMEtelAPNEnabledServicesTableNotFound if the EF_EST file cannot be found on the USIM.\n
	- KErrGsm0707SimPin1Required if PIN1, which is required to read the EF_EST file has not been supplied or was incorrect.\n

If basic error codes are supported.\n
	- KErrNotFound if the EF_EST file cannot be found on the USIM.\n
	- KErrAccessDenied if PIN1, which is required to read the EF_EST file has not been supplied or was incorrect.\n

@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyAPNControlListServiceStatusChange, aAPNControlListServiceStatus);

	Get(EMobilePhoneNotifyAPNControlListServiceStatusChange, aReqStatus, ptr1);
	}

/************************************************************************************/
//
// Mobile Air Time Duration Functional Unit
//
/************************************************************************************/

EXPORT_C TInt RMobilePhone::GetAirTimeDuration(TTimeIntervalSeconds& aTime) const
/**
Retrieves the current duration of the air time.

@param aTime Will contain the current air time duration.
@return KErrNone if successful.
@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPckg<TTimeIntervalSeconds> ptr1(aTime);
	return Get(EMobilePhoneGetAirTimeDuration,ptr1);
	}
	
EXPORT_C void RMobilePhone::NotifyAirTimeDurationChange(TRequestStatus& aReqStatus,TTimeIntervalSeconds& aTime) const
/**
Notifies the client when the last call has reached its disconnected state.  
It will return the current air time duration as of the last call being disconnected.

@param aReqStatus returns the result code after the asynchronous call completes
@param aTime Will contain the current air time duration.

@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1NotifyAirTimeDurationChange, aTime);	
	Get(EMobilePhoneNotifyAirTimeDurationChange,aReqStatus,ptr1);
	}
	
/************************************************************************************/
//
// Mobile All Calls Termination
//
/************************************************************************************/

EXPORT_C void RMobilePhone::TerminateAllCalls(TRequestStatus& aReqStatus) const
/**
Terminates all calls on all lines.
 
@param aReqStatus returns the result code after the asynchronous call completes.

@capability NetworkServices
*/
	{
	Blank(EMobilePhoneTerminateAllCalls,aReqStatus);
	}

/************************************************************************************/
//
// Mobile Active Calls Termination
//
/************************************************************************************/

EXPORT_C void RMobilePhone::TerminateActiveCalls(TRequestStatus& aReqStatus) const
/**
Terminates all active calls on all lines.
Active calls are defined as those in either active or held state.
This will not terminate calls in dialling or connecting state.
 
@param aReqStatus returns the result code after the asynchronous call completes.

@capability NetworkServices
*/
    {
    Blank(EMobilePhoneTerminateActiveCalls,aReqStatus);
    }

/***********************************************************************************/
//
// Auto Redial Functional Unit
//
/***********************************************************************************/
	
EXPORT_C void RMobilePhone::ClearBlacklist(TRequestStatus& aReqStatus) const
/**
Clears the blacklist in the mobile device containing the phone-numbers that the TSY should
not allow an autoredial operation.

@param aReqStatus Returns the result code after the asynchronous call completes.

@capability WriteDeviceData
*/
	{
	Blank(EMobilePhoneClearBlacklist,aReqStatus);
	}

/***********************************************************************************/
//
// Mobile Personalisation Functional Unit
//
/***********************************************************************************/
	
EXPORT_C TInt RMobilePhone::GetPersonalisationCaps(TUint32& aCaps) const
/**
Gets the current personalisation scheme capabilities of the UICC/ME.

@param aCaps Bit-wise sum of the personalisation scheme capabilities
consisting of flags from RMobilePhone::TMobilePhonePersonalisationCaps.
@return KErrNone

@capability None
*/
	{
	TPckg<TUint32> ptr1(aCaps);
	return Get(EMobilePhoneGetPersonalisationCaps, ptr1);
	}
	
EXPORT_C void RMobilePhone::GetPersonalisationStatus(TRequestStatus& aReqStatus, TUint32& aPersSchemes) const
/**
Retrieves the current active personalisation schemes of the phone.
The phone maybe using more than one scheme.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aPersSchemes Bit-wise sum of the active personalisation scheme on the phone.
@capability ReadDeviceData
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1GetPersonalisationStatus, aPersSchemes);	
	Get(EMobilePhoneGetPersonalisationStatus,aReqStatus,ptr1);
	}
	
EXPORT_C void RMobilePhone::SetPersonalisationStatus(TRequestStatus& aReqStatus, const TMobilePhonePersonalisation aPersSchemes, const TDes& aPersCode) const
/**
Sets the personalisation scheme state of the phone.  If the status of the selected scheme is "off", then this operation
will personalise the ME using the code entered via aPersCode. 
If the status is currently "on", then this operation will de-personalise the ME if the code matched to the one stored in the UICC.
The personalisation codes for each scheme must be different.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param aPersSchemes The scheme to be activated or deactivated.
Consists of an enum from RMobilePhone::TMobilePhonePersonalisationScheme.
@param aPersCode Personlisation code to activate or deactivate the scheme.
@capability WriteDeviceData
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iSetPersonalisationScheme = aPersSchemes;	
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetPersonalisationStatus, iMmPtrHolder->iSetPersonalisationScheme);	
	
	Set(EMobilePhoneSetPersonalisationStatus,aReqStatus,ptr1, aPersCode);
	}
	
EXPORT_C RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1::TEtel3rdPartyMobileNetworkInfoV1()
/** Default constructor */
:	iMode(ENetworkModeUnknown),
	iStatus(ENetworkStatusUnknown),
	iBandInfo(EBandUnknown),
	iAccess(ENetworkAccessUnknown)
	{
	iExtensionId=KETelExt3rdPartyV1;
	}

EXPORT_C void RMobilePhone::NotifyImsAuthorizationInfoChange(TRequestStatus& aReqStatus) const
/**
Notifies when IMS authorization data has changed on the ISIM or USIM. On completion of this event the new
authorisation data can be retrieved using CAsyncRetrieveAuthorizationInfo.
 
@see CAsyncRetrieveAuthorizationInfo
 
@param aReqStatus Returns the result code after the asynchronous call completes.
@capability None
*/
 	{
 	Blank(EMobilePhoneNotifyImsAuthorizationInfoChanged, aReqStatus);	
 	}
/***********************************************************************************/

/** 
Default constructor initialises base class data member that identifies version of the API supported.
*/
EXPORT_C RMobilePhone::TImsAuthenticateDataV5::TImsAuthenticateDataV5()
	{
	iExtensionId=KEtelExtMultimodeV5;
	}

/** 
Sends IPC to ETEL/TSY to execute AUTHENTICATE command.

@param aReqStatus Returns the result code after the asynchronous call completes.
@param authenticateDataPckgd Contains authentication data to be sent to ISIM/USIM.  
@capability ReadDeviceData
 */
EXPORT_C void RMobilePhone::ImsAuthenticate(TRequestStatus& aReqStatus, TDes8& aAuthenticateDataPckgd) const
	{
	Get(EMobilePhoneIMSAuthenticate, aReqStatus, aAuthenticateDataPckgd);
	}

/**
Class factory Constructor.

@return Pointer to class instance.
*/
EXPORT_C RMobilePhone::CImsAuthorizationInfoV5* RMobilePhone::CImsAuthorizationInfoV5::NewL()
	{
/** standard two phase constructor*/
	return new(ELeave) CImsAuthorizationInfoV5;
	}
	
/** 
Destructor frees data held in aray data member.
*/
EXPORT_C RMobilePhone::CImsAuthorizationInfoV5::~CImsAuthorizationInfoV5()
	{
	iIMPUArray.Reset();
	iIMPUArray.Close();
	}

/**
Serialize data to the buffer.
If aBuffer points to already allocated memory, client has to be aware that it won't have
access to this location anymore as method reallocates memory and updates aBuffer accordingly.
Therefore, value passed in should not be freed upon completion of the method neither put onto
the cleanup stack before method is invoked.

After completion of the API, it's clients responsibility to control life scope
of allocated memory passed back via aBuffer.

@param aBuffer Heap-based buffer which will contain the serialized data.
*/
EXPORT_C void RMobilePhone::CImsAuthorizationInfoV5::ExternalizeL(HBufC8*& aBuffer)
	{
	TUint uintLen=sizeof(TUint);
	TUint size= uintLen/* extension id*/ + uintLen /* IMPI length*/ + iIMPI.Length() + uintLen/* hdn length*/ + iHomeNetworkDomainName.Length();
	TUint authEnumDataSourceLen=sizeof(TAuthorizationDataSource);
	
	size+=authEnumDataSourceLen;
	TUint count=iIMPUArray.Count();
	size+=uintLen; /** number of IMPUArray elements*/
	for(TUint i=0;i<count;i++)
		size+=iIMPUArray[i].Length()+uintLen;

	if(aBuffer)
		{
	 	delete aBuffer;
	 	aBuffer=NULL;
	 	}
	aBuffer=HBufC8::NewL(size);
	aBuffer->Des().SetLength(size);


	TInt cursor(0);


	Mem::Copy((TUint8*)(aBuffer->Des().Ptr()+cursor),(const TUint8*)(&iExtensionId),uintLen);
	cursor+=uintLen;

	TUint len=iIMPI.Length();
	Mem::Copy((TUint8* )(aBuffer->Des().Ptr()+cursor),&len,uintLen);
	cursor+=uintLen;
	Mem::Copy((TUint8* )(aBuffer->Des().Ptr()+cursor),iIMPI.Ptr(),len);
	cursor+=len;

	len=iHomeNetworkDomainName.Length();
	Mem::Copy((TUint8* )(aBuffer->Des().Ptr()+cursor),&len,uintLen);
	cursor+=uintLen;
	Mem::Copy((TUint8* )(aBuffer->Des().Ptr()+cursor),iHomeNetworkDomainName.Ptr(),len);
	cursor+=len;

	
	Mem::Copy((TUint8* )(aBuffer->Des().Ptr()+cursor),&iAuthenticationDataSource,authEnumDataSourceLen);
	cursor+=authEnumDataSourceLen;

	Mem::Copy((TUint8* )(aBuffer->Des().Ptr()+cursor),(const TUint8* )(&count),uintLen);
	cursor+=uintLen;

	for(TUint i=0;i<count;i++)
		{
		len=iIMPUArray[i].Length();
		Mem::Copy((TUint8* )(aBuffer->Des().Ptr()+cursor),&len,uintLen);
		cursor+=uintLen;
		Mem::Copy((TUint8* )(aBuffer->Des().Ptr()+cursor),iIMPUArray[i].Ptr(),len);
		cursor+=len;
		}
	}


/** 
Default constructor.
*/
RMobilePhone::CImsAuthorizationInfoV5::CImsAuthorizationInfoV5():iExtensionId(KEtelExtMultimodeV5)
	{
	}

/** 
De-serialize data from provided buffer.

@param aBuffer Descriptor containing data to be de-serialized.
*/
EXPORT_C void RMobilePhone::CImsAuthorizationInfoV5::InternalizeL(const TDesC8& aBuffer)
	{
	iIMPUArray.Reset();
	TInt cursor=sizeof(TUint);

	if(aBuffer.Length() < cursor)
		User::Leave(KErrOverflow);	
	Mem::Copy(&iExtensionId,aBuffer.Ptr(),cursor);


	if(aBuffer.Length() < cursor + sizeof(TUint))
		User::Leave(KErrOverflow);
	
	TInt len(0);
	Mem::Copy(&len,aBuffer.Ptr()+cursor,sizeof(TUint));
	cursor+=sizeof(TUint);

	if(aBuffer.Length() < cursor + len)
		User::Leave(KErrOverflow);
		
	iIMPI.Copy(aBuffer.Mid(cursor,len));
	cursor+=len;
	
	if(aBuffer.Length() < cursor + sizeof(TUint))
		User::Leave(KErrOverflow);

	Mem::Copy(&len,aBuffer.Ptr()+cursor,sizeof(TUint));
	cursor+=sizeof(TUint);

	if(aBuffer.Length() < cursor + len)
		User::Leave(KErrOverflow);
	
	iHomeNetworkDomainName.Copy(aBuffer.Mid(cursor,len));
	cursor+=len;


	TUint authEnumDataSourceLen=sizeof(TAuthorizationDataSource);
	
	if(aBuffer.Length() < cursor + authEnumDataSourceLen)
		User::Leave(KErrOverflow);
	
	Mem::Copy(&iAuthenticationDataSource,aBuffer.Ptr()+cursor,authEnumDataSourceLen);
	cursor+=authEnumDataSourceLen;

	TUint count(0);
	if(aBuffer.Length() < cursor + sizeof(TUint))
		User::Leave(KErrOverflow);

	Mem::Copy(&count,aBuffer.Ptr()+cursor,sizeof(TUint));
	cursor+=sizeof(TUint);

	for(TUint i=0;i<count;i++)
		{
		if(aBuffer.Length() < cursor + sizeof(TUint))
			User::Leave(KErrOverflow);
		Mem::Copy(&len,aBuffer.Ptr()+cursor,sizeof(TUint));
		cursor+=sizeof(TUint);
		
		if(aBuffer.Length() < cursor + len)
			User::Leave(KErrOverflow);
		
		iIMPUArray.Append(aBuffer.Mid(cursor,len));
		cursor+=len;
		}
	}

/**
Extension id for the data.
*/
EXPORT_C TUint RMobilePhone::CImsAuthorizationInfoV5::ExtensionId() const
	{
	return iExtensionId;
	}

	
//
//
// (U)ICC Network Application Authenticate
//
//

/** 
Default TSmartCardAuthenticateDataV6 constructor, which initialises the
version of the API supported.

@see TMultimodeEtelV6Api

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TSmartCardAuthenticateDataV6::TSmartCardAuthenticateDataV6()
	{
	iExtensionId = KEtelExtMultimodeV6;
	}

/** 
Sends request to base band to execute "The GSM Algorithm" (see section
9.2.16 of 3GPP TS 51.011) or AUTHENTICATE command (see section 7 of
3GPP TS 31.102).
 
This request can be used in the below different contexts
1. This request will execute the AUTHENTICATE command on the default GSM
   SIM in 2G mode.
2. When using 3G smart card applications, this request will attempt to execute 
   the AUTHENTICATE command on the default (U)SIM application.
3. When using GBA(Generic Bootstrapping Architecture) authentication,in two different
   modes - Bootstrapping mode and NAF derivation mode
4. when using MBMS(Multimedia Broadcast Multicast Service) authentication, in four
   different modes - MSK update mode,MTK generation mode,MSK deletion mode and MUK 
   deletion mode.

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aAuthDataPckgd Packaged TSmartCardAuthenticateDataV6 or TGbaBootstrapAuthDataV8
					  or TGbaNafAuthDataV8 or TMbmsMskUpdateDataV8 or TMbmsMtkGenerateDataV8
					  or TMbmsMskDeleteDataV8 or TMbmsMukDeleteDataV8 object for
                      passing data to base band for authentication and
                      retrieving response data. In 3G context the client must
                      populate iAUTN and iRAND, all other members will
                      be populated by base band's AUTHENTICATE.
                      In 2G context only iRAND, iRES and iKc are
                      meaningful, thus, client should only populate
                      iRAND.In GBA Bootstrapping mode client must
                      populate iRAND and iAUTN and the output would be either
                      iRES or iAUTS.In GBA NAF derivation mode the client must
                      populate iNafId,iImpi and the output would be iKsExtNaf.
                      In MBMS MSK update mode the client should populate iInputMikey
                      and the output would be iOutputMikey.In MBMS MTK generation mode
                      the client should populate iMikey and the output would be 
                      iMtk and optionally iSaltKey.In MBMS MSK deletion mode the client should 
                      populate iKeyDomainId and iMskIdKeyGroup.In MBMS MUK deletion mode the 
                      client must populate iMukIdTlv.

@capability ReadDeviceData
@publishedPartner
@released
*/
EXPORT_C void RMobilePhone::SmartCardAuthenticate(TRequestStatus& aReqStatus, TDes8& aAuthDataPckgd) const
	{
	Get(EMobilePhoneSmartCardAuthenticate, aReqStatus, aAuthDataPckgd);
	}

/** 
Sends IPC to ETEL/TSY to execute "The GSM/GBA/MBMS Algorithm" AUTHENTICATE
command.

The caller can use this overload to specify an application ID on the
UICC.  This method can be used for calling AUTHENTICATE on any
application.

If calling this method for an ISIM application, the behaviour will be
identical to the RMobilePhone::ImsAuthenticate() request.

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aAID The application identifier that should be used for running
            the AUTHENTICATE algorithm.
@param aAuthDataPckgd Packaged TSmartCardAuthenticateDataV6 or 
					  TGbaBootstrapAuthDataV8 or TGbaNafAuthDataV8 or 
					  TMbmsMskUpdateDataV8 or TMbmsMtkGenerateDataV8
					  or TMbmsMskDeleteDataV8 or TMbmsMukDeleteDataV8 object
                      for passing data to base band for authentication and
                      retrieving response data.  The client must
                      populate the relevant input parameter, all other members will
                      be populated by base band's AUTHENTICATE.
@see RMobilePhone::ImsAuthenticate()

@capability ReadDeviceData
@publishedPartner
@released
*/
EXPORT_C void RMobilePhone::SmartCardAuthenticate(TRequestStatus& aReqStatus, const TAID& aAID, TDes8& aAuthDataPckgd) const
	{
	__ASSERT_ALWAYS(iPtrHolder != NULL, PanicClient(EEtelPanicNullHandle));

	iMmPtrHolder->iScAuthAID = aAID;
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobilePhonePtrHolder::ESlot1SmartCardAuthenticate, iMmPtrHolder->iScAuthAID);

	Get(EMobilePhoneSmartCardAuthenticate, aReqStatus, aAuthDataPckgd, ptr1);
	}


/*********************************************************************/
//
// Request data structure for Smart Card Application based EAP
// authenticate.
//
/*********************************************************************/

// Public Methods //

/**
Constructs a new object for passing EAP-Request packets to the base
band.

@return Pointer to new RMobileSmartCardEap::CEapAuthenticateRequestDataV6 object.

@publishedPartner
@released
*/
EXPORT_C RMobileSmartCardEap::CEapAuthenticateRequestDataV6* RMobileSmartCardEap::CEapAuthenticateRequestDataV6::NewL()
	{
	CEapAuthenticateRequestDataV6* self = new(ELeave) CEapAuthenticateRequestDataV6();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Externalise the data in this object.

@param aBuffer Ptr will be set to point at the a buffer where this
               object serialises its data, including EAP-Request
               packet.

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::CEapAuthenticateRequestDataV6::ExternalizeL(TPtr8& aBuffer)
	{
	aBuffer.Set(GetEapReqPacket());
	}

/**
Internalise the data to this object.

@param aBuffer Populates this object with the serialised data passed in
               this argument.

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::CEapAuthenticateRequestDataV6::InternalizeL(const TDesC8& aBuffer)
	{
	SetEapReqPacketL(aBuffer);
	}

/**
Allows client to identify the type of this object by its version
number.

@return The version identification of this data structure.
@see TMultimodeEtelV6Api

@publishedPartner
@released
*/
EXPORT_C TUint RMobileSmartCardEap::CEapAuthenticateRequestDataV6::VersionId() const
	{
	return iVersionId;
	}

/**
Gets the request packet stored in this object.

@return A Ptr, pointing specifically to the EAP-Request packet stored
        in this object.

@publishedPartner
@released
*/
EXPORT_C TPtr8 RMobileSmartCardEap::CEapAuthenticateRequestDataV6::GetEapReqPacket() const
	{
	return iEapReqPacket->Ptr(0);
	}

/**
Store the EAP-Request packet within this object.

@param aData Copy the EAP-Request packet in this argument descriptor to
             this data structures memory.

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::CEapAuthenticateRequestDataV6::SetEapReqPacketL(const TDesC8& aData)
	{
	iEapReqPacket->Reset();

	TInt dataSize = aData.Size();
	TInt availSize = iEapReqPacket->Capacity();

	if (dataSize > availSize)
		{
		// Leave if cannot resize
		iEapReqPacket->ResizeL(dataSize);
		}

	iEapReqPacket->Write(0, aData);
	}

/**
Default destructor for this object.

@publishedPartner
@released
*/
EXPORT_C RMobileSmartCardEap::CEapAuthenticateRequestDataV6::~CEapAuthenticateRequestDataV6()
	{
	delete iEapReqPacket;
	iEapReqPacket = NULL;
	}

// Private Methods //

/**
Default constructor; initialises the version of this object.
*/
RMobileSmartCardEap::CEapAuthenticateRequestDataV6::CEapAuthenticateRequestDataV6()
: iVersionId(KEtelExtMultimodeV6), iEapReqPacket(NULL)
	{
	}

/**
Second-phase constructor, which just initialises the flat buffer for
later EAP-Request packet storage.
*/
void RMobileSmartCardEap::CEapAuthenticateRequestDataV6::ConstructL()
	{
	iEapReqPacket = CBufFlat::NewL(50);
	}
/*****************************************************************************************************/
//
//    	USIM R6 GBA and MBMS Authentication
//
/******************************************************************************************************/
/** 
Default TBsfDataV8 constructor, which initialises the
version of the API supported.

@see TMultimodeEtelV8Api

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TBsfDataV8::TBsfDataV8()
	{
	iExtensionId = KEtelExtMultimodeV8;
	}
	
/** 
Default TAuthInfoV8 constructor, which initialises the
version of the API supported.

@see TMultimodeEtelV8Api

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TAuthInfoV8::TAuthInfoV8()
	{
	iExtensionId = KEtelExtMultimodeV8;
	}

/** 
Default TMbmsUserKeyV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TMbmsUserKeyV8::TMbmsUserKeyV8()
	{
	iExtensionId = KETelMobileMbmsUserKeyV8;
	}
	
/** 
Default TGbaBootstrapAuthDataV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TGbaBootstrapAuthDataV8::TGbaBootstrapAuthDataV8()
	{
	iExtensionId = KETelMobileGbaBootstrapAuthDataV8;
	}

/** 
Default TGbaNafAuthDataV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TGbaNafAuthDataV8::TGbaNafAuthDataV8()
	{
	iExtensionId = KETelMobileGbaNafAuthDataV8;
	}

/** 
Default TMbmsMskUpdateDataV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TMbmsMskUpdateDataV8::TMbmsMskUpdateDataV8()
	{
	iExtensionId = KETelMobileMbmsMskUpdateDataV8;
	}

/** 
Default TMbmsMtkGenerateDataV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TMbmsMtkGenerateDataV8::TMbmsMtkGenerateDataV8()
	{
	iExtensionId = KETelMobileMbmsMtkGenerateDataV8;
	}

/** 
Default TMbmsMskDeleteDataV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TMbmsMskDeleteDataV8::TMbmsMskDeleteDataV8()
	{
	iExtensionId = KETelMobileMbmsMskDeleteDataV8;
	}

/** 
Default TMbmsMukDeleteDataV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TMbmsMukDeleteDataV8::TMbmsMukDeleteDataV8()
	{
	iExtensionId = KETelMobileMbmsMukDeleteDataV8;
	}

/** 
Default TGbaBootstrapParamsV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/	
EXPORT_C RMobilePhone::TGbaBootstrapParamsV8::TGbaBootstrapParamsV8()
	{
	iExtensionId = KETelMobileGbaBootstrapParamsV8;
	}

/** 
Retrieves authentication related parameters for different types of authentication EF_GBANL
and EF_MUK(Generic Bootstrapping parameter and MBMS User Key parameters).
The client needs to supply the type of the authentication data and the AID of the UICC 
application in the aAuthInfo parameter, which will contain the packaged version of TAuthInfoV8. 
The authentication parameters are returned in the aParams parameter. Depending on the 
type of authentication, the client will supply the packaged instance of one of the data 
structures TGbaBootstrapParamsV8 or TMbmsUserKeyV8(See section 4.2.79 and 4.2.81 of 3GPP 
TS 31.102 v6.18). 

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aAuthInfo  Packaged TAuthInfoV8 object to supply the type of the authentication data
				  (Generic Bootstrapping parameter and MBMS User Key parameters) and the AID
				  of the UICC application. 
				 
@param aParams	  Packaged TGbaBootstrapParamsV8 or TMbmsUserKeyV8 object will be returned depending 
				  on the type of authentication data selected.
                     
@capability ReadDeviceData

@publishedPartner
@released
*/

EXPORT_C void RMobilePhone::GetAuthenticationParams(TRequestStatus& aReqstatus,const TDesC8& aAuthInfo,TDes8& aParams) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	SetAndGet(EMobilePhoneGetAuthenticationParams,aReqstatus,aAuthInfo,aParams);
	}

/** 
Stores the Bootstrapping Transaction Identifier (B-TID) and the Key Life Time associated with 
the bootstrapping keys produced by the above GBA Bootstrapping procedure. The B-TID and the 
key lifetime are supplied by the network (BSF) and they are stored in the 
EF_GBABP(see section 4.2.79 of 3GPP TS 31.102 v6.18) file of the UICC application.
The B-TID and the key lifetime parameters are supplied within the aParams parameter, which is 
a packaged version of the TBsfDataV8 class. The UICC application which will store these
parameters is identified by the AID supplied within the aAID parameter.  

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aAID       The application identifier that should be used for running
                  the AUTHENTICATE algorithm.
@param aParams    Packaged TBsfDataV8 object to update the GBA parameters.

@capability WriteDeviceData

@publishedPartner
@released
*/	

EXPORT_C void RMobilePhone::SetGbaBootstrapParams(TRequestStatus& aReqStatus,const TAID& aAID,const TDesC8& aParams) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobilePhonePtrHolder::ESlot1SetGbaBootstrapParams, aAID);
	Set(EMobilePhoneSetGbaBootstrapParams,aReqStatus,aParams,ptr1);
	}

/** 
This function member posts an asynchronous notification request.
It notifies when the authentication related data(EF_GBABP,EF_GBANL,EF_MSK,EF_MUK) changes 
as a result of an UICC authentication request. 
The client supplies a packaged version of TAuthInfoV8 and gets notified when an UICC 
authentication request, affecting any of the data specified by TAuthData(GBA,MBMS), 
successfully completes. Upon completion, the client will be informed of the authentication 
data having just been modified as a result of the authentication.The client can then 
retrieve the relevant authentication parameters using the data retrieval methods for 
the relevant authentication. 

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aAuthInfo  Packaged TAuthInfoV8 object to supply the type of the authentication data
				  and the AID of the UICC application. 

@capability ReadDeviceData

@publishedPartner
@released
*/
	
EXPORT_C void RMobilePhone::NotifyAuthenticateDataChange(TRequestStatus& aReqstatus,TDes8& aAuthInfo) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	Get(EMobilePhoneNotifyAuthenticateDataChange,aReqstatus,aAuthInfo);
	}

/** 
Default TGbaNafEntryV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/	
EXPORT_C RMobilePhone::TGbaNafEntryV8::TGbaNafEntryV8()
	{
	iExtensionId= KEtelExtMultimodeV8;
	}

void RMobilePhone::TGbaNafEntryV8::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the TGbaNafEntryV8 entry from a stream
 *
 * @param aStream The read stream containing the TGbaNafEntryV8 fixed size entry
 */
	{
	TMultimodeType::InternalizeL(aStream);
	aStream >> iNafId;
	aStream >> iBtid;
	}

void RMobilePhone::TGbaNafEntryV8::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the TGbaNafEntryV8 entry into a stream
 *
 * @param aStream The write stream that will contain the TGbaNafEntryV8 entry
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream << iNafId;
	aStream << iBtid;
	}
	
/** 
Default TMskEntryV8 constructor, which initialises the
version of the API supported.

@see TMobilePhoneAuthDataExtId

@publishedPartner
@released
*/

EXPORT_C RMobilePhone::TMskEntryV8::TMskEntryV8()
	{
	iExtensionId=KEtelExtMultimodeV8;
	}

void RMobilePhone::TMskEntryV8::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the TMskEntryV8 entry from a stream
 *
 * @param aStream The read stream containing the TMskEntryV8 fixed size entry
 */
	{
	TMultimodeType::InternalizeL(aStream);
	aStream >> iKeyDomainId;
	aStream >> iMskId;
	aStream >> iTimeStampCounter;
	}

void RMobilePhone::TMskEntryV8::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the TMskEntryV8 entry into a stream
 *
 * @param aStream The write stream that will contain the TMskEntryV8 entry
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream << iKeyDomainId;
	aStream << iMskId;
	aStream << iTimeStampCounter;
	}

/*****************************************************************************************************/
//
//    	USIM R6 Wlan Authentication
//
/******************************************************************************************************/
/** 
Default TUsimWlanDataV8 constructor, which initialises the
version of the API supported.

@see TMultimodeEtelV8Api

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TUsimWlanDataV8::TUsimWlanDataV8()
	{
	iExtensionId = KEtelExtMultimodeV8;
	}
	
/**
Stores the WLAN parameters stored on the USIM. The client supplies a 
packaged instance of TUsimWlanDataV8 in the aData parameter. The client will use the iFieldsUsed 
data member of TUsimWlanDataV8 to indicate which fields of the supplied data structure contain 
valid data. 
see section 4.4.5.1 and 4.4.5.6 of 3GPP TS 31.102 v6.18. 

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the Wlan identifiers
(Pseudonym/Reauthentication id) are not present on the USIM.
@param aData An instance of TUsimWlanDataV8Pckg containing the Wlan identifiers to set.

@see TUsimWlanDataV8

@capability WriteDeviceData

@publishedPartner
@released
*/	
EXPORT_C void RMobilePhone::SetWlanData(TRequestStatus& aReqStatus, const TDesC8& aData) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	Set(EMobilePhoneSetWlanData,aReqStatus,aData);
	}
	
/**
Retrieve the WLAN parameters stored on the USIM. 
The requested data is returned within the aData parameter, which will contain the packaged
instance of TUsimWlanDataV8. The client can use the iFieldsUsed data member of TUsimWlanDataV8
to determine which fields of the returned data structure contain valid data. 
see section 4.4.5.1 and 4.4.5.6 of 3GPP TS 31.102 v6.18. 

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the Wlan identifiers
(Pseudonym/Reauthentication id) are not present on the USIM.
@param aData An instance of TUsimWlanDataV8Pckg containing the Wlan identifiers to set.

@see TUsimWlanDataV8

@capability ReadDeviceData

@publishedPartner
@released
*/	
EXPORT_C void RMobilePhone::GetWlanData(TRequestStatus& aReqStatus, TDes8& aData) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	Get(EMobilePhoneGetWlanData,aReqStatus,aData);
	}
	
/**
This function member posts an asynchronous notification request.
Notifies of any changes to the WLAN parameters stored on the USIM. 
The new WLAN data is returned within the aData parameter, which will contain the packaged instance
of TUsimWlanDataV8. The client can use the iFieldsUsed data member of TUsimWlanDataV8 to determine 
which fields of the returned data structure contain valid data
see section 4.4.5.1 and 4.4.5.6 of 3GPP TS 31.102 v6.18. 

@param aReqStatus On return, KErrNone if successful, KErrNotFound if the Wlan identifiers
(Pseudonym/Reauthentication id) are not present on the USIM.
@param aData An instance of TUsimWlanDataV8Pckg containing the Wlan identifiers to set.

@see TUsimWlanDataV8

@capability ReadDeviceData

@publishedPartner
@released
*/	
EXPORT_C void RMobilePhone::NotifyWlanDataChange(TRequestStatus& aReqStatus, TDes8& aData) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	Get(EMobilePhoneNotifyWlanDataChange,aReqStatus,aData);
	}
	
/** 
Default TWlanSIDV8 constructor, which initialises the
version of the API supported.

@see TMultimodeEtelV8Api

@publishedPartner
@released
*/
EXPORT_C RMobilePhone::TWlanSIDV8::TWlanSIDV8()
	{
	iExtensionId = KEtelExtMultimodeV8;
	}
	
void RMobilePhone::TWlanSIDV8::InternalizeL(RReadStream& aStream)
/**
 * This method internalizes the TWlanSIDV8 entry from a stream
 *
 * @param aStream The read stream containing the TWlanSIDV8 fixed size entry
 */
	{
	TMultimodeType::InternalizeL(aStream);
	aStream >> iWSID;
	iUserDefined = static_cast<TBool>(aStream.ReadInt32L());
	}

void RMobilePhone::TWlanSIDV8::ExternalizeL(RWriteStream& aStream) const
/**
 * This method externalizes the TWlanSIDV8 entry into a stream
 *
 * @param aStream The write stream that will contain the TWlanSIDV8 entry
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream << iWSID;
	aStream.WriteInt32L(iUserDefined); 
	}
	
/**
Stores a new version of the entire list of Wlan SIDs entries. The list, which is supplied by the client in 
an instance of CMobilePhoneStoredWlanSIDList, should not contain more entries than can be stored phone-side.

The list can be User controlled WLAN Specific IDentifier List (EF_UWSIDL) or Operator controlled 
WLAN Specific IDentifier List(EF_OWSIDL).

see section 4.4.5.4 and 4.4.5.5 of 3GPP TS 31.102 v6.18

@param aReqStatus returns the result code after the asynchronous call completes.
@param aList List of Wlan SIDs to be stored phone-side.

@see TWlanSIDV8

@capability WriteDeviceData

@publishedPartner
@released
*/		
EXPORT_C void RMobilePhone::StorePreferredWlanSIDListL(TRequestStatus& aReqStatus, CMobilePhoneStoredWlanSIDList* aList) const

	{
	__ASSERT_ALWAYS(aList!=NULL,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	delete iMmPtrHolder->iStoreWlanSIDBuf;
	iMmPtrHolder->iStoreWlanSIDBuf = NULL;

	iMmPtrHolder->iStoreWlanSIDBuf=aList->StoreLC();
	CleanupStack::Pop();

	(iMmPtrHolder->iStoreWlanSIDPtr).Set((iMmPtrHolder->iStoreWlanSIDBuf)->Ptr(0));

	Set(EMobilePhoneStorePreferredWlanSIDList,aReqStatus,iMmPtrHolder->iStoreWlanSIDPtr);	
	}
	
/**	
This function member posts an asynchronous notification request.
Notifies the client that changes have been made to the stored Wlan SIDs list. 
When it completes, it does not return the new version of the list so the client will have 
to retrieve the list again if it is interested in the new list.

@param aReqStatus returns the result code after the asynchronous call completes.

@capability None
*/
EXPORT_C void RMobilePhone::NotifyPreferredWlanSIDListChange(TRequestStatus& aReqStatus) const
	{
	Blank(EMobilePhoneNotifyPreferredWlanSIDListChange, aReqStatus);
	}	
