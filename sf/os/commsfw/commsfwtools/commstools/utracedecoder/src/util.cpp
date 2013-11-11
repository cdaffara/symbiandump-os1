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
//

#include <ctype.h>
#include <string.h>
#include <iomanip>

#include "util.h"

char hexChars[] = "0123456789abcdef";

unsigned int HexToVal(const char* aHex)
    {
    int shift = 0;
    int pos = (int)strlen(aHex) -1;
    unsigned int v = 0;
    int i;

    while (pos > 1 || ((!(aHex[0] == '0' && tolower(aHex[1]) == 'x')) && pos >= 0))
        {
        for (i=0; i<16; i++)
            {
            if (hexChars[i] == tolower(aHex[pos]))
                {
                v += (i << shift);
                break;
                }
            }
        shift += 4;
        --pos;
        }

    return v;
    }

void DumpBytes(std::ostream& aStream, const unsigned char* aData, unsigned int aLength, unsigned int* aZones, unsigned int aZoneCount)
    {
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int dumpLength = (aLength + 7) & ~7UL;
    unsigned int zone = 0;

    while (i < dumpLength)
        {
        aStream << " ";
        while (i < dumpLength)
            {
            if (i < aLength)
                {
                if (aZones
                    && zone < aZoneCount
                    && i == aZones[zone])
                    {
                    aStream << "[";
                    }
                else
                    {
                    aStream << " ";
                    }
                aStream << std::noshowbase << std::setw(2)
                    << std::setfill('0') << std::nouppercase << std::hex
                    << int(aData[i]);

                if (aZones && (
                    (zone+1) < aZoneCount && i == aZones[zone+1])
                    || ((zone+1) == aZoneCount && (i+1) == aLength)
                    )
                    {
                    aStream << "]";
                    }
                else
                    {
                    aStream << " ";
                    }
                }
            else
                {
                aStream << "    ";
                }

            ++i;

            if (i % 8 == 0)
                {
                break;
                }
            else if (i % 4 == 0)
                {
                aStream << " - ";
                }
            }

        aStream << "  ";

        while (j < dumpLength)
            {
            if (j < aLength)
                {
                if (isprint(aData[j]))
                    {
                    aStream << aData[j];
                    }
                else 
                    {
                    aStream << ".";
                    }
                }
            else
                {
                aStream << " ";
                }

            ++j;
            
            if ((j % 8) == 0)
                {
                break;
                }
            }

        aStream << std::endl;
        }
    }

void DumpBytes(std::ostream& aStream, const unsigned char* aData, unsigned int aLength)
    {
    DumpBytes(aStream, aData, aLength, NULL, 0);
/*
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int dumpLength = (aLength + 7) & ~7UL;
    while (i < dumpLength)
        {
        aStream << "  ";
        while (i < dumpLength)
            {
            if (i < aLength)
                {
                aStream << std::noshowbase << std::setw(2)
                    << std::setfill('0') << std::nouppercase << std::hex
                    << int(aData[i]) << "  ";
                }
            else
                {
                aStream << "    ";
                }

            ++i;

            if (i % 8 == 0)
                {
                break;
                }
            else if (i % 4 == 0)
                {
                aStream << "-  ";
                }
            }

        aStream << "  ";

        while (j < dumpLength)
            {
            if (j < aLength)
                {
                if (isprint(aData[j]))
                    {
                    aStream << aData[j];
                    }
                else 
                    {
                    aStream << ".";
                    }
                }
            else
                {
                aStream << " ";
                }

            ++j;
            
            if ((j % 8) == 0)
                {
                break;
                }
            }

        aStream << std::endl;
        }
*/
    }

