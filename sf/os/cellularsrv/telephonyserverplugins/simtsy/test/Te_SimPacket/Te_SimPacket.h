// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_SimPacket_H__
#define TE_SimPacket_H__

#include "Te_SimPacketTestStepBase.h"

//------------------------------------------------------------------

/**
 * This class substitutes function GprsDefaultContextParamsTestL() from T_SimPacket
 *
*/
class CGprsDefaultContextParamsTest : public CSimPacketTestStepBase 
	{
public:
	CGprsDefaultContextParamsTest() ;
	~CGprsDefaultContextParamsTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function QoSTestL() from T_SimPacket
 *
*/
class CQoSTest : public CSimPacketTestStepBase 
	{
public:
	CQoSTest() ;
	~CQoSTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsUnSupportedTestL() from T_SimPacket
 *
*/
class CGprsUnSupportedTest : public CSimPacketTestStepBase 
	{
public:
	CGprsUnSupportedTest() ;
	~CGprsUnSupportedTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsContextAttachTestL() from T_SimPacket
 *
*/
class CGprsContextAttachTest : public CSimPacketTestStepBase 
	{
public:
	CGprsContextAttachTest() ;
	~CGprsContextAttachTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsContextDeleteTestL() from T_SimPacket
 *
*/
class CGprsContextDeleteTest : public CSimPacketTestStepBase 
	{
public:
	CGprsContextDeleteTest() ;
	~CGprsContextDeleteTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsContextLoanComportL() from T_SimPacket
 *
*/
class CGprsContextLoanComport : public CSimPacketTestStepBase 
	{
public:
	CGprsContextLoanComport() ;
	~CGprsContextLoanComport(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsContextUnSupportedTestL() from T_SimPacket
 *
*/
class CGprsContextUnSupportedTest : public CSimPacketTestStepBase 
	{
public:
	CGprsContextUnSupportedTest() ;
	~CGprsContextUnSupportedTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * Tests dynamic capabilities APIs: RPacketService::GetDynamicCaps and RPacketService::NotifyDynamicCapsChange 
 */
class CTestDynamicCaps : public CSimPacketTestStepBase
	{
public:
	CTestDynamicCaps();
	~CTestDynamicCaps() {}
	virtual TVerdict doTestStepL();
	};

//------------------------------------------------------------------

/**
 * This class substitutes function GprsCancelTestL() from T_SimPacket
 *
*/
class CGprsCancelTest : public CSimPacketTestStepBase 
	{
public:
	CGprsCancelTest() ;
	~CGprsCancelTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsAttachTestL() from T_SimPacket
 *
*/
class CGprsAttachTest : public CSimPacketTestStepBase 
	{
public:
	CGprsAttachTest() ;
	~CGprsAttachTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsNotificationTestL() from T_SimPacket
 *
*/
class CGprsNotificationTest : public CSimPacketTestStepBase 
	{
public:
	CGprsNotificationTest() ;
	~CGprsNotificationTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsMsClassTestL() from T_SimPacket
 *
*/
class CGprsMsClassTest : public CSimPacketTestStepBase 
	{
public:
	CGprsMsClassTest() ;
	~CGprsMsClassTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsContextConfigTestL() from T_SimPacket
 *
*/
class CGprsContextConfigTest : public CSimPacketTestStepBase 
	{
public:
	CGprsContextConfigTest() ;
	~CGprsContextConfigTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function GprsContextConfigNotificationsTestL() from T_SimPacket
 *
*/
class CGprsContextConfigNotificationsTest : public CSimPacketTestStepBase 
	{
public:
	CGprsContextConfigNotificationsTest() ;
	~CGprsContextConfigNotificationsTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function OpenGprsMaxContextsTestL() from T_SimPacket
 *
*/
class COpenGprsMaxContextsTest : public CSimPacketTestStepBase 
	{
public:
	COpenGprsMaxContextsTest() ;
	~COpenGprsMaxContextsTest(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------

/**
 * This class test the Case when the Phone ask for an Arbitrary PdpAddress
 *
 */
class CGprsContextConfigArbitraryAddressTest : public CSimPacketTestStepBase 
	{
public:
	CGprsContextConfigArbitraryAddressTest() ;
	virtual TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function OpenNewContext() from RPacketMbmsContext
 *
 */
class CMbmsOpenNewContextTest : public CSimPacketTestStepBase 
	{
public:
	CMbmsOpenNewContextTest() ;
	~CMbmsOpenNewContextTest(){} ;
	virtual TVerdict doTestStepL( void );

private:
	RPacketMbmsContext iPacketMbmsContext;
	} ;

//------------------------------------------------------------------
/**
 * This class substitutes function UpdateSession() from RPacketMbmsContext
 *
 */
class CMbmsUpdateSessionTest : public CSimPacketTestStepBase 
	{
public:
	CMbmsUpdateSessionTest() ;
	~CMbmsUpdateSessionTest(){} ;
	virtual TVerdict doTestStepL( void );

private:
	RPacketMbmsContext iPacketMbmsContext;
	} ;


//------------------------------------------------------------------
/**
 * This class substitutes function GetMbmsNetworkServiceStatus() from RPacketService
 *
 */
class CGetMbmsNetworkServiceStatus : public CSimPacketTestStepBase 
	{
public:
	CGetMbmsNetworkServiceStatus() ;
	~CGetMbmsNetworkServiceStatus(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------
/**
 * This class substitutes function NotifyMbmsNetworkServiceStatusChange() from RPacketService
 *
 */
class CNotifyMbmsNetworkServiceStatusChange : public CSimPacketTestStepBase 
	{
public:
	CNotifyMbmsNetworkServiceStatusChange() ;
	~CNotifyMbmsNetworkServiceStatusChange(){} ;
	virtual TVerdict doTestStepL( void );

	} ;

//------------------------------------------------------------------
/**
 * This class substitutes function NotifyMbmsServiceAvailabilityChange() from RPacketService
 *
 */
class CNotifyMbmsServiceAvailabilityChange : public CSimPacketTestStepBase 
	{
public:
	CNotifyMbmsServiceAvailabilityChange() ;
	~CNotifyMbmsServiceAvailabilityChange(){} ;
	virtual TVerdict doTestStepL( void );

	} ;


//------------------------------------------------------------------
/**
 * This class substitutes function UpdateMbmsMonitorServiceList() from RPacketService
 *
 */
class CUpdateMbmsMonitorServiceList : public CSimPacketTestStepBase 
	{
public:
	CUpdateMbmsMonitorServiceList() ;
	~CUpdateMbmsMonitorServiceList(){} ;
	virtual TVerdict doTestStepL( void );

private:
	} ;

//------------------------------------------------------------------
/**
 * This class substitutes function EnumerateMbmsActiveServiceList() from RPacketService
 *
 */
class CEnumerateMbmsActiveServiceList : public CSimPacketTestStepBase 
	{
public:
	CEnumerateMbmsActiveServiceList() ;
	~CEnumerateMbmsActiveServiceList(){} ;
	virtual TVerdict doTestStepL( void );

	};

//------------------------------------------------------------------
/**
 * This class substitutes function EnumerateMbmsMonitorServiceList() from RPacketService
 *
 */
class CEnumerateMbmsMonitorServiceList : public CSimPacketTestStepBase 
	{
public:
	CEnumerateMbmsMonitorServiceList() ;
	~CEnumerateMbmsMonitorServiceList(){} ;
	virtual TVerdict doTestStepL( void );

	};


/**
 *  Tests closing one of two handles that both have notifications on them.
 *  This can cause ETel to close multiple requests during the same cancel
 *  request, which in turn can panic ETel if the iterator gets corrupted.
 */
class CTestMultipleCompletionWithClosingContext : public CSimPacketTestStepBase 
	{
public:
	virtual TVerdict doTestStepL();
	};

#endif// TE_SimPacket_H__
