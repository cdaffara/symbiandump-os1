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

#ifndef TSU_3GPLIBRARY_COMPOSER_API_H
#define TSU_3GPLIBRARY_COMPOSER_API_H

#include <test/testexecutestepbase.h>
#include <c3gplibrary.h>

class CActiveScheduler;

//
// Base class for testing composer.  Provides a framework for:
//
// 1. Setup
// 2. Add video/audio data
// 3. Add user data
//
class C3GPLibComposeBase : public CTestStep
	{
	public:
		C3GPLibComposeBase();
		
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();
		TVerdict doTestStepL();
		
	protected:		
		virtual C3GPCompose* doTestStepCreateComposerL();

		virtual void doTestStepComposeOpenL(C3GPCompose& aComposer) = 0;
		
		// default to do nothing & return the current test result
		virtual void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);		
		virtual void doTestStepComposeSetUserDataL(C3GPCompose& aComposer);		

	private:
		CActiveScheduler* iScheduler;
	};

//
// Setup a composer via filepath supplied by the configuration file
// 
_LIT(K3GPComposeFilename, "3GPComposeFilename");
class C3GPLibComposeFilename : public C3GPLibComposeBase
	{
	public:
		C3GPLibComposeFilename();
		
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();	

	protected:
		virtual void doTestStepComposeOpenL(C3GPCompose& aComposer);

	protected:
		RBuf iFilename;
	};

//
// Class to test execution of composer APIs without any setup.
//
_LIT(K3GPComposeWithoutOpen, "3GPComposeWithoutOpen");
class C3GPLibComposeWithoutOpen : public C3GPLibComposeBase
	{
	public:
		C3GPLibComposeWithoutOpen();
		
	private:
		// virtual functions overriding base class implementations
		void doTestStepComposeOpenL(C3GPCompose& aComposer);		
		void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);		
		void doTestStepComposeSetUserDataL(C3GPCompose& aComposer);				
	};

//
// Class to test composer with video/audio properties supplied by 
// the configuration file.
//
_LIT(K3GPComposeFile, "3GPComposeFile");
class C3GPLibComposeFile : public C3GPLibComposeBase
	{
	public:
		C3GPLibComposeFile();
		
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();	

	protected:
		virtual void doTestStepComposeOpenL(C3GPCompose& aComposer);		
		virtual void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);
		
		T3GPVideoPropertiesBase* SetupVideoPropertiesL();
		virtual T3GPVideoPropertiesBase* SetupAvcVideoL();
		virtual T3GPVideoPropertiesBase* SetupH263VideoL();
		virtual T3GPVideoPropertiesBase* SetupMpeg4VideoL();
		
		T3GPAudioPropertiesBase* SetupAudioPropertiesL();
		virtual T3GPAudioPropertiesBase* SetupQcelpAudioL();
		virtual T3GPAudioPropertiesBase* SetupAmrAudioL();
		virtual T3GPAudioPropertiesBase* SetupMpeg4AudioL();		
		
	protected:
		T3GPFileFormatType iInputFileFormat;	// requested file format
		T3GPVideoType iVideoType;
		T3GPAudioType iAudioType;
		RFs iFs;
		RFile iFile;
		RBuf iFileName;
		RFile64 iFile64;		
	};

//
// Class to compose file conforms to the expected file format as supplied by 
// the test configuration.
//
_LIT(K3GPComposeFileWithFileFormatCheck, "3GPComposeFileWithFileFormatCheck");
class C3GPLibComposeFileWithFileFormatCheck : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeFileWithFileFormatCheck();
		
		TVerdict doTestStepPreambleL();		
		TVerdict doTestStepL();	

	private:
		TVerdict VerifyFileFormatL(const RFile& aFile);

	private:
		T3GPFileFormatType iOutputFileFormat;	// expected file format		
	};

//
// Class to compose file with video frames of all possible combinations of different 
// dependencies 
//
_LIT(K3GPComposeFrameDependency, "3GPComposeFrameDependency");
class C3GPLibComposeFrameDependency : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeFrameDependency();
		
	private:
		void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);
	};

//
// Class to compose file with video properties as specified in the test
// configuration
//
_LIT(K3GPComposeVideoProperties, "3GPComposeVideoProperties");
class C3GPLibComposeVideoProperties : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeVideoProperties();		
		
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();	
		
	private:
		virtual T3GPVideoPropertiesBase* SetupAvcVideoL();
		virtual T3GPVideoPropertiesBase* SetupH263VideoL();
		virtual T3GPVideoPropertiesBase* SetupMpeg4VideoL();
		
	private:
		TSize iSize;
		TUint iMaxBitRate;
		TUint iAvgBitRate;
		RBuf8 iDecoderSpecInfo;
		TUint iTimescale;
		TInt iVideoLevel;
	};

//
// Class to compose file with audio properties as specified in the test
// configuration
//
_LIT(K3GPComposeAudioProperties, "3GPComposeAudioProperties");
class C3GPLibComposeAudioProperties : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeAudioProperties();		
		
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();	
		
	private:
		T3GPAudioPropertiesBase* SetupQcelpAudioL();
		T3GPAudioPropertiesBase* SetupAmrAudioL();
		T3GPAudioPropertiesBase* SetupMpeg4AudioL();		
		
	private:
		TInt iFps;
		RBuf8 iDecoderSpecInfo;
		TInt iTimescale;
		T3GPQcelpStorageMode iQCelpMode;
		TInt iModeSet;		
	};

//
// Class to create composer using buffer sizes and counts specified by test
// configuration
//
_LIT(K3GPComposeWithSpecificBufferSize, "3GPComposeWithSpecificBufferSize");
class C3GPLibComposeWithSpecificBufferSize : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeWithSpecificBufferSize();		
		
		TVerdict doTestStepPreambleL();		
		
	private:
		C3GPCompose* doTestStepCreateComposerL();
		
	private:
		TInt iBufferSize;
		TInt iBufferMaxCount;
	};

//
// Class to compose file with user data as specified by the test 
// configuration
//
_LIT(K3GPComposeUserData, "3GPComposeUserData");
class C3GPLibComposeUserData : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeUserData();	
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();
		
	private:	
		void doTestStepComposeSetUserDataL(C3GPCompose& aComposer);
		
	private:
		RBuf8 iUserData;		
		TBool iUdtaMoov;
		TBool iUdtaVideo;
		TBool iUdtaAudio;	
		TInt  iUdtaLocation;
	};

//
// Class to compose file using different composer flags as specified in the test 
// configuration
//
_LIT(K3GPComposeWithFlag, "3GPComposeWithFlag");
class C3GPLibComposeWithFlag : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeWithFlag();
		TVerdict doTestStepPreambleL();
		
	protected:	
		virtual void doTestStepComposeOpenL(C3GPCompose& aComposer);
		
	protected:
		TInt iComposeFlag;
	};

//
// Class to compose file specifying no video nor audio.
//
_LIT(K3GPComposeWithNoAudioVideo, "3GPComposeWithNoAudioVideo");
class C3GPLibComposeWithNoAudioVideo : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeWithNoAudioVideo();
		TVerdict doTestStepPreambleL();		
	};

//
// Class to compose file with audio frames of properties specified by test
// configuration
//
_LIT(K3GPComposeAudioFrames, "3GPComposeAudioFrames");
class C3GPLibComposeAudioFrames : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeAudioFrames();
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();	
		
	private:
		void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);		
	
	private:
		TInt iDuration;
		RBuf8 iAudioFrames;
	};

//
// Class to compose file with video frames of properties specified by test
// configuration
//
_LIT(K3GPComposeVideoFrame, "3GPComposeVideoFrame");
class C3GPLibComposeVideoFrame : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeVideoFrame();
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();	
		
	private:
		void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);		
	
	private:
		TBool iKeyFrame;
		TInt iDuration;
		RBuf8 iVideoFrame;
		T3GPFrameDependencies* iDependencies;
	};


//
// Class to open the composer again before closing it
//
_LIT(K3GPComposeOpenAgain, "3GPComposeOpenAgain");
class C3GPLibComposeOpenAgain: public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeOpenAgain();
		
	private:
		void doTestStepComposeOpenL(C3GPCompose& aComposer);
	};

//
// Class to setup composer with a Read-Only file via file handle
//
_LIT(K3GPComposeOpenReadOnlyFile, "3GPComposeOpenReadOnlyFile");
class C3GPLibComposeOpenReadOnlyFile: public C3GPLibComposeWithFlag
	{
	public:
		C3GPLibComposeOpenReadOnlyFile();		
		TVerdict doTestStepL();
		
	private:
		void doTestStepComposeOpenL(C3GPCompose& aComposer);
	};

//
// Class to setup composer using file path of an opened file using 
// ShareAny and then ShareReadersOrWriters mode.
//
_LIT(K3GPComposeOpenedFile, "3GPComposeOpenedFile");
class C3GPLibComposeOpenedFile: public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeOpenedFile();
		
	private:
		void doTestStepComposeOpenL(C3GPCompose& aComposer);
	};

//
// Class to use 2 composer opening the same file 
//
_LIT(K3GPComposeMultiComposeOnSameFile, "3GPComposeMultiComposeOnSameFile");
class C3GPLibComposeMultiComposeOnSameFile : public C3GPLibComposeFilename
	{
	public:
		C3GPLibComposeMultiComposeOnSameFile();
		
	private:
		void doTestStepComposeOpenL(C3GPCompose& aComposer);
	};

//
// Class to have multiple composers / parser working at the same time 
//
_LIT(K3GPComposeMultiComposers, "3GPComposeMultiComposers");
class C3GPLibComposeMultiComposers : public C3GPLibComposeFilename
	{
	public:
		C3GPLibComposeMultiComposers();
		
	private:
		void doTestStepComposeOpenL(C3GPCompose& aComposer);
	};

//
// Class to compose file without calling complete first 
//
_LIT(K3GPComposeCloseComposerWithoutComplete, "3GPComposeCloseComposerWithoutComplete");
class C3GPLibComposeCloseComposerWithoutComplete : public C3GPLibComposeFilename
	{
	public:
		C3GPLibComposeCloseComposerWithoutComplete();
		
	private:
		void doTestStepComposeOpenL(C3GPCompose& aComposer);
	};

//
// Class to test cleanup of temporary files when composer panics    
//
_LIT(K3GPComposePanic, "3GPComposePanic");
class C3GPLibComposePanic : public C3GPLibComposeFilename
	{
	public:
		C3GPLibComposePanic();
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();
		
	private:
		void doTestStepComposeOpenL(C3GPCompose& aComposer);
		
	private:
		TBool iPanic;
		RBuf iDir;
	};

//
// Class to compose a large file >2GB
//
_LIT(K3GPComposeLargeFile, "3GPComposeLargeFile");
class C3GPLibComposeLargeFile : public C3GPLibComposeFile
	{
	public:
		C3GPLibComposeLargeFile();		
		
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepPostambleL();	
		
	private:
		C3GPCompose* doTestStepCreateComposerL();

		void doTestStepComposeOpenL(C3GPCompose& aComposer);
		void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);	

	private:
		TInt iComposeFlag;
	};

//
// Class to compose a large file >2GB with user data
//
_LIT(K3GPComposeLargeFileUserData, "3GPComposeLargeFileUserData");
class C3GPLibComposeLargeFileUserData : public C3GPLibComposeLargeFile
	{
	public:
		C3GPLibComposeLargeFileUserData();		
		
	private:
		void doTestStepComposeSetUserDataL(C3GPCompose& aComposer);
		void WriteInt32(TUint8* aPtr, TInt32 aData);	

	private:
	};

//
// Class to compose a large file >2GB using 32bit APIs
//
_LIT(K3GPComposeLargeFile32bitAPI, "3GPComposeLargeFile32bitAPI");
class C3GPLibComposeLargeFile32bitAPI : public C3GPLibComposeWithFlag
	{
	public:
		C3GPLibComposeLargeFile32bitAPI();		
				
	private:
		C3GPCompose* doTestStepCreateComposerL();
		void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);	

	private:
	};

//
// Class to compose a large file on an mmc card until the disk is full
//
_LIT(K3GPComposeLargeFileDiskFull, "3GPComposeLargeFileDiskFull");
class C3GPLibComposeLargeFileDiskFull : public C3GPLibComposeLargeFile
	{
	public:
		C3GPLibComposeLargeFileDiskFull();		
		
	private:
		void doTestStepComposeWriteVideoAudioL(C3GPCompose& aComposer);

	private:
	};

//
// Class to compose file with AVC video that conforms to the expected AVC profile.
//
_LIT(K3GPComposeFileWithAvcProfileCheck, "3GPComposeFileWithAvcProfileCheck");
class C3GPLibComposeFileWithAvcProfileCheck : public C3GPLibComposeFile
    {
    public:
        C3GPLibComposeFileWithAvcProfileCheck();
        virtual T3GPVideoPropertiesBase* SetupAvcVideoL();
        TVerdict doTestStepL(); 

    private:
        TVerdict VerifyAvcProfileL(const RFile& aFile);
        
    private:
        TBuf8<7> iAvcDecoderConfigRecord;
    };


#endif // TSU_3GPLIBRARY_COMPOSER_API_H

