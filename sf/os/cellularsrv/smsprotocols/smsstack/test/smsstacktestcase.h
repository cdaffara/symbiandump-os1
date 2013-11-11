// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#ifndef SMSSTACKTESTCASE_H
#define SMSSTACKTESTCASE_H

#include <gsmuelem.h>
#include <smsuaddr.H>

class TTestCase //: public CSmsPrtTestStep
	{
public:
	IMPORT_C TTestCase();
	IMPORT_C TTestCase(const TDesC& aMsg, const TDesC& aMatch);
	IMPORT_C TTestCase(const TDesC& aMsg, CSmsInformationElement::TSmsInformationElementIdentifier aId);
	IMPORT_C TTestCase(const TDesC& aMsg, TSmsFirstOctet::TSmsStatusReportRequest aSRR);
	IMPORT_C TTestCase(const TDesC& aMsg, TSmsDataCodingScheme::TSmsIndicationType aIndicationType,
	        TSmsDataCodingScheme::TSmsDCSBits7To4 aBits7To4, TSmsDataCodingScheme::TSmsIndicationState iIndicationState);
	IMPORT_C TTestCase(const TDesC& aMsg);
	IMPORT_C const TTestCase& operator=(const TTestCase& aTest);
	IMPORT_C void SetSmsClass(TSmsDataCodingScheme::TSmsClass aClass);
	IMPORT_C void SetValidityPeriod(const TTimeIntervalMinutes& aTimeInterval);
	IMPORT_C void SetRecvForever();
	
private:
	void CommonInit();
	
public:
	TSmsAddrFamily iMatchType;
	TPtrC iMsg;
	TPtrC iMatch;
	CSmsInformationElement::TSmsInformationElementIdentifier iIdentifierMatch;

	TBool iTestSmsClass;
	TSmsDataCodingScheme::TSmsClass iSmsClass;
	TBool iTestValidityPeriod;
	TTimeIntervalMinutes iValidityPeriod;
	TBool iTestForever;
	TBool iTestIndicators;
	TSmsDataCodingScheme::TSmsIndicationType iIndicationType;	// for ESmsAddrMessageIndication
	TSmsDataCodingScheme::TSmsDCSBits7To4 iBits7To4;			// for ESmsAddrMessageIndication
	TSmsDataCodingScheme::TSmsIndicationState iIndicationState; // for ESmsAddrMessageIndication
	};

#endif // SMSSTACKTESTCASE_H
