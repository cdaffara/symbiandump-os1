// PropertyAccess.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/common.mmh>
#include <e32cmn.h>
#include <fshell/extrabtrace.h>

#include <kernel.h>
#include <kern_priv.h>
#include "memoryaccess.h"
#include "PropertyAccess.h"

DPropertyAccess::DPropertyAccess(DThread* aClient)
	: iClient(aClient)
	{
	}

TInt DPropertyAccess::HandleProperty(TInt aFunction, TAny* aParams, TAny* aVal)
	{
	return DoHandleProperty(iClient, aFunction, aParams, aVal);
	}

TInt DPropertyAccess::DoHandleProperty(DThread* aClient, TInt aFunction, TAny* aParams, TAny* aVal)
	{
	if (aFunction == RMemoryAccess::EControlPropertyNotify)
		{
		if (iNotifyClientStatus) return KErrAlreadyExists;
		iNotifyClientStatus = (TRequestStatus*)aVal;
		iClientNotifyPtr = aParams;
		return KErrNone;
		}
	else if (aFunction == RMemoryAccess::EControlPropertyNotifyCancel)
		{
		CancelNotifys();
		return KErrNone;
		}

	TInt err;
	TProp clientParams;
	err = Kern::ThreadRawRead(aClient, aParams, &clientParams, sizeof(TProp));
	if (err)
		{
		return err;
		}
	//Kern::Printf("HandleProperty %d, iDefine=%d iCat=0x%x iKey=%d", aFunction, clientParams.iDefine, clientParams.iCategory, clientParams.iKey);

	NKern::ThreadEnterCS();
	RPropertyRef property;
	err = property.Open(clientParams.iCategory, clientParams.iKey);
	//Kern::Printf("HandleProperty Open ret=%d", err);
	if (!err)
		{
		// Check if it exists but isn't defined (and if it isn't, treat it the same as if it didn't exist)
		TPropertyStatus stat;
		NKern::LockSystem();
		TBool defined = property.GetStatus(stat);
		NKern::UnlockSystem();
		if (!defined)
			{
			property.Close();
			err = KErrNotFound;
			}
		}

	if (err == KErrNotFound && clientParams.iDefine && (aFunction == RMemoryAccess::EControlSetPropertyDesc || aFunction == RMemoryAccess::EControlSetPropertyInt))
		{
		err = property.Attach(clientParams.iCategory, clientParams.iKey);
		TInt attr = (aFunction == RMemoryAccess::EControlSetPropertyInt) ? RProperty::EInt : RProperty::EByteArray;
		_LIT_SECURITY_POLICY_PASS(KPass);
		if (!err) err = property.Define(attr, KPass, KPass);
		}
	NKern::ThreadLeaveCS();
	if (err)
		{
		return err;
		}

	switch (aFunction)
		{
		case RMemoryAccess::EControlGetPropertyInt:
			{
			TInt val;
			err = property.Get(val);
			if (!err)
				{
				TPckg<TInt> valBuf(val);
				err = Kern::ThreadDesWrite(aClient, aVal, valBuf, 0);
				}
			break;
			}
		case RMemoryAccess::EControlGetPropertyDesc:
			{
			err = Kern::ThreadGetDesMaxLength(aClient, aVal);
			TInt size = err;
			if (size < 0) break;
			
			NKern::ThreadEnterCS();
			HBuf8* buf = HBuf8::New(size);
			if (!buf)
				{
				err = KErrNoMemory;
				NKern::ThreadLeaveCS();
				break;
				}
			err = property.Get(*buf);
			if (!err || err == KErrOverflow)
				{
				TInt writeerr = Kern::ThreadDesWrite(aClient, aVal, *buf, 0, NULL);
				if (writeerr) err = writeerr;
				}
			delete buf;
			NKern::ThreadLeaveCS();
			if (err == KErrOverflow)
				{
				NKern::LockSystem();
				TPropertyStatus status;
				property.GetStatus(status);
				NKern::UnlockSystem();

				clientParams.iActualSize = status.iSize;
				err = Kern::ThreadRawWrite(aClient, aParams, (TAny*)&clientParams, sizeof(TProp));
				}
			break;
			}
		case RMemoryAccess::EControlSetPropertyInt:
			{
			err = property.Set((TInt)aVal);
			break;
			}
		case RMemoryAccess::EControlSetPropertyDesc:
			{
			err = Kern::ThreadGetDesLength(aClient, aVal);
			TInt size = err;
			if (size < 0) break;
			
			NKern::ThreadEnterCS();
			HBuf8* buf = HBuf8::New(size);
			if (!buf)
				{
				err = KErrNoMemory;
				NKern::ThreadLeaveCS();
				break;
				}
			buf->SetLength(size);
			err = Kern::ThreadDesRead(aClient, aVal, *buf, 0);
			if (!err)
				{
				err = property.Set(*buf);
				}
			delete buf;
			NKern::ThreadLeaveCS();
			break;
			}
		case RMemoryAccess::EControlDeleteProperty:
			NKern::ThreadEnterCS();
			err = property.Delete();
			NKern::ThreadLeaveCS();
			break;
		case RMemoryAccess::EControlSubscribeToProperty:
			{
			TBool useBtrace = (TBool)aVal;
			TNotifyRequest* req = new TNotifyRequest(&PropertyChanged, clientParams.iCategory, clientParams.iKey, this, useBtrace);
			err = req->iProperty.Attach(clientParams.iCategory, clientParams.iKey); // Allow non-existant properties
			if (!err)
				{
				err = req->iProperty.Subscribe(req->iRequest);
				if (err)
					{
					// Don't think this will ever happen
					req->iProperty.Close();
					req->iRequest.iPtr = NULL;
					}
				}
			if (err)
				{
				delete req;
				}
			else
				{
				iRequests.Add(&req->iLink);
				}
			}
			break;			
		default:
			break;
		}

	NKern::ThreadEnterCS();
	property.Close();
	NKern::ThreadLeaveCS();
	return err;
	}

DPropertyAccess::~DPropertyAccess()
	{
	CancelNotifys();
	}

void DPropertyAccess::CancelNotifys()
	{
	for (SDblQueLink* link = iRequests.First(); link != NULL && link != &iRequests.iA;)
		{
		TNotifyRequest* request = _LOFF(link, TNotifyRequest, iLink);
		link=link->iNext; // Do this before anything else because we'll be deleting the TNotifyRequest
		request->iProperty.Cancel(request->iRequest);
		request->iProperty.Close();
		delete request;
		}

	if (iNotifyClientStatus)
		{
		Kern::RequestComplete(iClient, iNotifyClientStatus, KErrCancel);
		}
	iClientNotifyPtr = NULL;
	}

void DPropertyAccess::PropertyChanged(TAny* aPtr, TInt aReason)
	{
	TNotifyRequest* req = static_cast<TNotifyRequest*>(aPtr);
	req->iPropertyAccess->DoPropertyChanged(*req, aReason);
	}

void DPropertyAccess::DoPropertyChanged(TNotifyRequest& aRequest, TInt aReason)
	{
	// First, rerequest
	if (aReason == KErrNone)
		{
		aRequest.iProperty.Subscribe(aRequest.iRequest);
		}
	// Then try and notify
	if (iNotifyClientStatus && !aRequest.iBtrace)
		{
		TPropNotifyResult result;
		result.iCategory = aRequest.iCat.iUid;
		result.iKey = aRequest.iKey;
		result.iMissedChanges = iMissedChanges;
		result.iError = aReason;
		iMissedChanges = 0; // No need to lock/safeswap because we run in the context of the pubsub DFC
		TInt err = Kern::ThreadRawWrite(iClient, iClientNotifyPtr, &result, sizeof(TPropNotifyResult));
		Kern::RequestComplete(iClient, iNotifyClientStatus, err);
		}
	else if (aRequest.iBtrace)
		{
#ifdef FSHELL_TRACE_SUPPORT
		TUint cat = aRequest.iCat.iUid;
		TUint key = aRequest.iKey;

		TInt intVal;
		TInt err = aRequest.iProperty.Get(intVal);
		if (err == KErrNone)
			{
			BTrace12(ExtraBTrace::EPubSub, ExtraBTrace::EPubSubIntPropertyChanged, cat, key, intVal);
			}
		else if (err == KErrArgument)
			{
			TBuf8<KMaxBTraceDataArray> buf; // No point using larger buffer, as this is the max we can output over btrace (80 bytes)
			err = aRequest.iProperty.Get(buf);
			if (err == KErrNone || err == KErrOverflow)
				{
				BTraceN(ExtraBTrace::EPubSub, ExtraBTrace::EPubSubDataPropertyChanged, cat, key, buf.Ptr(), buf.Length());
				}
			}
#endif
		}
	else
		{
		iMissedChanges++;
		}
	}
