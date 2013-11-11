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
//

TBool TUncompressedVideoFormat::operator==(const TUncompressedVideoFormat& aOther) const
	{
	TBool result = EFalse;
	if (iDataFormat == aOther.iDataFormat)
		{
		switch (iDataFormat)
			{
		case EYuvRawData:
			result = (iYuvFormat == aOther.iYuvFormat);
			break;
		case ERgbRawData:
		case ERgbFbsBitmap:
			result = (iRgbFormat == aOther.iRgbFormat);
			break;
		default:
			DevVideoPanic(EDevVideoPanicPreConditionViolation);
			break;
			}
		}
	return result;
	}

void TUncompressedVideoFormat::operator=(const TUncompressedVideoFormat& aOther)
	{
	iDataFormat = aOther.iDataFormat;
	switch (iDataFormat)
		{
	case EYuvRawData:
		iYuvFormat = aOther.iYuvFormat;
		break;
	case ERgbRawData:
	case ERgbFbsBitmap:
		iRgbFormat = aOther.iRgbFormat;
		break;
	default:
		DevVideoPanic(EDevVideoPanicPreConditionViolation);
		break;
		}
	
	}


TBool TYuvFormat::operator==(const TYuvFormat& aOther) const
	{
	return (iCoefficients==aOther.iCoefficients
		 && iPattern == aOther.iPattern
		 && iDataLayout == aOther.iDataLayout
		 && iYuv2RgbMatrix == aOther.iYuv2RgbMatrix
		 && iRgb2YuvMatrix == aOther.iYuv2RgbMatrix
		 && iAspectRatioNum == aOther.iAspectRatioNum
		 && iAspectRatioDenom == aOther.iAspectRatioDenom);
	}

