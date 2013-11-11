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
//

/**
 @file
 @internalAll 
*/

class CPtrHolder : public CBase
/**
Contains the TPtr's used by asynchronous client-side functions.
Used by Client-side code to ensure BC by keeping size of Rxxx classes constant even if a 
new function has to be added.
@internalComponent
*/
	{
public:
	static CPtrHolder* NewL(const TInt aSizeOfPtrArray,const TInt aSizeOfPtrCArray=0);
	~CPtrHolder();
public:
	TPtr8& Ptr(const TInt aIndex);
	TPtrC8& PtrC(const TInt aIndex);
	virtual RCall::TCallParamsPckg* CallParamsPckg();
	// kludgy but cannot increase size of RCall by adding a member pointer to CCallPtrHolder,
	// as it would break BC, so use RTelSubSessionBase::iPtrArray to access the extra member data.
protected:
	virtual void ConstructL(const TInt aSizeOfPtrArray,const TInt aSizeOfPtrCArray);	
	CPtrHolder();
protected:
	RArray<TPtr8> iPtrArray;
	RArray<TPtrC8> iPtrCArray;
	};

class CCallPtrHolder : public CPtrHolder
/**
@internalComponent
*/
	{
public:
	static CCallPtrHolder* NewL(const TInt aSizeOfPtrArray,const TInt aSizeOfPtrCArray=0);
	~CCallPtrHolder();
public:
	virtual RCall::TCallParamsPckg* CallParamsPckg();
protected:
	CCallPtrHolder();
protected:
	RCall::TCallParams iCallParams;
	RCall::TCallParamsPckg iCallParamsPckg;
	};

