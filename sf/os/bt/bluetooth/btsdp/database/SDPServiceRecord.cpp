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

#include <es_sock.h>
#include <btsdp.h>
#include "attrvalueencoded.h"
#include "sdpconsts.h"
#include "SDPServiceRecord.h"
#include "sdputil.h"


// ******************* CSdpServRecord ********************************

EXPORT_C CSdpServRecord *CSdpServRecord::NewL()
	{
	CSdpServRecord *self=new (ELeave) CSdpServRecord(KNullUid);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CSdpServRecord *CSdpServRecord::NewServerSideL(const TUid& aUid)
	{
	CSdpServRecord *self=new (ELeave) CSdpServRecord(aUid);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpServRecord::CSdpServRecord(const TUid& aClientUid)
	: iAttributes(CSdpAttr::LinkOffset()), iClientUid(aClientUid)
	{
	}

void CSdpServRecord::ConstructL()
	{
	TPckg<TUint> intBuf(0);
	iRecordState = CSdpAttrValueUint::NewUintL(intBuf);

	// Create the buffer the correct size for storing a Uint
	TInt encodedSize = TElementEncoder::EncodedSize(iRecordState->Type(), iRecordState->DataSize());
	iEncodeBuf.CreateL(encodedSize);
	TElementEncoder encoder(iEncodeBuf);

	iEncoderVisitor = CAttrEncoderVisitor::NewL(encoder);
	}


CSdpServRecord::~CSdpServRecord()
	{
	// Delete all attributes on the attribute list
	iAttributes.ResetAndDestroy();
	
	// Remove this servRecord from any Q it might be on
	iLink.Deque();

	delete iEncoderVisitor;
	delete iRecordState;
	iEncodeBuf.Close();
	}

// handle may be encoded
EXPORT_C TSdpServRecordHandle CSdpServRecord::Handle() const
	{
	__ASSERT_ALWAYS(!iAttributes.IsEmpty(), DbPanic(ESdpDbRecordBadHandle));

	CSdpAttr *first = iAttributes.First();
	__ASSERT_ALWAYS(first->AttributeID() == 0, DbPanic(ESdpDbRecordBadHandle));
//	__ASSERT_ALWAYS(first->Value().Type() == ETypeUint, DbPanic(ESdpDbRecordBadHandle));
// needs to test for an encoded value, can we decode ?

	TSdpServRecordHandle retVal;
	TUint8* ptr = (TUint8*)first->Value().Des().Ptr();
 
	switch (first->Value().Type())
		{
		case ETypeUint:
			retVal = first->Value().Uint();
			break;
		case ETypeEncoded:
			{
			const TUint8 KTUint32Header = (ETypeUint << KSdpElemHdrTypeShift) | ESizeFourBytes;
			if (*ptr++ != KTUint32Header) DbPanic(ESdpAttrValueBadSize);
			retVal = BigEndian::Get32(ptr);
			}
			break;
		default:
			DbPanic(ESdpDbRecordBadHandle);
			return 0;	// no warnings
		}
	return retVal;
	}

EXPORT_C TUid CSdpServRecord::ClientUid() const
	{
	return iClientUid;
	}

/**
@see RecordStateChange
*/
EXPORT_C void CSdpServRecord::AddAttribute(CSdpAttr* aAttr)
/**
	This function adds an attribute to this record
**/
	{
	iAttributes.AddInOrder(*aAttr);
	}

/**
If an attribute for this record is updated or deleted this method
shall be called.  It will update the state attribute, which may
be used by remote devices to aid caching.

@internalTechnology
@released
*/
EXPORT_C void CSdpServRecord::RecordStateChange()
	{
	TDblQueIter<CSdpAttr> attrIter(iAttributes);
	CSdpAttr* attr = attrIter++;

	while(attr)
		{
		if(attr->AttributeID() == KSdpAttrIdServiceRecordState)
			{
			TUint state = iRecordState->Uint();

			TPckg<TUint> stateBuf(0);
			SdpUtil::PutUint(&stateBuf[0], ++state, sizeof(TUint));

			iRecordState->SetUintValue(stateBuf);

			// This encodes iRecordState into the buffer provided at construction
			iEncodeBuf.SetLength(0);
			TRAPD(err, iEncoderVisitor->EncodeAttributeL(*iRecordState));
			// Attribute encoding can only fail if the attribute is of an unknown type 
			// or the supplied buffer is too small.  We have set the length of the
			// buffer to the correct length on creation so that will not fail.
			// We know iRecordState is a CSdpAttrValueUint so cannot fail.
			__ASSERT_ALWAYS(!err, DbPanic(ESdpDbAttributeEncodingFailed));

			__ASSERT_ALWAYS(attr->Value().Type() == ETypeEncoded, DbPanic(ESdpDbStoredAttrValNotEncoded));
			reinterpret_cast<CSdpAttrValueEncoded&>(attr->Value()).SetEncodedValue(iEncodeBuf);

			break;
			}
		attr = attrIter++;
		}
	}

MSdpElementBuilder *CSdpServRecord::BuildUintL(const TDesC8& aUint)
/**
	This function builds a new attribute
**/
	{
	TUint16 id=0;
	TInt len = aUint.Length();
	
	if (len==1)
		id = aUint[0];	// Strictly this is an error
	else if (len==2)
		id = BigEndian::Get16(&aUint[0]);
	else
		User::Leave(KErrSdpBadAttributeId);

	CSdpAttr *attr=CSdpAttr::NewL(id,this);
	iAttributes.AddInOrder(*attr);
	return attr;
	}

EXPORT_C CRecordAttr::CRecordAttr()
: iValue(0)
	{
	}

EXPORT_C CRecordAttr::~CRecordAttr()
	{
	}

MSdpElementBuilder* CRecordAttr::BuildUintL(const TDesC8& aUint)
	{
	iValue = SdpUtil::GetUint(aUint);
	return this;
	}
