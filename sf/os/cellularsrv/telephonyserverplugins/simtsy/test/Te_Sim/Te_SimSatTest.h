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
//

#ifndef __TE_SIM_SAT_TEST__
#define __TE_SIM_SAT_TEST__

#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimSatTest : public CSimTestsTestStepBase
	{
public:
	CSimSatTest();
 	virtual TVerdict doTestStepL();

private:
	RSat iSat;
	};


class CSimSatRetMMPCmdTest : public CSimTestsTestStepBase
	{
public:
	CSimSatRetMMPCmdTest();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8501L();
	TInt testSAT8502L();
	TInt testSAT8503L();
	TInt testSAT8504L();
	TInt testSAT8505L();
	TInt testSAT8506L();
	TInt testSAT8507L();
	TInt testSAT8508L();
	
	RSat iSat;
		
	};

class CSimSatSubMMPCmdTest	: public CSimTestsTestStepBase
	{
public:
	CSimSatSubMMPCmdTest();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8509L();
	TInt testSAT8510L();
	TInt testSAT8511L();
	TInt testSAT8512L();
	TInt testSAT8513L();
	TInt testSAT8514L();
	TInt testSAT8515L();
	TInt testSAT8516L();
	
	RSat iSat;
	
	};

class CSimSatDispMMPCmdTest : public CSimTestsTestStepBase
	{
		
public:
	CSimSatDispMMPCmdTest();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8516L();
	TInt testSAT8517L();
	TInt testSAT8518L();
	TInt testSAT8519L();
	TInt testSAT8520L();
	TInt testSAT8521L();
	TInt testSAT8522L();
	TInt testSAT8523L();
	TInt testSAT8524L();
	TInt testSAT8525L();
	TInt testSAT8526L();
	
	RSat iSat;

	};

class CSimSatSetFrmsPCmdTest : public CSimTestsTestStepBase
	{

public:
	CSimSatSetFrmsPCmdTest();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8527L();
	TInt testSAT8528L();
	TInt testSAT8529L();
	TInt testSAT8530L();
	
	RSat iSat;
	};

class CSimSatGetFrmsPCmdTest : public CSimTestsTestStepBase
	{
public:
	CSimSatGetFrmsPCmdTest();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8531L();
	TInt testSAT8532L();
	TInt testSAT8533L();
	
	RSat iSat;

	};
	
class CSimSatMmsNotificationDownload :  public CSimTestsTestStepBase
	{
public:
	CSimSatMmsNotificationDownload();
	virtual TVerdict doTestStepL();
	
private:
	TInt testSAT8534L();
	TInt testSAT8535L();
	TInt testSAT8536L();
	TInt testSAT8537L();
	TInt testSAT8538L();
	
	RSat iSat;
	};

class CSimSatUssdDataDownload : public CSimTestsTestStepBase
	{
public:
	CSimSatUssdDataDownload();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8539L();
	TInt testSAT8540L();
	
	RSat iSat;
	};

class CSimSatProvideLocalInfo : public CSimTestsTestStepBase
	{
public:
	CSimSatProvideLocalInfo();
	virtual TVerdict doTestStepL();
private:
	TInt testSAT8544L();
	TInt testSAT8545L();
	TInt testSAT8546L();
	TInt testSAT8547L();
	TInt testSAT8548L();
	TInt testSAT8549L();
	
	RSat iSat;
	
	};
	
class CSimSatLnchBrwsr : public CSimTestsTestStepBase
	{
public:
	CSimSatLnchBrwsr();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8550L();
	TInt testSAT8551L();
	TInt testSAT8552L();
	TInt testSAT8553L();
	TInt testSAT8554L();	
	TInt testSAT8555L();	
	RSat iSat;
	}; 

class CSimSatOpenChnl : public CSimTestsTestStepBase
	{
public:
	CSimSatOpenChnl();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8556L();
	TInt testSAT8557L();
	TInt testSAT8558L();
	TInt testSAT8559L();
		
	RSat iSat;
	};

class CSimSatPlayTone : public CSimTestsTestStepBase
	{
public:
	CSimSatPlayTone();
	virtual TVerdict doTestStepL();

private:
	TInt testSAT8560L();
	TInt testSAT8561L();
	TInt testSAT8562L();
	TInt testSAT8563L();
		
	RSat iSat;
	};

class CSimSatSetupCall : public CSimTestsTestStepBase
	{
public:
	CSimSatSetupCall();
	virtual TVerdict doTestStepL();
	
private:
	TInt testSAT8564L();
	TInt testSAT8565L();
	TInt testSAT8566L();
	TInt testSAT8567L();
	TInt testSAT8568L();
	
	RSat iSat;	
	};

class CSimSatRefresh : public CSimTestsTestStepBase
	{
public:
	CSimSatRefresh();
		virtual TVerdict doTestStepL();
	
private:
	TInt testSAT8569L();
	TInt testSAT8570L();
	TInt testSAT8571L();
		
	RSat iSat;	
	};

class CSimSatEventDnld : public CSimTestsTestStepBase
	{
public:
	CSimSatEventDnld();
	virtual TVerdict doTestStepL();
	
private:
	TInt testSAT8572L();
	TInt testSAT8573L();
	TInt testSAT8574L();
	TInt testSAT8575L();
	TInt testSAT8576L();
	TInt testSAT8577L();
		
	RSat iSat;	
	};

class CSimSatSendUssd : public CSimTestsTestStepBase
	{
public:
	CSimSatSendUssd();
	virtual TVerdict doTestStepL();
	
private:
	TInt testSAT8578L();
	TInt testSAT8579L();
	TInt testSAT8580L();
	TInt testSAT8581L();
	TInt testSAT8582L();
		
	RSat iSat;	
	};

class CSimSatGenPCmd : public CSimTestsTestStepBase
	{
public:
	CSimSatGenPCmd();
	virtual TVerdict doTestStepL();
	
private:
	TInt testSAT8583L();
	TInt testSAT8584L();
		
	RSat iSat;	
	};

class CSimSatElemFile : public CSimTestsTestStepBase
	{
public:
	CSimSatElemFile();
	virtual TVerdict doTestStepL();
	
private:
	TInt testSAT8585L();
	TInt testSAT8586L();
		
	RSat iSat;	
	};

#endif // __TE_SIM_SAT_TEST__

