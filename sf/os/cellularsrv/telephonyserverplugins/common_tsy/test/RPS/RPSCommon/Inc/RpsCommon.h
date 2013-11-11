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

/**
 @file
 @internalComponent
*/
 
#ifndef __RPSMSG_H__
#define __RPSMSG_H__


#include <badesca.h>


class CRpsMsg : public CBase
  	{
public:
	IMPORT_C static CRpsMsg* NewL (const TDesC8& aMsgType, const TDesC8& aOpCode, const TUint aMsgNum, const CDesCArrayFlat& aArgArray);
	IMPORT_C static CRpsMsg* NewLC(const TDesC8& aMsgType, const TDesC8& aOpCode, const TUint aMsgNum, const CDesCArrayFlat& aArgArray);
	
	IMPORT_C static CRpsMsg* NewL();
	IMPORT_C static CRpsMsg* NewLC();

	IMPORT_C ~CRpsMsg();
	
public:
	
	// buf to array
	IMPORT_C TInt InternalizeL(const TDesC8& aBufIn);
	
	// array to buf
	IMPORT_C void ExternalizeL(HBufC8*& aBufOut);
	IMPORT_C TInt ArraySize() const;
	IMPORT_C TPtrC8 GetArgL(TInt aArgNo);
	IMPORT_C TPtrC8 GetRawDataL(TInt aOffset);
	IMPORT_C TPtrC8 GetOpCode() const;
	IMPORT_C TPtrC8 GetMsgType() const;
	IMPORT_C TInt GetMsgNum() const;
	

private:

	void ConstructL(const TDesC8& aMsgType, const TDesC8& aOpCode, const TUint aMsgNum, const CDesCArrayFlat& aArgArray);
	void ConstructL();
	
private:
	
	CDesC8ArrayFlat*	iMsgDataArray;
	};
	
#endif // __RPSMSG_H__
