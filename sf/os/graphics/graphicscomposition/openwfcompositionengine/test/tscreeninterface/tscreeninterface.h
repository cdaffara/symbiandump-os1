// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
//

#ifndef TSCREENINTERFACE_H
#define TSCREENINTERFACE_H

// INCLUDES
#include <test/tefunit.h>
#include <w32std.h>
#include <khronos_types.h>
#include <graphics/symbianstream.h>
#include "streamutility.h"

class CTestExecuteLogger;
class CSurfaceStream;
/**
 *  CTestScreenInterface
 * 
 */
class CTestScreenInterface : public CTestFixture
	{
public:
	CTestScreenInterface();
	~CTestScreenInterface();

	virtual void SetupL();
	virtual void TearDownL();
	void TearDown();
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
    enum TCacheAttribute
        {
        /** not CPU cached */
        ENotCached = 0,
        /** CPU cached */
        ECached = 1,
        /** Not listed */
        ECacheNotlisted = 2
        };

    enum TContiguousFlag
        {
        ENonContiguous = 0,
        EContiguous = 1,
        EFlagNotListed = 2
        };

private:
    void ReadIniData();
    TInt BaseTestContiguousFlag(TBool aContiguous);
	// Private tests
	void GRAPHICS_OPENWFC_SCREENINTERFACE_0100L();
	void GRAPHICS_OPENWFC_SCREENINTERFACE_0101L();
	void GRAPHICS_OPENWFC_SCREENINTERFACE_0102L();
	void GRAPHICS_OPENWFC_SCREENINTERFACE_0103L();
    void GRAPHICS_OPENWFC_SCREENINTERFACE_0104L();
    void GRAPHICS_OPENWFC_SCREENINTERFACE_0105L();
    void GRAPHICS_OPENWFC_SCREENINTERFACE_0106L();
    void GRAPHICS_OPENWFC_SCREENINTERFACE_0107L();
    void GRAPHICS_OPENWFC_SCREENINTERFACE_0108L();
    void GRAPHICS_OPENWFC_SCREENINTERFACE_0109L();

private:
	CStreamUtility* iUtility;
	TRgb iRed, iGreen, iBlue;
    TInt iFastpathableWidth;
    TInt iFastpathableHeight;
    TInt iStride;        
    TContiguousFlag iContiguous;
    TUidPixelFormat iFastpathablePixelFormat;
    TUidPixelFormat iNonFastpathablePixelFormat;
    TInt iOffsetToFirstBuffer; 
    TInt iAlignment;           
    TBool iMappable;
    TInt iMaxBuffers;              
    TInt iMinBuffers;
    TCacheAttribute iCacheAttrib;
    TInt iOffsetBetweenBuffers;

	};

#endif // TSCREENINTERFACE_H
