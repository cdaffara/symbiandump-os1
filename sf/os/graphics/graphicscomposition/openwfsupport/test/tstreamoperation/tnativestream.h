// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test class for interop implementation of NativeStream
// 
//

#ifndef TESTNATIVESTREAM_H
#define TESTNATIVESTREAM_H

// INCLUDES
#include <test/tefunit.h>
#include <w32std.h>
#include <khronos_types.h>
#include "surfaceutility.h"

#include <graphics/symbianstream.h>
class CTestExecuteLogger;
class CSurfaceStream;

template <class Referee,class LoggerOwner>
class	SafePointer
	{
public:
	SafePointer(LoggerOwner* aOwner,Referee* aReferee=NULL):
		iOwner(aOwner),	iReferee(aReferee)
		{}
	Referee*	operator=(Referee* aReferee)
		{
		return iReferee=aReferee;
		}
	Referee*&	operator()()
		{
		return iReferee;
		}
	Referee*	operator->()
		{
		if (iReferee==NULL)
			{
			iOwner -> INFO_PRINTF1(_L("Tried to dereference a pointer that is null!"));
			User::Panic(_L("null pointer"),__LINE__);
			}
		return iReferee;
		}
private:
	LoggerOwner* iOwner;
	Referee* iReferee;
	};


// MAIN CLASS DECLARATION

class TGlobalNativeStreamVar
	{
public:
	static const TGlobalNativeStreamVar& Instance();
	
	void SetSurfaceID(TSurfaceId aSurfaceID);
	void SetTestComplete (TBool aTestComplete);
	void SetBuffers (TInt aBuffers);
	
	TSurfaceId SurfaceID() const;
	TBool TestComplete() const;
	TInt Buffers() const;
	
public:
	TSurfaceId		iSurfaceID;
	TBool			iTestComplete;
	TInt			iBuffers;
	TInt            iMultithreadCounter;
	};

struct TCallbackData
    {
    SymbianStreamType		iStream;
    TInt32	                iEvent;
    TAny*					iData;
    };

/**
 *  CTestNativeStream
 * 
 */
class CTestNativeStream : public CTestFixture
	{
public:
	// Constructors and destructor

	/**
	 * Constructor for performing 1st stage construction
	 */
	CTestNativeStream();

	/**
	 * Destructor.
	 */
	~CTestNativeStream();

	/**
	 * test Suite furniture 
	 **/
	virtual void SetupL();
	
	/**
	 * test Suite furniture 
	 **/
	virtual void TearDownL();


	/**
	 * test Suite furniture 
	 **/
	static CTestSuite* CreateSuiteL(const TDesC& aName);
		
	/**
	 * Checks acquire or find native streams operations
	 **/
	SymbianStreamType NsCheckL(const TSurfaceId aId, TInt aCheck, TBool aFind);
	
	/**
	 * For concurrent tests, create a shared stream for all threads to access.
	 */
	void CreateSharedNativeStreamL(TInt aBuffers);
	
	/**
	 * For concurrent tests, destroy the shared stream.
	 */	
	void DestroySharedNativeStreamL();
	
	static void SourceStreamUpdatedCallback(SymbianStreamType aNs, 
	        khronos_int32_t aEvents, void* aData, void* aParam);
	
	void CheckSourceStreamUpdated(SymbianStreamType aNs, void* aParam);
	void RemoveNsNotifications();

	/**
     * A helper function that returns the bytes per pixel for a given pixel format UID
     * @param aPixelFormat Pixel format UID to convert
     * @return Positive: bytes per pixel; negative is pixels per byte; 0 is error 
     */
    
    TInt BytesPerPixel(TUidPixelFormat aPixelFormat);
	
private:
	// Private tests
	void GRAPHICS_OPENWFC_NATIVESTREAM_0100L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0101L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0102L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0103L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0104L(TInt aNumBuffers);
	void GRAPHICS_OPENWFC_NATIVESTREAM_0105L(TInt aNumBuffers);
	void GRAPHICS_OPENWFC_NATIVESTREAM_0106L(TInt aNumBuffers);
	void GRAPHICS_OPENWFC_NATIVESTREAM_0107_1L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0107_2L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0107_3L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0108L(TInt aNumBuffers);
	void GRAPHICS_OPENWFC_NATIVESTREAM_0109L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0110L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0111L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0112_1L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0112_2L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0112_3L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0112_4L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0113L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0114L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0115L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0116L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0117_1L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0117_2L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0117_3L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0117_4L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0118_1L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0118_2L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0118_3L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0118_4L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0119_1L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0119_2L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0119_3L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0120_1L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0120_2L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0120_3L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0122L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0130L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0131L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0132L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0133L();
	void GRAPHICS_OPENWFC_NATIVESTREAM_0140L(TInt aNumBuffers);
	void GRAPHICS_OPENWFC_NATIVESTREAM_0141L(TInt aNumBuffers);
	void GRAPHICS_OPENWFC_NATIVESTREAM_0142L();
    void GRAPHICS_OPENWFC_NATIVESTREAM_0143L();
    void GRAPHICS_OPENWFC_NATIVESTREAM_0144L();
	
private:

	struct OWF_IMAGE_FORMAT;
	TSurfaceId helperCreateSurfaceL(khronos_int32_t width,
	        khronos_int32_t height,
	        const OWF_IMAGE_FORMAT*   format,
	        khronos_int32_t nbufs,
	        TUidPixelFormat overridePixelFormat = EUidPixelFormatUnknown);
	
	SymbianStreamType helperCreateImageStream(khronos_int32_t width,
			khronos_int32_t height,
			const OWF_IMAGE_FORMAT*  format,
			khronos_int32_t nbufs,
			TUidPixelFormat overridePixelFormat = EUidPixelFormatUnknown);

private:
	//data
	SafePointer<CSurfaceUtility,CTestNativeStream> iUtility;
	khronos_int32_t iExpectedSourceStreamUpdatedEventMask;
    khronos_int32_t iImmediateAvailable;
    khronos_int32_t iImmediateVisible;
    khronos_int32_t iStreamUpdatedParameter;
    khronos_int32_t iStreamUpdatedSerialNumber;
	SymbianStreamType iNs;
	TInt32 iScreenNo;
	void* iObserverAvailableParameter;
	void* iObserverDisplayedParameter;
	void* iObserverDisplayedXParameter;
	khronos_int32_t iSourceStreamUpdatedCalled;
	khronos_int32_t iContextUpdatedFlags;
public:
	static CTestNativeStream* iTester;
	};

_LIT(KComposeThreadSemaphore, "Composer Thread Semaphore");
_LIT(KMainThreadSemaphore, "Main Thread Semaphore");

#endif // TESTNATIVESTREAM_H
