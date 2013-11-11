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

#ifndef __RESPBUILDER_H
#define __RESPBUILDER_H

#include <e32base.h>


// Not intended for derivation
class CRespBuilder : public CBase
	{
public:
	static CRespBuilder* NewL();
	~CRespBuilder();
	
	void BuildResponseL(TInt aErrorCode, TUint aMsgNum, HBufC8*& aRespBuff, CDesCArrayFlat* aRespArgArray);
	
private:
	void ConstructL();
	const TDesC8& ErrorCodeAscii(TInt aErrorCode);
	};



#endif
