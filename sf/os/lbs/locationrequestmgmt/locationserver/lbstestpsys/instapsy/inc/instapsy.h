/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _LCFINSTAPSY_H_
#define _LCFINSTAPSY_H_


#include <lbs/epos_cpositioner.h>


class CInstaPsy: public CPositioner
{
public:
        static CInstaPsy* NewL(TAny* aConstructionParameters);
        CInstaPsy::~CInstaPsy();

private:
        void        ConstructL(TAny* aConstructionParameters);

		void 		GetPositionInfoL(TPositionInfoBase& aPosInfo);
		void 		GetBasicPositionInfoL(TPositionInfoBase& aPosInfo);

public: // from CPositioner

        void        NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
                                                          TRequestStatus& aStatus);
        void        CancelNotifyPositionUpdate();

private: // member variables

        TRequestStatus* iStatusPtr;

};




#endif

// End of file
