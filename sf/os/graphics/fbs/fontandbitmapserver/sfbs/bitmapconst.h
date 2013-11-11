// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BITMAPCONST_H
#define BITMAPCONST_H

/**
@file
@internalComponent
@released
*/

const TInt KCompressionBookMarkThreshold=0x2000;
const TInt KNumBytesPerBitmapHandle = sizeof(TInt); 						// Bitmap handle is the size of a TInt 

class TRgb24bit
	{
public:
	TUint8 iRed;
	TUint8 iGreen;
	TUint8 iBlue;
	};

#endif	// BITMAPCONST_H
