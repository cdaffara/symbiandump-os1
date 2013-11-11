/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Qos.TestSteps.h
*/
#if (!defined QOS_TESTSTEPS_H)
#define QOS_TESTSTEPS_H

#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include <cs_subconparams.h>
#include <ip_subconparams.h>
#include <networking/mbmsparams.h>

/**
Class implementing compareextiplink99parametersetStep

@internalComponent
*/
class CCompareExtIPLink99ParameterSetStep : public CTe_EsockStepBase
	{
public:
	CCompareExtIPLink99ParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	TBool ParseCompareDetails();
	TBool CompareR4Params(CSubConQosIPLinkR99ParamSet& ps1,CSubConQosIPLinkR99ParamSet& ps2);

    TPtrC iParams1Name;
    TPtrC iParams2Name;
    TInt iFamily1Id;
	TInt iFamily2Id;
	CSubConParameterFamily::TParameterSetType iFamily1Type;
	CSubConParameterFamily::TParameterSetType iFamily2Type;
	};

_LIT(KCompareExtIPLink99ParameterSetStep,"CompareExtIPLink99ParameterSetStep");


/**
Class implementing CompareExtSBLPParameterSetStep

@internalComponent
*/
class CCompareExtSBLPParameterSetStep : public CTe_EsockStepBase
	{
public:
	CCompareExtSBLPParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	TBool Compare(CSubConSBLPR5ExtensionParamSet& ps1,CSubConSBLPR5ExtensionParamSet& ps2);

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KCompareExtSBLPParameterSetStep,"CompareExtSBLPParameterSetStep");


/**
Class implementing comparegenqosparametersetStep

@internalComponent
*/
class CCompareGenQosParameterSetStep : public CTe_EsockStepBase
	{
public:
	CCompareGenQosParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	TBool Compare(CSubConQosGenericParamSet& ps1,CSubConQosGenericParamSet& ps2);

private:
    TPtrC iParams1Name;
    TPtrC iParams2Name;
    TInt iFamily1Id;
	TInt iFamily2Id;
	CSubConParameterFamily::TParameterSetType iFamily1Type;
	CSubConParameterFamily::TParameterSetType iFamily2Type;
	};

_LIT(KCompareGenQosParameterSetStep,"CompareGenQosParameterSetStep");


/**
Class implementing comparegensblpparametersetStep

@internalComponent
*/
class CCompareGenSBLPParameterSetStep : public CTe_EsockStepBase
	{
public:
	CCompareGenSBLPParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	TBool Compare(CSubConAuthorisationGenericParamSet ps1,CSubConAuthorisationGenericParamSet ps2);

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KCompareGenSBLPParameterSetStep,"CompareGenSBLPParameterSetStep");

/**
Class implementing closersubconparameterbundleStep

@internalComponent
*/
class CCloseRSubConParameterBundleStep : public CTe_EsockStepBase
	{
public:
	CCloseRSubConParameterBundleStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KCloseRSubConParameterBundleStep,"CloseRSubConParameterBundleStep");


class TGenericSetParams
    {
public:
    TPtrC iName;
    CSubConParameterFamily::TParameterSetType iType;
    TBool iHeaderMode;
    TInt iDownlinkBandwidth;
    TInt iUplinkBandwidth;
    TInt iDownLinkMaximumBurstSize;
    TInt iUpLinkMaximumBurstSize;
    TInt iDownLinkAveragePacketSize;
    TInt iUpLinkAveragePacketSize;
    TInt iDownLinkMaximumPacketSize;
    TInt iUpLinkMaximumPacketSize;
    TInt iDownLinkDelay;
    TInt iUpLinkDelay;
    TInt iDownLinkDelayVariation;
    TInt iUpLinkDelayVariation;
    TInt iDownLinkPriority;
    TInt iUpLinkPriority;
    };

class CInitialiseRSubConParameterFamilyStep : public CTe_EsockStepBase
	{
public:
	CInitialiseRSubConParameterFamilyStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	TBool ReadGenSetDescription();
	void ConstructFamilyFromIniL();
	void ConstructExtensionSetL(const TDesC& aFieldName, CSubConParameterFamily& aFamily);
	void ConstructGenericSetL(CSubConParameterFamily& aFamily);
	
private:
    TInt iFamilyId;
    TGenericSetParams iGenSetParams;
    TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KInitialiseRSubConParameterFamilyStep,"InitialiseRSubConParameterFamilyStep");


/**
Class containing IP R99 Qos parameters

@internalComponent
*/
class TExtensionQosParamsSetIPLink99
    {
public:
    CSubConParameterFamily::TParameterSetType iType;
	RPacketQoS::TTrafficClass				iTrafficClass;			// Traffic class
	RPacketQoS::TDeliveryOrder				iDeliveryOrder;			// Delivery order
	RPacketQoS::TErroneousSDUDelivery		iDeliveryOfErroneusSdu;	// Delivery of erroneous SDUs
	RPacketQoS::TBitErrorRatio				iResidualBer;			// Residual BER
	RPacketQoS::TSDUErrorRatio				iErrorRatio;			// SDU error ratio
	RPacketQoS::TTrafficHandlingPriority	iPriority;				// Traffic handling priority
	TInt									iTransferDelay;			// Transfer delay
	TInt									iMaxSduSize;			// Maximum SDU size
	TInt									iMaxBitrateUplink;		// Maximum bit rate for uplink
	TInt									iMaxBitrateDownlink;	// Maximum bit rate for downlink
	TInt									iGuaBitrateUplink;		// Guaranteed bit rate for uplink
	TInt									iGuaBitrateDownlink;	// Guaranteed bit rate for downlink
    };

/**
Class implementing initialisation of Qos parameter family

@internalComponent
*/
class CinitialiseExtIPLink99ParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseExtIPLink99ParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	
    //current params (.ini)
    TInt iFamilyId;
    TExtensionQosParamsSetIPLink99 iParams;
    TPtrC iSetName;
    TPtrC iBundleName;

protected:
	void ConstructIPLink99SetL();
	TBool ParseIniIPLinkParameters();
	void InitialiseR99Params(CSubConQosIPLinkR99ParamSet* aParams);
	};

_LIT(KinitialiseExtIPLink99ParameterSetStep,"initialiseExtIPLink99ParameterSetStep");


/**
Class implementing initialisation of CinitialiseExtMbmsParameterSet parameter family

@internalComponent
*/
class CinitialiseExtMbmsParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseExtMbmsParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	
    //current params (.ini)
    TInt iFamilyId;
    TPtrC iSetName;
    TPtrC iBundleName;
    CSubConParameterFamily::TParameterSetType iType;
	TInt iSessionId;
	TInt iServiceMode;
	TInt iOperationType;

protected:
	void ConstructMbmsL();
	};

_LIT(KinitialiseExtMbmsParameterSetStep,"initialiseExtMbmsParameterSetStep");



/**
Class containing generic SBLP parameters

@internalComponent
*/
class TGenericSetSblpParams
    {
public:
	CSubConParameterFamily::TParameterSetType iType;
   	TBool iHeaderMode;
    };

/**
Class implementing initialiseGenSblpParameterStep

@internalComponent
*/
class CinitialiseGenSblpParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseGenSblpParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	void ConstructGenericSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TGenericSetSblpParams iParams;
    TPtrC iSetName;
    TPtrC iBundleName;
	};

_LIT(KinitialiseGenSblpParameterSetStep,"initialiseGenSblpParameterSetStep");

/**
Class implementing initialiseGenMbmsParameterStep

@internalComponent
*/
class CinitialiseGenMbmsParameterStep : public CTe_EsockStepBase
	{
public:
	CinitialiseGenMbmsParameterStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	void ConstructGenericMbmsSetL();

private:
    //current params (.ini)
	CSubConParameterFamily::TParameterSetType iType;
    TInt iFamilyId;
    TPtrC iSetName;
    TPtrC iBundleName;
	TBool iIsMultiConnection;
	TBool iIsDeactivationReqd;
	TBool iIsActivationSuccess;
	
	TUint iNumTmgis;
	TUint iMCC;
	TUint iMNC;
	TUint iServiceId;
	TInt iAccessBearer;
	TInt iServicePrty;
	};

_LIT(KinitialiseGenMbmsParameterSetStep,"initialiseGenMbmsParameterSetStep");

/**
Class containing generic sub connection parameters

@internalComponent
*/
class TGenericQosParamsSet
    {
public:
    CSubConParameterFamily::TParameterSetType iType;
    TBool iHeaderMode;
    TInt iDownlinkBandwidth;
    TInt iUplinkBandwidth;
    TInt iDownLinkMaximumBurstSize;
    TInt iUpLinkMaximumBurstSize;
    TInt iDownLinkAveragePacketSize;
    TInt iUpLinkAveragePacketSize;
    TInt iDownLinkMaximumPacketSize;
    TInt iUpLinkMaximumPacketSize;
    TInt iDownLinkDelay;
    TInt iUpLinkDelay;
    TInt iDownLinkDelayVariation;
    TInt iUpLinkDelayVariation;
    TInt iDownLinkPriority;
    TInt iUpLinkPriority;
    };

/**
Class implementing initialiseGenQosParameterSetStep

@internalComponent
*/
class CinitialiseGenQosParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseGenQosParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	void ConstructGenericSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TGenericQosParamsSet iParams;
    TPtrC iSetName;
    TPtrC iBundleName;
	};

_LIT(KinitialiseGenQosParameterSetStep,"initialiseGenQosParameterSetStep");


/**
Class containing extension SBLP parameters

@internalComponent
*/
class TExtensionSetSblpParams
    {
public:
    ~TExtensionSetSblpParams();
    
	CSubConParameterFamily::TParameterSetType iType;
	TAuthToken iAuthorisationToken;
	RArray<TPtrC> iFlowNames;
    };

/**
Class implementing initialiseExtSblpParameterSetStep

@internalComponent
*/
class CinitialiseExtSblpParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseExtSblpParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	void ConstructExtensionSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TExtensionSetSblpParams iParams;
    TPtrC iSetName;
    TPtrC iBundleName;
    };

_LIT(KinitialiseExtSblpParameterSetStep,"initialiseExtSblpParameterSetStep");

/**
Class implementing creatersubconparameterbundleStep

@internalComponent
*/
class CCreateRSubConParameterBundleStep : public CTe_EsockStepBase
	{
public:
	CCreateRSubConParameterBundleStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	void ConstructParametersFromIniL();

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KCreateRSubConParameterBundleStep,"CreateRSubConParameterBundleStep");


/**
Class implementing creatersubconparameterfamilyStep

@internalComponent
*/
class CCreateRSubConParameterFamilyStep : public CTe_EsockStepBase
	{
public:
	CCreateRSubConParameterFamilyStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	TBool ReadGenSetDescription();
	void ConstructFamilyFromIniL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TPtrC iSubConnectionParamsName;
	};

_LIT(KCreateRSubConParameterFamilyStep,"CreateRSubConParameterFamilyStep");


/**
Class implementing createsblpflowidStep

@internalComponent
*/
class CCreateSBLPFlowIdStep : public CTe_EsockStepBase
	{
public:
	CCreateSBLPFlowIdStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	
private:
	TPtrC iFlowExtName; //current params (.ini)
	};

_LIT(KCreateSBLPFlowIdStep,"CreateSBLPFlowIdStep");


/**
Class implementing GetRSubConParametersStep

@internalComponent
*/
class CGetRSubConParametersStep : public CTe_EsockStepBase
	{
public:
	CGetRSubConParametersStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams; //current params
	};

_LIT(KGetRSubConParametersStep,"GetRSubConParametersStep");


/**
Class implementing SetRSubConParametersStep

@internalComponent
*/
class CSetRSubConParametersStep : public CTe_EsockStepBase
	{
public:
	CSetRSubConParametersStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KSetRSubConParametersStep,"SetRSubConParametersStep");


/**
Class implementing SetSBLPFlowIdStep

@internalComponent
*/
class CSetSBLPFlowIdStep : public CTe_EsockStepBase
	{
public:
	CSetSBLPFlowIdStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
private:
    //current params (.ini)
    TPtrC iFlowExtName;
  	TInt iMediaComponent;
	TInt iIPFlowNumber;
	};

_LIT(KSetSBLPFlowIdStep,"SetSBLPFlowIdStep");



/**
Class implementing initialisation of Qos R5 parameter family

@internalComponent
*/
class CinitialiseExtIPLinkR5ParameterSetStep : public CinitialiseExtIPLink99ParameterSetStep
	{
public:
	CinitialiseExtIPLinkR5ParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	void ConstructIPLinkR5SetL();
	void InitialiseR5Params(CSubConQosR5ParamSet* ipLinkR5ParmSet);

private:
    
    TExtensionQosParamsSetIPLinkR5 iLinkR5Params;
	};

_LIT(KinitialiseExtIPLinkR5ParameterSetStep,"initialiseExtIPLinkR5ParameterSetStep");


/**
Class implementing compareextiplink5parametersetStep

@internalComponent
*/
class CCompareExtIPLinkR5ParameterSetStep : public CCompareExtIPLink99ParameterSetStep
	{
public:
	CCompareExtIPLinkR5ParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	TBool CompareR5Params(CSubConQosR5ParamSet& ps1,CSubConQosR5ParamSet& ps2);
	};

_LIT(KCompareExtIPLinkR5ParameterSetStep,"compareextiplinkr5parametersetStep");


/**
Class implementing initialisation of IMS parameter family

@internalComponent
*/
class CinitialiseIMSExtParameterSetStep : public CTe_EsockStepBase
	{
public:
	CinitialiseIMSExtParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	void ConstructIMSParamSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TPtrC iSetName;
    TPtrC iBundleName;
    TExtensionIMSParamsSet iIMSParams;
	};

_LIT(KinitialiseIMSExtParameterSetStep,"initialiseIMSExtParameterSetStep");



// Protocol extension support for subconnections
//----------------------------------------------

/**
Class implementing initialisation of dummy protocol parameter set (generic and extension sets)

@internalComponent
*/
#include <comms-infras/ss_protocolparameterset.h>

class CInitialiseDummyProtocolParameterSetStep : public CTe_EsockStepBase
	{
public:
	CInitialiseDummyProtocolParameterSetStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

protected:
	void ConstructDummyParamSetL();

private:
    //current params (.ini)
    TInt iFamilyId;
    TPtrC iSetName;
    TPtrC iBundleName;
	};

_LIT(KInitialiseDummyProtocolParameterSetStep,"InitialiseDummyProtocolParameterSetStep");






#endif // QOS_TESTSTEPS_H

