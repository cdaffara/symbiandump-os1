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

#ifndef __T_PSEUDOAPPUTILS_H__
#define __T_PSEUDOAPPUTILS_H__

#include <iniparser.h>

#include "t_wservconsts.h"			//KWServPseudoAppConfigFile

const TInt KMaxUiBitmapNameLength = 70;

_LIT(KSimVideoConfigSectionName,  "default");
_LIT(KUiBitmapScaling,	          "ui_bitmap%d_scaling");
_LIT(KWindowSizes,	              "window_size%d");
_LIT(KWindowPositions,	          "window_position%d");
_LIT(KWindowOpacitys,	          "window_opacity%d");

_LIT(KScreenSurface,	          "screen%d_window%d_");
_LIT(KScreenAnimation,	          "screen%d_window%d_animation_");


#define READ_INI5(A, B, C, D, E, F, G, H, I, J, K, M, N)  {\
                                                             TImportScreenConfig::ReadIni( TPtrC((const TText *)L ## #A) , TPtrC((const TText *)L ## #B), TPtrC((const TText *)L ## #C), TPtrC((const TText *)L ## #D), TPtrC((const TText *)L ## #E), TPtrC((const TText *)L ## #F), G, H, I, J, K, M, N);\
                                                             }

#define READ_INI5A(A, B, C, D, E, F, G, H, I, J, K, M, N, O)  {\
                                                                 TImportScreenConfig::ReadIni(A, TPtrC((const TText *)L ## #B) , TPtrC((const TText *)L ## #C), TPtrC((const TText *)L ## #D), TPtrC((const TText *)L ## #E), TPtrC((const TText *)L ## #F), TPtrC((const TText *)L ## #G), H, I, J, K, M, N, O);\
                                                                 }

#define READ_INI4(A, B, C, D, E, F, G, H, I, J, K)  {\
                                                       TImportScreenConfig::ReadIni( TPtrC((const TText *)L ## #A) , TPtrC((const TText *)L ## #B), TPtrC((const TText *)L ## #C), TPtrC((const TText *)L ## #D), TPtrC((const TText *)L ## #E), F, G, H, I, J, K);\
                                                       }

#define READ_INI4A(A, B, C, D, E, F, G, H, I, J, K, M)  {\
                                                           TImportScreenConfig::ReadIni(A, TPtrC((const TText *)L ## #B) , TPtrC((const TText *)L ## #C), TPtrC((const TText *)L ## #D), TPtrC((const TText *)L ## #E), TPtrC((const TText *)L ## #F), G, H, I, J, K, M);\
                                                           }

#define READ_INI3(A, B, C, D, E, F, G, H, I)  {\
                                                 TImportScreenConfig::ReadIni( TPtrC((const TText *)L ## #A) , TPtrC((const TText *)L ## #B), TPtrC((const TText *)L ## #C), TPtrC((const TText *)L ## #D), E, F, G, H, I);\
                                                 }

#define READ_INI3A(A, B, C, D, E, F, G, H, I, J)  {\
                                                  TImportScreenConfig::ReadIni(A, TPtrC((const TText *)L ## #B) , TPtrC((const TText *)L ## #C), TPtrC((const TText *)L ## #D), TPtrC((const TText *)L ## #E), F, G, H, I, J);\
                                                  }

#define READ_INI2(A, B, C, D, E, F, G)  {\
                                           TImportScreenConfig::ReadIni( TPtrC((const TText *)L ## #A) , TPtrC((const TText *)L ## #B), TPtrC((const TText *)L ## #C), D, E, F, G);\
                                           }

#define READ_INI2A(A, B, C, D, E, F, G, H)  {\
                                            TImportScreenConfig::ReadIni(A, TPtrC((const TText *)L ## #B) , TPtrC((const TText *)L ## #C), TPtrC((const TText *)L ## #D), E, F, G, H);\
                                            }

#define READ_INI1(A, B, C)  {\
                            TImportScreenConfig::ReadIni( TPtrC((const TText *)L ## #A) ,B ,C);\
                            }

#define READ_INI1A(A, B, C, D)  {\
                                TImportScreenConfig::ReadIni(A, TPtrC((const TText *)L ## #B) ,C ,D);\
                                }


class TImportScreenConfig
	{
public:
	IMPORT_C static void ReadIni(const TPtrC& aIniParam, TPtrC& aMemberVar, CIniData* iUtils);
	IMPORT_C static void ReadIni(const TDesC& tempStore, const TPtrC& aIniParam , TPtrC& aMemberVar, CIniData* iUtils);

	IMPORT_C static void ReadIni(const TPtrC& aIniParam,  TInt& aMemberVar, CIniData* iUtils);
	IMPORT_C static void ReadIni(const TDesC& tempStore, const TPtrC& aIniParam,  TInt& aMemberVar, CIniData* iUtils);
	IMPORT_C static void ReadIni(const TPtrC& aIniParam, const TPtrC& aIniValue1, const TPtrC& aIniValue2, TInt& aMemberVar,
                                 TInt aValue1, TInt aValue2, CIniData* aUtils);
	IMPORT_C static void ReadIni(const TDesC& tempStore, const TPtrC& aIniParam,  const TPtrC& aIniValue1,
                                 const TPtrC& aIniValue2, TInt& aMemberVar, TInt aValue1,
                                 TInt aValue2, CIniData* aUtils);
	IMPORT_C static void ReadIni(const TPtrC& aIniParam,  const TPtrC& aIniValue1, const TPtrC& aIniValue2, const TPtrC& aIniValue3,
                                 TInt& aMemberVar, TInt aValue1, TInt aValue2, TInt aValue3, CIniData* aUtils);
	IMPORT_C static void ReadIni(const TDesC& tempStore, const TPtrC& aIniParam, const TPtrC& aIniValue1, const TPtrC& aIniValue2,
                                 const TPtrC& aIniValue3, TInt& aMemberVar, TInt aValue1, TInt aValue2,
                                 TInt aValue3, CIniData* aUtils);
	IMPORT_C static void ReadIni(const TPtrC& aIniParam, const TPtrC& aIniValue1, const TPtrC& aIniValue2, const TPtrC& aIniValue3,
                                 const TPtrC& aIniValue4, TInt& aMemberVar, TInt aValue1, TInt aValue2,
                                 TInt aValue3, TInt aValue4, CIniData* aUtils);
	IMPORT_C static void ReadIni(const TDesC& tempStore, const TPtrC& aIniParam,  const TPtrC& aIniValue1, const TPtrC& aIniValue2,
                                 const TPtrC& aIniValue3, const TPtrC& aIniValue4, TInt& aMemberVar,
                                 TInt aValue1, TInt aValue2, TInt aValue3,
                                 TInt aValue4, CIniData* aUtils);
	IMPORT_C static void ReadIni(const TPtrC& aIniParam, const TPtrC& aIniValue1, const TPtrC& aIniValue2, const TPtrC& aIniValue3,
                                 const TPtrC& aIniValue4, const TPtrC& aIniValue5, TInt& aMemberVar, TInt aValue1,
                                 TInt aValue2, TInt aValue3, TInt aValue4, TInt aValue5, CIniData* aUtils);
	IMPORT_C static void ReadIni(const TDesC& tempStore, const TPtrC& aIniParam,  const TPtrC& aIniValue1, const TPtrC& aIniValue2,
                                 const TPtrC& aIniValue3, const TPtrC& aIniValue4, const TPtrC& aIniValue5, TInt& aMemberVar,
                                 TInt aValue1, TInt aValue2, TInt aValue3, TInt aValue4,
                                 TInt aValue5, CIniData* aUtils);
	IMPORT_C static TInt Numeral(TInt val, TInt& aDigit);
	IMPORT_C static TInt ExtractNumber(const TPtrC& aRectTextString, TInt& aSearchFrom, TInt& aNumber);
	IMPORT_C static TInt ExtractRect(const TPtrC& aRectTextString, TRect& aRect, const TSize& aScreenSize);
	IMPORT_C static TInt ExtractAspectRatio(const TPtrC& aRatioString, TSize& aAspectRatio);
	IMPORT_C static TInt ExtractPoint(const TPtrC& aPointString, TPoint& aPoint);
	IMPORT_C static TInt ExtractSize(const TPtrC& aSizeString, TSize& aSize);
	IMPORT_C static void VerifyFramesPerTraverse(TInt& aFramesPerTraverse, TInt aScreenPixelDimension);
	static TInt ReadIniAllowMissing(const TPtrC& aIniParam, TPtrC& aMemberVar, CIniData* aUtils);
	static TBool GetShareSurfaceInfoL(TInt aScreenNo, TInt aWindowNo, TInt& aShareScreenNumber, TInt& aShareWindowNumber, CIniData* aUtils);

	};

#endif //__T_PSEUDOAPPUTILS_H__
