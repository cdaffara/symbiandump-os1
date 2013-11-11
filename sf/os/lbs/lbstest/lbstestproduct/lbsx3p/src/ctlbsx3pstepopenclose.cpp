// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsx3pstepopenclose.cpp
// This is the class implementation for the Module Information Tests
// EPOC includes.
// 
//

// LBS includes. 
#include <lbs.h>
#include <lbs/lbsx3p.h>

// LBS test includes.
#include "ctlbsx3pstepopenclose.h"
#include <lbs/test/tlbsutils.h>

#define NUM_MAX_SERVER 10
#define NUM_MAX_SESSION 10

_LIT(Ksrvvermajor, "srv_ver_major");
_LIT(Ksrvverminor, "srv_ver_minor");
_LIT(Ksrvverbuild, "srv_ver_build");

/**
 * Destructor
 */
CT_LbsX3PStep_OpenClose::~CT_LbsX3PStep_OpenClose()
	{
	}


/**
 * Constructor
 */
CT_LbsX3PStep_OpenClose::CT_LbsX3PStep_OpenClose(CT_LbsX3PServer& aParent) : CT_LbsX3PStep(aParent)
	{
	SetTestStepName(KLbsX3PStep_OpenClose);
	}


/**
Static Constructor
*/
CT_LbsX3PStep_OpenClose* CT_LbsX3PStep_OpenClose::New(CT_LbsX3PServer& aParent)
	{
	
	CT_LbsX3PStep_OpenClose* testStep = new CT_LbsX3PStep_OpenClose(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;

	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
		
	return testStep;
	}

void CT_LbsX3PStep_OpenClose::ConstructL()
	{
		//Call the base class c'tor to create timer.
		CT_LbsX3PStep::ConstructL();
	}

 
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsX3PStep_OpenClose::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep_OpenClose::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
		TInt err = KErrNone;
		RLbsTransmitPositionServer server;
		RLbsTransmitPosition position;
		TInt i=0,j=0;
		
		// Carry out common test actions (such as connecting to a server).
		err = server.Connect();	// Function under test.

		if (KErrNone == err)
			{
			INFO_PRINTF1(_L("server opened"));

			// Carry out unique test actions.
			TInt testCaseId;
			if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
				{
				switch (testCaseId)
					{
					// Test case LBS-X3P-ConnectClose-0001
					case 01:
						{
						RLbsTransmitPositionServer arrServer[NUM_MAX_SERVER];
						for(i=0;i<NUM_MAX_SERVER;++i)
							{
							err = arrServer[i].Connect();
							if(KErrNone != err)
								{
								INFO_PRINTF2(_L("<FONT><B>Server %d Connect didnt return KErrNone</B></FONT>"), i);
								SetTestStepResult(EFail);	
								break;
								}
							}
						for(j=0; j<i;++j)
							arrServer[j].Close();
						}
						break;
						
					// Test case LBS-X3P-ConnectClose-0002
					case 02:
						{
						RLbsTransmitPositionServer arrServer[NUM_MAX_SERVER];
						for(i=0;i<NUM_MAX_SERVER;++i)
							{
							err = arrServer[i].Connect();
							if(KErrNone != err)
								{
								INFO_PRINTF2(_L("<FONT><B>Server %d Connect didnt return KErrNone</B></FONT>"), i);
								SetTestStepResult(EFail);	
								break;
								}
							arrServer[i].Close();
							}
						}
						break;

					// Test case LBS-X3P-ConnectClose-0003
					case 03:
						{
						err = server.Connect();
						if(KErrNone == err)
							{
							INFO_PRINTF2(_L("<FONT><B>Server connect should have returned error</B></FONT>"), i);
							SetTestStepResult(EFail);	
							}
						}
						break;

					// Test case LBS-X3P-OpenClose-0001
					case 11:
						{
						RLbsTransmitPosition arrPosition[NUM_MAX_SESSION];
						for(i=0;i<NUM_MAX_SESSION;++i)
							{
							err = arrPosition[i].Open(server);
							if(KErrNone != err)
								{
								INFO_PRINTF2(_L("<FONT><B>Server %d Open didnt return KErrNone</B></FONT>"), i);
								SetTestStepResult(EFail);	
								break;
								}
							}
						for(j=0; j<i;++j)
							arrPosition[j].Close();
						}
						break;
						
					// Test case LBS-X3P-OpenClose-0002
					case 12:
						{
						err = position.Open(server);
						if(KErrNone != err)
							{
							INFO_PRINTF1(_L("<FONT><B>RLbTransmitPosition Open() didnt return KErrNone</B></FONT>"));
							SetTestStepResult(EFail);	
							}
						else
							{
							err = position.Open(server);  //This test should panic here so no need to close in next line
							//position.Close();
							}
						}
						break;
						
					// Test case LBS-X3P-OpenClose-0003
					case 13:
						{
						err = position.Open(server);
						if(KErrNone == err)
							{
							//This Close() should cause panic
							server.Close();
							}
						}
						break;
						
					// Test case LBS-X3P-OpenClose-0004
					case 14:
						{
						RLbsTransmitPositionServer arrServer[NUM_MAX_SERVER];
						RLbsTransmitPosition arrPosition[NUM_MAX_SERVER][NUM_MAX_SESSION];
						TInt ii=0,jj,iNum[NUM_MAX_SERVER];  // = {0...0};
						TBool bFlag = EFalse;
						//iNum[0]=0;

						for(i=0;i<NUM_MAX_SERVER;++i)
							{
							if(bFlag) // == ETrue) 
								break;

							err = arrServer[i].Connect();
							if(KErrNone != err)
								{
								INFO_PRINTF2(_L("<FONT><B>RLbsTransmitPositionServer %d Connect didn't return KErrNone</B></FONT>"), i);
								SetTestStepResult(EFail);	
								break;
								}
							else
								{
								for(ii=0;ii<NUM_MAX_SESSION;++ii)
									{
									err = arrPosition[i][ii].Open(arrServer[i]);
									if(KErrNone != err)
										{
										INFO_PRINTF2(_L("<FONT><B>RLbsTransmitPosition %d Open didn't return KErrNone</B></FONT>"), i);
										SetTestStepResult(EFail);	
										bFlag = ETrue;
										break;
										}
									}
								iNum[i]=ii;
								}
							}
						for(j=0; j<i;++j)
							{
							for(jj=0; jj<iNum[j];++jj)
								arrPosition[j][jj].Close();

							arrServer[j].Close();
							}
						}
						break;
									
					// Test case LBS-X3P-OpenClose-0005
					case 15:
						{
						RLbsTransmitPositionServer tServer;
						err = position.Open(tServer);
						if(KErrNone == err)
							{
							INFO_PRINTF2(_L("<FONT><B>Session should not have opened</B></FONT>"), i);
							SetTestStepResult(EFail);	
							}
						}
						break;

					// Test case LBS-X3P-OpenClose-0006
					case 16:
						{
						position.Close();
						}
						break;

					// Test case LBS-X3P-Version-0001
					case 21:
						{
						TVersion serverversion;
						serverversion = server.Version();
						TInt majorVersionNumber = 0;
						TInt minorVersionNumber = 0;
						TInt buildVersionNumber = 0;
						if(!(GetIntFromConfig(ConfigSection(), Ksrvvermajor, majorVersionNumber) &&
						       GetIntFromConfig(ConfigSection(), Ksrvverminor, minorVersionNumber) &&
							GetIntFromConfig(ConfigSection(), Ksrvverbuild, buildVersionNumber)))
							{
							INFO_PRINTF1(_L("Bad ini file Build info."));
							User::Leave(KErrArgument);
							}
						
						//Check expected Version matches tested version
						if (majorVersionNumber == static_cast<TInt>(serverversion.iMajor) &&
							minorVersionNumber == static_cast<TInt>(serverversion.iMinor) &&
							buildVersionNumber == static_cast<TInt>(serverversion.iBuild))
							{
							INFO_PRINTF1(_L("Expected Version Matches"));
							}
						else
							{
							INFO_PRINTF1(_L("Expected Version Differs"));
							SetTestStepResult(EFail);
							}
						}
						break;

					default:
						{
						User::Panic(KLbsX3PStep_OpenClose, KErrUnknown);
						}					
					}
				}
			server.Close();
			}
		else
			{
			INFO_PRINTF1(_L("<FONT><B>Unable to open Server</B></FONT>"));
			SetTestStepResult(EFail);	
			}
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsX3PStep_OpenClose::doTestStepL()"));

	return TestStepResult();
	}

