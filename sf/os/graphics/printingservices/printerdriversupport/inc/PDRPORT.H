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

#ifndef PDRPORT_H
#define PDRPORT_H

#include <e32std.h>
#include <c32comm.h>
#include <gdi.h>
#include <s32file.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <printerdriversupport/printerport.h>
#endif

/**
 @publishedAll
 @released 
 */
class CFilePrinterPort : public CPrinterPort
	{
public:
	IMPORT_C static CFilePrinterPort* NewL(const TDesC& aFileName);
	IMPORT_C ~CFilePrinterPort();
	void WriteRequest(const TDesC8& aBuf,TRequestStatus& aRequestStatus);
	void Cancel();
	inline const TDesC& FileName() { return iFileName; }
private:
	void ConstructL();
	CFilePrinterPort(const TDesC& aFileName);
private:
	RFs iFs;
	TFileName iFileName;
	RFile iFile;
	TBool iCancelled;
	};

#endif
