// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bitdev.h>
#include <hal.h>
#include "TBMP.H"
#include "TClip.h"
#include <graphics/fbsdefs.h>

//Note: Some display modes will be scaled - see 
//CreateTestEnvironment(), SetScalingFactor() call

//aTestAppNo is a new argument and it is not used yet.
//It can have 0 or 1 value depending on which test app 
//uses TestClip functionality - TClip or TClip2.
CTClip::CTClip(CTestStep* aStep,
					TInt aTestAppNo,
					CFbsScreenDevice* aDev,
					CFbsBitGc* aCon,
					CFbsBitmap* aBmp,
					CFbsFont* aFont,
				   	CFbsFont* aLargeFont):
	CTGraphicsBase(aStep),
	iTestAppNo(aTestAppNo),
	iDev(aDev),
	iCon(aCon),
	iClientRect(TRect(0,0,0,0)),
	iGopNum(EFirstGop),
	iClipList(NULL),
	iBitmap(aBmp),
	iBitmap64K(NULL),
	iBitmap16MU(NULL),
	iBitmap16MA(NULL), 
	iBitmapMask(NULL),
	iBitmapAlpha(NULL),
	iFont(aFont),
	iLargeFont(aLargeFont),
	iBmpSize(TSize(0,0))
	{}

void CTClip::ConstructL()
	{
	iClipList=(TRect*)User::Alloc(sizeof(TRect)*ERegions);
	if(!iClipList)
		User::Panic(_L("Construct failure"),KErrGeneral);
	iBmpSize=iBitmap->SizeInPixels();
	iBitmap64K=new(ELeave) CFbsBitmap();
	iBitmap16MU=new(ELeave) CFbsBitmap();
	iBitmap16MA=new(ELeave) CFbsBitmap();
	iBitmapMask=new(ELeave) CFbsBitmap();
	iBitmapAlpha=new(ELeave) CFbsBitmap();
	}

TBool CTClip::SetUpTest(TInt &aTestModeIndex)
	{
	CFbsBitGc::TGraphicsOrientation orientation;
	do
		{
		orientation=(CFbsBitGc::TGraphicsOrientation)(aTestModeIndex/2);
		if (orientation>CFbsBitGc::EGraphicsOrientationRotated270)
			return(EFalse);
		aTestModeIndex++;
		TPoint scalingOrigin(0,0);
		TInt scale=1;
		if ((aTestModeIndex&0x1)==CFbsBitGc::EGraphicsOrientationNormal)
			{
			scalingOrigin=TPoint(20, 10);
			scale=2;
			}
		iDev->SetScalingFactor(scalingOrigin, scale, scale, 1, 1);
		} while(!iCon->SetOrientation(orientation));
	iDev->GetDrawRect(iClientRect);
	TInt dims[2*ERegions]={0,0, 1,1, 1,2, 2,1, 2,2, 1,10, 10,1, 2,10, 10,2, 10,10};
	for(TUint count=0;count<ERegions;count++)
		{
		iClipList[count].iTl.iX=iClientRect.iBr.iX*2/3;
		iClipList[count].iTl.iY=iClientRect.iBr.iY/3;
		iClipList[count].iBr.iX=iClipList[count].iTl.iX+dims[count*2];
		iClipList[count].iBr.iY=iClipList[count].iTl.iY+dims[count*2+1];
		}
	iDev->SetAutoUpdate(ETrue);
	iCon->SetPenStyle(CGraphicsContext::ESolidPen);
	iCon->SetPenColor(KRgbBlack);
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iCon->SetBrushColor(TRgb(170,170,170));
	Clear();
	iCon->UseBrushPattern(iBitmap);

	TSize halfScreen(iClientRect.Width()/2-2, iClientRect.Height()-2);
	User::LeaveIfError(iBitmap64K->Load(_L("z:\\system\\data\\16ram.mbm"),0,EFalse));
	User::LeaveIfError(iBitmap64K->Resize(halfScreen));
	User::LeaveIfError(iBitmap16MU->Load(_L("z:\\system\\data\\32ram.mbm"),0,EFalse));
	User::LeaveIfError(iBitmap16MU->Resize(halfScreen));

	User::LeaveIfError(iBitmap16MA->Load(_L("z:\\system\\data\\32ram.mbm"),0,EFalse));
	User::LeaveIfError(iBitmap16MA->Resize(halfScreen));
	User::LeaveIfError(iBitmapMask->Create(halfScreen, EGray2));
	User::LeaveIfError(iBitmapAlpha->Create(halfScreen, EGray256));
	return(ETrue);
	}

inline CTClipStep* CTClip::Step()
	{
	return static_cast<CTClipStep*>(iStep);
	}

void CTClip::Clear()
	{
	iCon->SetPenStyle(CGraphicsContext::ENullPen);
	iCon->SetBrushColor(KRgbWhite);
	iCon->DrawRect(iClientRect);
	iCon->SetPenStyle(CGraphicsContext::ESolidPen);
	iCon->SetBrushColor(TRgb(170,170,170));
	}

CTClip::~CTClip()
	{
	for(TInt i=0;i<ERegions;i++)
		{
		iClipList[i].~TRect();
		}
	User::Free(iClipList);
	delete iBitmap64K;
	delete iBitmap16MU;
	delete iBitmapMask;
	delete iBitmapAlpha;
	delete iBitmap16MA;
	}

void CTClip::RunTestCaseL(const TInt aCurTestCase)
	{
	((CTClipStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTClipStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0042"));
		TestComplexRgn();
		INFO_PRINTF1(_L("Complex Rgn Test complete"));
		break;
	case 2:
		((CTClipStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0038"));
		TestRectRgn();
		INFO_PRINTF1(_L("Rect Rgn Test complete"));
		break;
	case 3:
#if !defined(__X86GCC__)		//Tests take too long to run and don't test anything useful anyway
		((CTClipStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0039"));
		TestSimpleRgn();
		INFO_PRINTF1(_L("Simple Rgn Test complete"));
		break;
	case 4:
		((CTClipStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0041"));
		TestHoriRgn();
		INFO_PRINTF1(_L("Horizontal Rgn Test complete"));
		break;
	case 5:
		((CTClipStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0040"));
		TestVertRgn();
		INFO_PRINTF1(_L("Vertical Rgn Test complete"));
		break;
	case 6:
#endif	//__X86GCC__
		((CTClipStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTClipStep*)iStep)->CloseTMSGraphicsStep();
		INFO_PRINTF1(_L("Test complete\n"));
		TestComplete();
		break;
		}
	((CTClipStep*)iStep)->RecordTestResultL();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0038
 
  @SYMDEF             

  @SYMTestCaseDesc Tests clipping to a rect region
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Iterates through a number of regions and graphic operations testing clipping with each
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTClip::TestRectRgn()
	{
	TInt testMode=0;
	while(SetUpTest(testMode))
		{
		for(TUint j=0;j<ERegions;j++)
			{
			TRect reg=iClipList[j];
			for(iGopNum=EFirstGop+1;iGopNum<EGraphicsOps;iGopNum++)
				{
				RRegion rectreg(reg);
				DrawToScreen(rectreg,iClientRect);
				rectreg.Close();
				ScanArea(iClientRect.iBr.iX/2,iClientRect.iTl.iY,iClientRect.iBr.iX/2,reg.iTl.iY-iClientRect.iTl.iY); // above
				ScanArea(iClientRect.iBr.iX/2,reg.iTl.iY,reg.iTl.iX-iClientRect.iBr.iX/2,reg.Height()); // left
				ScanArea(reg.iBr.iX,reg.iTl.iY,iClientRect.iBr.iX-reg.iBr.iX,reg.Height()); // right
				ScanArea(iClientRect.iBr.iX/2,reg.iBr.iY,iClientRect.iBr.iX/2,iClientRect.iBr.iY-reg.iBr.iY); // below
				CheckInside(reg,iClientRect);
				}
			}
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0039
 
  @SYMDEF             

  @SYMTestCaseDesc Tests clipping to a simple region
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Iterates through a number of simple regions and graphic operations testing clipping with each
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTClip::TestSimpleRgn()
	{
	TInt testMode=0;
	while(SetUpTest(testMode))
		{
		for(TUint j=0;j<ERegions;j++)
			{
			TRect reg=iClipList[j];
			for(iGopNum=EFirstGop+1;iGopNum<EGraphicsOps;iGopNum++)
				{
				DrawToScreen(reg,iClientRect);
				ScanArea(iClientRect.iBr.iX/2,iClientRect.iTl.iY,iClientRect.iBr.iX/2,reg.iTl.iY); // above
				ScanArea(iClientRect.iBr.iX/2,reg.iTl.iY,reg.iTl.iX-iClientRect.iBr.iX/2,reg.Height()); // left
				ScanArea(reg.iBr.iX,reg.iTl.iY,iClientRect.iBr.iX-reg.iBr.iX,reg.Height()); // right
				ScanArea(iClientRect.iBr.iX/2,reg.iBr.iY,iClientRect.iBr.iX/2,iClientRect.iBr.iY-reg.iBr.iY); // below
				CheckInside(reg,iClientRect);
				}
			}
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0040
 
  @SYMDEF             

  @SYMTestCaseDesc Tests clipping to a vertical region
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Iterates through a number of regions of different widths and graphic operations testing clipping with each
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTClip::TestVertRgn()
	{
	TInt widths[3]={1,2,5};
	TInt testMode=0;
	while(SetUpTest(testMode))
		{
		for(TInt count=0;count<3;count++) // check vertical strips
			{
			TInt i=widths[count];
			for(iGopNum=EFirstGop+1;iGopNum<EGraphicsOps;iGopNum++)
				{
				for(TInt j=0;j<iClientRect.iBr.iX/2-i;j+=35)
					{
					TRect reg=TRect(iClientRect.iBr.iX/2+j,iClientRect.iTl.iY,iClientRect.iBr.iX/2+j+i,iClientRect.iBr.iY);
					RRegion rectreg(reg);
					DrawToScreen(rectreg,iClientRect);
					rectreg.Close();
		 			ScanArea(iClientRect.iBr.iX/2,reg.iTl.iY,reg.iTl.iX-iClientRect.iBr.iX/2,reg.Height()); // left
					ScanArea(reg.iBr.iX,reg.iTl.iY,iClientRect.iBr.iX-reg.iBr.iX,reg.Height()); // right
					CheckInside(reg,iClientRect);
					}
				}
			}
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0041
 
  @SYMDEF             

  @SYMTestCaseDesc Tests clipping to a horizontal region
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Iterates through a number of regions of different widths and graphic operations testing clipping with each
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTClip::TestHoriRgn()
	{
	TInt widths[3]={1,2,5};
	TInt testMode=0;
	while(SetUpTest(testMode))
		{
		for(TInt count=0;count<3;count++) // check horizontal strips
			{
			TInt i=widths[count];
			for(iGopNum=EFirstGop+1;iGopNum<EGraphicsOps;iGopNum++)
				{
				for(TInt j=0;j<iClientRect.iBr.iY;j+=35)
					{
					TRect reg=TRect(iClientRect.iBr.iX/2,j,iClientRect.iBr.iX,j+i);
					RRegion rectreg(reg);
					DrawToScreen(rectreg,iClientRect);
					rectreg.Close();
					ScanArea(iClientRect.iBr.iX/2,iClientRect.iTl.iY,iClientRect.iBr.iX/2,reg.iTl.iY); // above
					ScanArea(iClientRect.iBr.iX/2,reg.iBr.iY,iClientRect.iBr.iX/2,iClientRect.iBr.iY-reg.iBr.iY); // below
					CheckInside(reg,iClientRect);
					}
				}
			}
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0042
 
  @SYMDEF             

  @SYMTestCaseDesc Tests clipping to a complex region
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Creates a complex region then iterates through graphic operations testing clipping with each
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTClip::TestComplexRgn()
	{
	TInt testMode=0;
	while(SetUpTest(testMode))
		{
		TRect rect[8];
		rect[0].SetRect(iClientRect.iBr.iX*12/20,0,iClientRect.iBr.iX*15/20,iClientRect.iBr.iY/4);
		rect[1].SetRect(iClientRect.iBr.iX*11/20,iClientRect.iBr.iY/8,iClientRect.iBr.iX*13/20,iClientRect.iBr.iY/2);
		rect[2].SetRect(iClientRect.iBr.iX*14/20,iClientRect.iBr.iY/8,iClientRect.iBr.iX*17/20,iClientRect.iBr.iY/2);
		rect[3].SetRect(iClientRect.iBr.iX*12/20,iClientRect.iBr.iY*3/8,iClientRect.iBr.iX*18/20,iClientRect.iBr.iY*5/8);
		rect[4].SetRect(iClientRect.iBr.iX*13/20,iClientRect.iBr.iY*9/16,iClientRect.iBr.iX*19/20,iClientRect.iBr.iY*7/8);
		rect[5].SetRect(iClientRect.iBr.iX*17/20,iClientRect.iBr.iY*3/4,iClientRect.iBr.iX,iClientRect.iBr.iY);
		rect[6].SetRect(iClientRect.iBr.iX*11/20,iClientRect.iBr.iY*5/8,iClientRect.iBr.iX*11/20+1,iClientRect.iBr.iY*5/8+1);
		rect[7].SetRect(iClientRect.iBr.iX*18/20,iClientRect.iBr.iY/8,iClientRect.iBr.iX*18/20,iClientRect.iBr.iY/8);
		TRect space[12];
		space[0].SetRect(iClientRect.iBr.iX/2,0,rect[1].iTl.iX,rect[1].iBr.iY);
		space[1].SetRect(space[0].iBr.iX,0,rect[0].iTl.iX,rect[1].iTl.iY);
		space[2].SetRect(rect[0].iBr.iX,0,rect[2].iBr.iX,rect[2].iTl.iY);
		space[3].SetRect(rect[2].iBr.iX,0,iClientRect.iBr.iX,rect[3].iTl.iY);
		space[4].SetRect(rect[1].iBr.iX,rect[0].iBr.iY,rect[2].iTl.iX,rect[3].iTl.iY);
		space[5].SetRect(iClientRect.iBr.iX/2,rect[1].iBr.iY,rect[3].iTl.iX,rect[6].iTl.iY);
		space[6].SetRect(iClientRect.iBr.iX/2,rect[6].iTl.iY,rect[6].iTl.iX,rect[6].iBr.iY);
		space[7].SetRect(iClientRect.iBr.iX/2,rect[6].iBr.iY,rect[6].iBr.iX,iClientRect.iBr.iY);
		space[8].SetRect(rect[6].iBr.iX,rect[6].iTl.iY,rect[4].iTl.iX,iClientRect.iBr.iY);
		space[9].SetRect(rect[4].iTl.iX,rect[4].iBr.iY,rect[5].iTl.iX,iClientRect.iBr.iY);
		space[10].SetRect(rect[4].iBr.iX,rect[4].iTl.iY,iClientRect.iBr.iX,rect[5].iTl.iY);
		space[11].SetRect(rect[3].iBr.iX,rect[3].iTl.iY,iClientRect.iBr.iX,rect[4].iTl.iY);
		RRegion creg(rect[0],8);
		creg.AddRect(rect[1]);
		creg.AddRect(rect[2]);
		creg.AddRect(rect[3]);
		creg.AddRect(rect[4]);
		creg.AddRect(rect[5]);
		creg.AddRect(rect[6]);
		creg.AddRect(rect[7]);
		for(iGopNum=EFirstGop+1;iGopNum<EGraphicsOps;iGopNum++)
			{
			DrawToScreen(creg,iClientRect);
			TInt count=0;
			for(;count<8;count++)
				CheckInside(rect[count],iClientRect);
			for(count=0;count<12;count++)
				ScanArea(space[count].iTl.iX,space[count].iTl.iY,space[count].iBr.iX-space[count].iTl.iX,space[count].iBr.iY-space[count].iTl.iY);
			}
		creg.Close();
		}
	}

void CTClip::ScanArea(TInt x,TInt y,TInt length,TInt height)
	{
	if (length<1 || height<1)
		{
		return;
		}

	HBufC8* buf = HBufC8::NewL(length * 4);
	TPtr8 des = buf->Des();
	
	for (TInt row=0; row<height; row++)
		{
		TPoint point(x, y+row);
		
		des.Zero();
		iDev->GetScanLine(des,point,length,EColor16MA);
		
		for(TInt i=0; i<des.Length(); i++)
			{
			if(des[i] != 0xFF)
				{
				INFO_PRINTF1(_L("Clipping failure outside!\n"));
				INFO_PRINTF2(_L("Graphics operation: %d\n"),iGopNum);
				INFO_PRINTF5(_L("x=%d, y=%d, length=%d, height=%d\n"),x,y,length,height);
				INFO_PRINTF3(_L("Index=%d, Found=%x, Expected=0xFF\n"),i,des[i]);
				TEST(0);
				delete buf;
				return;
				}
			}
		}
				
	delete buf;
	}

void CTClip::CheckInside(const TRect& aClip,const TRect& aRect)
	{
	const TInt length=aClip.Width();
	const TInt unOffsetX=-(aRect.iBr.iX/2);
#if defined(__X86GCC__) || defined(__MARM__)
	const TInt KMaxLinesToPrint=2;
#else
	const TInt KMaxLinesToPrint=10;
#endif

	HBufC8* unBuf = HBufC8::NewL(length * 4);
	HBufC8* clBuf = HBufC8::NewL(length * 4);
	
	TPtr8 unDes = unBuf->Des();
	TPtr8 clDes = clBuf->Des();

	TInt linesDiffs=0;
	for (TUint yy=aClip.iTl.iY; yy<aClip.iBr.iY; yy++)
		{
		TPoint unPoint(aClip.iTl.iX+unOffsetX, yy);
		TPoint clPoint(aClip.iTl.iX,           yy);
		
		unDes.Zero();
		clDes.Zero();
		iDev->GetScanLine(unDes,unPoint,length,EColor16MA);
		iDev->GetScanLine(clDes,clPoint,length,EColor16MA);
		
		TInt different = unDes.Compare(clDes);
		if (different)
			{
			if (linesDiffs++<KMaxLinesToPrint)
				{
				_LIT(KDots,"..");
				_LIT(KStart,"|.");
				_LIT(KEnd,".|");
				const TInt KMaxNumBytesToLog=14;
				TBuf<8*KMaxNumBytesToLog> buf1,buf2;
				TInt end=Min(length,KMaxNumBytesToLog);
				end*=4;
				TInt ii;
				TInt matches=ETrue;
				for (ii=0; ii<end; ++ii)
					{
					_LIT(KHex,"%02x");
					buf1.AppendFormat(KHex,unDes[ii]);
					if (unDes[ii]!=clDes[ii])
						{
						buf2.AppendFormat(KHex,clDes[ii]);
						matches=EFalse;
						}
					else
						{		//Show each pixel as |......| on the 2nd line if it matches
						TPtrC ptr(KDots);
						switch (ii%4)
							{
						case 0:
							ptr.Set(KStart);
							break;
						case 3:
							ptr.Set(KEnd);
							break;
							}
						buf2.Append(ptr);
						}
					}
				TBuf<256> buf;
				if (!matches)
					{
					_LIT(KLog,"ClipRect (%d,%d,%d,%d)  Row=%d  (First %d of %d pixels shown, only differing values shown on comparison line)");
					buf.Format(KLog,aClip.iTl.iX,aClip.iTl.iY,aClip.iBr.iX,aClip.iBr.iY,yy,end,aClip.Width());
					INFO_PRINTF1(buf);
					INFO_PRINTF1(buf1);
					INFO_PRINTF1(buf2);
					}
				else
					{
					_LIT(KLog,"ClipRect (%d,%d,%d,%d)  Row=%d  (First %d of %d pixels all match)");
					buf.Format(KLog,aClip.iTl.iX,aClip.iTl.iY,aClip.iBr.iX,aClip.iBr.iY,yy,end,aClip.Width());
					INFO_PRINTF1(buf);
					}
				}
			}
		}
	if (linesDiffs>0)
		{
		_LIT(KLog,"Clipping failure inside!  Graphics operation: %d  Lines With Diffs: %d/%d");
		INFO_PRINTF4(KLog,iGopNum,linesDiffs,aClip.iBr.iY-aClip.iTl.iY);
		}
	if (!Step()->IgnoreDiffs())
		TEST(linesDiffs==0);

	delete unBuf;
	delete clBuf;
	}

void CTClip::DrawToScreen(TRegion& clip_reg,TRect r)
	{
	Clear();
	TRect halfWid(r);
	halfWid.iBr.iX=halfWid.iBr.iX/2;
	iCon->SetClippingRect(halfWid);
	DoDraw(r,EFalse);
	iCon->CancelClippingRect();
	iCon->SetClippingRegion(&clip_reg);
	iCon->SetOrigin(TPoint(r.iBr.iX/2,0));
	DoDraw(r,ETrue);
	iCon->CancelClippingRegion();
	iCon->SetOrigin(TPoint(0,0));
	}

void CTClip::DrawToScreen(const TRect& cliprect,TRect r)
	{
	Clear();
	TRect halfWid(r);
	halfWid.iBr.iX=halfWid.iBr.iX/2;
	iCon->SetClippingRect(halfWid);
	DoDraw(r,EFalse);
	iCon->SetClippingRect(cliprect);
	iCon->SetOrigin(TPoint(r.iBr.iX/2,0));
	DoDraw(r,ETrue);
	iCon->CancelClippingRect();
	iCon->SetOrigin(TPoint(0,0));
	}

void CTClip::DoDraw(TRect r,TBool clipped)
	{
	TRect sh;
	TPoint p,z;
	switch(iGopNum)
		{
	case EPlot:
		iCon->Plot(TPoint(r.iBr.iX/6-10,r.iBr.iY/3-10));
		iCon->Plot(TPoint(r.iBr.iX/6,r.iBr.iY/3));
	break;
	case EDrawLine:
		p.SetXY(r.iBr.iX/2-1,r.iBr.iY+1);
		iCon->DrawLine(z,p);
	break;
	case EDottedLine:
		p.SetXY(r.iBr.iX/2-1,r.iBr.iY+1);
		iCon->SetPenStyle(CGraphicsContext::EDottedPen);
		iCon->DrawLine(z,p);
	break;
	case EDashedLine:
		p.SetXY(r.iBr.iX/2-1,r.iBr.iY+1);
		iCon->SetPenStyle(CGraphicsContext::EDashedPen);
		iCon->DrawLine(z,p);
	break;
	case EWideLine:
		p.SetXY(r.iBr.iX/2-11,r.iBr.iY-9);
		iCon->SetPenSize(TSize(5,5));
		iCon->DrawLine(z,p);
	break;
	case EDrawArc:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		p.SetXY(r.iBr.iX/2-1,0);
		iCon->DrawArc(sh,z,p);
	break;
	case EDottedArc:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		p.SetXY(r.iBr.iX/2-1,0);
		iCon->SetPenStyle(CGraphicsContext::EDottedPen);
		iCon->DrawArc(sh,z,p);
	break;
	case EDrawRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->DrawRect(sh);
	break;
	case EVerticalHatchRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::EVerticalHatchBrush);
		iCon->DrawRect(sh);
	break;
	case EForwardDiagonalHatchRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::EForwardDiagonalHatchBrush);
		iCon->DrawRect(sh);
	break;
	case EHorizontalHatchRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::EHorizontalHatchBrush);
		iCon->DrawRect(sh);
	break;
	case ERearwardDiagonalHatchRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::ERearwardDiagonalHatchBrush);
		iCon->DrawRect(sh);
	break;
	case ESquareCrossHatchRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::ESquareCrossHatchBrush);
		iCon->DrawRect(sh);
	break;
	case EDiamondCrossHatchRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::EDiamondCrossHatchBrush);
		iCon->DrawRect(sh);
	break;
	case EVerticalHatchEllipse:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::EVerticalHatchBrush);
		iCon->DrawEllipse(sh);
	break;
	case EForwardDiagonalHatchEllipse:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::EForwardDiagonalHatchBrush);
		iCon->DrawEllipse(sh);
	break;
	case EHorizontalHatchEllipse:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::EHorizontalHatchBrush);
		iCon->DrawEllipse(sh);
	break;
	case ERearwardDiagonalHatchEllipse:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::ERearwardDiagonalHatchBrush);
		iCon->DrawEllipse(sh);
	break;
	case ESquareCrossHatchEllipse:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::ESquareCrossHatchBrush);
		iCon->DrawEllipse(sh);
	break;
	case EDiamondCrossHatchEllipse:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetBrushStyle(CGraphicsContext::EDiamondCrossHatchBrush);
		iCon->DrawEllipse(sh);
	break;
	case EDottedRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetPenStyle(CGraphicsContext::EDottedPen);
		iCon->DrawRect(sh);
	break;
	case ECopyRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		if(clipped)
			{
			iCon->SetOrigin(z);
			iCon->CopyRect(TPoint(r.iBr.iX/2,0),sh);
			}
		else
			iCon->DrawRect(sh);
	break;
	case EDrawEllipse:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->DrawEllipse(sh);
	break;
	case EDottedEllipse:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetPenStyle(CGraphicsContext::EDottedPen);
		iCon->DrawEllipse(sh);
	break;
	case EDrawRoundRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		p.SetXY(r.iBr.iX>>2,r.iBr.iY>>4);
		iCon->DrawRoundRect(sh,p.AsSize());
	break;
	case EDottedRoundRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		p.SetXY(r.iBr.iX>>2,r.iBr.iY>>4);
		iCon->SetPenStyle(CGraphicsContext::EDottedPen);
		iCon->DrawRoundRect(sh,p.AsSize());
	break;
	case EDrawPie:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		p.SetXY(r.iBr.iX/2-1,0);
		iCon->DrawPie(sh,z,p);
	break;
	case EDottedPie:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		p.SetXY(r.iBr.iX/2-1,0);
		iCon->SetPenStyle(CGraphicsContext::EDottedPen);
		iCon->DrawPie(sh,z,p);
	break;
	case EDrawPolygon:
		{
		CArrayFixFlat<TPoint>* t=new CArrayFixFlat<TPoint>(3);
		TPoint temp(r.iBr.iX/6,r.iBr.iY/4);
		TRAPD(errCode, t->AppendL(temp));
		if(errCode != KErrNone)
			{
			return;
			}
		temp.SetXY(r.iBr.iX/12,r.iBr.iY/2);
		TRAP(errCode, t->AppendL(temp));
		if(errCode != KErrNone)
			{
			return;
			}
		temp.SetXY(r.iBr.iX/4,r.iBr.iY/2);
		TRAP(errCode, t->AppendL(temp));
		if(errCode != KErrNone)
			{
			return;
			}
		iCon->DrawPolygon(t);
		delete t;
		}
	break;
	case EDrawText:
		{
		iCon->UseFont(iFont);
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		iCon->DrawText(testtext,p);
		}
	break;
	case EBoxText:
		{
		iCon->UseFont(iFont);
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		TInt ascent=iFont->AscentInPixels();
		TInt height=iFont->HeightInPixels();
		TInt width=iFont->TextWidthInPixels(testtext);
		TRect box(p.iX,p.iY,p.iX+width,p.iY+height);
		iCon->DrawText(testtext,box,ascent);
		}
	break;
	case EDrawTextVertical:
		iCon->UseFont(iFont);
		p.SetXY(r.iBr.iX/6-13,r.iBr.iY/3+5);
		p.iX+=5;
		p.iY-=10;
		iCon->DrawTextVertical(testtext,p,EFalse);
	break;
	case ELargeText:
		iCon->UseFont(iLargeFont);
		p.SetXY(r.iBr.iX/6-13,r.iBr.iY/3+5);
		iCon->DrawText(largetesttext,p);
	break;
	case ELargeBoxText:
		{
		iCon->UseFont(iLargeFont);
		p.SetXY(r.iTl.iX, r.iTl.iY);
		TInt ascent=iLargeFont->AscentInPixels();
		TInt height=iLargeFont->HeightInPixels();
		TInt width=iLargeFont->TextWidthInPixels(largetesttext);
		TRect box(p.iX, p.iY, p.iX+width, p.iY+height);
		iCon->DrawText(largetesttext,box,ascent);
		}
	break;
	case ELargeTextVertical:
		iCon->UseFont(iLargeFont);
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		iCon->DrawTextVertical(largetesttext,p,EFalse);
	break;
	case EPaintRect:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->SetPenStyle(CGraphicsContext::EDottedPen);
		iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
		iCon->DrawRect(sh);
	break;
	case EBitBltContext:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		if(clipped)
			{
			iCon->SetPenStyle(CGraphicsContext::ESolidPen);
			iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
			iCon->BitBlt(z,*iCon,sh);
			}
		else
			{
			iCon->SetPenStyle(CGraphicsContext::EDottedPen);
			iCon->SetBrushStyle(CGraphicsContext::EPatternedBrush);
			iCon->DrawRect(sh);
			}
	break;
	case EBitmapLarge:
		sh.SetRect(0,0,r.iBr.iX/2-1,r.iBr.iY);
		iCon->DrawBitmap(sh,iBitmap);
	break;
	case EBitmapSmall:
		p.SetXY(r.iBr.iX/6-40,r.iBr.iY/3-20);
		sh.SetRect(p,TSize(iBitmap->SizeInPixels().iWidth/2,iBitmap->SizeInPixels().iHeight/2));
		iCon->DrawBitmap(sh,iBitmap);
	break;
	case EBitBltBitmap:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);		
		iCon->BitBlt(p,iBitmap64K);
	break;
	case EDrawBitmap:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(p,iBitmap64K->SizeInPixels());
		iCon->DrawBitmap(sh,iBitmap);
	break;
	case EBitBltMasked:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap64K->SizeInPixels());
		iCon->BitBltMasked(p,iBitmap,sh,iBitmap,ETrue);
	break;
	case EBitBltMaskedBinary:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap64K->SizeInPixels());
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->BitBltMasked(p,iBitmap,sh,iBitmapMask,EFalse);
	break;
	case EBitBltMaskedBinary64K:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap64K->SizeInPixels());
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->BitBltMasked(p,iBitmap64K,sh,iBitmapMask,EFalse);
	break;
	case EBitBltMaskedBinary16MU:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap16MU->SizeInPixels());
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->BitBltMasked(p,iBitmap16MU,sh,iBitmapMask,EFalse);
	break;
	case EBitBltMaskedBinary16MA:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap16MA->SizeInPixels());
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->BitBltMasked(p,iBitmap16MA,sh,iBitmapMask,EFalse);
	break;	
	case EBitBltMaskedAlpha:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap64K->SizeInPixels());
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->BitBltMasked(p,iBitmap,sh,iBitmapAlpha,EFalse);
	break;
	case EBitBltMaskedAlpha64K:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap64K->SizeInPixels());
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->BitBltMasked(p,iBitmap64K,sh,iBitmapAlpha,EFalse);
	break;
	case EBitBltMaskedAlpha16MU:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap16MU->SizeInPixels());
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->BitBltMasked(p,iBitmap16MU,sh,iBitmapAlpha,EFalse);
	break;
	case EBitBltMaskedAlpha16MA:
		p.SetXY(r.iTl.iX+1, r.iTl.iY+1);
		sh.SetRect(TPoint(0,0),iBitmap16MA->SizeInPixels());
		iCon->SetBrushStyle(CGraphicsContext::ENullBrush);
		iCon->BitBltMasked(p,iBitmap16MA,sh,iBitmapAlpha,EFalse);
	break;
		}
	iCon->SetPenStyle(CGraphicsContext::ESolidPen);
	iCon->SetPenSize(TSize(1,1));
	iCon->SetBrushStyle(CGraphicsContext::ESolidBrush);
	}

//--------------
CTClipStep::CTClipStep() :iIgnoreDiffs(EFalse)
	{
	SetTestStepName(KTClipStep);
	}
	
CTClipStep::~CTClipStep()
	{
	DestroyTestEnvironment();
	}

inline TBool CTClipStep::IgnoreDiffs()
	{
	return iIgnoreDiffs;
	}

CTGraphicsBase* CTClipStep::CreateTestL()
	{
	_LIT(KLog,"Create CTClip");
	INFO_PRINTF1(KLog);
	return new (ELeave) CTClip(this, iTestAppNo, iDev, iGc, iBmp, iFont, iLargeFont);
	}

void CTClipStep::TestSetupL()
	{
	FbsStartup();
	RFbsSession::Connect();
	
	CreateTestEnvironment();
	}
	
void CTClipStep::TestClose()
	{
	DestroyTestEnvironment();
	RFbsSession::Disconnect();
	}

TInt CTClipStep::CreateScreenDevice(TDisplayMode aMode)
	{
	TRAPD(err,iDev=CFbsScreenDevice::NewL(KNullDesC,aMode));
	const TPtrC modeName=CTGraphicsBase::ColorModeName(aMode);
	if (err==KErrNone)
		{
		_LIT(KLog,"Sucessfully created Screen Device, mode %S");
		INFO_PRINTF2(KLog,&modeName);
		}
	else
		{
		if (err==KErrNotSupported)
			{
			_LIT(KLog,"Screen Device with mode %S not available on this device");
			INFO_PRINTF2(KLog,&modeName);
			}
		else
			{
			_LIT(KLog,"Failed to created Screen Device with mode %S  err=%d");
			INFO_PRINTF3(KLog,&modeName,err);
			}
		}
	return err;
	}

void CTClipStep::CreateTestEnvironment()
	{
	iDev=NULL;
	//This test will fail if any display mode other than EColor64K, EColor256, EGray256
	//is selected
	TInt ret=CreateScreenDevice(EColor64K);
	if (ret!=KErrNone)
		ret=CreateScreenDevice(EColor256);
	if (ret!=KErrNone)
		ret=CreateScreenDevice(EGray256);
	if (ret!=KErrNone)
		ret=CreateScreenDevice(EColor16MU);
	if (ret!=KErrNone)
		ret=CreateScreenDevice(EColor16M);
	if (ret!=KErrNone)
		ret=CreateScreenDevice(EColor4K);
	if (ret!=KErrNone)
		{
		ret=CreateScreenDevice(EColor16MA);
		if (ret==KErrNone)
			{
			iIgnoreDiffs=ETrue;
			_LIT(KLog,"Ignore Diffs set to %d (ETrue?)");
			INFO_PRINTF2(KLog,iIgnoreDiffs);
			}
		}
	if (ret!=KErrNone)
		{
		ret=CreateScreenDevice(EColor16MAP);
		if (ret==KErrNone)
			{
			iIgnoreDiffs=ETrue;
			_LIT(KLog,"Ignore Diffs set to %d (ETrue?)");
			INFO_PRINTF2(KLog,iIgnoreDiffs);
			}
		}
	if(ret!=KErrNone)
		User::Panic(_L("Device not created"),ret);
	iDev->ChangeScreenDevice(NULL);
	iGc=NULL;
	ret=iDev->CreateContext(iGc);
	if(iGc==NULL)
		User::Panic(_L("Context not created"),KErrGeneral);
	iGc->Activate(iDev);
	iFont=NULL;
	TFontSpec fs(_L("Swiss"),12);
	ret=iDev->GetNearestFontToDesignHeightInPixels(iFont,fs);
	if(ret!=KErrNone)
		User::Panic(_L("Font not created"),KErrGeneral);
	iLargeFont=NULL;
	fs.iHeight=100;
	ret=iDev->GetNearestFontToDesignHeightInPixels(iLargeFont,fs);
	if(ret!=KErrNone)
		User::Panic(_L("Font not created"),KErrGeneral);
	
	iBmp=new CFbsBitmap;
	if(iBmp==NULL)
		User::Panic(_L("Bitmap not created"),KErrGeneral);
	if(iTestAppNo == 0)
		{
		// Uncompressed
		TInt ret=iBmp->Load(_L("z:\\system\\data\\tbmp.mbm"),EMbmTbmpTbmp,EFalse);
		if(ret!=KErrNone)
			User::Panic(_L("Bitmap not loaded"),ret);
		}
	else
		{
		TInt ret=iBmp->LoadAndCompress(_L("z:\\system\\data\\tbmp.mbm"),EMbmTbmpTbmp,EFalse);
		if(ret!=KErrNone)
			User::Panic(_L("Bitmap not loaded"),ret);
		}
	
	}

void CTClipStep::DestroyTestEnvironment()
	{
	delete iGc;
	iGc = NULL;
	
	delete iBmp;
	iBmp = NULL;

	if(iFont)
		{
		iDev->ReleaseFont(iFont);
		iFont = NULL;
		}
	if(iLargeFont)
		{
		iDev->ReleaseFont(iLargeFont);
		iLargeFont = NULL;	
		}
	
	delete iDev;
	iDev = NULL;
	}

//--------------
CTClip2Step::CTClip2Step() :
	CTClipStep()
	{
	iTestAppNo = 1;
	SetTestStepName(KTClip2Step);
	}
