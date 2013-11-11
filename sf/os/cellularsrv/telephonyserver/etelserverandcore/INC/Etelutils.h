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
// Contains various utility classes which are used throughout Telephony.
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#if !defined(__ETELUTILS_H__)
/** @publishedPartner */
#define __ETELUTILS_H__

#include <e32base.h>

/**
Base class for generic actions in retrieving a variable length buffer in two phases.

This class is abstract.

@publishedPartner
@released
*/
class CAsyncRetrieveVariableLengthBufferV2 : public CActive
	{
protected:
	//
	// Start the retrieval
	//

	IMPORT_C void Start(TRequestStatus& aReqStatus, TDes8* aPhase1Request, TDes8* aPhase2Request);
	IMPORT_C CAsyncRetrieveVariableLengthBufferV2();
	IMPORT_C virtual ~CAsyncRetrieveVariableLengthBufferV2();

	IMPORT_C TBool CompleteIfInUse(TRequestStatus& aReqStatus);
	IMPORT_C void FreeBuffer();

private:
	IMPORT_C virtual void RestoreListL();
	virtual void Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2) = 0;
	virtual void CancelReq(TInt aIpc1,TInt aIpc2) = 0;

	IMPORT_C virtual void DoCancel();
	IMPORT_C virtual void RunL();

	void StartPhase2L();

protected:
	/**
	Maintains the current phase of buffer retrieval.
	*/
	enum {
		EIdle,
		ERetrievePhase1,
		ERetrievePhase2
		} iState;

	CBufBase* iResultsBuf;
	TPtr8 iResultsPtr;
	TInt iIpcPhase1;
	TInt iIpcPhase2;
	TInt iIpcCancel;
private:
	/**
	Pointer to the user's asynchronous request status object.
	*/
	TRequestStatus* iUserStatus;
	TDes8* iPhase1RequestData;
	TDes8* iPhase2RequestData;
	TPckgBuf<TInt> iBufferSize;
	};
	

//
/* following classes define interface to  TLV ( TYPE- LENGTH-VALUE) structured data:
-------------------------------------------------------------------------------------------------
|       |                         |             |        |                          |           |
|ItemId	| Length Of The Item Data |	Item Data	| ItemId |	Length Of The Item Data	| Item Data |
|       |                         |             |        |                          |           |
-------------------------------------------------------------------------------------------------
*/

/**
Defines interface for specifying the Tag of a TLV object.

@publishedPartner
@released
*/
class MTlvItemIdType
{
public:
/**Externalize object by serializing to provided descriptor*/
	virtual void ExternalizeL(TDes8& aBuffer) const =0;
/** Internalize object by de-serializing of data in provided buffer*/
	virtual void InternalizeL(TDesC8& aBuffer)=0;
/** The length of serialized data member */
	virtual TUint SerializedLength() const =0;
/**  compares whether two objects contains the same data*/
	virtual TBool IsEqual(const MTlvItemIdType&) const=0;
};

/**
Defines interface for specifying the Length of a TLV object.

@publishedPartner
@released
*/
class MTlvItemDataLengthType
{
public:
/**Externalize object by serializing to provided descriptor*/
	virtual void ExternalizeL(TDes8& aBuffer)const=0;
/** Internalize object by de-serializing of data in provided buffer*/
	virtual void InternalizeL(TDesC8& aBuffer)=0;
/** The length of serialized data member */
	virtual TUint SerializedLength() const=0;
/** Sets length of the data it relates to*/
	virtual void SetDataLength(TUint)=0;
/** Gets length of the data it relates to*/
	virtual TUint DataLength() const =0;
};

/**
Provides methods to append, remove or perform iterative lookup for items in container buffer.
Classes ItemIdType and ItemDataLengthType have to implement interfaces MTlvItemIdType and
MTlvItemDataLengthType in order to enable proper encoding and decoding of the first two fields
in the unit.

@publishedPartner
@released
*/
class TTlvStructBase
    {
  protected:
  	/** Default constructor initializes data members*/
   	IMPORT_C TTlvStructBase(TPtr8&,TUint8);    
 	/** Base class implementation of methods in the templated class*/
   	IMPORT_C TInt AppendItemL(MTlvItemIdType& aId,MTlvItemDataLengthType& aDataLengthType,const TPtr8& aData);
   	IMPORT_C TInt RemoveNextItemL(MTlvItemIdType& aIdToRemove,MTlvItemIdType& aId, MTlvItemDataLengthType& aDataLength);
   	IMPORT_C TInt AnyNextItemL(MTlvItemIdType& aIdFound,TPtr8& aData,MTlvItemIdType& aId, MTlvItemDataLengthType& aDataLength);
   	IMPORT_C TInt NextItemL(const MTlvItemIdType& aWantedId,TPtr8& aData,MTlvItemIdType& aId, MTlvItemDataLengthType& aDataLength);
  public:
	/** Sets position of the cursor to start position (0)*/
	IMPORT_C void ResetCursorPos();

  protected:
    /** Reference to external buffer that holds encoded TLV data*/
    TPtr8& iPtr;
    /** Cursor indicates last accessed item in the buffer*/
	TUint iCursorPos;
	/** Position in the buffer that indicates start of the zone that hasn't been assigned to any element.
	 	this free zone ends at the end of the buffer*/
	TUint iFreeSpacePos;
	/** Character used to populate the zone that hasn't been assigned to any element.
	this free zone ends at the end of the buffer*/
	TUint8 iFreeSpaceChar;
    };

/**
Provides methods to append, remove or perform iterative lookup for items in container buffer.
Classes ItemIdType and ItemDataLengthType have to implement interfaces MTlvItemIdType and 
MTlvItemDataLengthType in order to enable proper encoding and decoding of the first two fields 
in the unit. 

@publishedPartner
@released
*/  
template <class ItemIdType, class ItemDataLengthType>
class TTlvStruct: public TTlvStructBase
    {
 public:
 	
   	inline TTlvStruct(TPtr8&,TUint8);   
	inline TInt NextItemL(ItemIdType aId,TPtr8& aData);
	inline TInt AnyNextItemL(ItemIdType& aId, TPtr8& aData);
	inline TInt AppendItemL(ItemIdType aId,const TPtr8& aData);
	inline TInt RemoveNextItemL(ItemIdType aId);
	
 protected:
	/** Default constructor is protected in order to enforce proper initialization of reference to external data buffer via provided public constructor*/
    TTlvStruct();    	
    /** Type of the identifier*/
    ItemIdType iItemIdType;
    /** The type used to define length of data portion of the item*/ 
    ItemDataLengthType 	iItemDataLengthType;     
};

/** 
Default constructor initializes data members and cursor position to 0.

@param aPtr Ptr descriptor to TLV buffer that is to be read or written to.
@param aFreeSpaceChar Character used to populate the zone that hasn't been assigned to any element.
*/
template <class ItemIdType,class ItemDataLengthType>
	TTlvStruct<ItemIdType,ItemDataLengthType>::TTlvStruct(TPtr8& aPtr,TUint8 aFreeSpaceChar):TTlvStructBase(aPtr,aFreeSpaceChar)
	{	
	}
	
/**
Look up in the buffer for an item with specified identifier.
Look-up starts from the position of the cursor; 					
Returns KErrNone if the item is present in the buffer, KErrNotFound otherwise.
Sets supplied pointer so that it has length of item's data portion and points to it.
Internal cursor is moved to first position after the end of the found item 
(subsequent item start position in the buffer).

@param aId Id of item to find.
@param aData Descriptor which will hold the found item.
@return System-wide error code.. If item of requested Id was not found then KErrNotFound will be returned. 
*/
template <class ItemIdType, class ItemDataLengthType>
TInt TTlvStruct<ItemIdType,ItemDataLengthType>::NextItemL(ItemIdType aId,TPtr8& aData)
	{
	return TTlvStructBase::NextItemL(aId,aData,iItemIdType,iItemDataLengthType);	
	}

/**
Look up in the buffer for the item with specified identifier.
Look-up starts from the position of the cursor in the buffer. 
			
Returns KErrNone if item is found, KErrNotFound otherwise (end of buffer is reached).
Sets supplied pointer so that it points to item data portion and has length set to value of data length. 
Internal cursor is moved to first position after the end of the found item (subsequent item start position in the buffer).

@param aId Id of found item.
@param aData Descriptor which will hold the found item.
@return System-wide error code.. If no next item found then KErrNotFound will be returned. 
*/
template <class ItemIdType, class ItemDataLengthType>
TInt TTlvStruct<ItemIdType,ItemDataLengthType>::AnyNextItemL(ItemIdType& aId,TPtr8& aData)
	{
	return TTlvStructBase::AnyNextItemL(aId,aData,iItemIdType,iItemDataLengthType);
	}		
	
/**
Removes item identified by specified identifier (aId) from the buffer, where look-up starts at current cursor position, or 0 if it's reset. 
returns KErrNone if item is found ( and removed), otherwise error code - in the case where there is no more space in the assigned buffer, KErrOverflow is passed back. 

@param aId Id of item to remove.
@return System-wide error code.. If item of requested Id was not found then KErrNotFound will be returned. 
*/	
template <class ItemIdType, class ItemDataLengthType>
TInt TTlvStruct<ItemIdType,ItemDataLengthType>::RemoveNextItemL(ItemIdType aId)
	{
	return TTlvStructBase::RemoveNextItemL(aId,iItemIdType,iItemDataLengthType);	
	}
	
/**
Adds item identified by supplied aId argument to the buffer; content of the item is copied from provided descriptor to the buffer. 
Supplied item identifier (aId) and length of the descriptor are used to set item identifier field and length field at the start of 
item unit within the buffer.
Returns KErrNone if successful, error code otherwise.	
Internal cursor is moved to first position after the end of the found item (subsequent item start position in the buffer).

@param aId Id of item to add.
@param aData Descriptor containing data to add.
@return System-wide error code.. If size of item to be appended is greater than free space in buffer then KErrOverflow will be returned.
*/	
template <class ItemIdType, class ItemDataLengthType>
TInt TTlvStruct<ItemIdType,ItemDataLengthType>::AppendItemL(ItemIdType aId,const TPtr8& aData)
	{
	return TTlvStructBase::AppendItemL(aId,iItemDataLengthType,aData);
	}
#endif
