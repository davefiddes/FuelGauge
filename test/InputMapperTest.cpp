///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Unit test the input value mapper module
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

#include "gtest/gtest.h"
#include <stdint.h>

#include "mapper.h"

const uint16_t LinearFullScale[ MAPSIZE ] = { 0x0000, 0x2000, 0x4000,
                                              0x6000, 0x8000, 0xA000,
                                              0xC000, 0xE000, 0xFFFF };

// Check that mapping works with a simple linear map
TEST( InputMapper, LinearFullScale )
{
    EXPECT_EQ( MapValue( 0x0000, LinearFullScale, LinearFullScale ), 0x0000 );
    EXPECT_EQ( MapValue( 0x2000, LinearFullScale, LinearFullScale ), 0x2000 );
    EXPECT_EQ( MapValue( 0x1000, LinearFullScale, LinearFullScale ), 0x1000 );
    EXPECT_EQ( MapValue( 0x3000, LinearFullScale, LinearFullScale ), 0x3000 );
    EXPECT_EQ( MapValue( 0x8000, LinearFullScale, LinearFullScale ), 0x8000 );
    EXPECT_EQ( MapValue( 0x0001, LinearFullScale, LinearFullScale ), 0x0001 );
    EXPECT_EQ( MapValue( 0xFFFF, LinearFullScale, LinearFullScale ), 0xFFFF );
    EXPECT_EQ( MapValue( 0xC100, LinearFullScale, LinearFullScale ), 0xC100 );
    EXPECT_EQ( MapValue( 0xF000, LinearFullScale, LinearFullScale ), 0xF000 );
}

const uint16_t LinearHalf[ MAPSIZE ] = { 0x0000, 0x1000, 0x2000, 0x3000, 0x4000,
                                         0x5000, 0x6000, 0x7000, 0x8000 };

// Check that mapping works with a map that should double our input values
TEST( InputMapper, LinearHalf )
{
    EXPECT_EQ( MapValue( 0x0000, LinearHalf, LinearFullScale ), 0x0000 );
    EXPECT_EQ( MapValue( 0x2000, LinearHalf, LinearFullScale ), 0x4000 );
    EXPECT_EQ( MapValue( 0x2800, LinearHalf, LinearFullScale ), 0x5000 );
    EXPECT_EQ( MapValue( 0x3000, LinearHalf, LinearFullScale ), 0x6000 );
    EXPECT_EQ( MapValue( 0x8000, LinearHalf, LinearFullScale ), 0xffff );
    EXPECT_EQ( MapValue( 0x0001, LinearHalf, LinearFullScale ), 0x0002 );
    EXPECT_EQ( MapValue( 0x0002, LinearHalf, LinearFullScale ), 0x0004 );
    EXPECT_EQ( MapValue( 0xffff, LinearHalf, LinearFullScale ), 0xffff );
    EXPECT_EQ( MapValue( 0x8001, LinearHalf, LinearFullScale ), 0xffff );
    // Result not quite 2x 0x7800 as full-scale is 0xffff
    EXPECT_EQ( MapValue( 0x7800, LinearHalf, LinearFullScale ), 0xefff );
}

const uint16_t LinearHalfOffset[ MAPSIZE ] = { 0x3000, 0x4000, 0x5000,
                                               0x6000, 0x7000, 0x8000,
                                               0x9000, 0xa000, 0xb000 };

// Check that mapping works with a map that should double our input values
// and remove an offset from the input
TEST( InputMapper, LinearHalfOffset )
{
    EXPECT_EQ( MapValue( 0x0000, LinearHalfOffset, LinearFullScale ), 0x0000 );
    EXPECT_EQ( MapValue( 0x3000, LinearHalfOffset, LinearFullScale ), 0x0000 );
    EXPECT_EQ( MapValue( 0x5000, LinearHalfOffset, LinearFullScale ), 0x4000 );
    EXPECT_EQ( MapValue( 0x5800, LinearHalfOffset, LinearFullScale ), 0x5000 );
    EXPECT_EQ( MapValue( 0xb000, LinearHalfOffset, LinearFullScale ), 0xffff );
    EXPECT_EQ( MapValue( 0x6001, LinearHalfOffset, LinearFullScale ), 0x6002 );
    EXPECT_EQ( MapValue( 0x6002, LinearHalfOffset, LinearFullScale ), 0x6004 );
    EXPECT_EQ( MapValue( 0xffff, LinearHalfOffset, LinearFullScale ), 0xffff );
    EXPECT_EQ( MapValue( 0xb001, LinearHalfOffset, LinearFullScale ), 0xffff );
    // Result not quite 2x 0x7800+0x300 as full-scale is 0xffff
    EXPECT_EQ( MapValue( 0xa800, LinearHalfOffset, LinearFullScale ), 0xefff );
}

const uint16_t LinearInverseHalfOffset[ MAPSIZE ] = { 0xb000, 0xa000, 0x9000,
                                                      0x8000, 0x7000, 0x6000,
                                                      0x5000, 0x4000, 0x3000 };

// Check that mapping works with a map that should double our input values
// and remove an offset from the input and is also the inverse of the input
TEST( InputMapper, LinearInverseHalfOffset )
{
    EXPECT_EQ(
        MapValue( 0x0000, LinearInverseHalfOffset, LinearFullScale ), 0xffff );
    EXPECT_EQ(
        MapValue( 0x2000, LinearInverseHalfOffset, LinearFullScale ), 0xffff );
    // Result 1 less than expected because it is in the e000-ffff bin
    EXPECT_EQ(
        MapValue( 0x3800, LinearInverseHalfOffset, LinearFullScale ), 0xefff );
    EXPECT_EQ(
        MapValue( 0x4000, LinearInverseHalfOffset, LinearFullScale ), 0xe000 );
    EXPECT_EQ(
        MapValue( 0x8000, LinearInverseHalfOffset, LinearFullScale ), 0x6000 );
    EXPECT_EQ(
        MapValue( 0x0001, LinearInverseHalfOffset, LinearFullScale ), 0xffff );
    EXPECT_EQ(
        MapValue( 0x0002, LinearInverseHalfOffset, LinearFullScale ), 0xffff );
    EXPECT_EQ(
        MapValue( 0xffff, LinearInverseHalfOffset, LinearFullScale ), 0x0000 );
    EXPECT_EQ(
        MapValue( 0x8001, LinearInverseHalfOffset, LinearFullScale ), 0x5ffe );
    EXPECT_EQ(
        MapValue( 0x7800, LinearInverseHalfOffset, LinearFullScale ), 0x7000 );
}

const uint16_t RealInputMap[ MAPSIZE ] = { 0xbb9f, 0xb6b9, 0xa2e1,
                                           0x8f39, 0x7abc, 0x667b,
                                           0x4d7e, 0x2cfc, 0x0bfb };

// Check that mapping works with a real map obtained via tank calibration
TEST( InputMapper, RealInputMap )
{
    // Extra empty tank
    EXPECT_EQ( MapValue( 0xbbff, RealInputMap, LinearFullScale ), 0x0000 );

    // Empty tank
    EXPECT_EQ( MapValue( 0xbb9f, RealInputMap, LinearFullScale ), 0x0000 );

    // Full tank
    EXPECT_EQ( MapValue( 0x0bfb, RealInputMap, LinearFullScale ), 0xffff );

    // Overly full tank
    EXPECT_EQ( MapValue( 0x0480, RealInputMap, LinearFullScale ), 0xffff );
}
