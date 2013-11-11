// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "rotatingstrbuf.h"

CRotatingStrBuf* CRotatingStrBuf::NewL(TInt aSize)
	{
	CRotatingStrBuf *self = new (ELeave) CRotatingStrBuf;
	CleanupStack::PushL(self);
	self->ConstructL(aSize);
	CleanupStack::Pop();
	return self;
	}
	
CRotatingStrBuf::~CRotatingStrBuf()
	{
	TEntry* entry;
	while (!iQueue.IsEmpty())
		{
		entry = iQueue.First();
		entry->iLink.Deque();
		delete entry->iText;
		delete entry;
		}
	}

CRotatingStrBuf::CRotatingStrBuf()
	:iQueue(_FOFF(TEntry,iLink))
	{
	}

void CRotatingStrBuf::ConstructL(TInt aSize)
	{
	for(TInt cnt=0; cnt<aSize; cnt++)
		{
		TEntry* entry = new (ELeave) TEntry;
		entry->iText = NULL;
		iQueue.AddLast(*entry);
		}
	}

void CRotatingStrBuf::Put(const TDesC& aText)
	{
	TEntry* entry = iQueue.Last();
	// move last entry to the top of the queue
	entry->iLink.Deque();
	iQueue.AddFirst(*entry);
	// alocate/resize buffer
	if (entry->iText == NULL)
		entry->iText = HBufC::New(aText.Length());
	else if (entry->iText->Des().MaxLength() < aText.Length())
		entry->iText = entry->iText->ReAlloc(aText.Length());
	// copy the text
	if (entry->iText != NULL) // New or ReAlloc may have fail.
		*entry->iText = aText;
	}

HBufC* CRotatingStrBuf::Get()
	{
	TDblQueIter<TEntry> iter(iQueue);
	iter.SetToLast();

	TEntry* entry;
	while ((entry = iter--) != NULL && entry->iText == NULL)
		{};
	
	if (entry != NULL)
		{
		HBufC* ret = entry->iText;
		entry->iText = NULL;
		return ret;
		}
	return NULL;
	}
	
