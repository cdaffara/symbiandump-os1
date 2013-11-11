// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <e32base.h>
#include <remcon/remconextapi1.h>
#include <remconabsvolcontroller.h>
#include <remconabsvolcontrollerobserver.h>
#include <remconinterfaceselector.h>
#include "absvolutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_EXTAPI1);
#endif

// Used to pad over the results field in the operation-specific data.
_LIT8(KResultsPad, "    ");

EXPORT_C CRemConAbsVolController* CRemConAbsVolController::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
																	MRemConAbsVolControllerObserver& aObserver)
	{
	LOG_STATIC_FUNC

	CRemConAbsVolController* self = new(ELeave) CRemConAbsVolController(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRemConAbsVolController::CRemConAbsVolController(CRemConInterfaceSelector& aInterfaceSelector, 
													 MRemConAbsVolControllerObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConAbsVolApiUid), 
						 KMaxOperationDataSize,
						 aInterfaceSelector,
						 ERemConClientTypeController),
	iObserver(aObserver)
	{
	}

EXPORT_C CRemConAbsVolController::~CRemConAbsVolController()
	{
	LOG_FUNC
	}

TAny* CRemConAbsVolController::GetInterfaceIf(TUid aUid)
	{
	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KRemConInterfaceIf1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConInterfaceIf*>(this)
			);
		}

	return ret;
	}

void CRemConAbsVolController::MrcibNewMessage(TUint aOperationId, const TDesC8& aData)
	{
	LOG_FUNC
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	LOG1(_L("\taData.Length = %d"), aData.Length());

	// Get the response error out of aData.
	if ( aData.Length() < KRemConExtApi1MinimumDataLength )
		{
		return; // ditch malformed messages
		}
	TInt err = static_cast<TInt>(aData.Ptr()[0]);

	switch ( aOperationId )
		{
	case ERemConGetAbsoluteVolume:
		{
		// 20 is the length of the abs volume data- 10 bytes for each of two 
		// '0x%08x'-formatted fields.
		if ( aData.Length() < KRemConExtApi1MinimumDataLength + 20 )
			{
			//Silently drop the message
			LOG(_L("Warning: Message is dropped due to invalid length!"));
			__DEBUGGER();
			return;
			}
		TUint absVol;
		TUint maxVol;
		if ( GetAbsMaxVol(aData.Mid(KRemConExtApi1MinimumDataLength, 20), absVol, maxVol) == KErrNone )
			{
			iObserver.MrcavcoGetAbsoluteVolumeResponse(absVol, maxVol, err);
			}
		}
		break;

	case ERemConSetAbsoluteVolume:
		iObserver.MrcavcoSetAbsoluteVolumeResponse(err);
		break;

	default:
		break;
		}
	}

EXPORT_C void CRemConAbsVolController::GetAbsoluteVolume(TRequestStatus& aStatus, 
												   TUint& aNumRemotes)
	{
	LOG_FUNC

	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConAbsVolApiUid), 
		(TUint)ERemConGetAbsoluteVolume,
		aNumRemotes,
		ERemConCommand,
		KNullDesC8());
	}

EXPORT_C void CRemConAbsVolController::SetAbsoluteVolume(TRequestStatus& aStatus, 
												   TUint aAbsVol, 
												   TUint aMaxVol,
												   TUint& aNumRemotes)
	{
	LOG_FUNC

	iOutData.Copy(KResultsPad());
	iOutData.AppendFormat(_L8("0x%08x0x%08x"), aAbsVol, aMaxVol);

	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConAbsVolApiUid), 
		(TUint)ERemConSetAbsoluteVolume,
		aNumRemotes,
		ERemConCommand,
		iOutData);
	}
