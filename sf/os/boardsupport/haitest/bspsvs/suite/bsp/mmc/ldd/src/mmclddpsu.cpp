/*
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
*
*/


#include <kernel.h>
#include <mmc.h>
#include <locmedia.h>

#include "mmcldd.h"
#include "d_mmcsdif.h"

/**
 * Do a kernel side control for DMMCPsu
 *
 * @param TInt aFunction - Request Function ID
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::PsuDoControl(TInt aFunction, TAny* a1, TAny* a2)
	{
	MMCSDLOG("<--PsuDoControl-->");
	TInt r = KErrNone;
	
	if (!iPsu)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::PsuDoControl - iPsu is NULL");		
		return KErrGeneral;
		}
	else if (!iClient)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::PsuDoControl - iClient is NULL");		
		return KErrGeneral;
		}

	switch(TTCFunctionId(aFunction).FunctionId())
		{
		case RMMCSDTestControllerInterface::EPsu:
      		{
			r = Kern::ThreadRawWrite(iClient, a1, &iPsu, sizeof(iPsu));
      		}
	  		break;
		case RMMCSDTestControllerInterface::EPsuInfo:
	    	{
			TPBusPsuInfo psuInfo;
			MMCSDLOG("<--PsuInfo-->");
			iPsu->PsuInfo(psuInfo);
			r = Kern::ThreadRawWrite(iClient, a1, &psuInfo, sizeof(TPBusPsuInfo));
	     	break;
			}
        case RMMCSDTestControllerInterface::EPsuState:
			 {
			if(iPsu != NULL)
				{
				TPBusPsuState aState;
				aState = (TPBusPsuState&)a1;
				r= iPsu->SetState(aState);
				MMCSDLOG2("EPsuState :%d", aState);
				}
			else
				{
				r= KErrGeneral;
				}
			break;
            }
        case RMMCSDTestControllerInterface::EPsuVoltage:
			{
			if(iPsu != NULL)
				{
				TUint32 aVoltOpRange;
				aVoltOpRange = (TUint32)a1;
				iPsu->SetVoltage(aVoltOpRange);
				MMCSDLOG2("EPsuVoltage :0x%x", aVoltOpRange);
				}
			else
				{
				r= KErrGeneral;
				}
			break;
            }
        case RMMCSDTestControllerInterface::EPsuNum:
			{
			if(iPsu != NULL)
				{
				r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iPsuNum, sizeof(TInt));
				MMCSDLOG2("EPsuNum :%d", &iPsu->iPsuNum);
				}
			else
				{
				r= KErrGeneral;
				}
			break;
            }
        case RMMCSDTestControllerInterface::EMediaChangeNum:
			{
			if(iPsu != NULL)
				{
				r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iMediaChangeNum, sizeof(TInt));
				MMCSDLOG2("EMediaChangeNum :%d", &iPsu->iMediaChangeNum);
				}
			else
				{
				r= KErrGeneral;
				}
			break;
            }
        case RMMCSDTestControllerInterface::EPsuCurLimit:
			{
			if(iPsu != NULL)
				{
				r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iCurrLimited, sizeof(TBool));
				MMCSDLOG2("EPsuCurLimit :%d", &iPsu->iCurrLimited);
				}
			else
				{
				r= KErrGeneral;
				}
			break;
            }
	    case RMMCSDTestControllerInterface::EPbusState:
		 	{
			if (iSocket)
				{
				r = Kern::ThreadRawWrite(iClient, a1, &iSocket->iState, sizeof(TInt));
				MMCSDLOG2("EPbusState :%d", &iSocket->iState);
				}
			else
				{
		  		MMCSDLOG("RMMCSDTestControllerInterface::EPbusState - iSocket is NULL");		
				r = KErrGeneral;
				}
			break;
           	}
	    case RMMCSDTestControllerInterface::EVccState:
	        {
    	    if(iPsu)
        		{
           	    r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iState,sizeof(TInt));
           	    MMCSDLOG2("EVccState :%d", &iPsu->iState);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EIsLocked:
	        {
    	    if(iPsu)
        		{
				TBool lock =iPsu->IsLocked();
           	    r = Kern::ThreadRawWrite(iClient, a1, &lock,sizeof(TBool));
           	    MMCSDLOG2("EIsLocked :%d", lock);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EIsOff:
	        {
    	    if(iPsu)
        		{
				TBool isoff =iPsu->IsOff();
           	    r = Kern::ThreadRawWrite(iClient, a1, &isoff,sizeof(TBool));
           	    MMCSDLOG2("EIsOff :%d", isoff);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EMaxCurrentInMicroAmps:
	        {
    	    if(iPsu)
        		{
				TInt current =iPsu->MaxCurrentInMicroAmps();
           	    r = Kern::ThreadRawWrite(iClient, a1, &current,sizeof(TInt));
           	    MMCSDLOG2("EMaxCurrentInMicroAmps :%d", current);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EResetInactivityTimer:
	        {
    	    if(iPsu)
        		{
				iPsu->ResetInactivityTimer();
           	    MMCSDLOG("EResetInactivityTimer");
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EVoltageSupported:
	        {
    	    if(iPsu)
        		{
				TUint vol =iPsu->VoltageSupported();
           	    r = Kern::ThreadRawWrite(iClient, a1, &vol,sizeof(TUint));
           	    MMCSDLOG2("EVoltageSupported :0x%x", vol);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EMemInactivityCount:
	        {
    	    if(iPsu)
        		{
           	    r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iInactivityCount,sizeof(&iPsu->iInactivityCount));
           	   MMCSDLOG2("EMemInactivityCount :%d", &iPsu->iInactivityCount);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EMemNotLockedCount:
	        {
    	    if(iPsu)
        		{
           	    r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iNotLockedCount,sizeof(&iPsu->iNotLockedCount));
           	    MMCSDLOG2("EMemNotLockedCount :%d", &iPsu->iNotLockedCount);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EMemInactivityTimeout:
	        {
    	    if(iPsu)
        		{
           	    r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iInactivityTimeout,sizeof(&iPsu->iInactivityTimeout));
           	    MMCSDLOG2("EMemInactivityTimeout :%d", &iPsu->iInactivityTimeout);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EMemNotLockedTimeout:
	        {
    	    if(iPsu && iClient)
        		{
           	    r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iNotLockedTimeout,sizeof(&iPsu->iNotLockedTimeout));
           	    MMCSDLOG2("EMemNotLockedTimeout :%d", &iPsu->iNotLockedTimeout);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EMemVoltageSupported:
	        {
    	    if(iPsu && iClient)
        		{
           	    r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iVoltageSupported,sizeof(&iPsu->iVoltageSupported));
           	    MMCSDLOG2("EMemVoltageSupported :%d", &iPsu->iVoltageSupported);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EMemMaxCurrentInMicroAmps:
	        {
    	    if(iPsu && iClient)
        		{
           	    r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iMaxCurrentInMicroAmps,sizeof(&iPsu->iMaxCurrentInMicroAmps));
           	    MMCSDLOG2("EMemMaxCurrentInMicroAmps :%d", &iPsu->iMaxCurrentInMicroAmps);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		case RMMCSDTestControllerInterface::EMemVoltCheckInterval:
	        {
    	    if(iPsu && iClient)
        		{
           	    r = Kern::ThreadRawWrite(iClient, a1, &iPsu->iVoltCheckInterval,sizeof(&iPsu->iVoltCheckInterval));
           	    MMCSDLOG2("EMemVoltCheckInterval :%d", &iPsu->iVoltCheckInterval);
            	}
            else
            	{
            	r=KErrGeneral;
            	}
            break;
            }
		default:
			r = KErrNotFound;
			break;

		}
	return (r);
	}
