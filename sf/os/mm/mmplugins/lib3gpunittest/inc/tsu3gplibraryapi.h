// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TSU3GPLIBRARYAPI_H
#define TSU3GPLIBRARYAPI_H

#include <test/testexecutestepbase.h>
#include <c3gplibrary.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <f32file.h>
#include <sys/unistd.h>
#include <badesca.h>

_LIT( K3GPLibTestFilePath, "c:\\3gplibrary\\" );

const TInt KBufferSize = 1024; // buffer size for read audio/video frames
const TInt KNotLeave = 11;

// define different test files
_LIT(KEmptyFileString, "");
_LIT(KInvalidFileString, "c:\3gplibrary\aaa.3gp"); // Non-existing file
_LIT(KNoAudioNoVideo, "MP4-NO-A-V");
_LIT(KSameLenAudioVideo, "3GP-SAME-LEN");
_LIT(KAudioOnly, "3GP-AUDIO-ONLY");
_LIT(KVideoOnly, "MP4-VIDEO-ONLY");
_LIT(KAudioMore, "3GP-AUDIO-MORE");
_LIT(KVideoMore, "3GP-VIDEO-MORE");
_LIT(KSqcp, "3G2-H263-QCEL");
_LIT(KEsds, "MP4-QCEL");
_LIT(KWmaFile, "WMA");
_LIT(KUnsupportedAudioCodec, "UNSUPORTED-AUDIO-CODEC");
_LIT(KUnsupportedVideo, "UNSUPPORTED-VIDEO");
_LIT(KUnsupportedAudioStream, "UNSUPORTED-AUDIO-STREAM");
_LIT(KMisMatch, "MISMATCHED");
_LIT(KMultiUdta, "USER-ATOM-ALL");

// Base class used for most test cases to open and close parser, parse data from file
class C3GPLibParser_base : public CTestStep
	{
public:
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TInt CheckError(TInt aError, TInt aExpectError, const TDesC& aFunction);
	void FileOpenL(TUint aFileMode = EFileRead | EFileShareReadersOnly);
	void FileClose();
	void FileOpen64L(TUint aFileMode = EFileRead | EFileShareReadersOnly);
	void FileClose64();
	void ParserOpenL(C3GPParse* aParser, TInt aExpectedError = KErrNone);	
	void ParserOpenFileL(C3GPParse* aParser, TInt aExpectedError = KErrNone);
	void ParserCompleteL(C3GPParse* aParser);
	void ParserOpenFileHandleL(C3GPParse* aParser, TUint aFileMode = EFileRead | EFileShareReadersOnly, TInt aExpectedError = KErrNone);
	void ParserCompleteHandleL(C3GPParse* aParser);
	void ParserOpenFileHandle64L(C3GPParse* aParser, TUint aFileMode = EFileRead | EFileShareReadersOnly, TInt aExpectedError = KErrNone);
	void ParserCompleteHandle64L(C3GPParse* aParser);
	void ParserOpenCafLC(C3GPParse* aParser, TInt aExpectedError = KErrNone);
	void ParserCompleteCafL(C3GPParse* aParser);
	void InsertDataL(C3GPParse* aParse, RFile aFile, TInt aBufferSize, TInt aPos);
	TInt ReadAudioVideoFrame(C3GPParse* aParse, TBool aRepeat = ETrue, TBool aReadAudio = EFalse);
	TInt GetClipProperties(C3GPParse* aParse, TInt& videoError, TUint& videoLength, TUint& audioLength);
	TInt ParserGetDecoderInfo(C3GPParse* aParser);
	TInt CallUDTApi(C3GPParse* aParse, TUint32 aUdtType, TUint& aIndex, T3GPUdtaLocation aLocation = E3GPUdtaVideoTrak);
protected:
	void DoGetVideoAudioAttributesL(C3GPParse* aParser);
	TInt DoReadFrame(C3GPParse* aParse);
	void DoSeekTestL(C3GPParse* aParser);
	void ReadFrameBufferModeL(C3GPParse* aParser);		
protected:
	RBuf iInputFile;
	RBuf iTestSection;
	RFs iFsSession;
	RFile iSourceFile;
	RFile64 iSourceFile64;
	};

// Base class for asynchronous read
class C3GPLib_AsyncObserver : public C3GPLibParser_base, public M3GPParseCallback
	{
public:
	C3GPLib_AsyncObserver::C3GPLib_AsyncObserver();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TInt ReadAudioVideoFrameAsync(C3GPParse* aParse, TBool aReadAudio = EFalse, TBool aCancel = EFalse);
	
	// from M3GPParseCallback
	virtual void AudioFramesAvailable(TInt aError, TUint aReturnedFrames, 
			TUint aTimeStampInMs, TUint aTimeStampInTimescale);		
	virtual void  VideoFrameAvailable(TInt aError, TBool aKeyFrame, TUint 
			aTimeStampInMs, TUint aTimeStampInTimescale);
	
protected:
		TBool iWaitForNotification;
		TBool iAsyncLastNotificationFromAudio;
		CActiveSchedulerWait* iSchedulerWait;
		TInt iAsyncError;
		TUint iAsyncAudioTimeStampInMs; 
		TUint iAsyncAudioReturnedFrames;
		TUint iAsyncAudioTimestampInTimescale;
		TUint iAsyncVideoTimeStampInMs; 
		TBool iAsyncVideoKeyFrame;
		TUint iAsyncVideoTimestampInTimescale;
	};

_LIT(KTestCase_0001, "MM-3GP-PARSE-U-0001-CP");
class C3GPLibParser_0001 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0001();
	TVerdict doTestStepL();
private:
	void OpenCompleteL(C3GPParse* aParser);
	};

_LIT(KTestCase_0002, "MM-3GP-PARSE-U-0002-CP");
class C3GPLibParser_0002 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0002();
	TVerdict doTestStepL();
private:
	void OpenCompleteBufferModeL(C3GPParse* aParser);
	};

_LIT(KTestCase_0003, "MM-3GP-PARSE-U-0003-CP");
class C3GPLibParser_0003 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0003();
	TVerdict doTestStepL();
private:
	void OpenCompleteFileHandleL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0004, "MM-3GP-PARSE-U-0004-CP");
class C3GPLibParser_0004 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0004();
	TVerdict doTestStepL();
	};

_LIT(KTestCase_0005, "MM-3GP-PARSE-U-0005-CP");
class C3GPLibParser_0005 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0005();
	TVerdict doTestStepL();
private:
	void GetVideoAudioAttributesL(C3GPParse* aParser);
	};

_LIT(KTestCase_0006, "MM-3GP-PARSE-U-0006-CP");
class C3GPLibParser_0006 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0006();
	TVerdict doTestStepL();
private:
	enum TBufferModeTest
		{
		ENumBufferedBytes,
		EClipProperties,
		EDecoderInfo,
		EVideoFrameProp,
		};
	
	void VideoAudioAttributesBufferModeL(C3GPParse* aParser);
	TInt CallTest(C3GPParse* aParse, TBufferModeTest aTest, TInt aExpectedNumBuffered = 0);
	TInt GetVideoAudioAttributes(C3GPParse* aParse, TBufferModeTest aTest);
	TInt GetVideoFramePropAndNumVideoFrame(C3GPParse* aParse, TBool aExpectPass);
	};

_LIT(KTestCase_0007, "MM-3GP-PARSE-U-0007-CP");
class C3GPLibParser_0007 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0007();
	TVerdict doTestStepL();
private:
	void GetstreamableL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0008, "MM-3GP-PARSE-U-0008-CP");
class C3GPLibParser_0008 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0008();
	TVerdict doTestStepL();
private:
	void GetstreamableL(C3GPParse* aParser);
	};
		
_LIT(KTestCase_0011, "MM-3GP-PARSE-U-0011-CP");
class C3GPLibParser_0011 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0011();
	TVerdict doTestStepL();
private:
	void ReadFrameL(C3GPParse* aParse);
	};

_LIT(KTestCase_0014, "MM-3GP-PARSE-U-0014-CP");
class C3GPLibParser_0014 : public C3GPLib_AsyncObserver
	{
public:
	C3GPLibParser_0014();
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	void ReadFrameAsyncL(C3GPParse* aParser);
	TInt DoReadFrameAsync(C3GPParse* aParser, TBool aCancel = EFalse);
private:
	CActiveScheduler* iScheduler;
	};

_LIT(KTestCase_0015, "MM-3GP-PARSE-U-0015-CP");
class C3GPLibParser_0015 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0015();
	TVerdict doTestStepL();
private:
	void ReadFrameL(C3GPParse* aParser);
	};

_LIT(KTestCase_0016, "MM-3GP-PARSE-U-0016-CP");
class C3GPLibParser_0016 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0016();
	TVerdict doTestStepL();
private:
	void GetFrameTypeL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0017, "MM-3GP-PARSE-U-0017-CP");
class C3GPLibParser_0017 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0017();
	TVerdict doTestStepL();
private:
	void GetQcelModeL(C3GPParse* aParser);
	};

_LIT(KTestCase_0021, "MM-3GP-PARSE-U-0021-CP");
class C3GPLibParser_0021 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0021();
	TVerdict doTestStepL();
private:
	void GetH263LevelL(C3GPParse* aParser);
	};

_LIT(KTestCase_0023, "MM-3GP-PARSE-U-0023-CP");
class C3GPLibParser_0023 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0023();
	TVerdict doTestStepL();
private:
	void GetDecoderInfoL(C3GPParse* aParser);
	};

_LIT(KTestCase_0027, "MM-3GP-PARSE-U-0027-CP");
class C3GPLibParser_0027 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0027();
	TVerdict doTestStepL();
private:
	void SeekTestL(C3GPParse* aParser);
	TInt ParserSeek(C3GPParse* aParser, TInt aLen, TUint& aAudioPos, TUint& aVideoPos);
	};
		
_LIT(KTestCase_0031, "MM-3GP-PARSE-U-0031-CP");
class C3GPLibParser_0031 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0031();
	TVerdict doTestStepL();
private:
	void SeekTestL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0032, "MM-3GP-PARSE-U-0032-CP");
class C3GPLibParser_0032 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0032();
	TVerdict doTestStepL();
private:
	void GetFrameAvailabilityL(C3GPParse* aParser);
	void DoGetFrameAvailabilityL(C3GPParse* aParser);
	};

_LIT(KTestCase_0033, "MM-3GP-PARSE-U-0033-CP");
class C3GPLibParser_0033 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0033();
	TVerdict doTestStepL();
private:
	void VideoInfoL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0034, "MM-3GP-PARSE-U-0034-CP");
class C3GPLibParser_0034 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0034();
	TVerdict doTestStepL();
private:
	void VideoInfoBufferModeL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0035, "MM-3GP-PARSE-U-0035-CP");
class C3GPLibParser_0035 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0035();
	TVerdict doTestStepL();
private:
	void GetUserDataAtomL(C3GPParse* aParser);
	};
	
_LIT(KTestCase_0036, "MM-3GP-PARSE-U-0036-CP");
class C3GPLibParser_0036 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0036();
	TVerdict doTestStepL();
private:
	void GetUserDataAtomL(C3GPParse* aParser);
	};

_LIT(KTestCase_0037, "MM-3GP-PARSE-U-0037-CP");
class C3GPLibParser_0037 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0037();
	TVerdict doTestStepL();
private:
	void VideoDependencyL(C3GPParse* aParser);
	};

_LIT(KTestCase_0038, "MM-3GP-PARSE-U-0038-CP");
class C3GPLibParser_0038 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0038();
	TVerdict doTestStepL();
private:
	void VideoInfoL(C3GPParse* aParser);
	};

_LIT(KTestCase_0039, "MM-3GP-PARSE-U-0039-CP");
class C3GPLibParser_0039 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0039();
	TVerdict doTestStepL();
private:
	void AudioAndVideoFrameL(C3GPParse* aParser);
	};

_LIT(KTestCase_0041, "MM-3GP-PARSE-U-0041-CP");
class C3GPLibParser_0041 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0041();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();
private:
	void MultipleParserL(C3GPParse* aParser1, C3GPParse* aParser2, C3GPParse* aParser3);
private:
	RBuf iInputFile1;
	RBuf iInputFile2;
	};
	
_LIT(KTestCase_0043, "MM-3GP-PARSE-U-0043-CP");
class C3GPLibParser_0043 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0043();
	TVerdict doTestStepL();
private:
	void SeekBufferModeL(C3GPParse* aParser);
	};

_LIT(KTestCase_0045, "MM-3GP-PARSE-U-0045-CP");
class C3GPLibParser_0045 : public C3GPLib_AsyncObserver
	{
public:
	C3GPLibParser_0045();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();	
private:
	void TestParserWithLargeFileL(C3GPParse* aParser);
	TInt DoReadFrameAsync(C3GPParse* aParser, TBool aCancel = EFalse);
private:
	CActiveScheduler* iScheduler;
	};

_LIT(KTestCase_0047, "MM-3GP-PARSE-U-0047-CP");
class C3GPLibParser_0047 : public C3GPLibParser_base
	{
public:
	C3GPLibParser_0047();
	TVerdict doTestStepL();
private:
	void ReadFrameL(C3GPParse* aParser);
	};

_LIT(KTestUserDataAtomRetrieval, "UserDataAtomRetrieval");
class C3GPUserDataAtomRetrieval : public CTestStep
	{
	public:
		C3GPUserDataAtomRetrieval();
		
		TVerdict doTestStepL();		
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();

	private:
		TInt 				iExpectedNumAtom;
		TUint32 			iAtomType;	
		RFs 				iFs;
		RFile 				iInputFile;
		CDesCArrayFlat* 	iAtomFileArray;
		T3GPUdtaLocation    iAtomLocation;
		TBool				iLargeFile;
		RFile64 			iInputFile64;		
	};

#endif
	