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

#ifndef __TSU_MMF_SRSSNK_H__
#define __TSU_MMF_SRSSNK_H__

#include <e32math.h>

#include <testframework.h>
#include <mmf/server/mmfdatapath.h>

#include "CmmfTestControllerUIDs.hrh"
#include <mmf/plugin/mmfformatimplementationuids.hrh>
#include "TS_Codes.h"

#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfurl.h>
#include <mmf/server/mmfaudioinput.h>
#include <mmf/server/mmfaudiooutput.h>
#include "../../../../../mmplugins/mmfwplugins/src/Plugin/Format/MmfWAVFormat/mmfwavformat.h"

const TInt KTestBufferSize = 1032; // 1024 + 8
const TInt KTestDataSize = 51200; // 50K
const TInt KTestOverDataSize = KTestDataSize + 20;

const TInt KTestReadSize = KTestBufferSize/2;
const TInt KTestOverReadSize = KTestBufferSize + 10;

const TInt KTestWriteSize = KTestBufferSize/2;
const TInt KTestOverWriteSize = KTestBufferSize + 10;

class CTestSuite;
class CTestSuite_MMF_SRSSNK;

enum TSourceType
	{
	ESourceTypeDes,
	ESourceTypeFile,
	ESourceTypeFileHandle,
	ESourceTypeAudio
	};
	
enum TSinkType
	{
	ESinkTypeFile,
	ESinkTypeFileHandle
	};


class TThreadData
	{
public:
	TThreadData()
	: iFileSize(-1), iErr(KErrNone)
	{ }

public:
	CMMFFile* iFilePtr;
	MAsyncEventHandler *iEventHandler;
	TInt iFileSize;
	TInt iErr;
	};

const TUid KTestDummyUid = { -1 };
class CMMFTestBuffer : public CMMFBuffer
	{
public:
	CMMFTestBuffer()
	: CMMFBuffer(KTestDummyUid)
	{ }

	TUint BufferSize() const
	{ return 0; }
	};
	
	
MDataSink* CreateFileSinkL(TSinkType aSinkType, RFs& aFs, const TDesC& aFileName); 
MDataSource* CreateFileSourceL(TSourceType aSourceType, RFs& aFs, const TDesC& aFileName); 
MDataSource* CreateFileHandleSourceL(RFs& aFs, const TDesC& aFileName);

class CMMFDummySource : public CBase, public MDataSource, public MAsyncEventHandler
	{
public:
	CMMFDummySource();
	
	void SetExpectedEmptyCount(TInt aExpectedEmptyCount)
	{ iExpectedEmptyCount = aExpectedEmptyCount; }

	TBool BufferEmptiedOk()
		{
		TBool bufferEmptiedOk = iBufferEmptiedOk;
		if (bufferEmptiedOk)
			ResetBufferEmptiedOk();

		return bufferEmptiedOk;
		}

	void ResetBufferEmptiedOk()
	{ iBufferEmptiedOk = EFalse ; iEmptyCount = 0 ; iErrorCount = 0 ; iExpectedEmptyCount = 1; } 
	
	// From MDataSource
	TFourCC SourceDataTypeCode(TMediaId aMediaId);
	void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId);
	void BufferEmptiedL(CMMFBuffer* aBuffer);
	TBool CanCreateSourceBuffer();
	CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference);

	// MAsyncEventHandler
	TInt SendEventToClient(const TMMFEvent& aEvent);

	TInt Error()
	{ ResetBufferEmptiedOk() ; return iError; }

	TInt ErrorCount()
	{ return iErrorCount; }

	void SetFillBuffer()
	{ iFillBuffer = ETrue; }

	void SetActiveScheduler()
	{ iActiveScheduler = CActiveScheduler::Current(); }

	void ClearActiveScheduler()
	{ iActiveScheduler = NULL; }

	CMMFBuffer* MmfBuffer()
	{ return iBuffer; }

protected:
	void ConstructSourceL(const TDesC8& aInitData);

private:
	TFourCC iSourceDataTypeCode;
	TBool iBufferEmptiedOk;

	TInt iEmptyCount;
	TInt iExpectedEmptyCount;

	CActiveScheduler *iActiveScheduler;
	TInt iError;
	TInt iErrorCount;

	TBool iFillBuffer;

	CMMFBuffer* iBuffer;	// buffer returned in BufferEmptiedL callback
	};


class CMMFDummySink : public CBase, public MDataSink, public MAsyncEventHandler
	{
public:
	CMMFDummySink();
	
	void SetExpectedFillCount(TInt aExpectedFillCount)
	{ iExpectedFillCount = aExpectedFillCount; }

	TBool BufferFilledOk()
		{
		TBool bufferFilledOk = iBufferFilledOk;
		if (bufferFilledOk)
			ResetBufferFilledOk();

		return bufferFilledOk;
		}

	void ResetBufferFilledOk()
	{ iBufferFilledOk = EFalse ; iFillCount = 0 ; iErrorCount = 0 ; iExpectedFillCount = 1; }

	void BufferFilledError();
	
	// From MDataSink
	TFourCC SinkDataTypeCode(TMediaId aMediaId);
	void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId);
	void BufferFilledL(CMMFBuffer* aBuffer);
	TBool CanCreateSinkBuffer();
	CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool &aReference);

	// MAsyncEventHandler
	TInt SendEventToClient(const TMMFEvent& aEvent);

	TInt Error()
	{ ResetBufferFilledOk() ; return iError; }

	TInt ErrorCount()
	{ return iErrorCount; }

	CMMFBuffer* MmfBuffer()
	{ return iBuffer; }

protected:
	void ConstructSinkL(const TDesC8& aInitData);

private:
	TFourCC iSinkDataTypeCode;
	TBool iBufferFilledOk;

	TInt iFillCount;
	TInt iExpectedFillCount;

	CActiveScheduler *iActiveScheduler;
	TInt iError;
	TInt iErrorCount;

	CMMFBuffer* iBuffer;	// buffer returned in BufferFilledL callback
	};

class TTestStep
	{
public:
	TTestStep()
	: iSource(NULL), iSink(NULL)
	{ }

	TTestStep(MDataSource* aSource)
	: iSource(aSource), iSink(NULL)
	{ }
	
	TTestStep(MDataSink* aSink)
	: iSource(NULL), iSink(aSink)
	{ }

	TTestStep(MDataSource* aSource, MDataSink* aSink)
	: iSource(aSource), iSink(aSink)
	{ }

	virtual TVerdict DoTestStepL() = 0;

	virtual void DoTestPreambleL();
	virtual TVerdict DoTestPostambleL(TBool aCheck);
	virtual TVerdict DoNegativeTestPostambleL();
	virtual MDataSource* CreateSourceL(TSourceType aSourceType, HBufC8* aDes);

public:
	MDataSource* iSource;
	MDataSink* iSink;
	};


class TFileTestStep : public TTestStep
	{
public:
	TFileTestStep()
	: iFileFailureExpected(EFalse)
	{ }

	TFileTestStep(MDataSource* aSource)
	: TTestStep(aSource), iFileFailureExpected(EFalse)
	{ }
	
	TFileTestStep(MDataSink* aSink)
	: TTestStep(aSink), iFileFailureExpected(EFalse)
	{ }

	TFileTestStep(MDataSource* aSource, MDataSink* aSink)
	: TTestStep(aSource, aSink), iFileFailureExpected(EFalse)
	{ }

	TVerdict DoTestPostambleL(TBool aCheck);
	
public:
	TBool iFileFailureExpected;
	};

class TReadBufferTestStep : public TFileTestStep
	{
	public:
		TReadBufferTestStep(MDataSource* aSource, CMMFBuffer* aBuffer, TInt aPosition, const TDes8& aSourceDes)
		: TFileTestStep(aSource), iBuffer(aBuffer), iPosition(aPosition), iSourceDes(aSourceDes), iAsynchronousRead(EFalse)
		{ InitRandomNumberGenerator(); }

		TReadBufferTestStep(MDataSource* aSource, CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aSink, const TDes8& aSourceDes)
		: TFileTestStep(aSource, aSink), iBuffer(aBuffer), iPosition(aPosition), iSourceDes(aSourceDes), iAsynchronousRead(EFalse)
		{ InitRandomNumberGenerator(); }

		void InitRandomNumberGenerator()
			{
			TTime seedTime;
			seedTime.UniversalTime();
			iSeed = seedTime.Int64();
			}

		void SetPosition(TInt aPosition)
		{ iPosition = aPosition; }

		void SetAsynchronousRead(TBool aAsynchronousRead)
		{ iAsynchronousRead = aAsynchronousRead; }
		
		void RandomiseBuffer();
		TInt CheckBuffer();

	public:
		CMMFBuffer* iBuffer;
		TInt64 iSeed;

		TInt iPosition;
		const TDes8& iSourceDes;

		TBool iAsynchronousRead;
	};

class TWriteBufferTestStep : public TFileTestStep
	{
	public:
		TWriteBufferTestStep(MDataSink* aSink, CMMFBuffer* aBuffer)
		: TFileTestStep(aSink), iBuffer(aBuffer), iPosition(0), iAsynchronousWrite(EFalse)
		{ InitRandomNumberGenerator(); }

		TWriteBufferTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, TInt aPosition)
		: TFileTestStep(aSink), iBuffer(aBuffer), iPosition(aPosition), iAsynchronousWrite(EFalse)
		{ InitRandomNumberGenerator(); }

		TWriteBufferTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, MDataSource* aSource)
		: TFileTestStep(aSource, aSink), iBuffer(aBuffer), iPosition(0), iAsynchronousWrite(EFalse)
		{ InitRandomNumberGenerator(); }

		TWriteBufferTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSource)
		: TFileTestStep(aSource, aSink), iBuffer(aBuffer), iPosition(aPosition), iAsynchronousWrite(EFalse)
		{ InitRandomNumberGenerator(); }

		void InitRandomNumberGenerator()
			{
			TTime seedTime;
			seedTime.UniversalTime();
			iSeed = seedTime.Int64();
			}

		void SetPosition(TInt aPosition)
		{ iPosition = aPosition; }

		void SetAsynchronousWrite(TBool aAsynchronousWrite)
		{ iAsynchronousWrite = aAsynchronousWrite; }

		void RandomiseBuffer();

		TVerdict DoTestPostambleL(TBool aCheck);
		virtual TInt CheckDestinationL() = 0;

	public:
		CMMFBuffer* iBuffer;
		TInt64 iSeed;

		TInt iPosition;
		TBool iAsynchronousWrite;
	};

class TWriteBufferToDescriptorTestStep : public TWriteBufferTestStep
	{
	public:
		TWriteBufferToDescriptorTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, const TDes8& aDestinationDes, TDes8& aCopyOfDestinationDes)
		: TWriteBufferTestStep(aSink, aBuffer), iDestinationDes(aDestinationDes), iCopyOfDestinationDes(aCopyOfDestinationDes)
		{ }

		TWriteBufferToDescriptorTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, TInt aPosition, const TDes8& aDestinationDes, TDes8& aCopyOfDestinationDes)
		: TWriteBufferTestStep(aSink, aBuffer, aPosition), iDestinationDes(aDestinationDes), iCopyOfDestinationDes(aCopyOfDestinationDes)
		{ }

		TWriteBufferToDescriptorTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, MDataSource* aSource, const TDes8& aDestinationDes, TDes8& aCopyOfDestinationDes)
		: TWriteBufferTestStep(aSink, aBuffer, aSource), iDestinationDes(aDestinationDes), iCopyOfDestinationDes(aCopyOfDestinationDes)
		{ }

		TWriteBufferToDescriptorTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSource, const TDes8& aDestinationDes, TDes8& aCopyOfDestinationDes)
		: TWriteBufferTestStep(aSink, aBuffer, aPosition, aSource), iDestinationDes(aDestinationDes), iCopyOfDestinationDes(aCopyOfDestinationDes)
		{ }

		TInt CheckDestinationL();

	public:
		const TDes8& iDestinationDes;
		TDes8& iCopyOfDestinationDes;
	};

class TWriteBufferToFileTestStep : public TWriteBufferTestStep
	{
	public:
		TWriteBufferToFileTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes)
		: TWriteBufferTestStep(aSink, aBuffer), iFsSession(aFsSession), iDestinationFileName(aDestinationFileName), iCopyOfDestinationDes(aCopyOfDestinationDes)
		{ }

		TWriteBufferToFileTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, TInt aPosition, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes)
		: TWriteBufferTestStep(aSink, aBuffer, aPosition), iFsSession(aFsSession), iDestinationFileName(aDestinationFileName), iCopyOfDestinationDes(aCopyOfDestinationDes)
		{ }

		TWriteBufferToFileTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, MDataSource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes)
		: TWriteBufferTestStep(aSink, aBuffer, aSource), iFsSession(aFsSession), iDestinationFileName(aDestinationFileName), iCopyOfDestinationDes(aCopyOfDestinationDes)
		{ }

		TWriteBufferToFileTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes)
		: TWriteBufferTestStep(aSink, aBuffer, aPosition, aSource), iFsSession(aFsSession), iDestinationFileName(aDestinationFileName), iCopyOfDestinationDes(aCopyOfDestinationDes)
		{ }

		TInt CheckDestinationL();
		void DoTestPreambleL();

	public:
		RFs& iFsSession;
		const TFileName& iDestinationFileName;
		TDes8& iCopyOfDestinationDes;
	};


class TNewSourceTestStep : public TFileTestStep
	{
public:
	TNewSourceTestStep(TUid aUid, TDesC8& aConfigDes)
	: iUid(aUid), iConfigDes(&aConfigDes)
	{ }

	TVerdict DoTestStepL();

public:
	TUid iUid;
	TDesC8* iConfigDes;
	};


class TSourceTypeTestStep : public TTestStep
	{
public:
	TSourceTypeTestStep(MDataSource* aSource, TUid aUid, TFourCC aFourCC, TBool aCanSetSourceDataType)
	: TTestStep(aSource), iUid(aUid), iFourCC(aFourCC), iCanSetSourceDataType(aCanSetSourceDataType)
	{ }

	TVerdict DoTestStepL();

public:
	TUid iUid;
	TFourCC iFourCC;
	TBool iCanSetSourceDataType;
	};

class TCanCreateSourceBufferTestStep : public TTestStep
	{
public:
	TCanCreateSourceBufferTestStep(MDataSource* aSource, TBool aCanCreateSourceBuffer)
	: TTestStep(aSource), iCanCreateSourceBuffer(aCanCreateSourceBuffer)
	{ }

	TVerdict DoTestStepL();

public:
	TBool iCanCreateSourceBuffer;
	};

class TSourceSampleConvertTestStep : public TTestStep
	{
public:
	TSourceSampleConvertTestStep(MDataSource* aSource, TBool aSourceSampleConvert)
	: TTestStep(aSource), iSourceSampleConvert(aSourceSampleConvert)
	{ }

	TVerdict DoTestStepL();

public:
	TBool iSourceSampleConvert;
	};

class TCreateSourceBuffer1TestStep : public TTestStep, public MAsyncEventHandler
	{
public:
	TCreateSourceBuffer1TestStep(TSourceType aSourceType, MDataSink* aSink, TUid aExpectedType, TBool aExpectedRef, TInt aExpectedMaxLength = 0)
	: TTestStep(NULL, aSink), iSourceType(aSourceType), iExpectedType(aExpectedType), iExpectedRef(aExpectedRef), iExpectedMaxLength(aExpectedMaxLength)
	{ }

	TVerdict DoTestStepL();
	TInt SendEventToClient(const TMMFEvent& aEvent);

public:
	TSourceType iSourceType;
	CMMFBuffer* iSinkBuffer;
	TUid iExpectedType;
	TBool iExpectedRef;
	TInt iExpectedMaxLength;
};

class TCreateSourceBuffer2TestStep : public TTestStep, public MAsyncEventHandler
	{
public:
	TCreateSourceBuffer2TestStep(TSourceType aSourceType, MDataSink* aSink, CMMFBuffer* aSinkBuffer, TUid aExpectedType, TBool aExpectedRef, TInt aExpectedMaxLength = 0)
	: TTestStep(NULL, aSink), iSourceType(aSourceType), iSinkBuffer(aSinkBuffer), iExpectedType(aExpectedType), iExpectedRef(aExpectedRef), iExpectedMaxLength(aExpectedMaxLength)
	{ }

	TVerdict DoTestStepL();
	TInt SendEventToClient(const TMMFEvent& aEvent);

public:
	TSourceType iSourceType;
	CMMFBuffer* iSinkBuffer;
	TUid iExpectedType;
	TBool iExpectedRef;
	TInt iExpectedMaxLength;
};

class TSourceThreadLogonTestStep : public TFileTestStep
	{
public:
	TSourceThreadLogonTestStep(MDataSource* aSource, MAsyncEventHandler* aEventHandler)
	: TFileTestStep(aSource), iEventHandler(aEventHandler)
	{ }

	TVerdict DoTestStepL();

public:
	MAsyncEventHandler* iEventHandler;
	};

class TNegotiateSourceTestStep : public TTestStep
	{
public:
	TNegotiateSourceTestStep(MDataSource* aSource, MDataSink* aSink)
	: TTestStep(aSource, aSink)
	{ }

	TVerdict DoTestStepL();
	};

class TFillBufferTestStep : public TReadBufferTestStep
	{
public:
	TFillBufferTestStep(MDataSource* aSource, CMMFBuffer* aBuffer, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TReadBufferTestStep(aSource, aBuffer, 0, aSink, aSourceDes), iReadLength(aReadLength)
	{ }

	void SetReadLength(TInt aReadLength)
	{ iReadLength = aReadLength; }

	TVerdict DoTestStepL();

public:
	TInt iReadLength;
	};

class TFillLastBufferTestStep : public TReadBufferTestStep
	{
public:
	TFillLastBufferTestStep(TUid aSourceUid, TDesC8& aConfigDes, CMMFDummySink* aSink, const TDes8& aSourceDes)
	: TReadBufferTestStep(NULL, NULL, 0, aSink, aSourceDes), iSourceUid(aSourceUid), iConfigDes(aConfigDes)
	{ }

	TVerdict DoTestStepL();

public:
	TUid iSourceUid;
	TDesC8 &iConfigDes;
	};

class TReadLastBufferTestStep : public TReadBufferTestStep
	{
public:
	TReadLastBufferTestStep(CMMFClip* aClip, CMMFDummySink* aSink, const TDes8& aSourceDes)
	: TReadBufferTestStep(aClip, NULL, 0, aSink, aSourceDes)
	{ }

	TVerdict DoTestStepL();
	};

class TReadLastBuffer2TestStep : public TReadBufferTestStep
	{
public:
	TReadLastBuffer2TestStep(CMMFClip* aClip, CMMFDummySink* aSink, const TDes8& aSourceDes)
	: TReadBufferTestStep(aClip, NULL, 0, aSink, aSourceDes)
	{ }

	TVerdict DoTestStepL();
	};

class TReadLastBuffer3TestStep : public TReadBufferTestStep
	{
public:
	TReadLastBuffer3TestStep(CMMFClip* aClip, const TDes8& aSourceDes)
	: TReadBufferTestStep(aClip, NULL, 0, NULL, aSourceDes)
	{ }

	TVerdict DoTestStepL();
	};


class TNewSinkTestStep : public TFileTestStep
	{
public:
	TNewSinkTestStep(TUid aUid, TDesC8& aConfigDes)
	: iUid(aUid), iConfigDes(&aConfigDes)
	{ }

	TVerdict DoTestStepL();

public:
	TUid iUid;
	TDesC8* iConfigDes;
	};


class TSinkTypeTestStep : public TTestStep
	{
public:
	TSinkTypeTestStep(MDataSink* aSink, TUid aUid, TFourCC aFourCC, TBool aCanSetSinkDataType)
	: TTestStep(aSink), iUid(aUid), iFourCC(aFourCC), iCanSetSinkDataType(aCanSetSinkDataType)
	{ }

	TVerdict DoTestStepL();

public:
	TUid iUid;
	TFourCC iFourCC;
	TBool iCanSetSinkDataType;
	};

class TCanCreateSinkBufferTestStep : public TTestStep
	{
public:
	TCanCreateSinkBufferTestStep(MDataSink* aSink, TBool aCanCreateSinkBuffer)
	: TTestStep(aSink), iCanCreateSinkBuffer(aCanCreateSinkBuffer)
	{ }

	TVerdict DoTestStepL();

public:
	TBool iCanCreateSinkBuffer;
	};

class TCreateSinkBufferTestStep0 : public TTestStep
	{
public:
	TCreateSinkBufferTestStep0(MDataSink* aSink, TUid aExpectedType, TBool aExpectedRef, TInt aExpectedMaxLength = 0, CMMFDummySource* aSource = NULL)
	: TTestStep(aSource, aSink), iExpectedType(aExpectedType), iExpectedRef(aExpectedRef), iExpectedMaxLength(aExpectedMaxLength)
	{ }

	TVerdict DoTestStepL();

public:
	TUid iExpectedType;
	TBool iExpectedRef;
	CMMFFormatDecode* iFormat;
	TInt iExpectedMaxLength;
	};

class TCreateSinkBufferTestStep : public TTestStep
	{
public:
	TCreateSinkBufferTestStep(TUid aExpectedType, TBool aExpectedRef, CMMFFormatDecode* aFormat,
							  TInt aExpectedMaxLength = 0, CMMFDummySource* aSource = NULL)
		: TTestStep(aSource, NULL), iExpectedType(aExpectedType), iExpectedRef(aExpectedRef), iFormat(aFormat), iExpectedMaxLength(aExpectedMaxLength)
		{
		}

	TVerdict DoTestPostambleL(TBool aCheck);
	
public:
	TUid iExpectedType;
	TBool iExpectedRef;
	CMMFFormatDecode* iFormat;
	TInt iExpectedMaxLength;
	};

class TCreateSinkBufferTestStep1 : public TCreateSinkBufferTestStep 
	{
public:
	TCreateSinkBufferTestStep1(TUid aExpectedType, TBool aExpectedRef, CMMFFormatDecode* aFormat,
							  TInt aExpectedMaxLength = 0, CMMFDummySource* aSource = NULL)
		: TCreateSinkBufferTestStep(aExpectedType, aExpectedRef, aFormat, aExpectedMaxLength, aSource)
		{
		}

	TVerdict DoTestStepL();
	};

class TCreateSinkBufferTestStep2 : public TCreateSinkBufferTestStep 
	{
public:
	TCreateSinkBufferTestStep2(TUid aExpectedType, TBool aExpectedRef, CMMFFormatDecode* aFormat,
							  TInt aExpectedMaxLength = 0, CMMFDummySource* aSource = NULL)
		: TCreateSinkBufferTestStep(aExpectedType, aExpectedRef, aFormat, aExpectedMaxLength, aSource)
		{
		}

	TVerdict DoTestStepL();
	};

class TCreateSinkBufferTestStep3 : public TCreateSinkBufferTestStep 
	{
public:
	TCreateSinkBufferTestStep3(TUid aExpectedType, TBool aExpectedRef, CMMFFormatDecode* aFormat,
							  TInt aExpectedMaxLength = 0, CMMFDummySource* aSource = NULL)
		: TCreateSinkBufferTestStep(aExpectedType, aExpectedRef, aFormat, aExpectedMaxLength, aSource)
		{
		}

	TVerdict DoTestStepL();
	};

class TCreateSinkBufferTestStep4 : public TCreateSinkBufferTestStep 
	{
public:
	TCreateSinkBufferTestStep4(TUid aExpectedType, TBool aExpectedRef, CMMFFormatDecode* aFormat,
							  TInt aExpectedMaxLength = 0, CMMFDummySource* aSource = NULL)
		: TCreateSinkBufferTestStep(aExpectedType, aExpectedRef, aFormat, aExpectedMaxLength, aSource)
		{
		}

	TVerdict DoTestStepL();
	};

class TSinkThreadLogonTestStep : public TFileTestStep
	{
public:
	TSinkThreadLogonTestStep(MDataSink* aSink, MAsyncEventHandler* aEventHandler)
	: TFileTestStep(aSink), iEventHandler(aEventHandler)
	{ }

	TVerdict DoTestStepL();

public:
	MAsyncEventHandler* iEventHandler;
	};

class TNegotiateTestStep : public TTestStep
	{
public:
	TNegotiateTestStep(MDataSink* aSink, MDataSource* aSource)
	: TTestStep(aSource, aSink)
	{ }

	TVerdict DoTestStepL();
	};

class TEmptyBufferTestStep : public TWriteBufferToDescriptorTestStep
	{
public:
	TEmptyBufferTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, CMMFDummySource* aSource, const TDes8& aDestinationDes, TDes8& aCopyOfDestinationDes)
	: TWriteBufferToDescriptorTestStep(aSink, aBuffer, aSource, aDestinationDes, aCopyOfDestinationDes), iWriteLength((aBuffer != NULL) ? aBuffer->BufferSize() : 0)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TInt iWriteLength;
	};


class TClipBytesFreeTestStep : public TFileTestStep
	{
public:
	TClipBytesFreeTestStep(CMMFClip* aClip, TInt64 aExpectedBytesFree)
	: TFileTestStep(aClip, aClip), iExpectedBytesFree(aExpectedBytesFree)
	{ }

	TVerdict DoTestStepL();

public:
	TInt64 iExpectedBytesFree;
	};
	
class TClipSizeTestStep : public TFileTestStep
	{
public:
	TClipSizeTestStep(CMMFClip* aClip, TInt aExpectedSize)
	: TFileTestStep(aClip, aClip), iExpectedSize(aExpectedSize)
	{ }

	TVerdict DoTestStepL();

public:
	TInt iExpectedSize;
	};

class TClipSetSizeTestStep : public TFileTestStep
	{
public:
	TClipSetSizeTestStep(CMMFClip* aClip, TBool aCanSetSize)
	: TFileTestStep(aClip, aClip), iCanSetSize(aCanSetSize)
	{ }

	TVerdict DoTestStepL();

public:
	TBool iCanSetSize;
	};

class TClipDeleteTestStep : public TFileTestStep
	{
public:
	TClipDeleteTestStep(CMMFClip* aClip)
	: TFileTestStep(aClip, aClip)
	{ }
	
	TVerdict DoTestStepL();
	};

class TClipReadBufferTestStep : public TReadBufferTestStep
	{
public:
	TClipReadBufferTestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TReadBufferTestStep(aClip, aBuffer, aPosition, aSink, aSourceDes), iReadLength(aReadLength)
	{ }

	void SetReadLength(TInt aReadLength)
	{ iReadLength = aReadLength; }

	TVerdict DoTestStepL();

public:
	TInt iReadLength;
	};

class TClipReadBuffer2TestStep : public TReadBufferTestStep
	{
public:
	TClipReadBuffer2TestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TReadBufferTestStep(aClip, aBuffer, aPosition, aSink, aSourceDes), iReadLength(aReadLength)
	{ }

	void SetReadLength(TInt aReadLength)
	{ iReadLength = aReadLength; }

	TVerdict DoTestStepL();

public:
	TInt iReadLength;
	};
	
class TClipReadBuffer3TestStep : public TReadBufferTestStep
	{
public:
	TClipReadBuffer3TestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TReadBufferTestStep(aClip, aBuffer, aPosition, aSourceDes), iReadLength(aReadLength)
	{ }

	void SetReadLength(TInt aReadLength)
	{ iReadLength = aReadLength; }

	TVerdict DoTestStepL();

public:
	TInt iReadLength;
	};

class TClipReadBuffer4TestStep : public TReadBufferTestStep
	{
public:
	TClipReadBuffer4TestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TReadBufferTestStep(aClip, aBuffer, aPosition, aSink, aSourceDes), iReadLength(aReadLength), iExpectedReadLength(aReadLength)
	{ }

	void SetReadLength(TInt aReadLength)
	{ iReadLength = aReadLength; }

	void SetExpectedReadLength(TInt aExpectedReadLength)
	{ iExpectedReadLength = aExpectedReadLength; }

	TVerdict DoTestStepL();

public:
	TInt iReadLength;
	TInt iExpectedReadLength;
	};


class TDescriptorClipWriteBufferTestStep : public TWriteBufferToDescriptorTestStep
	{
public:
	TDescriptorClipWriteBufferTestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySource* aSource, const TDes8& aDestinationDes, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToDescriptorTestStep(aClip, aBuffer, aPosition, aSource, aDestinationDes, aCopyOfDestinationDes), iWriteLength(aWriteLength)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TInt iWriteLength;
	};

class TDescriptorClipWriteBuffer2TestStep : public TWriteBufferToDescriptorTestStep
	{
public:
	TDescriptorClipWriteBuffer2TestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySource* aSource, const TDes8& aDestinationDes, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToDescriptorTestStep(aClip, aBuffer, aPosition, aSource, aDestinationDes, aCopyOfDestinationDes), iWriteLength(aWriteLength)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TInt iWriteLength;
	};

class TDescriptorClipWriteBuffer3TestStep : public TWriteBufferToDescriptorTestStep
	{
public:
	TDescriptorClipWriteBuffer3TestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, const TDes8& aDestinationDes, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToDescriptorTestStep(aClip, aBuffer, aPosition, aDestinationDes, aCopyOfDestinationDes), iWriteLength(aWriteLength)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TInt iWriteLength;
	};

class TNewFileSourceTestStep : public TNewSourceTestStep
	{
public:
	TNewFileSourceTestStep(TUid aUid, TDesC8& aConfigDes)
	: TNewSourceTestStep(aUid, aConfigDes)
	{ }

	TVerdict DoTestStepL();
	};

class TFileSourceThreadLogonTestStep : public TSourceThreadLogonTestStep
	{
public:
	TFileSourceThreadLogonTestStep(MDataSource* aSource, MAsyncEventHandler* aEventHandler, TInt aFileSize)
	: TSourceThreadLogonTestStep(aSource, aEventHandler), iFileSize(aFileSize)
	{ }

	TVerdict DoTestStepL();

public:
	TInt iFileSize;
	};

class TFileFillBufferTestStep : public TFillBufferTestStep
	{
public:
	TFileFillBufferTestStep(TSourceType aSourceType, TDesC& aFileName, CMMFBuffer* aBuffer, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TFillBufferTestStep(NULL, aBuffer, aSink, aSourceDes, aReadLength), iFileName(aFileName), iSourceType(aSourceType)
	{ }

	TVerdict DoTestStepL();

public:
	TDesC& iFileName;
	TSourceType iSourceType;
	};

class TFileFillBuffer2TestStep : public TFileFillBufferTestStep
	{
public:
	TFileFillBuffer2TestStep(TSourceType aSourceType, TDesC& aFileName, CMMFBuffer* aBuffer, CMMFDummySink* aSink, const TDes8& aSourceDes)
	: TFileFillBufferTestStep(aSourceType, aFileName, aBuffer, aSink, aSourceDes)
	{ }

	TVerdict DoTestStepL();
	};

class TFileFillBuffer3TestStep : public TFileFillBufferTestStep
	{
public:
	TFileFillBuffer3TestStep(TSourceType aSourceType, TDesC& aFileName, RPointerArray<CMMFBuffer>* aBufferArray, CMMFDummySink* aSink, const TDes8& aSourceDes, TInt aReadLength = 0)
	: TFileFillBufferTestStep(aSourceType, aFileName, NULL, aSink, aSourceDes, aReadLength), iBufferArray(aBufferArray), iMaxRequestCount(20)
	{ }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }
	
	TVerdict DoTestStepL();

public:
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileFillBuffer4TestStep : public TFillBufferTestStep
	{
public:
	TFileFillBuffer4TestStep(MDataSource* aSource, RPointerArray<CMMFBuffer>* aBufferArray, CMMFDummySink* aSink, const TDes8& aSourceDes, TInt aReadLength = 0)
	: TFillBufferTestStep(aSource, NULL, aSink, aSourceDes, aReadLength), iBufferArray(aBufferArray), iMaxRequestCount(3)
	{ }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }
	
	TVerdict DoTestStepL();

public:
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileReadBufferTestStep : public TClipReadBufferTestStep
	{
public:
	TFileReadBufferTestStep(TSourceType aSourceType, const TDesC& aFileName, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TClipReadBufferTestStep(NULL, aBuffer, aPosition, aSink, aSourceDes, aReadLength), iSourceType(aSourceType), iFileName(aFileName)
	{ }

	TVerdict DoTestStepL();

public:
	TSourceType iSourceType;
	const TDesC& iFileName;
	
	};

class TFileReadBuffer2TestStep : public TClipReadBuffer2TestStep
	{
public:
	TFileReadBuffer2TestStep(TSourceType aSourceType, const TDesC& aFileName, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TClipReadBuffer2TestStep(NULL, aBuffer, aPosition, aSink, aSourceDes, aReadLength), iSourceType(aSourceType), iFileName(aFileName)
	{ }

	TVerdict DoTestStepL();

public:
	TSourceType iSourceType;
	const TDesC& iFileName;
	};

class TFileReadBuffer3TestStep : public TClipReadBuffer3TestStep
	{
public:
	TFileReadBuffer3TestStep(TSourceType aSourceType, const TDesC& aFileName, CMMFBuffer* aBuffer, TInt aPosition, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TClipReadBuffer3TestStep(NULL, aBuffer, aPosition, aSourceDes, aReadLength), iSourceType(aSourceType), iFileName(aFileName)
	{ }

	void SetReadLength(TInt aReadLength)
	{ iReadLength = aReadLength; }

	TVerdict DoTestStepL();

public:
	TSourceType iSourceType;
	const TDesC& iFileName;
	};

class TFileReadBuffer4TestStep : public TClipReadBuffer4TestStep
	{
public:
	TFileReadBuffer4TestStep(TSourceType aSourceType, const TDesC& aFileName, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TClipReadBuffer4TestStep(NULL, aBuffer, aPosition, aSink, aSourceDes, aReadLength), iSourceType(aSourceType), iFileName(aFileName)
	{ }

	TVerdict DoTestStepL();

public:
	TSourceType iSourceType;
	const TDesC& iFileName;
	};

class TFileReadBuffer5TestStep : public TClipReadBuffer4TestStep
	{
public:
	TFileReadBuffer5TestStep(TSourceType aSourceType, const TDesC& aFileName, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TClipReadBuffer4TestStep(NULL, NULL, aPosition, aSink, aSourceDes, aReadLength), iSourceType(aSourceType), iFileName(aFileName), iBufferArray(aBufferArray), iMaxRequestCount(20)
	{ }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }

	TVerdict DoTestStepL();

public:
	TSourceType iSourceType;
	const TDesC& iFileName;
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileReadBuffer6TestStep : public TClipReadBuffer4TestStep
	{
public:
	TFileReadBuffer6TestStep(TSourceType aSourceType, const TDesC& aFileName, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TClipReadBuffer4TestStep(NULL, NULL, aPosition, aSink, aSourceDes, aReadLength), iSourceType(aSourceType), iFileName(aFileName), iBufferArray(aBufferArray), iMaxRequestCount(20)
	{ }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }

	TVerdict DoTestStepL();

public:
	TSourceType iSourceType;
	const TDesC& iFileName;
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileReadBuffer7TestStep : public TClipReadBuffer4TestStep
	{
public:
	TFileReadBuffer7TestStep(CMMFClip* aSource, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TClipReadBuffer4TestStep(aSource, NULL, aPosition, aSink, aSourceDes, aReadLength), iBufferArray(aBufferArray), iMaxRequestCount(3)
	{ }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }

	TVerdict DoTestStepL();

public:
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileReadBuffer8TestStep : public TClipReadBuffer4TestStep
	{
public:
	TFileReadBuffer8TestStep(CMMFClip* aSource, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySink* aSink, const TDes8& aSourceDes, TUint aReadLength = 0)
	: TClipReadBuffer4TestStep(aSource, NULL, aPosition, aSink, aSourceDes, aReadLength), iBufferArray(aBufferArray), iMaxRequestCount(3)
	{ }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }

	TVerdict DoTestStepL();

public:
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};


class TNewFileSinkTestStep : public TNewSinkTestStep
	{
public:
	TNewFileSinkTestStep(TUid aUid, TDesC8& aConfigDes)
	: TNewSinkTestStep(aUid, aConfigDes)
	{ }

	TVerdict DoTestStepL();
	};

class TFileSinkSetSizeTestStep : public TClipSetSizeTestStep
	{
public:
	TFileSinkSetSizeTestStep(CMMFClip* aClip, TBool aCanSetSize)
	: TClipSetSizeTestStep(aClip, aCanSetSize)
	{ }

	TVerdict DoTestStepL();
	};

class TFileSinkThreadLogonTestStep : public TSinkThreadLogonTestStep
	{
public:
	TFileSinkThreadLogonTestStep(MDataSink* aSink, MAsyncEventHandler* aEventHandler, TInt aFileSize)
	: TSinkThreadLogonTestStep(aSink, aEventHandler), iFileSize(aFileSize)
	{ }

	TVerdict DoTestStepL();

public:
	TInt iFileSize;
	};

class TFileEmptyBufferTestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileEmptyBufferTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFile, TDes8& aCopyOfDestinationDes)
	: TWriteBufferToFileTestStep(aSink, aBuffer, aSource, aFsSession, aDestinationFile, aCopyOfDestinationDes), iWriteLength((aBuffer != NULL) ? aBuffer->BufferSize() : 0)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TInt iWriteLength;
	};

class TFileEmptyBuffer2TestStep : public TFileEmptyBufferTestStep
	{
public:
	TFileEmptyBuffer2TestStep(TSinkType aSinkType, CMMFBuffer* aBuffer, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFile, TDes8& aCopyOfDestinationDes)
	: TFileEmptyBufferTestStep(NULL, aBuffer, aSource, aFsSession, aDestinationFile, aCopyOfDestinationDes), iSinkType(aSinkType)
	{ }

	TVerdict DoTestStepL();
protected:
	TSinkType iSinkType;
	};

class TFileEmptyBuffer3TestStep : public TFileEmptyBuffer2TestStep
	{
public:
	TFileEmptyBuffer3TestStep(TSinkType aSinkType, CMMFBuffer* aBuffer, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFile, TDes8& aCopyOfDestinationDes)
	: TFileEmptyBuffer2TestStep(aSinkType, aBuffer, aSource, aFsSession, aDestinationFile, aCopyOfDestinationDes)
	{ }

	TVerdict DoTestStepL();

	void DoTestPreambleL();
	TVerdict DoTestPostambleL(TBool aCheck);
	};

class TFileEmptyBuffer4TestStep : public TFileEmptyBuffer3TestStep
	{
public:
	TFileEmptyBuffer4TestStep(TSinkType aSinkType, RPointerArray<CMMFBuffer>* aBufferArray, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFile, TDes8& aCopyOfDestinationDes)
	: TFileEmptyBuffer3TestStep(aSinkType, NULL, aSource, aFsSession, aDestinationFile, aCopyOfDestinationDes), iBufferArray(aBufferArray), iMaxRequestCount(20)
	{ }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }
	
	TVerdict DoTestStepL();

	void DoTestPreambleL();
	TVerdict DoTestPostambleL(TBool aCheck);

public:
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileEmptyBuffer5TestStep : public TFileEmptyBuffer4TestStep
	{
public:
	TFileEmptyBuffer5TestStep(TSinkType aSinkType, RPointerArray<CMMFBuffer>* aBufferArray, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFile, TDes8& aCopyOfDestinationDes)
	: TFileEmptyBuffer4TestStep(aSinkType, aBufferArray, aSource, aFsSession, aDestinationFile, aCopyOfDestinationDes)
	{ }

	TInt CheckDestinationL();
	};

class TFileEmptyBuffer6TestStep : public TFileEmptyBufferTestStep
	{
public:
	TFileEmptyBuffer6TestStep(MDataSink* aSink, RPointerArray<CMMFBuffer>* aBufferArray, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFile, TDes8& aCopyOfDestinationDes)
	: TFileEmptyBufferTestStep(aSink, NULL, aSource, aFsSession, aDestinationFile, aCopyOfDestinationDes), iBufferArray(aBufferArray), iMaxRequestCount(3)
	{ }

	TVerdict DoTestStepL();
	TInt CheckDestinationL();

public:
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileWriteBufferTestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileWriteBufferTestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToFileTestStep(aClip, aBuffer, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes), iWriteLength(aWriteLength)
	{ iAsynchronousWrite = ETrue; }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TInt iWriteLength;
	};

class TFileWriteBuffer2TestStep : public TFileWriteBufferTestStep
	{
public:
	TFileWriteBuffer2TestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TFileWriteBufferTestStep(aClip, aBuffer, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes, aWriteLength)
	{ iAsynchronousWrite = ETrue; }

	TVerdict DoTestStepL();
	};

class TFileWriteBuffer3TestStep : public TFileWriteBufferTestStep
	{
public:
	TFileWriteBuffer3TestStep(CMMFClip* aClip, CMMFBuffer* aBuffer, TInt aPosition, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TFileWriteBufferTestStep(aClip, aBuffer, aPosition, NULL, aFsSession, aDestinationFileName, aCopyOfDestinationDes, aWriteLength)
	{ }

	TVerdict DoTestStepL();
	};

class TFileWriteBuffer4TestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileWriteBuffer4TestStep(TSinkType aSinkType, TDesC& aFileName, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToFileTestStep(NULL, aBuffer, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes), iWriteLength(aWriteLength), iSinkType(aSinkType), iFileName(aFileName)
	{ iAsynchronousWrite = ETrue; }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TInt iWriteLength;
	TSinkType iSinkType;
	TDesC& iFileName;
	};

class TFileWriteBuffer5TestStep : public TFileWriteBuffer4TestStep
	{
public:
	TFileWriteBuffer5TestStep(TSinkType aSinkType, TDesC& aFileName, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TFileWriteBuffer4TestStep(aSinkType, aFileName, aBuffer, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes, aWriteLength)
	{ }

	TInt CheckDestinationL();
	TVerdict DoNegativeTestPostambleL();
	};

class TFileWriteBuffer6TestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileWriteBuffer6TestStep(TSinkType aSinkType, TDesC& aFileName, CMMFBuffer* aBuffer, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToFileTestStep(NULL, aBuffer, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes), iWriteLength(aWriteLength), iSinkType(aSinkType), iFileName(aFileName)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TInt iWriteLength;
	TSinkType iSinkType;
	TDesC& iFileName;
	};

class TFileWriteBuffer7TestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileWriteBuffer7TestStep(TSinkType aSinkType, TDesC& aFileName, CMMFBuffer* aBuffer, TInt aPosition, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToFileTestStep(NULL, aBuffer, aPosition, aFsSession, aDestinationFileName, aCopyOfDestinationDes),iSinkType(aSinkType), iFileName(aFileName), iWriteLength(aWriteLength)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	TVerdict DoTestStepL();

public:
	TSinkType iSinkType;
	TDesC& iFileName;
	TInt iWriteLength;
	};

class TFileWriteBuffer8TestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileWriteBuffer8TestStep(TSinkType aSinkType, TDesC& aFileName, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToFileTestStep(NULL, NULL, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes), iSinkType(aSinkType), iFileName(aFileName), iWriteLength(aWriteLength), iBufferArray(aBufferArray), iMaxRequestCount(20)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }

	TVerdict DoTestStepL();

public:
	TSinkType iSinkType;
	TDesC& iFileName;
	TInt iWriteLength;
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileWriteBuffer9TestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileWriteBuffer9TestStep(TSinkType aSinkType, TDesC& aFileName, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToFileTestStep(NULL, NULL, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes), iFileName(aFileName), iWriteLength(aWriteLength), iBufferArray(aBufferArray), iMaxRequestCount(20), iSinkType(aSinkType)
	{ }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }

	TVerdict DoTestStepL();

public:
	TDesC& iFileName;
	TInt iWriteLength;
	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	TSinkType iSinkType;
	};

class TFileWriteBuffer10TestStep : public TFileWriteBuffer8TestStep
	{
public:
	TFileWriteBuffer10TestStep(TSinkType aSinkType, TDesC& aFileName, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TFileWriteBuffer8TestStep(aSinkType, aFileName, aBufferArray, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes, aWriteLength)
	{ iMaxRequestCount = 3; }

	TInt CheckDestinationL();
	};

class TFileWriteBuffer11TestStep : public TFileWriteBuffer9TestStep
	{
public:
	TFileWriteBuffer11TestStep(TSinkType aSinkType, TDesC& aFileName, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TFileWriteBuffer9TestStep(aSinkType, aFileName, aBufferArray, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes, aWriteLength)
	{ iMaxRequestCount = 3; }

	TInt CheckDestinationL();
	};

class TFileWriteBuffer12TestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileWriteBuffer12TestStep(MDataSink* aSink, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToFileTestStep(aSink, NULL, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes), iWriteLength(aWriteLength), iBufferArray(aBufferArray), iMaxRequestCount(3)
	{ ASSERT(iBufferArray->Count() == 5); }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }

	TVerdict DoTestStepL();
	TInt CheckDestinationL();

public:
	TInt iWriteLength;

	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TFileWriteBuffer13TestStep : public TWriteBufferToFileTestStep
	{
public:
	TFileWriteBuffer13TestStep(MDataSink* aSink, RPointerArray<CMMFBuffer>* aBufferArray, TInt aPosition, CMMFDummySource* aSource, RFs& aFsSession, const TFileName& aDestinationFileName, TDes8& aCopyOfDestinationDes, TUint aWriteLength = 0)
	: TWriteBufferToFileTestStep(aSink, NULL, aPosition, aSource, aFsSession, aDestinationFileName, aCopyOfDestinationDes), iWriteLength(aWriteLength), iBufferArray(aBufferArray), iMaxRequestCount(3)
	{ ASSERT(iBufferArray->Count() == 5); }

	void SetWriteLength(TInt aWriteLength)
	{ iWriteLength = aWriteLength; }

	void SetMaxRequestCount(TInt aMaxRequestCount)
	{ iMaxRequestCount = aMaxRequestCount; }

	TVerdict DoTestStepL();
	TInt CheckDestinationL();

public:
	TInt iWriteLength;

	RPointerArray<CMMFBuffer>* iBufferArray;
	TInt iMaxRequestCount;
	};

class TAudioInputCreateSourceBuffer1TestStep : public TTestStep
	{
public:
	TAudioInputCreateSourceBuffer1TestStep(CMMFAudioInput* aSource)
	: TTestStep(aSource)
	{ }

	TVerdict DoTestStepL();
};

class TAudioInputCreateSourceBuffer2TestStep : public TTestStep
	{
public:
	TAudioInputCreateSourceBuffer2TestStep(CMMFAudioInput* aSource, CMMFBuffer* aSinkBuffer)
	: TTestStep(aSource), iSinkBuffer(aSinkBuffer)
	{ }

	TVerdict DoTestStepL();

public:
	CMMFBuffer* iSinkBuffer;
};

class TAudioInputFillBufferTestStep : public TTestStep
	{
public:
	TAudioInputFillBufferTestStep(MDataSource* aSource, CMMFBuffer* aBuffer, CMMFDummySink* aSink)
	: TTestStep(aSource, aSink), iBuffer(aBuffer)
	{ }

	TVerdict DoTestStepL();

public:
	CMMFBuffer* iBuffer;
	};

class TAudioInputFillBuffer2TestStep : public TAudioInputFillBufferTestStep
	{
public:
	TAudioInputFillBuffer2TestStep(MDataSource* aSource, CMMFBuffer* aBuffer, CMMFDummySink* aSink)
	: TAudioInputFillBufferTestStep(aSource, aBuffer, aSink)
	{ }

	TVerdict DoTestStepL();
	};

class TAudioInputFillBuffer3TestStep : public TAudioInputFillBufferTestStep
	{
public:
	TAudioInputFillBuffer3TestStep(MDataSource* aSource, CMMFBuffer* aBuffer, CMMFDummySink* aSink)
	: TAudioInputFillBufferTestStep(aSource, aBuffer, aSink)
	{ }

	TVerdict DoTestStepL();
	};

class TAudioInputFillBuffer4TestStep : public TAudioInputFillBufferTestStep
	{
public:
	TAudioInputFillBuffer4TestStep(CMMFBuffer* aBuffer, CMMFDummySink* aSink)
	: TAudioInputFillBufferTestStep(NULL, aBuffer, aSink)
	{ }

	TVerdict DoTestStepL();
	};
	
class TAudioInputFillBuffer5TestStep : public TAudioInputFillBufferTestStep
	{
public:
	TAudioInputFillBuffer5TestStep(CMMFBuffer* aBuffer, CMMFDummySink* aSink)
	: TAudioInputFillBufferTestStep(NULL, aBuffer, aSink)
	{ }

	TVerdict DoTestStepL();
	};

class TAudioInputFillBuffer6TestStep : public TAudioInputFillBufferTestStep
	{
public:
	TAudioInputFillBuffer6TestStep(CMMFBuffer* aBuffer, CMMFDummySink* aSink)
	: TAudioInputFillBufferTestStep(NULL, aBuffer, aSink)
	{ }

	TVerdict DoTestStepL();
	};

class TAudioInputFillBuffer7TestStep : public TAudioInputFillBufferTestStep
	{
public:
	TAudioInputFillBuffer7TestStep(CMMFBuffer* aBuffer, CMMFDummySink* aSink)
	: TAudioInputFillBufferTestStep(NULL, aBuffer, aSink)
	{ }

	TVerdict DoTestStepL();
	};

class TAudioInputFillBuffer8TestStep : public TAudioInputFillBufferTestStep
	{
public:
	TAudioInputFillBuffer8TestStep(CMMFBuffer* aBuffer, CMMFDummySink* aSink)
	: TAudioInputFillBufferTestStep(NULL, aBuffer, aSink)
	{ }

	TVerdict DoTestStepL();
	};


class TAudioOutputEmptyBufferTestStep : public TTestStep
	{
public:
	TAudioOutputEmptyBufferTestStep(MDataSink* aSink, CMMFBuffer* aBuffer, CMMFDummySource* aSource)
	: TTestStep(aSource, aSink), iBuffer(aBuffer), iAsynchronousWrite(EFalse)
	{ }

	void SetAsynchronousWrite()
	{ iAsynchronousWrite = ETrue; }
	
	TVerdict DoTestStepL();

public:
	CMMFBuffer* iBuffer;
	TBool iAsynchronousWrite;
	};

class TAudioOutputEmptyBuffer2TestStep : public TAudioOutputEmptyBufferTestStep
	{
public:
	TAudioOutputEmptyBuffer2TestStep(MDataSink* aSink, CMMFBuffer* aBuffer, CMMFDummySource* aSource)
	: TAudioOutputEmptyBufferTestStep(aSink, aBuffer, aSource)
	{ iAsynchronousWrite = ETrue; }
	
	TVerdict DoTestStepL();
	};

class TAudioOutputEmptyBuffer3TestStep : public TAudioOutputEmptyBufferTestStep
	{
public:
	TAudioOutputEmptyBuffer3TestStep(MDataSink* aSink, CMMFBuffer* aBuffer, CMMFDummySource* aSource)
	: TAudioOutputEmptyBufferTestStep(aSink, aBuffer, aSource)
	{ iAsynchronousWrite = ETrue; }
	
	TVerdict DoTestStepL();
	};

class TAudioOutputEmptyBuffer4TestStep : public TAudioOutputEmptyBufferTestStep
	{
public:
	TAudioOutputEmptyBuffer4TestStep(CMMFBuffer* aBuffer, CMMFDummySource* aSource, MDataSource& aRealFormat)
	: TAudioOutputEmptyBufferTestStep(NULL, aBuffer, aSource), iRealFormat(aRealFormat)
	{ }
	
	TVerdict DoTestStepL();

public:
	MDataSource& iRealFormat;
	};
	
class TAudioOutputEmptyBuffer5TestStep : public TAudioOutputEmptyBufferTestStep
	{
public:
	TAudioOutputEmptyBuffer5TestStep(CMMFBuffer* aBuffer, CMMFDummySource* aSource, MDataSource& aRealFormat)
	: TAudioOutputEmptyBufferTestStep(NULL, aBuffer, aSource), iRealFormat(aRealFormat)
	{ }
	
	TVerdict DoTestStepL();

public:
	MDataSource& iRealFormat;
	};

class TAudioOutputEmptyBuffer6TestStep : public TAudioOutputEmptyBufferTestStep
	{
public:
	TAudioOutputEmptyBuffer6TestStep(CMMFBuffer* aBuffer, CMMFDummySource* aSource, MDataSource& aRealFormat)
	: TAudioOutputEmptyBufferTestStep(NULL, aBuffer, aSource), iRealFormat(aRealFormat)
	{ }
	
	TVerdict DoTestStepL();

public:
	MDataSource& iRealFormat;
	};

class TPrimeStopTestStep : public TFileTestStep
	{
public:
	TPrimeStopTestStep()
		{ }

	virtual TVerdict DoTestStepL();
	};


class TPrimeStopSinkTestStep : public TFileTestStep
	{
public:
	TPrimeStopSinkTestStep()
		{ }

	virtual TVerdict DoTestStepL();
	};


/**
 *
 * CTestStep_MMF_SRSSNK, which is the base class for all 
 * the SelfTest suite test steps.
 *
 * @lib "TSU_MMF_SRSSNK.lib"
 *
 * @xxxx
 *
 */
const TInt KErrFileFail = -12345;
class CTestStep_MMF_SRSSNK : public CTestStep
	{
public:
	CTestStep_MMF_SRSSNK(TBool aIsAllocTest = ETrue);
	~CTestStep_MMF_SRSSNK();
	static void CleanUp(TAny *aPtr);
	inline void SetTestSuite(const CTestSuite_MMF_SRSSNK* aTestSuite);

	void DoTestStep(TTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult, TInt aHeapCellCount = 0);
	void DoTestStepWithAllocFail(TTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult, TInt aHeapCellCount = 0);
	void DoTestStepWithFileFail(TFileTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult, TInt aHeapCellCount = 0);
	void DoTestStepWithAllocAndFileFail(TFileTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult);

	void DoTest(TTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult, TInt aHeapCellCount = 0);
	void DoFileTest(TFileTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult);
	void DoFileTestWithoutAllocFail(TFileTestStep& aTestStep, TPtrC aTestDescription, TInt aExpectedTestResult, TInt aHeapCellCount);
	
	// from CTestStep
public:
	TVerdict OpenL();

protected:
	void CopyFileL(RFs& aFs, const TDesC& aOld, const TDesC& aNew);
	
	void OpenFileL(const TDesC& aFileName, TInt aMode, TBool aShareProtected=EFalse);
	void CloseFile();
	
	void DeleteFileSinkL(TSinkType aSinkType, RFs& aFs, TDesC& aFileName);


protected:
	// pointer to suite which owns this test 
	const CTestSuite_MMF_SRSSNK* iMMFBaseClTestSuite;

	TVerdict iTestResult;
	TBool iIsAllocTest;
	TBool iFileFailureTestingEnabled;
	RFile iFile;
	RFs iFs;
	};

inline void CTestStep_MMF_SRSSNK::SetTestSuite(const CTestSuite_MMF_SRSSNK* aTestSuite)
	{
	iMMFBaseClTestSuite = aTestSuite;
	}

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0001 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0001();
	~CTestStep_MMF_SRSSNK_U_0001();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0002 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0002();
	~CTestStep_MMF_SRSSNK_U_0002();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0003 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0003();
	~CTestStep_MMF_SRSSNK_U_0003();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0004 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0004();
	~CTestStep_MMF_SRSSNK_U_0004();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0005 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0005();
	~CTestStep_MMF_SRSSNK_U_0005();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0006 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0006();
	~CTestStep_MMF_SRSSNK_U_0006();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0007 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0007();
	~CTestStep_MMF_SRSSNK_U_0007();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0008 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0008();
	~CTestStep_MMF_SRSSNK_U_0008();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0009 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0009();
	~CTestStep_MMF_SRSSNK_U_0009();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0010 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0010();
	~CTestStep_MMF_SRSSNK_U_0010();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0011 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0011();
	~CTestStep_MMF_SRSSNK_U_0011();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0012 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0012();
	~CTestStep_MMF_SRSSNK_U_0012();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0013 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0013();
	~CTestStep_MMF_SRSSNK_U_0013();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0014 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0014();
	~CTestStep_MMF_SRSSNK_U_0014();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0015 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0015();
	~CTestStep_MMF_SRSSNK_U_0015();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0101 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0101();
	~CTestStep_MMF_SRSSNK_U_0101();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0102 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0102();
	~CTestStep_MMF_SRSSNK_U_0102();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0103 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0103();
	~CTestStep_MMF_SRSSNK_U_0103();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0104 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0104();
	~CTestStep_MMF_SRSSNK_U_0104();
	virtual TVerdict DoTestStepL();
	};



/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileSourceMisc : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileSourceMisc(TSourceType aSourceType);
	~CTestStep_MMF_SRSSNK_FileSourceMisc();
	virtual TVerdict DoTestStepL();
private:
	TSourceType iSourceType;
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileFillBuffer : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileFillBuffer(TSourceType aSourceType);
	~CTestStep_MMF_SRSSNK_FileFillBuffer();
	virtual TVerdict DoTestStepL();
private:
	TSourceType iSourceType;
	};




/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0107 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0107();
	~CTestStep_MMF_SRSSNK_U_0107();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileReadBuffer : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileReadBuffer(TSourceType aSourceType);
	~CTestStep_MMF_SRSSNK_FileReadBuffer();
	virtual TVerdict DoTestStepL();
private:
	TSourceType iSourceType;
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0109 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0109();
	~CTestStep_MMF_SRSSNK_U_0109();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileSinkMisc : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileSinkMisc(TSinkType aSinkType);
	~CTestStep_MMF_SRSSNK_FileSinkMisc();
	virtual TVerdict DoTestStepL();
private:
	TSinkType iSinkType;
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileEmptyBuffer : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileEmptyBuffer(TSinkType aSinkType);
	~CTestStep_MMF_SRSSNK_FileEmptyBuffer();
	virtual TVerdict DoTestStepL();
	
private:
	TSinkType iSinkType;
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0112 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0112();
	~CTestStep_MMF_SRSSNK_U_0112();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileWriteBuffer : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileWriteBuffer(TSinkType aSinkType);
	~CTestStep_MMF_SRSSNK_FileWriteBuffer();
	virtual TVerdict DoTestStepL();

private:
	TSinkType iSinkType;
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0114 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0114();
	~CTestStep_MMF_SRSSNK_U_0114();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0115 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0115();
	~CTestStep_MMF_SRSSNK_U_0115();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileEvaluateIntent : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileEvaluateIntent(TSourceType aSourceType);
	~CTestStep_MMF_SRSSNK_FileEvaluateIntent();
	virtual TVerdict DoTestStepL();
private:
	TSourceType iSourceType;
	TSinkType iSinkType;
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileExecuteIntent : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileExecuteIntent(TSourceType aSourceType);
	~CTestStep_MMF_SRSSNK_FileExecuteIntent();
	virtual TVerdict DoTestStepL();
private:
	TSourceType iSourceType;
	TSinkType iSinkType;
	};


/*
 *
 *
 */
class CTestStep_MMF_SRSSNK_FileIsProtected : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_FileIsProtected(TSourceType aSourceType);
	~CTestStep_MMF_SRSSNK_FileIsProtected();
	virtual TVerdict DoTestStepL();
private:
	TSourceType iSourceType;
	TSinkType iSinkType;
	};

/*
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0119 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0119();
	~CTestStep_MMF_SRSSNK_U_0119();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0201 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0201();
	~CTestStep_MMF_SRSSNK_U_0201();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0202 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0202();
	~CTestStep_MMF_SRSSNK_U_0202();
	virtual TVerdict DoTestStepL();
	};


/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0203 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0203();
	~CTestStep_MMF_SRSSNK_U_0203();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0204 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0204();
	~CTestStep_MMF_SRSSNK_U_0204();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0205 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0205();
	~CTestStep_MMF_SRSSNK_U_0205();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0206 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0206();
	~CTestStep_MMF_SRSSNK_U_0206();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0207 : public CTestStep_MMF_SRSSNK, public MDevSoundObserver
	{
public:
	CTestStep_MMF_SRSSNK_U_0207();
	~CTestStep_MMF_SRSSNK_U_0207();
	virtual TVerdict DoTestStepL();

	// from MDevSoundObserver
	virtual void InitializeComplete(TInt aError);
	virtual void ToneFinished(TInt /*aError*/) {};
	virtual void BufferToBeFilled(CMMFBuffer* /*aBuffer*/) {};
	virtual void PlayError(TInt /*aError*/) {};
	virtual void BufferToBeEmptied(CMMFBuffer* /*aBuffer*/) {};
	virtual void RecordError(TInt /*aError*/) {};
	virtual void ConvertError(TInt /*aError*/) {};
	virtual void DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {};
	virtual void SendEventToClient(const TMMFEvent& /*aEvent*/) {};
	CMMFAudioInput* iSource; // not owned by CTestStep_MMF_SRSSNK_U_0207, but transient
	TInt iError;
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0208 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0208();
	~CTestStep_MMF_SRSSNK_U_0208();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0209 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0209();
	~CTestStep_MMF_SRSSNK_U_0209();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0210 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0210();
	~CTestStep_MMF_SRSSNK_U_0210();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0211 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0211();
	~CTestStep_MMF_SRSSNK_U_0211();
	virtual TVerdict DoTestStepL();
	};

/**
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0212 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0212();
	~CTestStep_MMF_SRSSNK_U_0212();
	virtual TVerdict DoTestStepL();
	};

/*
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0301 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0301();
	~CTestStep_MMF_SRSSNK_U_0301();
	virtual TVerdict DoTestStepL();
	};

/*
 *
 *
 */
class CTestStep_MMF_SRSSNK_U_0302 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0302();
	~CTestStep_MMF_SRSSNK_U_0302();
	virtual TVerdict DoTestStepL();
	};


/*  Testing of the Source and Sink
 *
 *
 */

class CTestStep_MMF_SRSSNK_U_0400 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0400();
	~CTestStep_MMF_SRSSNK_U_0400();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0405 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0405();
	~CTestStep_MMF_SRSSNK_U_0405();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0406 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0406();
	~CTestStep_MMF_SRSSNK_U_0406();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0407 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0407();
	~CTestStep_MMF_SRSSNK_U_0407();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0408 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0408();
	~CTestStep_MMF_SRSSNK_U_0408();
	virtual TVerdict DoTestStepL();
	};


class CTestStep_MMF_SRSSNK_U_0409 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0409();
	~CTestStep_MMF_SRSSNK_U_0409();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0410 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0410();
	~CTestStep_MMF_SRSSNK_U_0410();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0411 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0411();
	~CTestStep_MMF_SRSSNK_U_0411();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0412 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0412();
	~CTestStep_MMF_SRSSNK_U_0412();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0413 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0413();
	~CTestStep_MMF_SRSSNK_U_0413();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0414 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0414();
	~CTestStep_MMF_SRSSNK_U_0414();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0415 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0415();
	~CTestStep_MMF_SRSSNK_U_0415();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0416 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0416();
	~CTestStep_MMF_SRSSNK_U_0416();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0417 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0417();
	~CTestStep_MMF_SRSSNK_U_0417();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0418 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0418();
	~CTestStep_MMF_SRSSNK_U_0418();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0419 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0419();
	~CTestStep_MMF_SRSSNK_U_0419();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0420 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0420();
	~CTestStep_MMF_SRSSNK_U_0420();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0421 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0421();
	~CTestStep_MMF_SRSSNK_U_0421();
	virtual TVerdict DoTestStepL();
	};

/*
 *
 *  Sinks
 *
 */

class CTestStep_MMF_SRSSNK_U_0500 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0500();
	~CTestStep_MMF_SRSSNK_U_0500();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0505 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0505();
	~CTestStep_MMF_SRSSNK_U_0505();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0506 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0506();
	~CTestStep_MMF_SRSSNK_U_0506();
	virtual TVerdict DoTestStepL();
	};


class CTestStep_MMF_SRSSNK_U_0507 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0507();
	~CTestStep_MMF_SRSSNK_U_0507();
	virtual TVerdict DoTestStepL();
	};


class CTestStep_MMF_SRSSNK_U_0508 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0508();
	~CTestStep_MMF_SRSSNK_U_0508();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_SRSSNK_U_0509 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0509();
	~CTestStep_MMF_SRSSNK_U_0509();
	virtual TVerdict DoTestStepL();
	};



class CTestStep_MMF_SRSSNK_U_0510 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0510();
	~CTestStep_MMF_SRSSNK_U_0510();
	virtual TVerdict DoTestStepL();
	};



class CTestStep_MMF_SRSSNK_U_0511 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0511();
	~CTestStep_MMF_SRSSNK_U_0511();
	virtual TVerdict DoTestStepL();
	};


class CTestStep_MMF_SRSSNK_U_0512 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0512();
	~CTestStep_MMF_SRSSNK_U_0512();
	virtual TVerdict DoTestStepL();
	};


class CTestStep_MMF_SRSSNK_U_0513 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0513();
	~CTestStep_MMF_SRSSNK_U_0513();
	virtual TVerdict DoTestStepL();
	};



class CTestStep_MMF_SRSSNK_U_0514 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0514();
	~CTestStep_MMF_SRSSNK_U_0514();
	virtual TVerdict DoTestStepL();
	};


class CTestStep_MMF_SRSSNK_U_0515 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0515();
	~CTestStep_MMF_SRSSNK_U_0515();
	virtual TVerdict DoTestStepL();
	};


class CTestStep_MMF_SRSSNK_U_0516 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0516();
	~CTestStep_MMF_SRSSNK_U_0516();
	virtual TVerdict DoTestStepL();
	};


class CTestStep_MMF_SRSSNK_U_0517 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0517();
	~CTestStep_MMF_SRSSNK_U_0517();
	virtual TVerdict DoTestStepL();
	};



class CTestStep_MMF_SRSSNK_U_0518 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0518();
	~CTestStep_MMF_SRSSNK_U_0518();
	virtual TVerdict DoTestStepL();
	};



class CTestStep_MMF_SRSSNK_U_0519 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0519();
	~CTestStep_MMF_SRSSNK_U_0519();
	virtual TVerdict DoTestStepL();
	};
	
	
class CTestStep_MMF_SRSSNK_MultipleStop : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_MultipleStop(); // U_520
	~CTestStep_MMF_SRSSNK_MultipleStop();
	virtual TVerdict DoTestStepL();
private:
	TSourceType iSourceType;
	};

// INC085391: CMMFFile::iFilePath and iFileDrive should be initialised for KMMFileHandleSource
class CTestStep_MMF_SRSSNK_U_0600 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0600();
	~CTestStep_MMF_SRSSNK_U_0600();
	virtual TVerdict DoTestStepL();
	};

//CR0854 - Provide access to the file data via the CData object 
class CTestStep_MMF_SRSSNK_U_0521 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0521();
	~CTestStep_MMF_SRSSNK_U_0521();
	virtual TVerdict DoTestStepL();
	};

//CR0854 - Provide access to the file data via the CData object 
class CTestStep_MMF_SRSSNK_U_0522 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0522();
	~CTestStep_MMF_SRSSNK_U_0522();
	virtual TVerdict DoTestStepL();
	};
	 
// PDEF102849: Invalid Remaining Space value obtained for E:\ (memory card)
class CTestStep_MMF_SRSSNK_U_0523 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0523();
	~CTestStep_MMF_SRSSNK_U_0523();
	virtual TVerdict DoTestStepL();
	};

//PDEF137282 - Answering VoIP call doesn't work 
class CTestStep_MMF_SRSSNK_U_0524 : public CTestStep_MMF_SRSSNK
	{
public:
	CTestStep_MMF_SRSSNK_U_0524();
	~CTestStep_MMF_SRSSNK_U_0524();
	virtual TVerdict DoTestStepL();
	};

class TCreateSourceBuffer3TestStep : public TTestStep, public MAsyncEventHandler
	{
public:
	TCreateSourceBuffer3TestStep(TSourceType aSourceType, MDataSink* aSink, TUid aExpectedType, TBool aExpectedRef, TInt aExpectedMaxLength = 0)
	: TTestStep(NULL, aSink), iSourceType(aSourceType), iExpectedType(aExpectedType), iExpectedRef(aExpectedRef), iExpectedMaxLength(aExpectedMaxLength)
	{ }

	TVerdict DoTestStepL();
	TInt SendEventToClient(const TMMFEvent& aEvent);

public:
	TSourceType iSourceType;
	CMMFBuffer* iSinkBuffer;
	TUid iExpectedType;
	TBool iExpectedRef;
	TInt iExpectedMaxLength;
	};

#endif /* __TSU_MMF_SRSSNK_H__ */






