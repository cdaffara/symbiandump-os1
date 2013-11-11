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
//

#include "tsu_mdf_codecapi_opaque.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/codecapiresolverdata.h>
#include <mdf/codecapiuids.hrh>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/codecapiresolverutils.h>
#include <mdf/codecapivideoresolverutils.h>
#include "../../../../mdf/src/codecapi/codecapiresolverconsts.h"
#include <mmf/devvideo/devvideobase.h>

//
// RTestStepCodecApiOpaqueDataL
// Create opaque data with an L tag.

/**
 * Constructor
 */
RTestStepCodecApiOpaqueDataL::RTestStepCodecApiOpaqueDataL()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0300-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiOpaqueDataL::DoTestStepL()
	{
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test Opaque Data - L tag"));

	CCodecApiVideoOpaqueData* parse = NULL;
	_LIT8(KRateSize, "<l>10.0,352,288");
	const TDesC8* opaqueData = &KRateSize;
		
	TRAPD(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		return EFail;
		}	
	CleanupStack::PushL(parse);
	
	// we'll check these here for zero - for sake of code coverage
	const TDesC8 inputData = parse->InputDataType();
	const TDesC8 outputData = parse->OutputDataType();

	// this may be unsupported so don't worry about the return value	
	TSize maxSize = parse->MaxPictureSize();
	
	RArray<TPictureRateAndSize> maxRates = parse->MaxPictureRates();	
	
	TReal rate = maxRates[0].iPictureRate;
	TInt width = maxRates[0].iPictureSize.iWidth;
	TInt height = maxRates[0].iPictureSize.iHeight;
	
	CleanupStack::PopAndDestroy(); // parse		

	REComSession::FinalClose();
	__UHEAP_MARKEND;

	if(rate != 10.0 || width != 352 || height != 288) 
		{
		INFO_PRINTF1(_L("Opaque data did not return the correct values"));
		return EFail;
		}

	if (inputData.CompareF(KNullDesC8) != 0)
		{
		INFO_PRINTF1(_L("Input data type was nonzero"));		
		return EFail;
		}
		
	if (outputData.CompareF(KNullDesC8) != 0)
		{
		INFO_PRINTF1(_L("Output data type was nonzero"));		
		return EFail;
		}	
	
	INFO_PRINTF1(_L("Opaque data ok"));
	return EPass;
	}
	

//
// RTestStepCodecApiOpaqueDataP
// Create opaque data with a P tag.

/**
 * Constructor
 */
RTestStepCodecApiOpaqueDataP::RTestStepCodecApiOpaqueDataP()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0301-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiOpaqueDataP::DoTestStepL()
	{
	TVerdict verdict = EPass;
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test Opaque Data - P tag"));

	CCodecApiVideoOpaqueData* parse = NULL;
	_LIT8(KMaxPicSize, "<p>100,200");
	const TDesC8* opaqueData = &KMaxPicSize;
	INFO_PRINTF1(_L("Testing valid &lt;p&gt; tag"));
	TRAPD(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error==KErrNone)
		{
		CleanupStack::PushL(parse);
		TSize maxSize = parse->MaxPictureSize();
		CleanupStack::PopAndDestroy(parse); // parse		
		parse = NULL;
		if(maxSize.iWidth != 100 || maxSize.iHeight != 200) 
			{
			INFO_PRINTF1(_L("Opaque data did not return the correct values"));
			verdict = EFail;
			}
		}	
	else
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}
	// get opaque data values in hex format
	_LIT8(KMaxPicSize1, "<p>0x100, 0x200");
	opaqueData = &KMaxPicSize1;
	INFO_PRINTF1(_L("Testing valid hex &lt;p&gt; tag"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error==KErrNone)
		{
		CleanupStack::PushL(parse);
		TSize maxSize = parse->MaxPictureSize();
		CleanupStack::PopAndDestroy(parse); // parse		
		parse = NULL;
		if(maxSize.iWidth != 0x100 || maxSize.iHeight != 0x200) 
			{
			INFO_PRINTF1(_L("Opaque data did not return the correct values"));
			verdict = EFail;
			}		
		}
	else
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}	
		
	_LIT8(KMaxPicSize2, "<p>0X100,0X200");
	opaqueData = &KMaxPicSize2;
	INFO_PRINTF1(_L("Testing valid &lt;p&gt; tag with 0X"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error==KErrNone)
		{
		CleanupStack::PushL(parse);
		TSize maxSize = parse->MaxPictureSize();
		CleanupStack::PopAndDestroy(); // parse		
		if(maxSize.iWidth != 0x100 || maxSize.iHeight != 0x200) 
			{
			INFO_PRINTF1(_L("Opaque data did not return the correct values"));
			verdict = EFail;
			}
		
		}
	else
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}	
		
	_LIT8(KMaxPicSize3, "<p>0xocubes");
	opaqueData = &KMaxPicSize3;
	INFO_PRINTF1(_L("Testing invalid hex &lt;p&gt; tag"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error!=KErrGeneral && error != KErrCorrupt)
		{
		INFO_PRINTF2(_L("Unexpected error %d"), error);
		verdict = EFail;
		}	

	_LIT8(KMaxPicSize4, "<p>0x");
	opaqueData = &KMaxPicSize4;
	INFO_PRINTF1(_L("Testing truncated hex &lt;p&gt; tag"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error!=KErrCorrupt)
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}	

	_LIT8(KMaxPicSize5, "<p>");
	opaqueData = &KMaxPicSize5;
	INFO_PRINTF1(_L("Testing empty &lt;p&gt; tag"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error!=KErrCorrupt)
		{
		INFO_PRINTF2(_L("Unexpected error %d"), error);
		verdict = EFail;
		}	

	REComSession::FinalClose();
	__UHEAP_MARKEND;

	return verdict;
	}
	
//
// RTestStepCodecApiOpaqueDataI
// Create opaque data with an I tag.

/**
 * Constructor
 */
RTestStepCodecApiOpaqueDataI::RTestStepCodecApiOpaqueDataI()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0302-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiOpaqueDataI::DoTestStepL()
	{
	TVerdict verdict = EPass;
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test Opaque Data - I tag"));

	CCodecApiVideoOpaqueData* parse = NULL;
	_LIT8(KThisTypeUid, "<i>0x12345678");
	const TDesC8* opaqueData = &KThisTypeUid;
	INFO_PRINTF1(_L("Testing valid &lt;i&gt; tag"));
	TRAPD(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}	
	CleanupStack::PushL(parse);
	TUid typeUid = parse->TypeUid();
	CleanupStack::PopAndDestroy(); // parse		
	if(typeUid.iUid != 0x12345678) 
		{
		INFO_PRINTF1(_L("Opaque data did not return the correct values"));
		verdict = EFail;
		}
		
	_LIT8(KThisTypeUid2, "<i>0X12345678");
	opaqueData = &KThisTypeUid2;
	INFO_PRINTF1(_L("Testing valid &lt;i&gt; tag with 0X"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}	
	CleanupStack::PushL(parse);
	typeUid = parse->TypeUid();
	CleanupStack::PopAndDestroy(); // parse		
	if(typeUid.iUid != 0x12345678) 
		{
		INFO_PRINTF1(_L("Opaque data did not return the correct values"));
		verdict = EFail;
		}
		
	_LIT8(KThisTypeUid3, "<i>0X12");
	opaqueData = &KThisTypeUid3;
	INFO_PRINTF1(_L("Testing truncated &lt;i&gt; tag"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	// this should fail
	if (error !=KErrCorrupt)
		{
		INFO_PRINTF2(_L("Unexpected error %d"), error);
		verdict = EFail;
		}	

	_LIT8(KThisTypeUid4, "<i>0xrubbish1");
	opaqueData = &KThisTypeUid4;
	INFO_PRINTF1(_L("Testing invalid &lt;i&gt; tag"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	// this should fail
	if (error !=KErrGeneral)
		{
		INFO_PRINTF2(_L("Unexpected error %d"), error);
		verdict = EFail;
		}	

	_LIT8(KThisTypeUid4a, "<i>rubbish123");
	opaqueData = &KThisTypeUid4a;
	INFO_PRINTF1(_L("Testing invalid &lt;i&gt; tag without 0x"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	// this should fail
	if (error !=KErrCorrupt)
		{
		INFO_PRINTF2(_L("Unexpected error %d"), error);
		verdict = EFail;
		}	

	_LIT8(KThisTypeUid5, "<i>");
	opaqueData = &KThisTypeUid5;
	INFO_PRINTF1(_L("Testing empty &lt;i&gt; tag"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	// this should fail
	if (error !=KErrCorrupt)
		{
		INFO_PRINTF2(_L("Unexpected error %d"), error);
		verdict = EFail;
		}	
		
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	return verdict;
	}
	
//
// RTestStepCodecApiOpaqueDataS
// Create opaque data with an S or D tag.

/**
 * Constructor
 */
RTestStepCodecApiOpaqueDataS::RTestStepCodecApiOpaqueDataS()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0303-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiOpaqueDataS::DoTestStepL()
	{
	TVerdict verdict = EPass;
	
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test Opaque Data - S or D tag"));

	CCodecApiVideoOpaqueData* parse = NULL;
	_LIT8(KThisSrc, "<s>VORB");
	const TDesC8* opaqueData = &KThisSrc;
	INFO_PRINTF1(_L("Testing valid &lt;s&gt; tag"));
	TRAPD(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}	
	CleanupStack::PushL(parse);
	
	const TDesC8& inputData = parse->InputDataType();
	
	_LIT8(KThisSrcData, "VORB");
	if(inputData != KThisSrcData)
		{
		INFO_PRINTF1(_L("Opaque data did not return the correct values"));
		verdict = EFail;
		}

	CleanupStack::PopAndDestroy(); // parse		

	_LIT8(KThisDst, "<d>VORB");
	opaqueData = &KThisDst;
	INFO_PRINTF1(_L("Testing valid &lt;d&gt; tag"));
	TRAP(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}	
	CleanupStack::PushL(parse);
	
	const TDesC8& outputData = parse->OutputDataType();
	
	_LIT8(KThisDstData, "VORB");
	if(outputData != KThisDstData)
		{
		INFO_PRINTF1(_L("Opaque data did not return the correct values"));
		verdict = EFail;
		}

	CleanupStack::PopAndDestroy(); // parse		

	REComSession::FinalClose();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Opaque data ok"));
	return verdict;
	}
	
//
// RTestStepCodecApiOpaqueDataNone
// Create opaque data with no tag.

/**
 * Constructor
 */
RTestStepCodecApiOpaqueDataNone::RTestStepCodecApiOpaqueDataNone()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0304-HP");	
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiOpaqueDataNone::DoTestStepL()
	{
	TVerdict verdict = EPass;
	
	__UHEAP_MARK;

	INFO_PRINTF1(_L("Test Opaque Data - no tag"));

	CCodecApiVideoOpaqueData* parse = NULL;
	_LIT8(KNullTag, "");
	const TDesC8* opaqueData = &KNullTag;
	INFO_PRINTF1(_L("Testing blank tag"));
	TRAPD(error, parse = CCodecApiVideoOpaqueData::NewL(*opaqueData));
	// this should succeed
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Invalid opaque data, error %d"), error);
		verdict = EFail;
		}	
	delete parse;	
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("Opaque data ok"));
	return verdict;
	}
	
// end
