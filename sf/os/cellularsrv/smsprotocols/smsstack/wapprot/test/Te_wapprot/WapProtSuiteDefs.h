/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
    @file
    @test
*/

#ifndef WAPPROTSUITEDEFS_H
#define WAPPROTSUITEDEFS_H

// Keys in the configuration ini file
_LIT(KSCNumber,"Service_Center");
_LIT(KWapPort,"Wap_Port");
_LIT(KWapPort2,"Wap_Port2");
_LIT(KTestData1,"Test_Data1");
_LIT(KTestData2,"Test_Data2");

// For test step panics
_LIT(KWapProtSuitePanic,"WapProtSuite");

enum TDemoSuitePanicCodes{ETCPDataCorrupt = 1,EUDPDataCorrupt};

// Port for the echo server
// Could be read from the ini file
const TInt KEchoPort = 7;

//////////////////////////////////////////////////////////////////////////////
// Wap Address / Port Settings
//////////////////////////////////////////////////////////////////////////////
/*
enum TWapPortNumber
	{
	EWapPortUnspecified = -1,  // Unspecified
	EWapPortWsp         = 9200,  // Connectionless session protocol
	EWapPortWspWtp      = 9201,  // Connection oriented session protocol
	EWapPortWspWtls     = 9202,  // Secure connectionless session protocol
	EWapPortWspWtpWtls  = 9203,  // Secure connection oriented session protocol
	EWapPortVCard       = 9204,  // vCard
	EWapPortVCardWtls   = 9206,  // Secure vCard
	EWapPortVCal        = 9205,  // vCal
	EWapPortVCalWtls    = 9207   // Secure vCal
	};
*/

#endif // WAPPROTSUITEDEFS_H
