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

#ifndef __TSU_MMF_SWCDWRAP_CONVERTHWDEVOBSERVER_H__
#define __TSU_MMF_SWCDWRAP_CONVERTHWDEVOBSERVER_H__

#include "TSU_MMF_SWCDWRAP_HwDevObserver.h"

class CTestConvertMMFHwDeviceObserver : public CTestMMFHwDeviceObserver
	{
public:
	CTestConvertMMFHwDeviceObserver(CMMFHwDevice* aHwDevice);
	~CTestConvertMMFHwDeviceObserver();

	//MMFHwDeviceObserver
	virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual void Stopped();

	TInt SetConvertFiles(const TDesC& aSourceFileName, const TDesC& aSinkFileName, TUint aNumberOfBuffers);

	//callback from CTestFileReader
	virtual void FileReadComplete(TInt aError);

	//callback from CTestFileWriter
	virtual void FileWriteComplete(TInt aError);

	//gets Read File handle
	RFile& ReadFile();

	//gets Write File Handle
	RFile& WriteFile();

private:
	TInt FillThisHwBufferFromFile(CMMFDataBuffer& aHwDataBuffer);
private:
	CTestFileReader* iFileReader;
	CTestFileWriter* iFileWriter;
	RFile iSourceFile;
	RFile iSinkFile;
	};

#endif /* __TSU_MMF_SWCDWRAP_CONVERTHWDEVOBSERVER_H__ */
