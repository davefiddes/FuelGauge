///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Transform a value from one number system to another using a map
//!
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 David J. Fiddes
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#include <mapper.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Interpolate between two map bins
//!
//! The value supplied is between the specified lowerBin and the one higher
//!
///////////////////////////////////////////////////////////////////////////////
static uint16_t InterpolateBinValue(
    uint16_t        value,
    uint8_t         lowerBin,
    uint8_t         upperBin,
    const uint16_t* inputMap,
    const uint16_t* outputMap )
{
    //
    // Use int32_t values to differences to avoid overflowing a uin16_t when
    // multiplying on an 8-bit PIC
    //
    // Signed values are used because for either map the upper bin may have a
    // higher or lower value than the lower bin
    //
    int32_t valueDiff = value - inputMap[ lowerBin ];
    int32_t inputBinDiff = inputMap[ upperBin ] - inputMap[ lowerBin ];
    int32_t outputBinDiff = outputMap[ upperBin ] - outputMap[ lowerBin ];

    int32_t output = ( valueDiff * outputBinDiff ) / inputBinDiff;

    output = outputMap[ lowerBin ] + output;

    return (uint16_t)output;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Map a value by scanning through the input map of increasing values
//!
//! This function assumes that the inputMap refers to a map that has steadily
//! increasing values
//!
///////////////////////////////////////////////////////////////////////////////
static uint16_t MapInputValueIncreasing(
    uint16_t        value,
    const uint16_t* inputMap,
    const uint16_t* outputMap )
{
    //
    // Check to see if we are below the bottom map bin
    //
    if ( value < inputMap[ 0 ] )
    {
        //
        // Clamp at the bottom output map value
        //
        return outputMap[ 0 ];
    }

    //
    // Scan to find bounding bin
    //
    uint8_t lowerBin = 0;
    bool    found = false;
    while ( !found && lowerBin < MAPSIZE - 1 )
    {
        if ( value >= inputMap[ lowerBin ] && value < inputMap[ lowerBin + 1 ] )
        {
            found = true;
        }
        else
        {
            lowerBin++;
        }
    }

    //
    // If we haven't found a value we must be above the top map bin
    //
    if ( !found )
    {
        //
        // Clamp at the top output map value
        //
        return outputMap[ lowerBin ];
    }

    //
    // Having found the input bins we can interpolate between them
    //
    return InterpolateBinValue(
        value, lowerBin, lowerBin + 1, inputMap, outputMap );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Map a value by scanning the input map of decreasing values
//!
//! This function assumes that the inputMap refers to a map that has steadily
//! decreasing values
//!
///////////////////////////////////////////////////////////////////////////////
static uint16_t MapInputValueDecreasing(
    uint16_t        value,
    const uint16_t* inputMap,
    const uint16_t* outputMap )
{
    //
    // Check to see if we are above the bottom map bin
    //
    if ( value > inputMap[ 0 ] )
    {
        //
        // Clamp at the bottom output map value
        //
        return outputMap[ 0 ];
    }

    //
    // Scan to find bounding bin
    //
    uint8_t lowerBin = 0;
    bool    found = false;
    while ( !found && lowerBin < MAPSIZE - 1 )
    {
        if ( value < inputMap[ lowerBin ] && value >= inputMap[ lowerBin + 1 ] )
        {
            found = true;
        }
        else
        {
            lowerBin++;
        }
    }

    //
    // If we haven't found a value we must be below the top map bin
    //
    if ( !found )
    {
        //
        // Clamp at the top output map value
        //
        return outputMap[ lowerBin ];
    }

    //
    // Having found the input bins we can interpolate between them
    //
    return InterpolateBinValue(
        value, lowerBin, lowerBin + 1, inputMap, outputMap );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Map a value using the supplied map bins as input values
//!
//! The value supplied is used to identify which bins constrain it. The bins
//! dictate the values to be output. The value output is a linear interpolation
//! between the values. It is assumed that map is not NULL and has 9 bins
//!
///////////////////////////////////////////////////////////////////////////////
uint16_t MapValue(
    uint16_t        value,
    const uint16_t* inputMap,
    const uint16_t* outputMap )
{
    //
    // Does our input map count up or down
    // This is super naive and will likely not work for a complex map
    //
    if ( inputMap[ 0 ] < inputMap[ MAPSIZE - 1 ] )
    {
        return MapInputValueIncreasing( value, inputMap, outputMap );
    }
    else
    {
        return MapInputValueDecreasing( value, inputMap, outputMap );
    }
}
