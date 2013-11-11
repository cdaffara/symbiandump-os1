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

/**
 @file
 @test
 @internalComponent
*/

#include <e32debug.h> //RDebug

#include "t_pseudoapputils.h"

EXPORT_C void TImportScreenConfig::VerifyFramesPerTraverse(TInt& aFramesPerTraverse, TInt aScreenPixelDimension)
	{
	if(aFramesPerTraverse <= 0 || aFramesPerTraverse > aScreenPixelDimension)
		{
		RDebug::Print(_L("Illegal FramesPerTraverse value of %d Value must be between 1 and %d inclusive\n"), aFramesPerTraverse, aScreenPixelDimension);
		User::Exit(0);
		}
	else
		{
		if(aScreenPixelDimension % aFramesPerTraverse)
			{
			RDebug::Print(_L("Dimension Divisor of %d must be a sub-multiple of the screen pixel dimension %d\n"), aFramesPerTraverse, aScreenPixelDimension);
			User::Exit(0);
			}
		}
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TPtrC& aIniParam, TPtrC& aMemberVar, CIniData* aUtils)
	{
	TPtrC readInValue;

	if(!aUtils->FindVar(KSimVideoConfigSectionName, aIniParam, readInValue))
		{
		RDebug::Print(_L("Could not read %s value from config file, aborting\n"), aIniParam.Ptr());
		User::Exit(0);
		}
	else
		{
		aMemberVar.Set(readInValue);
		}
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TDesC& tempStore, const TPtrC& aIniParam, TPtrC& aMemberVar, CIniData* aUtils)
	{
	TBuf<KMaxUiBitmapNameLength> temp = tempStore;
	temp.Append(aIniParam);
	ReadIni(temp, aMemberVar, aUtils);
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TPtrC& aIniParam, TInt& aMemberVar, CIniData* aUtils)
	{
	TInt readInValue;

	if(!aUtils->FindVar(KSimVideoConfigSectionName, aIniParam, readInValue))
		{
		RDebug::Print(_L("Could not read %s value from config file, aborting\n"), aIniParam.Ptr());
		User::Exit(0);
		}
	else
		{
		aMemberVar = readInValue;
		}
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TDesC& tempStore, const TPtrC& aIniParam, TInt& aMemberVar, CIniData* aUtils)
	{
	TBuf<KMaxUiBitmapNameLength> temp = tempStore;
	temp.Append(aIniParam);
	ReadIni(temp, aMemberVar, aUtils);
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TPtrC& aIniParam, const TPtrC& aIniValue1, const TPtrC& aIniValue2, TInt& aMemberVar,
                                           TInt aValue1, TInt aValue2, CIniData* aUtils)
	{
	TPtrC readInValue;

	if(!aUtils->FindVar(KSimVideoConfigSectionName, aIniParam, readInValue))
		{
		RDebug::Print(_L("Could not read %s value from config file, aborting\n"), aIniParam.Ptr());
		User::Exit(0);
		}
	else
		{
		if(readInValue == aIniValue1)
			{
			aMemberVar = aValue1;
			}
		else
			{
			if(readInValue == aIniValue2)
				{
				aMemberVar = aValue2;
				}
			else
				{
				RDebug::Print(_L("Invalid %s value from config file\n"), aIniParam.Ptr());
				}
			}
		}
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TDesC& tempStore, const TPtrC& aIniParam,  const TPtrC& aIniValue1,
                                           const TPtrC& aIniValue2, TInt& aMemberVar, TInt aValue1,
                                           TInt aValue2, CIniData* aUtils)
	{
	TBuf<KMaxUiBitmapNameLength> temp = tempStore;
	temp.Append(aIniParam);
	ReadIni(temp, aIniValue1, aIniValue2, aMemberVar, aValue1, aValue2, aUtils);
	}


EXPORT_C void TImportScreenConfig::ReadIni(const TPtrC& aIniParam,  const TPtrC& aIniValue1, const TPtrC& aIniValue2, const TPtrC& aIniValue3,
                                           TInt& aMemberVar, TInt aValue1, TInt aValue2, TInt aValue3, CIniData* aUtils)
	{
	TPtrC readInValue;

	if(!aUtils->FindVar(KSimVideoConfigSectionName, aIniParam, readInValue))
		{
		RDebug::Print(_L("Could not read %s value from config file, aborting\n"), aIniParam.Ptr());
		User::Exit(0);
		}
	else
		{
		if(readInValue == aIniValue1)
			{
			aMemberVar = aValue1;
			}
		else
			{
			if(readInValue == aIniValue2)
				{
				aMemberVar = aValue2;
				}
			else
				{
				if(readInValue == aIniValue3)
					{
					aMemberVar = aValue3;
					}
				else
					{
					RDebug::Print(_L("Invalid %s value from config file\n"), aIniParam.Ptr());
					}
				}
			}
		}
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TDesC& tempStore, const TPtrC& aIniParam, const TPtrC& aIniValue1, const TPtrC& aIniValue2,
                                           const TPtrC& aIniValue3, TInt& aMemberVar, TInt aValue1, TInt aValue2,
                                           TInt aValue3, CIniData* aUtils)
	{
	TBuf<KMaxUiBitmapNameLength> temp = tempStore;
	temp.Append(aIniParam);
	ReadIni(temp, aIniValue1, aIniValue2, aIniValue3, aMemberVar, aValue1, aValue2, aValue3, aUtils);
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TPtrC& aIniParam, const TPtrC& aIniValue1, const TPtrC& aIniValue2, const TPtrC& aIniValue3,
                                           const TPtrC& aIniValue4, TInt& aMemberVar, TInt aValue1, TInt aValue2,
                                           TInt aValue3, TInt aValue4, CIniData* aUtils)
	{
	TPtrC readInValue;

	if(!aUtils->FindVar(KSimVideoConfigSectionName, aIniParam, readInValue))
		{
		RDebug::Print(_L("Could not read %s value from config file, aborting\n"), aIniParam.Ptr());
		User::Exit(0);
		}
	else
		{
		if(readInValue == aIniValue1)
			{
			aMemberVar = aValue1;
			}
		else
			{
			if(readInValue == aIniValue2)
				{
				aMemberVar = aValue2;
				}
			else
				{
				if(readInValue == aIniValue3)
					{
					aMemberVar = aValue3;
					}
				else
					{
					if(readInValue == aIniValue4)
						{
						aMemberVar = aValue4;
						}
					else
						{
						RDebug::Print(_L("Invalid %s value from config file\n"), aIniParam.Ptr());
						}
					}
				}
			}
		}
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TDesC& tempStore, const TPtrC& aIniParam,  const TPtrC& aIniValue1, const TPtrC& aIniValue2,
                                           const TPtrC& aIniValue3, const TPtrC& aIniValue4, TInt& aMemberVar,
                                           TInt aValue1, TInt aValue2, TInt aValue3,
                                           TInt aValue4, CIniData* aUtils)
	{
	TBuf<KMaxUiBitmapNameLength> temp = tempStore;
	temp.Append(aIniParam);
	ReadIni(temp, aIniValue1, aIniValue2, aIniValue3, aIniValue4, aMemberVar, aValue1, aValue2, aValue3, aValue4, aUtils);
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TPtrC& aIniParam, const TPtrC& aIniValue1, const TPtrC& aIniValue2, const TPtrC& aIniValue3,
                                           const TPtrC& aIniValue4, const TPtrC& aIniValue5, TInt& aMemberVar, TInt aValue1,
                                           TInt aValue2, TInt aValue3, TInt aValue4, TInt aValue5, CIniData* aUtils)
	{
	TPtrC readInValue;

	if(!aUtils->FindVar(KSimVideoConfigSectionName, aIniParam, readInValue))
		{
		RDebug::Print(_L("Could not read %s value from config file, aborting\n"), aIniParam.Ptr());
		User::Exit(0);
		}
	else
		{
		if(readInValue == aIniValue1)
			{
			aMemberVar = aValue1;
			}
		else
			{
			if(readInValue == aIniValue2)
				{
				aMemberVar = aValue2;
				}
			else
				{
				if(readInValue == aIniValue3)
					{
					aMemberVar = aValue3;
					}
				else
					{
					if(readInValue == aIniValue4)
						{
						aMemberVar = aValue4;
						}
					else
						{
						if(readInValue == aIniValue5)
							{
							aMemberVar = aValue5;
							}
						else
							{
							RDebug::Print(_L("Invalid %s value from config file\n"), aIniParam.Ptr());
							}
						}
					}
				}
			}
		}
	}

EXPORT_C void TImportScreenConfig::ReadIni(const TDesC& tempStore, const TPtrC& aIniParam,  const TPtrC& aIniValue1, const TPtrC& aIniValue2,
                                           const TPtrC& aIniValue3, const TPtrC& aIniValue4, const TPtrC& aIniValue5, TInt& aMemberVar,
                                           TInt aValue1, TInt aValue2, TInt aValue3, TInt aValue4,
                                           TInt aValue5, CIniData* aUtils)
	{
	TBuf<KMaxUiBitmapNameLength> temp = tempStore;
	temp.Append(aIniParam);
	ReadIni(temp, aIniValue1, aIniValue2, aIniValue3, aIniValue4, aIniValue5, aMemberVar, aValue1, aValue2, aValue3, aValue4, aValue5, aUtils);
	}

EXPORT_C TInt TImportScreenConfig::Numeral(TInt val, TInt& aDigit)
	{
	if( (val > 47) && (val < 58) )
		{
		aDigit = val - 48; //ascii to int conversion trick
		return KErrNone;
		}
	else if(val == 45)
		{
		aDigit = -1;       //ascii code 45 is a minus sign
		return KErrNone;
		}

	return KErrNotFound;
	}

EXPORT_C TInt TImportScreenConfig::ExtractNumber(const TPtrC& aRectTextString, TInt& aSearchFrom, TInt& aNumber)
	{
	TInt initFlag = 0;
	TInt digit;
	aNumber = 0;
	TInt sign = 1;

	for(TInt i=aSearchFrom; i<aRectTextString.Length(); i++)
		{
		if(Numeral(aRectTextString[i], digit) != KErrNotFound)
			{
			if(initFlag == 0)
				{
				initFlag = 1;
				if(digit == -1)
					{
					sign = digit;
					}
				else
					{
					aNumber = digit;
					}
				}
			else
				{
				if(digit == -1)
					{
					RDebug::Print(_L("Error - cannot read value from string, spurious minus sign\n"));
					return KErrNotFound;
					}
				else
					{
					aNumber = aNumber*10 + digit*sign;
					}
				}
			}
		else
			{
			if(initFlag == 1)
				{
				aSearchFrom = i + 1;
				return KErrNone;
				}
			}
		}

	if(initFlag == 1)
		{
		return KErrNone;
		}
	return KErrNotFound;
	}

EXPORT_C TInt TImportScreenConfig::ExtractRect(const TPtrC& aRectTextString, TRect& aRect, const TSize& aScreenSize)
	{
	TInt searchFrom = 0;
	TInt number = 0;
	TInt pointvalue[4] = {0};

	for(TInt i=0; i<4; i++)
		{
		if(ExtractNumber(aRectTextString, searchFrom, number) == KErrNotFound)
			{
			RDebug::Print(_L("Error - cannot read rectangle dimensions from string, setting to zero size\n"));
			aRect = TRect(0,0,0,0);
			return KErrNotFound;
			}
		else
			{
			pointvalue[i] = number;
			}
		}

	aRect = TRect(TPoint((pointvalue[0]*aScreenSize.iWidth)/100, (pointvalue[2]*aScreenSize.iHeight)/100)
	             ,TPoint((pointvalue[1]*aScreenSize.iWidth)/100, (pointvalue[3]*aScreenSize.iHeight)/100));

	return KErrNone;
	}

EXPORT_C TInt TImportScreenConfig::ExtractAspectRatio(const TPtrC& aRatioString, TSize& aAspectRatio)
	{
	TInt searchFrom = 0;
	TInt number = 0;
	TInt pointvalue[2] = {0};

	for(TInt i=0; i<2; i++)
		{
		if(ExtractNumber(aRatioString, searchFrom, number) == KErrNotFound)
			{
			RDebug::Print(_L("Error - cannot read aspect ratio dimensions from string\n"));
			return KErrNotFound;
			}
		else
			{
			pointvalue[i] = number;
			}
		}

	aAspectRatio = TSize(pointvalue[0], pointvalue[1]);
	return KErrNone;
	}

EXPORT_C TInt TImportScreenConfig::ExtractPoint(const TPtrC& aPointString, TPoint& aPoint)
	{
	TInt searchFrom = 0;
	TInt number = 0;
	TInt pointvalue[2] = {0};

	for(TInt i=0; i<2; i++)
		{
		if(ExtractNumber(aPointString, searchFrom, number) == KErrNotFound)
			{
			RDebug::Print(_L("Error - cannot read point dimensions from string\n"));
			return KErrNotFound;
			}
		else
			{
			pointvalue[i] = number;
			}
		}

	aPoint = TPoint(pointvalue[0], pointvalue[1]);
	return KErrNone;
	}

EXPORT_C TInt TImportScreenConfig::ExtractSize(const TPtrC& aSizeString, TSize& aSize)
	{
	TInt searchFrom = 0;
	TInt number = 0;
	TInt pointvalue[2] = {0};

	for(TInt i=0; i<2; i++)
		{
		if(ExtractNumber(aSizeString, searchFrom, number) == KErrNotFound)
			{
			RDebug::Print(_L("Error - cannot read size dimensions from string\n"));
			return KErrNotFound;
			}
		else
			{
			pointvalue[i] = number;
			}
		}

	aSize = TSize(pointvalue[0], pointvalue[1]);
	return KErrNone;
	}

TInt TImportScreenConfig::ReadIniAllowMissing(const TPtrC& aIniParam, TPtrC& aMemberVar, CIniData* aUtils)
	{
	TPtrC readInValue;

	if(!aUtils->FindVar(KSimVideoConfigSectionName, aIniParam, readInValue))
		{
		RDebug::Print(_L("%s value not found in config file\n"), aIniParam.Ptr());
		return KErrNotFound;
		}
	else
		{
		aMemberVar.Set(readInValue);
		return KErrNone;
		}
	}

TBool TImportScreenConfig::GetShareSurfaceInfoL(TInt aScreenNo, TInt aWindowNo, TInt& aShareScreenNumber, TInt& aShareWindowNumber, CIniData* aUtils)
	{
	TBuf<KMaxUiBitmapNameLength> tempStore;
	tempStore.Format(KScreenSurface, aScreenNo, aWindowNo);
	tempStore.Append(_L("share_surface"));
	TPtrC shareSurfaceString;
	TInt res = ReadIniAllowMissing(tempStore, shareSurfaceString, aUtils);

	if(res == KErrNotFound)
		{
		RDebug::Print(_L("%s value not found in config file\n"), tempStore.Ptr());
		return EFalse;
		}
	else if(res != KErrNone)
		{
		RDebug::Print(_L("Could not read %s value correctly from config file, aborting\n"), tempStore.Ptr());
		User::Exit(0);
		}
	
	// Format of aShareSurfaceString should be "screenX_windowY"
	TInt searchFrom = 5;
	TInt ret = ExtractNumber(shareSurfaceString, searchFrom, aShareScreenNumber);
	if(KErrNone != ret)
		{
		RDebug::Print(_L("Error - cannot read screen number from shared_surface string\n"));
		User::Exit(0);
		}

	ret = ExtractNumber(shareSurfaceString, searchFrom, aShareWindowNumber);
	if(KErrNone != ret)
		{
		RDebug::Print(_L("Error - cannot read window number from shared_surface string\n"));
		User::Exit(0);
		}

	// Read screen number and window number correctly - surface is shared
	return ETrue;
	}
