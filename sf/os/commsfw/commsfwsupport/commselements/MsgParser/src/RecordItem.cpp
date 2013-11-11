// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Offers a tool to declare message structure as a class and parse/compose the message
// from/into a descriptor
// 
//

/**
 @file
*/

#include "RecordItem.h"
#include <limits.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemMsgParserRcrc, "ElemMsgParsrRcrc");
#endif

EXPORT_C TInt TBigEndian::GetValue( const TUint8 *aPtr, TInt aValueLength )
/**
 * Extracts the value from the data buffer in the big endian fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::GetValue()
 * @param aPtr a pointer to the source data buffer
 * @param aValueLength a length of the value in bytes (octets)
 * @return A TInt-bit unsigned integer type representing the extracted value
 */
   {
  	// note - amend this check to comply with different addressing modes, this would work just
   	//for byte addressing
   	__ASSERT_DEBUG(static_cast<TInt>(sizeof(TInt)) >= aValueLength, User::Panic(KSpecAssert_ElemMsgParserRcrc, 1));
   	TInt n = 0;
   	aPtr += aValueLength;
   	for ( TInt j = 0; j < aValueLength; ++j )
		{
       	n |= *(--aPtr) << (CHAR_BIT * j);
	   	}
  	return n;
   	}

EXPORT_C void TBigEndian::SetValue( TUint8 *aPtr, TInt aValueLength, TInt aValue )
/**
 * Sets the value into the data buffer in the big endian fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::SetValue()
 * @param aPtr a pointer to the destination data buffer
 * @param aValueLength a length of the value in bytes (octets)
 * @param aValue the value to set
 */
   	{
	// note - amend this check to comply with different addressing modes, this would work just
	//for byte addressing
	__ASSERT_DEBUG(static_cast<TInt>(sizeof(TInt)) >= aValueLength, User::Panic(KSpecAssert_ElemMsgParserRcrc, 2));
	aPtr += aValueLength;
	for ( TInt j = 0; j < aValueLength; j++ )
		{
		*(--aPtr) = TUint8((aValue >> (8 * j)) & 0xff);
		}
   	}

EXPORT_C TInt TLittleEndian::GetValue( const TUint8 *aPtr, TInt aValueLength )
/**
 * Extracts the value from the data buffer in the little endian fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::GetValue()
 * @param aPtr a pointer to the source data buffer
 * @param aValueLength a length of the value in bytes (octets)
 * @return A TInt-bit unsigned integer type representing the extracted value
 */
  	{
	// note - amend this check to comply with different addressing modes, this would work just
	//for byte addressing
	__ASSERT_DEBUG(static_cast<TInt>(sizeof(TInt)) >= aValueLength, User::Panic(KSpecAssert_ElemMsgParserRcrc, 3));
	TInt n = 0;
	for ( TInt j = 0; j < aValueLength; ++j )
		{
		n |= (*aPtr++) << (CHAR_BIT * j);
		}
	return n;
   	}

EXPORT_C void TLittleEndian::SetValue( TUint8 *aPtr, TInt aValueLength, TInt aValue )
/**
 * Sets the value into the data buffer in the little endian fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::SetValue()
 * @param aPtr a pointer to the destination data buffer
 * @param aValueLength a length of the value in bytes (octets)
 * @param aValue the value to set
 */
   	{
   	// note - amend this check to comply with different addressing modes, this would work just
   	//for byte addressing
   	__ASSERT_DEBUG(static_cast<TInt>(sizeof(TInt)) >= aValueLength, User::Panic(KSpecAssert_ElemMsgParserRcrc, 4));
	for ( TInt j = 0; j < aValueLength; j++ )
		{
		*(aPtr++) = TUint8((aValue >> (8 * j)) & 0xff);
		}
   	}

template <class LAYOUT>
inline TInt TValue<LAYOUT>::GetValue( const TUint8* aPtr ) const
/**
 * Extracts the value from the data buffer in the template parameter fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::GetValue()
 * @param aPtr a pointer to the source data buffer
 * @return A TInt-bit unsigned integer type representing the extracted value
 */
	{
	LAYOUT lay;
	return lay.GetValue( aPtr, iValueLength );
	}

template <class LAYOUT>
inline void TValue<LAYOUT>::SetValue( TUint8* aPtr8, TInt aValue ) const
/**
 * Sets the value into the data buffer in the template parameter fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::SetValue()
 * @param aPtr8 a pointer to the destination data buffer
 * @param aValue the value to set
 */
	{
	LAYOUT lay;
	lay.SetValue( aPtr8, iValueLength, aValue );
	}

EXPORT_C TInt TValueBigEndian::GetValue( const TUint8* aPtr ) const
/**
 * Extracts the value from the data buffer in the big endian fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::GetValue()
 * @param aPtr a pointer to the source data buffer
 * @return A TInt-bit unsigned integer type representing the extracted value
 */
	{
	return TValue<TBigEndian>::GetValue( aPtr );
	}

EXPORT_C void TValueBigEndian::SetValue( TUint8* aPtr8, TInt aValue ) const
/**
 * Sets the value into the data buffer in the big endian fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::SetValue()
 * @param aPtr8 a pointer to the destination data buffer
 * @param aValue the value to set
 */
	{
	TValue<TBigEndian>::SetValue( aPtr8, aValue );
	}

EXPORT_C TInt TValueLittleEndian::GetValue( const TUint8* aPtr ) const
/**
 * Extracts the value from the data buffer in the little endian fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::GetValue()
 * @param aPtr a pointer to the source data buffer
 * @return A TInt-bit unsigned integer type representing the extracted value
 */
	{
	return TValue<TLittleEndian>::GetValue( aPtr );
	}

EXPORT_C void TValueLittleEndian::SetValue( TUint8* aPtr8, TInt aValue ) const
/**
 * Sets the value into the data buffer in the big endian fashion.

 * It panics if the body length > sizeof(TInt).
 * It assumes that there is enough bytes aPtr points to.
 *
 * @see TConstant::SetValue()
 * @param aPtr8 a pointer to the destination data buffer
 * @param aValue the value to set
 */
	{
	TValue<TLittleEndian>::SetValue( aPtr8, aValue );
	}

EXPORT_C void CItemBase::SetBody( const TDesC8& aDes8, TInt aHeaderLength )
/**
 * Copies the aDes into the item body.

 * The item must be parsed or initialised
 *
 * @param aDes8 An 8 bit non-modifiable descriptor representing the source data
 * @param aHeaderLength a length of the item header in bytes(octets)
 */
	{
	TPtr8 ptr( iPtr8 +  aHeaderLength, 0, aDes8.Length() );
	ptr.Copy( aDes8 );
	}

EXPORT_C void CItemBase::CopyBodyToL( HBufC8*& aBuf8, TInt aLength, TInt aHeaderLength )
/**
 * Copies the message body into the HBufC8.

 * Allocates/reallocates the specified aBuf8
 * if necessary. The item must be parsed or initialised
 *
 * @param aBuf8 descriptor to set
 * @param aLength a length of the item in bytes(octets)
 * @param aHeaderLength a length of the item header in bytes(octets)
 */
	{
	if ( !aBuf8 || aBuf8->Des().MaxLength() < aLength )
		{
		  delete aBuf8;
		  aBuf8 = HBufC8::NewL( aLength );
		}
	aBuf8->Des().Copy( iPtr8 + aHeaderLength, aLength );
	}

EXPORT_C void CItemBase::InitialiseL( TPtr8& aBuf8, TInt aInitialLength, TInt aHeaderLength )
/**
 * Initialises the item pointer to point to the beginning of the buffer
 *
 * @param aBuf8  An 8 bit modifiable pointer descriptor to hold the item's data
 * @param aInitialLength a length of the item in bytes(octets)
 * @param aHeaderLength a length of the item header in bytes(octets)
 */
	{
	TInt nItemLength = aInitialLength + aHeaderLength;
	User::LeaveIfError( aBuf8.MaxLength() - aBuf8.Length() >= nItemLength ? KErrNone : KErrBadDescriptor );
	//initialise iPtr8 to point to the beginning of its buffer
	//iPtr8 payload is empty so far
	iPtr8 = const_cast<TUint8*>(aBuf8.Ptr() + aBuf8.Length());
	aBuf8.SetLength( aBuf8.Length() + nItemLength );
	}

EXPORT_C void CItemBase::ParseL( TPtr8& aDes8, TInt aLength, TInt aHeaderLength )
/**
 * Checks the input and initialises the item pointer to point to the beginning
 * of the input buffer
 *
 * @param aDes8 An 8 bit modifiable pointer descriptor representing the descriptor to be checked
 * @param aLength a length of the item in bytes(octets)
 * @param aHeaderLength a length of the item header in bytes(octets)
 */
	{
	TInt n;
	User::LeaveIfError( (n = aHeaderLength + aLength) <= aDes8.Length() ? KErrNone : KErrBadDescriptor );
	SetPtr( aDes8 );
	//this could've been Mid(..) if it worked
	TInt l = aDes8.Length() - n;
	aDes8.Set( const_cast<TUint8*>(aDes8.Ptr() + n), l, l );
	}

EXPORT_C CItemBase& TRecord::Get( TInt nIndex )
/**
 * Finds the n-th item in the item's list where n==nIndex.

 * Panics if no such item exists
 *
 * @param nIndex an index of desired item
 */
	{
	CItemBase* pItem = iFirst;
	while ( --nIndex )
		{
		pItem = pItem->iNext;
		}
	//always disaster this should not ever happen
	__ASSERT_ALWAYS( pItem != 0, User::Panic(KNullDesC,KErrGeneral) );
	return *pItem;
	}

EXPORT_C void TRecord::ParseL( TPtr8& aDes8 )
/**
 * Checks the input and initialises the items' pointers to point to the beginning
 * of theirs input buffers
 *
 * @param aDes8 An 8 bit modifiable pointer descriptor representing the descriptor to be checked
 */
	{
	CItemBase* pItem = iFirst;
	while ( pItem != 0 )
		{
		pItem->ParseL( aDes8 );
		pItem = pItem->iNext;
		}
	}

EXPORT_C TInt TRecord::CalcTotalInitialiseLength() const
/**
 * Calculates the length of the data buffer to hold the data for all items in the record.

 * An initial value must be assigned to the items' headers.
 *
 * @return the buffer length
 */
	{
	TInt n = 0;
	CItemBase* pItem = iFirst;
	while ( pItem != 0 )
		{
		n += pItem->CalcTotalInitialiseLength();
		pItem = pItem->iNext;
		}
   	return n;
	}

EXPORT_C void TRecord::InitialiseL( TPtr8& aBuf8 )
/**
 * Initialises the pointers and length for all the items in the record
 *
 * @param aBuf8  An 8 bit modifiable pointer descriptor to hold the item's data
 */
	{
   	CItemBase* pItem = iFirst;
	while ( pItem != 0 )
		{
		  pItem->InitialiseL( aBuf8 );
		  pItem = pItem->iNext;
		}
	}

EXPORT_C CListNode::~CListNode()
	{
	delete iNext;
	}

EXPORT_C CListItem::~CListItem()
	{
	delete iRecord.iFirst;
	}

EXPORT_C void CListItem::AddNode( CItemBase* aItemBase )
/**
 * Adds the item to the tail of the dynamic record.

 * Once added the item is owned by the record
 * Function is used to create a dynamic part of the message record
 *
 * @param aItemBase the item to be added
 */
	{
	CItemBase** ppNode = &iRecord.iFirst;
	while ( *ppNode != 0 )
		{
		  ppNode = &((*ppNode)->iNext);
		}
	*ppNode = aItemBase;
	}

EXPORT_C void CListItem::AddNodeL( TInt aInitialLength )
/**
 * Adds the CListNode item to the tail of the dynamic record.

 * Once added the item is owned by the record
 * Function is used to create a dynamic part of the message record
 *
 * @param aInitialLength the item body's initial length
 */
	{
	CListNode* pNode = new(ELeave) CListNode( CListItemHeader::GetItemLength() );
	AddNode( pNode );
	pNode->Header().SetInitialValue( aInitialLength );
	}

EXPORT_C TInt CListItem::CalcTotalInitialiseLength() const
/**
 * Calculates the length of the data buffer to hold the data for all items in the
   dynamic record.

 * An initial value must be assigned to the items' headers.
 *
 * @return the buffer length
 */
	{
	TInt n = CListItemHeader::CalcTotalInitialiseLength();
	return n + iRecord.CalcTotalInitialiseLength();
	}

EXPORT_C void CListItem::InitialiseL( TPtr8& aBuf8 )
/**
 * Initialises the pointers and length for all the items in the dynamic record
 *
 * @param aBuf8  An 8 bit modifiable pointer descriptor to hold the item's data
 */
	{
	CListItemHeader::InitialiseL( aBuf8 );
	TInt n = aBuf8.Length();
	iRecord.InitialiseL( aBuf8 );
	CListItemHeader::SetBigEndian( aBuf8.Length() - n );
	}

EXPORT_C void CListItem::ParseL( TPtr8& aDes8 )
/**
 * Checks the input and initialises the items' pointers to point to the beginning
 * of theirs input buffers
 *
 * @param aDes8 An 8 bit modifiable pointer descriptor representing the descriptor to be checked
 */
	{
	__ASSERT_DEBUG(iRecord.iFirst == NULL, User::Panic(KSpecAssert_ElemMsgParserRcrc, 5));
	CListItemHeader::ParseL( aDes8 );
	TInt nLenExpected = CListItemHeader::GetBigEndian();
	TInt nLengthOfItemHeader = CListItemHeader::GetItemLength();

	User::LeaveIfError( nLenExpected <= aDes8.Length() ? KErrNone : KErrBadDescriptor );
	CListNode** ppNode = reinterpret_cast<CListNode**>(&iRecord.iFirst);
	while ( nLenExpected > 0 )
		{
		*ppNode = new(ELeave) CListNode( nLengthOfItemHeader );//the parameter assumes that all items in list
		//have the same header length equal to length of the list item header
		//CListItem'd have to have additional member if the assumption doen't hold
		CleanupStack::PushL(*ppNode);
		(*ppNode)->ParseL( aDes8 );
		TInt nItemLen = (*ppNode)->GetItemLength();
		CleanupStack::Pop(*ppNode);
		ppNode = reinterpret_cast<CListNode**>(&(*ppNode)->iNext);
		nLenExpected -= nItemLen;
		User::LeaveIfError( nLenExpected >= 0 ? KErrNone : KErrBadDescriptor );
		}
	}

#ifdef __X86GCC__
// Ugly hack: GCC3.4 needs extra persuasion to unroll enough of the template; without this it won't link with a missing vtbl
static void ForceVtbl()
	{
	CItem<TConstant> x(NULL, 0);
	}
#endif

