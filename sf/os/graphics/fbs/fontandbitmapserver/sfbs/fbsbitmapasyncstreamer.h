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

#ifndef FBSBITMAPASYNCSTREAMER_H
#define FBSBITMAPASYNCSTREAMER_H

/**
@file
@internalComponent
@released
 */

const TUint32 KFontBitmapServerUidValue = 0x10003a16;
const TUid KFontBitmapServerUid = { KFontBitmapServerUidValue };

class RFbsSession;
class CDirectFileStore;

/**
@deprecated
*/
class CFbsBitmapAsyncStreamer : public CBase
	{
public:
	enum TMode {ELoad, ESave};
public:
	IMPORT_C ~CFbsBitmapAsyncStreamer();
	IMPORT_C static CFbsBitmapAsyncStreamer* NewL(TMode aMode);
	IMPORT_C TInt Load(const TDesC& aFilename,TInt32 aId,TInt& aScanLines);
	IMPORT_C TBool LoadScanLinesL(TInt aNumberOfScanLines,CFbsBitmap*& aBitmap);
	IMPORT_C TInt Save(const TDesC& aFilename,CFbsBitmap* aBitmap,TInt32& aId,TInt& aScanLines);
	IMPORT_C TBool SaveScanLinesL(TInt aNumberOfScanLines);
private:
	CFbsBitmapAsyncStreamer(TMode aMode);
	void ConstructL();
	void DoLoadL(const TDesC& aFilename,TInt32 aId);
	void DoSaveL(RFile& aFile);
private:
	RFbsSession* iFbs;
	RStoreReadStream iReadStream;
	RStoreWriteStream iWriteStream;
	TStreamId iId;
	CDirectFileStore* iStore;
	TInt iCurrentScanLine;
	TUint32* iScanLineBase;
	CFbsBitmap* iBitmap;
	SEpocBitmapHeader iHeader;
	TDisplayMode iDispMode;
	TMode iMode;
	};

#endif // FBSBITMAPASYNCSTREAMER_H
