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

#include <remconcoreapi.h>
#include "remconserialbeareruid.h"
#include "coreserialconverter.h"

_LIT8(KCmdText, "Cmd");
_LIT8(KRspText, "Rsp");

/**
Factory function.
@return Ownership of a new CRemConSerialBearer.
*/
CCoreSerialConverter* CCoreSerialConverter::NewL()
	{
	CCoreSerialConverter* self = new(ELeave) CCoreSerialConverter;
	return self;
	}

/**
Destructor.
*/
CCoreSerialConverter::~CCoreSerialConverter()
	{
	}

/**
Constructor.
*/
CCoreSerialConverter::CCoreSerialConverter()
	{
	}

TAny* CCoreSerialConverter::GetInterface(TUid aUid)
	{
	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KRemConConverterInterface1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConConverterInterface*>(this)
			);
		}

	return ret;
	}

TBool CCoreSerialConverter::SupportedUids(const TUid& aInterfaceUid, const TUid& aBearerUid) const
	{
	return (aInterfaceUid == TUid::Uid(KRemConCoreApiUid) &&
	   aBearerUid == TUid::Uid(KRemConSerialBearerUid));
	}

TBool CCoreSerialConverter::SupportedInterface(const TDesC8& /*aInterfaceData*/, const TUid& aBearerUid) const
	{
	// aInterfaceData is not used by the serial bearer.
	return aBearerUid == TUid::Uid(KRemConSerialBearerUid);
	}

/**
Convert data from the API's form (as we're given it by RemCon) the
bearer-specific form.
*/
TInt CCoreSerialConverter::InterfaceToBearer(TUid aInterfaceUid,
		TUint aOperationId,
		const TDesC8& aData,
		TRemConMessageType aMsgType,
		TDes8& aBearerData) const
	{
	TInt ret = KErrCorrupt;

	if ( aData.Length() <= 10 )
		{
		if ( aMsgType == ERemConCommand )
			{
			aBearerData.Format(_L8("0x%08x 0x%02x %S %S"), aInterfaceUid, aOperationId, &KCmdText(), &aData);
			// Pad it up in case aData was less than 4 characters.
			while ( aBearerData.Length() < KRemConSerialBearerMessageLength )
				{
				aBearerData.Append(_L8(" "));
				}
			ret = KErrNone;
			}
		else if ( aMsgType == ERemConResponse )
			{
			aBearerData.Format(_L8("0x%08x 0x%02x %S %S"), aInterfaceUid, aOperationId, &KRspText(), &aData);
			// Pad it up in case aData was less than 4 characters.
			while ( aBearerData.Length() < KRemConSerialBearerMessageLength )
				{
				aBearerData.Append(_L8(" "));
				}
			ret = KErrNone;
			}
		}

	return ret;
	}


TInt CCoreSerialConverter::BearerToInterface(const TDesC8& aBearerData,
		TUid& aInterfaceUid,
		TUint& aOperationId,
		TRemConMessageType& aMsgType,
		TDes8& aData) const
	{
	TInt ret = KErrCorrupt;

	// We read KRemConSerialBearerMessageLength bytes at a time, so we should
	// get precisely one message.
	if ( aBearerData.Length() == KRemConSerialBearerMessageLength )
		{
		TPtrC8 ptr;
		ptr.Set(aBearerData);

		TPtrC8 interfaceUid;
		interfaceUid.Set(ptr.Mid(2, 8));
		TLex8 interfaceLex(interfaceUid);
		TUint uid = 0;
		ret = interfaceLex.Val(uid, EHex);
		if ( ret == KErrNone )
			{
			aInterfaceUid = TUid::Uid(uid);

			TPtrC8 operationId;
			operationId.Set(ptr.Mid(13, 2));
			TLex8 operationLex(operationId);
			TUint id = 0;
			ret = operationLex.Val(id, EHex);
			if ( ret == KErrNone )
				{
				aOperationId = id;

				TPtrC8 data;
				data.Set(ptr.Mid(16, 3));
				if ( data == KCmdText() )
					{
					aMsgType = ERemConCommand;
					}
				else if ( data == KRspText() )
					{
					aMsgType = ERemConResponse;
					}
				else
					{
					ret = KErrCorrupt;
					}

				if ( ret == KErrNone )
					{
					TPtrC8 data;
					data.Set(ptr.Mid(20, 10));
					aData.Copy(data);
					}
				}
			}
		}

	return ret;
	}
