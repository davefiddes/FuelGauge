///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Unit test the command processor
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

#include "command.h"
#include "hal.h"

#include "gtest/gtest.h"
#include <memory>
#include <stdarg.h>
#include <string>
#include <vector>

//
// Dummy HAL used for testing
//

// Current tank input value
uint16_t g_tank;

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Return the current tank input value
//!
///////////////////////////////////////////////////////////////////////////////
uint16_t HAL_GetTankInput()
{
    return g_tank;
}

// Current gauge output value
uint16_t g_gauge;

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Return the last output value of the guage
//!
///////////////////////////////////////////////////////////////////////////////
uint16_t HAL_GetGaugeOutput()
{
    return g_gauge;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Store the value the gauge should be set to output
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_SetGaugeOutput( uint16_t value )
{
    g_gauge = value;
}

// output buffer used to accumulate lines of character output
std::vector< std::string > g_output;

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Print into a string buffer array to allow output to be compared
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_Printf( const char* format, ... )
{
    va_list args, args_size;
    va_start( args, format );

    // Take a copy of the args so we can determine the size of our output buffer
    va_copy( args_size, args );
    size_t size =
        vsnprintf( nullptr, 0, format, args_size ) + 1; // Extra space for '\0'
    std::unique_ptr< char[] > buf( new char[ size ] );

    vsnprintf( buf.get(), size, format, args );

    va_end( args );
    va_end( args_size );

    // We don't want the '\0' inside the string
    g_output.push_back( std::string( buf.get(), buf.get() + size - 1 ) );
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test switching between Run and Program modes
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, RunProgramSwitching )
{
    // Basic flipping between Run and Program
    ASSERT_TRUE( IsRunning() );
    ASSERT_TRUE( ProcessCommand( "p" ) );
    ASSERT_FALSE( IsRunning() );
    ASSERT_TRUE( ProcessCommand( "r" ) );
    ASSERT_TRUE( IsRunning() );

    // Run and Program should be idempotent
    ASSERT_TRUE( IsRunning() );
    ASSERT_TRUE( ProcessCommand( "r" ) );
    ASSERT_TRUE( IsRunning() );

    ASSERT_TRUE( ProcessCommand( "p" ) );
    ASSERT_FALSE( IsRunning() );
    ASSERT_TRUE( ProcessCommand( "p" ) );
    ASSERT_FALSE( IsRunning() );

    // Anything after the command character should be ignored
    ASSERT_TRUE( ProcessCommand( "r " ) );
    ASSERT_TRUE( IsRunning() );
    ASSERT_TRUE( ProcessCommand( "pr" ) );
    ASSERT_FALSE( IsRunning() );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test reading tank readings and set gauge output values
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, InputOutput )
{
    // Set the external conditions
    g_output.clear();
    g_tank = 0x1234;
    g_gauge = 0x5678;

    // Ensure we are Running
    ASSERT_TRUE( ProcessCommand( "r" ) );

    // Display the current tank and gauge values
    ASSERT_TRUE( ProcessCommand( "d" ) );
    ASSERT_EQ( g_output.size(), 1 );
    ASSERT_STREQ( g_output[ 0 ].c_str(), "Tank: 0x1234 Gauge: 0x5678\n" );

    // Attempt to set the gauge output (this will fail in Run mode)
    ASSERT_FALSE( ProcessCommand( "g 1234" ) );

    // Change to programming mode to test the gauge output
    ASSERT_TRUE( ProcessCommand( "p" ) );
    ASSERT_FALSE( IsRunning() );

    // Check that invalid gauge output commands fail
    ASSERT_FALSE( ProcessCommand( "g" ) );
    ASSERT_FALSE( ProcessCommand( "g " ) );
    ASSERT_FALSE( ProcessCommand( "g qwio" ) );

    // Check that gauge output works and additional input is ignored
    ASSERT_TRUE( ProcessCommand( "g fedc" ) );
    ASSERT_EQ( g_gauge, 0xfedc );
    ASSERT_TRUE( ProcessCommand( "g 123456789" ) );
    ASSERT_EQ( g_gauge, 0x6789 );
}
