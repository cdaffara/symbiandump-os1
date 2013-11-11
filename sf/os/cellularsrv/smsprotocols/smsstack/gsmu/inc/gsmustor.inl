// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements inline methods for classes TSAREntry and CSARStore
// 
//

/**
 @file
*/

#include <s32file.h>

/**
 *  Gets the Reference field.
 *  
 *  @return Concatenation reference 
 */
inline TInt TSAREntry::Reference() const
	{
	return iReference;
	}


/**
 *  Sets the Reference field.
 *  
 *  @param aReference The concatenation reference 
 */
inline void TSAREntry::SetReference(TInt aReference)
	{
	iReference=aReference;
	}


/**
 *  Gets the Total field.
 *  
 *  @return The total number of PDUs in SMS message 
 */
inline TInt TSAREntry::Total() const
	{
	return iTotal;
	}


/**
 *  Sets the total number of PDUs in SMS message
 *  
 *  @param aTotal The total number of PDUs in the message 
 */
inline void TSAREntry::SetTotal(TInt aTotal)
	{
	iTotal=aTotal;
	}


/**
 *  Gets the Count field.
 *  
 *  @return Number of PDUs sent/received 
 */
inline TInt TSAREntry::Count() const
	{
	return iCount;
	}


/**
 *  Sets the Count field.
 *  
 *  @param aCount The number of PDUs sent/received 
 */
inline void TSAREntry::SetCount(TInt aCount)
	{
	iCount=aCount;
	}


/**
 *  Tests if the whole message is sent/received.
 *  
 *  @return True if the whole message is sent/received 
 */
inline TBool TSAREntry::IsComplete() const
	{
	return iCount==iTotal;
	}


/**
 *  Gets the Description1 field: a pointer to the first
 *  32 characters from buffer.
 *  
 *  @return Description1 field 
 */
inline TPtrC TSAREntry::Description1() const
	{
	return iDescription1;
	}


/**
 *  Sets the Description1 field: the first 32 characters in buffer.
 *  
 *  @param aDescription Description1 field 
 */
inline void TSAREntry::SetDescription1(const TDesC& aDescription)
	{
	iDescription1=aDescription;
	}


/**
 *  Gets the Description2 field: a pointer to the original address.
 *  
 *  @return Description2 field 
 */
inline TPtrC TSAREntry::Description2() const
	{
	return iDescription2;
	}


/**
 *  Sets the Description2 field: the original address.
 *  
 *  @param aDescription Description2 field 
 */
inline void TSAREntry::SetDescription2(const TDesC& aDescription)
	{
	iDescription2=aDescription;
	}


/**
 *  Gets the Data3 field.
 *  
 *  @return Data3 field 
 */
inline TInt TSAREntry::Data3() const
	{
	return iData3;
	}


/**
 *  Sets the Data3 field.
 *  
 *  @param aData Data3 field 
 */
inline void TSAREntry::SetData3(TInt aData)
	{
	iData3=aData;
	}


/**
 *  Gets the Time field: the time when this message was sent/received.
 *  
 *  @return Time field 
 */
inline const TTime& TSAREntry::Time() const
	{
	return iTime;
	}


/**
 *  Sets the Time field: the time when this message was sent/received.
 *  
 *  @param aTime Time field 
 */
inline void TSAREntry::SetTime(const TTime& aTime)
	{
	iTime=aTime;
	}


/**
 *  Gets the data stream ID.
 *  
 *  @return Data stream ID 
 */
inline TStreamId TSAREntry::DataStreamId() const
	{
	return iDataStreamId;
	}


/**
 *  Sets the data stream ID.
 *  
 *  @param aStreamId Data stream ID 
 */
inline void TSAREntry::SetDataStreamId(TStreamId aStreamId)
	{
	iDataStreamId=aStreamId;
	}


/**
 *  @return ETrue if this message is deleted
 */
inline TBool TSAREntry::IsDeleted() const
	{
	return iFlags&ESAREntryIsDeleted;
	}


/**
 *  Sets if this message should be marked as deleted
 *  
 *  @param aIsDeleted Set to True to mark a message as deleted
 */
inline void TSAREntry::SetIsDeleted(TBool aIsDeleted)
	{
	iFlags=aIsDeleted?iFlags|ESAREntryIsDeleted: iFlags&(~ESAREntryIsDeleted);
	}


/**
 *  @return ETrue if this message is added
 */
inline TBool TSAREntry::IsAdded() const
	{
	return iFlags&ESAREntryIsAdded;
	}


/**
 *  Sets if this message should be marked as added
 *  
 *  @param aIsDeleted Set to True to mark a message as added
 */
inline void TSAREntry::SetIsAdded(TBool aIsAdded)
	{
	iFlags=aIsAdded?iFlags|ESAREntryIsAdded: iFlags&(~ESAREntryIsAdded);
	}


/**
 *  Gets the file server handle.
 *  
 *  @return File server handle 
 */
inline RFs& CSARStore::FileSession() const
	{
	return iFs;
	}


/**
 *  @return whether it's in transaction
 */
inline TBool CSARStore::InTransaction() const
	{
	return iInTransaction;
	}
