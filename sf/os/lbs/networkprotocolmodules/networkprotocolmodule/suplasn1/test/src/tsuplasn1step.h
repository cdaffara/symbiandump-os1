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
//

/**
 @file 
 @internalComponent - Internal Symbian test code  
*/

#ifndef __TSUPLASN1BASE_H__
#define __TSUPLASN1BASE_H__

#include "tsuplasn1server.h"

_LIT(KSuplMessageStep, "SuplMessageStep");
_LIT(KRrcPayLoadStep, "RrcPayLoadStep");

class CSuplMessageContent;
class CSuplCellInfoContent;
class CSuplSetCapabilitiesContent;
class CSuplQoPContent;
class CSuplPositionContent;
class RUEPositioningGpsReferenceTimeBuilder;
class RUEPositioningGpsNavigationModelBuilder;
class RUEPositioningGpsIonosphericModelBuilder;
class RUEPositioningGpsUtcModelBuilder;
class RBadSatListBuilder;
class RUEPositioningGpsAlmanacBuilder;
class RReferenceLocationBuilder;
class RUEPositioningGpsAcquisitionAssistanceBuilder;

class CSuplMessageStep : public CTestStep
	{
public:	
	CSuplMessageStep();
	~CSuplMessageStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
protected:	
	void BuildMessageL(CSuplMessageContent* aMessageContent);
	void BuildSuplStartMessageL(CSuplMessageContent* aMessageContent);
	void BuildSuplResponseMessageL(CSuplMessageContent* aMessageContent);
	void BuildSuplPosInitMessageL(CSuplMessageContent* aMessageContent);
	void BuildSuplPosMessageL(CSuplMessageContent* aMessageContent);
	void BuildSuplEndMessageL(CSuplMessageContent* aMessageContent);
	void BuildCellInfoL(CSuplCellInfoContent* aCellInfoContent);
	void BuildSetCapabilitiesL(CSuplSetCapabilitiesContent* aSetCapabilitiesContent);
	void BuildQoPL(CSuplQoPContent* aContent);
	void BuildPositionInfoL(CSuplPositionContent* posContent);

	RFs iTheFs;
	TFileName iFileName;	
	};


class CRrcPayloadStep : public CTestStep
	{
public:
	CRrcPayloadStep();
	~CRrcPayloadStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	void ConstructL();
	HBufC8* ConvertDes16toHBufC8LC(TDesC& source);
	void CheckDecodedReferenceTimeResultsL(RUEPositioningGpsReferenceTimeBuilder* aGpsReferenceTimeBuilder);
	void CheckDecodedGpsNavigationModelResultsL(RUEPositioningGpsNavigationModelBuilder* aGpsNavigationModelBuilder);
	void CheckDecodedGpsIonosphericModelResultsL(RUEPositioningGpsIonosphericModelBuilder* aGpsIonosphericModelBuilder);
	void CheckDecodedGpsUtcModelResultsL(RUEPositioningGpsUtcModelBuilder* aGpsUtcModelBuilder);
	void CheckDecodedBadSatListResultsL(RBadSatListBuilder* aBadSatListBuilder);
	void CheckDecodedGpsAlmanacResultsL(RUEPositioningGpsAlmanacBuilder* aGpsAlmanacBuilder);
	void CheckDecodedReferenceLocationResultsL(RReferenceLocationBuilder* aReferenceLocationBuilder);
	void CheckDecodedGpsAcquisitionAssistanceResultsL(RUEPositioningGpsAcquisitionAssistanceBuilder* aGpsAcquisitionAssistanceBuilder);
private:
	RFs iTheFs;
	TFileName iFileName;
	};



#endif //SUPLASN1BASE



