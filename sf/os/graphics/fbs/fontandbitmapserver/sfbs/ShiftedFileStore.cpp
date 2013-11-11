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
//

#include "ShiftedFileStore.h"

/**
@internalComponent
CShiftedFileStore::OpenLC(RFs& aFs, const TDesC& aName, TUint aFileMode, TUint aFileOffset) method.
CFileStore::OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode,const TFileStoreFactoryFunction aFactory[])
method was used as an origin for the method's source code.
*/
CShiftedFileStore* CShiftedFileStore::OpenLC(RFs& aFs, const TDesC& aName, TUint aFileMode, TUint aFileOffset)
	{
	RFile file;
	User::LeaveIfError(file.Open(aFs, aName, aFileMode));
	CShiftedFileStore* store = FromL(file, aFileOffset);
	CleanupStack::PushL(store);
	return store;
	}

/**
@internalComponent
CShiftedFileStore::FromL(RFile& aFile, TUint aFileOffset) method.
CFileStore::FromL(RFile& aFile,TFileStoreFactoryFunction aFunction) method was used as an origin
for the method's source code. The only difference is that RFileBuf::Attach is called with possibly
non-zero offset. The original implementation always calls RFileBuf::Attach with zero offset.
*/
CShiftedFileStore* CShiftedFileStore::FromL(RFile& aFile, TUint aFileOffset)
	{
	if (static_cast<TInt>(aFileOffset) < 0)
		User::Leave(KErrArgument);
	RFileBuf buf;
	buf.Attach(aFile, static_cast <TInt> (aFileOffset));
	buf.PushL();
	RReadStream stream(&buf);
	TCheckedUid chk;
	stream >> chk;
	CFileStore* store = (*KDirectFileStoreFactoryFunction)(buf, chk.UidType());
	if(store == NULL)
		User::Leave(KErrNotSupported);
	//CleanupStack::Pop(2) is called, because:
	//1) buf.PushL() - actually MStreamBuf::PushL() is called and it pushes buf 
	//                 into CleanupStack.
	//1) CFileStore* store = (*KDirectFileStoreFactoryFunction)(buf, chk.UidType()) -
	//                 FileStoreFactory::DirectLC() is called.
	CleanupStack::Pop(2);
	return static_cast <CShiftedFileStore*> (store);
	}

/**
@internalComponent
Constructor. Prevents compiler warnings.
Actually, CShiftedFileStore instance is newer created. CFileStore or CDirectFileStore
instance is created and downcasted to CShiftedFileStore. It is possible to do that:
CShiftedFileStore doesn't have data members at all.
*/
CShiftedFileStore::CShiftedFileStore(RFile& aFile) :
	CDirectFileStore(aFile)
	{
	}

