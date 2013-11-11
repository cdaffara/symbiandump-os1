 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HELLOWORLDQUERYDIALOG_H
#define HELLOWORLDQUERYDIALOG_H


#include <aknquerydialog.h>


/**
*  CHelloWorldQueryDialog class.
*  This class is used to query the user for text information
*/
class CHelloWorldQueryDialog : public CAknTextQueryDialog
    {
    public:
        /**
        * C++ default constructor.
        * @param aBuf buffer to store the input
        * @param aDefInput default input for the dialog
        */
        CHelloWorldQueryDialog(TDes& aBuf, HBufC *aDefInput);

        virtual ~CHelloWorldQueryDialog() {};

    private:
        /**
        * From CEikDialog
        * Sets the default value to the dialog.
        */
        void  PreLayoutDynInitL();

    private:
        /**
        * the default input for the dialog
        */
        HBufC&  iDefInput;
    };

#endif // HELLOWORLDQUERYDIALOG_H
