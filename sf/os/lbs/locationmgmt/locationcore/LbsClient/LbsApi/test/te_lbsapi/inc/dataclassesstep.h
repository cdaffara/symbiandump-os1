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
 @file DataClassesStep.h
 @internalTechnology
*/
#if (!defined __DATACLASSES_STEP_H__)
#define __DATACLASSES_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbsapisuitestepbase.h"

class CDataClassesStep : public CTe_LbsApiSuiteStepBase
	{
public:
	CDataClassesStep();
	~CDataClassesStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:

	void StartL(TInt aIndex);
// Data Classes
	void TCoordinate_BlankConstructionL();
	void TCoordinate_ConstructorsL();
	void TCoordinate_Constructors2L();
	void TCoordinate_InvalidL();
	void TCoordinate_NormalizationWithNansL();
	void TLocality_BlankConstructionL();
	void TLocality_ConstructorsL();
	void TLocality_Constructors2L();
	void TLocality_NegativeL();
	void TPosition_ConstructionL();
	void TPosition_SettersL();
	void TPositionInfo_ConstructionL();
	void TPositionInfo_ModificationL();
	void CourseClasses_ConstructionL();
	void CourseClasses_Modification2L();
	void CourseClasses_ModificationL();
	void CourseClasses_InvalidL();
	void SatelliteClasses_ConstructionL();
	void SatelliteClasses_PropertiesL();
	void SatelliteClasses_Properties2L();
	void SatelliteClasses_InfoL();
	void SatelliteClasses_SatelliteInfoL();
	void SatelliteClasses_OverflowL();
	void GpsMeasurementClasses_PropertiesL();

	};

_LIT(KDataClassesStep,"DataClassesStep");

#endif
