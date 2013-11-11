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
 @file QoS.TestSteps.cpp
*/

#include <test/dummy_protocol_parameter_sets.h>

#include "Qos.TestSteps.h"
#include "SubConnections.TestSteps.h"

// Compare Parameters (IPLink99)
//------------------------------

CCompareExtIPLink99ParameterSetStep::CCompareExtIPLink99ParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCompareExtIPLink99ParameterSetStep);
	}

TInt CCompareExtIPLink99ParameterSetStep::ConfigureFromIni()
	{
	iFamily1Id = 0;
	iFamily2Id = 0;
	iParams1Name.Set(KNullDesC);
	iParams2Name.Set(KNullDesC);

   	return ParseCompareDetails();
	}
    
TBool CCompareExtIPLink99ParameterSetStep::ParseCompareDetails()
    {
    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection,KTe_SubConnectionParams1Name,iParams1Name)!=1
        || iParams1Name.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionParams2Name,iParams2Name)!=1
        || iParams2Name.Length()==0)
        {
        INFO_PRINTF1(_L("Set 2 name missing."));
        return KErrNotFound;
        }
    
    if (GetIntFromConfig(iSection,KTe_CompareFamily1Type,iFamily1Id)!=1)
        {
        INFO_PRINTF1(_L("Family 1 id missing."));
        return KErrNotFound;
        }

    if (GetIntFromConfig(iSection,KTe_CompareFamily2Type,iFamily2Id)!=1)
        {
        INFO_PRINTF1(_L("Family 2 id missing."));
        return KErrNotFound;
        }
    
    //no need to clear (after previous runs) as all parameters must be read
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,KTe_SubConnectionParams1Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iFamily1Type = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iFamily1Type = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iFamily1Type = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }

    //no need to clear (after previous runs) as all parameters must be read
    if (GetStringFromConfig(iSection,KTe_SubConnectionParams2Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iFamily2Type = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iFamily2Type = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iFamily2Type = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }
        
    if (!GetBoolFromConfig(iSection,KTe_ExpectedCompareResult,iExpectedCompareResult))  
	   {
   		iExpectedCompareResult = ETrue;
   	   }   
   		else 
   		{
   		INFO_PRINTF2(_L("Expected to match: %d"),iExpectedCompareResult);
   		}

    // All ok if we got this far
    return KErrNone;
	}

TBool CCompareExtIPLink99ParameterSetStep::CompareR4Params(CSubConQosIPLinkR99ParamSet& ps1,CSubConQosIPLinkR99ParamSet& ps2)
	{
	
	TBool match = ETrue;
	INFO_PRINTF3(_L("Comparing CSubConQosIPLinkR99ParamSets (%S vs %S)."),&iParams1Name,&iParams2Name);
	if (ps1.GetTrafficClass() != ps2.GetTrafficClass())
	    {
	    INFO_PRINTF5(_L("TrafficClass differs! %S: %d %S: %d"),&iParams1Name, ps1.GetTrafficClass(),&iParams2Name, ps2.GetTrafficClass());
	    match = EFalse;
	    }
	if (ps1.GetDeliveryOrder() != ps2.GetDeliveryOrder())
	    {
	    INFO_PRINTF5(_L("DeliveryOrder differs! %S: %d %S: %d"),&iParams1Name, ps1.GetDeliveryOrder(), &iParams2Name, ps2.GetDeliveryOrder());
	    match = EFalse;
	    }
	if (ps1.GetErroneousSDUDelivery() != ps2.GetErroneousSDUDelivery())
	    {
	    INFO_PRINTF5(_L("ErroneousSDUDelivery differs! %S: %d %S: %d"),&iParams1Name, ps1.GetErroneousSDUDelivery(), &iParams2Name, ps2.GetErroneousSDUDelivery());
	    match = EFalse;
	    }
	if (ps1.GetResidualBitErrorRatio() != ps2.GetResidualBitErrorRatio())
	    {
	    INFO_PRINTF5(_L("ResidualBitErrorRatio differs! %S: %d %S: %d"),&iParams1Name, ps1.GetResidualBitErrorRatio(), &iParams2Name, ps2.GetResidualBitErrorRatio());
	    match = EFalse;
	    }
	if (ps1.GetSDUErrorRatio() != ps2.GetSDUErrorRatio())
	    {
	    INFO_PRINTF5(_L("SDUErrorRatio differs! %S: %d %S: %d"),&iParams1Name, ps1.GetSDUErrorRatio(), &iParams2Name, ps2.GetSDUErrorRatio());
	    match = EFalse;
	    }
	if (ps1.GetTrafficHandlingPriority() != ps2.GetTrafficHandlingPriority())
	    {
	    INFO_PRINTF5(_L("TrafficHandlingPriority differs! %S: %d %S: %d"),&iParams1Name, ps1.GetTrafficHandlingPriority(), &iParams2Name, ps2.GetTrafficHandlingPriority());
	    match = EFalse;
	    }
	if (ps1.GetTransferDelay() != ps2.GetTransferDelay())
	    {
	    INFO_PRINTF5(_L("TransferDelay differs! %S: %d %S: %d"),&iParams1Name, ps1.GetTransferDelay(), &iParams2Name, ps2.GetTransferDelay());
	    match = EFalse;
	    }
	if (ps1.GetMaxSduSize() != ps2.GetMaxSduSize())
	    {
	    INFO_PRINTF5(_L("MaxSduSize differs! %S: %d %S: %d"),&iParams1Name, ps1.GetMaxSduSize(), &iParams2Name, ps2.GetMaxSduSize());
	    match = EFalse;
	    }
	if (ps1.GetMaxBitrateUplink() != ps2.GetMaxBitrateUplink())
	    {
	    INFO_PRINTF5(_L("MaxBitrateUplink differs! %S: %d %S: %d"),&iParams1Name, ps1.GetMaxBitrateUplink(), &iParams2Name, ps2.GetMaxBitrateUplink());
	    match = EFalse;
	    }
	if (ps1.GetMaxBitrateDownlink() != ps2.GetMaxBitrateDownlink())
	    {
	    INFO_PRINTF5(_L("MaxBitrateDownlink differs! %S: %d %S: %d"),&iParams1Name, ps1.GetMaxBitrateDownlink(), &iParams2Name, ps2.GetMaxBitrateDownlink());
	    match = EFalse;
	    }
	if (ps1.GetGuaBitrateUplink() != ps2.GetGuaBitrateUplink())
	    {
	    INFO_PRINTF5(_L("GuaBitrateUplink differs! %S: %d %S: %d"),&iParams1Name, ps1.GetGuaBitrateUplink(), &iParams2Name, ps2.GetGuaBitrateUplink());
	    match = EFalse;
	    }
	if (ps1.GetGuaBitrateDownlink() != ps2.GetGuaBitrateDownlink())
	    {
	    INFO_PRINTF5(_L("GuaBitrateDownlink differs! %S: %d %S: %d"),&iParams1Name, ps1.GetGuaBitrateDownlink(), &iParams2Name, ps2.GetGuaBitrateDownlink());
	    match = EFalse;
	    }
	
	if (match)
	{
	INFO_PRINTF1(_L("R4 Param sets are identical."));	
	}
    
	
	return match;
	}

TVerdict CCompareExtIPLink99ParameterSetStep::doSingleTestStep()
	{
    CSubConParameterFamily* f1 = iEsockTest->FindRSubConnectionParameterFamily(iParams1Name,iFamily1Id);
    if (f1==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters family (%S) not found"),&iParams1Name);
        return EFail;
        }
    CSubConQosIPLinkR99ParamSet* ps1 = static_cast<CSubConQosIPLinkR99ParamSet*>(f1->FindExtensionSet(STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConQosIPLinkR99ParamsType ), iFamily1Type));
    if (ps1==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters (%S) not found"),&iParams1Name);
        return EFail;
        }

    CSubConParameterFamily* f2 = iEsockTest->FindRSubConnectionParameterFamily(iParams2Name,iFamily2Id);
    if (f2==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters family (%S) not found"),&iParams2Name);
        return EFail;
        }
    CSubConQosIPLinkR99ParamSet* ps2 = static_cast<CSubConQosIPLinkR99ParamSet*>(f2->FindExtensionSet(STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConQosIPLinkR99ParamsType ), iFamily2Type));
    if (ps2==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters (%S) not found"),&iParams2Name);
        return EFail;
        }

    TBool cmp = CompareR4Params(*ps1,*ps2);
    if (cmp && iExpectedCompareResult)
        {
        INFO_PRINTF1(_L("Subcon parameters match."));
        return EPass;
        }

	if (iExpectedCompareResult)
	{
	INFO_PRINTF1(_L("Subcon parameters do not match."));
	return EFail;		
	}
	else
	{
	INFO_PRINTF1(_L("Subcon parameters do not match as expected."));
	return EPass;	
	}
	
	}



// Compare Parameters (ExtSBLP)
//-----------------------------

CCompareExtSBLPParameterSetStep::CCompareExtSBLPParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCompareExtSBLPParameterSetStep);
	}

TInt CCompareExtSBLPParameterSetStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
    iParams.Reset();

	INFO_PRINTF1(_L("This test step has not yet been implemented."));
    return KErrNotFound;
    }

TBool CCompareExtSBLPParameterSetStep::Compare(CSubConSBLPR5ExtensionParamSet& /*ps1*/,CSubConSBLPR5ExtensionParamSet& /*ps2*/)
	{
	//not yet implemented
	return EFalse;
	}

TVerdict CCompareExtSBLPParameterSetStep::doSingleTestStep()
	{
    TBool cmp = EFalse; //Compare();
    if (cmp)
        {
        INFO_PRINTF1(_L("Subcon parameters match."));
        return EPass;
        }

	INFO_PRINTF1(_L("Subcon parameters do not match."));
	return EFail;
	}


// Compare Parameters (GenQoS)
//----------------------------

CCompareGenQosParameterSetStep::CCompareGenQosParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
    SetTestStepName(KCompareGenQosParameterSetStep);
    }

TInt CCompareGenQosParameterSetStep::ConfigureFromIni()
    {
    //clean parameters from previous runs
	iFamily1Id = 0;
	iFamily2Id = 0;
	iParams1Name.Set(KNullDesC);
	iParams2Name.Set(KNullDesC);

    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection,KTe_SubConnectionParams1Name,iParams1Name)!=1
        || iParams1Name.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionParams2Name,iParams2Name)!=1
        || iParams2Name.Length()==0)
        {
        INFO_PRINTF1(_L("Set 2 name missing."));
        return KErrNotFound;
        }
    
    if (GetIntFromConfig(iSection,KTe_CompareFamily1Type,iFamily1Id)!=1)
        {
        INFO_PRINTF1(_L("Family 1 id missing."));
        return KErrNotFound;
        }

    if (GetIntFromConfig(iSection,KTe_CompareFamily2Type,iFamily2Id)!=1)
        {
        INFO_PRINTF1(_L("Family 2 id missing."));
        return KErrNotFound;
        }
    
    //no need to clear (after previous runs) as all parameters must be read
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,KTe_SubConnectionParams1Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iFamily1Type = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iFamily1Type = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iFamily1Type = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }

    //no need to clear (after previous runs) as all parameters must be read
    if (GetStringFromConfig(iSection,KTe_SubConnectionParams2Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iFamily2Type = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iFamily2Type = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iFamily2Type = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }

    return KErrNone;
    }

TBool CCompareGenQosParameterSetStep::Compare(CSubConQosGenericParamSet& ps1,CSubConQosGenericParamSet& ps2)
    {
	INFO_PRINTF3(_L("Comparing CSubConQosGenericParamSets (%S vs %S)."),&iParams1Name,&iParams2Name);
	if (ps1.GetHeaderMode() != ps2.GetHeaderMode())
	    {
	    INFO_PRINTF1(_L("HeaderMode differs!"));
	    return EFalse;
	    }
	if (ps1.GetDownlinkBandwidth() != ps2.GetDownlinkBandwidth())
	    {
	    INFO_PRINTF1(_L("DownlinkBandwidth differs!"));
	    return EFalse;
	    }
	if (ps1.GetUplinkBandwidth() != ps2.GetUplinkBandwidth())
	    {
	    INFO_PRINTF1(_L("UplinkBandwidth differs!"));
	    return EFalse;
	    }
	if (ps1.GetDownLinkMaximumBurstSize() != ps2.GetDownLinkMaximumBurstSize())
	    {
	    INFO_PRINTF1(_L("DownLinkMaximumBurstSize differs!"));
	    return EFalse;
	    }
	if (ps1.GetUpLinkMaximumBurstSize() != ps2.GetUpLinkMaximumBurstSize())
	    {
	    INFO_PRINTF1(_L("UpLinkMaximumBurstSize differs!"));
	    return EFalse;
	    }
	if (ps1.GetDownLinkAveragePacketSize() != ps2.GetDownLinkAveragePacketSize())
	    {
	    INFO_PRINTF1(_L("DownLinkAveragePacketSize differs!"));
	    return EFalse;
	    }
	if (ps1.GetUpLinkAveragePacketSize() != ps2.GetUpLinkAveragePacketSize())
	    {
	    INFO_PRINTF1(_L("UpLinkAveragePacketSize differs!"));
	    return EFalse;
	    }
	if (ps1.GetDownLinkMaximumPacketSize() != ps2.GetDownLinkMaximumPacketSize())
	    {
	    INFO_PRINTF1(_L("DownLinkMaximumPacketSize differs!"));
	    return EFalse;
	    }
	if (ps1.GetUpLinkMaximumPacketSize() != ps2.GetUpLinkMaximumPacketSize())
	    {
	    INFO_PRINTF1(_L("UpLinkMaximumPacketSize differs!"));
	    return EFalse;
	    }
	if (ps1.GetDownLinkDelay() != ps2.GetDownLinkDelay())
	    {
	    INFO_PRINTF1(_L("DownLinkDelay differs!"));
	    return EFalse;
	    }
	if (ps1.GetUpLinkDelay() != ps2.GetUpLinkDelay())
	    {
	    INFO_PRINTF1(_L("UpLinkDelay differs!"));
	    return EFalse;
	    }
	if (ps1.GetDownLinkDelayVariation() != ps2.GetDownLinkDelayVariation())
	    {
	    INFO_PRINTF1(_L("DownLinkDelayVariation differs!"));
	    return EFalse;
	    }
	if (ps1.GetUpLinkDelayVariation() != ps2.GetUpLinkDelayVariation())
	    {
	    INFO_PRINTF1(_L("UpLinkDelayVariation differs!"));
	    return EFalse;
	    }
	if (ps1.GetDownLinkPriority() != ps2.GetDownLinkPriority())
	    {
	    INFO_PRINTF1(_L("DownLinkPriority differs!"));
	    return EFalse;
	    }

    INFO_PRINTF1(_L("Param sets are identical."));
	return ETrue;
    }




TVerdict CCompareGenQosParameterSetStep::doSingleTestStep()
    {
    CSubConParameterFamily* f1 = iEsockTest->FindRSubConnectionParameterFamily(iParams1Name,iFamily1Id);
    if (f1==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters family (%S) not found"),&iParams1Name);
        return EFail;
        }
    CSubConQosGenericParamSet* ps1 = static_cast<CSubConQosGenericParamSet*>(f1->GetGenericSet(iFamily1Type));
    if (ps1==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters (%S) not found"),&iParams1Name);
        return EFail;
        }

    CSubConParameterFamily* f2 = iEsockTest->FindRSubConnectionParameterFamily(iParams2Name,iFamily2Id);
    if (f2==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters family (%S) not found"),&iParams2Name);
        return EFail;
        }
    CSubConQosGenericParamSet* ps2 = static_cast<CSubConQosGenericParamSet*>(f2->GetGenericSet(iFamily2Type));
    if (ps2==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters (%S) not found"),&iParams2Name);
        return EFail;
        }

    TBool cmp = Compare(*ps1,*ps2);
    if (cmp)
        {
        INFO_PRINTF1(_L("Subcon parameters match."));
        return EPass;
        }

    INFO_PRINTF1(_L("Subcon parameters do not match."));
    return EFail;
    }


// Compare Parameters (GenSBLP)
//-----------------------------

CCompareGenSBLPParameterSetStep::CCompareGenSBLPParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCompareGenSBLPParameterSetStep);
	}

TInt CCompareGenSBLPParameterSetStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
    iParams.Reset();

	INFO_PRINTF1(_L("This test step has not yet been implemented."));
    return KErrNotFound;
    }

TBool CCompareGenSBLPParameterSetStep::Compare(CSubConAuthorisationGenericParamSet /*ps1*/,CSubConAuthorisationGenericParamSet /*ps2*/)
	{
	//not yet implemented
	return EFalse;
	}

TVerdict CCompareGenSBLPParameterSetStep::doSingleTestStep()
	{
    TBool cmp = EFalse; //Compare();
    if (cmp)
        {
        INFO_PRINTF1(_L("Subcon parameters match."));
        return EPass;
        }

	INFO_PRINTF1(_L("Subcon parameters do not match."));
	return EFail;
	}



// Close SubConnection Parameter Bundle
//-------------------------------------

CCloseRSubConParameterBundleStep::CCloseRSubConParameterBundleStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCloseRSubConParameterBundleStep);
	}

TInt CCloseRSubConParameterBundleStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iParams.iSubConnectionParamsName)!=1
        || iParams.iSubConnectionParamsName.Length()==0)
            return KErrNotFound;

	return KErrNone;
    }

TVerdict CCloseRSubConParameterBundleStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CloseRSubConnectionParams(iParams.iSubConnectionParamsName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not close subcon parameters (%S)."),&iParams.iSubConnectionParamsName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Create SubConnection Parameter Bundle
//--------------------------------------
CCreateRSubConParameterBundleStep::CCreateRSubConParameterBundleStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreateRSubConParameterBundleStep);
	}

TInt CCreateRSubConParameterBundleStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection, KTe_SubConnectionParamsName, iParams.iSubConnectionParamsName) != 1
        || iParams.iSubConnectionParamsName.Length()==0)
            return KErrNotFound;

	return KErrNone;
    }

TVerdict CCreateRSubConParameterBundleStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CreateRSubConnectionParams(iParams.iSubConnectionParamsName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not create subcon parameters (%S)."),&iParams.iSubConnectionParamsName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Create SubConnection Parameter Family
//--------------------------------------
CCreateRSubConParameterFamilyStep::CCreateRSubConParameterFamilyStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreateRSubConParameterFamilyStep);
	}

TInt CCreateRSubConParameterFamilyStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iSubConnectionParamsName.Set(KNullDesC);
	iFamilyId = 0;

    //try reading the name of the subconnection params (bundle)
    if (GetIntFromConfig(iSection,KTe_Type,iFamilyId)!=1)
        return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iSubConnectionParamsName)!=1
        || iSubConnectionParamsName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }

    return KErrNone;
    }

void CCreateRSubConParameterFamilyStep::ConstructFamilyFromIniL()
	{
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iSubConnectionParamsName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//create a container for QoS sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* family = CSubConParameterFamily::NewL(*bundle, iFamilyId);
	}

TVerdict CCreateRSubConParameterFamilyStep::doSingleTestStep()
	{
	TRAPD(error,ConstructFamilyFromIniL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise subcon parameters (%d)"),error);
        return EFail;
        }
	return EPass;
	}



// Create SBLP Flow ID
//--------------------
CCreateSBLPFlowIdStep ::CCreateSBLPFlowIdStep (CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreateSBLPFlowIdStep);
	}

TInt CCreateSBLPFlowIdStep::ConfigureFromIni()
    {
    /*
      Read Sblp Flow details from corresponding ini files 
    */
	iFlowExtName.Set(KNullDesC);
	
    if (GetStringFromConfig(iSection,KTe_SubConnectionSblpExtFlowName,iFlowExtName)!=1
        || iFlowExtName.Length()==0)
            return KErrNotFound;

    return KErrNone;
    }

TVerdict CCreateSBLPFlowIdStep ::doSingleTestStep()
	{
	//create only
   	TInt error = iEsockTest->CreateTFlowId(iFlowExtName);
   	if(error!=KErrNone)
   		{
   		INFO_PRINTF2(_L("Could not create Flow Id for sblp events (%S)."),&iFlowExtName);
   		return EFail;
        }
    return EPass;
	}


// Get SubConnection Parameters
//-----------------------------
CGetRSubConParametersStep::CGetRSubConParametersStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KGetRSubConParametersStep);
	}

TInt CGetRSubConParametersStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iParams.iSubConnectionParamsName)!=1
        || iParams.iSubConnectionParamsName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1)
        {
        INFO_PRINTF1(_L("Subconnection name missing."));
        return KErrNotFound;
        }

    return KErrNone;
    }

TVerdict CGetRSubConParametersStep::doSingleTestStep()
	{
    TInt error = iEsockTest->GetSubConnectionParameters(iParams);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not retrieve subconnection parameters (%d)"),error);
        return EFail;
        }
	return EPass;
	}


// Set SubConnection Parameters
//-----------------------------
CSetRSubConParametersStep::CSetRSubConParametersStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSetRSubConParametersStep);
	}

TInt CSetRSubConParametersStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection, KTe_SubConnectionParamsName, iParams.iSubConnectionParamsName)!=1
        || iParams.iSubConnectionParamsName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1)
        {
        INFO_PRINTF1(_L("Subconnection name missing."));
        return KErrNotFound;
        }
        
    if (!GetIntFromConfig(iSection, KExpectedError, iExpectedError))  
       {
       	iExpectedError = KErrNone;
       }
       
       else 
       {
       	INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
       }
       

    return KErrNone;
    }

TVerdict CSetRSubConParametersStep::doSingleTestStep()
	{
    TInt error = iEsockTest->SetSubConnectionParameters(iParams);
    
   	
	
	if (error!=KErrNone)
          {
          INFO_PRINTF2(_L("Could not set qos parameters (%d)"),error);
          INFO_PRINTF2(_L("Error: %d."),error);
          } 

    if (error!=iExpectedError)
          {
          INFO_PRINTF3(_L("Returned error (%d) differs from expected (%d)."), error, iExpectedError);
          return EFail;
          }
          
         return EPass;
    
	}


// Initialise Parameter Set (IPLink99)
//------------------------------------
CinitialiseExtIPLink99ParameterSetStep::CinitialiseExtIPLink99ParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KinitialiseExtIPLink99ParameterSetStep);
	}

TInt CinitialiseExtIPLink99ParameterSetStep::ConfigureFromIni()
    {
	return ParseIniIPLinkParameters() ? KErrNone : KErrNotFound;
    }

void CinitialiseExtIPLink99ParameterSetStep::ConstructIPLink99SetL()
	{
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//find the family
	CSubConParameterFamily* family = bundle->FindFamily(iFamilyId);
	if (family==NULL)
        {
        INFO_PRINTF1(_L("Family not found in the bundle."));
        User::Leave(KErrNotFound);
        
        }
 	//   RProcess proc;
 	//   proc = RProcess::
 
 	TSecurityInfo secinfo;
 	secinfo.SetToCurrentInfo();
    
     //construct the generic set when we have all parameters available
    CSubConQosIPLinkR99ParamSet* ipLink99ParSet = CSubConQosIPLinkR99ParamSet::NewL(*family,iParams.iType);
    InitialiseR99Params(ipLink99ParSet);
	}
	
void CinitialiseExtIPLink99ParameterSetStep::InitialiseR99Params(CSubConQosIPLinkR99ParamSet* ipLink99ParSet)
	{
	ipLink99ParSet->SetTrafficClass(iParams.iTrafficClass);
    ipLink99ParSet->SetDeliveryOrder(iParams.iDeliveryOrder);
    ipLink99ParSet->SetErroneousSDUDelivery(iParams.iDeliveryOfErroneusSdu);
    ipLink99ParSet->SetResidualBitErrorRatio(iParams.iResidualBer);
    ipLink99ParSet->SetSDUErrorRatio(iParams.iErrorRatio);
    ipLink99ParSet->SetTrafficHandlingPriority(iParams.iPriority);
    ipLink99ParSet->SetTransferDelay(iParams.iTransferDelay);
    ipLink99ParSet->SetMaxSduSize(iParams.iMaxSduSize);
    ipLink99ParSet->SetMaxBitrateUplink(iParams.iMaxBitrateUplink);
    ipLink99ParSet->SetMaxBitrateDownlink(iParams.iMaxBitrateDownlink);
    ipLink99ParSet->SetGuaBitrateUplink(iParams.iGuaBitrateUplink);
    ipLink99ParSet->SetGuaBitrateDownlink(iParams.iGuaBitrateDownlink);
	}

TVerdict CinitialiseExtIPLink99ParameterSetStep::doSingleTestStep()
	{
	TRAPD(error, ConstructIPLink99SetL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise IPLink99 set (%d)"),error);
        return EFail;
        }
	return EPass;
	}
TBool CinitialiseExtIPLink99ParameterSetStep::ParseIniIPLinkParameters()
	{		
	iSetName.Set(KNullDesC);
	iBundleName.Set(KNullDesC);
	iFamilyId = 0;

    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection,KTe_Name,iSetName)!=1
        || iSetName.Length()==0)
            return EFalse;
    
    if (GetIntFromConfig(iSection,KTe_ParamsFamilyType,iFamilyId)!=1)
        {
        INFO_PRINTF1(_L("Family id missing."));
        return EFalse;
        }

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iBundleName)!=1
        || iBundleName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return EFalse;
        }

    //no need to clear (after previous runs) as all parameters must be read
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,KTe_Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return EFalse;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iParams.iType = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iParams.iType = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iParams.iType = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return EFalse;
        }

    TPtrC paramSetTrafficClass;
    //fieldName.Copy(KTe_SubConIPLink99ParamsSetName);
    //fieldName.Append(KTe_SubConIPLink99TrafficClass);
    if (GetStringFromConfig(iSection,KTe_TrafficClass,paramSetTrafficClass)!=1
        || paramSetTrafficClass.Length()==0)
        {
        INFO_PRINTF1(_L("Traffic class missing."));
        return EFalse;
        }

    if (paramSetTrafficClass.Compare(KTe_SubConIPLink99TrafficClassUnspecified)==0)
	    { iParams.iTrafficClass = RPacketQoS::ETrafficClassUnspecified; }
    else if (paramSetTrafficClass.Compare(KTe_SubConIPLink99TrafficClassConversational)==0)
	    { iParams.iTrafficClass = RPacketQoS::ETrafficClassConversational; }
    else if (paramSetTrafficClass.Compare(KTe_SubConIPLink99TrafficClassStreaming)==0)
	    { iParams.iTrafficClass = RPacketQoS::ETrafficClassStreaming; }
    else if (paramSetTrafficClass.Compare(KTe_SubConIPLink99TrafficClassInteractive)==0)
	    { iParams.iTrafficClass = RPacketQoS::ETrafficClassInteractive; }
    else if (paramSetTrafficClass.Compare(KTe_SubConIPLink99TrafficClassBackground)==0)
	    { iParams.iTrafficClass = RPacketQoS::ETrafficClassBackground; }
    else
        {
        INFO_PRINTF1(_L("Traffic class not recognised!"));
        return EFalse;
        }

    TPtrC paramSetDeliveryOrder;
    //fieldName.Copy(KTe_SubConIPLink99ParamsSetName);
    //fieldName.Append(KTe_SubConIPLink99DeliveryOrder);
    if (GetStringFromConfig(iSection,KTe_DeliveryOrder,paramSetDeliveryOrder)!=1
        || paramSetDeliveryOrder.Length()==0)
        {
        INFO_PRINTF1(_L("Delivery order missing."));
        return EFalse;
        }

    if (paramSetDeliveryOrder.Compare(KTe_SubConIPLink99DeliveryOrderUnspecified)==0)
	    { iParams.iDeliveryOrder = RPacketQoS::EDeliveryOrderUnspecified; }
    else if (paramSetDeliveryOrder.Compare(KTe_SubConIPLink99DeliveryOrderRequired)==0)
	    { iParams.iDeliveryOrder = RPacketQoS::EDeliveryOrderRequired; }
    else if (paramSetDeliveryOrder.Compare(KTe_SubConIPLink99DeliveryOrderNotRequired)==0)
	    { iParams.iDeliveryOrder = RPacketQoS::EDeliveryOrderNotRequired; }
    else
        {
        INFO_PRINTF1(_L("Delivery order not recognised!"));
        return EFalse;
        }

    TPtrC paramSetErroneousSDUDelivery;
    //fieldName.Copy(KTe_SubConIPLink99ParamsSetName);
    //fieldName.Append(KTe_SubConIPLink99ErroneousSDUDelivery);
    if (GetStringFromConfig(iSection,KTe_ErroneousSDUDelivery,paramSetErroneousSDUDelivery)!=1
        || paramSetErroneousSDUDelivery.Length()==0)
        {
        INFO_PRINTF1(_L("Erroneous SDU delivery missing."));
        return EFalse;
        }

    if (paramSetErroneousSDUDelivery.Compare(KTe_SubConIPLink99ErrSDUDeliveryUnspecified)==0)
	    { iParams.iDeliveryOfErroneusSdu = RPacketQoS::EErroneousSDUDeliveryUnspecified; }
    else if (paramSetErroneousSDUDelivery.Compare(KTe_SubConIPLink99ErrSDUDeliveryDetection)==0)
	    { iParams.iDeliveryOfErroneusSdu = RPacketQoS::EErroneousSDUNoDetection; }
    else if (paramSetErroneousSDUDelivery.Compare(KTe_SubConIPLink99ErrSDUDeliveryRequired)==0)
	    { iParams.iDeliveryOfErroneusSdu = RPacketQoS::EErroneousSDUDeliveryRequired; }
    else if (paramSetErroneousSDUDelivery.Compare(KTe_SubConIPLink99ErrSDUDeliveryNotRequired)==0)
	    { iParams.iDeliveryOfErroneusSdu = RPacketQoS::EErroneousSDUDeliveryNotRequired; }
    else
        {
        INFO_PRINTF1(_L("Erroneous SDU delivery not recognised!"));
        return EFalse;
        }

    TPtrC paramSetResBer;
    //fieldName.Copy(KTe_SubConIPLink99ParamsSetName);
    //fieldName.Append(KTe_SubConIPLink99ResidualBitErrorRatio);
    if (GetStringFromConfig(iSection,KTe_ResidualBitErrorRatio,paramSetResBer)!=1
        || paramSetResBer.Length()==0)
        {
        INFO_PRINTF1(_L("Residual bit error ratio missing."));
        return EFalse;
        }

    if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioUnspecified)==0)
	    { iParams.iResidualBer = RPacketQoS::EBERUnspecified; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioFivePerHundred)==0)
	    { iParams.iResidualBer = RPacketQoS::EBERFivePerHundred; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioOnePerHundred)==0)
	    { iParams.iResidualBer = RPacketQoS::EBEROnePerHundred; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioFivePerThousand)==0)
	    { iParams.iResidualBer = RPacketQoS::EBERFivePerThousand; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioFourPerThousand)==0)
	    { iParams.iResidualBer = RPacketQoS::EBERFourPerThousand; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioOnePerThousand)==0)
	    { iParams.iResidualBer = RPacketQoS::EBEROnePerThousand; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioOnePerTenThousand)==0)
	    { iParams.iResidualBer = RPacketQoS::EBEROnePerTenThousand; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioOnePerHundredThousand)==0)
	    { iParams.iResidualBer = RPacketQoS::EBEROnePerHundredThousand; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioOnePerMillion)==0)
	    { iParams.iResidualBer = RPacketQoS::EBEROnePerMillion; }
    else if (paramSetResBer.Compare(KTe_SubConIPLink99ResBitErrorRatioSixPerHundredMillion)==0)
	    { iParams.iResidualBer = RPacketQoS::EBERSixPerHundredMillion; }
    else
        {
        INFO_PRINTF1(_L("Residual bit error ratio not recognised!"));
        return EFalse;
        }

    TPtrC paramSetSDUErrorRatio;
    //fieldName.Copy(KTe_SubConIPLink99ParamsSetName);
    //fieldName.Append(KTe_SubConIPLink99SDUErrorRatio);
    if (GetStringFromConfig(iSection,KTe_SDUErrorRatio,paramSetSDUErrorRatio)!=1
        || paramSetSDUErrorRatio.Length()==0)
        {
        INFO_PRINTF1(_L("SDU error ratio missing."));
        return EFalse;
        }

    if (paramSetSDUErrorRatio.Compare(KTe_SubConIPLink99SDUErrorRatioUnspecified)==0)
	    { iParams.iErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified; }
    else if (paramSetSDUErrorRatio.Compare(KTe_SubConIPLink99SDUErrorRatioOnePerTen)==0)
	    { iParams.iErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTen; }
    else if (paramSetSDUErrorRatio.Compare(KTe_SubConIPLink99SDUErrorRatioOnePerHundred)==0)
	    { iParams.iErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundred; }
    else if (paramSetSDUErrorRatio.Compare(KTe_SubConIPLink99SDUErrorRatioSevenPerThousand)==0)
	    { iParams.iErrorRatio = RPacketQoS::ESDUErrorRatioSevenPerThousand; }
    else if (paramSetSDUErrorRatio.Compare(KTe_SubConIPLink99SDUErrorRatioOnePerThousand)==0)
	    { iParams.iErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand; }
    else if (paramSetSDUErrorRatio.Compare(KTe_SubConIPLink99SDUErrorRatioOnePerTenThousand)==0)
	    { iParams.iErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand; }
    else if (paramSetSDUErrorRatio.Compare(KTe_SubConIPLink99SDUErrorRatioOnePerHundredThousand)==0)
	    { iParams.iErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundredThousand; }
    else if (paramSetSDUErrorRatio.Compare(KTe_SubConIPLink99SDUErrorRatioOnePerMillion)==0)
	    { iParams.iErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion; }
    else
        {
        INFO_PRINTF1(_L("SDU error ratio not recognised!"));
        return EFalse;
        }

    TPtrC paramSetTrafficHandlingPriority;
    if (GetStringFromConfig(iSection,KTe_TrafficHandlingPriority,paramSetTrafficHandlingPriority)!=1
        || paramSetTrafficHandlingPriority.Length()==0)
        {
        INFO_PRINTF1(_L("Traffic handling priority missing."));
        return EFalse;
        }

    if (paramSetTrafficHandlingPriority.Compare(KTe_SubConIPLink99TrafficPriorityUnspecified)==0)
	    { iParams.iPriority = RPacketQoS::ETrafficPriorityUnspecified; }
    else if (paramSetTrafficHandlingPriority.Compare(KTe_SubConIPLink99TrafficPriority1)==0)
	    { iParams.iPriority = RPacketQoS::ETrafficPriority1; }
    else if (paramSetTrafficHandlingPriority.Compare(KTe_SubConIPLink99TrafficPriority2)==0)
	    { iParams.iPriority = RPacketQoS::ETrafficPriority2; }
    else if (paramSetTrafficHandlingPriority.Compare(KTe_SubConIPLink99TrafficPriority3)==0)
	    { iParams.iPriority = RPacketQoS::ETrafficPriority3; }
    else
        {
        INFO_PRINTF1(_L("Traffic handling priority not recognised."));
        return EFalse;
        }

    if (GetIntFromConfig(iSection,KTe_TransferDelay,iParams.iTransferDelay)!=1)
        {
        INFO_PRINTF1(_L("Transfer delay missing."));
        return EFalse;
        }

    if (GetIntFromConfig(iSection,KTe_MaxSduSize,iParams.iMaxSduSize)!=1)
        {
        INFO_PRINTF1(_L("Max SDU size missing."));
        return EFalse;
        }

    if (GetIntFromConfig(iSection,KTe_MaxBitrateUplink,iParams.iMaxBitrateUplink)!=1)
        {
        INFO_PRINTF1(_L("Max bitrate uplink missing."));
        return EFalse;
        }

    if (GetIntFromConfig(iSection,KTe_MaxBitrateDownlink,iParams.iMaxBitrateDownlink)!=1)
        {
        INFO_PRINTF1(_L("Max bitrate downlink missing."));
        return EFalse;
        }

    if (GetIntFromConfig(iSection,KTe_GuaBitrateUplink,iParams.iGuaBitrateUplink)!=1)
        {
        INFO_PRINTF1(_L("Max bitrate downlink missing."));
        return EFalse;
        }

    if (GetIntFromConfig(iSection,KTe_GuaBitrateDownlink,iParams.iGuaBitrateDownlink)!=1)
        {
        INFO_PRINTF1(_L("Max bitrate downlink missing."));
        return EFalse;
        }

	return ETrue;

	}

// Initialise ExtMBMSParameter Set

CinitialiseExtMbmsParameterSetStep::CinitialiseExtMbmsParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KinitialiseExtMbmsParameterSetStep);
	}

TInt CinitialiseExtMbmsParameterSetStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iSetName.Set(KNullDesC);
	iBundleName.Set(KNullDesC);
	iFamilyId = 0;
	
    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection,KTe_Name,iSetName)!=1
        || iSetName.Length()==0)
            return KErrNotFound;
    
    if (GetIntFromConfig(iSection,KTe_ParamsFamilyType,iFamilyId)!=1)
        {
        INFO_PRINTF1(_L("Family id missing."));
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iBundleName)!=1
        || iBundleName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }

    //no need to clear (after previous runs) as all parameters must be read
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,KTe_Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iType = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iType = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iType = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }

    if(GetIntFromConfig(iSection, KTe_SessionId, reinterpret_cast<TInt&> (iSessionId)) != 1)
    	{		
    	INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SessionId);
    	}

    if(GetIntFromConfig(iSection, KTe_ServiceMode, reinterpret_cast<TInt&> (iServiceMode)) != 1)
    	{		
    	INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_ServiceMode);
    	}
    
	if(GetIntFromConfig(iSection, KTe_SessionOperationType, reinterpret_cast<TInt&> (iOperationType)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_SessionOperationType);
		}
    
    // All ok if we got this far
    return KErrNone;
    
    }

	
void CinitialiseExtMbmsParameterSetStep::ConstructMbmsL()
	{
	
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
	       {
	       INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
	       User::Leave(KErrNotFound);
	       }

	//find the family
	CSubConParameterFamily* mbmsFamily = bundle->FindFamily(iFamilyId);
	if (mbmsFamily==NULL)
	       {
	       INFO_PRINTF1(_L("Family not found in the bundle."));
	       User::Leave(KErrNotFound);

	       }
		
	ConnectionServ::CSubConMBMSExtensionParamSet *mbmsEx= ConnectionServ::CSubConMBMSExtensionParamSet::NewL(*mbmsFamily, iType);
	mbmsEx->SetSessionId(iSessionId); 
	mbmsEx->SetServiceMode(reinterpret_cast<TMbmsServiceMode&>(iServiceMode));
	mbmsEx->SetOperationType(reinterpret_cast<ConnectionServ::CSubConMBMSExtensionParamSet::TOperationType&> (iOperationType));
	
	}

TVerdict CinitialiseExtMbmsParameterSetStep::doSingleTestStep()
	{
	TRAPD(error, ConstructMbmsL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise MBMS Extension set (%d)"),error);
        return EFail;
        }
	return EPass;
	}


// Initialise Parameter Set (ExtSBLP)
//-----------------------------------
TExtensionSetSblpParams::~TExtensionSetSblpParams()
    {
    iFlowNames.Reset();
    }

CinitialiseExtSblpParameterSetStep::CinitialiseExtSblpParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KinitialiseExtSblpParameterSetStep);
	}

TInt CinitialiseExtSblpParameterSetStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iSetName.Set(KNullDesC);
	iBundleName.Set(KNullDesC);
	iFamilyId = 0;

    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection,KTe_Name,iSetName)!=1
        || iSetName.Length()==0)
            return KErrNotFound;
    
    if (GetIntFromConfig(iSection,KTe_ParamsFamilyType,iFamilyId)!=1)
        {
        INFO_PRINTF1(_L("Family id missing."));
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iBundleName)!=1
        || iBundleName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }

    //no need to clear (after previous runs) as all parameters must be read
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,KTe_Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iParams.iType = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iParams.iType = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iParams.iType = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }
    
    TPtrC authToken;
    if (GetStringFromConfig(iSection,KTe_AuthorisationToken,authToken)!=1
        || authToken.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }
    iParams.iAuthorisationToken.Copy(authToken);

    TPtrC flowName;
    TBool continueSearch = ETrue;
    for (TInt i = 0; continueSearch; i++)
    {
        TBuf<KMaxTestExecuteNameLength> fieldName(KTe_SubConExtensionSblpFlowName);
	    fieldName.AppendNum(i);

        if (GetStringFromConfig(iSection,fieldName,flowName)!=1
            || flowName.Length()==0)
            {
            continueSearch = EFalse;
            }
        else
            {
            iParams.iFlowNames.Append(flowName);
            INFO_PRINTF3(_L("Added flow (%S) to set (%S)."),&flowName,&iSetName);
            }
    }
    if (iParams.iFlowNames.Count()==0)
        {
        INFO_PRINTF1(_L("No flow found for this set."));
        return KErrNotFound;
        }

    return KErrNone;
    }

void CinitialiseExtSblpParameterSetStep::ConstructExtensionSetL()
	{
	/*
	  Set extension Sblp parameters.Sblp parameters contain Media Authorisation Token
	  and Flow Id's.Those informations are read from corresponding ini files.
	*/
    //find bundle
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//find family
	CSubConParameterFamily* family = bundle->FindFamily(iFamilyId);
	if (family==NULL)
        {
        INFO_PRINTF1(_L("Family not found in the bundle."));
        User::Leave(KErrNotFound);
        }

   	//create set
   	CSubConSBLPR5ExtensionParamSet* extSBLPR5ParSet = CSubConSBLPR5ExtensionParamSet::NewL(*family,iParams.iType);
    
    //set params
    extSBLPR5ParSet->SetMAT(iParams.iAuthorisationToken);

    //find and add flows
    for (TInt i = 0; i < iParams.iFlowNames.Count(); i++)
        {
        TFlowId* flow = iEsockTest->FindTFlowId(iParams.iFlowNames[i]);
	    if (flow==NULL)
            {
            INFO_PRINTF1(_L("Flow not found in repository."));
            User::Leave(KErrNotFound);
            }

	    extSBLPR5ParSet->AddFlowIdL(*flow);
        }
    }

TVerdict CinitialiseExtSblpParameterSetStep::doSingleTestStep()
	{
	/*Set Sblp extension parameters.Sblp extension parameters contain
	  Authorisation Token Name and FlowId's.*/

	TRAPD(error, ConstructExtensionSetL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise Sblp extension set (%d)"),error);
        return EFail;
        }
	return EPass;
	}


// Initialise Parameter Set (GenQoS)
//----------------------------------
CinitialiseGenQosParameterSetStep::CinitialiseGenQosParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KinitialiseGenQosParameterSetStep);
	}

TInt CinitialiseGenQosParameterSetStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iSetName.Set(KNullDesC);
	iBundleName.Set(KNullDesC);
	iFamilyId = 0;
	
    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection,KTe_Name,iSetName)!=1
        || iSetName.Length()==0)
            return KErrNotFound;
    
    if (GetIntFromConfig(iSection,KTe_ParamsFamilyType,iFamilyId)!=1)
        {
        INFO_PRINTF1(_L("Family id missing."));
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iBundleName)!=1
        || iBundleName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }

    //no need to clear (after previous runs) as all parameters must be read
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,KTe_Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iParams.iType = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iParams.iType = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iParams.iType = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }

    if (GetBoolFromConfig(iSection,KTe_HeaderMode,iParams.iHeaderMode)!=1)
        {
        INFO_PRINTF1(_L("Generic set header mode missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_DownlinkBandwidth);
    if (GetIntFromConfig(iSection,KTe_DownlinkBandwidth,iParams.iDownlinkBandwidth)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink bandwidth missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_UplinkBandwidth);
    if (GetIntFromConfig(iSection,KTe_UplinkBandwidth,iParams.iUplinkBandwidth)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink bandwidth missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_DownLinkMaximumBurstSize);
    if (GetIntFromConfig(iSection,KTe_DownLinkMaximumBurstSize,iParams.iDownLinkMaximumBurstSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink max burst size missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_UpLinkMaximumBurstSize);
    if (GetIntFromConfig(iSection,KTe_UpLinkMaximumBurstSize,iParams.iUpLinkMaximumBurstSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink max burst size missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_DownLinkAveragePacketSize);
    if (GetIntFromConfig(iSection,KTe_DownLinkAveragePacketSize,iParams.iDownLinkAveragePacketSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink av packet size missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_UpLinkAveragePacketSize);
    if (GetIntFromConfig(iSection,KTe_UpLinkAveragePacketSize,iParams.iUpLinkAveragePacketSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink av packet size missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_DownLinkMaximumPacketSize);
    if (GetIntFromConfig(iSection,KTe_DownLinkMaximumPacketSize,iParams.iDownLinkMaximumPacketSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink max packet size missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_UpLinkMaximumPacketSize);
    if (GetIntFromConfig(iSection,KTe_UpLinkMaximumPacketSize,iParams.iUpLinkMaximumPacketSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink max packet size missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_DownLinkDelay);
    if (GetIntFromConfig(iSection,KTe_DownLinkDelay,iParams.iDownLinkDelay)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink delay missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_UpLinkDelay);
    if (GetIntFromConfig(iSection,KTe_UpLinkDelay,iParams.iUpLinkDelay)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink delay missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_DownLinkDelayVariation);
    if (GetIntFromConfig(iSection,KTe_DownLinkDelayVariation,iParams.iDownLinkDelayVariation)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink delay variation missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_UpLinkDelayVariation);
    if (GetIntFromConfig(iSection,KTe_UpLinkDelayVariation,iParams.iUpLinkDelayVariation)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink delay variation missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_DownLinkPriority);
    if (GetIntFromConfig(iSection,KTe_DownLinkPriority,iParams.iDownLinkPriority)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink priority missing."));
        return KErrNotFound;
        }

    //fieldName.Copy(KTe_SubConGenericParamsSetName);
    //fieldName.Append(KTe_UpLinkPriority);
    if (GetIntFromConfig(iSection,KTe_UpLinkPriority,iParams.iUpLinkPriority)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink priority missing."));
        return KErrNotFound;
        }

	return KErrNone;
    }

void CinitialiseGenQosParameterSetStep::ConstructGenericSetL()
	{
    //find bundle
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//find family
	CSubConParameterFamily* family = bundle->FindFamily(iFamilyId);
	if (family==NULL)
        {
        INFO_PRINTF1(_L("Family not found in repository."));
        User::Leave(KErrNotFound);
        }

    //construct the generic set when we have all parameters available
    CSubConQosGenericParamSet* subConQosGenParSet = CSubConQosGenericParamSet::NewL(*family,iParams.iType);
    subConQosGenParSet->SetName(iSetName);
    subConQosGenParSet->SetHeaderMode(iParams.iHeaderMode);
    subConQosGenParSet->SetDownlinkBandwidth(iParams.iDownlinkBandwidth);
    subConQosGenParSet->SetUplinkBandwidth(iParams.iUplinkBandwidth);
    subConQosGenParSet->SetDownLinkMaximumBurstSize(iParams.iDownLinkMaximumBurstSize);
    subConQosGenParSet->SetUpLinkMaximumBurstSize(iParams.iUpLinkMaximumBurstSize);
    subConQosGenParSet->SetDownLinkAveragePacketSize(iParams.iDownLinkAveragePacketSize);
    subConQosGenParSet->SetUpLinkAveragePacketSize(iParams.iUpLinkAveragePacketSize);
    subConQosGenParSet->SetDownLinkMaximumPacketSize(iParams.iDownLinkMaximumPacketSize);
    subConQosGenParSet->SetUpLinkMaximumPacketSize(iParams.iUpLinkMaximumPacketSize);
    subConQosGenParSet->SetDownLinkDelay(iParams.iDownLinkDelay);
    subConQosGenParSet->SetUpLinkDelay(iParams.iUpLinkDelay);
    subConQosGenParSet->SetDownLinkDelayVariation(iParams.iDownLinkDelayVariation);
    subConQosGenParSet->SetUpLinkDelayVariation(iParams.iUpLinkDelayVariation);
    subConQosGenParSet->SetDownLinkPriority(iParams.iDownLinkPriority);
    subConQosGenParSet->SetUpLinkPriority(iParams.iUpLinkPriority);
	}

TVerdict CinitialiseGenQosParameterSetStep::doSingleTestStep()
	{
	TRAPD(error,ConstructGenericSetL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise subcon parameters (%d)"),error);
        return EFail;
        }
	return EPass;
	}


// Initialise Parameter Set (GenSBLP)
//-----------------------------------
CinitialiseGenSblpParameterSetStep::CinitialiseGenSblpParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KinitialiseGenSblpParameterSetStep);
	}

TInt CinitialiseGenSblpParameterSetStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iSetName.Set(KNullDesC);
	iBundleName.Set(KNullDesC);
	iFamilyId = 0;
	
    if (GetStringFromConfig(iSection,KTe_Name,iSetName)!=1
        || iSetName.Length()==0)
            return KErrNotFound;
    
    if (GetIntFromConfig(iSection,KTe_ParamsFamilyType,iFamilyId)!=1)
        {
        INFO_PRINTF1(_L("Family id missing."));
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iBundleName)!=1
        || iBundleName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }

    //no need to clear (after previous runs) as all parameters must be read
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,KTe_Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iParams.iType = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iParams.iType = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iParams.iType = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }

    if (GetBoolFromConfig(iSection,KTe_HeaderMode,iParams.iHeaderMode)!=1)
        {
        INFO_PRINTF1(_L("Generic set header mode missing."));
        return KErrNotFound;
        }

	return KErrNone;
    }

void CinitialiseGenSblpParameterSetStep::ConstructGenericSetL()
	{
    //find bundle
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//find family
	CSubConParameterFamily* family = bundle->FindFamily(iFamilyId);
	if (family==NULL)
        {
        INFO_PRINTF1(_L("Family not found in repository."));
        User::Leave(KErrNotFound);
        }

    //construct the generic set when we have all parameters available
    CSubConAuthorisationGenericParamSet* subConQosGenParSet = CSubConAuthorisationGenericParamSet::NewL(*family,iParams.iType);
    
    //set all other params here
    const TInt id = 32;
	
	subConQosGenParSet->SetId(id);
	
	TInt val = subConQosGenParSet->GetId();
	
	if (id != val)
		User::Leave(KErrNotFound);
	}

TVerdict CinitialiseGenSblpParameterSetStep::doSingleTestStep()
	{
	TRAPD(error,ConstructGenericSetL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise subcon parameters (%d)"),error);
        return EFail;
        }
	return EPass;
	}

// Initialise MBMS Parameter Set
CinitialiseGenMbmsParameterStep::CinitialiseGenMbmsParameterStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KinitialiseGenMbmsParameterSetStep);
	}

TInt CinitialiseGenMbmsParameterStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iSetName.Set(KNullDesC);
	iBundleName.Set(KNullDesC);
	iFamilyId = 0;
	
    //try reading the name of the subconnection params (bundle)
    if (GetStringFromConfig(iSection,KTe_Name,iSetName)!=1
        || iSetName.Length()==0)
            return KErrNotFound;
    
    if (GetIntFromConfig(iSection,KTe_ParamsFamilyType,iFamilyId)!=1)
        {
        INFO_PRINTF1(_L("Family id missing."));
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iBundleName)!=1
        || iBundleName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }

    //no need to clear (after previous runs) as all parameters must be read
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,KTe_Type,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return KErrNotFound;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iType = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iType = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iType = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF2(_L("Family type (%S) not recognised."),&paramSetTypeName);
        return KErrNotFound;
        }

	if(GetIntFromConfig(iSection, KTe_AccessBearer, reinterpret_cast<TInt&> (iAccessBearer)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_AccessBearer);
		}

	if(GetIntFromConfig(iSection, KTe_ServicePrty, reinterpret_cast<TInt&> (iServicePrty)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_ServicePrty);
		}

	if(GetIntFromConfig(iSection, KTe_MCC, reinterpret_cast<TInt&> (iMCC)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_MCC);
		}

	if(GetIntFromConfig(iSection, KTe_MNC, reinterpret_cast<TInt&> (iMNC)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_MNC);
		}
	
	if(GetIntFromConfig(iSection, KTe_ServiceId, reinterpret_cast<TInt&> (iServiceId)) != 1)
		{		
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_ServiceId);
		}
	
	iIsMultiConnection = EFalse;
	GetBoolFromConfig(iSection, KTe_MultiConnection, iIsMultiConnection);
	
	iIsDeactivationReqd = EFalse;
	GetBoolFromConfig(iSection, KTe_DeactivationReqd, iIsDeactivationReqd);
	
	iIsActivationSuccess = EFalse;
	GetBoolFromConfig(iSection, KTe_ActivationSuccess, iIsActivationSuccess);
	
    return KErrNone;
    }

void CinitialiseGenMbmsParameterStep::ConstructGenericMbmsSetL()
	{
    //find bundle
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//find family
	CSubConParameterFamily* mbmsFamily = bundle->FindFamily(iFamilyId);
	if (mbmsFamily==NULL)
        {
        INFO_PRINTF1(_L("Family not found in repository."));
        User::Leave(KErrNotFound);
        }

    //construct the generic set when we have all parameters available
	ConnectionServ::CSubConChannelParamSet* reqMBMSParams = ConnectionServ::CSubConChannelParamSet::NewL(*mbmsFamily , iType);

	//The MBMS Service is identified by TMGI, Session ID, Bearer Type
	TTmgi tmgi;
	tmgi.SetMCC(iMCC);
	tmgi.SetMNC(iMNC);
	tmgi.SetServiceId(iServiceId);

	TMbmsScope mbmsScope;
	ConnectionServ::TMBMSChannelInfoV1* ptr=reinterpret_cast <ConnectionServ::TMBMSChannelInfoV1*>(reqMBMSParams->GetChannelInfo());
	mbmsScope=reinterpret_cast<TMbmsScope&> (iAccessBearer);
	
	ptr->SetTmgi(tmgi);
	ptr->SetScope(mbmsScope);
	ptr->SetServicePriority(reinterpret_cast<TMbmsServicePriority&>(iServicePrty));

	}

TVerdict CinitialiseGenMbmsParameterStep::doSingleTestStep()
	{
	TRAPD(error,ConstructGenericMbmsSetL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise subcon parameters (%d)"),error);
        return EFail;
        }
	return EPass;
	}



// Initialise Parameter Family
//----------------------------
CInitialiseRSubConParameterFamilyStep::CInitialiseRSubConParameterFamilyStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KInitialiseRSubConParameterFamilyStep);
	}

TBool CInitialiseRSubConParameterFamilyStep::ReadGenSetDescription()
    {
    //no need to clear (after previous runs) as all parameters must be read

    TBuf<KMaxTestExecuteNameLength> fieldName(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_Type);
    TPtrC paramSetTypeName;
    if (GetStringFromConfig(iSection,fieldName,paramSetTypeName)!=1
        || paramSetTypeName.Length() == 0)
        return EFalse;

    if (paramSetTypeName.Compare(KTe_ParamsTypeRequested)==0)
	    { iGenSetParams.iType = CSubConParameterFamily::ERequested; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeAcceptable)==0)
	    { iGenSetParams.iType = CSubConParameterFamily::EAcceptable; }
    else if (paramSetTypeName.Compare(KTe_ParamsTypeGranted)==0)
	    { iGenSetParams.iType = CSubConParameterFamily::EGranted; }
    else
        {
        INFO_PRINTF1(_L("Family type not recognised!"));
        return EFalse;
        }

	//try reading generic parameters
    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_Name);
    if (GetStringFromConfig(iSection,fieldName,iGenSetParams.iName)!=1)
        {
        INFO_PRINTF1(_L("Generic set name missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_HeaderMode);
    if (GetBoolFromConfig(iSection,fieldName,iGenSetParams.iHeaderMode)!=1)
        {
        INFO_PRINTF1(_L("Generic set header mode missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_DownlinkBandwidth);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iDownlinkBandwidth)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink bandwidth missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_UplinkBandwidth);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iUplinkBandwidth)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink bandwidth missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_DownLinkMaximumBurstSize);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iDownLinkMaximumBurstSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink max burst size missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_UpLinkMaximumBurstSize);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iUpLinkMaximumBurstSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink max burst size missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_DownLinkAveragePacketSize);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iDownLinkAveragePacketSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink av packet size missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_UpLinkAveragePacketSize);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iUpLinkAveragePacketSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink av packet size missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_DownLinkMaximumPacketSize);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iDownLinkMaximumPacketSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink max packet size missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_UpLinkMaximumPacketSize);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iUpLinkMaximumPacketSize)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink max packet size missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_DownLinkDelay);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iDownLinkDelay)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink delay missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_UpLinkDelay);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iUpLinkDelay)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink delay missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_DownLinkDelayVariation);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iDownLinkDelayVariation)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink delay variation missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_UpLinkDelayVariation);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iUpLinkDelayVariation)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink delay variation missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_DownLinkPriority);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iDownLinkPriority)!=1)
        {
        INFO_PRINTF1(_L("Generic set downlink priority missing."));
        return EFalse;
        }

    fieldName.Copy(KTe_SubConGenericParamsSetName);
    fieldName.Append(KTe_UpLinkPriority);
    if (GetIntFromConfig(iSection,fieldName,iGenSetParams.iUpLinkPriority)!=1)
        {
        INFO_PRINTF1(_L("Generic set uplink priority missing."));
        return EFalse;
        }
	
	return ETrue;    
    }

TInt CInitialiseRSubConParameterFamilyStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the name of the subconnection params (bundle)
    if (GetIntFromConfig(iSection,KTe_Type,iFamilyId)!=1)
        return KErrNotFound;
            
    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iParams.iSubConnectionParamsName)!=1
        || iParams.iSubConnectionParamsName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }

    return ReadGenSetDescription() ? KErrNone : KErrNotFound;
    }

void CInitialiseRSubConParameterFamilyStep::ConstructFamilyFromIniL()
	{
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iParams.iSubConnectionParamsName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//create a container for QoS sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* family = CSubConParameterFamily::NewL(*bundle, iFamilyId);

	//create the generic set
	ConstructGenericSetL(*family); //we do not expect to leave here
	}

void CInitialiseRSubConParameterFamilyStep::ConstructGenericSetL(CSubConParameterFamily& aFamily)
	{
    //construct the generic set when we have all parameters available
    CSubConQosGenericParamSet* subConQosGenParSet = CSubConQosGenericParamSet::NewL(aFamily,iGenSetParams.iType);
    subConQosGenParSet->SetName(iGenSetParams.iName);
    subConQosGenParSet->SetHeaderMode(iGenSetParams.iHeaderMode);
    subConQosGenParSet->SetDownlinkBandwidth(iGenSetParams.iDownlinkBandwidth);
    subConQosGenParSet->SetUplinkBandwidth(iGenSetParams.iUplinkBandwidth);
    subConQosGenParSet->SetDownLinkMaximumBurstSize(iGenSetParams.iDownLinkMaximumBurstSize);
    subConQosGenParSet->SetUpLinkMaximumBurstSize(iGenSetParams.iUpLinkMaximumBurstSize);
    subConQosGenParSet->SetDownLinkAveragePacketSize(iGenSetParams.iDownLinkAveragePacketSize);
    subConQosGenParSet->SetUpLinkAveragePacketSize(iGenSetParams.iUpLinkAveragePacketSize);
    subConQosGenParSet->SetDownLinkMaximumPacketSize(iGenSetParams.iDownLinkMaximumPacketSize);
    subConQosGenParSet->SetUpLinkMaximumPacketSize(iGenSetParams.iUpLinkMaximumPacketSize);
    subConQosGenParSet->SetDownLinkDelay(iGenSetParams.iDownLinkDelay);
    subConQosGenParSet->SetUpLinkDelay(iGenSetParams.iUpLinkDelay);
    subConQosGenParSet->SetDownLinkDelayVariation(iGenSetParams.iDownLinkDelayVariation);
    subConQosGenParSet->SetUpLinkDelayVariation(iGenSetParams.iUpLinkDelayVariation);
    subConQosGenParSet->SetDownLinkPriority(iGenSetParams.iDownLinkPriority);
    subConQosGenParSet->SetUpLinkPriority(iGenSetParams.iUpLinkPriority);
	}

void CInitialiseRSubConParameterFamilyStep::ConstructExtensionSetL(const TDesC& /*aFieldName*/, CSubConParameterFamily& /*aFamily*/)
	{
	//not yet supported
	}

TVerdict CInitialiseRSubConParameterFamilyStep::doSingleTestStep()
	{
	TRAPD(error,ConstructFamilyFromIniL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise subcon parameters (%d)"),error);
        return EFail;
        }
	return EPass;
	}
	

// Set SBLP Flow ID
//-----------------
CSetSBLPFlowIdStep ::CSetSBLPFlowIdStep (CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KSetSBLPFlowIdStep);
	}

TInt CSetSBLPFlowIdStep::ConfigureFromIni()
    {
    /*
      Read connection details from corresponding ini files 
    */
	iFlowExtName.Set(KNullDesC);
	
    if (GetStringFromConfig(iSection, KTe_SubConnectionSblpExtFlowName, iFlowExtName)!=1
        || iFlowExtName.Length()==0)
            return KErrNotFound;

    if (GetIntFromConfig(iSection,KTe_SubConExtSblpMediaComponent,iMediaComponent)!=1)
        {
        INFO_PRINTF2(_L("Media component missing for flow id %S"),&iFlowExtName);
        return KErrNotFound;
        }

    if (GetIntFromConfig(iSection,KTe_SubConExtensionSblpIPFlowNumber,iIPFlowNumber)!=1)
        {
        INFO_PRINTF2(_L("IP flow number missing for flow id %S"),&iFlowExtName);
        return KErrNotFound;
        }

    return KErrNone;
    }

TVerdict CSetSBLPFlowIdStep ::doSingleTestStep()
	{
     /* Creates FlowId using Media Component Number and Flow Number read from ini file
 		A FlowId contains a Media Component Number and a Flow Number.	       
     */ 
        
    //find and set params
    TFlowId* fid = iEsockTest->FindTFlowId(iFlowExtName);
   	if(fid==NULL)
   		{
   		INFO_PRINTF2(_L("Could not find Flow Id for sblp events (%S)."),&iFlowExtName);
   		return EFail;
        }
    TUint16 val = iMediaComponent;
    fid->SetMediaComponentNumber(val);
    
    val = iIPFlowNumber;
    fid->SetIPFlowNumber(val);
    
    return EPass;
	}


// R5
//---

CinitialiseExtIPLinkR5ParameterSetStep::CinitialiseExtIPLinkR5ParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CinitialiseExtIPLink99ParameterSetStep(aEsockTest)
	{
	SetTestStepName(KinitialiseExtIPLinkR5ParameterSetStep);
	}

TInt CinitialiseExtIPLinkR5ParameterSetStep::ConfigureFromIni()
    {
	//parse R5 parameters from ini files
    if (ParseIniIPLinkParameters() == EFalse)
    	{
    	return KErrNotFound;
    	}
    
    if (GetBoolFromConfig(iSection, KTe_SignallingIndicator, iLinkR5Params.aSignallingIndicator)!= 1)
        {
        INFO_PRINTF1(_L("Signalling Indicator missing."));
        return KErrNotFound;
        }
     
    TPtrC paramSetSrcStatisticsDescriptor;    
	if (GetStringFromConfig(iSection, KTe_SrcStatisticsDescriptor, paramSetSrcStatisticsDescriptor)!= 1)
        {
        INFO_PRINTF1(_L("Source Statistics Descriptor missing."));
        return KErrNotFound;
        }
        
    if (paramSetSrcStatisticsDescriptor.Compare(KTe_SubConIPLinkR5SrcStatisticDescUnknown)==0)
	   { 
	   iLinkR5Params.iSrcStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown; 
	   }
	else if(paramSetSrcStatisticsDescriptor.Compare(KTe_SubConIPLinkR5SrcStatisticDescSpeech)==0)
	   { 
	   iLinkR5Params.iSrcStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorSpeech; 
	   }
    else
       {
       INFO_PRINTF1(_L("Source Statistics Descriptor not recognised."));
	   return KErrNotFound;
       }
	return KErrNone;
    }

void CinitialiseExtIPLinkR5ParameterSetStep::ConstructIPLinkR5SetL()
	{
	//get the RSubConnection parameter bundle.
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//find the family
	CSubConParameterFamily* family = bundle->FindFamily(iFamilyId);
	if (family==NULL)
        {
        INFO_PRINTF1(_L("Family not found in the bundle."));
        User::Leave(KErrNotFound);
        }
	//set the R5 Qos parameters if all parameters are available	
	CSubConQosR5ParamSet* ipLinkR5ParmSet = CSubConQosR5ParamSet::NewL(*family,iParams.iType);
	InitialiseR5Params (ipLinkR5ParmSet);
	
	}
void CinitialiseExtIPLinkR5ParameterSetStep::InitialiseR5Params(CSubConQosR5ParamSet* ipLinkR5ParmSet)
	{
	InitialiseR99Params(ipLinkR5ParmSet);
	ipLinkR5ParmSet->SetSignallingIndicator(iLinkR5Params.aSignallingIndicator);
	ipLinkR5ParmSet->SetSourceStatisticsDescriptor(iLinkR5Params.iSrcStatisticsDescriptor);
	}

TVerdict CinitialiseExtIPLinkR5ParameterSetStep::doSingleTestStep()
	{
	TRAPD(error, ConstructIPLinkR5SetL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise IPLinkR5 set (%d)"),error);
        return EFail;
        }
      
	return EPass;
	}

CCompareExtIPLinkR5ParameterSetStep::CCompareExtIPLinkR5ParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CCompareExtIPLink99ParameterSetStep(aEsockTest)
	{
	SetTestStepName(KCompareExtIPLinkR5ParameterSetStep);
	}

TInt CCompareExtIPLinkR5ParameterSetStep::ConfigureFromIni()
    {
    //clean parameters from previous runs
	iFamily1Id = 0;
	iFamily2Id = 0;
	iParams1Name.Set(KNullDesC);
	iParams2Name.Set(KNullDesC);

    return ParseCompareDetails();
    }

TBool CCompareExtIPLinkR5ParameterSetStep::CompareR5Params(CSubConQosR5ParamSet& ps1,CSubConQosR5ParamSet& ps2)
	{
	
	TBool match = ETrue;
	
	match = CompareR4Params(ps1,ps2);
	
	INFO_PRINTF3(_L("Comparing CSubConQosIPLinkR5ParamSets (%S vs %S)."),&iParams1Name,&iParams2Name);
	if (ps1.GetSignallingIndicator() != ps2.GetSignallingIndicator())
	    {
	    INFO_PRINTF5(_L("Signalling Indicator differs! %S: %d %S: %d"),&iParams1Name, ps1.GetSignallingIndicator(),&iParams2Name, ps2.GetSignallingIndicator());
	    match = EFalse;
	    }
	if (ps1.GetSourceStatisticsDescriptor() != ps2.GetSourceStatisticsDescriptor())
	    {
	    INFO_PRINTF5(_L("Source Statistics Descriptor  differs! %S: %d %S: %d"),&iParams1Name, ps1.GetSourceStatisticsDescriptor(), &iParams2Name, ps2.GetSourceStatisticsDescriptor());
	    match = EFalse;
	    }
	
	if (match)
		{
		INFO_PRINTF1(_L("R5 Qos Param sets are identical."));	
		}
    	
	return match;
	}

TVerdict CCompareExtIPLinkR5ParameterSetStep::doSingleTestStep()
	{
    CSubConParameterFamily* f1 = iEsockTest->FindRSubConnectionParameterFamily(iParams1Name,iFamily1Id);
    if (f1==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters family (%S) not found"),&iParams1Name);
        return EFail;
        }
    CSubConQosR5ParamSet* ps1 = static_cast<CSubConQosR5ParamSet*>(f1->FindExtensionSet(STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConQosR5ParamsType ), iFamily1Type));
    if (ps1==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters (%S) not found"),&iParams1Name);
        return EFail;
        }

    CSubConParameterFamily* f2 = iEsockTest->FindRSubConnectionParameterFamily(iParams2Name,iFamily2Id);
    if (f2==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters family (%S) not found"),&iParams2Name);
        return EFail;
        }
    CSubConQosR5ParamSet* ps2 = static_cast<CSubConQosR5ParamSet*>(f2->FindExtensionSet(STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConQosR5ParamsType ), iFamily2Type));
    if (ps2==NULL)
        {
        INFO_PRINTF2(_L("Subcon parameters (%S) not found"),&iParams2Name);
        return EFail;
        }

    TBool cmp = CompareR5Params(*ps1,*ps2);
    if (cmp && iExpectedCompareResult)
        {
        INFO_PRINTF1(_L("Subcon parameters match."));
        return EPass;
        }

	if (iExpectedCompareResult)
		{
		INFO_PRINTF1(_L("Subcon parameters do not match."));
		return EFail;		
		}
	else
		{
		INFO_PRINTF1(_L("Subcon parameters do not match as expected."));
		return EPass;	
		}
	
	}

// IMS
//----

CinitialiseIMSExtParameterSetStep::CinitialiseIMSExtParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KinitialiseIMSExtParameterSetStep);
	}

TInt CinitialiseIMSExtParameterSetStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iSetName.Set(KNullDesC);
	iBundleName.Set(KNullDesC);
	iFamilyId = 0;

	//parse IMS details from ini files.
    if (GetStringFromConfig(iSection,KTe_Name,iSetName)!=1
        || iSetName.Length()==0)
            return KErrNotFound;
    
    if (GetIntFromConfig(iSection,KTe_ParamsFamilyType,iFamilyId)!=1)
        {
        INFO_PRINTF1(_L("Family id missing."));
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iBundleName)!=1
        || iBundleName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }
         
    if (GetBoolFromConfig(iSection,KTe_IMSSignallingIndicator,iIMSParams.iIMSSignallingIndicator)!=1)
        {
        INFO_PRINTF1(_L("IMS Signalling Indicator missing."));
        return KErrNotFound;
        }   
	return KErrNone;
    }

void CinitialiseIMSExtParameterSetStep::ConstructIMSParamSetL()
	{
	//get RSubConnection parameter bundle
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//find the family
	CSubConParameterFamily* family = bundle->FindFamily(iFamilyId);
	if (family==NULL)
        {
        INFO_PRINTF1(_L("Family not found in the bundle."));
        User::Leave(KErrNotFound);
        }
        
    //set the IMS parameters if all the parameters are available.
    CSubConImsExtParamSet  * ipIMSParmSet = CSubConImsExtParamSet ::NewL(*family,iIMSParams.iType);
	ipIMSParmSet->SetImsSignallingIndicator(iIMSParams.iIMSSignallingIndicator);
	}

TVerdict CinitialiseIMSExtParameterSetStep::doSingleTestStep()
	{
    TRAPD(error, ConstructIMSParamSetL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise IMS Parameter set (%d)"),error);
        return EFail;
        }
           
	return EPass;
	}

//Extension Parameter Set

// Protocol extension support for subconnections
//----------------------------------------------

CInitialiseDummyProtocolParameterSetStep::CInitialiseDummyProtocolParameterSetStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KInitialiseDummyProtocolParameterSetStep);
	}

TInt CInitialiseDummyProtocolParameterSetStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iSetName.Set(KNullDesC);
	iBundleName.Set(KNullDesC);
	iFamilyId = 0;

	//parse IMS details from ini files.
    if (GetStringFromConfig(iSection,KTe_Name,iSetName)!=1
        || iSetName.Length()==0)
            return KErrNotFound;
    
    if (GetIntFromConfig(iSection,KTe_ParamsFamilyType,iFamilyId)!=1)
        {
        INFO_PRINTF1(_L("Family id missing."));
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_SubConnectionParamsName,iBundleName)!=1
        || iBundleName.Length()==0)
        {
        INFO_PRINTF1(_L("Sub connection param bundle name missing."));
        return KErrNotFound;
        }
         
	return KErrNone;
    }

void CInitialiseDummyProtocolParameterSetStep::ConstructDummyParamSetL()
	{
	//get RSubConnection parameter bundle
	RSubConParameterBundle* bundle = iEsockTest->FindRSubConnectionParams(iBundleName);
	if (bundle==NULL)
        {
        INFO_PRINTF1(_L("RSubConnection parameter bundle not found in repository."));
        User::Leave(KErrNotFound);
        }

	//find the family
	CSubConParameterFamily* family = bundle->FindFamily(iFamilyId);
	if (family==NULL)
        {
        INFO_PRINTF1(_L("Family not found in the bundle."));
        User::Leave(KErrNotFound);
        }

	// Create the protocol parameter set
	CDummyProtocolParameterSet* dummy_protocol_parameters =
		CDummyProtocolParameterSet::NewL(*family, CSubConParameterFamily::ERequested);
	}

TVerdict CInitialiseDummyProtocolParameterSetStep::doSingleTestStep()
	{
    TRAPD(error, ConstructDummyParamSetL());
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not initialise parameter set (%d)"),error);
        return EFail;
        }
           
	return EPass;
	}

