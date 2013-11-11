// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// utility fucntions etc for various bits of the location server
// 
//

/**
 @file
 @InternalComponent
*/

#ifndef __LBSNMEAFUNCTIONS_H__
#define __LBSNMEAFUNCTIONS_H__

#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbs/lbsextendedsatellite.h>

void CreateGga(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom);
void CreateGll(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom);
void CreateGsa(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom);
void CreateGst(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom);
void CreateGsv(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom, TInt& aTotalSentences, TInt& aLastSentence);
void CreateRmc(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom);
void CreateVtg(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom);
void CreateUtcTimeBuffer(TDes8& aBuffer, const TPosition& aPos);
void CreateLatitudeBuffer(TDes8& aBuffer, const TPosition& aPos);
void CreateLongitudeBuffer(TDes8& aBuffer, const TPosition& aPos);
void CreateDoPBuffer(TDes8& aBuffer, TReal32 aDop, TBool aValidFix);
void CreateSpeedBuffer(TDes8& aBuffer, const TCourse& aCourse);
void CreateHeadingBuffer(TDes8& aBuffer, const TCourse& aCourse);


#endif // __LBSNMEAFUNCTIONS_H__
