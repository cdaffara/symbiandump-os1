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
#include <remconabsvoltarget.h>
#include <remconabsvoltargetobserver.h>
#include <remconinterfaceselector.h>
#include "absvolutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_EXTAPI1);
#endif

EXPORT_C CRemConAbsVolTarget* CRemConAbsVolTarget::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
																	MRemConAbsVolTargetObserver& aObserver)
	{
	LOG_STATIC_FUNC

	CRemConAbsVolTarget* self = new(ELeave) CRemConAbsVolTarget(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRemConAbsVolTarget::CRemConAbsVolTarget(CRemConInterfaceSelector& aInterfaceSelector, 
													 MRemConAbsVolTargetObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConAbsVolApiUid), 
						 KMaxOperationDataSize,
						 aInterfaceSelector,
						 ERemConClientTypeTarget),
	iObserver(aObserver)
	{
	}

EXPORT_C CRemConAbsVolTarget::~CRemConAbsVolTarget()
	{
	LOG_FUNC
	}

TAny* CRemConAbsVolTarget::GetInterfaceIf(TUid aUid)
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

void CRemConAbsVolTarget::MrcibNewMessage(TUint aOperationId, const TDesC8& aData)
	{
	LOG_FUNC
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	LOG1(_L("\taData.Length = %d"), aData.Length());

	switch ( aOperationId )
		{
	case ERemConGetAbsoluteVolume:
		iObserver.MrcavtoGetAbsoluteVolume();
		break;

	case ERemConSetAbsoluteVolume:
		{
		TUint absVol;
		TUint maxVol;
		// 20 is the length of the abs vol data.
		if(aData.Length()<KRemConExtApi1MinimumDataLength+20)
			{
			// Silently drop the message
			LOG(_L("Warning: Message is dropped due to invalid length!"));
			__DEBUGGER();
			}
		else if ( GetAbsMaxVol(aData.Mid(KRemConExtApi1MinimumDataLength, 20), absVol, maxVol) == KErrNone )
			{
			iObserver.MrcavtoSetAbsoluteVolume(absVol, maxVol);
			}
		break;
		}

	default:
		break;
		}
	}

EXPORT_C void CRemConAbsVolTarget::GetAbsoluteVolumeResponse(TRequestStatus& aStatus, TUint aAbsVol, TUint aMaxVol, TInt aError)
	{
	LOG_FUNC

	iOutData.Copy((TUint8*)&aError, KRemConExtApi1ResultDataLength);
	iOutData.AppendFormat(_L8("0x%08x0x%08x"), aAbsVol, aMaxVol);
	
	// We pass iNumRemotes even though we're not interested in its value but 
	// RemCon will write to this location so we need it to be somewhere safe.
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConAbsVolApiUid), 
		(TUint)ERemConGetAbsoluteVolume, 
		iNumRemotes,
		ERemConResponse,
		iOutData);
	}

EXPORT_C void CRemConAbsVolTarget::SetAbsoluteVolumeResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC

	iOutData.Copy((TUint8*)&aError, KRemConExtApi1ResultDataLength);
	
	// We pass iNumRemotes even though we're not interested in its value but 
	// RemCon will write to this location so we need it to be somewhere safe.
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConAbsVolApiUid), 
		(TUint)ERemConSetAbsoluteVolume, 
		iNumRemotes,
		ERemConResponse,
		KNullDesC8());
	}
