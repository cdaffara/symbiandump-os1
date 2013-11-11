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
// trpfunctor.cpp
// TRpsFunctor implementation
// 
//

 #include "trpsfunctor.h"
 
 #

//constructor - takes pointer to the RPS master object, pointer to the RPS function to invoke and 1 paramter
template <class TRpsParam1, class TRpsParam2, class TRpsParam3>
TRpsFunctor<TRpsParam1, TRpsParam2, TRpsParam3>::TRpsFunctor(CRPSMaster* aRpsMaster, const TDesC& aDisplayToClientTxt, TEtelLine aLine, TRpsWith1ParamFptr aRpsFptr, TRpsParam1 aRpsParam1)
: iRpsFptrWith1Param(aRpsFptr),
  iRpsMaster(aRpsMaster),  
  iRpsParam1(aRpsParam1),
  iRpsCallType(EOneParam)
	{
	SetDisplayToClientTxt(aDisplayToClientTxt, aLine);	
	}

//constructor - takes pointer to the RPS master object, pointer to the RPS function to invoke and 2 paramters
template <class TRpsParam1, class TRpsParam2, class TRpsParam3>
TRpsFunctor<TRpsParam1, TRpsParam2, TRpsParam3>::TRpsFunctor(CRPSMaster* aRpsMaster, const TDesC& aDisplayToClientTxt, TEtelLine aLine, TRpsWith2ParamFptr aRpsFptr, TRpsParam1 aRpsParam1, TRpsParam2 aRpsParam2)
: iRpsFptrWith2Param(aRpsFptr),
  iRpsMaster(aRpsMaster),
  iRpsParam1(aRpsParam1),
  iRpsParam2(aRpsParam2),
  iRpsCallType(ETwoParam)
	{
	SetDisplayToClientTxt(aDisplayToClientTxt, aLine);	
	}
	
//constructor - takes pointer to the RPS master object, pointer to the RPS function to invoke and 3 paramters
template <class TRpsParam1, class TRpsParam2, class TRpsParam3>
TRpsFunctor<TRpsParam1, TRpsParam2, TRpsParam3>::TRpsFunctor(CRPSMaster* aRpsMaster, const TDesC& aDisplayToClientTxt, TEtelLine aLine, TRpsWith3ParamFptr aRpsFptr, TRpsParam1 aRpsParam1, TRpsParam2 aRpsParam2, TRpsParam3 aRpsParam3)
: iRpsFptrWith3Param(aRpsFptr),
  iRpsMaster(aRpsMaster),  
  iRpsParam1(aRpsParam1),
  iRpsParam2(aRpsParam2),
  iRpsParam3(aRpsParam3),
  iRpsCallType(EThreeParam)
	{
	SetDisplayToClientTxt(aDisplayToClientTxt, aLine);	
	}

//setup msg to display to client	
template <class TRpsParam1, class TRpsParam2, class TRpsParam3>
void TRpsFunctor<TRpsParam1, TRpsParam2, TRpsParam3>::SetDisplayToClientTxt(const TDesC& aDisplayToClientTxt, TEtelLine aLine)
	{
	_LIT(KVoice, "voice");
	_LIT(KData, "data");
	_LIT(KFax, "fax");
	if(aLine >=0 )
		{
		switch(aLine)
			{
		case EDataLine:
			iDisplayToClientTxt.Format(aDisplayToClientTxt, &KData);
			break;
		case EFaxLine:
			iDisplayToClientTxt.Format(aDisplayToClientTxt, &KFax);
			break;
		case EVoiceLine:
			default:
			iDisplayToClientTxt.Format(aDisplayToClientTxt, &KVoice);
			}
		}
	else
		{
		iDisplayToClientTxt.Copy(aDisplayToClientTxt);	
		}
	}
	
//return msg that is displayed to the client
template <class TRpsParam1, class TRpsParam2, class TRpsParam3>
const TDesC& TRpsFunctor<TRpsParam1, TRpsParam2, TRpsParam3>::DisplayToClientTxt()
		{
		return 	iDisplayToClientTxt;	
		}

//execute the RPS request based on the number of parameters that was passed in
template <class TRpsParam1, class TRpsParam2, class TRpsParam3>
void TRpsFunctor<TRpsParam1, TRpsParam2, TRpsParam3>::ExecuteRpsRequestL()
	{
	if(iRpsCallType == EOneParam)
		{
		(*iRpsMaster.*iRpsFptrWith1Param)(iRpsParam1);	
		}
	else if(iRpsCallType == ETwoParam)
		{
		(*iRpsMaster.*iRpsFptrWith2Param)(iRpsParam1, iRpsParam2);
		}
	else if(iRpsCallType == EThreeParam)
		{
		(*iRpsMaster.*iRpsFptrWith3Param)(iRpsParam1, iRpsParam2, iRpsParam3);
		}
	else
		{
		__ASSERT_ALWAYS(EFalse, User::Panic(KPanic,KCTSYPanic_invalidrpsparameters));
		}
	
	return;
	}

	
