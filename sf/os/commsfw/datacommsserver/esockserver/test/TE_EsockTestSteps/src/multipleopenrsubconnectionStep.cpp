// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file multipleopenrsubconnectionStep.cpp
*/

#include "SubConnections.TestSteps.h"
#include "SocketServer.TestSteps.h"
#include "Connections.TestSteps.h"

CMultipleOpenRSubConnectionStep::CMultipleOpenRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KMultipleOpenRSubConnectionStep);
	}

TInt CMultipleOpenRSubConnectionStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the next socket's name
    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SocketServName,iParams.iSockServName)!=1)
        {
        INFO_PRINTF2(_L("%S: Socket server name missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)!=1)
        {
        INFO_PRINTF2(_L("%S: Connection name missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

	TPtrC subConnTypeName;
    if (GetStringFromConfig(iSection,KTe_SubConnectionTypeName,subConnTypeName)!=1)
        {
        INFO_PRINTF2(_L("%S: SubConnection type missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

    if (subConnTypeName.Compare(KTe_SubConnectionTypeAttach)==0)
	    { iParams.iSubConnType = RSubConnection::EAttachToDefault; }
    else if (subConnTypeName.Compare(KTe_SubConnectionTypeNew)==0)
	    { iParams.iSubConnType = RSubConnection::ECreateNew; }
    else
       {
       INFO_PRINTF3(_L("%S: SubConnection type (%S) not recognised."),&iParams.iSubConnectionName,&subConnTypeName);
       return KErrNotFound;
       }

    return KErrNone;
    }

TVerdict CMultipleOpenRSubConnectionStep::doSingleTestStep()
	{
    TInt error = iEsockTest->OpenSubConnection(iParams);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not open subconnection (%S)."),&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }

	error = iEsockTest->OpenSubConnection(iParams);
    if (error==KErrNone)
        {
        INFO_PRINTF1(_L("KErrNone returned on multiple RSubConnection::Open()."));
        return EFail;
        }

	return EPass;
	}

