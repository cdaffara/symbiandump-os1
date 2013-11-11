// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Inline definitions for CSmsSegmentationStore.
// 
//

/**
 @file
 @internalAll 
*/

#include <gsmuelem.h>


inline TBool CSmsSegmentationStore::IsPermanentStatus(TInt aStatus)
	{
	//
	// The question is are the temporary2 errors really permanent in every case?
	// Maybe this is depending from the SC as well?
	// Or the problem is with some kind of timeouts due to our performance problems
	// The log file will show it!
	// the theoretical assumption is that the failed counter had be incremented
	// but the recipient received the message
	//
	return (aStatus == TSmsStatus::ESmsShortMessageReceivedBySME  ||
			(aStatus >= TSmsStatus::ESmsPermErrorRemoteProcedureError  &&
		     aStatus <= TSmsStatus::ESmsPermErrorDoesNotExist));
	} // CSmsSegmentationStore::IsPermanentStatus


inline CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::TSmsSegmentationStoreRefStatus(TInt aRef,
																							 TInt aStatus /* = ENullStatus */)
  : iReference(aRef),
    iStatus(aStatus)
	{
	// NOP
	} // CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::TSmsSegmentationStoreRefStatus


inline TInt CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::Reference() const
	{
	return iReference;
	} // CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::Reference


inline TInt CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::Status() const
	{
	return iStatus;
	} // CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::Status


inline void CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::SetStatus(TInt aStatus)
	{
	iStatus = aStatus;
	} // CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::SetStatus


inline CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::RSmsSegmentationStoreRefStatusArray()
  : RArray<CSmsSegmentationStore::TSmsSegmentationStoreRefStatus>()
	{
	// NOP
	} // CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::RSmsSegmentationStoreRefStatusArray

