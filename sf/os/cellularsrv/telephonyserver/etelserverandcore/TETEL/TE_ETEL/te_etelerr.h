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

#ifndef _TE_ETELErr_H_
#define _TE_ETELPANICENUMERATEPHONES_H_

#include <etel.h>

// GetTsy 

class CTestErrGetTsyOk : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrGetTsyIllegalBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrGetTsySmallBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


// GetTsyVersionNumber

class CTestErrGetTsyVersionNumberOk : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrGetTsyVersionNumberIllegalBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrGetTsyVersionNumberSmallBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

// GetPhoneInfo

class CTestErrGetPhoneInfoOk : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrGetPhoneInfoIllegalBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrGetPhoneInfoSmallBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};



// IsSupportedByModule

class CTestErrIsSupportedByModuleOk: public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrIsSupportedByModuleRetIllegalBuffer: public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrIsSupportedByModuleRetSmallBuffer: public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrIsSupportedByModuleIllegalBuffer: public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrIsSupportedByModuleSmallBuffer: public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

// SetExtendedErrorGranularity

class CTestErrSetExtendedErrorGranularityOk : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrSetExtendedErrorGranularityPanic : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


// NewTelObject

class CTestErrNewTelObjectOk : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrNewTelObjectIllegalBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};

class CTestErrNewTelObjectSmallBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


class CTestErrNewTelObjectHandleIllegalBuffer : public CEtelTestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


#endif
