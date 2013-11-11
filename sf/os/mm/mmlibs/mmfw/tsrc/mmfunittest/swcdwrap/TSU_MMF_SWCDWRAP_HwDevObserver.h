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

#ifndef __TSU_MMF_SWCDWRAP_HWDEVOBSERVER_H__
#define __TSU_MMF_SWCDWRAP_HWDEVOBSERVER_H__

#include <mmf/server/mmfswcodecwrapper.h>

class CTestMMFHwDeviceObserver : public CBase, public MMMFHwDeviceObserver
	{
public:
	CTestMMFHwDeviceObserver(CMMFHwDevice* aHwDevice);
	~CTestMMFHwDeviceObserver();

	//MMFHwDeviceObserver
	virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt MsgFromHwDevice(TUid aMessageType, const TDesC8& aMsg);
	virtual void Stopped();
	virtual void Error(TInt aError=KErrNone);

	//callback from CTestFileReader
	virtual void FileReadComplete(TInt aError);

	//callback from CTestFileWriter
	virtual void FileWriteComplete(TInt aError);

	TInt ErrorStatus();

	TUint NumberOfBuffersObserved();
protected:
	void StopActiveScheduler();
protected:
	class CTestFileReadWriter : public CActive
		{
	public:
		CTestFileReadWriter(RFile& aFile, CTestMMFHwDeviceObserver* aParent)
			: CActive(EPriorityStandard), 
			iFile(aFile),
			iParent(aParent){CActiveScheduler::Add(this);};
		~CTestFileReadWriter() {Cancel();};

		//CActive
		virtual void RunL() {};
		virtual void DoCancel() {};

	protected:
		RFile iFile;
		CTestMMFHwDeviceObserver* iParent;
		CMMFDataBuffer* iDataBuffer;
		TUint iPosition;
		};
	class CTestFileReader : public CTestFileReadWriter
		{
	public:
		CTestFileReader(RFile& aSourceFile, CTestMMFHwDeviceObserver* aParent)
			: CTestFileReadWriter(aSourceFile, aParent) {};
		TInt Read(CMMFDataBuffer& aBuffer);

		//CActive
		virtual void RunL();
		virtual void DoCancel();
		};

	class CTestFileWriter : public CTestFileReadWriter
		{
	public:
		CTestFileWriter(RFile& aSinkFile, CTestMMFHwDeviceObserver* aParent)
			: CTestFileReadWriter(aSinkFile, aParent) {};
		TInt Write(CMMFDataBuffer& aBuffer);

		//CActive
		virtual void RunL();
		virtual void DoCancel();
		};
protected:
	TInt iError;
	CMMFHwDevice* iHwDevice;
	CMMFDataBuffer* iCurrentHwDeviceDataBuffer;
	TUint iNumberOfBuffers;
	TUint iCurrentBufferNumber;
	RFs iFileServerSession;
	};

#endif /* __TSU_MMF_SWCDWRAP_HWDEVOBSERVER_H__ */
