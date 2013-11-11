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
// imsapi.cpp - IMS QoS API
//

#include "imsextn.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "imsextnTraces.h"
#endif

//#ifdef SYMBIAN_NETWORKING_UMTS5

#include "qosextn_constants.h"
#include <networking/qosparameters.h>
// Buffer size
const TUint KImsApiBufSize = 8192;


EXPORT_C TImsParameter::TImsParameter() : iIMSSignallingIndicator(EFalse)
	{
	}

EXPORT_C TBool TImsParameter::GetIMSSigallingIndicator() const
	{
	return iIMSSignallingIndicator;
	}

EXPORT_C void TImsParameter::SetIMSSigallingIndicator(const TBool aIMSSignallingIndicator)
	{
	iIMSSignallingIndicator = aIMSSignallingIndicator;
	}

//
// CImsPolicy
//
EXPORT_C CImsPolicy* CImsPolicy::NewL()
	{
	CImsPolicy* policy = new (ELeave) CImsPolicy();
	CleanupStack::PushL(policy);
	policy->ConstructL();
	CleanupStack::Pop();
	return policy;
	}

CImsPolicy::CImsPolicy()
	{
	iType = KPfqosExtensionIMS;
	}

void CImsPolicy::ConstructL()
	{
	iData = HBufC8::NewL(KImsApiBufSize);
	}

EXPORT_C CImsPolicy::~CImsPolicy()
	{
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



EXPORT_C TDesC8& CImsPolicy::Data()
	{
	TPtr8 bufPtr = iData->Des();
	bufPtr.SetLength(0);
	const int byte_len = (sizeof(pfqos_configure)+sizeof(pfqos_extension)+sizeof(pfqos_configblock_int));

	pfqos_configure header;
	header.pfqos_configure_len = (TUint16)((byte_len + 7) / 8);
	header.pfqos_ext_type = EPfqosExtExtension;
	header.reserved = 0;
	header.protocol_id = 0;
	bufPtr.Append((TUint8*)&header, sizeof(pfqos_configure));

	pfqos_extension extensionType;
	extensionType.pfqos_ext_len = 0;
	extensionType.pfqos_ext_type = EPfqosExtExtension;
	extensionType.pfqos_extension_type = KPfqosExtensionIMS;
	bufPtr.Append((TUint8*)&extensionType, sizeof(pfqos_extension));

	pfqos_configblock_int iExt;

	// Minimum
	SetIntValue(iExt, iIms.GetIMSSigallingIndicator(), KDescIMSSignallingIndicator);
	bufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	bufPtr.AppendFill(0, header.pfqos_configure_len * 8 - byte_len);
	return *iData;
	}

EXPORT_C TInt CImsPolicy::ParseMessage(const TDesC8& /*aData*/)
	{
	return KErrNone;
	}

EXPORT_C CExtensionBase* CImsPolicy::CreateL()
	{
	CImsPolicy *extension = CImsPolicy::NewL();
	return extension;
	}

EXPORT_C TInt CImsPolicy::Copy(const CExtensionBase& aExtension)
	{
	if (aExtension.Type() != iType)
		return KErrArgument;
	const CImsPolicy& policy = (const CImsPolicy&)aExtension;
	policy.GetImsParameter(iIms);
	return KErrNone;
	}

EXPORT_C void CImsPolicy::SetImsParameter(const TImsParameter& aIms)
	{
	iIms = aIms;
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CIMSPOLICY_SETIMSPARAMETER_1, "<------------------------------------------------\n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CIMSPOLICY_SETIMSPARAMETER_2, "CImsPolicy::SetImsParameter");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CIMSPOLICY_SETIMSPARAMETER_3, "\n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CIMSPOLICY_SETIMSPARAMETER_4, "IMS VALUE SUPPLIED BY CLIENT IS \n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CIMSPOLICY_SETIMSPARAMETER_5, "\n");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CIMSPOLICY_SETIMSPARAMETER_6, "[IMS Signalling Indicator value is  :   = %d]\n",aIms.GetIMSSigallingIndicator());
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CIMSPOLICY_SETIMSPARAMETER_7, "------------------------------------------------>\n");
	
	}

EXPORT_C void CImsPolicy::GetImsParameter(TImsParameter& aIms) const
	{
	aIms = iIms;
	}

//#endif // SYMBIAN_NETWORKING_UMTS5
