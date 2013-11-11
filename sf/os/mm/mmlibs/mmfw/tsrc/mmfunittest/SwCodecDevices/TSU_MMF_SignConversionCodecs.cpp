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
// TSU_MMF_CodecTests.cpp
// 
//

// EPOC includes
#include <e32base.h>
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>

// Test system includes
#include "TSU_MMF_DeviceSuite.h"
#include "TSU_MMF_SignConversionCodecs.h"

//[ codec includes ]
#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include "MmfPcm16toAlawhwDevice.h"
#include "MmfALawToPcm16HwDevice.h"
#include "mmfpcm16toMulawhwdevice.h"
#include "MmfMuLawToPcm16hwDevice.h"
#include "mmfpcmS8ToPcmS16HwDevice.h"
#include "mmfpcmS16PcmS8HwDevice.h"
#include "mmfpcm16topcmU16BEHwDevice.h"
#include "mmfpcm16SwapEndianhwdevice.h"
#include "mmfpcm16ToImaAdpcm.h"
#include "MmfImaAdpcmtopcm16hwdevice.h"
#include "MMFpcm16ToPcm16HwDevice.h"
#include "MMFpcm16ToPcmU8HwDevice.h"
#include "MMFpcmU8ToPcm16HwDevice.h"
#include "mmfpcmS16PcmS8HwDevice.h"


//[ Codec Unit tests  structure 
// The unit tests shall use text files
// for small portions of test data 
// which should be stored are stored in a simple format
// containing the relevant parameters for the test
//]
class TSignConversionTestParams
	{	
public:
	const TText* iTestName;       // name of the test
	};

// constant table of parameters for tests
const TSignConversionTestParams KSignConversionParameters[] =
	{	
		{_S("MM-MMF-SWCODECDEVICES-U-0017-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0018-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0019-HP")},
		{_S("MM-MMF-SWCODECDEVICES-U-0020-HP")}, 
		{_S("MM-MMF-SWCODECDEVICES-U-0021-HP")}
	};

/**
*
* PrintComparisonDataL
* @param aCodedData the coded data buffer
* @param aRefCodedData the reference coded data buffer
* 
*/
void CMMFDataGenerator::PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData )
	{
	//[precondition reference coded data is equal in size or longer than coded data ]
	if( aRefCodedData.Data().Length() < aCodedData.Data().Length() )
		{
         //[coded data is longer than reference data]
	//	RDebug::Print(_L("Coded Data is longer than refernce data")); Commented under DEF105143
		User::Leave( KErrCorrupt );	 
		}

	TUint8* ptr1 = CONST_CAST(TUint8*, aCodedData.Data().Ptr());
	TUint8* ptr2 = CONST_CAST(TUint8*, aRefCodedData.Data().Ptr());
    TInt length = aCodedData.Data().Length();
	//[ now print the results for comparison ]
	for( TInt i = 0; i < length; i++ )
		{
        RDebug::Print( _L("difference: %d, coded: %d, RefCoded: %d"), (*ptr1-*ptr2), *ptr1++, *ptr2++ );
		} 
	}

/**
*
* AssembleValue
* @param aData src data buffer 
* @param aValue resultant value
* @param aBigEndian whether the value is in BigEndian format
* @precondition aData is not null and has length >= 2
*
*/
void CMMFDataGenerator::AssembleValue( TUint8* aData, TInt16& aValue, TBool aBigEndian )
	{
	__ASSERT_DEBUG( aData, Panic(EBadArgument));
    if( aBigEndian )
		{
		aValue  = static_cast<TInt16>( aData[1] &KAndMask8bit );  
		aValue |=  static_cast<TInt16>((aData[0] << 8 ));
		}
	else
		{
		aValue  = static_cast<TInt16>( aData[0] &KAndMask8bit);  
		aValue |=  static_cast<TInt16>((aData[1] << 8 ));
		}
	}

/**
*
* AssembleValue
* @param aData src data buffer 
* @param aValue resultant value
* @param aBigEndian whether the value is in BigEndian format
* @precondition aData is not null and has length >= 2
*
*/
void CMMFDataGenerator::AssembleValue( TUint8* aData, TUint16& aValue, TBool aBigEndian )
	{
	__ASSERT_DEBUG( aData, Panic(EBadArgument));
	if( aBigEndian )
		{
		aValue     = aData[1] ; //[ big endian format]
		aValue    |= (aData[0] << 8 ); 
		}
	else
		{
		aValue     = aData[0] ; 
		aValue    |= (aData[1] << 8 ); 
		}
	}
/**
*
* CMMFPcmU16toPcmU16BGenerator
*
**/
CMMFPcmU16toPcmU16BGenerator::CMMFPcmU16toPcmU16BGenerator()
	{
	}

/**
*
* ~CMMFPcmU16toPcmU16BGenerator
*
**/
CMMFPcmU16toPcmU16BGenerator::~CMMFPcmU16toPcmU16BGenerator()
	{
	}

/**
*
* GenerateSourceDataL
*
**/
void CMMFPcmU16toPcmU16BGenerator::GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TUint16 KScale2 = 32000;

	 //[ create a buffer large enough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TUint16 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 

	 TInt noSamples = aSize/sizeof(TInt16);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = (KPi*i)/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
          srcValue = static_cast<TUint16>( (KScale2 * val ) + KScale2+0.5);
          *pDst++ = static_cast<TUint8>( srcValue &KAndMask8bit);
		  *pDst++ = static_cast<TUint8>( (srcValue >>8) &KAndMask8bit);
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	}

/**
*
* GenerateComparisonDataL
*
**/
void CMMFPcmU16toPcmU16BGenerator::GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TUint16 KScale2 = 32000;

	 //[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TUint16 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 

	 TInt noSamples = aSize/sizeof(TInt16);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = (KPi*i)/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
          srcValue = static_cast<TUint16>( (KScale2 * val)+KScale2 +0.5);
          //[endian swap here ]
		  *pDst++ = static_cast<TUint8>( (srcValue >>8) &KAndMask8bit);
		  *pDst++ = static_cast<TUint8>( srcValue &KAndMask8bit);
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	}

/**
*
* PrintComparisonDataL
* Prints out coded results for comparison with the 
* Reference values and is used when the results differ
*
**/
void CMMFPcmU16toPcmU16BGenerator::PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData )
	{
	//[precondition reference coded data is equal in size or longer than coded data ]
	if( aRefCodedData.Data().Length() < aCodedData.Data().Length() )
		{
         //[coded data is longer than reference data]
	//	RDebug::Print( _L("Coded Data is longer than refernce data"));Commented under DEF105143
		User::Leave( KErrCorrupt );	 
		}

	//[ now print the results for comparison ]
    TUint8* ptr1 = CONST_CAST(TUint8*, aCodedData.Data().Ptr());
	TUint8* ptr2 = CONST_CAST(TUint8*, aRefCodedData.Data().Ptr());
    TInt length = aCodedData.Data().Length();
	length /= 2; // [for 16bit words length is half]
	//[ assert invariant length is even ]
	__ASSERT_DEBUG( (length % 2 == 0 ), Panic(EBadInvariant));

	TUint16 codedValue = 0;
	TUint16 refCodedValue = 0;
	//[ now print the results for comparison ]
	for( TInt i = 0; i < length; i++ )
		{
		//[ assemble 16bit values from buffer values ]    
		AssembleValue( ptr1, codedValue, ETrue );
		AssembleValue( ptr2, refCodedValue, ETrue );       
		RDebug::Print( _L("difference: %u, coded: %u, RefCoded: %u"), (refCodedValue-codedValue), codedValue, refCodedValue );
		if( i % 100 == 0 ) //deal with debug printf deficiencies
			{
			User::After(100 );
			}
		ptr1+=2;
		ptr2+=2;
		} 
 	}

/**
*
* CMMFPcmU16BtoPcmU16Generator
*
**/
CMMFPcmU16BtoPcmU16Generator::CMMFPcmU16BtoPcmU16Generator()
	{
	}

/**
*
* ~CMMFPcmU16BtoPcmU16Generator
*
**/
CMMFPcmU16BtoPcmU16Generator::~CMMFPcmU16BtoPcmU16Generator()
	{
	}

/**
*
* GenerateSourceDataL
*
**/
void CMMFPcmU16BtoPcmU16Generator::GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TUint8 KScale2 = 127;

	 //[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TInt8 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 

	 TInt noSamples = aSize/sizeof(TInt16);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = (KPi*i)/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
          srcValue = static_cast<TUint8>( (KScale2 * val) );
          //[endian swap here ]
		  *pDst++ = static_cast<TUint8>( (srcValue >>8) &KAndMask8bit);
		  *pDst++ = static_cast<TUint8>( srcValue &KAndMask8bit);
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	}

/**
*
* GenerateComparisonDataL
*
**/
void CMMFPcmU16BtoPcmU16Generator::GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TUint8 KScale2 = 127;

	 //[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TUint16 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 
	 TInt noSamples = aSize/sizeof(TInt16);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = (KPi*i)/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
          srcValue = static_cast<TUint16>( KScale2 * val );
		  *pDst++ = static_cast<TUint8>( srcValue &KAndMask8bit);
		  *pDst++ = static_cast<TUint8>( (srcValue >>8) &KAndMask8bit);
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	}

/**
*
* PrintComparisonDataL
* Prints out coded results for comparison with the 
* Reference values and is used when the results differ
*
**/
void CMMFPcmU16BtoPcmU16Generator::PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData )
	{
	//[precondition reference coded data is equal in size or longer than coded data ]
	if( aRefCodedData.Data().Length() < aCodedData.Data().Length() )
		{
		//[coded data is longer than reference data]
	//	RDebug::Print( _L("Coded Data is longer than refernce data"));Commented under DEF105143
		User::Leave( KErrCorrupt );	 
		}
	
	//[ now print the results for comparison ]
    TUint8* ptr1 = CONST_CAST(TUint8*, aCodedData.Data().Ptr());
	TUint8* ptr2 = CONST_CAST(TUint8*, aRefCodedData.Data().Ptr());
    TInt length = aCodedData.Data().Length();
	length /= 2; //[ for 16bit words ]
	TUint16 codedValue = 0;
	TUint16 refCodedValue = 0;
	//[ now print the results for comparison ]
	for( TInt i = 0; i < length; i++ )
		{
		// assemble 16bit values
		AssembleValue( ptr1, codedValue, EFalse);
		AssembleValue( ptr2, refCodedValue, EFalse );        
		RDebug::Print( _L("difference: %u, coded: %u, RefCoded: %u"), (refCodedValue-codedValue), codedValue, refCodedValue );
		if( i % 100 == 0 ) //deal with debug printf deficiencies
			{
			User::After( 100 );
			}
		ptr1+=2;
		ptr2+=2;
		} 	
	}

/**
*
* CMMFPcm8ToPcm16Generator
*
**/
CMMFPcm8ToPcm16Generator::CMMFPcm8ToPcm16Generator()
	{
	}

/**
*
* CMMFPcm8ToPcm16Generator
*
**/
CMMFPcm8ToPcm16Generator::~CMMFPcm8ToPcm16Generator()
	{
	}

/**
*
* GenerateSourceDataL
*
**/
void CMMFPcm8ToPcm16Generator::GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TInt8 KScale2 = 127;

	 //[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TInt8 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 
     TInt noSamples = aSize/sizeof(TInt8);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = (KPi*i)/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
          srcValue = static_cast<TInt8>( (KScale2 * val) );
          *pDst++ = srcValue;
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	}

/**
*
* GenerateSourceDataL
*
**/
void CMMFPcm8ToPcm16Generator::GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TInt8 KScale2 = 127;

	 //[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TInt16 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 
	 TInt noSamples = aSize/sizeof(TInt16);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = (KPi*i)/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
		  TInt8 temp = static_cast<TInt8>((KScale2 * val));
          srcValue =  static_cast<TInt16>( temp << 8); 
		  *pDst++ = static_cast<TInt8>( srcValue&KAndMask8bit);
		  *pDst++ = static_cast<TInt8>((srcValue>>8)&KAndMask8bit);
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	}

/**
*
* PrintComparisonDataL
* Prints out coded results for comparison with the 
* Reference values and is used when the results differ
*
**/
void CMMFPcm8ToPcm16Generator::PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData )
	{
	//[precondition reference coded data is equal in size or longer than coded data ]
	if( aRefCodedData.Data().Length() < aCodedData.Data().Length() )
		{
		//[coded data is longer than reference data]
	//	RDebug::Print( _L("Coded Data is longer than refernce data"));Commented under DEF105143
		User::Leave( KErrCorrupt );	 
		}
	
	//[ now print the results for comparison ]
    TUint8* ptr1 = CONST_CAST(TUint8*, aCodedData.Data().Ptr());
	TUint8* ptr2 = CONST_CAST(TUint8*, aRefCodedData.Data().Ptr());
    TInt length = aCodedData.Data().Length();
	length /= 2; //[ for 16bit words ]
	TInt16 codedValue = 0;
	TInt16 refCodedValue = 0;
	//[ now print the results for comparison ]
	for( TInt i = 0; i < length; i++ )
		{
		// assemble 16bit values
  		AssembleValue( ptr1, codedValue, EFalse );
		AssembleValue( ptr2, refCodedValue, EFalse );        
		RDebug::Print( _L("difference: %d, coded: %d, RefCoded: %d"), (refCodedValue-codedValue), codedValue, refCodedValue );
		if( i % 100 == 0 ) //deal with debug printf deficiencies
			{
			User::After(100 );
			}
		ptr1+=2;
		ptr2+=2;
		} 	
	}

/**
*
* CMMFPcmU8ToPcm16Generator
*
**/
CMMFPcmU8ToPcm16Generator::CMMFPcmU8ToPcm16Generator()
	 {
	 }

/**
*
* ~CMMFPcmU8ToPcm16Generator
*
**/
CMMFPcmU8ToPcm16Generator::~CMMFPcmU8ToPcm16Generator()
	 {
	 }

/**
*
* GenerateSourceDataL
* @param aBuffer
* @param aSize in bytes
*
**/
void CMMFPcmU8ToPcm16Generator::GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	 {
	 const TUint8 KScale2 = 127;

	 //[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TUint8 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 
	 TInt noSamples = aSize/sizeof(TUint8);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = KPi*i/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));

          srcValue = static_cast<TUint8>((KScale2 * val+KScale2+0.5));
          *pDst++ = srcValue;
		   //RDebug::Print( _L("U8 = %u"), srcValue );  Statement commented under DEf105143
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	 }

/**
*
* GenerateComparisonDataL
* @param aBuffer
* @param aSize
*
**/
void CMMFPcmU8ToPcm16Generator::GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TInt8 KScale2 = 127;
	
	//[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);
	
	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());
	
	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
//	RDebug::Print( _L("Generation"));Commented under DEF105143
	TUint8 theValue = 0;
	TReal val   = 0.0;
	TReal theta = 0.0; 
	TUint8 rrr  = 0;
	TInt length = aSize/sizeof(TInt16);
	for(TInt i=0; i< length; i++)
		{
		//[ assign data and place in buffer]
		theta = KPi*i/10; // fundamental of 400hz sampled @8khz has 20 db or
		// better reproduction through gsm codec
		User::LeaveIfError(Math::Sin(val,theta));
		theValue = static_cast<TUint8>(KScale2 *val + KScale2 + 0.5); 
		//[ apply the same transformation as the codec ]
		rrr = static_cast<TUint8>( theValue^KMaskSign8bit );
		//RDebug::Print( _L("U8 = %u"), theValue ); Statement commented under DEf105143
		*pDst++ = rrr;
		*pDst++ = rrr;
		}
    
	aBuffer->Data().SetLength(aSize); 
	}

/**
*
* PrintComparisonDataL
* Prints out coded results for comparison with the 
* Reference values and is used when the results differ
*
**/
void CMMFPcmU8ToPcm16Generator::PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData )
	{
	//[precondition reference coded data is equal in size or longer than coded data ]
	if( aRefCodedData.Data().Length() < aCodedData.Data().Length() )
		{
		//[coded data is longer than reference data]
	//	RDebug::Print( _L("Coded Data is longer than reference data"));Commented under DEF105143
		User::Leave( KErrCorrupt );	 
		}
	
	//[ now print the results for comparison ]
    TUint8* ptr1 = CONST_CAST(TUint8*, aCodedData.Data().Ptr());
	TUint8* ptr2 = CONST_CAST(TUint8*, aRefCodedData.Data().Ptr());
    TInt length = aCodedData.Data().Length();
	length /= 2; //[ for 16bit words ]
	TInt16 codedValue = 0;
	TInt16 refCodedValue = 0;
	//[ now print the results for comparison ]
	for( TInt i = 0; i < length; i++ )
		{
		// assemble 16bit values
  		AssembleValue( ptr1, codedValue, EFalse );
		AssembleValue( ptr2, refCodedValue, EFalse );        
		RDebug::Print( _L("delta %d c %d, rf %d"), (refCodedValue-codedValue), codedValue, refCodedValue );
		if( i % 100 == 0 ) //deal with debug printf deficiencies
			{
			User::After(100 );
			}
		ptr1+=2;
		ptr2+=2;
		} 

	}

/**
*
* CMMFPcm16ToPcmU8Generator
*
**/
CMMFPcm16ToPcmU8Generator::CMMFPcm16ToPcmU8Generator()
	{
     //Nothing doing
	}

/**
*
* CMMFPcm16ToPcmU8Generator
*
**/
CMMFPcm16ToPcmU8Generator::~CMMFPcm16ToPcmU8Generator()
	{
     //Nothing doing
	}

/**
*
* GenerateSourceDataL
* @param aBuffer
* @param aSize in bytes
* This function generates a 400hz sine wav
* under the assumption the sampling frequency is 8khz
*
*/
void CMMFPcm16ToPcmU8Generator::GenerateSourceDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TInt KScale = 320;

	 //[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TInt16 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 
	 TInt noSamples = aSize/sizeof(TInt16);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = (KPi*i)/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
          srcValue = static_cast<TInt16>( KScale * val );
          *pDst++ = static_cast<TUint8>( srcValue & KAndMask8bit);
		  *pDst++ = static_cast<TUint8>((srcValue >> 8) & KAndMask8bit );
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	}

/**
*
* GenerateComparisonDataL
* @param aBuffer
* @param aSize in bytes
* This function generates a 400hz sine wav
* under the assumption the sampling frequency is 8khz
*/
//[ use partial sopecialization to provide copncrete implmentation]
void CMMFPcm16ToPcmU8Generator::GenerateComparisonDataL( CMMFDataBuffer* &aBuffer, TInt aSize )
	{
	const TInt KScale2 = 320; //low value chosen to emphasize the distortion of this codec!

	 //[ create a buffer large eneough to fill with the data ]
	aBuffer = CMMFDescriptorBuffer::NewL(aSize);

	TUint8* pDst = CONST_CAST(TUint8*,aBuffer->Data().Ptr());

	//[ lets fill it with a 400hz signal at approx -3db ]
	//[encode the data]
	 TInt16 srcValue = 0;
	 TReal val   = 0.0;
	 TReal theta = 0.0; 
	 TInt noSamples = aSize/sizeof(TUint8);
	 for(TInt i=0; i< noSamples; i++)
		 {
		 //[ assign data and place in buffer]
		  theta = (KPi*i)/10; // fundamental of 400hz sampled @8khz has 20 db or
		                      // better reproduction through gsm codec
		  User::LeaveIfError(Math::Sin(val,theta));
          srcValue = static_cast<TInt16>( KScale2 * val );
          *pDst++ = static_cast<TUint8>( (srcValue >> 8) - KMaskSign8bit);
		 }
    
	 aBuffer->Data().SetLength(aSize); 
	}

/**
*
* PrintComparisonDataL
* Prints out coded results for comparison with the 
* Reference values and is used when the results differ
*
**/
void CMMFPcm16ToPcmU8Generator::PrintComparisonDataL( CMMFDataBuffer& aCodedData, CMMFDataBuffer& aRefCodedData )
	{
	//[precondition reference coded data is equal in size or longer than coded data ]
	if( aRefCodedData.Data().Length() < aCodedData.Data().Length() )
		{
         //[coded data is longer than reference data]
	//	RDebug::Print( _L("Coded Data is longer than refernce data"));Commented under DEF105143
		User::Leave( KErrCorrupt );	 
		}

	//[ now print the results for comparison ]
    TUint8* ptr1 = CONST_CAST(TUint8*, aCodedData.Data().Ptr());
	TUint8* ptr2 = CONST_CAST(TUint8*, aRefCodedData.Data().Ptr());
    TInt length = aCodedData.Data().Length();
	//[ now print the results for comparison ]
	for( TInt i = 0; i < length; i++ )
		{
		RDebug::Print( _L("difference: %u, coded: %u, RefCoded: %u"), (*ptr1-*ptr2), *ptr1, *ptr2 );
		if( i % 100 == 0 ) //deal with debug printf deficiencies
			{
			User::After(100 );
			}
		ptr1++;
		ptr2++;
		} 
	}

/**
*
* CompareData
* @param aData1 first data buffer for comparison
* @param aData2 second data buffer for comparison
* result TBool data is the same
* @precondition aData1 has the same amount of data as aData2
*
*/
template <class T, class Generator, TInt index>
TBool CTestStepSignConversionTest<T, Generator, index>::CompareData( CMMFDataBuffer* aData1, CMMFDataBuffer* aData2)
	{
	TBool result = ETrue;

	//[ precondition aData1 != NULL ]
	if( !aData1 )
		{
		User::Leave( KErrArgument );
		}

    //[ precondition aData2 != NULL ]
	if( !aData2 )
		{
		User::Leave( KErrArgument );
		}

    //[precondition aData1 length == aData2 length ]
	if( aData1->Data().Length() != aData2->Data().Length() )
		{
		User::Leave( KErrArgument );
		}

	//[Now Compare the data]
	TUint8* ptr1 = CONST_CAST(TUint8*, aData1->Data().Ptr());
	TUint8* ptr2 = CONST_CAST(TUint8*, aData2->Data().Ptr());
	TInt dataLength = aData2->Data().Length();
   	if( Mem::Compare( ptr1, dataLength, ptr2, dataLength )!=0)
		{
		TUint8* p1 = ptr1; 
        TUint8* p2 = ptr2;
		TInt16 s1  = 0;
		TInt16 s2  = 0;
		INFO_PRINTF1(_L("------------------------------"));
		for( TInt i = 0; i < dataLength/2; i++ )
			{
		    s1  = static_cast<TInt16>( p1[0] &KAndMask8bit);  
		    s1 |=  static_cast<TInt16>((p1[1] << 8 ));
		    s2  = static_cast<TInt16>( p2[0] &KAndMask8bit);  
		    s2 |=  static_cast<TInt16>((p2[1] << 8 ));
			INFO_PRINTF3(_L("%d  %d"), s1, s2);
			p1+=2;
			p2+=2;
			}
		INFO_PRINTF1(_L("------------------------------"));
	//	RDebug::Print( _L("Comparison has failed")); Commented under DEF105143
		iGenerator->PrintComparisonDataL( *aData1, *aData2 );
		result = EFalse ;
		}
	return result;
	}

/**
*
* DoTestStepL
* @result TVerdict
*
*/
template <class T, class Generator, TInt index>
TVerdict CTestStepSignConversionTest<T, Generator, index>::DoTestStepL()
	{
	__MM_HEAP_MARK;	
	TVerdict result = EPass;

	//[pre condition iSourceData ]
	if( !iSourceData )
		{
		INFO_PRINTF1(_L("Source Data Failure"));
		User::Leave( KErrCorrupt);
		}
	//[precondition iCodedData  ]
	if( !iCodedData )
		{
		INFO_PRINTF1(_L("Coded Data Failure"));
		User::Leave( KErrCorrupt);
		}
	//[precondition iRefData ]
	if( !iRefCodedData )
		{
		INFO_PRINTF1(_L("RefCodedData Argument Failure"));
		User::Leave( KErrCorrupt);
		}

	//[ lets code the data and compare it to the reference data ]
    iCodecUnderTest->ProcessL(*iSourceData, *iCodedData);
    if(!CompareData(iCodedData, iRefCodedData))
		{
		INFO_PRINTF1(_L("Coded Results do not match reference coded results"));

		result = EFail;
		}
	
	__MM_HEAP_MARKEND;
	return result;
	}

/**
*
* DoTestStepPostambleL
* @result TVerdict
*
*/
template <class T, class Generator, TInt index>
TVerdict CTestStepSignConversionTest<T, Generator, index>::DoTestStepPostambleL()
	{
	TVerdict result = EPass;	
	//[delete the buffers & Codec]
	delete iCodecUnderTest;
	delete iGenerator;
	delete iSourceData;
	delete iCodedData;
	delete iRefCodedData;
	return result;
	}

/**
*
* DoTestStepPreambleL
* @result TVerdict
*
*/
template <class T, class Generator, TInt index>
TVerdict CTestStepSignConversionTest<T, Generator, index>::DoTestStepPreambleL()
	{
	TVerdict result = EPass;

	iCodecUnderTest =  new(ELeave) T;   // a cmmfcodec ;
	iGenerator = new(ELeave) Generator; // src generator

  	//[ensure the number of samples is >= srcbuffers]
	const TInt KNumBuffers = 2;
	const TInt dataSrcSize = KNumBuffers *  iCodecUnderTest->SourceBufferSize();

	//[generate src data]
	iGenerator->GenerateSourceDataL( iSourceData, dataSrcSize);
	
	const TInt dataSinkSize = KNumBuffers *  iCodecUnderTest->SinkBufferSize();

	//[generate comparison data]
	iGenerator->GenerateComparisonDataL( iRefCodedData, dataSinkSize);

	//[reserve space for coded data ]
    iCodedData = CMMFDescriptorBuffer::NewL(dataSinkSize);

	return result;
	}

/**
*
* CTestStepCodecUnitTest
*
*/
template <class T, class Generator, TInt index>
CTestStepSignConversionTest<T, Generator, index>::CTestStepSignConversionTest()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = (&KSignConversionParameters[index])->iTestName;
	}

/**
*
* CTestStepCodecUnitTest
*
*/
template <class T, class Generator, TInt index>
CTestStepSignConversionTest<T, Generator, index>::~CTestStepSignConversionTest()
	{
	}

/**
*
* This is used for template instantiation.
*
**/

template class CTestStepSignConversionTest<CMMFPcm16ToPcmU8Codec,CMMFPcm16ToPcmU8Generator,0>;
template class CTestStepSignConversionTest<CMMFPcm8ToPcm16Codec,CMMFPcm8ToPcm16Generator,1>;
template class CTestStepSignConversionTest<CMMFPcm16SwapEndianCodec,CMMFPcmU16toPcmU16BGenerator,2>;
template class CTestStepSignConversionTest<CMMFPcm16SwapEndianCodec,CMMFPcmU16BtoPcmU16Generator,3>;
template class CTestStepSignConversionTest<CMMFPcmU8ToPcm16Codec,CMMFPcmU8ToPcm16Generator,4>;

