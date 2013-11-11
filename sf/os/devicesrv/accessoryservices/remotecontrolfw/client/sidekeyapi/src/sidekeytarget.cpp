// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <remcon/remconsidekeyapi.h>
#include <remcon/remconsidekeytarget.h>
#include <remcon/remconsidekeytargetobserver.h>
#include <remconinterfaceselector.h>
#include "remconsidekeyapipaniccodes.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, "RemconSideKeyApi");
#endif

#ifdef _DEBUG
// only called via __ASSERT_DEBUG statements
static void Panic(TSideKeyApiPanicCat aCode)
	{
	User::Panic(KSideKeyPanicCat, aCode);
	}
#endif

EXPORT_C CRemConSideKeyTarget* CRemConSideKeyTarget::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
																	MRemConSideKeyTargetObserver& aObserver)
	{
	LOG_STATIC_FUNC

	CRemConSideKeyTarget* self = new(ELeave) CRemConSideKeyTarget(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRemConSideKeyTarget::CRemConSideKeyTarget(CRemConInterfaceSelector& aInterfaceSelector, 
													 MRemConSideKeyTargetObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConSideKeyApiUid), 
						 KMaxOperationDataSize,
						 aInterfaceSelector,
						 ERemConClientTypeTarget),
	iObserver(aObserver)
	{
	}

EXPORT_C CRemConSideKeyTarget::~CRemConSideKeyTarget()
	{
	LOG_FUNC
	}

TAny* CRemConSideKeyTarget::GetInterfaceIf(TUid aUid)
	{
	LOG_FUNC
	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KRemConInterfaceIf1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConInterfaceIf*>(this)
			);
		}

	return ret;
	}

void CRemConSideKeyTarget::MrcibNewMessage(TUint aOperationId, const TDesC8& aData)
	{
	LOG_FUNC
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	LOG1(_L("\taData.Length = %d"), aData.Length());

	// Get the button action
	if ( aData.Length() < KRemConSideKeyApiButtonDataLength )
		{
		// The bearer sent us incorrect data. We cannot panic the bearer
		// from here, so we panic ourselves in udeb mode only instead.
		__ASSERT_DEBUG(EFalse,Panic(ESideKeyMissingButtonData));
		LOG(_L("\tdropping command because button data not found"));
		return; // ditch malformed messages
		}
	TRemConSideKeyApiButtonAction action = static_cast<TRemConSideKeyApiButtonAction>(aData.Ptr()[0]);

	switch ( aOperationId )
		{
	case ERemConSideKeyUp:
		iObserver.MrcsktoSideKeyUp(action);
		break;

	case ERemConSideKeyDown:
		iObserver.MrcsktoSideKeyDown(action);
		break;

	default:
		// The bearer sent us incorrect data. We cannot panic the bearer
		// from here, so we panic ourselves in udeb mode only instead.
		__ASSERT_DEBUG(EFalse,Panic(ESideKeyInvalidOperationId));
		break;
		}
	}

EXPORT_C void CRemConSideKeyTarget::SideKeyUpResponse(TInt aError)
	{
	LOG_FUNC

	iOutData.Copy((TUint8*)&aError, KRemConSideKeyApiResultDataLength);
	
	InterfaceSelector().SendUnreliable( 
		TUid::Uid(KRemConSideKeyApiUid), 
		(TUint)ERemConSideKeyUp, 
		ERemConResponse,
		iOutData);
	}

EXPORT_C void CRemConSideKeyTarget::SideKeyDownResponse(TInt aError)
	{
	LOG_FUNC

	iOutData.Copy((TUint8*)&aError, KRemConSideKeyApiResultDataLength);
	
	InterfaceSelector().SendUnreliable(
		TUid::Uid(KRemConSideKeyApiUid), 
		(TUint)ERemConSideKeyDown, 
		ERemConResponse,
		iOutData);
	}
