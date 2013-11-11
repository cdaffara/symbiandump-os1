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
// Definition of dvbhtypes.h inline functions.
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef DVBHTYPES_INL
#define DVBHTYPES_INL

/**
* @publishedPartner
* @prototype
*
* C++ constructor.
*/
inline TDvbhNetwork::TDvbhNetwork() :
    iId( 0 )
{
};

/**
* @publishedPartner
* @prototype
*
* C++ constructor.
*/
inline TDvbhPlatform::TDvbhPlatform() :
    iId( 0 )
{
};

/**
* @publishedPartner
* @prototype
*
* C++ constructor.
*/
inline TDvbhScanResult::TDvbhScanResult() :
    iFrequency( 0 ), iSignalStatus ( ENoSignalFound ), iProgress( 0 )
{
};

/**
* @publishedPartner
* @prototype
*
* C++ constructor.
*/
inline TDvbhPerformanceData::TDvbhPerformanceData() :
    iFilterCount( 0 ), iMaxFilters( 0 ), iDataCount( 0 ),
    iWriteCount( 0 ), iWriteCommandCount( 0 ),
    iReadCount( 0 ), iReadCommandCount( 0 )
{
};

/**
* @publishedPartner
* @prototype
*
* C++ constructor.
*/
inline TDvbhNetworkTime::TDvbhNetworkTime() :
    iNetworkTime( 0 ), iTerminalTimeStamp( 0 ), iPlatformId( 0 )
{
};

/**
* @publishedPartner
* @prototype
*
* C++ constructor.
*/
inline TDvbhScanConfiguration::TDvbhScanConfiguration() :
    iScanStartFrequency( 0 ), iScanEndFrequency( 0 ), iSignalBandwidth( 0 ),
    iScanOptions( EDvbhScanOptionNormal )
{
};
	
#endif //DVBHTYPES_INL