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

#ifndef __TSU_MMF_SWCDWRAP_RECORDHWDEVOBSERVER_H__
#define __TSU_MMF_SWCDWRAP_RECORDHWDEVOBSERVER_H__

#include "TSU_MMF_SWCDWRAP_HwDevObserver.h"

class CTestRecordMMFHwDeviceObserver : public CTestMMFHwDeviceObserver
	{
public:
	CTestRecordMMFHwDeviceObserver(CMMFHwDevice* aHwDevice);
	~CTestRecordMMFHwDeviceObserver();

	//MMFHwDeviceObserver
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual void Stopped();

	TInt WriteBufferDirectToFileAsync(const TDesC& aFileName, TUint aNumberOfBuffers);

	//callback from CTestFileWriter
	virtual void FileWriteComplete(TInt aError);

	//gets Write File Handle
	RFile& WriteFile();
	
private:
	CTestFileWriter* iFileWriter;
	RFile iSinkFile;
	};

#endif /* __TSU_MMF_SWCDWRAP_RECORDHWDEVOBSERVER_H__ */
