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

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Map a value using the supplied map bins
//!
//! Map a 16-bit full-scale value to a 16-bit. The top 3 bits in the value
//! identify which bins are used to interpolate between.
//! It is assumed that map is not NULL and has 9 bins
//!
///////////////////////////////////////////////////////////////////////////////
uint16_t MapValue( const uint16_t* map, uint16_t value )
{
    uint8_t  lowerBin = value >> 13;
    uint8_t  upperBin = lowerBin + 1;
    uint16_t lowerValue = value & 0xE000;

    //
    // Use uint32_t values to differences to avoid overflowing a uin16_t when
    // multiplying on an 8-bit PIC
    //
    uint32_t valueDiff = value - lowerValue;
    uint32_t binDiff = map[ upperBin ] - map[ lowerBin ];

    uint32_t output = valueDiff * binDiff;

    output = map[ lowerBin ] + ( output >> 13 );

    return (uint16_t)output;
}
