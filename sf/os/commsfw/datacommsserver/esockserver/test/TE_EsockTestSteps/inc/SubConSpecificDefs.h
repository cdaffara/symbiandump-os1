/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the subconnection specific constants
* 
*
*/



/**
 @file SubConSpecificDefs.h
*/
#if (!defined SUBCONSPECIFICDEFS_H)
#define SUBCONSPECIFICDEFS_H

//subconnection generic
_LIT(KTe_SubConnectionSectionName,"SubConnection");
_LIT(KTe_SubConnectionParamsSectionName,"SubConnectionParams");
_LIT(KTe_SubConnectionParamsFamilySectionName,"SubConnectionParamsFamily");
_LIT(KTe_SubConnectionParamsCompareGenQosParameterSetSectionName,"CompareGenericQosSets");
_LIT(KTe_SubConnectionParamsCompareGenSblpParameterSetSectionName,"CompareGenericSplpSets");
_LIT(KTe_SubConnectionParamsCompareExtIPLink99ParameterSetSectionName,"CompareExtIPLink99Sets");
_LIT(KTe_SubConnectionParamsCompareExtIPLink5ParameterSetSectionName,"CompareExtIPLink5Sets");
_LIT(KTe_SubConnectionParamsCompareExtIMSParameterSetSectionName,"CompareExtIMSSets");
_LIT(KTe_SubConnectionParamsCompareExtSblpParameterSetSectionName,"CompareExtSblpSets");

_LIT(KTe_SubConnectionName,"SubConnName");
_LIT(KTe_SubConnectionParamsName,"SubConnParamsName");
_LIT(KTe_SubConnectionTypeName,"SubConnType");
_LIT(KTe_SubConnectionEventSectionName,"SubConnectionEvents");

_LIT(KTe_SubConnectionTypeAttach,"EAttachToDefault");
_LIT(KTe_SubConnectionTypeNew,"ECreateNew");

//subcon parameters


_LIT(KTe_ParamsTypeIdUidName,"Type_UID");
_LIT(KTe_ParamsTypeIdIntName,"Type_INT");
_LIT(KTe_ParamsTypeRequested,"Requested");
_LIT(KTe_ParamsTypeAcceptable,"Acceptable");
_LIT(KTe_ParamsTypeGranted,"Granted");

_LIT(KTe_HeaderMode,"HeaderMode");
_LIT(KTe_DownlinkBandwidth,"DownlinkBandwidth");
_LIT(KTe_UplinkBandwidth,"UplinkBandwidth");
_LIT(KTe_DownLinkMaximumBurstSize,"DownLinkMaxBurstSize");
_LIT(KTe_UpLinkMaximumBurstSize,"UpLinkMaxBurstSize");
_LIT(KTe_DownLinkAveragePacketSize,"DownLinkAvPacketSize");
_LIT(KTe_UpLinkAveragePacketSize,"UpLinkAvPacketSize");
_LIT(KTe_DownLinkMaximumPacketSize,"DownLinkMaxPacketSize");
_LIT(KTe_UpLinkMaximumPacketSize,"UpLinkMaxPacketSize");
_LIT(KTe_DownLinkDelay,"DownLinkDelay");
_LIT(KTe_UpLinkDelay,"UpLinkDelay");
_LIT(KTe_DownLinkDelayVariation,"DownLinkDelayVariation");
_LIT(KTe_UpLinkDelayVariation,"UpLinkDelayVariation");
_LIT(KTe_DownLinkPriority,"DownLinkPriority");
_LIT(KTe_UpLinkPriority,"UpLinkPriority");

_LIT(KTe_ParamsTypeInvite,"Invite");
_LIT(KTe_ParamsTypeSubscribe,"Subscribe");
//subcon events
_LIT(KTe_EventTypeParamsGranted,"ParamsGranted");
_LIT(KTe_EventTypeDataClientJoined,"DataClientJoined");
_LIT(KTe_EventTypeDataClientLeft,"DataClientLeft");
_LIT(KTe_EventTypeSubConDown,"SubConDown");
_LIT(KTe_EventTypeParamsChanged,"ParamsChanged");
_LIT(KTe_EventTypeParamsRejected,"ParamsRejected");


_LIT(KTe_SubConGenericParamsSetName,"GSet");
_LIT(KTe_SubConParamsSetName,"Set");

_LIT(KTe_CompareFamily1Type,"FamilyType1");
_LIT(KTe_CompareFamily2Type,"FamilyType2");
_LIT(KTe_SubConnectionParams1Name,"SubConnParamsName1");
_LIT(KTe_SubConnectionParams2Name,"SubConnParamsName2");
_LIT(KTe_SubConnectionParams1Type,"SubConnParamsType1");
_LIT(KTe_SubConnectionParams2Type,"SubConnParamsType2");

#endif //SUBCONSPECIFICDEFS_H

