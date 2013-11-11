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

/**
 @file
 @test
*/

#ifndef __T_WSERV_INTEG_STEP_SURFACE_H__
#define __T_WSERV_INTEG_STEP_SURFACE_H__

#include "t_wservintegstepbase.h"

_LIT(KWServPseudoApp,           "z:\\sys\\bin\\t_pseudoapp.exe");
_LIT(KWServPseudoApp1,          "z:\\sys\\bin\\t_pseudoapp1.exe");

_LIT(KWServTApp1,               "z:\\sys\\bin\\t_app1.exe");

_LIT(KPseudoAppData,			"pseudoapp_data%d");
_LIT(KPseudoAppDataName,		"pseudoapp_data%d_name");

_LIT(KPseudoApp1Data,			"pseudoapp1_data%d");
_LIT(KPseudoApp1DataName,		"pseudoapp1_data%d_name");

_LIT(KSimLoadData,				"simload_data%d");
_LIT(KSimLoadDataName,			"simload_data%d_name");

_LIT(KTApp1Data,				"tapp1_data%d");
_LIT(KTApp1DataName,			"tapp1_data%d_name");

_LIT(KPseudoAppResultData,			"pseudoappresult_data%d");
_LIT(KPseudoAppResultDataName,		"pseudoappresult_data%d_name");

_LIT(KPseudoApp1ResultData,			"pseudoapp1result_data%d");
_LIT(KPseudoApp1ResultDataName,		"pseudoapp1result_data%d_name");

_LIT(KT_WServIntegStepSurface, "WServIntegSurface");

_LIT(KCreatePseudoApp1,"create_pseudoapp1");


class CT_WServIntegStepSurface : public CT_WServIntegStep
	{
public:
    // Constructor
	CT_WServIntegStepSurface();
	virtual enum TVerdict	doTestStepL();

private:
	void CreatePseudoAppProcL();
	void CreateSimLoadProcL();
	};

#endif //__T_WSERV_INTEG_STEP_SURFACE_H__
