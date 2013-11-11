// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HCIINIFILE_H
#define HCIINIFILE_H

/**
@file
@internalComponent
*/

#include <e32base.h>

/**
 Defines an interface to access the ini data file
 */
NONSHARABLE_CLASS(CIniFileData) : public CBase 
	{
public:
	// Constructor, pass in name of ini file to open
	// Default search path is '\private\101F7989\Bluetooth'
	// on target filesystem ie.
	// 'NewL(_L("ttools.ini"))' is equivalent to
	// 'NewL(_L("\\private\\101F7989\\Bluetooth\\ttools.ini"))'
	// with drives searched in standard F32 order.
	static CIniFileData* NewL(const TDesC& aName);
	virtual ~CIniFileData();
	TBool FindVar(const TDesC &aSectName,const TDesC &aKeyName,TPtrC &aResult);

private:
	CIniFileData();
	void ConstructL(const TDesC& aName);

private:
	HBufC* iName;
	HBufC* iToken;
	TPtr iPtr;
	};

#endif // HCIINIFILE_H
