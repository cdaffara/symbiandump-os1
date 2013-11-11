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
//

#include <bluetooth/logger.h>
#include <es_sock.h>
#include "DataEncoder.h"
#include "sdputil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDPDATABASE);
#endif

#ifdef _DEBUG
PANICCATEGORY("dataenc");
#endif

EXPORT_C TElementEncoder::TElementEncoder(TDes8& aWriteBuffer)
: iWriteBuf(aWriteBuffer)
	{
	}

EXPORT_C TInt TElementEncoder::WriteNil()
	{
	return WriteHeader(ETypeNil, 0);
	}

EXPORT_C TInt TElementEncoder::WriteUint(const TDesC8& aUint)
	{
	return WriteElement(ETypeUint, aUint);
	}

EXPORT_C TInt TElementEncoder::WriteUint(TUint aVal, TInt aNumberOfBytes)
	{
	TBuf8<sizeof(aVal)> val(aNumberOfBytes);
	PutUint(&val[0], aVal, aNumberOfBytes);
	return WriteUint(val);
	}

EXPORT_C TInt TElementEncoder::WriteInt(const TDesC8& aInt)
	{
	return WriteElement(ETypeInt, aInt);
	}

EXPORT_C TInt TElementEncoder::WriteInt(TInt aVal, TInt aNumberOfBytes)
	{
	TBuf8<sizeof(aVal)> val(aNumberOfBytes);
	PutUint(&val[0], static_cast<TInt>(aVal), aNumberOfBytes);
	return WriteInt(val);
	}

EXPORT_C TInt TElementEncoder::WriteUUID(const TUUID& aUUID)
	{
	return WriteElement(ETypeUUID, aUUID.Des());
	}

EXPORT_C TInt TElementEncoder::WriteString(const TDesC8& aString)
	{
	return WriteElement(ETypeString, aString);
	}

EXPORT_C TInt TElementEncoder::WriteBoolean(TBool aBoolean)
	{
	TBuf8<1> val(1);
	val[0] = TUint8(aBoolean ? 1 : 0);
	return WriteElement(ETypeBoolean, val);
	}

EXPORT_C TInt TElementEncoder::WriteDES(TUint aDataSize)
	{
	return WriteHeader(ETypeDES, aDataSize);
	}

EXPORT_C TInt TElementEncoder::WriteDEA(TUint aDataSize)
	{
	return WriteHeader(ETypeDEA, aDataSize);
	}

EXPORT_C TInt TElementEncoder::WriteURL(const TDesC8 &aUrl)
	{
	return WriteElement(ETypeURL, aUrl);
	}

EXPORT_C TInt TElementEncoder::WriteElement(TSdpElementType aType, const TDesC8 &aData)
	{
	TInt n = WriteHeader(aType, aData.Length());
	return n + WriteDesc(aData);
	}

EXPORT_C TInt TElementEncoder::WriteHeader(TSdpElementType aType, TUint aDataSize)
	{
	TUint headLen = HeaderSize(aType, aDataSize);
	ASSERT_DEBUG(headLen < 6);
	TBuf8<5> header(headLen);
	header[0] = TUint8(aType << 3 | SizeDescriptor(aType, aDataSize));

	if (headLen > 1)
		{// Write out the additional size bytes
		PutUint(&header[1], aDataSize, headLen-1);
		}

	return WriteDesc(header);
	}

EXPORT_C TInt TElementEncoder::WriteDesc(const TDesC8 &aDesc)
/**
	Append descriptor onto the write buffer.
	returns the number of bytes that did NOT fit into the write buffer.
**/
	{
	TInt remSpace = iWriteBuf.MaxLength() - iWriteBuf.Length();
	TInt writeLen = Min(aDesc.Length(), remSpace);
	iWriteBuf.Append(aDesc.Left(writeLen));
	return aDesc.Length() - writeLen;
	}

EXPORT_C void TElementEncoder::PutUint(TUint8* aPtr, TUint aUint, TInt aNumberOfBytes)
	{
	TUint test = aNumberOfBytes == 4 ? 1 : !(aUint & (0xffffffff << (aNumberOfBytes * 8)));
	 
	__ASSERT_ALWAYS(test,
					DbPanic(ESdpAttrValueBadSize));
	switch(aNumberOfBytes)
		{
	case 4:
		*aPtr++ = TUint8(aUint>>24);
	case 3:
		*aPtr++ = TUint8(aUint>>16);
	case 2:
		*aPtr++ = TUint8(aUint>>8);
	case 1:
		*aPtr = TUint8(aUint);
		break;
	default:
		DbPanic(ESdpAttrValueBadSize);
		}
	}

EXPORT_C TSdpElementSizeDesc TElementEncoder::SizeDescriptor(TSdpElementType aType, TUint aDataSize)
	{
	__ASSERT_ALWAYS(aType < TInt(sizeof(KSdpElementValidSizes)), DbPanic(ESdpAttrValueBadSize));
	if (aType == ETypeBoolean && aDataSize == 1)
		return ESizeOneByte;
	if (aType == ETypeNil && aDataSize == 0)
		return ESizeOneByte;
	

	TUint8 validSizes = KSdpElementValidSizes[aType];
	if (validSizes&0x1f)
		{
		//if size is in range [0,4] and aType is not Nil
		//then aDataSize should be non-zero.
		//Indeed the logic in the loop below the "__ASSERT"
		//fails for aDataSize = 0...
		__ASSERT_DEBUG(aDataSize, DbPanic(ESdpAttrValueBadSize)); 
		for (TInt i=0; i <= ESizeSixteenBytes; ++i)
			{
			if (TUint((1<<i)&validSizes) == aDataSize)
				return TSdpElementSizeDesc(i);
			}
		}

	if (validSizes&0xe0)
		{
		for (TInt i=ESizeOneAdditional; i <= ESizeFourAdditional; ++i)
			{
			if ((aDataSize >> (8*(i-(ESizeOneAdditional-1))) == 0) && TUint((1<<i)&validSizes))
				return TSdpElementSizeDesc(i);
			}
		}
	DbPanic(ESdpAttrValueBadSize);
	return ESizeOneByte;
	}

EXPORT_C TInt TElementEncoder::HeaderSize(TSdpElementType aType, TUint aDataSize)
	{
	TInt sizeDesc = SizeDescriptor(aType, aDataSize);
	return 1 + (sizeDesc >= 5 ? 1<<(sizeDesc-5) : 0);
	}

EXPORT_C TInt TElementEncoder::EncodedSize(TSdpElementType aType, TUint aDataSize)
	{
	return HeaderSize(aType, aDataSize) + aDataSize;
	}
