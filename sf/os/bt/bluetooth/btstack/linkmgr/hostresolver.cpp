// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// L2CAP Host resolver.
// Implements Inquiry and Name lookup
// 
//

#include "hostresolver.h"
#include <bluetooth/logger.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/hciconsts.h>
#include <bluetooth/hci/inquirycommand.h>
#include <bluetooth/hci/inquirycancelcommand.h>
#include <bluetooth/hci/writeinquirymodecommand.h>
#include <bluetooth/hci/writeextendedinquiryresponsecommand.h>
#include <bluetooth/hci/readextendedinquiryresponsecommand.h>
#include <bluetooth/hci/remotenamerequestcommand.h>
#include <bluetooth/hci/remotenamereqcompleteevent.h>
#include <bluetooth/hci/remotenamerequestcancelcommand.h>
#include <bluetooth/hci/remotehostsupportedfeaturesnotificationevent.h>
#include <bluetooth/hci/readlocalnamecommand.h>

// Command Events
#include <bluetooth/hci/inquiryresultevent.h>
#include <bluetooth/hci/inquiryresultwithrssievent.h>
#include <bluetooth/hci/extendedinquiryresultevent.h>

// Command Complete Events
#include <bluetooth/hci/readlocalnamecompleteevent.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/commandstatusevent.h>

#include <e32std.h>
#include <s32mem.h>
#include <bt_sock.h>
#include <utf.h>
#include "debug.h"
#include "linkutil.h"
#include "physicallinksmanager.h"	// to get baseband freed up for inquiries
#include "BTSec.h"

//Diagnostic string for security check failures, in builds without platsec
//diagnostics this will be NULL.
const char* const KBT_HOSTRESOLVER_NAME_DIAG = __PLATSEC_DIAGNOSTIC_STRING("Bluetooth Host Resolver");

const static TUint KHostResDontBlock = 4;

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HOSTRESOLVER);
#endif



//
//	BT Inquiry Result record.
//

CBTInqResultRecord::CBTInqResultRecord(const TBTDevAddr& aAddr)
	: iCodec(iEntry.iExtendedInquiryResponse)
	{
	LOG_FUNC
	iEntry.iBdaddr = aAddr;
	// Default to page scan repetition mode R2 as recommended by Bray to give best
	// chance of success for host resolver needs.
	// Also allow other default values to be entered here
	iEntry.iPageScanRepetitionMode = KDefaultBluetoothPageScanRepMode;
	iEntry.iPageScanMode = KDefaultBluetoothPageScanMode;
	iEntry.iClockOffset = KDefaultBluetoothClockOffset;
	iEntry.iCoD = KDefaultBluetoothClassOfDevice;
	
	if(aAddr!=TBTDevAddr())
		{
		iJuiceFromHCIMask = EBluetoothAddr;
		}
	//else 0 cos 'C' class
	
	SetNameValid(EFalse);
	}

CBTInqResultRecord::~CBTInqResultRecord()
	{
	LOG_FUNC
	iIACs.Close();
	}

inline void CBTInqResultRecord::Open()
/**
	Some reference is now pointing at us.
	Inc the ref count.
**/
	{
	LOG_FUNC
	++iRefCount;
	}

void CBTInqResultRecord::Close()
/**
	Reference closing.
	Auto delete if this was the last reference pointing to us.
**/
	{
	LOG_FUNC
	--iRefCount;
	if (iRefCount <= 0)
		delete this;
	}

TInquiryLogEntry& CBTInqResultRecord::LogEntry()
/**
	The inquiry result.
	This is stored as it was returned from the HCI.
	The host resolver will convert it into an InquirySockAddr
	when a client actually wants to retreve it.
**/
	{
	LOG_FUNC
	return iEntry;
	}

inline void CBTInqResultRecord::SetName(const TDesC8& aName)
	{
	LOG_FUNC
	iName = aName;
	}

inline const TDesC8& CBTInqResultRecord::Name() const
/**
	The UTF name of the remote device.
	This is stored as it comes in off the wire, the host resolver
	converts to Unicode if & when a client actually requires it.
**/
	{
	LOG_FUNC
	return iName;
	}

inline TInt CBTInqResultRecord::IncFlushes()
/**
	Age the result record
	Returns it's new age.
**/
	{
	LOG_FUNC
	return ++iFlushes;
	}

inline TBool CBTInqResultRecord::IsNameValid() const
	{
	LOG_FUNC
	return iNameLookupResultCode == KErrNone;
	}

inline void CBTInqResultRecord::SetNameValid(TBool aBool)
	{
	LOG_FUNC
	if(aBool)
		iNameLookupResultCode = KErrNone;
	else
		iNameLookupResultCode = 1;
	}

void CBTInqResultRecord::SetNameLookupResultCode(TInt aResultCode)
	{// Only record error, if we don't have a valid name already
	LOG_FUNC
	if(!IsNameValid())
		iNameLookupResultCode = aResultCode;
	}

inline TBool CBTInqResultRecord::IsNameRequestPending() const
	{
	LOG_FUNC
	return iNameStatus & ENamePending;
	}

inline void CBTInqResultRecord::SetNamePending(TBool aBool)
	{
	LOG_FUNC
	if(aBool)
		iNameStatus |= ENamePending;
	else
		iNameStatus &= ~ENamePending;
	}

inline TBool CBTInqResultRecord::IsNameRefreshRequested() const
	{
	LOG_FUNC
	return iNameStatus & ENameRefreshRequested;
	}

inline void CBTInqResultRecord::SetNameRefreshRequested(TBool aBool)
	{
	LOG_FUNC
	if(aBool)
		iNameStatus |= ENameRefreshRequested;
	else
		iNameStatus &= ~ENameRefreshRequested;
	}

inline TBool CBTInqResultRecord::IsNameComplete() const
	{
	LOG_FUNC
	return iNameStatus & ENameComplete;
	}

inline void CBTInqResultRecord::SetNameComplete(TBool aBool)
	{
	LOG_FUNC
	if(aBool)
		iNameStatus |= ENameComplete;
	else
		iNameStatus &= ~ENameComplete;
	}

inline TBool CBTInqResultRecord::IsExplicitNameRequest() const
	{
	LOG_FUNC
	return iNameStatus & ENameExplicitRequest;
	}

inline void CBTInqResultRecord::SetExplicitNameRequest(TBool aBool)
	{
	LOG_FUNC
	if(aBool)
		iNameStatus |= ENameExplicitRequest;
	else
		iNameStatus &= ~ENameExplicitRequest;
	}

inline TBool CBTInqResultRecord::HaveNameLookupResult() const
	{
	LOG_FUNC
	return iNameLookupResultCode <= KErrNone;
	}

inline TInt CBTInqResultRecord::NameLookupResultCode() const
	{// Only valid if HaveNameLookupResult == ETrue
	LOG_FUNC
	__ASSERT_DEBUG(HaveNameLookupResult(), Panic(EBTNameLookupResultNotFound));
	return iNameLookupResultCode;
	}

inline TInt CBTInqResultRecord::NameLookupAttempts() const
	{
	LOG_FUNC
	return iNameLookupAttempts;
	}

void CBTInqResultRecord::GetInquirySockAddr(TInquirySockAddr& aAddr)
/**
	Read this result record out into a TInquirySockAddr.
**/
	{
	LOG_FUNC
	aAddr.SetBTAddr(LogEntry().iBdaddr);

	TBTDeviceClass cod(LogEntry().iCoD);
	aAddr.SetMajorServiceClass(cod.MajorServiceClass());
	aAddr.SetMajorClassOfDevice(cod.MajorDeviceClass());
	aAddr.SetMinorClassOfDevice(cod.MinorDeviceClass());
	
	TUint8 resultFlags = 0;
	
	if(iJuiceFromHCIMask & EBluetoothRssi)
		{
		TInquiryLogEntry* logEntry = &(LogEntry());
		resultFlags |= TInquirySockAddr::ERssiValid;
		aAddr.SetRssi(static_cast<TInquiryLogEntryWithRssi*>(logEntry)->iRssi.RSSI());
		}

	aAddr.SetResultFlags(resultFlags);
	}

TInt CBTInqResultRecord::GetEir(TNameRecord& aNameRec, TBool aIgnoreCachedName)
	{
	// Reformat the EIR (i.e. remove 0s from EIR) for the constructor
	LOG_FUNC
	TInt error = KErrNone;

	aNameRec.iName.Zero();
	iCodec.Set(aNameRec);
	if(IsEirPresent())
		{
		TInquiryLogEntry* logEntry = &(LogEntry());
		iCodec.Copy(static_cast<TInquiryLogEntryWithEir*>(logEntry)->iExtendedInquiryResponse);
		}
	// replace device name in eir with iName if there is a complete name present
	if(IsNameValid() && IsNameComplete() && !aIgnoreCachedName)
		{
		iCodec.SetDeviceName(iName, ETrue);
		}
	return error;
	}

TInt CBTInqResultRecord::GetName(TNameRecord& aNameRec)
	{
	LOG_FUNC
	TInt err = KErrNotFound;
	if(HaveNameLookupResult())
		{
		err = CnvUtfConverter::ConvertToUnicodeFromUtf8(aNameRec.iName, iName);
		if (err >= KErrNone)
			{
			err = KErrNone;
			if(!IsNameComplete())
				{
				aNameRec.iFlags |= TNameRecord::EPartial;
				}
			else
				{
				aNameRec.iFlags &= ~(TNameRecord::EPartial);
				}
			}
		}
	else
		{
		err = KErrNone; // just following the old way of working.
		aNameRec.iName.Zero();
		}
	return err;
	}

TInt CBTInqResultRecord::AddIAC(TUint aIAC)
/**
	Adds IAC into list of those this device has responded to
	@return KErrNone	IAC added without any bother
	@return KErrAlreadyExists	IAC already in list
**/
	{
	LOG_FUNC
	return iIACs.InsertInOrder(aIAC);
	}

inline TBool CBTInqResultRecord::HasRespondedToIAC(TUint aIAC)
	{
	LOG_FUNC
	return (iIACs.FindInOrder(aIAC) >= 0);
	}

inline TInt CBTInqResultRecord::NumberOfIACsRespondedTo()
	{
	LOG_FUNC
	return iIACs.Count();
	}

inline void CBTInqResultRecord::ClearIACs()
	{
	LOG_FUNC
	iIACs.Reset();
	}

//
//	Inquiry result reference.
//
CBTInqResultRef::CBTInqResultRef(CBTInqResultRef& aRef)
	: iRecord(aRef.iRecord)
	{
	LOG_FUNC
	iRecord.Open();
	}

CBTInqResultRef::~CBTInqResultRef()
	{
	LOG_FUNC
	iRecord.Close();
	iLink.Deque();
	}

CBTInqResultRef::CBTInqResultRef(CBTInqResultRecord& aRec)
	: iRecord(aRec)
	{
	LOG_FUNC
	iRecord.Open();
	}

inline CBTInqResultRecord& CBTInqResultRef::Result() const
	{
	LOG_FUNC
	return iRecord;
	}



//
//	BT Inquiry Result Set.
//

CBTInqResultSet::CBTInqResultSet()
	: iResultRefs(_FOFF(CBTInqResultRef, iLink)),
	iNextRefIter(iResultRefs)
	{
	LOG_FUNC
	}

CBTInqResultSet::~CBTInqResultSet()
	{
	LOG_FUNC
	Reset();
	}

void CBTInqResultSet::Reset()
	{
	LOG_FUNC
	while (!iResultRefs.IsEmpty())
		delete iResultRefs.First();
	ReturnToFirstResult();
	}

CBTInqResultRef* CBTInqResultSet::Add(CBTInqResultRecord& aRec)
	{
	LOG_FUNC
	CBTInqResultRef* ref = new CBTInqResultRef(aRec);
	if (!ref)
		{
		return 0;
		}
	iResultRefs.AddLast(*ref);
	if (!iNextRefIter)
		{
		iNextRefIter.SetToLast();
		}
	return ref;
	}

CBTInqResultRef* CBTInqResultSet::FindEntry(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	TResultQueIter iter(iResultRefs);
	while (iter)
		{
		CBTInqResultRef* ref = iter++;
		if (ref->Result().LogEntry().iBdaddr == aAddr)
			{
			return ref;
			}
		}
	return 0;
	}

CBTInqResultRef* CBTInqResultSet::NextResult()
	{
	LOG_FUNC
	return iCurrentResult = (iNextRefIter++);
	}

inline CBTInqResultRef* CBTInqResultSet::CurrentResult()
	{
	LOG_FUNC
	return iCurrentResult;
	}

void CBTInqResultSet::ReturnToFirstResult()
	{
	LOG_FUNC
	iNextRefIter.SetToFirst();
	iCurrentResult = 0;
	}

inline TBool CBTInqResultSet::IsEmpty()
	{
	LOG_FUNC
	return iResultRefs.IsEmpty();
	}

void CBTInqResultSet::MoveToback(CBTInqResultRef& aRef)
	{
	LOG_FUNC
	if(iNextRefIter == &aRef)
		NextResult();
	aRef.iLink.Deque();
	iResultRefs.AddLast(aRef);
	if (!iNextRefIter)
		{
		iNextRefIter.SetToLast();
		}
	}

//
//	BT Host resolver.
//


CBTHostResolver::CBTHostResolver(CBTInquiryMgr& aInquiryMgr)
	: iInquiryMgr(aInquiryMgr), 
	iRequestState(EIdle),
	iInquiryStatus(EInquiryReady)
	{
	LOG_FUNC
#ifdef _DEBUG
	iInquiryMgr.IncrementHRCount();
#endif
	}

CBTHostResolver::~CBTHostResolver()
	{
	LOG_FUNC
	iLink.Deque();
	iInquiryMgr.DeletingHostResolver();
#ifdef _DEBUG
	iInquiryMgr.DecrementHRCount();
#endif
	}

void CBTHostResolver::GetByName(TNameRecord& /*aName*/)
	{
	LOG_FUNC
	iNotify->QueryComplete(KErrNotSupported);
	}

void CBTHostResolver::GetByAddress(TNameRecord& aName)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iRequestState == EIdle, Panic(EHostResolverTwoRequests));
	iNameRecord = &aName;
	iNameRecord->iName.Zero();
	iSockAddr = TInquirySockAddr::Cast(aName.iAddr);

	LOG1(_L("Host Resolver\tAction = %d"),iSockAddr.Action());

	if(iSockAddr.Action() & KHostResCache)
		//Complete immediately with info if available!
		{
		TInt err = KErrNotFound;
		CBTInqResultRef* ref = iInquiryMgr.FindExistingCacheEntry(iSockAddr.BTAddr());
		if (ref)
			{// Got a result to send up
			CBTInqResultRecord& rec = ref->Result();
			rec.GetInquirySockAddr(TInquirySockAddr::Cast(aName.iAddr)); //Put BT address, CoD etc into 'aName'
			// Check whether client wants EIRs instead of names
			if(iSockAddr.Action() & KHostResEir)
				{
				// Client knows about EIR, we'll fill the TNameRecord with EIR
				err = rec.GetEir(aName, EFalse);
				}
			else
				{
				// Client don't knows about EIR, we'll fill the TNameRecord with just encoded device name if present
				err = rec.GetName(aName);
				}
			if (err != KErrNone)
				{// error getting name - but other stuff fine - 
				 // so ensure name is not gobbledigook
				aName.iName.Zero();
				}
			err = KErrNone; //err has new meaning now, vis "record exists"
			}
		else
			{
			//err is KErrNotFound
			}
					
		iNotify->QueryComplete(err);
		return;
		}
	
	// Must request at least one of inquiry or name lookup
	if (!(iSockAddr.Action() & (KHostResInquiry | KHostResName)))
		{
		iRequestState = EError;
		CompleteRequest(KErrArgument);
		return;
		}

	// Check the hw state - can't do anything if its switched off
	//
	if(iInquiryMgr.HWState()==CBTInquiryMgr::EOff)
		{
		iRequestState = EError;
		CompleteRequest(KErrHardwareNotAvailable);
		return;
		}

	iNameLookupMode = (iSockAddr.Action()) & KHostResName ? EDoGetNames : EDontGetNames;
	TBool ignoreCache = (iSockAddr.Action() & KHostResIgnoreCache) ? ETrue : EFalse;

	if (iSockAddr.Action() & KHostResInquiry)
		{
		// We only support GIAC and LIAC
		if (iSockAddr.IAC() != KLIAC && iSockAddr.IAC() != KGIAC)
			{
			iRequestState = EError;
			CompleteRequest(KErrNotSupported);
			return;
			}
		iSockAddr.SetBTAddr(TBTDevAddr());
		iRequestState = EInquiry;
		if (iInquiryStatus == EInquiryReady)
			{// Need to start the inquiry process
			iInquiryStatus = EInquiring;
			// We set this before calling StartInquiry, as StartInquiry can call InquiryComplete synchronously. 
			iResults.Reset();
			iInquiryMgr.StartInquiry(*this, iSockAddr.IAC(), ignoreCache);
			}
		}
	else // Not an inquiry - just a name lookup
		{
		// Just name lookup -- add it to the result set, and request a lookup of it
		iResults.Reset();

		CBTInqResultRef* ref = iInquiryMgr.AddEntryToCache(iSockAddr.BTAddr());
		if(ref)
			{// This ref is in cache -- need to make our own ref
			ref = iResults.Add(ref->Result());
			}
		if (!ref)
			{
			CompleteRequest(KErrNoMemory);
			return;
			}
		iResults.NextResult(); // Move iter onto first (only) result
		iRequestState = ENameLookup;
		iInquiryMgr.LookupName(*this, iSockAddr.BTAddr(), ignoreCache, ETrue);
		}
	TryToCompleteRequest();
	}

TInt CBTHostResolver::SetOption(TUint /*aLevel*/, TUint /*aName*/, const TDesC8& /*aOption*/)
	{
	LOG_FUNC
	return KErrNone;
	}

void CBTHostResolver::SetHostName(TDes& aNameBuf)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iRequestState == EIdle, Panic(EHostResolverTwoRequests));
	
	TBuf8<KHCILocalDeviceNameMaxLength> utf8Name;
	TInt ret = CnvUtfConverter::ConvertFromUnicodeToUtf8(utf8Name, aNameBuf);
	iRequestState=ESetLocalName;
	if (ret > KErrNone)
		{// Unconverted characters exist! Error the client
		ret = KErrHostResNameTooLong;
		}

	// Check the hw state - can't do anything if its switched off
	//
	if(iInquiryMgr.HWState()==CBTInquiryMgr::EOff)
		{
		ret = KErrHardwareNotAvailable;
		}

	if (ret == KErrNone)
		ret = iInquiryMgr.SetLocalName(utf8Name);

	if (ret != KErrNone)
		{
		CompleteRequest(ret);
		}
	}

void CBTHostResolver::GetHostName(TDes& aNameBuf)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iRequestState == EIdle, Panic(EHostResolverTwoRequests));

	iHostNameBuf = &aNameBuf;
	iRequestState=EGetLocalName;
	TInt ret = KErrNone;

	// Check the hw state - can't do anything if its switched off
	//
	if(iInquiryMgr.HWState()==CBTInquiryMgr::EOff)
		{
		ret = KErrHardwareNotAvailable;
		}

	if(ret==KErrNone)
		iInquiryMgr.GetLocalName();
	else
		CompleteRequest(ret);
	}


void CBTHostResolver::CancelCurrentOperation()
/**
	Current operation cancel by user app.
	Reset, so that we don't bother picking up any future results.
	We don't actually bother to cancel the async. operations, as
	if they succeed, the InquiryMgr may as well cache them anyway.
	The next operation after this (if there is any), must be a fresh
	one -- i.e. it can not be a next.
**/
	{
	LOG_FUNC
	iRequestState = EIdle;
	CompleteCurrentOperation();
	}

void CBTHostResolver::InquiryResult(CBTInqResultRecord& aResult)
	{
	LOG_FUNC
	if (iInquiryStatus != EInquiring)
		return;

	// Check if we've already seen this result
	CBTInqResultRef* ref = iResults.FindEntry(aResult.LogEntry().iBdaddr);
	if (ref)
		return;

	if(!aResult.HasRespondedToIAC(iSockAddr.IAC()))
		return; // It's never responded to our IAC, so ignore it

	ref = iResults.Add(aResult);
	if (!ref)
		{
		InquiryComplete(KErrNoMemory);
		return;
		}

	if(iNameLookupMode == EDoGetNames)
		{
		// Find out whether cache was supposed to be ignored from TNameRecord that was passed into GetByAddress() previously
		TBool ignoreCache = (iSockAddr.Action() & KHostResIgnoreCache) ? ETrue : EFalse;
		// Handle a "special" case: Cache is to be ignored, but we got the _complete_ name during the inquiry process inside an EIR
		// (which doesn't count as cached data, since it's freshly arrived from the radio). In this case we don't really want to 
		// launch a remote name request
		if(ignoreCache && ref->Result().IsEirPresent() && ref->Result().Codec().IsDataTypePresent(EEirLocalNameComplete))
			{
			// Do nothing, the entry already contains the name
			}
		else
			{
			iInquiryMgr.LookupName(*this, aResult.LogEntry().iBdaddr, ignoreCache, EFalse);
			}
		}

	if (iRequestState == EInquiry)
		{
		TryToCompleteRequest();
		}
	}

void CBTHostResolver::NameLookupResult(TInt aErr, const TBTDevAddr& aAddr, const TDesC8& aName)
	{
	LOG_FUNC
	if (iNameLookupMode == EDontGetNames)
		return; // We're not actually after names

	// Check we've got this result recorded (mostly for error cases)
	CBTInqResultRef* ref = iResults.FindEntry(aAddr);
	if (ref && !(ref->Result().HaveNameLookupResult() && ref->Result().IsNameComplete()))
		{
		// we have successfully created a reference and there isn't a complete name available.
		ref->Result().SetNameLookupResultCode(aErr);
		ref->Result().SetNameComplete(ETrue);
		ref->Result().SetName(aName);
		}

	TryToCompleteRequest();
	}

void CBTHostResolver::InquiryComplete(TInt aErr)
	{
	LOG_FUNC
	// LC changed - if we are just doing a name lookup, iInquiryStatus
	// doesn't get set so need additional check here.
	//
	if ((iInquiryStatus != EInquiring) && (iRequestState != ENameLookup))
		return;
	iInquiryStatus = EInquiryComplete;
	iInqCompletionCode = aErr;
	if(aErr==KErrHardwareNotAvailable)
		iRequestState=EError;
	TryToCompleteRequest();
	}

void CBTHostResolver::SetLocalNameComplete(TInt aErr)
	{
	LOG_FUNC
	if (iRequestState != ESetLocalName)
		return;
	CompleteRequest(aErr);
	}

void CBTHostResolver::GetLocalNameComplete(TInt aErr, const TDesC8& aName)
	{
	LOG_FUNC
	if (iRequestState != EGetLocalName)
		return;
	if (aErr == KErrNone)
		{
		aErr = CnvUtfConverter::ConvertToUnicodeFromUtf8(*iHostNameBuf, aName);
		// Just allow names to be truncated...
		}
	CompleteRequest(aErr >= 0 ? KErrNone : aErr);
	}

TUint CBTHostResolver::GetIAC() const
	{
	LOG_FUNC
	TUint ret = 0;
	if (iInquiryStatus == EInquiring)
		{
		ret = iSockAddr.IAC(); // Only return IAC if an inquiry is requested
		}
	
	return ret;
	}

TInt CBTHostResolver::SecurityCheck(MProvdSecurityChecker *aSecurityChecker)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aSecurityChecker, User::Panic(KSECURITY_PANIC, EBTPanicNullSecurityChecker));
		
	iSecurityChecker = aSecurityChecker;
	return iSecurityChecker->CheckPolicy(KLOCAL_SERVICES, KBT_HOSTRESOLVER_NAME_DIAG);			
	}

void CBTHostResolver::TryToCompleteRequest()
/**
	This is where the work is done.
	A big old state machine that tries to get us one step
	closer to completing the query.
	Gets an inquiry result out of the result set if appropriate and
	there is one available.
	Gets the name for the current result if required.
	Completes the request if there is then nothing more to do.
	Must only break out (i.e. return) if the request is completed, or
	non-blocking mode requested
**/
	{
	LOG_FUNC
	if ((iSockAddr.Action() & KHostResInquiry) && iRequestState == EInquiry)
		{
		CBTInqResultRef* refNextRes = iResults.NextResult();
		if (refNextRes)
			{// Got a result to send up
			if (iNameLookupMode == EDoGetNames)
				{// Need to get the name of this baby...
				iRequestState = ENameLookup;
				}
			else
				{//Complete!
				refNextRes->Result().GetInquirySockAddr(TInquirySockAddr::Cast(iNameRecord->iAddr));
				// If EIR was requested and we got one, stick it in the name field
				TInt err = KErrNone;
				if(iSockAddr.Action() & KHostResEir)
					{
					err = refNextRes->Result().GetEir(*iNameRecord, (iSockAddr.Action() & KHostResIgnoreCache) && !(iSockAddr.Action() & KHostResName));
					}
				else
					{
					err = refNextRes->Result().GetName(*iNameRecord);
					}
				if(err != KErrNone)
					{
					// Set name back to zero length
					iNameRecord->iName.Zero();
					}
				CompleteRequest(err);
				return;
				}
			}
		else if (iInquiryStatus == EInquiryComplete)
			{// Bad luck. No more results availble!
			if (iInqCompletionCode == KErrNone)
				{// Inquiry completed fine, so report end of file.
				CompleteRequest(KErrHostResNoMoreResults);
				}
			else
				{// Some error in the inquiry, so report that.
				CompleteRequest(iInqCompletionCode);
				}
			return;
			}
		}

	if ((iSockAddr.Action() & KHostResName) && iRequestState == ENameLookup)
		{
		CBTInqResultRef* refNextRes = iResults.CurrentResult();
		if(!refNextRes)
			{// Can't get the name... no result to lookup
			CompleteRequest(KErrHostResNoMoreResults);
			return;
			}
		// Walk through results and check if we have a name to complete
		CBTInqResultRef* startResult = refNextRes;
		do
			{
			CBTInqResultRecord& rec = refNextRes->Result();
			if(rec.HaveNameLookupResult() && !rec.IsNameRequestPending() && rec.IsNameComplete())
				{// Got a name! Complete the request
				rec.GetInquirySockAddr(TInquirySockAddr::Cast(iNameRecord->iAddr));
				TInt err = rec.NameLookupResultCode();
				if (err == KErrNone)
					{// Copy & convert the UTF-8 name over into the result record.
					if(iSockAddr.Action() & KHostResEir)
						{
						err = rec.GetEir(*iNameRecord, EFalse);
						}
					else
						{
						err = rec.GetName(*iNameRecord);
						}
					}
				if (err != KErrNone)
					{
					iNameRecord->iName.Zero();
					}
				// Name lookup complete (poss. with errors)
				if(iSockAddr.Action() & KHostResInquiry)
					{// Ignore name failures if we're also doing inquiry
					CompleteRequest(KErrNone);
					}
				else
					{// Report any error encountered (or none)
					CompleteRequest(err);
					}
				return;
				}

			iResults.MoveToback(*refNextRes);
			refNextRes = iResults.NextResult();
			} while(refNextRes != startResult);
		}
	else if(iRequestState==EError)
		{
		CompleteRequest(KErrHardwareNotAvailable);
		}
	if(iSockAddr.Action() & KHostResDontBlock)
		{// Complete any outstanding non-blocking operation
		CompleteRequest(KErrWouldBlock);
		}

	}

void CBTHostResolver::CompleteRequest(TInt aErr)
	{
	LOG_FUNC
	if (iRequestState == EIdle && aErr == KErrNone) 
		return;
	iRequestState = EIdle;
	if (aErr != KErrNone)
		{// End of query -- tidy up
		CompleteCurrentOperation();
		}
	iNotify->QueryComplete(aErr);
	}

void CBTHostResolver::CompleteCurrentOperation()
/**
	Operation known to be complete.
	This happens through a CancelCurrentOperation, or when
	the current operation has read all the results out.
**/
	{
	LOG_FUNC
	if (iInquiryStatus == EInquiryReady)
		return;
	iInquiryStatus = EInquiryReady;
	iResults.Reset();
	}


//
//	BT Inquiry Manager.
//
CBTInquiryMgr::CBTInquiryMgr(CLinkMgrProtocol& aProtocol)
	: iLinkMgrProtocol(aProtocol),
	iNamePageTimeout((KMaxNamePageTimeout + KMinNamePageTimeout)/2), // start with mid-point timeout
	iInquiryMode(EStandardInquiryFormat), iHRs(CBTHostResolver::LinkOffset())
	{
	LOG_FUNC
	}

CBTInquiryMgr::~CBTInquiryMgr()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iHRs.IsEmpty(), Panic(EBTHostControllerQueueNotEmpty));
	SetHWState(EOff);
	PublishStatus();// we're not doing anything more so publish status
	ClearHCICommandQueue();
	delete iFlusher;
	delete iConnectingStatus;
	}

void CBTInquiryMgr::ClearHCICommandQueue()
	{
	LOG_FUNC
	if (iCommandQueue)
		{
		iCommandQueue->MhcqRemoveAllCommands(*this);
		}
	iCommandQueue = NULL;
	}

void CBTInquiryMgr::SetHCICommandQueue(MHCICommandQueue& aCommandQueue)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iCommandQueue, Panic(EBTCommandQueueAlreadyExist));
	iCommandQueue = &aCommandQueue;
	}

MHCICommandQueue& CBTInquiryMgr::CommandQueue() const
	{
	LOG_FUNC
	__ASSERT_DEBUG(iCommandQueue, Panic(EBTCommandQueueNotFound));
	return *iCommandQueue;
	}

CBTInquiryMgr* CBTInquiryMgr::NewL(CLinkMgrProtocol& aProtocol)
	{
	LOG_STATIC_FUNC
	CBTInquiryMgr* self = new(ELeave) CBTInquiryMgr(aProtocol);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CBTInquiryMgr::ConstructL()
	{
	LOG_FUNC
	iFlusher = CPeriodic::NewL(CActive::EPriorityLow);
#ifdef CONNECTION_PREEMPTS_INQUIRY
	iConnectingStatus = CConnectingStatusSubscriber::NewL(*this);
#endif
	}

CBTHostResolver* CBTInquiryMgr::NewHostResolverL()
	{
	LOG_FUNC
	// Leave if HW switched off
	//
	if(iHWState==EOff)
		User::Leave(KErrHardwareNotAvailable);

	CBTHostResolver* r = new(ELeave) CBTHostResolver(*this);
	iHRs.AddFirst(*r);
	return r;
	}

void CBTInquiryMgr::DeletingHostResolver()
	{
	LOG_FUNC
	// A host resolver is being deleted - we need to check if we need to change IAC.
	// If no host resolver is requesting an inquiry, just let the inquiry run to fill up the cache 
	TUint requestedIAC = RequestedInquiryIAC();
	if (iHWState == EInquiry && requestedIAC != 0 && iCurrentInquiryIAC != requestedIAC)
		{
		TInt err = CancelHardwareInquiry();
		if(err==KErrNone)
			{
			SetHWState(ECancellingForNewIAC);
			iFlusher->Cancel();     // Stop watchdog, start flusher.
			}
		}
	}

TUint CBTInquiryMgr::RequestedInquiryIAC()
    {
    TUint ret = 0;
    TDblQueIter<CBTHostResolver> iter(iHRs);
    CBTHostResolver* hostRes = NULL;
    while ((hostRes = iter++) != NULL)
        {
    	// Only overwrite the requested IAC if:
    	// - We haven't found a host resolver requesting an IAC yet, or
    	// - This host resolver requests LIAC, which we prioritise over GIAC
        if (ret == 0 || hostRes->GetIAC() == KLIAC)
            {
            ret = hostRes->GetIAC();
            }
        }
    return ret;
    }

void CBTInquiryMgr::InquiryResult(TInt aErr,const TInquiryLogEntry& aEntry)
/**
	Single Inquiry result received.
	Called via HCI for each incoming inquiry result. Need to demultiplex these
	out to all CBTHostResolver's interested in them.

	@param  aErr	An EPOC error code. Any HCI mapping should be done before
					calling this function.
	@param	aEntry	The log entry returned through HCI.
**/
	{
	LOG_FUNC
	LOG5(_L("err: %d,ScanRepM: %d ScanPerM: %d ScanM: %d Type: %d"),
				 aErr,
				 aEntry.iPageScanRepetitionMode,
				 aEntry.iPageScanPeriodMode,
				 aEntry.iPageScanMode,
			   	 aEntry.iSpare );

	CBTInqResultRef* ref = 0;

	//Revise our cache record regardless - at least need to make sure
	//it knows we have received juice from the HCI	
	if(aErr == KErrNone)
		{
		ref = AddEntryWithJuiceToCache(aEntry);
		if (!ref)
			{
			aErr = KErrNoMemory;
			}
		else
			{
			ref->Result().iFlushes = 0;
			ref->Result().iFoundDuringCurrentInquiry = ETrue;

			TInt ret = ref->Result().AddIAC(iCurrentInquiryIAC);
			switch(ret)
				{
			case KErrNone:
				++iResultCount;
				// drop through
			case KErrAlreadyExists:
				break;
			default: // Got an error adding the IAC
				aErr = ret;
				} // switch
			ref->Result().AddIAC(KGIAC); // This would come back on a GIAC, so try adding GIAC.
			// If this doesn't work, it doesn't really matter, so ignore the error.
			} // else
		} // if (aErr == KErrNone)

	if (aErr != KErrNone)
		{
		InquiryComplete(aErr, 0);
		return;
		}

	// Tell host resolvers
	TDblQueIter<CBTHostResolver> iter(iHRs);
	CBTHostResolver* hr;
	while ((hr = iter ++) != NULL)
		{
		if (hr->GetIAC() == iCurrentInquiryIAC || hr->GetIAC() == KGIAC)
			{
			hr->InquiryResult(ref->Result());
			}
		}
	}

void CBTInquiryMgr::CoDResult(const TBTDevAddr& aAddr, TUint aCoD)
/**
	We have been donated a CoD from an incoming connection
	This is useful to keep in the registry for remotely
	initiated pairings - otherwise we will never have found
	the CoD of the device bonding with us
**/	
	{
	// create record if it doesn't already exist in cache. 
	// If this fails, we'll just not be able to store the CoD for this device.

	LOG_FUNC
	AddEntryWithCoDToCache(aAddr, aCoD);
	}

void CBTInquiryMgr::ClockOffsetResult(const TBTDevAddr& aAddr, TBasebandTime aClockOffset)
/**
	We have been donated a Clock Offset - maybe an update
**/	
	{
	LOG_FUNC
	AddEntryWithClockOffsetToCache(aAddr, aClockOffset);
	}

void CBTInquiryMgr::InquiryComplete(TInt aErr, TUint8 /*aNumResponses*/)
/**
	Current Inquiry has completed.
	Must tell all CBTHostResolver's who may be waiting for inquiry results.

	@param  aErr	An EPOC error code. Any HCI mapping should be done before
					calling this function.
	@param	aNumResponses	The number of calls to InquiryResult that should
					have been generated. We don't use this info atm, though.
**/
	{
	LOG_FUNC
	SetCacheAge(iCurrentInquiryIAC, 0);
	TUint iacToComplete = iCurrentInquiryIAC;
	iCurrentInquiryIAC = 0;
	iFlusher->Cancel(); // Stop watchdog, start flusher
	SetHWState(EIdle);
	// don't publish status here, might be doing a name lookup
	EnableFlusher();

	// The inquiry has completed so clear any results that were marked found during
	// the just finished inquiry
	ClearCurrentInquiryResults();

	if(aErr!=KErrHardwareNotAvailable)
		{
		LOG(_L("CBTInquiryMgr::InquiryComplete asking for another name lookup"));
		DoNameLookup(ETrue); // Get any pending name lookups going
		}
	else
		{
		SetHWState(EOff);
		LOG(_L("CBTInquiryMgr::InquiryComplete PublishState Idle"));
		PublishStatus();// we're not doing name lookup so publish status
		}
	TDblQueIter<CBTHostResolver> iter(iHRs);
	CBTHostResolver* hr;
	while ((hr = iter++) != NULL)
		{
		if (hr->GetIAC() == iacToComplete)
			{
			hr->InquiryComplete(aErr);
			}
		hr++;
		}
	// Only queue the inquiry if we have completed all the name requests from this one.
	// Otherwise, it will be issued after all the remote name requests have completed.
	if (RequestedInquiryIAC() != 0 && iPendingNameRequests == 0)
		{
		DoInquiry();
		}
	}

/**
Name lookup result received.
Called via HCI for each incoming name result.

The name lookup may not have been requested by the inquiry mgr as when the
stack gets a new connection it tries to solicit information about the
remote.  This sort of result is just to add to our cache.

Otherwise it's from a request on us, pass it on for handling.

@param	aErr	An EPOC error code. Any HCI mapping should be done before
				calling this function.
@param	aAddr	The address of the Bluetooth device that the name is associated
				with.
@param	aBuf	8 bit Descriptor up to KHCIRemoteDeviceNameMaxLength
				bytes long, containing the name.
*/
void CBTInquiryMgr::RemoteNameResult(TInt aErr, const TBTDevAddr& aAddr, const TBTDeviceName8& aBuf)
	{
	LOG_FUNC
	LOG1(_L("aErr = %d"), aErr);
	LOG6(_L(", aAddr: %02x %02x %02x %02x %02x %02x"),
			  TUint8(aAddr[0]), TUint8(aAddr[1]), TUint8(aAddr[2]), TUint8(aAddr[3]), TUint8(aAddr[4]), 
			  TUint8(aAddr[5]));

	CBTInqResultRef* ref = FindExistingCacheEntry(aAddr);
	if(ref)
		{
		CBTInqResultRecord& rec = ref->Result();
		if (rec.IsNameRequestPending())
			{
			// We asked for this
			HandleRemoteNameResult(aErr, *ref, aBuf);
			return;
			}
		}

	// Unsolicited, just wham in cache if it's good
	if (aErr == KErrNone)
		{
		CBTInqResultRef* ref = AddEntryToCache(aAddr);
		if(ref)
			{
			CBTInqResultRecord& rec = ref->Result();

			rec.SetName(aBuf);
			rec.SetNameComplete(ETrue);
			rec.iFlushes = 0;
			rec.SetNameValid(ETrue);
			rec.SetNameRefreshRequested(EFalse);
			}
		}
	}

/**
Remote host supported features notification received.

The remote host supported feature notification may be received during a
remote name request if the remote device provides entries in the remote
host supported feature page.

@param	aErr	An EPOC error code. Any HCI mapping should be done before
				calling this function.
@param	aAddr	The address of the Bluetooth device that the name is associated
				with.
@param	aHostSupportedFeatures	64 bit field holding the remote host supported
				features page.
*/
void CBTInquiryMgr::RemoteHostSupportedFeatures(TInt /*aErr*/, const TBTDevAddr& /*aAddr*/, const TUint64& /*aHostSupportedFeatures*/)
	{
	LOG_FUNC
	// This is currently not used by the stack since the optimisation it could provide would be
	// difficult to take advantage of and of minimal use.

	// If to be used the result should be stored in the inquiry result record along with whether
	// a name request has been made (with EIR it is possible to have retrieved the name without
	// a name request)
	}

/**
Name lookup result received.

We need to demultiplex these out to all CBTHostResolver's
interested in them.

@param  aErr	An EPOC error code. Any HCI mapping should be done before
				calling this function.
@param	aRef	The inquiry result for this info.
@param	aBuf	8 bit Descriptor up to KHCIRemoteDeviceNameMaxLength
				bytes long, containing the name.
*/
void CBTInquiryMgr::HandleRemoteNameResult(TInt aErr, CBTInqResultRef& aRef, const TBTDeviceName8& aBuf)
	{
	LOG_FUNC
	
	if(iHWState != EConnecting)
		{
		SetHWState(EIdle);
		// don't set publish status here, doinquiry will do that
		}
		
	if(aErr == KErrNone)
		{
		if(iNamePageTimeout > KMinNamePageTimeout)
			{// got name OK, dec the page timeout
			iNamePageTimeout -= KNamePageTimeoutIncrement;
			}
		}
	else if(aErr == KHCIErrorBase - EPageTimedOut)
		{
		if(iNamePageTimeout < KMaxNamePageTimeout)
			{// Got pagetime, inc. pagetime a bit
			iNamePageTimeout += KNamePageTimeoutIncrement;
			}
		}


	CBTInqResultRecord& rec = aRef.Result();

	--iPendingNameRequests;
	rec.SetNamePending(EFalse);
	++(rec.iNameLookupAttempts);
	if(iPendingNameRequests <= 0)
		{
		iNewPageRequestsPending = EFalse;
		}

	// Update our cached version.
	if (aErr == KErrNone)
		{
		rec.SetName(aBuf);
		rec.SetNameComplete(ETrue);
		rec.iFlushes = 0;
		rec.SetNameValid(ETrue);
		rec.SetNameRefreshRequested(EFalse);
		rec.SetExplicitNameRequest(EFalse); // If we had an explicit name request on this, we've completed it now
		}
	else
		{
		// got an error
		if(rec.NameLookupAttempts() < KMaxNameLookupAttempts)
			{// try to get later - put to back of Que for getting
			++iPendingNameRequests;
			rec.SetNamePending(ETrue);
			iCurrentResults.MoveToback(aRef);
			}
		else
			{// No more chances -- record lookup error code
			rec.SetNameLookupResultCode(aErr);
			rec.SetExplicitNameRequest(EFalse); // If we had an explicit name request on this, we've completed it now
			}
		}

	// Try to get more names, if needed
	LOG(_L("CBTInquiryMgr::HandleRemoteNameResult asking for another name lookup"));
	DoNameLookup(EFalse);

	if(!aRef.Result().IsNameRequestPending())
		{// Don't propogate if name request is still pending
		TDblQueIter<CBTHostResolver> iter(iHRs);
		while (iter)
			{
			(iter++)->NameLookupResult(aErr, rec.LogEntry().iBdaddr, aBuf);
			}
		}

	// In case we're now free, do inquiry
	LOG(_L("CBTInquiryMgr::HandleRemoteNameResult asking for another inquiry"));
	DoInquiry();
	}

const TDesC8* CBTInquiryMgr::DeviceNameFromCache(const TBTDevAddr& aAddr)
/**
	Used by other parts of the stack to get a name synchronously from cache if
	there is one

	@return NULL if device is unknwon
**/
	{
	LOG_FUNC
	CBTInqResultRef* entry = FindExistingCacheEntry(aAddr);
	if (!entry)
		return NULL;
	else
		{
		return &entry->Result().Name(); 
		}
	}

CBTInqResultRecord* CBTInquiryMgr::BasebandParametersFromCache(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	CBTInqResultRef* entry = FindExistingCacheEntry(aAddr);
	if (!entry)
		return NULL;
	else
		{
		return &(entry->Result());
		}
	}

void CBTInquiryMgr::SetHWState(THWState aState)
	{
	LOG_FUNC
	iHWState = aState;
	}

void CBTInquiryMgr::PublishStatus()
	{
	LOG_FUNC
	// sets the inquiry/discovery status to bool true or false
	TBool inquiryState = EFalse;

	switch (iHWState)
		{
	/* deliberate fall throughs */
	case EInquiry:
	case ENameLookup:
		{	
		inquiryState = ETrue;
		}
		break;
	case EIdle:
	case EConnecting:
	case EOff:
	default:
		{
		// defaulted above to EFalse;
		}
		break;
		}
	/* only publish status if we've changed	
	 it is however the case that if the state changes sufficiently quickly
	 the subscriber will miss some of the state changes 
	 this is particularly the case while doing name searches where we toggle into 
	 idle and back to namelookup quickly and regularly, the subscriber
	 sees a stream of etrues coming through, the efalses are missing
	 this is probably ok as long as they know because this means they are told 
	 more than once they are inquiring, and indeed they are, its certainly better
	 than being told they're not if they are */	
	if (inquiryState != iReportedInquiryState)
		{		
		iLinkMgrProtocol.SetUIDiscovering(inquiryState);
		iReportedInquiryState = inquiryState;
		if (inquiryState)
			{
			LOG(_L("CBTInquiryMgr::PublishState setting status TRUE"));
			}
		else
			{
			LOG(_L("CBTInquiryMgr::PublishState setting status FALSE"));
			}
		}
	else
		{
		LOG(_L("CBTInquiryMgr::PublishState not updating status"));
		}			
 	}

void CBTInquiryMgr::SetLocalNameComplete(TInt aErr)
	{
	LOG_FUNC
	TDblQueIter<CBTHostResolver> iter(iHRs);
	while (iter)
		{
		(iter++)->SetLocalNameComplete(aErr);
		}
	}

void CBTInquiryMgr::GetLocalNameComplete(TInt aErr, const TDesC8& aName)
	{
	LOG_FUNC
	TDblQueIter<CBTHostResolver> iter(iHRs);
	while (iter)
		{
		(iter++)->GetLocalNameComplete(aErr, aName);
		}
	}

void CBTInquiryMgr::StartInquiry(CBTHostResolver& aResolver, TUint aIAC, TBool aIgnoreCache)
	{
	LOG_FUNC
	// Pre-load any existing results into the CBTHostResolver
	if (!aIgnoreCache)
		{
		// Pre-load any existing results into the CBTHostResolver
		iCurrentResults.ReturnToFirstResult();
		while (CBTInqResultRef* ref = iCurrentResults.NextResult())
			{
			// If GIAC is being requested, we return all results
			if (aIAC == KGIAC || ref->Result().HasRespondedToIAC(aIAC))
				{
				aResolver.InquiryResult(ref->Result());
				}
			}
		}
	
	if ( !aIgnoreCache && !iCurrentResults.IsEmpty() && CacheAge(aIAC) <= KCacheStaleAge)
		{// Cache not yet stale, so just give these results back and dont start
		LOG(_L("CBTInquiryMgr::StartInquiry giving result from cache"));
		aResolver.InquiryComplete(KErrNone);
		return;
		}
	
	if (iCurrentInquiryIAC)
		{
		LOG(_L("CBTInquiryMgr::StartInquiry"));
        if (aIgnoreCache && (aIAC == iCurrentInquiryIAC || aIAC == KGIAC))
            {
            // an Inquiry is ongoing, return any results already found during the
            // current Inquiry if not already done so as part of the complete cache
            iCurrentResults.ReturnToFirstResult();
            while (CBTInqResultRef* ref = iCurrentResults.NextResult())
                {
                if (ref->Result().iFoundDuringCurrentInquiry)
                    {
                    aResolver.InquiryResult(ref->Result());
                    }
                }   
            }
		if(iCurrentInquiryIAC == aIAC)
			{
			// the current Inquiry will just continue
			return;			
			}
		if(iHWState == EInquiry && RequestedInquiryIAC() != iCurrentInquiryIAC)
			{
			// The requested IAC (which prioritises LIAC) is different to the current IAC, so cancel to change the IAC
			TInt err = CancelHardwareInquiry();
			if(err!=KErrNone)
				{
				LOG(_L("CBTInquiryMgr::StartInquiry cancel didn't work"));
				aResolver.InquiryComplete(err); // cancel went wrong
				return;
				}
			SetHWState(ECancellingForNewIAC);
			iFlusher->Cancel(); 	// Stop watchdog, start flusher.
			return;
			}
		}

	iInquiryInteruptions = 0;

	DoInquiry();
	}

void CBTInquiryMgr::Suspend()
/**
	Ah - somebody wishes to make a connection - we need to clear the way
	If we are doing a namerequest we can cancel.
	
	WARNING - The caller of this must ensure a call to Resume() ASAP
**/
	{
	LOG_FUNC

	if(iHWState == EInquiry)
		{
		// This is a best effort service, the Connect will be tried even
		// if we fail to stop the inquiry, so ignore errors.
		static_cast<void>(CancelHardwareInquiry());
		}
	else if (iHWState == ENameLookup)
		{
		
		iCurrentResults.ReturnToFirstResult();
		while(CBTInqResultRef *ref = iCurrentResults.NextResult())
			{

			CBTInqResultRecord& rec = ref->Result();
			if(rec.IsNameRequestPending())
				{
				// This is a best effort service, the Connect will be tried even
				// if we fail to stop the Name request, so ignore errors.
				TRAP_IGNORE(CancelRemoteNameL(rec.LogEntry().iBdaddr));
				}
			}

		}
		
	iFlusher->Cancel(); 	// Stop watchdog, start flusher.
	SetHWState(EConnecting);	// will stop inquirymanager doing anything else
	// don't publish status here, will be cleared up when manager tries to do soemthing else
	EnableFlusher();
	}

void CBTInquiryMgr::Resume()
/**
	We can now carry on...
**/
	{
	LOG_FUNC
	if(iHWState == EConnecting)
		{
		LOG(_L("InquiryMgr: Hardware connected; resuming operations (when a new inquiry starts"));
	 	SetHWState(EIdle);	
	 	// don't publish status here, it will be set by namelookup
		LOG(_L("CBTInquiryMgr::Resume asking for another name lookup"));
		DoNameLookup(EFalse);
		DoInquiry();
		EnableFlusher(); // in case no operations started
		}
	}

void CBTInquiryMgr::LookupName(CBTHostResolver& aResolver, const TBTDevAddr& aAddr, TBool aIgnoreCache, TBool aExplicitNameRequest)
/**
This method is called by a CBTHostResolver to look up the name of a device with a given address. If the
name is not immediately available a name lookup request is queued within the inquiry manager. This 
request will be actioned by CBTInquiryMgr::DoNameLookup(), and the remote device response will be 
sent (by CBTInquiryMgr::RemoteNameResult) to all current host resolvers.
**/
	{
	LOG_FUNC
	LOG1(_L("CBTHostResolver: 0x%8x"), &aResolver);
	LOG6(_L(", aAddr: %02x %02x %02x %02x %02x %02x"),
 				  TUint8(aAddr[0]), TUint8(aAddr[1]), TUint8(aAddr[2]), TUint8(aAddr[3]), TUint8(aAddr[4]), 
 				  TUint8(aAddr[5]));
 				  
	CBTInqResultRef* ref = AddEntryToCache(aAddr); //N.B. Creates a new entry if one doesn't exist

	if(!ref)
		{
		aResolver.NameLookupResult(KErrNoMemory, aAddr, KNullDesC8);
		return;
		}

	// check state of cache entry
	CBTInqResultRecord& rec = ref->Result();

	if(rec.HaveNameLookupResult() && rec.IsNameComplete() && (!aIgnoreCache|| rec.iFoundDuringCurrentInquiry))
		{// Either got a name, or we made an attempt that failed
		aResolver.NameLookupResult(rec.NameLookupResultCode(),
								   rec.LogEntry().iBdaddr,
								   rec.iName);
		return;
		}
	
	if (aExplicitNameRequest)
		{
		rec.SetExplicitNameRequest(ETrue);
		}
	
	if(rec.IsNameRequestPending())
		{
		return; // it's already happening
		}

	rec.SetNamePending(ETrue);
	rec.iNameLookupAttempts = 0;
	++iPendingNameRequests;
	iNewPageRequestsPending = ETrue;

	if(iHWState == EInquiry)
		{
		if(iPendingNameRequests > iInquiryInteruptions
		   || iInquiryInteruptions < KImmediateNameFetch)
			{
			TryToInterruptInquiryForNameLookup();
			}
		}
	else
		{
		LOG(_L("CBTInquiryMgr::LookupName asking for another name lookup"));
		DoNameLookup(EFalse);
		}
	}

void CBTInquiryMgr::ClearCache()
	{
	LOG_FUNC
	if(iHWState == EInquiry)
		{
		TInt err = CancelHardwareInquiry();
		if(err==KErrNone)
			{
			// Don't clear cache if we couldn't cancel inquiry.
			// We'll be slightly unfaithful in the case of OOM...
			iCurrentResults.Reset();
			InquiryComplete(KErrNone, 0);
			}
		}
	else
		{
		iCurrentResults.Reset();
		}
	}

TInt CBTInquiryMgr::SetLocalName(const TDesC8& aName)
	{
	LOG_FUNC
	return iLinkMgrProtocol.SetLocalDeviceName(aName);
	}

TInt CBTInquiryMgr::GetLocalName()
	{
	LOG_FUNC
	TRAPD(err, ReadLocalNameL());
	return err;
	}

/**
	This is called whenever the local HCI version is updated (usually only during startup), 
	so that the inquiry mode is set accordingly
**/
void CBTInquiryMgr::SetInquiryMode()
	{
	LOG_FUNC
	
	THCIInquiryMode inquiryMode = EStandardInquiryFormat;
	
	if (iLinkMgrProtocol.IsExtendedInquiryResponseSupportedLocally())
		{
		inquiryMode = EInquiryWithRssiOrEir;
		}
	else if (iLinkMgrProtocol.IsRssiWithInquiryResultsSupportedLocally())
		{
		inquiryMode = EInquiryWithRssi;
		}
	
	if (inquiryMode != EStandardInquiryFormat)
		{
		TRAPD(err, WriteInquiryModeL(inquiryMode));
		if (err == KErrNone)
			{
			iPendingInquiryMode = inquiryMode;
			}
		}
	}
/**
	This is called whenever the pending WriteInquiryMode command has completed. 
**/
void CBTInquiryMgr::WriteInquiryModeComplete(TBool aSucceeded)
	{
	LOG_FUNC
	if (aSucceeded)
		{
		iInquiryMode = iPendingInquiryMode;
		return;
		}

	// should we maybe try to set another mode? Probably worth a try...
	// let's downgrade the inquiry mode
	if (iPendingInquiryMode == EInquiryWithRssiOrEir)
		{
		TRAPD(err, WriteInquiryModeL(EInquiryWithRssi));
		if (err == KErrNone)
			{
			iPendingInquiryMode = EInquiryWithRssi;
			}
		}
	}

void CBTInquiryMgr::DoInquiry()
/**
	Try to start the inquiry.
	Only starts it if there isn't already one going.
**/
	{
	LOG_FUNC

	if(RequestedInquiryIAC() == 0 || iHWState != EIdle || iHRs.IsEmpty())
		{
#ifdef _DEBUG
		LOG3(_L("Not starting inquiry. RequestedInquiryIAC == %d, iHWState == %d, iHostResolverCount == %d"), RequestedInquiryIAC(), iHWState, iNumHRs);
		if (iHRs.IsEmpty())
			{
			LOG(_L("No HRs interested in results - Stopping discovery"));
			}
#endif
		if (iHWState == EIdle)
			{
			LOG(_L("CBTInquiryMgr::DoInquiry PublishStatus Idle"));	
			PublishStatus();// make sure the status says we're idle
			}
		if (RequestedInquiryIAC() == 0)
			{
			iCurrentInquiryIAC = 0;
			}
		return;
		}

	// If pending on name lookup, we'll do it when that finishes

	// Cache stale -- update it.
	TInt err = StartHardwareInquiry();

	if (err == KErrNone)
		{
	 	SetHWState(EInquiry);
		LOG(_L("CBTInquiryMgr::DoInquiry PublishState Inquiry"));
		PublishStatus();
		// Use cache age to estimate time elapsed while inquiring.
		// Synchronise it with the inquiry
		// Stop flusher, Start inquiry wathcdog
		iFlusher->Cancel();
		TCallBack cb(InquiryWatchdog, this);
		iFlusher->Start(KInquiryWatchdogPeriod * 1000000,
						KInquiryWatchdogPeriod * 1000000,
						cb);
		SetCacheAge(iCurrentInquiryIAC, 0);
		iInquirySilenceCount = 0;
		iResultCount = 0;
		}
	else
		{
		// Couldn't start inquiry.
		// Make sure the request is completed.
		iCurrentInquiryIAC = 0;
		InquiryComplete(err, 0);
		LOG(_L("CBTInquiryMgr::DoInquiry PublishState Idle couldn't start inquiry"));
		PublishStatus();
		return;
		}
	}

void CBTInquiryMgr::ClearCurrentInquiryResults()
	{
	LOG_FUNC
	iCurrentResults.ReturnToFirstResult();
	while (CBTInqResultRef* ref = iCurrentResults.NextResult())
		{
		ref->Result().iFoundDuringCurrentInquiry = EFalse;
		}	
	}

TInt CBTInquiryMgr::CancelHardwareInquiry()
	{
	LOG_FUNC
	// CancelInquiryL will stop the current inquiry so clear any results that
	// were marked found during the inquiry
	ClearCurrentInquiryResults();

	TRAPD(err, CancelInquiryL());
	return err;
	}

TInt CBTInquiryMgr::StartHardwareInquiry()
	{
	LOG_FUNC
	iCurrentInquiryIAC = RequestedInquiryIAC();
	// attempt to free up baseband space for best performance discovery
	iLinkMgrProtocol.PhysicalLinksMgr().RequireSlotSpace(); // will *eventually* put connections in hold - we dont wait though
	TRAPD(err, StartInquiryL(iCurrentInquiryIAC, KInquiryLength, KInquiryMaxResults));

	return err;
	}




void CBTInquiryMgr::DoNameLookup(TBool aInquiryComplete)
/**
This method attempts to issue a name lookup to the HCI
**/
	{
	LOG_FUNC

	if(iHWState != EIdle || iPendingNameRequests == 0 || iHRs.IsEmpty())
		{
#ifdef _DEBUG
		LOG3(_L("Not starting name lookup. iCurrentInquiryIAC == %d, iHWState == %d, iHostResolverCount == %d"), iCurrentInquiryIAC, iHWState, iNumHRs);
#endif
		/* if hw is idle and the inquiry is ready we are finished
		 and will not get another inquiry call so publish status idle
		 note that if doing 'inquiry with names'
		 in inquiry the aInquiryComplete flag stops state oscilating
		 thostres name request for all devices will still cause this to
		 oscilate because each name request is done individually, at this layer
		 we have no idea whether the test program will ask for another name */	
		if ((iHWState == EIdle) && (aInquiryComplete))
			{
			LOG(_L("CBTInquiryMgr::DoNameLookup inquiry finished with"));
			PublishStatus();
			}			
		// else publish status will be sorted by next inquiry 
		return;
		}

	iCurrentResults.ReturnToFirstResult();
	CBTInqResultRef *refToGet = NULL;
	while(CBTInqResultRef *ref = iCurrentResults.NextResult())
		{
		CBTInqResultRecord& rec = ref->Result();
		if(rec.IsNameRequestPending())
			{
			if(RequestedInquiryIAC() == 0 || rec.NameLookupAttempts() < KMaxNameLookupAttemptsDuringInquiry)
				{
				// We want the first record for the current IAC or a record for an explicit name request.
				// Failing that, we'll just have the first record
				if (rec.HasRespondedToIAC(iCurrentInquiryIAC) || rec.IsExplicitNameRequest())
					{
					refToGet = ref;
					break;
					}
				if (!refToGet)
					{
					refToGet = ref;
					}
				}
			}
		}
	if (refToGet)
		{
		CBTInqResultRecord& rec = refToGet->Result();

		// First set the page timeout, it is a best effort attempt
		iNamePageTimeout = iLinkMgrProtocol.PhysicalLinksMgr().TryToChangePageTimeout(iNamePageTimeout);

		// Request the name lookup.
		TRAPD(err, LookupNameL(rec.LogEntry()));

		if (err == KErrNone)
			{
			SetHWState(ENameLookup);
			return;
			}
		else
			{
			// got a synchronous error - put in a blank name
			HandleRemoteNameResult(err, *refToGet, KNullDesC8());
			}
		}
	// Nothing new to do!
	iNewPageRequestsPending = EFalse;
	}

void CBTInquiryMgr::TryToInterruptInquiryForNameLookup()
	{
	LOG_FUNC
	TInt err = CancelHardwareInquiry();

	if(err == KErrNone)
		{// Start doing name lookups
		iFlusher->Cancel();	//Stop watchdog, start flusher
	 	SetHWState(EIdle);
		// don't publish status here, namelookup/inquiry will deal with it
		EnableFlusher();
		++iInquiryInteruptions;
		LOG(_L("CBTInquiryMgr::TryToInterruptInquiryForNameLookup asking for another name lookup"));
		DoNameLookup(EFalse);
		DoInquiry(); // <- In case name lookup fails
		}
	}

// An inelegant workaround called from CLinkMgrProtocol::Error to get round
//  the fact that the stack doesn't (yet) keep track of commands that it has
//  been asked to do. If they aren't completed on a reset due to a
//  KErrHardwareNotFound error then the stack hangs as it waits for the responses.
void CBTInquiryMgr::CompleteCommands(TInt aErr)
	{
	LOG_FUNC
	SetLocalNameComplete(aErr);
	GetLocalNameComplete(aErr, _L8(""));

	// Reset any inquiry results left over from a previous inquiry
	iCurrentResults.Reset();
	InquiryComplete(aErr, 0);
	}


// Check whether the controller supports Extended Inquiry Response by examing
// the supported features mask using Link Manage Protocol
TBool CBTInquiryMgr::IsExtendedInquiryResponseSupported()
	{
	LOG_FUNC
	return iLinkMgrProtocol.IsExtendedInquiryResponseSupportedLocally();
	}

/******************************************************************************/
/*	If UPDATING THE CACHE, please use one of the "AddEntry..." methods below. */
/******************************************************************************/
CBTInqResultRef* CBTInquiryMgr::FindExistingCacheEntry(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	return(iCurrentResults.FindEntry(aAddr));
	}

CBTInqResultRef* CBTInquiryMgr::AddEntryToCache(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	CBTInqResultRef* ref = FindExistingCacheEntry(aAddr);
	if(ref)
		{// Already in there!
		return ref;
		}

	CBTInqResultRecord* rec = new CBTInqResultRecord(aAddr);
	if (!rec)
		{
		return 0;
		}

	ref = iCurrentResults.Add(*rec);
	if (!ref)
		{
		delete rec;
		return 0;
		}

	//default CBTInqResultRecord values - provided in CBTInqResultRecord constructer (unlike v1)
	//maybe overwritten later

	EnableFlusher(); // Make sure we're aging entries.
	return ref;
	}

CBTInqResultRef* CBTInquiryMgr::AddEntryWithJuiceToCache(const TInquiryLogEntry& aEntry)
/**
This method should be used if you are supplying cache with all valid
baseband parameters.
Setting the valid bit in clock offset allows us to tell btman that the clock
offset value, and by inferrence the other juice values are valid -
i.e. have been supplied by the HCI, as opposed to having been supplied
as default.
**/
	{
	LOG_FUNC
	CBTInqResultRef* ref = AddEntryToCache(aEntry.iBdaddr);

	if (ref)
		{
		CBTInqResultRecord& rec = ref->Result();
		// Update all, and register that values come from HCI
		rec.iJuiceFromHCIMask |= EBluetoothJuice;
		// First fill in the base class members, common to all inquiry results
		*static_cast<TInquiryLogEntry*>(&rec.iEntry) = aEntry;
		// Now complete with extra data depending on the inquiry result type
		switch(aEntry.iSpare)
			{
		case KInqLogEntryStandard:
			break;
		case KInqLogEntryWithEir:
			{
			const TInquiryLogEntryWithEir& entryEir = static_cast<const TInquiryLogEntryWithEir &>(aEntry);
			// Only replace EIR if it contains something, since sometimes EIR transmission fails
			if(entryEir.iExtendedInquiryResponse.Length() != 0)
				{
				rec.iEntry.iExtendedInquiryResponse = entryEir.iExtendedInquiryResponse;
				}
			rec.iJuiceFromHCIMask |= EBluetoothEir;
			// Now check whether we can extract a complete or partial friendly name from the EIR and add it to the cache
			TPtrC8 name;
			TBool complete = ETrue;
			rec.Codec().Set(rec.iEntry.iExtendedInquiryResponse);
			// use EirCodec doing sanity check of the eir data
			TInt error = rec.Codec().DoSanityCheck(rec.iEntry.iExtendedInquiryResponse);
			if(error == KErrNone)
				{
				error = rec.Codec().GetDeviceName(name);
				if(error >= KErrNone)
					{
					if(error == EDataPartial)
						{
						complete = EFalse;
						}
					// Do not overwrite complete names with partial ones
					if(complete || !rec.IsNameValid() || (rec.IsNameValid() && !rec.IsNameComplete()))
						{
						rec.SetName(name);
						rec.SetNameComplete(complete);
						rec.iFlushes = 0;
						rec.SetNameValid(ETrue);
						rec.SetNameRefreshRequested(EFalse);
						}
					}
				}
			}
		// Fall through
		case KInqLogEntryWithRssi:
			{
			const TInquiryLogEntryWithRssi& entryRssi = static_cast<const TInquiryLogEntryWithRssi &>(aEntry);
			rec.iEntry.iRssi = entryRssi.iRssi;
			rec.iJuiceFromHCIMask |= EBluetoothRssi;
			break;
			}
		default:
			__ASSERT_DEBUG(EFalse, Panic(EBTInvalidInquiryLogEntry));
			break;
			}
		// Mark as valid, HCI does not (necessarily) do this for you
		rec.iEntry.iClockOffset |= KHCIClockOffsetValidBit;
		}

	return ref;
	}

CBTInqResultRef* CBTInquiryMgr::AddEntryWithCoDToCache(const TBTDevAddr& aAddr, const TUint aCoD)
	{
	LOG_FUNC
	CBTInqResultRef* ref = AddEntryToCache(aAddr);

	if (ref)
		{
		// Update all, and register that values come from HCI
		ref->Result().iJuiceFromHCIMask |= EBluetoothCoD;
		ref->Result().iEntry.iCoD = aCoD;
		}

	return ref;
	}

CBTInqResultRef* CBTInquiryMgr::AddEntryWithClockOffsetToCache(const TBTDevAddr& aAddr, const TBasebandTime aClockOffset)
	{
	LOG_FUNC
	CBTInqResultRef* ref = AddEntryToCache(aAddr);

	if (ref)
		{
		//update the clock offset - useful for subsequent connections
		// mark as valid
		ref->Result().iJuiceFromHCIMask |= EBluetoothClockOffSet;
		ref->Result().LogEntry().iClockOffset = static_cast<TUint16>(aClockOffset | KHCIClockOffsetValidBit);
		}

	return ref;
	}


TInt CBTInquiryMgr::InquiryWatchdog(TAny* aPtr)
/*
Called second while doing enquiry.
Considers whether to give up enquiry
*/
	{
	LOG_STATIC_FUNC
	CBTInquiryMgr& self = *static_cast<CBTInquiryMgr*>(aPtr);
	__ASSERT_DEBUG(self.iHWState == EInquiry, Panic(EHostResolverBadHWState));
	if(self.iResultCount == 0)
		{
		++self.iInquirySilenceCount;
		}

	if(self.iNewPageRequestsPending
	   && (self.iInquirySilenceCount * KInquiryWatchdogPeriod) > self.iInquiryInteruptions)
		{// Silence, and names are waiting. Defer the inquiry
		self.TryToInterruptInquiryForNameLookup();
		}

	self.iResultCount = 0;
	return 0;
	}

TInt CBTInquiryMgr::Flush(TAny* aPtr)
	{
	LOG_STATIC_FUNC
	CBTInquiryMgr& self = *static_cast<CBTInquiryMgr*>(aPtr);
	__ASSERT_DEBUG(self.iHWState != EInquiry, Panic(EHostResolverBadHWState));
	self.DoFlush();
	return 0;
	}

void CBTInquiryMgr::DoFlush()
	{
	LOG_FUNC
	for (TInt i = 0; i < iCacheAge.Count(); i++)
		{
		++iCacheAge[i].iCacheAge;
		}
	iCurrentResults.ReturnToFirstResult();	

	while (CBTInqResultRef* ref = iCurrentResults.NextResult())
		{
		CBTInqResultRecord& rec = ref->Result();
		TInt age = rec.IncFlushes();
		if(age >= KRecordStaleAge && rec.NumberOfIACsRespondedTo() > 0)
			{
			// device is around, but not seen for a while
			rec.ClearIACs();
			LOG(_L("CBTInquiryMgr::DoFlush - ClearIACs"));
			}
			
		if(age >= KRecordDeathAge)
			{
			rec.SetNameValid(EFalse);
			LOG(_L("CBTInquiryMgr::DoFlush - age >= KRecordDeathAge setting name valid False"));
			}
			
		if(age >= KRecordDeathAge && rec.NumberOfIACsRespondedTo() == 0 && !rec.IsNameValid() 
			&& (!rec.IsNameRequestPending() || iHRs.IsEmpty())) //Don't flush old record if name pending and HRs present
			{// Record is dead. Delete it
			LOG(_L("CBTInquiryMgr::DoFlush - Deleting reference"));
			delete ref;
			}
		else if(rec.IsNameRefreshRequested() && iHWState == EIdle)
			{// Try to get this name now, perhaps?
			LOG(_L("CBTInquiryMgr::DoFlush - Try to get name straight away"));
			rec.SetNameRefreshRequested(EFalse);
			rec.SetNamePending(ETrue);
			rec.iNameLookupAttempts = 0;
			++iPendingNameRequests;
			LOG(_L("CBTInquiryMgr::DoFlush asking for another name lookup"));
			DoNameLookup(EFalse);
			}
		}
	if (iCurrentResults.IsEmpty())
		{
		iNewPageRequestsPending = EFalse;
		iPendingNameRequests = 0;
		iFlusher->Cancel();
		}
	}

void CBTInquiryMgr::EnableFlusher()
	{
	LOG_FUNC
	if (iFlusher->IsActive() || iHWState == EInquiry)
		return;
	static const TInt usec2sec = 1000000;
	TCallBack cb(Flush, this);
	iFlusher->Start(usec2sec / 4, // 1/4 sec for initial flush
					KFlushTimeoutSecs * usec2sec,
					cb);
	}

TInt CBTInquiryMgr::CacheAge(TUint aIAC) const
	{
	TInt ret = KMaxTInt; // If we haven't set a cache age for this IAC, return KMaxTInt
	for (TUint i = 0; i < iCacheAge.Count(); i++)
		{
		if (iCacheAge[i].iIAC == aIAC)
			{
			ret = iCacheAge[i].iCacheAge;
			break;
			}
		}
	return ret;
	}

void CBTInquiryMgr::SetCacheAge(TUint aIAC, TInt aAge)
	{
	TBool found = EFalse;
	for (TUint i = 0; i < iCacheAge.Count(); i++)
		{
		if (iCacheAge[i].iIAC == aIAC)
			{
			iCacheAge[i].iCacheAge = aAge;
			found = ETrue;
			break;
			}
		}
	if (!found)
		{
		TInquiryCacheAge ageInfo;
		ageInfo.iIAC = aIAC;
		ageInfo.iCacheAge = aAge;
		(void)iCacheAge.Append(ageInfo);
		// If we can't append, there's not a lot we can do - we'll just have
		// to return KMaxTInt when someone asks for the age
		}
	}


// -----------------------------------------------------------------------------------------

void CBTInquiryMgr::StartInquiryL(TUint aIAC, TUint8 aLength, TUint8 aNumResponses)
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CInquiryCommand* cmd = CInquiryCommand::NewL(aIAC, aLength, aNumResponses);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CBTInquiryMgr::CancelInquiryL()
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CInquiryCancelCommand* cmd = CInquiryCancelCommand::NewL();
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CBTInquiryMgr::CancelRemoteNameL(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	CRemoteNameRequestCancelCommand* cmd = CRemoteNameRequestCancelCommand::NewL(aAddr);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CBTInquiryMgr::WriteInquiryModeL(TUint8 aInquiryMode)
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CWriteInquiryModeCommand* cmd = CWriteInquiryModeCommand::NewL(aInquiryMode);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

/**
This function should be used by the inquiry manager to retrieve remote names.
*/
void CBTInquiryMgr::LookupNameL(const TInquiryLogEntry& aEntry)
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CRemoteNameRequestCommand* cmd = CRemoteNameRequestCommand::NewL(aEntry.iBdaddr, aEntry.iPageScanRepetitionMode, aEntry.iPageScanMode, aEntry.iClockOffset);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

/**
This function should only be used when there is an established physical link (and therefore from
CPhysicalLink only) to retrieve a remote name.
*/
void CBTInquiryMgr::ReadRemoteNameL(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CRemoteNameRequestCommand* cmd = CRemoteNameRequestCommand::NewL(aAddr, 0, 0, 0);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CBTInquiryMgr::ReadLocalNameL()
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CReadLocalNameCommand* cmd = CReadLocalNameCommand::NewL();
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CBTInquiryMgr::MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aCommand, Panic(EBTInquiryMgrUnmatchedEvent));
	LOG2(_L("MhcqcCommandErrored: error code:%d opcode:0x%x"), aErrorCode, aCommand->Opcode());
	
	if (aCommand->Opcode() == KInquiryOpcode)
		{
		// Clear any pending stuff
		CompleteCommands(aErrorCode);
		// this will also set the state to Idle
		}
	else if (aCommand->Opcode() == KWriteInquiryModeOpcode)
		{
		// command had error before response -- assume not supported
		WriteInquiryModeComplete(EFalse);
		}
	else if (aCommand->Opcode() == KRemoteNameRequestOpcode)
		{
		// command had error before response -- doctor a fake response and process it to restore sanity
		TBuf8<256> fakeEventBuffer;
		const TBTDevAddr remNameReqAddr = static_cast<const CRemoteNameRequestCommand*>(aCommand)->BDADDR();
		TRemoteNameReqCompleteEvent fakeRemoteNameRequestCompleteEvent(EOK, remNameReqAddr, _L8(""), fakeEventBuffer);
		RemoteNameReqCompleteEvent(fakeRemoteNameRequestCompleteEvent);
		// we will not change the state so a running inquiry can complete
		}
	else if (aCommand->Opcode() == KReadLocalNameOpcode)
		{
		// command had error before response -- doctor a fake response and process it to restore sanity
		TBuf8<256> fakeEventBuffer;
		TReadLocalNameCompleteEvent fakeLocalNameCompleteEvent(EUnspecifiedError, 0, _L8(""), fakeEventBuffer);
		ReadLocalNameOpcode(EUnspecifiedError, fakeLocalNameCompleteEvent);
		// this will also set the state to Idle
		}
	
	}

// From MHCICommandQueueClient
void CBTInquiryMgr::MhcqcCommandEventReceived(const THCIEventBase& aEvent,
											const CHCICommandBase* aRelatedCommand)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aRelatedCommand, Panic(EBTInquiryMgrUnmatchedEvent));
	static_cast<void>(aRelatedCommand);
	switch(aEvent.EventCode())
		{
	case ERemoteNameReqCompleteEvent:
		RemoteNameReqCompleteEvent(aEvent);
		break;

	case ERemoteHostSupportedFeaturesNotificationEvent:
		RemoteHostSupportedFeaturesNotificationEvent(aEvent);
		break;

	case EInquiryCompleteEvent:
		InquiryCompleteEvent(aEvent);
		break;

	case EInquiryResultEvent:
		InquiryResultEvent(aEvent);
		break;

	case EInquiryResultwithRSSIEvent:
		InquiryResultWithRSSIEvent(aEvent);
		break;

	case EExtendedInquiryResultEvent:
		ExtendedInquiryResultEvent(aEvent);
		break;

	case ECommandCompleteEvent:
		CommandCompleteEvent(aEvent);
		break;

	case ECommandStatusEvent:
		CommandStatusEvent(aEvent, *aRelatedCommand);
		break;

	default:
		LOG1(_L("Warning!! Unknown Command Event Received (event code:%d)"), aEvent.EventCode());
		__ASSERT_DEBUG(EFalse, Panic(EHCIUnknownCommandEvent));
		break;
		}
	}

void CBTInquiryMgr::CommandCompleteEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const THCICommandCompleteEvent& completeEvent = THCICommandCompleteEvent::Cast(aEvent);
	THCIOpcode opcode = completeEvent.CommandOpcode();
	THCIErrorCode hciErr = aEvent.ErrorCode();

	switch (opcode)
		{
	case KWriteInquiryModeOpcode:
		WriteInquiryModeOpcode(hciErr, aEvent);
		break;

	case KReadLocalNameOpcode:
		ReadLocalNameOpcode(hciErr, aEvent);
		break;

	case KInquiryCancelOpcode:
		InquiryCancelOpcode(hciErr, aEvent);
		break;

	// These commands are expected to not be handled and so can be safely ignored.
	case KRemoteNameRequestCancelOpcode:
		LOG1(_L("ignored Command Complete event (opcode: 0x%04x)"), opcode);
		break;

	// The commands below would most likely be used by the inquiry manager, however
	// they currently are not used.
	case KPeriodicInquiryModeOpcode:
	case KExitPeriodicInquiryModeOpcode:
	case KReadInquiryScanActivityOpcode:
	case KWriteInquiryScanActivityOpcode:
	case KReadInquiryScanTypeOpcode:
	case KWriteInquiryScanTypeOpcode:
	case KReadInquiryModeOpcode:
		LOG1(_L("Warning!! Unhandled Command Complete Event (opcode:%d)"), opcode);
		break;

	default:
		LOG2(_L("Error!! Unknown Command complete event! Opcode %d error code %d"), opcode, hciErr);
		__ASSERT_DEBUG(EFalse, Panic(EHCIUnknownCommandCompleteOpcode));
		break;
		}
	}

void CBTInquiryMgr::CommandStatusEvent(const THCIEventBase& aEvent, const CHCICommandBase& aCommand)
	{
	LOG_FUNC

	const TCommandStatusEvent& commandStatusEvent = TCommandStatusEvent::Cast(aEvent);
	THCIOpcode opcode = commandStatusEvent.CommandOpcode();
	THCIErrorCode hciErr = commandStatusEvent.ErrorCode();

	__ASSERT_DEBUG(aCommand.Opcode() == opcode, Panic(EBTInquiryMgrMismatchedStatusEvent));

	if (hciErr != EOK)
		{
		// got an error
		// map onto the event that would have occurred: some things we will have to let the client work out
		// e.g. they should check error and connection handle etc.
		switch (opcode)
			{
		case KInquiryOpcode:
			InquiryComplete(CHciUtil::SymbianErrorCode(hciErr), 0);
			break;

		case KRemoteNameRequestOpcode:
				{
				const CRemoteNameRequestCommand& remNameReq = static_cast<const CRemoteNameRequestCommand&>(aCommand);
				TInt err = CHciUtil::SymbianErrorCode(hciErr);
				TBTDevAddr addr = remNameReq.BDADDR();
				TBTDeviceName8 nullDevName(KNullDesC8);

				RemoteNameResult(err, addr, nullDevName);
				iLinkMgrProtocol.PhysicalLinksMgr().RemoteName(hciErr, addr, nullDevName);
				}
			break;

		default:
			// Complete any other commands with an error
			CommandCompleteEvent(aEvent);
			break;
			}
		}
	}

void CBTInquiryMgr::WriteInquiryModeOpcode(THCIErrorCode aHciErr, const THCIEventBase& /*aEvent*/)
	{
	LOG_FUNC
	WriteInquiryModeComplete(aHciErr == EOK);
	}

void CBTInquiryMgr::ReadLocalNameOpcode(THCIErrorCode aHciErr, const THCIEventBase& aEvent)
	{
	LOG_FUNC
	if (aHciErr == EOK)
		{
		const TReadLocalNameCompleteEvent& readLocalNameCompleteEvent = TReadLocalNameCompleteEvent::Cast(aEvent);
		TPtrC8 localName = readLocalNameCompleteEvent.LocalName();
		
		GetLocalNameComplete(CHciUtil::SymbianErrorCode(aHciErr), localName);
		iLinkMgrProtocol.UpdateLocalDeviceName(localName);
		}
	else
		{
		_LIT8(KNoName, "");
		GetLocalNameComplete(CHciUtil::SymbianErrorCode(aHciErr), KNoName());
		}
	}

void CBTInquiryMgr::InquiryCancelOpcode(THCIErrorCode aHciErr, const THCIEventBase& /* aEvent */)
	{
	LOG_FUNC
	if (aHciErr == EOK)
		{
		if (iHWState == ECancellingForNewIAC)
			{
			SetHWState(EIdle);
			DoInquiry();
			}
		}
	// If an error comes in, it may be because the inquiry complete came in first. In that case, everything
	// will be handled by the Inquiry complete logic
	}

// ----------------------------------------------------------------------------
// Event processing functions
// ----------------------------------------------------------------------------

void CBTInquiryMgr::InquiryCompleteEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	InquiryComplete(CHciUtil::SymbianErrorCode(aEvent.ErrorCode()), 0);
	}

void CBTInquiryMgr::InquiryResultEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	
	const TInquiryResultEvent& inquiryResultEvent = TInquiryResultEvent::Cast(aEvent);
	TInquiryLogEntry entry;
	TUint responses = inquiryResultEvent.NumResponses();
	
	for (TUint index = 0; index < responses; index++)
		{
		entry.iSpare = KInqLogEntryStandard;
		entry.iPageScanRepetitionMode = inquiryResultEvent.PageScanRepetitionMode(index);
		entry.iPageScanPeriodMode = inquiryResultEvent.Reserved1(index); // v1.1 spec, no meaning in v1.2
		entry.iPageScanMode = inquiryResultEvent.Reserved2(index); // v1.1 spec, no meaning in v1.2
		entry.iClockOffset = inquiryResultEvent.ClockOffset(index);
		entry.iBdaddr = inquiryResultEvent.BDADDR(index);
		entry.iCoD = inquiryResultEvent.ClassOfDevice(index);
		InquiryResult(CHciUtil::SymbianErrorCode(aEvent.ErrorCode()), entry);
		}
	}

void CBTInquiryMgr::InquiryResultWithRSSIEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	
	const TInquiryResultwithRSSIEvent& inquiryResultwithRSSIEvent = TInquiryResultwithRSSIEvent::Cast(aEvent);
	TInquiryLogEntryWithRssi entry;
	TUint responses = inquiryResultwithRSSIEvent.NumResponses();
	
	for (TUint index = 0; index < responses; index++)
		{
		entry.iSpare = KInqLogEntryWithRssi;
		entry.iPageScanRepetitionMode = inquiryResultwithRSSIEvent.PageScanRepetitionMode(index);
		entry.iPageScanPeriodMode = 0x00;
		entry.iPageScanMode = 0x00;
		entry.iClockOffset = inquiryResultwithRSSIEvent.ClockOffset(index);
		entry.iBdaddr = inquiryResultwithRSSIEvent.BDADDR(index);
		entry.iCoD = inquiryResultwithRSSIEvent.ClassOfDevice(index);
		entry.iRssi = inquiryResultwithRSSIEvent.RSSI(index);
		InquiryResult(CHciUtil::SymbianErrorCode(aEvent.ErrorCode()), entry);
		}
	}

void CBTInquiryMgr::ExtendedInquiryResultEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	
	const TExtendedInquiryResultEvent& extendedInquiryResultEvent = TExtendedInquiryResultEvent::Cast(aEvent);
	TInquiryLogEntryWithEir entry;
	
	entry.iSpare = KInqLogEntryWithEir;
	entry.iPageScanRepetitionMode = extendedInquiryResultEvent.PageScanRepetitionMode();
	entry.iPageScanPeriodMode = 0x00;
	entry.iPageScanMode = 0x00;
	entry.iClockOffset = extendedInquiryResultEvent.ClockOffset();
	entry.iBdaddr = extendedInquiryResultEvent.BDADDR();
	entry.iCoD = extendedInquiryResultEvent.ClassOfDevice();
	entry.iRssi = extendedInquiryResultEvent.RSSI();
	entry.iExtendedInquiryResponse = extendedInquiryResultEvent.ExtendedInquiryResponse();
	InquiryResult(CHciUtil::SymbianErrorCode(aEvent.ErrorCode()), entry);
	}

void CBTInquiryMgr::RemoteNameReqCompleteEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TRemoteNameReqCompleteEvent& remNameReqCompleteEvent = TRemoteNameReqCompleteEvent::Cast(aEvent);
	THCIErrorCode err = aEvent.ErrorCode();
	const TBTDevAddr addr = remNameReqCompleteEvent.BDADDR();
	TPtrC8 buf = remNameReqCompleteEvent.RemoteName();
	
	//shorten the name to the proper length so that we don't waste memory
	TInt nullTerminator = buf.Locate(0);
	if (nullTerminator == KErrNotFound)
		{
		nullTerminator = buf.Length();
		}
	TPtrC8 remoteName = buf.Left(nullTerminator);
	
	// name requests have two customers...
	// inquiry manager
	RemoteNameResult(CHciUtil::SymbianErrorCode(err), addr, remoteName);
	// phy manager
	iLinkMgrProtocol.PhysicalLinksMgr().RemoteName(err, addr, remoteName);
	}

void CBTInquiryMgr::RemoteHostSupportedFeaturesNotificationEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TRemoteHostSupportedFeaturesNotificationEvent& remHostSupFeatEvent = TRemoteHostSupportedFeaturesNotificationEvent::Cast(aEvent);
	THCIErrorCode err = remHostSupFeatEvent.ErrorCode();
	TBTDevAddr addr = remHostSupFeatEvent.BDADDR();
	TUint64 hostSupFeats = remHostSupFeatEvent.HostSupportedFeatures();

	RemoteHostSupportedFeatures(CHciUtil::SymbianErrorCode(err), addr, hostSupFeats);
	}


#ifdef CONNECTION_PREEMPTS_INQUIRY
// *******************************************************************
// ACL Connecting status subscriber
// *******************************************************************
/*static*/ CConnectingStatusSubscriber* CConnectingStatusSubscriber::NewL(CBTInquiryMgr& aInquiryMgr)
	{
	LOG_STATIC_FUNC
	CConnectingStatusSubscriber* self = new(ELeave) CConnectingStatusSubscriber(aInquiryMgr);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CConnectingStatusSubscriber::CConnectingStatusSubscriber(CBTInquiryMgr& aInquiryMgr)
: CActive(CActive::EPriorityStandard), iParent(aInquiryMgr)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

CConnectingStatusSubscriber::~CConnectingStatusSubscriber()
	{
	LOG_FUNC
	Cancel();
	iProperty.Close();
	}

void CConnectingStatusSubscriber::DoCancel()
	{
	LOG_FUNC
	iProperty.Cancel();
	}

void CConnectingStatusSubscriber::ConstructL()
	{
	LOG_FUNC
	User::LeaveIfError(iProperty.Attach(KPropertyUidBluetoothCategory,
										KPropertyKeyBluetoothGetConnectingStatus));
	Subscribe();
	}

void CConnectingStatusSubscriber::Subscribe()
	{
	LOG_FUNC
	iProperty.Subscribe(iStatus);
	SetActive();
	}

/**
This is looking for the connection to complete.  The inquiry must be
synchronously cancelled before the CreateConnection command is issued,
but we can resume it when we get round to it.
*/
void CConnectingStatusSubscriber::RunL()
	{
	LOG_FUNC
	// Subscribe to the next change of state.
	Subscribe();

	TInt isConnecting;
	// If this device is currently connecting an ACL suspend the
	// inquiry.
	iProperty.Get(isConnecting);
	
	if(!isConnecting)
		{
		iParent.Resume();
		}
	}
#endif	// CONNECTION_PREEMPTS_INQUIRY
