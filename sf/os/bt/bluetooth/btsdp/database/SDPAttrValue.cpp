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
// attribute value processing
// 
//


#include <btsdp.h>
#include <es_sock.h>
#include "attrvalueencoded.h"
#include "MAttributeVisitor.h"
#include "sdputil.h"
#include "DataEncoder.h"

const TUint8 KDummyUUIDPointer = 0xff;

// ******************* CSdpAttrValue ********************************

CSdpAttrValue::CSdpAttrValue()
/** Default constructor. */
	{
	}

CSdpAttrValue::~CSdpAttrValue()
/** Destructor. */
	{
	}

void CSdpAttrValue::AcceptVisitorL(MSdpAttributeValueVisitor& aVisitor)
/** Request a call back to pass the attribute value.

When called on attributes that are not lists (not DEA or DES), then this calls 
MSdpAttributeValueVisitor::VisitAttributeValueL(), passing the attribute value 
object itself (i.e. *this), and the value attribute type.

It is more useful when called on a DES or DEA object, as it then provides 
a simple method of enumerating each attribute in a list.

@param aVisitor Abstract interface that can be implemented to receive an enumeration 
of the values in an attribute list.
@see CSdpAttrValueList::AcceptVisitorL()
@see MSdpAttributeValueVisitor::VisitAttributeValueL() */
	{
	aVisitor.VisitAttributeValueL(*this, Type());
	}


// Universal Getters
// Defaults are to Panic -- should only be called when overriden

TUint CSdpAttrValue::Uint() const
/** Gets an unsigned integer attribute value.

The base class implementation panics.

The size of the unsigned integer should be checked with DataSize() before calling.

@return Attribute value */
	{
	DbPanic(ESdpDbBadAttrValueGetter);
	return 0;
	}

/*
UIDs used by the Extension_ method for CSdpAttrValue to identify the function called.

@internalComponent
*/

const TUint KSdpAttrValueUint64FunctionId  = 0x1028654A;
const TUint KSdpAttrValueUint128FunctionId = 0x1028654B;

EXPORT_C void CSdpAttrValue::Uint64(TUint64& aValue) const
/**
Gets the value as an unsigned 64 bit integer

The size of the unsigned integer should be checked with DataSize() before calling.

@param aValue A 64 bit unsigned integer
*/
	{
	TAny* valuePtr = &aValue;
	const_cast<CSdpAttrValue*>(this)->Extension_(KSdpAttrValueUint64FunctionId, valuePtr, NULL);
	}
	
EXPORT_C void CSdpAttrValue::Uint128(TUint64& aLo, TUint64& aHi) const 
/**
Gets two TUint64s, one being the high 64 bits and one being the low 64 bits of a 128 bit integer
  
The size of the unsigned integer should be checked with DataSize() before calling.

@param aLo The unsigned 64 bit integer contains the lower bits of the 128 bit integer contained in the descriptor
@param aHi The unsigned 64 bit integer contains the higher bits of the 128 bit integer contained in the descriptor
*/
 	{
	TAny* loPtr = &aLo;
	TAny* hiPtr = &aHi;
	const_cast<CSdpAttrValue*>(this)->Extension_(KSdpAttrValueUint128FunctionId, loPtr, hiPtr);
 	}

TInt CSdpAttrValue::Int() const
/** Gets a signed integer attribute value.

The base class implementation panics.

@return Attribute value */
	{
	DbPanic(ESdpDbBadAttrValueGetter);
	return 0;
	}

TBool CSdpAttrValue::DoesIntFit() const
/** Tests if the attribute can be stored in an integer value.

The base class implementation returns EFalse.

@return True if the attribute can be stored in an integer value */
	{
	return EFalse;
	}

TInt CSdpAttrValue::Bool() const
/** Gets a Boolean attribute value.

The base class implementation panics.

@return Attribute value */
	{
	DbPanic(ESdpDbBadAttrValueGetter);
	return 0;
	}

const TUUID& CSdpAttrValue::UUID() const
/** Gets a UUID attribute value.

The base class implementation panics.

@return Attribute value */
	{
	DbPanic(ESdpDbBadAttrValueGetter);
	return *(TUUID*)(KDummyUUIDPointer);
	}

const TPtrC8 CSdpAttrValue::Des() const
/** Gets a data buffer attribute value.

The base class implementation panics.

@return Attribute value */
	{
	DbPanic(ESdpDbBadAttrValueGetter);
	return TPtrC8(0,0);
	}

/** 
The extension method provides a polymorphic behaviour to allow interface extension.

The base class implementation panics.

@internalComponent
*/
TInt CSdpAttrValue::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)   
    {
    TInt ret = KErrNone;
    switch(aExtensionId)
        {
        case KSdpAttrValueUint64FunctionId:
        case KSdpAttrValueUint128FunctionId:
            {
        	DbPanic(ESdpDbBadAttrValueGetter);
            }
            break;

        default:
            {
            // Chain to base class
            ret = CBase::Extension_(aExtensionId, a0, a1);
            }
            break;
        }
    return ret;
    }

// ******************* Concrete CSdpAttrValues ************************

// LIST

CSdpAttrValueList::CSdpAttrValueList(MSdpElementBuilder *aParent)
	: iParent(aParent)
	{
	}

void CSdpAttrValueList::ConstructL()
	{
    iList = new (ELeave) CArrayPtrFlat<CSdpAttrValue> (2);
	}

CSdpAttrValueList::~CSdpAttrValueList()
/** Destructor. */
	{
	if ( iList )
		{
		// Destroy all elements on this list
		iList->ResetAndDestroy();
		delete iList;
		}
	}

void CSdpAttrValueList::AcceptVisitorL(MSdpAttributeValueVisitor& aVisitor)
/** Requests a call back to pass the attribute value.

This provides a simple method of enumerating each element in the list.

@param aVisitor Abstract interface that can be implemented to receive an enumeration 
of the values in the attribute list. */
	{
	CSdpAttrValue::AcceptVisitorL(aVisitor);
	aVisitor.StartListL(*this);
	TInt items = iList->Count();
	for (TInt i = 0; i < items; ++i)
		{
		(*iList)[i]->AcceptVisitorL(aVisitor);
		}
	aVisitor.EndListL();
	}

TUint CSdpAttrValueList::DataSize() const
/** Gets the size of the list.

@return Size of the list (in bytes) */
	{
	TUint size = 0;
	for (TInt i = 0; i < iList->Count(); ++i)
		{
		size += TElementEncoder::EncodedSize((*iList)[i]->Type(), (*iList)[i]->DataSize());
		}
	return size;
	}

EXPORT_C void CSdpAttrValueList::AppendValueL(CSdpAttrValue* aValue)
/** Add a new value onto the end on this list.

Ownership of the passed value is transferred to this list. It will be deleted 
when the list is destroyed.

If a leave occurs, aValue will be cleanup up automatically 

@param aValue Attribute value to be added onto this list. */
	{
	CleanupStack::PushL(aValue);
	iList->AppendL(aValue);
	CleanupStack::Pop(); // aValue
	}

MSdpElementBuilder* CSdpAttrValueList::BuildStringL(const TDesC8& aString)
/** Adds a Text String element.
@internalTechnology

@param aString Attribute to add
@return This attribute value with added element */
	{
	CSdpAttrValue *val=CSdpAttrValueString::NewStringL(aString);
	AppendValueL(val);
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildBooleanL(TBool aBool)
/** Adds a Boolean element.
@internalTechnology

@param aBool Attribute to add
@return This attribute value with added element */
	{
	CSdpAttrValue *val=CSdpAttrValueBoolean::NewBoolL(aBool);
	AppendValueL(val);
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildDESL()
/** Adds a Data element sequence (DES).
@internalTechnology

This should be followed by a call to StartListL(), and then calls to add elements 
to the list.

@return This attribute value with added element */
	{
	CSdpAttrValueList *val=CSdpAttrValueDES::NewDESL(this);
	AppendValueL(val);
	return val;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildDEAL()
/** Adds a Data element alternative (DEA).
@internalTechnology

This should be followed by a call to StartListL(), and then calls to add elements 
to the list.

@return This attribute value with added element */
	{
	CSdpAttrValueList *val=CSdpAttrValueDEA::NewDEAL(this);
	AppendValueL(val);
	return val;
	}

MSdpElementBuilder* CSdpAttrValueList::StartListL()
/** Indicates that subsequent elements added belong to a DES or DEA.
@internalTechnology

The end of the list should be indicated by a call to EndList().

@return This attribute value */
	{
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::EndListL()
/** Indicates the end of a list started by StartListL().
@internalTechnology

@return Parent of this attribute value */
	{
	return iParent;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildUnknownL(TUint8 /*aType*/, TUint8 /*aSizeDesc*/, const TDesC8& /*aData*/)
/** Does nothing.
@internalTechnology

@return This attribute value */
	{
	//CSdpAttrValue *val=CSdpAttrValueUnknown::NewUnknownL(aType,aSizeDesc,aData);
	//AppendValueL(val);
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildNilL()
/** Adds a null type element.
@internalTechnology

@return This attribute value with added element */
	{
	CSdpAttrValue *val=CSdpAttrValueNil::NewNilL();
	AppendValueL(val);
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildUintL(const TDesC8& aUint)
/** Adds an unsigned integer element.
@internalTechnology

@param aUint Attribute to add
@return This attribute value with added element */
	{
	CSdpAttrValue *val=CSdpAttrValueUint::NewUintL(aUint);
	AppendValueL(val);
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildIntL(const TDesC8& aInt)
/** Adds a signed integer element.
@internalTechnology

@param aInt Attribute to add
@return This attribute value with added element */
	{
	CSdpAttrValue *val=CSdpAttrValueInt::NewIntL(aInt);
	AppendValueL(val);
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildUUIDL(const TUUID& aUUID)
/** Adds a UUID element.
@internalTechnology

@param aUUID Attribute to add
@return This attribute value with added element */
	{
	CSdpAttrValue *val=CSdpAttrValueUUID::NewUUIDL(aUUID);
	AppendValueL(val);
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildURLL(const TDesC8& aString)
/** Adds a URL element.
@internalTechnology

@param aString URL to add
@return This attribute value with added element */
	{
	CSdpAttrValue *val=CSdpAttrValueURL::NewURLL(aString);
	AppendValueL(val);
	return this;
	}

MSdpElementBuilder* CSdpAttrValueList::BuildEncodedL(const TDesC8& aString)
/** Encode an attribute value.
@internalTechnology

@param aString The attribute value.
@return This attribute value. */
	{
	CSdpAttrValue* val=CSdpAttrValueEncoded::NewEncodedL(aString);
	AppendValueL(val);
	return this;
	}

// NIL

EXPORT_C CSdpAttrValueNil* CSdpAttrValueNil::NewNilL()
/** Allocates and constructs a new CSdpAttrValueNil object.

@return New CSdpAttrValueNil */
	{
	CSdpAttrValueNil* val=new (ELeave) CSdpAttrValueNil();
	return val;
	}

CSdpAttrValueNil::CSdpAttrValueNil()
	{
	}

CSdpAttrValueNil::~CSdpAttrValueNil()
/** Destructor. */
	{
	}

TSdpElementType CSdpAttrValueNil::Type() const
/** Gets the type of the attribute (always ETypeNil).

@return Type of the attribute */
	{
	return ETypeNil;
	}

TUint CSdpAttrValueNil::DataSize() const
/** Gets the size of the attribute (always 0).

@return Size of the attribute (in bytes). */
	{
	return 0;
	}

// UINT

EXPORT_C CSdpAttrValueUint* CSdpAttrValueUint::NewUintL(const TDesC8 &aUint)
/** Allocates and constructs a new CSdpAttrValueUint object.

@param aUint Buffer containing an unsigned integer value for the attribute
@return New CSdpAttrValueUint object */
	{
	if(aUint.Size()>KSdpMaxUintSize)
		{
		//Check less than or equal to 16 bytes = 128 bits
		User::Leave(KErrArgument);
		}
	CSdpAttrValueUint* val=new (ELeave) CSdpAttrValueUint(aUint);
	return val;
	}

/**
Replaces the current value. 

@param aValue The new value.  The provided value must not exceed KSdpMaxUintSize.
@panic USER 23 if the supplied value is too long.
*/
EXPORT_C void CSdpAttrValueUint::SetUintValue(const TDesC8& aValue)
	{
	// Copies data into iBuffer
	iUint = aValue;
	}

CSdpAttrValueUint::CSdpAttrValueUint(const TDesC8 &aUint)
	: iUint(aUint)
	{
	__ASSERT_DEBUG(aUint.Size()<=KSdpMaxUintSize, User::Panic(_L("CSdpAttrValueUint Constructor"), KErrArgument));
	}

CSdpAttrValueUint::~CSdpAttrValueUint()
/** Destructor. */
	{
	}

TSdpElementType CSdpAttrValueUint::Type() const
/** Gets the attribute type.

@return Attribute type. Always ETypeUint. */
	{
	return ETypeUint;
	}

TUint CSdpAttrValueUint::DataSize() const
/** Gets the size of the attribute.

@return Size of the attribute in bytes */
	{
	return iUint.Length();
	}

TUint CSdpAttrValueUint::Uint() const
/** Gets the value as an unsigned integer type.

The size of the unsigned integer should be checked with DataSize() before calling.

@return Attribute value */
	{
	return SdpUtil::GetUint(iUint);
	}
 
TBool CSdpAttrValueUint::DoesIntFit() const
/** Tests if the attribute can be stored in an integer value.

@return True if the attribute can be stored in an integer value */
	{
	return (iUint.Length()>16)?EFalse:ETrue;
	}

const TPtrC8 CSdpAttrValueUint::Des() const
/** Gets the value as a data buffer.

@return Attribute value */
	{
	return TPtrC8(iUint);
	}	

/** 
The extension method provides a polymorphic behaviour to allow interface extension.

Implements the functions for the Uint64 and Uint128 without breaking binary compatibility 

@internalComponent
*/
TInt CSdpAttrValueUint::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)    
	{
	switch(aExtensionId)
		{
		case KSdpAttrValueUint64FunctionId:
			{
			TUint64* value = reinterpret_cast<TUint64*>(a0);
			SdpUtil::GetUint64(iUint, *value);
			return KErrNone;
			}

		case KSdpAttrValueUint128FunctionId:
			{
			TUint64* lo = reinterpret_cast<TUint64*>(a0);
			TUint64* hi = reinterpret_cast<TUint64*>(a1);
		 	SdpUtil::GetUint128(iUint, *lo, *hi);
			return KErrNone;
			}

		default:
			{
			// Chain to base class
			return CSdpAttrValue::Extension_(aExtensionId, a0, a1);
			}
		}
	}

// INT

EXPORT_C CSdpAttrValueInt* CSdpAttrValueInt::NewIntL(const TDesC8 &aInt)
/** Allocates and constructs a new CSdpAttrValueInt object.

@param aInt Buffer containing a signed integer value for the attribute
@return New CSdpAttrValueInt object */
	{
	if(aInt.Size()>KSdpMaxIntSize)
		{ //Check less than 16 bytes = 128 bits
		User::Leave(KErrArgument);
		}
	CSdpAttrValueInt* val=new (ELeave) CSdpAttrValueInt(aInt);
	return val;
	}

CSdpAttrValueInt::CSdpAttrValueInt(const TDesC8 &aInt)
	: iInt(aInt)
	{
	__ASSERT_DEBUG(aInt.Size()<=KSdpMaxIntSize, User::Panic(_L("CSdpAttrValueInt Constructor"), KErrArgument));
	}

CSdpAttrValueInt::~CSdpAttrValueInt()
/** Destructor. */
	{
	}

TSdpElementType CSdpAttrValueInt::Type() const
/** Gets the attribute type.

@return Attribute type. Always ETypeInt. */
	{
	return ETypeInt;
	}

TUint CSdpAttrValueInt::DataSize() const
/** Gets the size of the attribute.

@return Size of the attribute in bytes */
	{
	return iInt.Length();
	}

TInt CSdpAttrValueInt::Int() const
/** Gets the value as a signed integer type.

@return Attribute value */
	{
	TUint uint = SdpUtil::GetUint(iInt);
	switch(iInt.Length())
		{
		case 1:
		return (TInt8)uint; //cast to remove sign obscuring leading zeros
		case 2:
		return (TInt16)uint; //cast to remove sign obscuring leading zeros
		default:
		return uint;
		}
	}

TBool CSdpAttrValueInt::DoesIntFit() const
/** Tests if the attribute can be stored in an integer value.

@return True if the attribute can be stored in an integer value */
	{
	return (iInt.Length()>4)?EFalse:ETrue;
	}

const TPtrC8 CSdpAttrValueInt::Des() const
/** Gets the value as a data buffer.

@return Attribute value */
	{
	return TPtrC8(iInt);
	}

// UUID

EXPORT_C CSdpAttrValueUUID* CSdpAttrValueUUID::NewUUIDL(const TUUID& aUUID)
/** Allocates and constructs a new CSdpAttrValueUUID object.

@param aUUID Attribute value
@return New CSdpAttrValueUUID object */
	{
	CSdpAttrValueUUID* val=new (ELeave) CSdpAttrValueUUID(aUUID);
	return val;
	}

CSdpAttrValueUUID::CSdpAttrValueUUID(const TUUID& aUUID)
	: iUUID(aUUID)
	{
	}

CSdpAttrValueUUID::~CSdpAttrValueUUID()
/** Destructor. */
	{
	}

TSdpElementType CSdpAttrValueUUID::Type() const
/** Gets the attribute type.

@return Attribute type. Always ETypeUUID. */
	{
	return ETypeUUID;
	}

TUint CSdpAttrValueUUID::DataSize() const
/** Gets the size of the attribute.

@return Size of the attribute in bytes */
	{// HACKME change this to vary the UUID size
	// make sure this is synchronized with Des()
	return iUUID.ShortestForm().Length();
	}

const TUUID& CSdpAttrValueUUID::UUID() const
/** Gets the attribute value.

@return Attribute value */
	{
	return iUUID;
	}

const TPtrC8 CSdpAttrValueUUID::Des() const
/** Gets the value as a data buffer.

@return Attribute value */
	{
//	return TPtrC8(iUUID.LongForm());
	return TPtrC8(iUUID.ShortestForm());
	}

// STRING

EXPORT_C CSdpAttrValueString* CSdpAttrValueString::NewStringL(const TDesC8& aString)
/** Allocates and constructs a new CSdpAttrValueString object.

@param aString Buffer containing a Text String value for the attribute
@return New CSdpAttrValueString object */
	{
	CSdpAttrValueString* st=new (ELeave) CSdpAttrValueString;
	CleanupStack::PushL(st);
	st->ConstructL(aString);
	CleanupStack::Pop();
	return st;
	}

void CSdpAttrValueString::ConstructL(const TDesC8& aString)
	{
	iBuffer=aString.AllocL();
	}

CSdpAttrValueString::CSdpAttrValueString()
	{
	}

CSdpAttrValueString::~CSdpAttrValueString()
/** Destructor. */
	{
	delete iBuffer;
	}

TSdpElementType CSdpAttrValueString::Type() const
/** Gets the attribute type.

@return Attribute type. Always ETypeString. */
	{
	return ETypeString;
	}

TUint CSdpAttrValueString::DataSize() const
/** Gets the size of the attribute.

@return Size of the attribute in bytes */
	{
	return (*iBuffer).Length();
	}

const TPtrC8 CSdpAttrValueString::Des() const
/** Gets the value as a data buffer.

@return Attribute value */
	{
	return TPtrC8(iBuffer->Des());
	}

// BOOL

EXPORT_C CSdpAttrValueBoolean* CSdpAttrValueBoolean::NewBoolL(TBool aBool)
/** Allocates and constructs a new CSdpAttrValueBoolean object.

@param aBool Value for the attribute
@return New CSdpAttrValueBoolean object */
	{
	CSdpAttrValueBoolean* val=new (ELeave) CSdpAttrValueBoolean(aBool);
	return val;
	}

CSdpAttrValueBoolean::CSdpAttrValueBoolean(TBool aBool)
	: iBool(aBool)
	{
	}

CSdpAttrValueBoolean::~CSdpAttrValueBoolean()
/** Destructor. */
	{
	}

TSdpElementType CSdpAttrValueBoolean::Type() const
/** Gets the attribute type.

@return Attribute type. Always ETypeBoolean. */
	{
	return ETypeBoolean;
	}

TUint CSdpAttrValueBoolean::DataSize() const
/** Gets the size of the attribute.

@return Size of the attribute in bytes. Always 1. */
	{
	return 1;
	}

TBool CSdpAttrValueBoolean::Bool() const
/** Gets the attribute value.

@return Attribute value */
	{
	return iBool;
	}


// DES

EXPORT_C CSdpAttrValueDES* CSdpAttrValueDES::NewDESL(MSdpElementBuilder* aBuilder)
/** Allocates and constructs a new CSdpAttrValueDES object.

@param aBuilder Parent for list. Set to NULL if the list is not nested in 
another list.
@return A new CSdpAttrValueDES object */
	{
	CSdpAttrValueDES* self=new (ELeave) CSdpAttrValueDES(aBuilder);
	CleanupStack::PushL(self);
	self->CSdpAttrValueList::ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpAttrValueDES::CSdpAttrValueDES(MSdpElementBuilder *aBuilder)
	: CSdpAttrValueList(aBuilder)
	{
	}	

TSdpElementType CSdpAttrValueDES::Type() const
/** Gets the attribute type.

@return Attribute type. Always ETypeDES. */
	{
	return ETypeDES;
	}

// DEA

EXPORT_C CSdpAttrValueDEA* CSdpAttrValueDEA::NewDEAL(MSdpElementBuilder* aBuilder)
/** Allocates and constructs a new CSdpAttrValueDEA object.

@param aBuilder Parent for list. Set to NULL if the list is not nested in 
another list.
@return A new CSdpAttrValueDEA object */
	{
	CSdpAttrValueDEA* self=new (ELeave) CSdpAttrValueDEA(aBuilder);
	CleanupStack::PushL(self);
	self->CSdpAttrValueList::ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpAttrValueDEA::CSdpAttrValueDEA(MSdpElementBuilder *aBuilder)
	: CSdpAttrValueList(aBuilder)
	{
	}

/** Gets the attribute type.

@return Attribute type. Always ETypeDEA. */
TSdpElementType CSdpAttrValueDEA::Type() const
	{
	return ETypeDEA;
	}


// URL

/** Allocates and constructs a new CSdpAttrValueURL object.

@param aString Buffer containing the attribute value
@return New CSdpAttrValueURL object */
EXPORT_C CSdpAttrValueURL* CSdpAttrValueURL::NewURLL(const TDesC8& aURL)
	{
	CSdpAttrValueURL* st=new (ELeave) CSdpAttrValueURL;
	CleanupStack::PushL(st);
	st->ConstructL(aURL);
	CleanupStack::Pop();
	return st;
	}

void CSdpAttrValueURL::ConstructL(const TDesC8& aURL)
	{
	iBuffer=aURL.AllocL();
	}

CSdpAttrValueURL::CSdpAttrValueURL()
	{
	}

CSdpAttrValueURL::~CSdpAttrValueURL()
/** Destructor. */
	{
	delete iBuffer;
	}

TSdpElementType CSdpAttrValueURL::Type() const
/** Gets the attribute type.

@return Attribute type. Always ETypeURL. */
	{
	return ETypeURL;
	}

TUint CSdpAttrValueURL::DataSize() const
/** Gets the size of the attribute.

@return Size of the attribute in bytes. */
	{
	return (*iBuffer).Length();
	}

const TPtrC8 CSdpAttrValueURL::Des() const
/** Gets the value as a data buffer.

@return Attribute value */
	{
	return TPtrC8(iBuffer->Des());
	}

// Encoded

EXPORT_C CSdpAttrValueEncoded* CSdpAttrValueEncoded::NewEncodedL(const TDesC8& aString)
	{
	CSdpAttrValueEncoded* st=new(ELeave) CSdpAttrValueEncoded();
	CleanupStack::PushL(st);
	st->ConstructL(aString);
	CleanupStack::Pop();
	return st;
	}

void CSdpAttrValueEncoded::ConstructL(const TDesC8& aString)
	{
	iBuffer=aString.AllocL();
	}

CSdpAttrValueEncoded::CSdpAttrValueEncoded()
	{
	}

CSdpAttrValueEncoded::~CSdpAttrValueEncoded()
	{
	delete iBuffer;
	}

TSdpElementType CSdpAttrValueEncoded::Type() const
	{
	return ETypeEncoded;
	}

TUint CSdpAttrValueEncoded::DataSize() const
	{
	return iBuffer->Length();
	}

const TPtrC8 CSdpAttrValueEncoded::Des() const
	{
	return TPtrC8(iBuffer->Des());
	}

/**
Replaces the current value. 

@param aValue The new value.  This must be an attribute value which has been 
			  appropriately encoded, for instance using the CSdpAttrEncoderVisitor
			  class.  The data in aValue will be copied.  The provided value must
			  not exceed the length of the current data.  This may be retrieved
			  via the CSdpAttrValueEncoded::Des() function and TPtrC8::Length()
			  function.
@panic USER 23 if the supplied value is too long.
*/
EXPORT_C void CSdpAttrValueEncoded::SetEncodedValue(const TDesC8& aValue)
	{
	// Copies data into iBuffer
	*iBuffer = aValue;
	}

