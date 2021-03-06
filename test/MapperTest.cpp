///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Unit test the value mapper module
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

const uint16_t LinearOneToOne[ MAPSIZE ] = { 0x0000, 0x2000, 0x4000,
                                             0x6000, 0x8000, 0xA000,
                                             0xC000, 0xE000, 0xFFFF };

// Check that mapping works with a simple linear map
TEST( Mapper, LinearOneToOne )
{
    ASSERT_EQ( MapValue( 0x0000, LinearOneToOne, LinearOneToOne ), 0x0000 );
    ASSERT_EQ( MapValue( 0x2000, LinearOneToOne, LinearOneToOne ), 0x2000 );
    ASSERT_EQ( MapValue( 0x1000, LinearOneToOne, LinearOneToOne ), 0x1000 );
    ASSERT_EQ( MapValue( 0x3000, LinearOneToOne, LinearOneToOne ), 0x3000 );
    ASSERT_EQ( MapValue( 0x8000, LinearOneToOne, LinearOneToOne ), 0x8000 );
    ASSERT_EQ( MapValue( 0x0001, LinearOneToOne, LinearOneToOne ), 0x0001 );
    ASSERT_EQ( MapValue( 0xFFFF, LinearOneToOne, LinearOneToOne ), 0xFFFF );
    ASSERT_EQ( MapValue( 0xC100, LinearOneToOne, LinearOneToOne ), 0xC100 );
    ASSERT_EQ( MapValue( 0xF000, LinearOneToOne, LinearOneToOne ), 0xF000 );
}

const uint16_t LinearHalf[ MAPSIZE ] = { 0x0000, 0x1000, 0x2000, 0x3000, 0x4000,
                                         0x5000, 0x6000, 0x7000, 0x8000 };

// Check that mapping works with a simple linear map
TEST( Mapper, LinearHalf )
{
    ASSERT_EQ( MapValue( 0x0000, LinearOneToOne, LinearHalf ), 0x0000 );
    ASSERT_EQ( MapValue( 0x2000, LinearOneToOne, LinearHalf ), 0x1000 );
    ASSERT_EQ( MapValue( 0x1000, LinearOneToOne, LinearHalf ), 0x0800 );
    ASSERT_EQ( MapValue( 0x3000, LinearOneToOne, LinearHalf ), 0x1800 );
    ASSERT_EQ( MapValue( 0x8000, LinearOneToOne, LinearHalf ), 0x4000 );
    ASSERT_EQ( MapValue( 0x0001, LinearOneToOne, LinearHalf ), 0x0000 );
    ASSERT_EQ( MapValue( 0x0002, LinearOneToOne, LinearHalf ), 0x0001 );
    ASSERT_EQ( MapValue( 0xFFFF, LinearOneToOne, LinearHalf ), 0x8000 );
    ASSERT_EQ( MapValue( 0xC100, LinearOneToOne, LinearHalf ), 0x6080 );
    ASSERT_EQ( MapValue( 0xF000, LinearOneToOne, LinearHalf ), 0x7800 );
}
