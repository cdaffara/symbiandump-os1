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
 * Do a kernel side control for DMMCSocket
 *
 * @param TInt aFunction - Request Function ID
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::SocketDoControl(TInt aFunction, TAny* a1, TAny* a2)
	{
	TInt ret=KErrNone;
	
	if (!iSocket)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::SocketDoControl - iSocket is NULL");		
		return KErrGeneral;
		}
	else if (!iClient)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::SocketDoControl - iClient is NULL");		
		return KErrGeneral;
		}

	switch (TTCFunctionId(aFunction).FunctionId())
		{
      	case RMMCSDTestControllerInterface::ESocket:
      		{
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocket");
			ret = Kern::ThreadRawWrite(iClient, a1, &iSocket, sizeof(iSocket)); 		
      		}
	  		break;
	  		
      	case RMMCSDTestControllerInterface::ESocketAdjustPartialRead:
      		{      		
      		TPartialReadData partialReadData;
    		ret = Kern::ThreadRawRead(iClient, a1, &partialReadData, sizeof(partialReadData));	
			if( ret == KErrNone )
				{    				    			    		
		  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketAdjustPartialRead");
	      		MMCSDLOG3("start=%d, end=%d", partialReadData.iStart, partialReadData.iEnd);
		  		MMCSDLOG2("cardPtr=%x", iCard);
	    		iSocket->AdjustPartialRead(iCard, partialReadData.iStart, partialReadData.iEnd
	    									, (TUint32*)&(partialReadData.iPhysStart)
	    									, (TUint32*)&(partialReadData.iPhysEnd));
	    		ret = Kern::ThreadRawWrite(iClient, a1, &partialReadData, sizeof(partialReadData));
				}
      		}
      		break;
      	
      	case RMMCSDTestControllerInterface::ESocketCardIsPresent:
      		{      		
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketCardIsPresent");
    		TBool cardPresent = iSocket->CardIsPresent();	    		
    		ret = Kern::ThreadRawWrite(iClient, a1, &cardPresent, sizeof(cardPresent));
      		}
      		break;
      		
      	case RMMCSDTestControllerInterface::ESocketGetBufferInfo:
      		{	
			TBufferInfo bufInfo;
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketGetBufferInfo");
	  		iSocket->GetBufferInfo(&bufInfo.iBuf, &bufInfo.iBufLen);
			ret = Kern::ThreadRawWrite(iClient, a1, &bufInfo, sizeof(bufInfo));
      		}
      		break;

	  	case RMMCSDTestControllerInterface::ESocketInit:
	  		{
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketInit");
	  		ret = iSocket->Init();
	  		}
	  		break;
	  		
	  	case RMMCSDTestControllerInterface::ESocketInitiatePowerUpSequence:
	  		{
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketInitiatePowerUpSequence");
	  		iSocket->InitiatePowerUpSequence();
	  		}
	  		break;
  		
      	case RMMCSDTestControllerInterface::ESocketMachineInfo:
      		{
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketMachineInfo");
      		TMachineInfo machineInfo(iSocket->MachineInfo());
      		ret = Kern::ThreadRawWrite(iClient, a1, &machineInfo, sizeof(machineInfo));	
      		}
  			break;	  		

      	case RMMCSDTestControllerInterface::ESocketPrepareStore:
      		{	
      		TPasswordPrepareStoreData pwdData;     		
      		ret = Kern::ThreadRawRead(iClient, a1, &pwdData, sizeof(pwdData));	 
			if( ret == KErrNone )
				{
		  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketPrepareStore");
	      		MMCSDLOG5("bus=%d, func=%d, newPassword=%S, oldPassword=%S"
	      				, pwdData.iBus, pwdData.iFunc, &pwdData.iNewPassword, &pwdData.iOldPassword);      		
	      		TLocalDrivePasswordData ld;
	      		if( pwdData.iNewPassword == _L("NULL") )
	      			{
		      		ld.iNewPasswd = NULL;
			  		MMCSDLOG("Using iNewPasswd=NULL");
	      			}
	      		else
	      			{
	      			ld.iNewPasswd = &pwdData.iNewPassword;
	      			}
	      		if( pwdData.iOldPassword == _L("NULL") )
	      			{
		      		ld.iOldPasswd = NULL;
			  		MMCSDLOG("Using iOldPasswd=NULL");
	      			}
	      		else
	      			{
	      			ld.iOldPasswd = &pwdData.iOldPassword;
	      			}
	      		ret = iSocket->PrepareStore(pwdData.iBus, pwdData.iFunc, ld);     
				}
      		}
      		break;

      	case RMMCSDTestControllerInterface::ESocketReset1:
      		{
  			MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketReset1");
			iSocket->Reset1();
      		}
	  		break;
	  		
	  	case RMMCSDTestControllerInterface::ESocketReset2:
	  		{
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketReset2");
	  		iSocket->Reset2();
	  		}
	  		break;
	  		
	  	case RMMCSDTestControllerInterface::ESocketResetInactivity:
	  		{
	  		TInt busNumber;
     		ret = Kern::ThreadRawRead(iClient, a1, &busNumber, sizeof(busNumber));     		
			if( ret == KErrNone )
				{
		  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketResetInactivity");
		  		MMCSDLOG2("busNumber=%d", busNumber);
	     		iSocket->ResetInactivity(busNumber); 
				}
	  		}
	  		break;

      	case RMMCSDTestControllerInterface::ESocketStack:
      		{
      		TStackPtr stackPtr;     		
      		ret = Kern::ThreadRawRead(iClient, a1, &stackPtr, sizeof(stackPtr));	 
			if( ret == KErrNone )
				{
		  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketStack");
		  		MMCSDLOG2("busNumber=%d", stackPtr.iBus);
				stackPtr.iStackPtr = iSocket->Stack(stackPtr.iBus);
				ret = Kern::ThreadRawWrite(iClient, a1, &stackPtr, sizeof(stackPtr));
				}
      		}
	  		break;

      	case RMMCSDTestControllerInterface::ESocketiState_Get:
	  		{
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketiState_Get");
	  		MMCSDLOG2("iState=%d", iSocket->iState);
			ret = Kern::ThreadRawWrite(iClient, a1, &iSocket->iState, sizeof(iSocket->iState));
	  		}
	  		break;
	  		
		case RMMCSDTestControllerInterface::ESocketiStandby_Get:
	        {
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketiStandby_Get");
       	    MMCSDLOG2("iStandby=%d", &iSocket->iStandby);
       	    ret = Kern::ThreadRawWrite(iClient, a1, &iSocket->iStandby, sizeof(iSocket->iStandby));
            }
            break;
            
		case RMMCSDTestControllerInterface::ESocketiType_Get:
	        {
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketiType_Get");
       	    MMCSDLOG2("iType=%d", &iSocket->iType);
       	    ret = Kern::ThreadRawWrite(iClient, a1, &iSocket->iType, sizeof(iSocket->iType));
            }
            break;
            
      	case RMMCSDTestControllerInterface::ESocketiDoorOpened_Get:
        	{
	  		MMCSDLOG("Calling RMMCSDTestControllerInterface::ESocketiDoorOpened_Get");
       	    MMCSDLOG2("iDoorOpened=%d", &iSocket->iDoorOpened);
       	    ret = Kern::ThreadRawWrite(iClient, a1, &iSocket->iDoorOpened, sizeof(iSocket->iDoorOpened));
			}
        	break;
	  			  		
        default:
           	ret=KErrNotFound;
			break;
		}
	return(ret);
	}

/**
 * Do a kernel side request for DMMCSocket
 *
 * @param TRequestStatus* aStatus - Request Status
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 * @param TInt aCommand - Request Function ID
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::SocketDoRequest(TRequestStatus* aStatus, TAny* a1, TAny *a2, TInt aCommand)
	{
	MMCSDLOG2("~~~ SocketDoRequest called iAsyncFunction(%x)", aCommand);

	if (!iSocket)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::SocketDoRequest - iSocket is NULL");		
		return KErrGeneral;
		}

	TInt ret = KErrNone;
	switch (TTCFunctionId(aCommand).FunctionId())
		// iAsyncFunction initialized in DoRequest
		{
		case RMMCSDTestControllerInterface::ESocketPowerUp:
			{
			MMCSDLOG("Performing RMMCSDTestControllerInterface::ESocketPowerUp");
			
#ifdef __DEBUG
			MMCSDLOG("RMMCSDTestControllerInterface::ESocketPowerUp - Not available in debug build");
			ret = KErrNotSupported;
#else
			ret = iSocket->PowerUp();
#endif

			if ( (ret == KErrNone) || (ret == KErrCompletion) )
				{
				CreateAsyncBusEventRequest( DLddMmcCntrlInterface::EMMcSocketPowerUp, aStatus, iUnit);
				}
			else
				{
				Kern::RequestComplete(iClient, aStatus, ret);
				}
			}
			break;
		default:
			break;
		}
	return ret;
	}
