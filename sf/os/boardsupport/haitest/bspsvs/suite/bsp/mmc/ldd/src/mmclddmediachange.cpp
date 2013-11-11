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
 * Do a kernel side control for DMMCMediChange
 *
 * @param TInt aFunction - Request Function ID
 * @param TAny* a1 - Given parameter
 * @param TAny* a2 - Given parameter
 *
 * @return TInt the error code
 *
 * @leave N/A
 */
TInt DLddMmcCntrlInterface::MediaChangeDoControl(TInt aFunction, TAny* a1, TAny* a2)
	{
	MMCSDLOG("<--MediaChangeDoControl-->");
	TInt r = KErrNone;
	
	if (!iSocket)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::MediaChangeDoControl - iSocket is NULL");		
		return KErrGeneral;
		}
	else if (!iClient)
		{
  		MMCSDLOG("DLddMmcCntrlInterface::MediaChangeDoControl - iClient is NULL");		
		return KErrGeneral;
		}

	switch(TTCFunctionId(aFunction).FunctionId())
		{
      	case RMMCSDTestControllerInterface::EMediaChange:
      		{
			r = Kern::ThreadRawWrite(iClient, a1, &iMediaChange, sizeof(iMediaChange));
      		}
	  		break;

		case RMMCSDTestControllerInterface::EMediaStateInfo:
	     	{
			TMediaState doorState;
			doorState = iMediaChange->MediaState();
			MMCSDLOG2("EMediaStateInfo : %d",doorState);
			r = Kern::ThreadRawWrite(iClient, a1, &doorState, sizeof(TMediaState));
			break;
	       	}
		case RMMCSDTestControllerInterface::EForceMediaChange:
        	{
			iMediaChange->ForceMediaChange();
			MMCSDLOG("Force MediaChange");
        	break;
			}
		case RMMCSDTestControllerInterface::EMediaChangeCounter:
        	{
    		r = Kern::ThreadRawWrite(iClient, a1, &iMediaChange->iReplyCount, sizeof(iMediaChange->iReplyCount));
    		MMCSDLOG2("EMediaChangeCounter : %d",&iMediaChange->iReplyCount);
        	break;
			}
		default:
			r = KErrNotFound;
			break;

		}
	return(r);
	}
