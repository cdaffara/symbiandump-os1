// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <hal.h>
#include <bitdraw.h>
#include <bitdrawscaling.h>
#include <bitdraworigin.h>
#include <bitdrawinterfaceid.h>
#include <graphics/gdi/gdiconsts.h>
#include "TScdvScaling.h"



//Test line properties: 
//[iX, iY] - the starting point of the line
//iLength - line length
struct TLineProps
	{
	TInt iX;
	TInt iY;
	TInt iLength;
	};
//
//Constants
//
//This structure defines [TDrawMode <-> clear screen color value] relation
//When the test uses KDrawMode[i].iDrawMode, then the screen will be cleared using
//KDrawMode[i].iClearColorVal value
struct TDrawModeProps
	{
	CGraphicsContext::TDrawMode iDrawMode;
	// keep the color value 8-bit in both EColor256 and EColor64K, special care need
	// to be done when filling the buffer, e.g color=0x55 
	// in EColor256 clearing means filling 0x55 0x55 0x55 etc, but
	// in EColor64K it has to be 0x0055 0x0055 0x0055 etc
	TUint8 iClearColorVal;
	};
const TDrawModeProps KDrawMode[] = 
	{
		{CGraphicsContext::EDrawModePEN,		0xFF},
		{CGraphicsContext::EDrawModeAND,		0x37},
		{CGraphicsContext::EDrawModeXOR,		0x38},
		{CGraphicsContext::EDrawModeOR,			0xB1},
		{CGraphicsContext::EDrawModeNOTSCREEN,	0xC0},
		{CGraphicsContext::EDrawModeNOTPEN,		0x1D}
	};
const TInt KDrawModesCnt = sizeof(KDrawMode) / sizeof(KDrawMode[0]);
//Shadow/Fade modes
const CFbsDrawDevice::TShadowMode KShadowMode[] = 
	{
	CFbsDrawDevice::EShadow,
	CFbsDrawDevice::EFade,
	CFbsDrawDevice::EShadowFade
	};
const TInt KShadowModesCnt = sizeof(KShadowMode) / sizeof(KShadowMode[0]);
//Test pixel color value
const TUint8 KTestColorVal = 0x55;
//All possible orientations
const CFbsDrawDevice::TOrientation KOrientation[] = 
	{
	CFbsDrawDevice::EOrientationNormal,
	CFbsDrawDevice::EOrientationRotated90,
	CFbsDrawDevice::EOrientationRotated180,
	CFbsDrawDevice::EOrientationRotated270
	};
const TInt KOrientationsCnt = sizeof(KOrientation) / sizeof(KOrientation[0]);
//Width and Height of legacy application screen
const TInt KLegacyAppSizeWidth = 60;
const TInt KLegacyAppSizeHeight = 40;
//Scaling factors: X and Y
const TInt KScalingFactorX = 3;
const TInt KScalingFactorY = 2;
const TInt KMaxScalingFactor = 3;		//Max of previous 2 values

//
//Declarations
//
GLDEF_C TInt ByteSize(TDisplayMode aDisplayMode,TInt aWidth)
	{
	TInt wordSize=aWidth;
	switch(aDisplayMode)
		{
		case EGray2:
			wordSize = (wordSize + 31) / 32;
			break;
		case EGray4:
			wordSize = (wordSize + 15) / 16;
			break;
		case EGray16:
		case EColor16:
			wordSize = (wordSize + 7) / 8;
			break;
		case EGray256:
		case EColor256:
			wordSize = (wordSize + 3) / 4;
			break;
		case EColor4K:
		case EColor64K:
			wordSize = (wordSize + 1) / 2;
			break;
		case EColor16M:
			wordSize = ((wordSize + 3) / 4) * 3;
			break;
		case EColor16MU:
		case EColor16MA:
			//Doesn't need changing
			break;
		default:
			break;
		};
	return wordSize * 4;
	}

static inline TInt ByteSize(TDisplayMode aDisplayMode, TSize aSize)
	{
	return ByteSize(aDisplayMode,aSize.iWidth) * aSize.iHeight;
	}

template <class TPixelType> 
inline void MemFill(TPixelType* aBuffer, TInt aSize, TPixelType aValue)
	{
	TPixelType* p = aBuffer;
	TInt i = 0;
	while (i++<aSize)
		*p++ = aValue;
	}
	
//Generic test class for both Color256 and Color64K
template <class TPixelType> class CTScaling: public CTGraphicsBase
	{
public:
	CTScaling(CTestStep *aTest, TDisplayMode aDisplayMode);
	~CTScaling();
	void RunTestCaseL(TInt aCurTestCase);
private:
	void ConstructL();
	void CreateScreenDeviceL();
	void SetScalingSettings(const TPoint& aOrigin, TInt aFx, TInt aFy, TInt aDivX, TInt aDivY);
	void CheckLine(const TLineProps& aLineProps, const TPoint& aOrg, const TDrawModeProps& aDrawModeProps, TPixelType aClearColorValue);
	void CheckWriteRgbMulti(const TRect& aRcProps, const TPoint& aOrg, const TDrawModeProps& aDrawModeProps, TPixelType aClearColorValue);
	void CheckRgbAlphaLine(const TLineProps& aLineProps, const TPoint& aOrg, TPixelType aClearColorValue);
	void CheckRect(const TRect& aRc, const TPoint& aOrg, TPixelType aClearColorValue);
	void WriteLine();
	void ClearScreen(const TDrawModeProps& aDrawModeProps);
	void SetTestData();
	void CheckChangedPixels(TInt aChangedPixelsCnt, TPixelType aClearColorVal);
	void CheckWriteBinary(const TPoint& aPt, const TPoint& aOrg, TPixelType aClearColorValue, TInt aLength, TInt aHeight);
	void CheckVertLine(const TLineProps& aLineProps, const TPoint& aOrg, TPixelType aClearColorValue);
	void WriteRgb();
	void WriteRgbMulti();
	void WriteRgbAlphaLine();
	void WriteBinary();
	void WriteBinaryLineVertical();
	void WriteBinaryLine();
	void WriteRgbAlphaMulti();
	void ShadowArea();
	void WriteRgbAlphaLine2();
	void TestScalingSettingsInterface();
	void PerformanceTest();
private:
	//Test data array
	TPixelType iTestData[KLegacyAppSizeWidth];
	//The device used in the tests
	CFbsDrawDevice* iDrawDevice;
	//Width and Height of the screen
	TSize iPhysSize;
	//The test allocates block of memory for a screen with PhysSize size 
	//mode. iBits will point to the allocated memory block.
	TPixelType* iBits;
	TDisplayMode iDisplayMode;
	//The scaling interface
	MScalingSettings* iScalingSettings;
	//The origin interface
	MDrawDeviceOrigin* iOriginInterface;
	TInt iCurOrientation;
	TInt iScalingFactorX;
	TInt iScalingFactorY;
	};
typedef CTScaling<TUint8> CTestNone;
typedef CTScaling<TUint8> CTestColor256;
typedef CTScaling<TUint16> CTestColor64K;



//
//Test code
//
/*template <class TPixelType>
CTScaling<TPixelType>* CTScaling<TPixelType>::NewL(TDisplayMode aDisplayMode)	
	{
	CTScaling<TPixelType>* self = new (ELeave) CTScaling<TPixelType>;
	CleanupStack::PushL(self);
	self->ConstructL(aDisplayMode);
	CleanupStack::Pop(self);
	return self;
	}
*/	

template <class TPixelType>
CTScaling<TPixelType>::CTScaling(CTestStep *aTest, TDisplayMode aDisplayMode) :
				CTGraphicsBase(aTest),
				iDisplayMode(aDisplayMode)
	{
	INFO_PRINTF1(_L("Scaling tests"));
	}
	

template <class TPixelType>
void CTScaling<TPixelType>::ConstructL()	
	{
	CreateScreenDeviceL();
	}
	
template <class TPixelType>
CTScaling<TPixelType>::~CTScaling()
	{
	((CTScalingStep*)iStep)->CloseTMSGraphicsStep();
	delete[] iBits;
	delete iDrawDevice;	
	}
	
template <class TPixelType>
void CTScaling<TPixelType>::SetScalingSettings(const TPoint& aOrigin, TInt aFx, TInt aFy, TInt aDivX, TInt aDivY)
	{
	TEST(iDrawDevice != NULL);	
	if(!iScalingSettings)
		{
		TInt err = iDrawDevice->GetInterface(KScalingSettingsInterfaceID, 
											  reinterpret_cast <TAny*&> (iScalingSettings));
		TEST2(err, KErrNone);
		}
	TEST(iScalingSettings != NULL);
	TInt err = iScalingSettings->Set(aFx, aFy, aDivX, aDivY);
	TEST2(err, KErrNone);
	if(!iOriginInterface)
		{
		TInt err = iDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID, 
											  reinterpret_cast <TAny*&> (iOriginInterface));
		TEST2(err, KErrNone);
		}
	TEST(iOriginInterface != NULL);
	err = iOriginInterface->Set(aOrigin);
	TEST2(err, KErrNone);
	}

//Clears the screen initializing each screen pixel with aDrawModeProps.iClearColorVal value
template <class TPixelType>
void CTScaling<TPixelType>::ClearScreen(const TDrawModeProps& aDrawModeProps)
	{
	::MemFill(iBits, ::ByteSize(EColor256, iPhysSize), TPixelType(aDrawModeProps.iClearColorVal));
	}

//Initializes iTestData array with KTestColorVal value
template <class TPixelType>
void CTScaling<TPixelType>::SetTestData()
	{
	::MemFill(iTestData, KLegacyAppSizeWidth, TPixelType(KTestColorVal));
	}

template <class TPixelType>
void CTScaling<TPixelType>::CheckChangedPixels(TInt aChangedPixelsCnt, TPixelType aClearColorVal)
	{
	const TInt KByteSize = ::ByteSize(EColor256, iPhysSize);
	TInt changedPixelsCnt = 0;
	for(TInt ii=0;ii<KByteSize;++ii)
		{
		if(iBits[ii]!=aClearColorVal)
			{
			++changedPixelsCnt;
			}
		}
	TEST(changedPixelsCnt == aChangedPixelsCnt);
	if (changedPixelsCnt!=aChangedPixelsCnt)
		{
		_LIT(KLog,"Wrong number of changed pixels, expected=%d, actual=%d, color=0x%x");
		INFO_PRINTF4(KLog,aChangedPixelsCnt,changedPixelsCnt,aClearColorVal);
		}
	}

//Checks a set of horisontal lines , which starting point is 
//[aOrg.iX + aLineProps.iX * iScalingFactorX, aOrg.iY + aLineProps.iY * iScalingFactorY]
//and length is aLineProps.iLength * iScalingFactorX. For each nexh line y-coordinate
//is incremented by 1.
//The screen lines pixel values are tested against aClearColorValue value.
//Then the screen is testsed pixel by pixel that nothing is written outside the tested lines.
template <class TPixelType>
void CTScaling<TPixelType>::CheckLine(const TLineProps& aLineProps, 
					  const TPoint& aOrg, 
					  const TDrawModeProps& aDrawModeProps, 
					  TPixelType aClearColorValue)
	{
	TPixelType data[KLegacyAppSizeWidth * KMaxScalingFactor];
	TInt ii;
	for(ii=0;ii<iScalingFactorY;++ii)
		{
		Mem::Fill(data, sizeof(data), 0x00);
		iDrawDevice->ReadLine(aOrg.iX + aLineProps.iX * iScalingFactorX, 
							   aOrg.iY + aLineProps.iY * iScalingFactorY + ii, 
							   aLineProps.iLength * iScalingFactorX, 
							   data, 
							   iDisplayMode);
		const TInt length=aLineProps.iLength*iScalingFactorX;
		TInt firstErr=length;
		TInt numErrs=0;
		for(TInt jj=0;jj<length;++jj)
			{
			//TEST(data[jj]!=aClearColorValue);
			if (data[jj]==aClearColorValue)
				{
				++numErrs;
				if (jj<firstErr)
					firstErr=jj;
				}
			}
		TEST(numErrs==0);
		if (numErrs>0)
			{
			_LIT(KLog,"Line %d (of %d) of length %d has %d errors first one at %d,  ClearCol=0x%x");
			INFO_PRINTF7(KLog,ii,iScalingFactorY,length,numErrs,firstErr,aClearColorValue);
			}
		}
	TInt changedPixelsCnt = iScalingFactorY * aLineProps.iLength * iScalingFactorX;
	CheckChangedPixels(changedPixelsCnt, aDrawModeProps.iClearColorVal);
	}

//Checks the rectangle filled using CFbsScreenDevice::WriteRgbMulti. 
//The screen lines pixel values are tested against aClearColorValue value.
//Then the screen is testsed pixel by pixel that nothing is written outside the tested rectangle.
template <class TPixelType>
void CTScaling<TPixelType>::CheckWriteRgbMulti(const TRect& aRcProps, 
							   const TPoint& aOrg, 
							   const TDrawModeProps& aDrawModeProps, 
							   TPixelType aClearColorValue)
	{
	TPixelType data[KLegacyAppSizeWidth * KMaxScalingFactor];
	TInt ii;
	TInt xx = aOrg.iX + aRcProps.iTl.iX * iScalingFactorX;
	TInt yy = aOrg.iY + aRcProps.iTl.iY * iScalingFactorY;
	for(ii=0;ii<(iScalingFactorY * aRcProps.Height());++ii)
		{
		Mem::Fill(data, sizeof(data), 0x00);
		iDrawDevice->ReadLine(xx, yy+ii, aRcProps.Width()*iScalingFactorX, data, iDisplayMode);
		const TInt width=aRcProps.Width()*iScalingFactorX;
		TInt firstErr=width;
		TInt numErrs=0;
		for(TInt jj=0;jj<width;++jj)
			{
			//TEST(data[jj]!=aClearColorValue);
			if (data[jj]==aClearColorValue)
				{
				++numErrs;
				if (jj<firstErr)
					firstErr=jj;
				}
			}
		TEST(numErrs==0);
		if (numErrs>0)
			{
			_LIT(KLog,"Line %d of width %d has %d errors first one at %d,  ClearCol=0x%x");
			INFO_PRINTF6(KLog,ii,width,numErrs,firstErr,aClearColorValue);
			}
		}
	TInt changedPixelsCnt = iScalingFactorY * aRcProps.Width() * aRcProps.Height() * iScalingFactorX;
	CheckChangedPixels(changedPixelsCnt, aDrawModeProps.iClearColorVal);
	}

//Checks a set of horisontal lines , which starting point is 
//[aOrg.iX + aLineProps.iX * iScalingFactorX, aOrg.iY + aLineProps.iY * iScalingFactorY]
//and length is aLineProps.iLength * iScalingFactorX. For each nexh line y-coordinate
//is incremented by 1.
//The screen lines pixel values are tested against aClearColorValue value.
//Then the screen is testsed pixel by pixel that nothing is written outside the tested lines.
template <class TPixelType>
void CTScaling<TPixelType>::CheckRgbAlphaLine(const TLineProps& aLineProps, 
							  const TPoint& aOrg, 
							  TPixelType aClearColorValue)
	{
	TPixelType data[KLegacyAppSizeWidth * KMaxScalingFactor];
	for(TInt ii=0;ii<iScalingFactorY;++ii)
		{
		Mem::Fill(data, sizeof(data), 0x00);
		iDrawDevice->ReadLine(aOrg.iX + aLineProps.iX * iScalingFactorX, 
							   aOrg.iY + aLineProps.iY * iScalingFactorY + ii, 
							   aLineProps.iLength * iScalingFactorX, 
							   data, 
							   iDisplayMode);
		const TInt length=aLineProps.iLength*iScalingFactorX;
		TInt firstErr=length;
		TInt numErrs=0;
		for(TInt jj=0;jj<(aLineProps.iLength * iScalingFactorX);++jj)
			{
			//TEST(data[jj]!=aClearColorValue);
			if (data[jj]==aClearColorValue)
				{
				++numErrs;
				if (jj<firstErr)
					firstErr=jj;
				}
			}
		TEST(numErrs==0);
		if (numErrs>0)
			{
			_LIT(KLog,"Line %d of length %d has %d errors first one at %d,  ClearCol=0x%x");
			INFO_PRINTF6(KLog,ii,length,numErrs,firstErr,aClearColorValue);
			}
		}
	TInt changedPixelsCnt = iScalingFactorY * aLineProps.iLength * iScalingFactorX;
	CheckChangedPixels(changedPixelsCnt, aClearColorValue);
	}

//Checks the rectangle filled using CFbsScreenDevice::WriteBinary. 
//The screen lines pixel values are tested against aClearColorValue value.
//Then the screen is testsed pixel by pixel that nothing is written outside the tested rectangle.
template <class TPixelType>
void CTScaling<TPixelType>::CheckWriteBinary(const TPoint& aPt, 
							 const TPoint& aOrg, 
							 TPixelType aClearColorValue, 
							 TInt aLength, TInt aHeight)
	{
	TPixelType data[KLegacyAppSizeWidth * KMaxScalingFactor];
	TInt ii;
	TInt xx = aOrg.iX + aPt.iX * iScalingFactorX;
	TInt yy = aOrg.iY + aPt.iY * iScalingFactorY;
	for(ii=0;ii<(iScalingFactorY * aHeight);++ii)
		{
		Mem::Fill(data, sizeof(data), 0x00);
		iDrawDevice->ReadLine(xx, yy + ii, aLength * iScalingFactorX, data, iDisplayMode);
		const TInt length=aLength*iScalingFactorX;
		TInt firstErr=length;
		TInt numErrs=0;
		for(TInt jj=0;jj<length;++jj)
			{
			//TEST(data[jj] != aClearColorValue);
			if (data[jj]==aClearColorValue)
				{
				++numErrs;
				if (jj<firstErr)
					firstErr=jj;
				}
			}
		TEST(numErrs==0);
		if (numErrs>0)
			{
			_LIT(KLog,"Line %d of length %d has %d errors first one at %d,  ClearCol=0x%x");
			INFO_PRINTF6(KLog,ii,length,numErrs,firstErr,aClearColorValue);
			}
		}
	TInt changedPixelsCnt = iScalingFactorY * aLength * aHeight * iScalingFactorX;
	CheckChangedPixels(changedPixelsCnt, aClearColorValue);
	}

//Checks a set of vertical lines , which starting point is 
//[aOrg.iX + aLineProps.iX * iScalingFactorX, aOrg.iY + aLineProps.iY * iScalingFactorY]
//and length is aLineProps.iLength * iScalingFactorX. For each nexh line y-coordinate
//is incremented by 1.
//The screen lines pixel values are tested against aClearColorValue value.
//Then the screen is testsed pixel by pixel that nothing is written outside the tested lines.
template <class TPixelType>
void CTScaling<TPixelType>::CheckVertLine(const TLineProps& aLineProps, const TPoint& aOrg, TPixelType aClearColorValue)
	{
	TInt x = aOrg.iX + aLineProps.iX * iScalingFactorX;
	TInt y = aOrg.iY + aLineProps.iY * iScalingFactorY;
	for(TInt i=0;i<iScalingFactorX;++i)
		{
		for(TInt j=0;j<(aLineProps.iLength * iScalingFactorY);++j)
			{
			TRgb val = iDrawDevice->ReadPixel(x + i, y + j);				   
			switch (iDisplayMode)
				{
				case EColor64K:
				TEST(val.Color64K() != aClearColorValue);
				break;
				
				case EColor256:
				TEST(val.Color256() != aClearColorValue);
				break;
				
				default:
				TEST(EFalse);
				}
			}
		}
	TInt changedPixelsCnt = iScalingFactorX * aLineProps.iLength * iScalingFactorY;
	CheckChangedPixels(changedPixelsCnt, aClearColorValue);
	}

//Checks the rectangle filled using CFbsScreenDevice::ShadowArea
//The screen lines pixel values are tested against aClearColorValue value.
//Then the screen is testsed pixel by pixel that nothing is written outside the tested rectangle.
template <class TPixelType>
void CTScaling<TPixelType>::CheckRect(const TRect& aRc, const TPoint& aOrg, TPixelType aClearColorValue)
	{
	TPixelType data[KLegacyAppSizeWidth * KMaxScalingFactor];
	TInt i;
	TInt x = aOrg.iX + aRc.iTl.iX * iScalingFactorX;
	TInt y = aOrg.iY + aRc.iTl.iY * iScalingFactorY;
	for(i=0;i<(iScalingFactorY * aRc.Height());++i)
		{
		Mem::Fill(data, sizeof(data), 0x00);
		iDrawDevice->ReadLine(x, y + i, aRc.Width() * iScalingFactorX, data, iDisplayMode);
		for(TInt j=0;j<(aRc.Width() * iScalingFactorX);++j)
			{
			TEST(data[j] != aClearColorValue);
			}
		}
	TInt changedPixelsCnt = iScalingFactorY * aRc.Width() * aRc.Height() * iScalingFactorX;
	CheckChangedPixels(changedPixelsCnt, aClearColorValue);
	}

//CFbsScreenDevice::WriteLine() and CFbsScreenDevice::ReadLine() test.
//(Set of test lines) X (Set of origins) X (Set of drawing modes) number of test cases.
template <class TPixelType>
void CTScaling<TPixelType>::WriteLine()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteLine"));

	TLineProps lineProps[] = 
		{
		{0, 0, KLegacyAppSizeWidth - 1}, 
		{0, KLegacyAppSizeHeight - 1, KLegacyAppSizeWidth - 1},
		{10, 20, KLegacyAppSizeWidth / 2},
		{-2, -5, 20},
		{-3, 1, 21},
		{2, -2, 11},
		{0, -4, 31},
		{-1, 11, 11}
		};
	const TInt KLinesCnt = sizeof(lineProps) / sizeof(lineProps[0]);
	TPoint ptOrg[] = 
		{
		TPoint(13, 21),
		TPoint(10, 17)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt ll=0;ll<KOriginsCnt;++ll)
		{
		for(TInt line=0;line<KLinesCnt;++line)
			{
			for(TInt kk=0;kk<KDrawModesCnt;++kk)
				{
				ClearScreen(KDrawMode[kk]);
				SetTestData();
				SetScalingSettings(ptOrg[ll], KScalingFactorX, KScalingFactorY, 1, 1);
				iDrawDevice->WriteLine(lineProps[line].iX, lineProps[line].iY,
									  lineProps[line].iLength, 
									  reinterpret_cast <TUint32*> (iTestData), 
									  KDrawMode[kk].iDrawMode);
				if(KDrawMode[kk].iDrawMode == CGraphicsContext::EDrawModePEN)
					{
					TPixelType writtenData[KLegacyAppSizeWidth];
					Mem::FillZ(writtenData, sizeof(writtenData));
					iDrawDevice->ReadLine(lineProps[line].iX, lineProps[line].iY,
										   lineProps[line].iLength, 
										   writtenData, iDisplayMode);
					for(TInt ii=0;ii<lineProps[line].iLength;++ii)
						{
						TEST(writtenData[ii] == iTestData[ii]);
						}
					}
				SetScalingSettings(TPoint(), 1, 1, 1, 1);
				CheckLine(lineProps[line], ptOrg[ll], KDrawMode[kk], KDrawMode[kk].iClearColorVal);
				}
			}
		}
	}

//CFbsScreenDevice::WriteRgb() and CFbsScreenDevice::ReadPixel() test.
//(Set of test points) X (Set of origins) X (Set of drawing modes) number of test cases.
template <class TPixelType>
void CTScaling<TPixelType>::WriteRgb()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteRgb"));

	TPoint pt[] = 
		{
		TPoint(0, 0), 
		TPoint(KLegacyAppSizeWidth - 1, 0), 
		TPoint(0, KLegacyAppSizeHeight - 1), 
		TPoint(KLegacyAppSizeWidth - 1, KLegacyAppSizeHeight - 1),
		TPoint(KLegacyAppSizeWidth / 2, KLegacyAppSizeHeight / 2),
		TPoint(-2, -3), 
		TPoint(0, -1),
		TPoint(-3, 0)
		};
	const TInt KPointsCnt = sizeof(pt) / sizeof(pt[0]);
	TPoint ptOrg[] = 
		{
		TPoint(9, 22),
		TPoint(17, 11)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KPointsCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				ClearScreen(KDrawMode[k]);
				SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
				TRgb val(KTestColorVal);
				iDrawDevice->WriteRgb(pt[i].iX, pt[i].iY, val, KDrawMode[k].iDrawMode);
				if(KDrawMode[k].iDrawMode == CGraphicsContext::EDrawModePEN)
					{
					TRgb writtenVal = iDrawDevice->ReadPixel(pt[i].iX, pt[i].iY);
					switch (iDisplayMode)
						{
						case EColor64K:
						TEST(writtenVal == TRgb::Color64K(val.Color64K()));
						break;
						
						case EColor256:
						TEST(writtenVal == val);
						break;
						
						default:
						TEST(EFalse);
						}
					}
				SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
				TLineProps props;
				props.iX = pt[i].iX;
				props.iY = pt[i].iY;
				props.iLength = 1;
				CheckLine(props, ptOrg[l], KDrawMode[k], KDrawMode[k].iClearColorVal);
				}
			}
		}
	}

//CFbsScreenDevice::WriteRgbMulti() test.
template <class TPixelType>
void CTScaling<TPixelType>::WriteRgbMulti()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteRgbMulti"));

	TRect rcProps[] =
		{
		TRect(TPoint(0, 0), TSize(KLegacyAppSizeWidth - 1, KLegacyAppSizeHeight - 1)), 
		TRect(TPoint(17, 11), TSize(KLegacyAppSizeWidth / 2, KLegacyAppSizeHeight / 2)),
		TRect(TPoint(-1, -4), TSize(31, 12)), 
		TRect(TPoint(-3, -1), TSize(11, 11)), 
		TRect(TPoint(0, -2), TSize(6, 17))
		};
	const TInt KRcCnt = sizeof(rcProps) / sizeof(rcProps[0]);
	TPoint ptOrg[] = 
		{
		TPoint(21, 29),
		TPoint(12, 14)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KRcCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				ClearScreen(KDrawMode[k]);
				SetTestData();
				SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
				TRgb val(KTestColorVal);
				iDrawDevice->WriteRgbMulti(rcProps[i].iTl.iX, rcProps[i].iTl.iY,
									  rcProps[i].Width(), rcProps[i].Height(),
									  val, 
									  KDrawMode[k].iDrawMode);
				SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
				CheckWriteRgbMulti(rcProps[i], ptOrg[l], KDrawMode[k], KDrawMode[k].iClearColorVal);
				}
			}
		}
	}

//CFbsScreenDevice::WriteRgbAlphaLine() test.
//(Set of test lines) X (Set of origins) X (Set of drawing modes) number of test cases.
template <class TPixelType>
void CTScaling<TPixelType>::WriteRgbAlphaLine()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteRgbAlphaLine"));

	TLineProps lineProps[] = 
		{
		{0, 0, KLegacyAppSizeWidth - 1}, 
		{0, KLegacyAppSizeHeight - 1, KLegacyAppSizeWidth - 1},
		{17, 3, KLegacyAppSizeWidth / 2},
		{-1, -2, 11},
		{-4, -5, 1},
		{0, -1, 3},
		{1, -3, 7}
		};
	const TInt KLinesCnt = sizeof(lineProps) / sizeof(lineProps[0]);
	TPoint ptOrg[] = 
		{
		TPoint(19, 17),
		TPoint(29, 25)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KLinesCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				ClearScreen(KDrawMode[k]);
				SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
				TUint8 rgbBuff[KLegacyAppSizeWidth * 3];
				Mem::Fill(rgbBuff, sizeof(rgbBuff), KTestColorVal);
				TUint8 maskBuff[KLegacyAppSizeWidth];
				TUint8 maskChar = 0xF1;
				Mem::Fill(maskBuff, sizeof(maskBuff), maskChar);
				iDrawDevice->WriteRgbAlphaLine(lineProps[i].iX, lineProps[i].iY,
									  lineProps[i].iLength, rgbBuff, maskBuff, CGraphicsContext::EDrawModePEN);
				SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
				CheckRgbAlphaLine(lineProps[i], ptOrg[l], KDrawMode[k].iClearColorVal);
				}
			}
		}
	}

//CFbsScreenDevice::WriteBinary() test.
template <class TPixelType>
void CTScaling<TPixelType>::WriteBinary()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteBinary"));

	TPoint pt[] =
		{
		TPoint(0, 0), 
		TPoint(27, 19),
		TPoint(-4, -4),
		TPoint(-1, -2),
		TPoint(-1, -2),
		TPoint(5, -5),
		TPoint(-5, 0)
		};
	const TInt KPtCnt = sizeof(pt) / sizeof(pt[0]);
	TPoint ptOrg[] = 
		{
		TPoint(19, 24),
		TPoint(29, 26)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KPtCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				ClearScreen(KDrawMode[k]);
				SetTestData();
				SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
				const TInt KHeight = 5;
				const TInt KLength = 11;
				TUint32 buff[KHeight];
				TUint32 buffChar = 0xFFFFFFFF;
				for(TInt ooo=0;ooo<KHeight;++ooo)
					{
					buff[ooo] = buffChar;
					}
				TRgb val(KTestColorVal);
				iDrawDevice->WriteBinary(pt[i].iX, pt[i].iY, buff, KLength, KHeight,
										  val, KDrawMode[k].iDrawMode);
				SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
				CheckWriteBinary(pt[i], ptOrg[l], KDrawMode[k].iClearColorVal, KLength, KHeight);
				}
			}
		}
	}

//CFbsScreenDevice::WriteBinaryLineVertical() test.
template <class TPixelType>
void CTScaling<TPixelType>::WriteBinaryLineVertical()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteBinaryLineVertical"));

	TLineProps lineProps[] = 
		{
		{0, 0, KLegacyAppSizeHeight - 1}, 
		{KLegacyAppSizeWidth - 1, 0, KLegacyAppSizeHeight - 1},
		{17, 3, 23},
		{-2, -5, 10},
		{-6, 24, 11},
		{18, -6, 12},
		{0, -3, 13},
		{-1, 0, 14}
		};
	const TInt KLinesCnt = sizeof(lineProps) / sizeof(lineProps[0]);
	TPoint ptOrg[] = 
		{
		TPoint(22, 22),
		TPoint(19, 20)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KLinesCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				ClearScreen(KDrawMode[k]);
				SetTestData();
				SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
				const TInt KLength = 30;
				TUint32 buff[KLength];
				TUint32 buffChar = 0xFFFFFFFF;
				for(TInt ooo=0;ooo<KLength;++ooo)
					{
					buff[ooo] = buffChar;
					}
				TRgb val(KTestColorVal);
				iDrawDevice->WriteBinaryLineVertical(lineProps[i].iX, lineProps[i].iY,
													  buff, lineProps[i].iLength, val, 
													  KDrawMode[k].iDrawMode, EFalse);
				SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
				CheckVertLine(lineProps[i], ptOrg[l], KDrawMode[k].iClearColorVal);
				}
			}
		}
	}

//CFbsScreenDevice::WriteBinaryLine() test.
template <class TPixelType>
void CTScaling<TPixelType>::WriteBinaryLine()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteBinaryLiine"));

	TPoint pt[] =
		{
		TPoint(0, 0), 
		TPoint(1, 7),
		TPoint(18, -8),
		TPoint(-7, 26),
		TPoint(-4, -7),
		TPoint(0, -2),
		TPoint(34, -1),
		TPoint(-1, 17)
		};
	const TInt KPtCnt = sizeof(pt) / sizeof(pt[0]);
	TPoint ptOrg[] = 
		{
		TPoint(21, 35),
		TPoint(40, 28)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KPtCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				ClearScreen(KDrawMode[k]);
				SetTestData();
				SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
				const TInt KHeight = 1;
				const TInt KLength = 11;
				TUint32 buff[KHeight];
				TUint32 buffChar = 0xFFFFFFFF;
				for(TInt ooo=0;ooo<KHeight;++ooo)
					{
					buff[ooo] = buffChar;
					}
				TRgb val(KTestColorVal);
				iDrawDevice->WriteBinaryLine(pt[i].iX, pt[i].iY, buff, KLength,
											  val, KDrawMode[k].iDrawMode);
				SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
				CheckWriteBinary(pt[i], ptOrg[l], KDrawMode[k].iClearColorVal, KLength, KHeight);
				}
			}
		}
	}

//CFbsScreenDevice::WriteRgbAlphaMulti() test.
template <class TPixelType>
void CTScaling<TPixelType>::WriteRgbAlphaMulti()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteRgbAlphaMulti"));

	TLineProps lineProps[] = 
		{
		{0, 0, KLegacyAppSizeWidth - 1}, 
		{0, KLegacyAppSizeHeight - 1, KLegacyAppSizeWidth - 1},
		{17, 3, KLegacyAppSizeWidth / 2},
		{-8, -8, 11},
		{-3, 15, 12},
		{29, -4, 13},
		{0, -3, 14},
		{-5, 0, 15}
		};
	const TInt KLinesCnt = sizeof(lineProps) / sizeof(lineProps[0]);
	TPoint ptOrg[] = 
		{
		TPoint(24, 17),
		TPoint(27, 20)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KLinesCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				ClearScreen(KDrawMode[k]);
				SetTestData();
				SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
				TUint8 maskBuff[KLegacyAppSizeWidth];
				TUint8 maskChar = 0xF1;
				Mem::Fill(maskBuff, sizeof(maskBuff), maskChar);
				TRgb val(KTestColorVal);
				iDrawDevice->WriteRgbAlphaMulti(lineProps[i].iX, lineProps[i].iY,
												 lineProps[i].iLength, val, maskBuff);
				SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
				CheckRgbAlphaLine(lineProps[i], ptOrg[l], KDrawMode[k].iClearColorVal);
				}
			}
		}
	}

//CFbsScreenDevice::ShadowArea() test.
template <class TPixelType>
void CTScaling<TPixelType>::ShadowArea()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::ShadowArea"));

	TRect rcProps[] =
		{
		TRect(TPoint(0, 0), TSize(KLegacyAppSizeWidth - 1, KLegacyAppSizeHeight - 1)), 
		TRect(TPoint(17, 11), TSize(KLegacyAppSizeWidth / 2, KLegacyAppSizeHeight / 2)),
		TRect(TPoint(-1, -1), TSize(1, 1)),
		TRect(TPoint(-4, -5), TSize(11, 8)),
		TRect(TPoint(0, -6), TSize(3, 23)),
		TRect(TPoint(-7, 0), TSize(24, 2)),
		TRect(TPoint(5, -2), TSize(8, 9)),
		TRect(TPoint(-4, 16), TSize(11, 8))
		};
	const TInt KRcCnt = sizeof(rcProps) / sizeof(rcProps[0]);
	TPoint ptOrg[] = 
		{
		TPoint(25, 24),
		TPoint(31, 29)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KRcCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				TDrawModeProps drawModeProps(KDrawMode[k]);
				--drawModeProps.iClearColorVal;//I want to avoid "255" clear color value.
				for(TInt m=0;m<KShadowModesCnt;++m)
					{
					iDrawDevice->SetShadowMode(KShadowMode[m]);
					ClearScreen(drawModeProps);
					SetTestData();
					SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
					iDrawDevice->ShadowArea(rcProps[i]);
					SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
					CheckRect(rcProps[i], ptOrg[l], drawModeProps.iClearColorVal);
					}
				}
			}
		}
	iDrawDevice->SetShadowMode(CFbsDrawDevice::ENoShadow);
	}

//CFbsScreenDevice::WriteRgbAlphaLine() test.
template <class TPixelType>
void CTScaling<TPixelType>::WriteRgbAlphaLine2()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice::WriteRgbAlphaLine-2"));

	TLineProps lineProps[] = 
		{
		{0, 0, KLegacyAppSizeWidth - 1}, 
		{0, KLegacyAppSizeHeight - 1, KLegacyAppSizeWidth - 1},
		{17, 3, KLegacyAppSizeWidth / 2},
		{-1, -7, 11},
		{0, -5, 12},
		{-3, 0, 13},
		{15, -7, 14},
		{-1, -7, 15},
		{-1, -7, 16}
		};
	const TInt KLinesCnt = sizeof(lineProps) / sizeof(lineProps[0]);
	TPoint ptOrg[] = 
		{
		TPoint(18, 28),
		TPoint(15, 15)
		};
	const TInt KOriginsCnt = sizeof(ptOrg) / sizeof(ptOrg[0]);
	for(TInt l=0;l<KOriginsCnt;++l)
		{
		for(TInt i=0;i<KLinesCnt;++i)
			{
			for(TInt k=0;k<KDrawModesCnt;++k)
				{
				ClearScreen(KDrawMode[k]);
				SetTestData();
				SetScalingSettings(ptOrg[l], KScalingFactorX, KScalingFactorY, 1, 1);
				TUint8 rgbBuff1[KLegacyAppSizeWidth * 3];
				TUint8 rgbBuff2[KLegacyAppSizeWidth * 3];
				Mem::Fill(rgbBuff1, sizeof(rgbBuff1), KTestColorVal - 15);
				Mem::Fill(rgbBuff2, sizeof(rgbBuff2), KTestColorVal + 22);
				TUint8 maskBuff[KLegacyAppSizeWidth];
				TUint8 maskChar = 0xF1;
				Mem::Fill(maskBuff, sizeof(maskBuff), maskChar);
				iDrawDevice->WriteRgbAlphaLine(lineProps[i].iX, lineProps[i].iY,
									  lineProps[i].iLength, rgbBuff1, rgbBuff2, maskBuff,
									  KDrawMode[k].iDrawMode);
				SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
				CheckRgbAlphaLine(lineProps[i], ptOrg[l], KDrawMode[k].iClearColorVal);
				}
			}
		}
	}

template <class TPixelType>
void CTScaling<TPixelType>::TestScalingSettingsInterface()
	{
	INFO_PRINTF1(_L("MScalingSettings functionality test"));
	TEST(iDrawDevice != NULL);	

	MScalingSettings* scalingSettings = NULL;
	TInt err = iDrawDevice->GetInterface(KScalingSettingsInterfaceID, 
										  reinterpret_cast <TAny*&> (scalingSettings));
	TEST2(err, KErrNone);
	TEST(scalingSettings != NULL);

	TEST(scalingSettings->IsScalingOff());

	const TInt factorXIn = 10, factorYIn = 13, divisorXIn = 1, divisorYIn = 1;
	TInt factorXOut = -1, factorYOut = -1, divisorXOut = -1, divisorYOut = -1;

	err = scalingSettings->Set(factorXIn, factorYIn, divisorXIn, divisorYIn);
	TEST2(err, KErrNone);
	TEST(!scalingSettings->IsScalingOff());
	scalingSettings->Get(factorXOut, factorYOut, divisorXOut, divisorYOut);

	TEST(factorXOut == factorXOut);
	TEST(factorYIn == factorYOut);
	TEST(divisorXIn == divisorXOut);
	TEST(divisorYIn == divisorYOut);

	MDrawDeviceOrigin* originInterface = NULL;
	err = iDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID, 
									 reinterpret_cast <TAny*&> (originInterface));
	TEST2(err, KErrNone);
	TEST(originInterface != NULL);

	const TPoint ptOriginIn(20, 45);
	TPoint ptOriginOut;
	err = originInterface->Set(ptOriginIn);
	TEST2(err, KErrNone);
	originInterface->Get(ptOriginOut);
	TEST(ptOriginIn == ptOriginOut);

	SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);
	}

//Creates screen device and initializes ::DrawDevice global variable.
template <class TPixelType>
void CTScaling<TPixelType>::CreateScreenDeviceL()
	{
	if (iDisplayMode == ENone)
		return;
	
	TInt address = NULL;
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayMemoryAddress,address));
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayXPixels, iPhysSize.iWidth));
	User::LeaveIfError(HAL::Get(KDefaultScreenNo, HALData::EDisplayYPixels, iPhysSize.iHeight));
	__ASSERT_DEBUG(iPhysSize.iWidth > 0 && iPhysSize.iHeight > 0 && address != NULL, User::Invariant());

	TPckgBuf<TScreenInfoV01> info;
	info().iScreenAddressValid = ETrue;
	info().iScreenAddress = reinterpret_cast <void*> (address);
	info().iScreenSize = iPhysSize;

	iDrawDevice = CFbsDrawDevice::NewScreenDeviceL(info(), iDisplayMode);
	TestScalingSettingsInterface();
	iBits = new (ELeave) TPixelType[::ByteSize(EColor256, iPhysSize)];
	iDrawDevice->SetUserDisplayMode(iDisplayMode);
	iDrawDevice->SetAutoUpdate(EFalse);
	iDrawDevice->SetBits(iBits);
	

	}

template <class TPixelType>
void CTScaling<TPixelType>::PerformanceTest()
	{
	INFO_PRINTF1(_L("CFbsDrawDevice, scaling - WriteRgb() performance test"));

	const TInt KDrawingsCnt = 1000000;
	TInt i, x, y;

	TUint time1 = User::TickCount();
	x = y = 0;
	for(i=0;i<KDrawingsCnt;i++)
		{
		if(++x > 50)
			{
			x = 0;
			if(++y > 50)
				{
				y = 0;
				}
			}
		TRgb val(0x11, 0x12, i);
		iDrawDevice->WriteRgb(x, y, val, CGraphicsContext::EDrawModePEN);
		}
	time1 = User::TickCount() - time1;

	TPoint ptOrigin(5, 3);
	SetScalingSettings(ptOrigin, KScalingFactorX, KScalingFactorY, 1, 1);

	TUint time2 = User::TickCount();
	x = y = 0;
	for(i=0;i<KDrawingsCnt;i++)
		{
		if(++x > 50)
			{
			x = 0;
			if(++y > 50)
				{
				y = 0;
				}
			}
		TRgb val(0x11, 0x12, i);
		iDrawDevice->WriteRgb(x, y, val, CGraphicsContext::EDrawModePEN);
		}
	time2 = User::TickCount() - time2;

	SetScalingSettings(TPoint(0, 0), 1, 1, 1, 1);

	RDebug::Print(_L("Non-scaled device, time=%d\r\n"), time1);
	RDebug::Print(_L("Scaled device, time=%d\r\n"), time2);
	}

template <class TPixelType>
void CTScaling<TPixelType>::RunTestCaseL(TInt aCurTestCase)
	{
	// EColor64K and EColor256 is not supported, stop the test
	if (iDisplayMode == ENone)
		{
		INFO_PRINTF1(_L("EColor64K and EColor256 are not supported, The test is not run"));
		TestComplete();
		}
	
	else
		{
	    ((CTScalingStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
		switch(aCurTestCase)
			{
		case 1:
			{
			if(iCurOrientation >= KOrientationsCnt)
				{
				((CTScalingStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
				TestComplete();
				}
			else
				{
				((CTScalingStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
				if (iCurOrientation==CFbsDrawDevice::EOrientationRotated90 || iCurOrientation==CFbsDrawDevice::EOrientationRotated270)
					{
					iScalingFactorX=KScalingFactorY;
					iScalingFactorY=KScalingFactorX;
					}
				else
					{
					iScalingFactorX=KScalingFactorX;
					iScalingFactorY=KScalingFactorY;
					}
				INFO_PRINTF3(_L("Set scalling %d,%d"),iScalingFactorX,iScalingFactorY);
				if(iDrawDevice->SetOrientation(KOrientation[iCurOrientation]))
					{
					INFO_PRINTF2(_L("Set orientation: ===EOrientation%S==="),&RotationName(iCurOrientation));
					}
				else
					{
					INFO_PRINTF2(_L("Failed to set orientation: ===EOrientation%S==="),&RotationName(iCurOrientation));
					ResetCounter();
					}
				iCurOrientation++;
				}	
			}	
			break;
		case 2:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0022
*/
            ((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0022"));
			WriteLine();
			break;
		case 3:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0023
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0023"));
			WriteRgb();
			break;
		case 4:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0024
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0024"));
			WriteRgbMulti();
			break;
		case 5:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0025
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0025"));
			WriteRgbAlphaLine();
			break;
		case 6:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0026
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0026"));
			WriteBinary();
			break;
		case 7:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0027
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0027"));
			WriteBinaryLineVertical();
			break;
		case 8:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0028
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0028"));
			WriteBinaryLine();
			break;
		case 9:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0029
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0029"));
			WriteRgbAlphaMulti();
			break;
		case 10:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0030
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0030"));
			ShadowArea();
			break;
		case 11:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0031
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0031"));
			WriteRgbAlphaLine2();
			break;
		case 12:
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0032
*/
			((CTScalingStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0032"));
			PerformanceTest();
			break;
		case 13:
			((CTScalingStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			ResetCounter();
			break;
			}
           ((CTScalingStep*)iStep)->RecordTestResultL();
		}
	}
	
static TDisplayMode GetDisplayModeL()
	{
	TDisplayMode mode = EColor64K;
	CFbsDrawDevice* device = NULL;
	TRAPD(err, device = CFbsDrawDevice::NewScreenDeviceL(KDefaultScreenNo, mode));
	if (err!=KErrNone)
		{
		mode = EColor256;
		TRAPD(err, device = CFbsDrawDevice::NewScreenDeviceL(KDefaultScreenNo, mode));
		
		if (err == KErrNotSupported)
			{
			return ENone;
			}
		}
	delete device;
	return mode;
	}

//---------------
CTScalingStep::CTScalingStep() 
	{ 
	SetTestStepName(KTScalingStep); 
	} 
	 
CTGraphicsBase* CTScalingStep::CreateTestL()
	{ 
	CTGraphicsBase* theTest = NULL;
	switch (GetDisplayModeL())
		{
		case EColor64K:
			{
			INFO_PRINTF1(_L("Scaling - EColor64K"));
			theTest = new (ELeave) CTestColor64K(this, EColor64K);
			}
			break;
	
		case EColor256:
			{
			INFO_PRINTF1(_L("Scaling - EColor256"));
			theTest = new (ELeave) CTestColor256(this, EColor256);
			}
			break;

		default:
			INFO_PRINTF1(_L("EColor64K and EColor256 are not supported"));
			theTest = new (ELeave) CTestNone(this, ENone);			
		}
	return 	theTest;		
	} 

void CTScalingStep::TestSetupL()
	{
	TInt temp = 0;
	HAL::Get(KDefaultScreenNo, HALData::EDisplayColors, temp);//force HAL memory allocation
	
	}
