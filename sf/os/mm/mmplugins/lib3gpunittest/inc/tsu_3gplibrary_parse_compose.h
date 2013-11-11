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

#ifndef TSU_3GPLIBRARY_PARSE_COMPOSER_H
#define TSU_3GPLIBRARY_PARSE_COMPOSER_H

#include <test/testexecutestepbase.h>
#include <c3gplibrary.h>

//
// C3GPLibParseComposeFile takes a directory path specified by the test configuration and 
// parses the video & audio data within the single file inside the directory.  Optionally,
// this class can parse then use the data retrieved to compose a copy of the original file.  
// The output files will reside in a user-specificed output directory, using the same name as the
// original file.  An extra option is to compare the input & output files' video & audio data.
// 
_LIT(K3GPParseComposeFile, "3GPParseComposeFile");
class C3GPLibParseComposeFile : public CTestStep
	{
	public:
		C3GPLibParseComposeFile();
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepL();
		TVerdict doTestStepPostambleL();
		
	protected:
		TInt ReadVideoFrame(C3GPParse& aParser,
							RBuf8& aVideoBuffer, 
							TBool& aVideoKeyFrame, 
							TUint& aVideoTimestampInMS, 
							T3GPFrameDependencies& aDependencies);
		
		TInt ReadAudioFrames(C3GPParse& aParser,
							 RBuf8& aBuffer, 
							 TInt& aFramesInSample,
							 TUint& aTimestampInTS);

		virtual void ParseFileL(const TDesC& aInputFile);
		
	private:
		
		// file properties
		TInt ParseVideoProperties(C3GPParse& aParser,
								  RBuf8& aDecoderSpecificInfo,				
								  T3GPVideoPropertiesBase*& aProperties,
								  TUint& aLengthInMs);
		TInt ParseAudioProperties(C3GPParse& aParser, 
								  RBuf8& aDecoderSpecificInfo,				
							      T3GPAudioPropertiesBase*& aProperties,
							      TUint& aLengthInMs);
		TInt GetAudioDecoderSpecificInfo(RBuf8& aBuffer);
		TInt GetVideoDecoderSpecificInfo(RBuf8& aBuffer);		
		
		// video / audio data
		void ReadWriteAudioVideoFramesL(const T3GPVideoPropertiesBase* aVideoProperties, 
									    const T3GPAudioPropertiesBase* aAudioProperties,
									    TUint aVideoLengthInMs, 
									    TUint aAudioLengthInMs);				
		
		TBool CompareInputOuputFileL(const TDesC& aInputFile);
		TBool CompareVideoPropertiesL(C3GPParse& aParser1, 
									  C3GPParse& aParser2);
		TBool CompareAudioPropertiesL(C3GPParse& aParser1, 
								      C3GPParse& aParser2);
		TBool CompareAudioVideoData(C3GPParse& aParser1, 
									C3GPParse& aParser2);
		TBool CompareAudioFrames(C3GPParse& aParser1, 
								 C3GPParse& aParser2);		
		TBool CompareVideoFrame(C3GPParse& aParser1, 
								C3GPParse& aParser2);
																		
	private:
		RBuf iOutputDir;		
		RBuf iInputDir;
		RBuf iComposedFile;
		
		TInt iFileFormat;				
		TInt iComposeFlag;
		TBool iCompareOriginal;		
		TInt iExpectedNumberOfFailure;
		
		CDir* iDirList;
		C3GPCompose* iComposer;
		C3GPParse* iParser;
		
		RFs	iFs;
		CActiveScheduler* iScheduler;
		
		TBool iLargeFile;
		RFile64 iParsedFileHandle64;		
		RFile64 iComposedFileHandle64;		
	};

#endif	// TSU_3GPLIBRARY_PARSE_COMPOSER_H
