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

#ifndef __TSU_MMF_DRMPLUGIN_H__
#define __TSU_MMF_DRMPLUGIN_H__

#include <testframework.h>
#include "TSU_MMF_DRMPluginSuite_U.h"
#include "MmfDrmPluginServer.h"

class CTestSuite;
class CTestSuite_MMF_DRMPlugin;

/**
 *
 * CTestStep_MMF_DRMPlugin, which is the base class for all 
 * the SelfTest suite test steps.
 *
 * @lib "TSU_MMF_DRMPlugin_U.lib"
 *
 * @xxxx
 *
 */
class CTestStep_MMF_DRMPlugin : public CTestStep
	{
public:
	CTestStep_MMF_DRMPlugin();
	~CTestStep_MMF_DRMPlugin();
	static void CleanUp(TAny *aPtr) ;
	void SetTestSuite(const CTestSuite_MMF_DRMPlugin* aTestSuite )
  { iMMFDRMPluginTestSuite = aTestSuite;}  

	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
protected:
	// pointer to suite which owns this test 
	const CTestSuite_MMF_DRMPlugin * iMMFDRMPluginTestSuite;
	};

/**
 * @class CTestStep_MMF_DRMPlugin_U_0001
 *Create a session with DRM Plugin Server. Test the method Open().
 *
 *@test 
 */
class CTestStep_MMF_DRMPlugin_U_0001 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0001() ;
	~CTestStep_MMF_DRMPlugin_U_0001(){} ;
	virtual TVerdict DoTestStepL( void );
 	} ;


/**
 * @class CTestStep_MMF_DRMPlugin_U_0002
 *Create 2 different sessions with DRM Plugin Server. 
 *
 *@test 
 */
class CTestStep_MMF_DRMPlugin_U_0002 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0002() ;
	~CTestStep_MMF_DRMPlugin_U_0002(){} ;
	virtual TVerdict DoTestStepL( void );
	} ;

/**
 * @class CTestStep_MMF_DRMPlugin_U_0003
 *Launch a MMF Controller server through DRM Plugin server
 *
 *@test 
 */
class CTestStep_MMF_DRMPlugin_U_0003 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0003() ;
	~CTestStep_MMF_DRMPlugin_U_0003(){} ;
	virtual TVerdict DoTestStepL( void );
	} ;
 
 
 /**
 * @class CTestStep_MMF_DRMPlugin_U_0004
 *Launch 2 different MMF Controller servers through DRM Plugin server
 *
 *@test 
 */
class CTestStep_MMF_DRMPlugin_U_0004 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0004() ;
	~CTestStep_MMF_DRMPlugin_U_0004(){} ;
	virtual TVerdict DoTestStepL( void );
	} ;
 
  /**
 * @class CTestStep_MMF_DRMPlugin_U_0005
 *Verify the returning Controller Server handle
 *
 *@test 
 */
class CTestStep_MMF_DRMPlugin_U_0005 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0005() ;
	~CTestStep_MMF_DRMPlugin_U_0005(){} ;
	virtual TVerdict DoTestStepL( void );
	} ;
 
  /**
 * @class CTestStep_MMF_DRMPlugin_U_0006
 *DRMPlugin Server is kept alive for DRM Plugin Server shut-down delay(5sec)
 *after the last controller server session has been closed.
 *
 *@test 
 */
class CTestStep_MMF_DRMPlugin_U_0006 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0006() ;
	~CTestStep_MMF_DRMPlugin_U_0006(){} ;
	virtual TVerdict DoTestStepL( void );
	} ;
 
  /**
 * @class CTestStep_MMF_DRMPlugin_U_0007
 *DRMPlugin Server is terminated 5sec (DRM Plugin Server shut-down delay) 
 *after the last controller server session has been closed.
 *
 *@test 
 */
class CTestStep_MMF_DRMPlugin_U_0007 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0007() ;
	~CTestStep_MMF_DRMPlugin_U_0007(){} ;
	virtual TVerdict DoTestStepL( void );
	} ;
	
  /**
 * @class CTestStep_MMF_DRMPlugin_U_0008
 *Session handle created by DRMPlugin Server is transferrable to client.  
 *The server is kept alive even if the session with DRMPlugin Server is closed, 
 *while session created by client has not been closed.
 *
 *@test 
 */
class CTestStep_MMF_DRMPlugin_U_0008 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0008() ;
	~CTestStep_MMF_DRMPlugin_U_0008() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0009
*Session handle created by DRMPlugin Server is transferrable to client.  
*However, DRMPluginServer is terminated 5sec after the last controller session 
*handle has been closed (in this test, DRMPlugin Server session is closed before transferred session).
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0009 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0009() ;
	~CTestStep_MMF_DRMPlugin_U_0009() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_ImproperClosing
*Close a Controller Server's thread through Kill() or Panic()
*
*@test 
*/
enum TImproperClosingType
	{
	ECloseByPanic,
	ECloseByKill
	};
		
class CTestStep_MMF_DRMPlugin_U_ImproperClosing : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_ImproperClosing(const TDesC& aTestName,
											TImproperClosingType aClosingType,
											TBool aWithinShutdownDelay) ;
	CTestStep_MMF_DRMPlugin_U_ImproperClosing() {} ;
	virtual TVerdict DoTestStepL( void );

private:
	TImproperClosingType iClosingType;
	TBool iWithinShutdownDelay;
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0014
*Reset a controller's thread prioity and confirm that
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0014 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0014() ;
	~CTestStep_MMF_DRMPlugin_U_0014() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0015
*Open a content through DRM Plugin Server API (file path)
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0015 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0015() ;
	~CTestStep_MMF_DRMPlugin_U_0015() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0016
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0016 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0016() ;
	~CTestStep_MMF_DRMPlugin_U_0016() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0017
*Open a content through DRM Plugin Server API (file path) and check the content info
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0017 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0017() ;
	~CTestStep_MMF_DRMPlugin_U_0017() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0018
*Create, operate, and delete CMMFDRMPluginServer and CStartAndMonitorControllerThread. 
*Make sure the CMMFDRMPluginServer and CStartAndMonitorControllerThread is cleanup properly.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0018 : 	public CTestStep_MMF_DRMPlugin,
										public MFSBListener
	{
public:	// data
	enum
		{
		EStartServer = 0,
		EInitControllerThread,
		ECancelThreadMonitor,
		EDeleteServer,
		EStopScheduler
		};

public:
	CTestStep_MMF_DRMPlugin_U_0018() ;
	~CTestStep_MMF_DRMPlugin_U_0018();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepL( void );
	virtual TVerdict DoTestStepPostambleL();
	
	void StateActivate(TInt aState);

private:
	void DoHandleStateResult(TInt aErr);
	void DoStartServerL();
	void DoInitControllerThreadL();
	void DoCancelThreadMonitor();
	void DoDeleteServer();
private:
	CActiveScheduler* iActiveScheduler;
	CFSMNotifier* iFsmNotifier;
	
	CMMFDRMPluginServer* iServer;
	CStartAndMonitorControllerThread* iThreadMonitor;
	TBool iHandleOpened;
	RMMFControllerServerProxy iControllerServer;
	TThreadId	iControllerThreadID;
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_InvalidHeapSize
*Launch a MMF Controller with invalid heap size.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_InvalidHeapSize : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_InvalidHeapSize(const TDesC& aTestName,
											const TDesC& aSectName,
											const TDesC& aKeyName) ;
	~CTestStep_MMF_DRMPlugin_U_InvalidHeapSize() {} ;
	virtual TVerdict DoTestStepL( void );

private:
	TInt iTestMaxHeapSize;
	TBuf<25>	iSectName;
	TBuf<25>	iKeyName;
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0022
*Lauch another MMF Controller through a DRMPlugin Server session which has a controller already launched.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0022 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0022() ;
	~CTestStep_MMF_DRMPlugin_U_0022() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0023
*Get controller server session handle through a DRM Plugin session without Controller server launched.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0023 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0023() ;
	~CTestStep_MMF_DRMPlugin_U_0023() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread
*Close an invalid Controller Server's thread through Kill() or Panic()
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread(const TDesC& aTestName,
											TImproperClosingType aClosingType) ;
	CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread() {} ;
	virtual TVerdict DoTestStepL( void );
	
private:
	TImproperClosingType iClosingType;
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0026
*Reset an invalid controller thread prioity through DRM Plugin Server 
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0026 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0026() ;
	~CTestStep_MMF_DRMPlugin_U_0026() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0027
*EvaluateDataContentIntent() returns error message KErrGeneral if no content
*has been open in DRM Plugin Server.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0027 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0027() ;
	~CTestStep_MMF_DRMPlugin_U_0027() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0028
*GetDataContentMimeTypeL() leaves with error message KErrGeneral if no content
*has been open in DRM Plugin Server.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0028 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0028() ;
	~CTestStep_MMF_DRMPlugin_U_0028() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0029
*GetDataContentFileHeaderL() leaves with error message KErrGeneral if no content
*has been open in DRM Plugin Server.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0029 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0029() ;
	~CTestStep_MMF_DRMPlugin_U_0029() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0030
*OpenDataContentL() leaves with error message KErrAlreadyExists if a content
*has already been created in a server session.
*has been open in DRM Plugin Server.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0030 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0030() ;
	~CTestStep_MMF_DRMPlugin_U_0030() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0031
*OpenDataContentL() leaves with error message KErrAlreadyExists if a content
*has already been created in a server session.
*has been open in DRM Plugin Server.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0031 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0031() ;
	~CTestStep_MMF_DRMPlugin_U_0031() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0032
*OpenDataContentL() leaves with error message KErrEof if NULL initdata is supplied.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0032 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0032() ;
	~CTestStep_MMF_DRMPlugin_U_0032() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0033
*GetDataContentFileHeaderL() will leave with KErrOverflow if maximum size to fetch
*is greater than the header buffer maximum length.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0033 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0033() ;
	~CTestStep_MMF_DRMPlugin_U_0033() {} ;
	virtual TVerdict DoTestStepL( void );
	};

/**
* @class CTestStep_MMF_DRMPlugin_U_0040
*Alloc failure test of Open() function.
*
*@test 
*/
class CTestStep_MMF_DRMPlugin_U_0040 : public CTestStep_MMF_DRMPlugin
	{
public:
	CTestStep_MMF_DRMPlugin_U_0040() ;
	~CTestStep_MMF_DRMPlugin_U_0040() {} ;
	virtual TVerdict DoTestStepL( void );
	};
 
/**
* @RMMFControllerServerTestProxy
*@Open a session to a server with a given client session handle
*/
class RMMFControllerServerTestProxy : public RMmfSessionBase
	{
public:
	TInt Open(TInt aControllerSessionHandle);
	};
 
/**
* @Utility class
*@Contain helper functions
*/
class Utility
	{
public:
	//@param aContent - The content file which will be generated
	//@param aContentRight - A file contains content and right with respect to specific
	//						 RTA format schema
	static void ImportRightL(const TDesC& aContent, const TDesC& aContentRight);
	};
#endif

