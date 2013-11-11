// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// sblpapi.cpp - SBLP QoS API
//


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "sblpextnTraces.h"
#endif

#include "sblpextn.h"
#include "qosextn_constants.h"
#include <networking/qosparameters.h>

// Buffer size
const TUint KSblpApiBufSize = 8192;
const TUint KNumberOfElementsPerFlowId = 2;
// Note that KAuthorizationTokenAdjustedStringLength value should be calculated 
// very carefully to fit the string inside the config block used by the pfqoslib.
// if KAuthorizationTokenActualStringLength = 255, KAuthorizationTokenAdjustedStringLength should
// be 260.
const TUint KAuthorizationTokenActualStringLength = 255;
const TUint KAuthorizationTokenStringZeroTerminated = KAuthorizationTokenActualStringLength + 1;
const TUint KAuthorizationTokenAdjustedStringLength = KAuthorizationTokenStringZeroTerminated + 4;


EXPORT_C CSblpParameters::CSblpParameters()
	{
	iAuthorizationToken = KNullDesC8;
	}

EXPORT_C CSblpParameters::~CSblpParameters()
	{
	iFlowIds.Reset();
	}

EXPORT_C CSblpParameters& CSblpParameters::operator= (const CSblpParameters& f)
	{
	if (this != &f)
		{
		SetMAT(f.iAuthorizationToken);
		SetFlowIds(f.iFlowIds);
		}
	return *this;
	}

EXPORT_C TInt CSblpParameters::GetMAT(TAuthorizationToken &aAuthToken) const
	{
	aAuthToken = iAuthorizationToken;
	return KErrNone;
	}

EXPORT_C void CSblpParameters::SetMAT(const TAuthorizationToken &aAuthToken )
	{
	iAuthorizationToken = aAuthToken;
	}

EXPORT_C CSblpParameters::TFlowIdentifier& CSblpParameters::GetFlowId(TInt aIndex)
	{
	__ASSERT_DEBUG(aIndex<iFlowIds.Count(),User::Panic(_L("CSblpParameters::GetFlowId"),KErrArgument));
	return iFlowIds[aIndex];
	}

EXPORT_C void CSblpParameters::SetFlowIds(const RArray<TFlowIdentifier> &aFlowIds)
	{
    //in case of low memory the function might not set all the ids
	iFlowIds.Reset();
	TInt i;
	for(i=0; i<aFlowIds.Count();i++)
		{
		TInt errorCode = this->iFlowIds.Append(aFlowIds[i]);
		if (errorCode != KErrNone) 
		    {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPARAMETERS_SETFLOWID, "Not Enough Memory");
		    }
		}
	}

EXPORT_C TInt CSblpParameters::GetFlowIds(RArray<TFlowIdentifier>& aFlowIds)
	{
    //in case of low memory the function might not get all the ids and returns an error code
	aFlowIds.Reset();
	TInt i;
	TInt errorCode = KErrNone;
	for(i=0; i<iFlowIds.Count();i++)
		{
        errorCode = aFlowIds.Append(this->iFlowIds[i]);
        if (errorCode != KErrNone) 
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPARAMETERS_GETFLOWID, "Not Enough Memory");
            return errorCode;
            }
		}
	return KErrNone;
	}

EXPORT_C TInt CSblpParameters::NrOfFlowIds()
	{
	return iFlowIds.Count();
	}

//
// CSblpPolicy
//
EXPORT_C CSblpPolicy* CSblpPolicy::NewL()
	{
	CSblpPolicy* policy = new (ELeave) CSblpPolicy();
	CleanupStack::PushL(policy);
	policy->ConstructL();
	CleanupStack::Pop();
	return policy;
	}

CSblpPolicy::CSblpPolicy()
	{
	iType = KPfqosExtensionSBLP;
	}

void CSblpPolicy::ConstructL()
	{
	iData = HBufC8::NewL(KSblpApiBufSize);
	iSblp = new (ELeave) CSblpParameters;
	}

EXPORT_C CSblpPolicy::~CSblpPolicy()
	{
	delete iSblp;
	//iData is deleted in the base
	}

static void SetIntValue(pfqos_configblock_int& data, TInt aValue,
						const TDesC8& aName)
	{
	data.len = sizeof(pfqos_configblock_int)/8;
	data.padding = data.reserved = 0;
	data.type = KPfqosTypeInteger;
	data.value = aValue;
	Mem::FillZ(data.id, KPfqosMaxName);
	Mem::Copy(data.id, aName.Ptr(), aName.Length());
	}



static void SetStringBlockHeader(pfqos_configblock& data,
								 TDesC8& /*aValue*/, const TDesC8& aName)
	{
	data.len = (sizeof(pfqos_configblock) + KAuthorizationTokenAdjustedStringLength )/8;
	data.padding = data.reserved = 0;
	data.type = KPfqosTypeString;
	Mem::FillZ(data.id, KPfqosMaxName);
	Mem::Copy(data.id, aName.Ptr(), aName.Length());
	}

EXPORT_C TDesC8& CSblpPolicy::Data()
	{
	TPtr8 bufPtr = iData->Des();
	bufPtr.SetLength(0);
	const TInt byte_len = (sizeof(pfqos_configure)+
						  sizeof(pfqos_extension)+
						  sizeof(pfqos_configblock)+
						  KAuthorizationTokenAdjustedStringLength+
			                          KNumberOfElementsPerFlowId * iSblp->NrOfFlowIds() *
						  sizeof(pfqos_configblock_int));

	pfqos_configure header;
	header.pfqos_configure_len = (TUint16)((byte_len + 7) / 8);
	header.pfqos_ext_type = EPfqosExtExtension;
	header.reserved = 0;
	header.protocol_id = 0;
	bufPtr.Append((TUint8*)&header, sizeof(pfqos_configure));
	
	pfqos_extension extensionType;
	extensionType.pfqos_ext_len = 0;
	extensionType.pfqos_ext_type = EPfqosExtExtension;
	extensionType.pfqos_extension_type = KPfqosExtensionSBLP;
	bufPtr.Append((TUint8*)&extensionType, sizeof(pfqos_extension));

	/* MAT
	
	MAT is a string
	The string is put after pfqos_configblock
	The pfqos_configblock length is the size of pfqos_configblock
	plus the string length
	*/
	TAuthorizationToken authToken; 
	pfqos_configblock stringBlock;
	iSblp->GetMAT(authToken);
	SetStringBlockHeader(stringBlock,authToken,KDescSblpMediaAuthorizationToken);
	bufPtr.Append((TUint8*)&stringBlock, sizeof(pfqos_configblock));

	// put the string now
	authToken.ZeroTerminate();
	bufPtr.Append((TUint8*)authToken.Ptr(),KAuthorizationTokenAdjustedStringLength);

	// Flowids
	pfqos_configblock_int ext;
	TInt i;
	for(i=0; i<iSblp->NrOfFlowIds();i++)
		{
		CSblpParameters::TFlowIdentifier& flowId = iSblp->GetFlowId(i);
		SetIntValue(ext,flowId.iMediaComponentNumber,KDescSblpMediaComponentNumber);
		bufPtr.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

		SetIntValue(ext,flowId.iIPFlowNumber,KDescSblpIPFlowNumber);
		bufPtr.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));
		}

	bufPtr.AppendFill(0, header.pfqos_configure_len * 8 - byte_len);

	return *iData;
	}

EXPORT_C TInt CSblpPolicy::ParseMessage(const TDesC8& /*aData*/)
	{
	return KErrNone;
	}

EXPORT_C CExtensionBase* CSblpPolicy::CreateL()
	{
	CSblpPolicy *extension = CSblpPolicy::NewL();
	return extension;
	}

EXPORT_C TInt CSblpPolicy::Copy(const CExtensionBase& aExtension)
	{
	if (aExtension.Type() != iType)
		return KErrArgument;
	const CSblpPolicy& policy = (CSblpPolicy&)aExtension;
	policy.GetSblpParameters(*iSblp);
	return KErrNone;
	}

EXPORT_C void CSblpPolicy::SetSblpParameters(const CSblpParameters& aSblp)
	{
	
	*iSblp = aSblp;


	// following code is to create logs
	TAuthorizationToken authToken;
	iSblp->GetMAT (authToken);
    RArray<CSblpParameters::TFlowIdentifier> flowIds;
	iSblp->GetFlowIds(flowIds);
	// in case of low memory GetFlowIds might return an error and flowIds might not have all the ids, but we still try to log as much as we can
	TBuf<KAuthorizationTokenSize> label;
	label.Copy(authToken);
		
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_1, "<------------------------------------------------\n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_2, "CSblpPolicy::SetSblpParameters");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_3, "\n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_4, "SBLP VALUES SUPPLIED BY CLIENT IS \n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_5, "\n");
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_6, "[MAT string           :   = %S]\n",label);
	
	TInt i;
	for(i=0; i<flowIds.Count();i++)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_7, "Media component number  :   = %d]\n",flowIds[i].iMediaComponentNumber);
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_8, "IP flow number       :  = %d]\n",flowIds[i].iIPFlowNumber);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSBLPPOLICY_SETSBLPPARAMETERS_9, "------------------------------------------------>\n");
	flowIds.Close();
	
	
	}

EXPORT_C void CSblpPolicy::GetSblpParameters(CSblpParameters& aSblp) const
	{
	aSblp = *iSblp;
	}
