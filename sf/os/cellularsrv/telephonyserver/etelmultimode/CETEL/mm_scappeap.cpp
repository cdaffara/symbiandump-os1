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
// This file contains the definition for the ETelMM sub-session
// RMobileSmartCardEap, which allows access to EAP supporting UICC
// applications and all their functionality.
// (See specification: ETSI TS 102 310 v6.2.0)
// 
//

/**
 @file
*/

// From core API
#include <etelext.h>

// Multimode header files
#include <etelmm.h>
#include "mm_hold.h"
#include "mmretrieve.h"

//
//
//  RMobileSmartCardEap
//
//

/**
Default empty constructor, and is present only to support virtual
function table export.

@publishedPartner
@released
*/
EXPORT_C RMobileSmartCardEap::RMobileSmartCardEap()
	: iMmPtrHolder(NULL), iOwnsEapMethodLock(EFalse)
	{
	}

/**
This function opens a RMobileSmartCardEap sub-session from RMobilePhone
that will refer to the application referenced by aAID.  It will be
assumed that the application exists and contains a DF_EAP for the
aEapType specified.  The client must call
RMobileSmartCardEap::InitialiseEapMethod() to ensure correct
functionality of this sub-session.

@param aPhone The RMobilePhone sub-session relative to which this
              sub-session will open.
@param aAId The UICC Application ID, which should be of one that has
            EAP support.
@param aEapType The EAP method type that this sub-session will use
                under the aAID application.

@return KErrNone if successful, otherwise a system-wide error code.
@see RMobileSmartCardEap::InitialiseEapMethod()

@capability None

@publishedPartner
@released
*/
EXPORT_C TInt RMobileSmartCardEap::Open(RMobilePhone& aPhone, const RMobilePhone::TAID& aAID, const TEapType& aEapType)
	{
	RSessionBase* session = &aPhone.SessionHandle();
	__ASSERT_ALWAYS(session != NULL, PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle = aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle != NULL, PanicClient(EEtelPanicNullHandle));

	TRAPD(ret, ConstructL());
	if (ret != KErrNone)
		{
		Destruct();
		return ret;
		}

	// Appending the application ID and Eap Type to the name of the
	// subsession; plus two one-byte delimeters indicating lengths.
	// See var appIdbuf for why KAIDSize is multiplied by 2.
	TBufC<SCEAP_SSN_LENGTH + RMobilePhone::KAIDSize*2 +
	      KEapTypeSize + 2> nameBuf(KETelSmartCardEapSession); // 2 for delimeters
	TPtr name(nameBuf.Des());

	// the length of the AID as a Sept ASCII character
	TChar lengthAIDChar = SeptChar(aAID.Length());

	// the value of the AID
	// converted to a 16-bit string representation.  Multiply by 2,
	// since each AID byte is represented as two sem-octects.
	TBufC<2*RMobilePhone::KAIDSize> appIdbuf;
	TPtr appIdPtr(appIdbuf.Des());
	ConvertBinToText(aAID, appIdPtr);

	// the length of the EapType
	TInt lengthEapType = aEapType.Length();
	TChar charEapType = SeptChar(lengthEapType);

	// the value of the EapType (converted to 16-bit)
	TBufC<KEapTypeSize> eapTypeBuf;
	TPtr eapTypePtr(eapTypeBuf.Des());
	eapTypePtr.Copy(aEapType);

	// appending...
	name.Append(lengthAIDChar);
	name.Append(appIdPtr);
	name.Append(charEapType);
	name.Append(eapTypePtr);

	TIpcArgs args(&name, TIpcArgs::ENothing, subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session, EEtelOpenFromSubSession, args);

	if (ret != KErrNone)
		{
		Destruct();
		}

	return ret;
	}

/**
This function member closes a RMobileSmartCardEap sub-session.  The
Close() request also attempts to release this instance's lock on the
<AID,EAPType> (DF_EAP).

@panic Panics the client with ETel Panic EEtelPanicHandleNotClosed, if
       this instance owns the lock on the DF_EAP but could not release
       some resource to allow other instances to gain access.
       (However, it should be noted that the TSY can take control of
       DF_EAP access in the event that a client dies in such a manner.)
@see RMobileSmartCardEap::ReleaseEapMethod()

@capability None

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::Close()
	{
	if (iOwnsEapMethodLock)
		{
		TInt err = ReleaseEapMethod();
		if (err != KErrNone)
			{
			PanicClient(EEtelPanicHandleNotClosed);
			}
		}

	CloseSubSession(EEtelClose);
	Destruct();
	}

/**
Initialises access to the DF_EAP.  This will ensure the aAID and
aEapType given in the RMobileSmartCardEap::Open() exist and are
accessible.  If for any reason the sub-session is inaccessible, the
client can request a notification for state changes using
RMobileSmartCardEap::NotifyEapMethodAccessStatusChange().

@param aReqStatus Returns the result code after the asynchronous call
                  completes.  Successful completion is only achieved
                  when the client is the first to request
                  initialisation on this sub-session.
                  KErrInUse will be returned if another
                  RMobileSmartCardEap instance successfully achieved
                  initialisation first.
                  Any other error code returned as request completion,
                  suggests another problem in the system and the client
                  must call RMobileSmartCardEap::Close() or
                  RMobileSmartCardEap::ReleaseEapMethod() at some
                  point, to allow other clients to use this same
                  <AID,EapType> sub-session.
@see RMobileSmartCardEap::Open()
@see RMobileSmartCardEap::NotifyEapMethodAccessStatusChange()

@capability ReadDeviceData
@capability NetworkControl

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::InitialiseEapMethod(TRequestStatus& aReqStatus)
	{
	__ASSERT_ALWAYS(iMmPtrHolder != NULL, PanicClient(EEtelPanicNullHandle));

	if (!iOwnsEapMethodLock)
		{
		TInt ret = KErrNone;

		TInt semHandle = SendReceive(EEtelGlobalKernelObjectHandle);

		if(semHandle > 0)
			{
			ret = iSemaphore.SetReturnedHandle(semHandle); // although this will take an error as handle, best to specify our own explicit KErrBadHandle in the following else.
			}
		else
			{
			ret = KErrBadHandle;
			}

		if (ret != KErrNone)
			{
			TRequestStatus* status1 = &aReqStatus;
			User::RequestComplete(status1, ret);
			return;
			}

		ret = iSemaphore.Wait(10);
		if (ret == KErrTimedOut)
			{
			TRequestStatus* status2 = &aReqStatus;
			User::RequestComplete(status2, KErrInUse);
			return;
			}
		else if (ret != KErrNone)
			{
			TRequestStatus* status3 = &aReqStatus;
			User::RequestComplete(status3, ret);
			return;
			}

		iOwnsEapMethodLock = ETrue;
		}

	RThread ownerThread;
	TThreadId ownerThreadId = ownerThread.Id();

	iMmPtrHolder->iOwnerThreadId = ownerThreadId;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobileSmartCardEapPtrHolder::ESlot1InitialiseEapMethod, iMmPtrHolder->iOwnerThreadId);

	Set(EMobileSmartCardEapInitialiseEapMethod, aReqStatus, ptr1);
	}

/**
Delayed construction of heap stored data members.

@leave KErrNoMemory Heap memory allocation failure for
                    CMobileSmartCardEapPtrHolder object.

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::ConstructL()
	{
	__ASSERT_ALWAYS(iMmPtrHolder == NULL, PanicClient(EEtelPanicHandleNotClosed));
	iMmPtrHolder = CMobileSmartCardEapPtrHolder::NewL(CMobileSmartCardEapPtrHolder::EMaxNumberSmartCardEapPtrSlots,
	                                                  CMobileSmartCardEapPtrHolder::EMaxNumberSmartCardEapPtrCSlots);
	}

/**
Called internally when RMobileSmartCardEap instance is no longer
required, to ensure clean up of data members from memory.

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::Destruct()
	{
	delete iMmPtrHolder;
	iMmPtrHolder = NULL;
	ResetSessionHandle();
	}

/**
Default constructor, initialising version number of this data
structure.

@see TMultimodeEtelV6Api

@publishedPartner
@released
*/
EXPORT_C RMobileSmartCardEap::TEapUserIdentityV6::TEapUserIdentityV6()
	{
	iExtensionId = KEtelExtMultimodeV6;
	}

/**
Default constructor, initialising version number of this data
structure.

@see TMultimodeEtelV6Api

@publishedPartner
@released
*/
EXPORT_C RMobileSmartCardEap::TEapKeyV6::TEapKeyV6()
	{
	iExtensionId = KEtelExtMultimodeV6;
	}

/**
This method allows the client to retrieve the user identity data to be
used for an EAP based authentication.  The client will specify which
identity type they want to read by setting the aRequestIdType parameter
to the appropriate enumeration value.  The user Id data is returned as
a packaged instance of TEapUserIdentityV6 within the aUserId.

An example base band functionality that would be used to service this
request is the +CEPR (see section 8.48 of 3GPP TS 27.007 v6.8.0) AT-
command, which returns "identity" and "pseudonym" as two of its defined
values.

EF_PUId and EF_Ps hold these identities (specified in sections 7.3 and
7.4, respectively, of ETSI TS 102.310 v6.2.0).

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aRequestIdType Used to request the specific identity the client
                      wishes to retrieve.
@param aUserId On completion, will be populated with the user identity
               requested by the client.

@capability ReadDeviceData

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::GetUserIdentity(TRequestStatus& aReqStatus, const TEapUserIdType aRequestedIdType, TDes8& aUserId)
	{
	__ASSERT_ALWAYS(iMmPtrHolder != NULL, PanicClient(EEtelPanicNullHandle));

	if (iSemaphore.Handle() == 0)
		{
		TRequestStatus* status1 = &aReqStatus;
		User::RequestComplete(status1, KErrBadHandle);
		return;
		}

	if (!iOwnsEapMethodLock)
		{
		TRequestStatus* status2 = &aReqStatus;
		User::RequestComplete(status2, KErrInUse);
		return;
		}

	iMmPtrHolder->iEapUserIdType = aRequestedIdType;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobileSmartCardEapPtrHolder::ESlot1GetUserId, iMmPtrHolder->iEapUserIdType);

	SetAndGet(EMobileSmartCardEapGetUserIdentity, aReqStatus, ptr1, aUserId);
	}

/**
The authentication status is obtained from the EAP supporting UICC
application's EF_EAPSTATUS.  It specifies the current state of
authentication in the DF_EAP.

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aAuthStatus On request completion, will store the current
                   authentication status of the DF_EAP.

@capability ReadDeviceData

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::GetAuthenticationStatus(TRequestStatus& aReqStatus, TEapAuthStatus& aAuthStatus)
	{
	__ASSERT_ALWAYS(iMmPtrHolder != NULL, PanicClient(EEtelPanicNullHandle));

	if (iSemaphore.Handle() == 0)
		{
		TRequestStatus* status1 = &aReqStatus;
		User::RequestComplete(status1, KErrBadHandle);
		return;
		}

	if (!iOwnsEapMethodLock)
		{
		TRequestStatus* status2 = &aReqStatus;
		User::RequestComplete(status2, KErrInUse);
		return;
		}

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileSmartCardEapPtrHolder::ESlot1GetAuthStatus, aAuthStatus);

	Get(EMobileSmartCardEapGetAuthenticationStatus, aReqStatus, ptr1);
	}

/**
Retrieves the generated key stored in EF_EAPKEYS of the DF_EAP (see
section 7.1 of ETSI TS 102.310 v6.2.0).

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aRequestedKey Used to specify which of the keys the client is
                     requesting.
@param aKey Populated with the requested key on request completion.

@capability ReadDeviceData

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::GetEapKey(TRequestStatus& aReqStatus, const TEapKeyTag aRequestedKey, TDes8& aKey)
	{
	__ASSERT_ALWAYS(iMmPtrHolder != NULL, PanicClient(EEtelPanicNullHandle));

	if (iSemaphore.Handle() == 0)
		{
		TRequestStatus* status1 = &aReqStatus;
		User::RequestComplete(status1, KErrBadHandle);
		return;
		}

	if (!iOwnsEapMethodLock)
		{
		TRequestStatus* status2 = &aReqStatus;
		User::RequestComplete(status2, KErrInUse);
		return;
		}

	iMmPtrHolder->iReqEapKeyTag = aRequestedKey;
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobileSmartCardEapPtrHolder::ESlot1GetEapKey, iMmPtrHolder->iReqEapKeyTag);

	SetAndGet(EMobileSmartCardEapGetEapKey, aReqStatus, ptr1, aKey);
	}

/**
Relinquishes ownership of the DF_EAP, to allow other clients to use it.
Although the request completes relatively quickly, it will set the
server's process running to attempt a deactivate on the sub-session's
corresponding application.  The deactivate will allow future clients to
initialise the application, which should reset all its DF_EAP states.

The initial state change of the sub-session will be
EEapMethodUnableToInitialise, as the request completes without waiting
for the application's deactivation.  After the sub-session is able to
deactivate the app, the state will change to EEapMethodAvailable.

The server will make a decision on deactivating the application based
on whether there are other sub-session open to the same application
(but different EAP types/ DF_EAP).  Only when the application is no
longer in use, that the server will deactivate it.

Notifications can be posted using
RMobileSmartCardEap::NotifyEapMethodAccessStatusChange() to observe
such state changes.

@return If an error is returned, this object will maintain lock on the
        UICC application's DF_EAP.
@see RMobileSmartCardEap::NotifyEapMethodAccessStatusChange()

@capability ReadDeviceData

@publishedPartner
@released
*/
EXPORT_C TInt RMobileSmartCardEap::ReleaseEapMethod()
	{
	if (iSemaphore.Handle() == 0)
		{
		return KErrBadHandle;
		}

	if (!iOwnsEapMethodLock)
		{
		return KErrInUse;
		}

	TInt ret = Blank(EMobileSmartCardEapReleaseEapMethod);

	if (ret == KErrNone)
		{
		iSemaphore.Signal();
		iOwnsEapMethodLock = EFalse;
		}

	return ret;
	}

/**
Synchronous request to get the current status of the DF_EAP.  This
state value is held by the platform to ensure exclusive access to a
DF_EAP; it is NOT a state of any Smart Card Application file.

@param aEapMethodStatus Returns the sub-session's current value of
                        RMobileSmartCardEap::TEapMethodAccessStatus.

@capability ReadDeviceData

@publishedPartner
@released
*/
EXPORT_C TInt RMobileSmartCardEap::GetEapMethodAccessStatus(TEapMethodAccessStatus& aEapMethodStatus)
	{
	TPckg<TEapMethodAccessStatus> ptr1(aEapMethodStatus);
	return Get(EMobileSmartCardEapGetEapMethodAccessStatus, ptr1);
	}

/**
Notifies the client when the EAP method's (DF_EAP's) access status
changes.

The status begins as EEapMethodAvailable when the DF_EAP is first used;
before RMobileSmartCardEap::InitialiseEapMethod() is called by the
client.  EEapMethodInUseApplicationActive state is given after the
first client initialises... various cases cause transformations to
states EEapMethodUnableToInitialise or
EEapMethodInUseApplicationInactive.

@param aReqStatus Returns the result code after the asynchronous call
                  completes.
@param aEapMethodStatus Returns the 
                        RMobileSmartCardEap::TEapMethodAccessStatus
                        value when the status changes on this sub-
                        session.
@see RMobileSmartCardEap::InitialiseEapMethod()
@see RMobileSmartCardEap::GetEapMethodAccessStatus()
@see RMobileSmartCardEap::ReleaseEapMethod()

@capability ReadDeviceData

@publishedPartner
@released
*/
EXPORT_C void RMobileSmartCardEap::NotifyEapMethodAccessStatusChange(TRequestStatus& aReqStatus, TEapMethodAccessStatus& aEapMethodStatus)
	{
	__ASSERT_ALWAYS(iMmPtrHolder != NULL, PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileSmartCardEapPtrHolder::ESlot1NotifyEapMethodAccess, aEapMethodStatus);

	Get(EMobileSmartCardEapNotifyEapMethodAccessStatusChange, aReqStatus, ptr1);
	}

/**
Returns whether this RMobileSmartCardEap instance has ownership of its
corresponding DF_EAP.

@return ETrue only if this instance of RMobileSmartCardEap was the
        first to call RMobileSmartCardEap::InitialiseEapMethod()
        successfully.
@see RMobileSmartCardEap::InitialiseEapMethod()
@see RMobileSmartCardEap::Open()

@capability None

@publishedPartner
@released
*/
EXPORT_C TBool RMobileSmartCardEap::IsEapMethodOwner() const
	{
	return iOwnsEapMethodLock;
	}


// helpers //

/**
Returns the character that corresponds to the value of its parameter
(base 17!).

@param aDigit an integer between 0 and 16.
@return A TChar representing one of '0' to '9' or 'A' to 'G'.

@internalComponent
@released
*/
TChar RMobileSmartCardEap::SeptChar(TInt aDigit)
	{
	TChar ret(0);
	if ((aDigit < 0) || (aDigit > 16))
		{
		PanicClient(EEtelPanicHandleNotOpen);
		}
	else
		{
		if (aDigit < 10)
			{
			ret = aDigit + (TUint)'0';
			}
		else
			{
			ret = aDigit - 10 + (TUint)'A';
			}
		}
	return ret;
	}

/**
This function is used by RMobileSmartCardEap::Open() to convert the
TAID (which is binary data) to a string that can be passed through
ETel.  The conversion changes the binary data to a string of
hexadecimal semi-octets.

@param aBinData Buffer containing the binary data to be converted.
@param aText Buffer will contain the text representation of aBinData on
             return.

@see RMobileSmartCardEap::Open()
*/
void RMobileSmartCardEap::ConvertBinToText(const TDesC8& aBinData, TDes& aText)
	{
	TInt binLength = aBinData.Length();

	for (TInt pos = 0; pos < binLength; pos++)
		{
		_LIT(format, "%02X");
		aText.AppendFormat(format, aBinData[pos]);
		}
	}
