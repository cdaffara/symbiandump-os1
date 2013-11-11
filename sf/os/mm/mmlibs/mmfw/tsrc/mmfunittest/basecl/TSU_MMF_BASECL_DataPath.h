// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_BASECL_DATAPATH_H__
#define __TSU_MMF_BASECL_DATAPATH_H__

#include <mmf/server/mmfdatapath.h>
#include <mmf/server/mmfdatapathproxy.h>
#include "TSU_MMF_BASECL_TestSrcSink.h"
#include "TSU_MMF_BASECL_TestFormatImplementationUID.hrh"
#include "TSU_MMF_BASECL_TestCodecImplementationUID.hrh"
#include <mmf/plugin/mmfplugininterfaceuids.hrh>
#include "TSU_MMF_BASECL.h"

#include "ToneGenerator.h" // used for testing

const TUint KMMFBASECLTestMediaId	=	0x101F7B21;
const TUint KMMFPauseAfterDelay		=	400000;
const TUint KMMFStartOfPlayWindow	=	100000;
const TUint KMMFEndOfPlayWindow		=	300000;
const TUint KMMFTestDataPathPosition=	300000;
const TUint KMMFDefaultSrcSinkFrameSize = 16;
const TUint KMMFNumFramesPlayWindowTest = 6;


/**
 * Test asynchronous event handler class to receive events from
 * the dataPath
 * If the event is from the datapath the active scheduler is stopped
 */
class CTestAsyncEventHandler : public CBase , public MAsyncEventHandler
	{
public:
	CTestAsyncEventHandler() {};
	~CTestAsyncEventHandler() {};

	//MasyncEventHandler
	virtual TInt SendEventToClient(const TMMFEvent& aEvent);

	TMMFEvent& EventReceived();
private:
	CActiveScheduler* iActiveScheduler;
	TMMFEvent iEvent;
	};

/**
 * Test asynchronous event handler class to receive events from
 * the dataPath proxy
 * The active scheduler is stopped when an event is received
 */
class CTestDataPathEventMonitorObserver : public CBase , public MMMFDataPathEventMonitorObserver
	{
public:
	CTestDataPathEventMonitorObserver() {};
	~CTestDataPathEventMonitorObserver() {};

	//MMMFDataPathEventMonitorObserver
	virtual void HandleEvent(const TMMFEvent& aEvent);

	TMMFEvent& EventReceived();
private:
	CActiveScheduler* iActiveScheduler;
	TMMFEvent iEvent;
	};


/**
 * Utiltiy class used by the test harness to make asynchronous client
 * calls ie Pause() and Stop() on the datapath & datapath proxy
 * from the test harness
 */
class CMMF_TSU_BASECL_MakeAsyncDataPathCall : public CTimer
	{
public:
	CMMF_TSU_BASECL_MakeAsyncDataPathCall();
	~CMMF_TSU_BASECL_MakeAsyncDataPathCall();
	void CallPauseAfter(CMMFDataPath* aDataPath, TTimeIntervalMicroSeconds32 aTimeInterval);
	void CallStopAfter(CMMFDataPath* aDataPath, TTimeIntervalMicroSeconds32 aTimeInterval);
	void CallPauseAfter(RMMFDataPathProxy& aDataPathProxy, TTimeIntervalMicroSeconds32 aTimeInterval);
	void CallStopAfter(RMMFDataPathProxy& aDataPathProxy, TTimeIntervalMicroSeconds32 aTimeInterval);
private:
	virtual void RunL();
private:
	CMMFDataPath* iDataPath;
	RMMFDataPathProxy iDataPathProxy;
	TBool iCallPause;
	TBool iCallStop;
	TBool iCallPauseProxy;
	TBool iCallStopProxy;
	};

/**
 * Test step base class for datapath & datapath proxy tests
 * Creates an active scheduler, a datapath async event handler and ensures the 
 * codec constructor test file is deleted
 */
class CTestStep_MMF_BASECL_DataPath : public CTestStep_MMF_BASECL
	{
public:
	CTestStep_MMF_BASECL_DataPath();
	~CTestStep_MMF_BASECL_DataPath();
protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
protected:
	TUid iTestCodecUid;
	TUid iTestDataSourceUid;
	TUid iTestDataSinkUid;
	CActiveScheduler* iActiveScheduler;
	CTestAsyncEventHandler* iTestAsyncEventHandler;
	};

/**
 * Test step base class for datapath & datapath proxy play tests
 * Puts the data path in playing mode and continues playing until
 * an event is received from the datapath
 */
class CTestStep_MMF_BASECL_DataPathPlay : public CTestStep_MMF_BASECL_DataPath
	{
public:
	CTestStep_MMF_BASECL_DataPathPlay();
	~CTestStep_MMF_BASECL_DataPathPlay();
protected:
	virtual TVerdict DoTestDataPathPlayL(TMMF_BASECL_TestSrcSinkConfig aSrcConfig, 
							TMMF_BASECL_TestSrcSinkConfig aSinkConfig,
							TBool aUseNullCodec = EFalse);
	virtual TVerdict DoTestDataPathProxyPlayL(TMMF_BASECL_TestSrcSinkConfig aSrcConfig, 
							TMMF_BASECL_TestSrcSinkConfig aSinkConfig,
							TBool aUseNullCodec = EFalse);
	};



class CTestStep_MMF_BASECL_U_0200: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0200() ;
 ~CTestStep_MMF_BASECL_U_0200(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0201: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0201() ;
 ~CTestStep_MMF_BASECL_U_0201(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0202: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0202() ;
 ~CTestStep_MMF_BASECL_U_0202(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0203: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0203() ;
 ~CTestStep_MMF_BASECL_U_0203(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0204: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0204() ;
 ~CTestStep_MMF_BASECL_U_0204(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0205: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0205() ;
 ~CTestStep_MMF_BASECL_U_0205(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0206: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0206() ;
 ~CTestStep_MMF_BASECL_U_0206(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0207: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0207() ;
 ~CTestStep_MMF_BASECL_U_0207(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0208: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0208() ;
 ~CTestStep_MMF_BASECL_U_0208(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0209: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0209() ;
 ~CTestStep_MMF_BASECL_U_0209(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0210: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0210() ;
 ~CTestStep_MMF_BASECL_U_0210(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0211: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0211() ;
 ~CTestStep_MMF_BASECL_U_0211(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0212: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0212() ;
 ~CTestStep_MMF_BASECL_U_0212(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0213: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0213() ;
 ~CTestStep_MMF_BASECL_U_0213(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0214: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0214() ;
 ~CTestStep_MMF_BASECL_U_0214(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0215: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0215() ;
 ~CTestStep_MMF_BASECL_U_0215(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0216: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0216() ;
 ~CTestStep_MMF_BASECL_U_0216(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0217: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0217() ;
 ~CTestStep_MMF_BASECL_U_0217(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0218: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0218() ;
 ~CTestStep_MMF_BASECL_U_0218(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0219: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0219() ;
 ~CTestStep_MMF_BASECL_U_0219(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0220: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0220() ;
 ~CTestStep_MMF_BASECL_U_0220(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0221: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0221() ;
 ~CTestStep_MMF_BASECL_U_0221(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0222: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0222() ;
 ~CTestStep_MMF_BASECL_U_0222(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0223: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0223() ;
 ~CTestStep_MMF_BASECL_U_0223(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0224: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0224() ;
 ~CTestStep_MMF_BASECL_U_0224(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0225: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0225() ;
 ~CTestStep_MMF_BASECL_U_0225(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0226: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0226() ;
 ~CTestStep_MMF_BASECL_U_0226(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0227: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0227() ;
 ~CTestStep_MMF_BASECL_U_0227(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0228: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0228() ;
 ~CTestStep_MMF_BASECL_U_0228(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0229: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0229() ;
 ~CTestStep_MMF_BASECL_U_0229(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0230: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0230() ;
 ~CTestStep_MMF_BASECL_U_0230(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0231: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0231() ;
 ~CTestStep_MMF_BASECL_U_0231(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0232: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0232() ;
 ~CTestStep_MMF_BASECL_U_0232(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0233: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0233() ;
 ~CTestStep_MMF_BASECL_U_0233(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0234: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0234() ;
 ~CTestStep_MMF_BASECL_U_0234(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0235: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0235() ;
 ~CTestStep_MMF_BASECL_U_0235(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0236: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0236() ;
 ~CTestStep_MMF_BASECL_U_0236(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0237: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0237() ;
 ~CTestStep_MMF_BASECL_U_0237(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0238: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0238() ;
 ~CTestStep_MMF_BASECL_U_0238(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0239: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0239() ;
 ~CTestStep_MMF_BASECL_U_0239(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0240: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0240() ;
 ~CTestStep_MMF_BASECL_U_0240(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0241: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0241() ;
 ~CTestStep_MMF_BASECL_U_0241(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0242: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0242() ;
 ~CTestStep_MMF_BASECL_U_0242(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0243: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0243() ;
 ~CTestStep_MMF_BASECL_U_0243(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0244: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0244() ;
 ~CTestStep_MMF_BASECL_U_0244(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0245: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0245() ;
 ~CTestStep_MMF_BASECL_U_0245(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0246: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0246() ;
 ~CTestStep_MMF_BASECL_U_0246(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0260: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0260() ;
 ~CTestStep_MMF_BASECL_U_0260(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0261: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0261() ;
 ~CTestStep_MMF_BASECL_U_0261(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0262: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0262() ;
 ~CTestStep_MMF_BASECL_U_0262(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0263: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0263() ;
 ~CTestStep_MMF_BASECL_U_0263(){} ;
 virtual TVerdict DoTestStepL();
 } ;
 
class CTestStep_MMF_BASECL_U_0264: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0264() ;
 ~CTestStep_MMF_BASECL_U_0264(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0265: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0265() ;
 ~CTestStep_MMF_BASECL_U_0265(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0266: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0266() ;
 ~CTestStep_MMF_BASECL_U_0266(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0267: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0267() ;
 ~CTestStep_MMF_BASECL_U_0267(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0280: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0280() ;
 ~CTestStep_MMF_BASECL_U_0280(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0281: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0281() ;
 ~CTestStep_MMF_BASECL_U_0281(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0282: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0282() ;
 ~CTestStep_MMF_BASECL_U_0282(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0300: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0300() ;
 ~CTestStep_MMF_BASECL_U_0300(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0301: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0301() ;
 ~CTestStep_MMF_BASECL_U_0301(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0302: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0302() ;
 ~CTestStep_MMF_BASECL_U_0302(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0303: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0303() ;
 ~CTestStep_MMF_BASECL_U_0303(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0304: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0304() ;
 ~CTestStep_MMF_BASECL_U_0304(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0305: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0305() ;
 ~CTestStep_MMF_BASECL_U_0305(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0306: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0306() ;
 ~CTestStep_MMF_BASECL_U_0306(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0307: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0307() ;
 ~CTestStep_MMF_BASECL_U_0307(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0308: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0308() ;
 ~CTestStep_MMF_BASECL_U_0308(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0309: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0309() ;
 ~CTestStep_MMF_BASECL_U_0309(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0310: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0310() ;
 ~CTestStep_MMF_BASECL_U_0310(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0311: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0311() ;
 ~CTestStep_MMF_BASECL_U_0311(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0312: public CTestStep_MMF_BASECL_DataPath
 {
public:
 CTestStep_MMF_BASECL_U_0312() ;
 ~CTestStep_MMF_BASECL_U_0312(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0313: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0313() ;
 ~CTestStep_MMF_BASECL_U_0313(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0314: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0314() ;
 ~CTestStep_MMF_BASECL_U_0314(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0315: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0315() ;
 ~CTestStep_MMF_BASECL_U_0315(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0316: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0316() ;
 ~CTestStep_MMF_BASECL_U_0316(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0317: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0317() ;
 ~CTestStep_MMF_BASECL_U_0317(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0318: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0318() ;
 ~CTestStep_MMF_BASECL_U_0318(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0319: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0319() ;
 ~CTestStep_MMF_BASECL_U_0319(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0320: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0320() ;
 ~CTestStep_MMF_BASECL_U_0320(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0321: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0321() ;
 ~CTestStep_MMF_BASECL_U_0321(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0322: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0322() ;
 ~CTestStep_MMF_BASECL_U_0322(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0323: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0323() ;
 ~CTestStep_MMF_BASECL_U_0323(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0324: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0324() ;
 ~CTestStep_MMF_BASECL_U_0324(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0325: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0325() ;
 ~CTestStep_MMF_BASECL_U_0325(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0326: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0326() ;
 ~CTestStep_MMF_BASECL_U_0326(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0327: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0327() ;
 ~CTestStep_MMF_BASECL_U_0327(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0328: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0328() ;
 ~CTestStep_MMF_BASECL_U_0328(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0329: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0329() ;
 ~CTestStep_MMF_BASECL_U_0329(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0330: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0330() ;
 ~CTestStep_MMF_BASECL_U_0330(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0331: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0331() ;
 ~CTestStep_MMF_BASECL_U_0331(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0332: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0332() ;
 ~CTestStep_MMF_BASECL_U_0332(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0333: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0333() ;
 ~CTestStep_MMF_BASECL_U_0333(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0334: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0334() ;
 ~CTestStep_MMF_BASECL_U_0334(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0335: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0335() ;
 ~CTestStep_MMF_BASECL_U_0335(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0336: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0336() ;
 ~CTestStep_MMF_BASECL_U_0336(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0337: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0337() ;
 ~CTestStep_MMF_BASECL_U_0337(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0338: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0338() ;
 ~CTestStep_MMF_BASECL_U_0338(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0339: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0339() ;
 ~CTestStep_MMF_BASECL_U_0339(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0340: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0340() ;
 ~CTestStep_MMF_BASECL_U_0340(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0341: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0341() ;
 ~CTestStep_MMF_BASECL_U_0341(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0342: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0342() ;
 ~CTestStep_MMF_BASECL_U_0342(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0343: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0343() ;
 ~CTestStep_MMF_BASECL_U_0343(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0344: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0344() ;
 ~CTestStep_MMF_BASECL_U_0344(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0345: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0345() ;
 ~CTestStep_MMF_BASECL_U_0345(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0360: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0360() ;
 ~CTestStep_MMF_BASECL_U_0360(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0361: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0361() ;
 ~CTestStep_MMF_BASECL_U_0361(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0362: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0362() ;
 ~CTestStep_MMF_BASECL_U_0362(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0363: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0363() ;
 ~CTestStep_MMF_BASECL_U_0363(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0364: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0364() ;
 ~CTestStep_MMF_BASECL_U_0364(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0365: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0365() ;
 ~CTestStep_MMF_BASECL_U_0365(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0366: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0366() ;
 ~CTestStep_MMF_BASECL_U_0366(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0367: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0367() ;
 ~CTestStep_MMF_BASECL_U_0367(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0380: public CTestStep_MMF_BASECL_DataPathPlay
 {
public:
 CTestStep_MMF_BASECL_U_0380() ;
 ~CTestStep_MMF_BASECL_U_0380(){} ;
 virtual TVerdict DoTestStepL();
 } ;
 
class CMMFChannelAndSampleRateConverter; //forward
class CMMFChannelAndSampleRateConverterFactory;
 
const TInt KNumBuffers = 4;
const TInt KBasicBufferSize = 4096;
class RTestStep_RateConvertBase : public RAsyncTestStep
	// abstract base class for rate conversion tests
	{
protected:
	RTestStep_RateConvertBase();
	
	TInt CompareFilesL(const TDesC& aReference, const TDesC& aGenerated);
	void SetupParams();
	
	// from RAsyncTestStep
	void CloseTest(); 
	
	RFs iFs;
	
	TInt iFromRate;
	TInt iToRate;
	TInt iFromChannels;
	TInt iToChannels;
	TInt iExpectedDestinationBufferSize;
	TFileName iFileToCompare;
	TMdaSimpleToneGenerator		iToneGen;
	CMMFDataBuffer*				iInputBuffer[KNumBuffers];
	CMMFDataBuffer*				iOutputBuffer[KNumBuffers];
	};

// Tests 10xx test the old MMF data-path (public) converter API
class RTestStep_MMF_BASECL_U_1000 : public RTestStep_RateConvertBase
	{
public:
	static RTestStep_MMF_BASECL_U_1000* NewL();
	
private:
	RTestStep_MMF_BASECL_U_1000();
	
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	
	CMMFChannelAndSampleRateConverter* iConverter; // not owned
	CMMFChannelAndSampleRateConverterFactory* iFactory; 
	};


// Tests 11xx test the internal audio utils unit. Otherwise same test(s) as 10xx
class CChannelAndSampleRateConverter;
class RTestStep_MMF_BASECL_U_1100 : public RTestStep_RateConvertBase
	{
public:
	static RTestStep_MMF_BASECL_U_1100* NewL();
	
private:
	RTestStep_MMF_BASECL_U_1100();
	
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	
	CChannelAndSampleRateConverter* iConverter;
	};


// Tests 12xx test the internal audio utils unit using short output buffers.
class CChannelAndSampleRateConverter;
class RTestStep_MMF_BASECL_U_1200 : public RTestStep_RateConvertBase
	{
public:
	static RTestStep_MMF_BASECL_U_1200* NewL();
	
private:
	RTestStep_MMF_BASECL_U_1200();
	
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

private:	
	CChannelAndSampleRateConverter* iConverter;
	TInt iOutputBufferSize;
	RBuf8 iOutpBuffer;
	};


#endif /* __TSU_MMF_BASECL_DATAPATH_H__ */
