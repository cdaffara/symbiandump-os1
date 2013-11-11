// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_A2DPBLUETOOTH_SVR_INITIALIZE_H__
#define __TSU_MMF_A2DPBLUETOOTH_SVR_INITIALIZE_H__


#include "TSU_MMF_A2DPBLUETOOTH_SVR_Step.h"


class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0001 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0001() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0001(){} ;
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPostambleL();
	} ;
		
	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0002 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0002() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0002(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0003 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0003() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0003(){} ;
	virtual TVerdict DoTestStepL();
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0004 : public CTestStep_MMF_A2DPBLUETOOTH_SVR//, MBluetoothPhysicalLinksNotifier
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0004() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0004(){} ;
	virtual TVerdict DoTestStepL();
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0005 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0005() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0005(){} ;
	virtual TVerdict DoTestStepL();
private:
	} ;
	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0006 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0006() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0006(){} ;
	virtual TVerdict DoTestStepL();
private:
	} ;
	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0007 : public CTestStep_MMF_A2DPBLUETOOTH_SVR//, MBluetoothPhysicalLinksNotifier
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0007() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0007(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TBool iInitializeComplete;
	} ;
	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0030 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0030() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0030(){} ;
	virtual TVerdict DoTestStepL();
	} ;
	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0010 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0010() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0010(){} ;
	virtual TVerdict DoTestStepL();
private:
	} ;
	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0020 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0020() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0020(){} ;
	virtual TVerdict DoTestStepL();
	static TInt NewThread(TAny* aTestStep); 
private:
	} ;
	

#endif /* __TSU_MMF_A2DPBLUETOOTH_SVR_INITIALIZE_H__ */
