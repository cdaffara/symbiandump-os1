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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// Offers a tool to declare message structure as a class and parse/compose the message
// from/into a descriptor. The design assumes byte addressing (byte == octet)
// Overall idea
// Data buffer => maps to a structure of items each linked to the next one.
// The first item is pointed to by the class TRecord
// |                        Data Buffer                                                 |
// |     an Item<THEADER> 1       |  2 |             3         |          N             |
// |Header|       Body            |
// | +-> Constant of length 0 bytes             => constant length payload (class TConstant)
// +---> Value of length 1 - sizeof(TInt) bytes => variable length payload (class TValue)
// <LAYOUT>
// V          V
// Big Endian          Little Endian
// (class TBigEndian)   (class TLittleEndian)
// class TRecord uses its member iFirst(pointing to the first CBaseItem in the structure)
// to process all items in the structure
// The solution is not thread safe
// 
//

/**
 @file
 @internalTechnology
 @code
 @endcode
*/

#include <e32base.h>
#include <comms-infras/commsdebugutility.h>

#ifndef _RECORDITEM_H_
#define _RECORDITEM_H_

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemMsgParserRcr, "ElemMsgParserRcr");
#endif

/**
 A template parameter for TValue<>; it sets/gets a
 value into/from a buffer as a big endian of length 0 - sizeof(TInt)
 @see TValue<>
 @internalTechnology
**/
class TBigEndian
{
public:
   TBigEndian() {};

   IMPORT_C static TInt GetValue( const TUint8 *aPtr, TInt aValueLength );
   IMPORT_C static void SetValue( TUint8 *aPtr, TInt aValueLength, TInt aValue );
};

/**
 A template parameter for TValue<>; it sets/gets a
 value into/from a buffer as a little endian of length 0 - sizeof(TInt)
 @see TValue<>
 @internalTechnology
 **/
class TLittleEndian
{
public:
   TLittleEndian() {};

   IMPORT_C static TInt GetValue( const TUint8* aPtr, TInt aValueLength );
   IMPORT_C static void SetValue( TUint8 *aPtr, TInt aValueLength, TInt aValue );
};

/**
 A template parameter to CItem<>; it sets/gets a
 value into/from a buffer as a little/big endian depending on the template parameter.

 Also, it keeps the value length and the initial value.
 @see CItem<>
 @see TLittleEndian
 @see TBigEndian
 @internalTechnology
**/
template < class LAYOUT >
class TValue
{
public:
   TValue( TInt aValueLength ) :
      iValueLength( aValueLength )
   {
   }

   /**
    * Gets the length of the value in bytes(octets)
    *
    * @return The value length
    */
   TInt GetValueLength() const
   {
      return iValueLength;
   }

   /**
    * Sets the value length in bytes(octets)
    *
    * @param aValueLength initial value
    */
   void SetValueLength( TInt aValueLength )
   {
      iValueLength = aValueLength;
   }

   /**
    * Gets the initial value
    *
    * @return The initial value
    */
   TInt GetInitialValue() const
   {
      return iInitialValue;
   }

   /**
    * Sets the initial value
    *
    * @param aInitialValue The initial value
    */
   void SetInitialValue( TInt aInitialValue )
   {
      iInitialValue = aInitialValue;
   }

protected:
   TInt GetValue( const TUint8* aPtr ) const;
   void SetValue( TUint8* aPtr8, TInt aValue ) const;

protected:
   TInt iValueLength;
   TInt iInitialValue;
};

/**
 A template parameter to CItem<> to create an item with
 a header of length 1 - sizeof(TInt) and with a big endian layout.
 @see CItem<>
 @see TBigEndian
 @internalTechnology
**/
class TValueBigEndian : public TValue<TBigEndian>
{
public:
   TValueBigEndian( TInt aValueLength ) :
      TValue<TBigEndian>( aValueLength )
      {
      }

   IMPORT_C TInt GetValue( const TUint8* aPtr ) const;
   IMPORT_C void SetValue( TUint8* aPtr8, TInt aValue ) const;
};

/**
 A template parameter to CItem<> to create an item with
 a header of length 1 - sizeof(TInt) and with a little endian layout.
 @see CItem<>
 @see TLittleEndian
 @internalTechnology
**/
class TValueLittleEndian : public TValue<TLittleEndian>
{
public:
   TValueLittleEndian( TInt aValueLength ) :
      TValue<TLittleEndian>( aValueLength )
      {
      }

   IMPORT_C TInt GetValue( const TUint8* aPtr ) const;
   IMPORT_C void SetValue( TUint8* aPtr8, TInt aValue ) const;
};

/**
 A template parameter to CItem<>
 in the case that the item has constant length (no header).

 It keeps the length of the item
 @see CItem<>
 @internalTechnology
**/
class TConstant
{
public:
   TConstant( TInt aValue ) :
      iValue( aValue )
   {
   }

   /**
    * Gets the value from the data buffer.

    * For constant length items, there is no
    * data buffer and the value is given by iValue.
    *
    * @see TValue::GetValue()
    * param not used
    * @return iValue
    */
   TInt GetValue( const TUint8* ) const
   {
      return iValue;
   }

   /**
    * Gets the length of the value in bytes(octets)
    *
    * @return 0
    */
   TInt GetValueLength() const
   {
      return 0;
   }

   /**
    * Sets the value length in bytes(octets)
    *
    * param not used - constant value has length 0
    */
   void SetValueLength( TInt /*aValueLength*/ )
   {
   }

   /**
    * Sets the value into the data buffer.

    * For constant length items, there is no
    * data buffer and the value is given by iValue => function does nothing.
    *
    * @see TValue::SetValue()
    * param not used
    */
   void SetValue( TUint8*, TInt ) const
   {
   }

   /**
    * Gets the initial value
    *
    * @return The initial value == iValue
    */
   TInt GetInitialValue() const
   {
      return iValue;
   }

protected:
   const TInt iValue;
};

/**
 A base class for a buffer item represented by CItem<>, which derives from it.

 It keeps a pointer to the beginning of the data belonging to the item and points to
 the next CItemBase object. It provides basic functions for the item.
 @see CItem<>
 @internalTechnology
  @version 0.01
 @date	12/11/2003
**/
class CItemBase : public CBase
{
public:
   CItemBase( CItemBase* aNext ) :
      iNext( aNext )
   {
   }

   virtual void Dump( const TDesC& aTag, const TDesC& aFile ) = 0;
   virtual void ParseL( TPtr8& aDes8 ) = 0;
   virtual void InitialiseL( TPtr8& aBuf8 ) = 0;  //buffer must be initialised beforehand
   virtual TInt CalcTotalInitialiseLength() const = 0;

   /**
    * Sets the iPtr8 to point to the beginning of the descriptor
    *
    * @param aDes8 An 8 bit non-modifiable descriptor representing the item's data
    */
   void SetPtr( TDesC8& aDes8 )
   {
      iPtr8 = (TUint8*)aDes8.Ptr();
   }

   /**
    * Returns a pointer to the beginning of the item data
    *
    * @return iPtr8 a pointer to an 8-bit unsigned integer representing the item data
    */
   TUint8* Ptr() const
   {
      return iPtr8;
   }

public:
   CItemBase* iNext;

protected:
   IMPORT_C void SetBody( const TDesC8& aDes8, TInt aHeaderLength );
   IMPORT_C void InitialiseL( TPtr8& aBuf8, TInt aInitialLength, TInt aHeaderLength );
   IMPORT_C void ParseL( TPtr8& aDes8, TInt aLength, TInt aHeaderLength );
   IMPORT_C void CopyBodyToL( HBufC8*& aBuf8, TInt aLength, TInt aHeaderLength );

protected:
   TUint8* iPtr8; //ptr to part of record belonging to the item
};

/**
 Represents an item having header and payload.

 It offers functions to manipulate the item data.
 @see TValue
 @see TConstant
 @internalTechnology
**/
template <class THEADER>
class CItem : public CItemBase
{
public:
   CItem( CItemBase* aNext, TInt aValue ) :
      CItemBase( aNext ),
      iHeader( aValue )
   {
   }
   virtual void InitialiseL( TPtr8& aBuf8 ); //buffer must be initialised beforehand
   virtual void ParseL( TPtr8& aDes8 );
   virtual TInt CalcTotalInitialiseLength() const;

   void SetLength(); //assumes there is enough space for the header
   TInt GetLength() const; //from the item header (must be initialised || parsed)
   TInt GetItemLength() const; //from the item header (must be initialised || parsed) + header length
   TInt GetHeaderLength() const;
   TUint8* GetBodyPtr() const;
   TPtr8 GetBodyDes() const;
   TPtr8 GetItemDes() const;
   void SetBody( const TDesC8& aDes8 );
   void CopyBodyToL( HBufC8*& aBuf8 );

   //the number is considered to have as many bytes as GetLengthL() value
   void SetBigEndian( TUint32 aUint );
   TUint32 GetBigEndian() const;
   void SetLittleEndian( TUint32 aUint );
   TUint32 GetLittleEndian() const;

   THEADER& Header();

   virtual void Dump( const TDesC& aTag, const TDesC& aFile );
public:
   TBuf<16> iName;


private:
   THEADER iHeader;
};

/**
 * Returns a reference to a header class
 *
 * @return iHeader (type is template parameter dependent)
 */
template <class THEADER>
inline THEADER& CItem<THEADER>::Header()
{
   return iHeader;
}

template <class THEADER>
inline void CItem<THEADER>::SetLength()
/**
 * Sets the initial item length into the item data buffer.

 * The function assumes there is enough space for the header which
 * implies that the item must be parsed or initialised
 *
 */
{
   iHeader.SetValue( iPtr8, iHeader.GetInitialValue() );
}

template <class THEADER>
inline TInt CItem<THEADER>::GetLength() const
/**
 * Gets the item length from the item data buffer.

 * The function assumes there is enough space for the header which
 * implies that the item must be parsed or initialised
 *
 * @return the length of the item body
 */
{
   return iHeader.GetValue( iPtr8 );
}

template <class THEADER>
inline TInt CItem<THEADER>::GetHeaderLength() const
/**
 * Gets the length of the header in bytes(octets)
 *
 * @return the length of the item's header
 */
{
   return iHeader.GetValueLength();
}

template <class THEADER>
TUint32 CItem<THEADER>::GetBigEndian() const
/**
 * Gets the big endian value from the item body.

 * It panics if the body length > sizeof(TInt). The item must be parsed or initialised
 *
 * @see TValue::GetValue()
 * @return  A 32-bit unsigned integer type representing the extracted value
 */
{
   TBigEndian val;
   return val.GetValue( GetBodyPtr(), GetLength() );
}

template <class THEADER>
TUint32 CItem<THEADER>::GetLittleEndian() const
/**
 * Gets the little endian value from the item body.

 * It panics if the body length > sizeof(TInt). The item must be parsed or initialised
 *
 * @see TValue::GetValue()
 * @return  A 32-bit unsigned integer type representing the extracted value
 */
{
   TLittleEndian val;
   return val.GetValue( GetBodyPtr(), GetLength() );
}

template <class THEADER>
inline TUint8* CItem<THEADER>::GetBodyPtr() const
/**
 * Gets the pointer to the item body.

 * The item must be parsed or initialised
 *
 * @return A pointer to the item body
 */
{
   return iPtr8 + GetHeaderLength();
}

template <class THEADER>
inline void CItem<THEADER>::SetBody( const TDesC8& aDes8 )
/**
 * Copies the aDes into the item body.

 * The item must be parsed or initialised
 *
 * @param aDes8 An 8 bit non-modifiable descriptor representing the source data
 */
{
   __ASSERT_DEBUG(iHeader.GetInitialValue() >= aDes8.Length(), User::Panic(KSpecAssert_ElemMsgParserRcr, 1));
   CItemBase::SetBody( aDes8, GetHeaderLength() );
}

template <class THEADER>
inline TInt CItem<THEADER>::GetItemLength() const
/**
 * Gets the item length (body+header).

 * The item must be parsed or initialised
 *
 * @return the item length
 */
{
   return GetLength() + GetHeaderLength();
}

template <class THEADER>
inline TPtr8 CItem<THEADER>::GetBodyDes() const
/**
 * Creates the item body descriptor.

 * The item must be parsed or initialised
 *
 * @return An 8 bit modifiable pointer descriptor representing the item bod
 */
{
   TInt n = GetLength();
   return TPtr8( GetBodyPtr(), n, n );
}

template <class THEADER>
inline TPtr8 CItem<THEADER>::GetItemDes() const
/**
 * Creates the item modifiable descriptor.

 * The item must be parsed or initialised
 *
 * @return the item
 */
{
   TInt n = GetItemLength();
   return TPtr8( Ptr(), n, n );
}

template <class THEADER>
void CItem<THEADER>::SetBigEndian( TUint32 aUint )
/**
 * Sets the big endian value into the item body.

 * It panics if the body length > sizeof(TInt). The item must be parsed or initialised
 *
 * @see TValue::SetValue()
 * @param aUint value to set
 */
{
   TBigEndian val;
   val.SetValue( iPtr8 + GetHeaderLength(), GetLength(), aUint );
}

template <class THEADER>
void CItem<THEADER>::SetLittleEndian( TUint32 aUint )
/**
 * Sets the little endian value into the item body.

 * It panics if the body length > sizeof(TInt). The item must be parsed or initialised
 *
 * @see TValue::SetValue()
 * @param aUint value to set
 */
{
   TLittleEndian val;
   val.SetValue( GetBodyPtr(), GetLength(), aUint );
}

template <class THEADER>
inline TInt CItem<THEADER>::CalcTotalInitialiseLength() const
/**
 * Calculates the length of the data buffer to hold the item's data.

 * An initial value must be assigned to the item's header.
 *
 * @return the buffer length
 */
{
   return iHeader.GetInitialValue() + GetHeaderLength();
}

template <class THEADER>
void CItem<THEADER>::CopyBodyToL( HBufC8*& aBuf8 )
/**
 * Copies the message body into the HBufC8.

 * Allocates/reallocates the specified aBuf8
 * if necessary. The item must be parsed or initialised
 *
 * @param aBuf8 descriptor to set
 */
{
   CItemBase::CopyBodyToL( aBuf8, GetLength(), GetHeaderLength() );
}

template <class THEADER>
void CItem<THEADER>::InitialiseL( TPtr8& aBuf8 )
/**
 * Initialises the item pointer to point to the beginning of the buffer
 *
 * @param aBuf8  An 8 bit modifiable pointer descriptor to hold the item's data
 */
{
   CItemBase::InitialiseL( aBuf8, iHeader.GetInitialValue(), GetHeaderLength() );
   //set expected length to the item header
   SetLength();
}

template <class THEADER>
void CItem<THEADER>::ParseL( TPtr8& aDes8 )
/**
 * Checks the input and initialises the item pointer to point to the beginning
 * of the input buffer
 *
 * @param aDes8 An 8 bit modifiable pointer descriptor representing the descriptor to be checked
 */
{
   CItemBase::ParseL( aDes8, iHeader.GetValue( aDes8.Ptr() ), GetHeaderLength() );
}

#ifdef _DEBUG
template <class THEADER>
inline void CItem<THEADER>::Dump( const TDesC& aTag, const TDesC& aFile )
/**
 * Dumps the item.

 * The item must be parsed or initialised
 *
 * @param aTag A log file tag
 * @param aFile A log file
 */
   {
#ifdef __FLOG_ACTIVE
   RFileLogger f;
   if ( f.Connect() == KErrNone )
      {
      f.CreateLog( aTag, aFile, EFileLoggingModeAppend );
	   f.WriteFormat( _L("Item[%S]"), &iName );
	   f.WriteFormat( _L("header length: %d, length from header: %d, initial length: %d"),
         GetHeaderLength(), GetLength(), iHeader.GetInitialValue() );
      if ( GetHeaderLength() )
         {
	      f.Write( _L("Header & Body:"));
	      f.HexDump( NULL, NULL, iPtr8, GetItemLength() );
         }
      else if ( GetLength() )
         {
	      f.Write( _L("Body:"));
	      f.HexDump( NULL, NULL, iPtr8, GetItemLength() );
         }
      f.CloseLog();
      f.Close();
      }
#else
(void)aTag;
(void)aFile;
#endif
   }
#else  // #ifdef _DEBUG
template <class THEADER>
inline void CItem<THEADER>::Dump( const TDesC& /*aTag*/, const TDesC& /*aFile*/ )
   {
   }
#endif

/**
 Keeps a pointer to the first item and processes all items in the message structure.

 There is one instance of this class in each message structure.
 @internalTechnology
 **/
class TRecord
{
public:
   TRecord( CItemBase* aFirst ) :
      iFirst( aFirst )
   {
   }

   void Dump( const TDesC& aTag, const TDesC& aFile );
   IMPORT_C void ParseL( TPtr8& aDes8 );
   IMPORT_C void InitialiseL( TPtr8& aBuf8 );  //buffer must be initialised beforehand
   IMPORT_C TInt CalcTotalInitialiseLength() const;

   CItemBase* First() const
   {
      return iFirst;
   }

protected:
   IMPORT_C CItemBase& Get( TInt nIndex );

public:
   CItemBase* iFirst;
};

//Dump methods must be present in UREL builds also to avoid vtable BC breaks
//when SSL component is debug but this one isn't. Still not exposing it
//as an export to avoid changing the DEF files now.
#ifdef _DEBUG
inline void TRecord::Dump( const TDesC& aTag, const TDesC& aFile )
/**
 * Dumps the item list starting with iFirst.

 * The items must be parsed or initialised
 *
 * @param aTag A log file tag
 * @param aFile A log file
 */
{
   CItemBase* pItem = iFirst;
   while ( pItem != 0 )
   {
      pItem->Dump( aTag, aFile );
      pItem = pItem->iNext;
   }
}
#else
inline void TRecord::Dump( const TDesC& /*aTag*/, const TDesC& /*aFile*/ )
{
}
#endif

/**
 Represents an item of constant length
 @internalTechnology
 **/
class CConstItem : public CItem< TConstant >
{
public:
   CConstItem( CItemBase* aNext, TInt aLength ) :
      CItem< TConstant >( aNext, aLength )
   {
   }

  // Empty destructor necessary to compile for x86gcc target.
  ~CConstItem()
	  {
	  }
};

/**
 Represents an item of variable length having a big endian layout
 @internalTechnology
 **/
class CVariableItem : public CItem< TValueBigEndian >
{
public:
   CVariableItem( TInt aHeaderLength, CItemBase* aNext, TInt aInitLength = 0 ) :
      CItem< TValueBigEndian >( aNext, aHeaderLength )
   {
      Header().SetInitialValue( aInitLength );
   }
};

/**
 The class represents a header of an item list.

 An item list is a variable part of a message. It's created as the data is being parsed
 @see CListItem
 @internalTechnology
 **/
class CListItemHeader : public CConstItem
{
public:
   CListItemHeader( CItemBase* aNext, TInt aHeaderLength ) :
      CConstItem( aNext, aHeaderLength  )
   {
   }
};

/**
 Represents one item of the item list.

 The item has variable length with a big endian header layout
 @see CListItem
 @internalTechnology
 **/
class CListNode : public CItem< TValueBigEndian >
{
   friend class CListItem;

public:
	 CListNode* Next() const;

protected:
   CListNode( TInt aHeaderLength );
   IMPORT_C ~CListNode();
};

inline CListNode::CListNode( TInt aHeaderLength ) :
   CItem< TValueBigEndian >( NULL, aHeaderLength )
{
}

inline CListNode* CListNode::Next() const
   {
      return static_cast<CListNode*>(iNext);
   }

/**
 Represents an item list consisting of CListNode items and
 having a CListItemHeader header.
 @see CListNode
 @see CListItemHeader
 @internalTechnology
 **/
class CListItem : public CListItemHeader
{
public:
   CListItem( CItemBase* aNext, TInt aHeaderLength );
   IMPORT_C virtual ~CListItem();

   virtual void Dump( const TDesC& aTag, const TDesC& aFile );
   IMPORT_C void AddNode( CItemBase* aItemBase );
   IMPORT_C void AddNodeL( TInt aInitialLength );
   IMPORT_C virtual void InitialiseL( TPtr8& aBuf8 );  //buffer must be initialised beforehand
   IMPORT_C virtual TInt CalcTotalInitialiseLength() const;

   IMPORT_C virtual void ParseL( TPtr8& aDes8 );

   void RemoveAllNodes();

   CListNode* First() const;

protected:
   TRecord iRecord;

};

inline CListItem::CListItem( CItemBase* aNext, TInt aHeaderLength ) :
   CListItemHeader( aNext, aHeaderLength ),
   iRecord( NULL )
{
}

inline CListNode* CListItem::First() const
   {
      return static_cast<CListNode*>( iRecord.First() );
   }

#ifdef _DEBUG
inline void CListItem::Dump( const TDesC& aTag, const TDesC& aFile )
/**
 * Dumps the variable item list's header and item list.

 * The items must be parsed or initialised
 *
 * @param aTag A log file tag
 * @param aFile A log file
 */
{
   CListItemHeader::Dump( aTag, aFile );
   iRecord.Dump( aTag, aFile );
}
#else
inline void CListItem::Dump( const TDesC& /*aTag*/, const TDesC& /*aFile*/ )
{
}
#endif

inline void CListItem::RemoveAllNodes()
/**
 * Deletes all items from the list.
 *
 */
{
   delete iRecord.iFirst;
   iRecord.iFirst = NULL;
}

#endif

