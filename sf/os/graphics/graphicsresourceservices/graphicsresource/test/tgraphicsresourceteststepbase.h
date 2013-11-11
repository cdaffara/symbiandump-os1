// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TGRAPHICSRESOURCETESTSTEPBASE_H
#define TGRAPHICSRESOURCETESTSTEPBASE_H

#include "sgimage.h"
#include "sgimagecollection.h"
#include "sgresource.h"
#include "tgraphicsresourcemultiprocessthread.h"
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <graphics/sgimage_sw.h>
#include <graphics/sgimage_chunk.h>
#include <test/ttmsgraphicsstep.h>


_LIT(KSgTestGenericPanicCategory, "SGRES");
_LIT(KSgTestAdapterPanicCategory, "SGRES-ADAPTER");
const TInt KMaxPixelFormats = 64;

class CTSgTestStepBase : public CTTMSGraphicsStep
	{
public:
	CTSgTestStepBase();
	~CTSgTestStepBase();
	IMPORT_C void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine);
protected:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();	
	
	//common utility functions
	void CreateImageL(RSgImage& aImage);
	void CreateImageCollectionL(RSgImageCollection& aCollection);
	TInt CreateSecondProcessAndDoTestL(const TDesC &aProcessName, TSgresTestInfo& aTestInfo);
	TInt CreateSecondThreadAndDoTestL(TSgresTestInfo aTestInfo);
	void CreateSecondThreadAndCheckPanicL(TSgresTestInfo aTestInfo, TInt aPanicCode, TExitCategoryName aExitCategory, const TDesC &aThreadName);
	void CreateSecondProcessAndCheckPanicL(TSgresTestInfo aTestInfo, TInt aPanicCode, TExitCategoryName aExitCategory, const TDesC &aProcessName);
	void CallGetPixelFormatsL(TSgCpuAccess aCpuAccess, TUint32 aUsage, TBool aShareable, TInt aScreenId);
	void TestGetPixelFormatCompatibilityGuaranteesL();
	void CheckPixelFormatPresent(TUidPixelFormat aPixelFormat);
	void TestOpenDriverL();
	void TestCloseDriver();
	static TBool CompareInfos(TSgImageInfo& info1, TSgImageInfo& info2);
	static TInt SecondThreadStart(TAny* aInfo);
	static void CloseDriverWhenLeave(TAny* aInfo);
	void CheckErrorL(TInt aExpectedErrorCode, TInt aActualErrorCode, const TText8* aFile, TInt aLine);
	
	//OOM test functions
	void TestOOM();
	virtual void DoMemoryTestsL();
	
protected:	
	RThread iSecondThread;
	TBool iRunningOomTests;
private:
	TInt iPixelFormatCount;
	TUidPixelFormat iPixelFormatArray[KMaxPixelFormats];
	};

//common variables and functions
const TUint16 KImageData[] = {
			0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,
			0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0,0x7E0
			};

const TInt KNumImagesInCollection = 10;

#define TESTWITHFILENAMEANDLINENUMBERL(a, f, l) \
	{\
	testBooleanTrueL((a), f, l); \
	}

#if	defined(__WINS__)
_LIT(KSecondProcess, "tgraphicsresourcesecondprocess.exe");
#elif defined(SYMBIAN_GRAPHICS_USE_GPU_MEMORY)
_LIT(KSecondProcess, "tgraphicsresourcesecondprocess_mbx.exe");
#else
_LIT(KSecondProcess, "tgraphicsresourcesecondprocess_sw.exe");
#endif

_LIT(KSecondThread, "Test_GraphicsResource_SecondThread");
_LIT(KSecondThreadSemaphore, "Second Thread Semaphore");

#endif
