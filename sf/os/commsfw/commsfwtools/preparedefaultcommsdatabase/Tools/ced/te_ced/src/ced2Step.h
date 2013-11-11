/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ced2Step.h
 @internalTechnology
*/
#if (!defined __CED2_STEP_H__)
#define __CED2_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_cedSuiteStepBase.h"

/**
 * These test steps are to follow different links from different tables.
 */
 
/**
 * This test step is follow a location from an appended IAP record. The referenced
 * location record is already in the database which is referenced from the IAP record.
 */
class Cced2Step : public CTe_cedSuiteStepBase
	{
public:
	Cced2Step();
	~Cced2Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

/**
 * This test step is to follow a location link from an IAP record where both the
 * IAP record and the Location record is appended from the same XML file.
 */
class Cced3Step : public CTe_cedSuiteStepBase
	{
public:
	Cced3Step();
	~Cced3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

/**
 * This test step is to follow a location link from an IAP record where the location
 * record is already in the database.
 */
class Cced4Step : public CTe_cedSuiteStepBase
	{
public:
	Cced4Step();
	~Cced4Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

/**
 * This test step is to follow a location and a network link form the 
 * GlobalSettings Table.
 */
class Cced5Step : public CTe_cedSuiteStepBase
	{
public:
	Cced5Step();
	~Cced5Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

/**
 * This test step is to follow an EAPSecuritySettings link form the 
 * WLANServiceExtension Table.
 *
 * note: Linking is not working in CommsDat...
 */	
class Cced6Step : public CTe_cedSuiteStepBase
	{
public:
	Cced6Step();
	~Cced6Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

/**
 * This test step is to follow an TunnelledEAPSettings link form the 
 * EAPSecuritySettings Table.
 *
 * note: Linking is not working in CommsDat...
 */	
class Cced7Step : public CTe_cedSuiteStepBase
	{
public:
	Cced7Step();
	~Cced7Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

/**
 * This test step is to follow an EAPSecuritySettings link form the 
 * TunnelledEAPSettings Table.
 */	
class Cced8Step : public CTe_cedSuiteStepBase
	{
public:
	Cced8Step();
	~Cced8Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

/**
 * This test step is to follow an EAPTLSSettings link form the 
 * EAPSecuritySettings Table.
 *
 * note: Linking is not working in CommsDat...
 */	
class Cced9Step : public CTe_cedSuiteStepBase
	{
public:
	Cced9Step();
	~Cced9Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};
	
_LIT(Kced2Step,"ced2Step");
_LIT(Kced3Step,"ced3Step");
_LIT(Kced4Step,"ced4Step");
_LIT(Kced5Step,"ced5Step");
_LIT(Kced6Step,"ced6Step");
_LIT(Kced7Step,"ced7Step");
_LIT(Kced8Step,"ced8Step");
_LIT(Kced9Step,"ced9Step");

#endif
