// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Load related tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_load.h
*/

#ifndef TSU_MDF_CODECAPI_LOAD_H
#define TSU_MDF_CODECAPI_LOAD_H

#include "tsu_mdf_codecapi_step.h"
#include <mmf/server/mmfhwdevice.h>

/**
 * 
 * Abstract base class for codec loading
 *
 * @class RTestStepCodecApiLoad 
 *
 */
class RTestStepCodecApiLoad : public RTestStepCodecApi,  public MMMFHwDeviceObserver
	{
public:
	virtual TInt FillThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/) {return 0;}
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/) {return 0;}
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped() {}
	virtual void Error(TInt /*aError=KErrNone*/) {}

	RTestStepCodecApiLoad() {}
	virtual TVerdict DoTestStepL() = 0;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0061-HP
 * Create a hardware device adapter
 *
 * @class RTestStepCodecApiLoadAdapter 
 *
 */
class RTestStepCodecApiLoadAdapter : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadAdapter();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0062-HP
 * Load a custom interface
 *
 * @class RTestStepCodecApiLoadCustomInterface 
 *
 */
class RTestStepCodecApiLoadCustomInterface : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCustomInterface();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0065-HP
 * Use the HwDeviceAdapter to load a codec PU 
 *
 * @class RTestStepCodecApiLoadCodec 
 *
 */
class RTestStepCodecApiLoadCodec : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCodec();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0066-HP
 * Use the HwDeviceAdapter to load and initialize a codec PU 
 *
 * @class RTestStepCodecApiLoadInitCodec 
 *
 */
class RTestStepCodecApiLoadInitCodec : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadInitCodec();
	virtual TVerdict DoTestStepL();
	};

//
// stop / pause tests

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0120-HP
 * Call Stop on a device after instantiation
 *
 * @class RTestStepCodecApiLoadStop 
 *
 */
class RTestStepCodecApiLoadStop : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadStop();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0121-HP
 * Call Stop on a device after custom interface creation
 *
 * @class RTestStepCodecApiLoadCIStop 
 *
 */
class RTestStepCodecApiLoadCIStop : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCIStop();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0122-HP
 * Call Stop on a device after setup
 *
 * @class RTestStepCodecApiLoadCISetupStop 
 *
 */
class RTestStepCodecApiLoadCISetupStop : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISetupStop();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0123-HP
 * Call Stop on a device after initialization
 *
 * @class RTestStepCodecApiLoadCISetupInitStop 
 *
 */
class RTestStepCodecApiLoadCISetupInitStop : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISetupInitStop();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0130-HP
 * Call Pause on a device after instantiation
 *
 * @class RTestStepCodecApiLoadPause 
 *
 */
class RTestStepCodecApiLoadPause : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadPause();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0131-HP
 * Call Pause on a device after custom interface creation
 *
 * @class RTestStepCodecApiLoadCIPause 
 *
 */
class RTestStepCodecApiLoadCIPause : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCIPause();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0132-HP
 * Call Pause on a device after setup
 *
 * @class RTestStepCodecApiLoadCISetupPause 
 *
 */
class RTestStepCodecApiLoadCISetupPause : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISetupPause();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0133-HP
 * Call Pause on a device after initialization
 *
 * @class RTestStepCodecApiLoadCISetupInitPause 
 *
 */
class RTestStepCodecApiLoadCISetupInitPause : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISetupInitPause();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0140-HP
 * Call StopAndDeleteCodec on a device after instantiation
 *
 * @class RTestStepCodecApiLoadSDC 
 *
 */
class RTestStepCodecApiLoadSDC : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadSDC();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0141-HP
 * Call StopAndDeleteCodec on a device after custom interface creation
 *
 * @class RTestStepCodecApiLoadCISDC 
 *
 */
class RTestStepCodecApiLoadCISDC : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISDC();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0142-HP
 * Call StopAndDeleteCodec on a device after setup
 *
 * @class RTestStepCodecApiLoadCISetupSDC 
 *
 */
class RTestStepCodecApiLoadCISetupSDC : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISetupSDC();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0143-HP
 * Call StopAndDeleteCodec on a device after initialization
 *
 * @class RTestStepCodecApiLoadCISetupInitSDC 
 *
 */
class RTestStepCodecApiLoadCISetupInitSDC : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISetupInitSDC();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0150-HP
 * Call DeleteCodec on a device after instantiation
 *
 * @class RTestStepCodecApiLoadDeleteCodec 
 *
 */
class RTestStepCodecApiLoadDeleteCodec : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadDeleteCodec();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0151-HP
 * Call DeleteCodec on a device after custom interface creation
 *
 * @class RTestStepCodecApiLoadCIDeleteCodec 
 *
 */
class RTestStepCodecApiLoadCIDeleteCodec : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCIDeleteCodec();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0152-HP
 * Call DeleteCodec on a device after setup
 *
 * @class RTestStepCodecApiLoadCISetupDeleteCodec 
 *
 */
class RTestStepCodecApiLoadCISetupDeleteCodec : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISetupDeleteCodec();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0153-HP
 * Call DeleteCodec on a device after initialization
 *
 * @class RTestStepCodecApiLoadCISetupInitDeleteCodec 
 *
 */
class RTestStepCodecApiLoadCISetupInitDeleteCodec : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCISetupInitDeleteCodec();
	virtual TVerdict DoTestStepL();
	};
	
//
// negative tests

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1061-HP
 * Attempt to create a Hw Device Adapter with an invalid Uid.
 *
 * @class RTestStepCodecApiLoadAdapterInvalid 
 *
 */
class RTestStepCodecApiLoadAdapterInvalid : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadAdapterInvalid();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1062-HP
 * Attempt to create a custom interface with an invalid Uid.
 *
 * @class RTestStepCodecApiLoadCustomInterfaceInvalid 
 *
 */
class RTestStepCodecApiLoadCustomInterfaceInvalid : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCustomInterfaceInvalid();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1065-HP
 * Attempt to load a codec using invalid FourCC codes.
 *
 * @class RTestStepCodecApiLoadCodecInvalid 
 *
 */
class RTestStepCodecApiLoadCodecInvalid : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadCodecInvalid();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1066-HP
 * Attempt to initialize a codec without creating a Custom Interface.
 *
 * @class RTestStepCodecApiLoadInitCodecInvalid 
 *
 */
class RTestStepCodecApiLoadInitCodecInvalid : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadInitCodecInvalid();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1067-HP
 * Attempt to initialize a device without loading a codec.
 *
 * @class RTestStepCodecApiLoadInitCodec2Invalid 
 *
 */
class RTestStepCodecApiLoadInitCodec2Invalid : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadInitCodec2Invalid();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1068-HP
 * Attempt to initialize a device without setting an observer.
 *
 * @class RTestStepCodecApiLoadInitCodec3Invalid 
 *
 */
class RTestStepCodecApiLoadInitCodec3Invalid : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadInitCodec3Invalid();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1069-HP
 * Attempt to load an OpenMax PU without initialising OpenMax.
 *
 * @class RTestStepCodecApiLoadNoOpenMax 
 *
 */
class RTestStepCodecApiLoadNoOpenMax : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadNoOpenMax();
	virtual TVerdict DoTestStepL();
	};
	
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1100-HP
 * Attempt to start a decode with incorrect arguments.
 *
 * @class RTestStepCodecApiLoadStartInvalid 
 *
 */
class RTestStepCodecApiLoadStartInvalid : public RTestStepCodecApiLoad
	{
public:
	RTestStepCodecApiLoadStartInvalid();
	virtual TVerdict DoTestStepL();
	};

#endif // TSU_MDF_CODECAPI_LOAD_H
