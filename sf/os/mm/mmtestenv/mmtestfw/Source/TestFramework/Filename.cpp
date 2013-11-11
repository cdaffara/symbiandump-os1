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
//

#include "Filename.h"

/**
 *
 * CFileName static constructor.
 *
 * @xxxx
 *
 */
CFileName* CFileName::NewL()
	{
	CFileName* s = CFileName::NewLC();
	CleanupStack::Pop();
	return s;
	}

/**
 *
 * CFileName static constructor.
 *
 * @xxxx
 *
 */
CFileName* CFileName::NewLC()
	{
	CFileName* s = new(ELeave) CFileName;
	CleanupStack::PushL(s);
	return s;
	}

/**
 *
 * CFileName assignment operator.
 *
 * @xxxx
 *
 */
TFileName& CFileName::operator=(const TText* aString)
	{
	iBuf = aString;
	return iBuf;
	}

/**
 *
 * CFileName assignment operator.
 *
 * @xxxx
 *
 */
TFileName& CFileName::operator=(const TDesC& aDes)
	{
	iBuf = aDes;
	return iBuf;
	}

/**
 *
 * CFileName assignment operator.
 *
 * @xxxx
 *
 */
TFileName& CFileName::operator=(const TPtrC16& aPtr)
	{
	iBuf = aPtr;
	return iBuf;
	}

/**
 *
 * CFileName assignment operator.
 *
 * @xxxx
 *
 */
TFileName& CFileName::operator=(const TFileName& aBuf)
	{
	iBuf = aBuf;
	return iBuf;
	}

/**
 *
 * CFileName assignment operator.
 *
 * @xxxx
 *
 */
TFileName& CFileName::operator=(const CFileName& aFilename)
	{
	iBuf = aFilename.FileName();
	return iBuf;
	}

/**
 *
 * CFileName copy function wrapper
 *
 * @xxxx
 *
 */
void CFileName::Copy(const TDesC8 &aDes)
	{
	iBuf.Copy(aDes);
	}

/**
 *
 * CFileName copy function wrapper
 *
 * @xxxx
 *
 */
void CFileName::Copy(const TDesC16 &aDes)
	{
	iBuf.Copy(aDes);
	}

/**
 *
 * CFileName copy function wrapper
 *
 * @xxxx
 *
 */
void CFileName::Copy(const TUint16 *aBuf,TInt aLength)
	{
	iBuf.Copy(aBuf,aLength);
	}

/**
 *
 * CFileName copy function wrapper
 *
 * @xxxx
 *
 */
void CFileName::Copy(const TUint16 *aString)
	{	
	iBuf.Copy(aString);
	}

/**
 *
 * CFileName location function wrapper
 *
 * @xxxx
 *
 */
TInt CFileName::Locate(TChar aChar) const
	{
	return iBuf.Locate(aChar);
	}

/**
 *
 * CFileName left function wrapper
 *
 * @xxxx
 *
 */
TPtrC16 CFileName::Left(TInt aLength) const
	{
	return iBuf.Left(aLength);
	}

/**
 *
 * CFileName accessor : file name
 *
 * @xxxx
 *
 */
TFileName CFileName::FileName() const
{ 
	return iBuf;
}


