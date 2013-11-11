// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include "AgtNotify.h"


TRealAgtNotify::TRealAgtNotify()
	{
	TInt err = iEtelSess.Connect();
	if (err != KErrNone)
		{
		_LIT(KPanicClientText,"Failed connect to etel server");
		User::Panic(KPanicClientText, err);
		}
	}

TRealAgtNotify::~TRealAgtNotify()
	{
	iEtelSess.Close();
	}

void TRealAgtNotify::SetTestCase(TInt aTestCase)
	{
	iEtelSess.SetTestNumber(aTestCase);
	iTestCase = aTestCase;
	}

void TRealAgtNotify::NotifyAgt(enum QoSWarningNotification aNotification)
	{
	switch(iTestCase)
	{
	case 201:
	case 202:
	case 203:
	case 204:
	case 1404:
		// Context Config Change
		if(aNotification == EReduce)
			{
			// Drop
			iEtelSess.TriggerContextConfigDrop();
			}
		else
			{
			// restore
			iEtelSess.TriggerContextConfigRestore();
			}
		break;
	case 301:
	case 302:
	case 303:
	case 304:
	case 305:
	case 306:
	case 307:
	case 308:
	case 309:
	case 310:
	case 311:
	case 312:
	case 313:
	case 314:
	case 315:
	case 316:
	case 317:
	case 318:
	case 319:
	case 320:
	case 321:
	case 322:
	case 323:
	case 324:
	case 325:
	case 326:
	case 327:
	case 328:
	case 329:
	case 330:
	case 401:
	case 1405:
		// QoS Change
		if(aNotification == EReduce)
			{
			// Drop
			iEtelSess.TriggerQoSConfigDrop();
			}
		else
			{
			// restore
			iEtelSess.TriggerQoSConfigRestore();
			}
		break;
	default:
		break;
	}
	}

void TRealAgtNotify::SetMode(RTelServer::TMobilePhoneNetworkMode aMode)
	{
	iEtelSess.SetMode(aMode);
	}
