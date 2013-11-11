// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMCOMMANDLISTRESOURCEREADER_H__
#define __SSMCOMMANDLISTRESOURCEREADER_H__

#include <e32base.h>
#include <f32file.h>

#include <ssm/ssmstate.h>
#include <ssm/ssmswp.h>

class CSsmCommandList;
class MSsmConditionalCallback;
class CSsmCommandListResourceReaderImpl;

/** Constructs command lists from resource files.

A resource reader must be initialised before it can be used to
construct a command list.

Once initialised, a command list may be constructed by first calling
one of the overloads of PrepareCommandList, followed by GetCommandList
to return the command list.

Command lists may only be constructed one at a time.

@publishedPartner
@released
*/
NONSHARABLE_CLASS(CSsmCommandListResourceReader) : public CBase
	{
public:
	IMPORT_C static CSsmCommandListResourceReader* NewL(RFs& aFs, const TDesC& aCommandListPath, MSsmConditionalCallback& aConditionalCallback);
	IMPORT_C ~CSsmCommandListResourceReader();

	IMPORT_C void Initialise(TRequestStatus& aStatus);
	IMPORT_C void InitialiseCancel();
	IMPORT_C void PrepareCommandList(TInt aCommandListId, const TSsmState& aState, TRequestStatus& aStatus);
	IMPORT_C void PrepareCommandList(TInt aCommandListId, const TSsmSwp& aSwp, TRequestStatus& aStatus);
	IMPORT_C void PrepareCommandListCancel();
	IMPORT_C CSsmCommandList* GetCommandList();
	IMPORT_C void GetCommandListIdsL(RArray<TInt>& aArray) const;

private:
	CSsmCommandListResourceReader();
	void ConstructL(RFs& aFs, const TDesC& aCommandListPath, MSsmConditionalCallback& aConditionalCallback);

private:
	CSsmCommandListResourceReaderImpl* iImpl;
	CSsmCommandList* iCommandList;
	};

#endif // __SSMCOMMANDLISTRESOURCEREADER_H__
