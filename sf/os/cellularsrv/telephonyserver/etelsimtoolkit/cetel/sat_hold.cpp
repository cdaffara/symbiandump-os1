// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CSatPtrHolder implementation
// CSatPtrHolder - class owned by SAT Rxxx objects to contain the TPtr8's and other member
// data required by asynchronous functions so that any additional functions will not require 
// breaking BC.
// 
//

/**
 @file
*/

#include "etelext.h"

// Sat header files
#include "Etelsat.h"
#include "Satptr.h"

//
// CSatPtrHolder - class owned by SAT Rxxx objects to contain the TPtr8's and other member
// data required by asynchronous functions so that any additional functions will not require 
// breaking BC.
//

CSatPtrHolder* CSatPtrHolder::NewL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * Two phase constructor.
 *
 * @param aSizeOfPtrArray Size of the array containing the pointers.
 */	{
	CSatPtrHolder* p = new (ELeave) CSatPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CSatPtrHolder::CSatPtrHolder()
	{}

CSatPtrHolder::~CSatPtrHolder()
	{
	iPtrArray.Close();
	iPtrCArray.Close();
	}

void CSatPtrHolder::ConstructL(TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * This method constructs the two arrays: one to hold TPtr8's and the other TPtrC8's
 */
	{
	TPtr8 ptr(reinterpret_cast<TUint8 *>(NULL),0);
	TInt i;
	for (i=0;i<aSizeOfPtrArray;i++)
		User::LeaveIfError(iPtrArray.Append(ptr));
		TPtrC8 ptrC(NULL,0);
	for (i=0;i<aSizeOfPtrCArray;i++)
		User::LeaveIfError(iPtrCArray.Append(ptrC));
	}



