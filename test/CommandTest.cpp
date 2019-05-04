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
#include "mapper.h"

#include "gtest/gtest.h"
#include <memory>
#include <stdarg.h>
#include <string>
#include <vector>

//
// Dummy HAL used for testing
//

//! Current tank input value
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

//! Current gauge output value
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

//! output buffer used to accumulate lines of character output
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

//! A test tank input to linear actual tank value map
uint16_t g_inputMap[ MAPSIZE ];

//! A test linear actual tank value to gauge output value map
uint16_t g_outputMap[ MAPSIZE ];

//! Dummy result to return failures
bool g_result = true;

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Load our test maps into the fuel gauge processor
//!
///////////////////////////////////////////////////////////////////////////////
bool HAL_LoadMaps( uint16_t* input, uint16_t* output )
{
    if ( g_result )
    {
        memcpy( input, &g_inputMap, sizeof( g_inputMap ) );
        memcpy( output, &g_outputMap, sizeof( g_outputMap ) );
    }
    return g_result;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Save the supplied maps into our test maps for checking
//!
///////////////////////////////////////////////////////////////////////////////
bool HAL_SaveMaps( const uint16_t* input, const uint16_t* output )
{
    if ( g_result )
    {
        memcpy( &g_inputMap, input, sizeof( g_inputMap ) );
        memcpy( &g_outputMap, output, sizeof( g_outputMap ) );
    }
    return g_result;
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
    EXPECT_STREQ( g_output[ 0 ].c_str(), "Tank: 0x1234 Gauge: 0x5678\n" );

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

const uint16_t ZeroMap[ MAPSIZE ] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
                                      0x0000, 0x0000, 0x0000, 0x0000 };

const uint16_t LinearOneToOne[ MAPSIZE ] = { 0x0000, 0x2000, 0x4000,
                                             0x6000, 0x8000, 0xA000,
                                             0xC000, 0xE000, 0xFFFF };

const uint16_t LinearInverse[ MAPSIZE ] = { 0xFFFF, 0xE000, 0xC000,
                                            0xA000, 0x8000, 0x6000,
                                            0x4000, 0x2000, 0x0000 };

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test one shot value mapping - linear input / reverse output map
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, OneShotValueMapping )
{
    //
    // Clear the output log
    //
    g_output.clear();

    //
    // Set the tank input and gauge output to known values
    //
    g_tank = 0x1234;
    g_gauge = 0x5678;

    //
    // Cue up some maps and then ask them to be loaded
    //
    memcpy( &g_inputMap, LinearOneToOne, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, LinearInverse, sizeof( g_outputMap ) );
    ASSERT_TRUE( ProcessCommand( "l" ) );

    //
    // Try a one-shot mapping
    //
    ASSERT_TRUE( ProcessCommand( "t" ) );
    ASSERT_EQ( g_gauge, 0xedcb );
    ASSERT_EQ( g_output.size(), 1 );
    EXPECT_STREQ(
        g_output[ 0 ].c_str(), "Tank: 0x1234 Actual: 0x1234 Gauge: 0xedcb\n" );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test one shot value mapping - reverse input / linear output map
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, OneShotValueMappingReverse )
{
    //
    // Clear the output log
    //
    g_output.clear();

    //
    // Set the tank input and gauge output to known values
    //
    g_tank = 0x1234;
    g_gauge = 0x5678;

    //
    // Cue up some maps and then ask them to be loaded
    //
    memcpy( &g_inputMap, LinearInverse, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, LinearOneToOne, sizeof( g_outputMap ) );
    ASSERT_TRUE( ProcessCommand( "l" ) );

    //
    // Try a one-shot mapping - slightly different from other test due to
    // distortion in the upper bin of the one to one map
    //
    ASSERT_TRUE( ProcessCommand( "t" ) );
    ASSERT_EQ( g_gauge, 0xedca );
    ASSERT_EQ( g_output.size(), 1 );
    EXPECT_STREQ(
        g_output[ 0 ].c_str(), "Tank: 0x1234 Actual: 0xedcb Gauge: 0xedca\n" );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test the setting of input and output map values
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, MapLoadAndSave )
{
    //
    // Cue up some maps
    //
    memcpy( &g_inputMap, LinearOneToOne, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, LinearInverse, sizeof( g_outputMap ) );

    //
    // Request that loading fails
    //
    g_result = false;
    ASSERT_FALSE( ProcessCommand( "l" ) );

    //
    // Load in our maps successfully
    //
    g_result = true;
    ASSERT_TRUE( ProcessCommand( "l" ) );

    //
    // Request the maps to be saved - ensure HAL fails the request
    //
    memcpy( &g_inputMap, ZeroMap, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, ZeroMap, sizeof( g_outputMap ) );
    g_result = false;
    ASSERT_FALSE( ProcessCommand( "s" ) );

    //
    // Verify the have not been saved
    //
    ASSERT_TRUE( memcmp( g_inputMap, ZeroMap, sizeof( LinearOneToOne ) ) == 0 );
    ASSERT_TRUE( memcmp( g_outputMap, ZeroMap, sizeof( LinearInverse ) ) == 0 );

    //
    // Request the maps to be saved - ensure HAL succeeds
    //
    g_result = true;
    ASSERT_TRUE( ProcessCommand( "s" ) );

    //
    // Verify the maps being saved match those loaded
    //
    ASSERT_TRUE(
        memcmp( g_inputMap, LinearOneToOne, sizeof( LinearOneToOne ) ) == 0 );
    ASSERT_TRUE(
        memcmp( g_outputMap, LinearInverse, sizeof( LinearInverse ) ) == 0 );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test the display of input and output map values
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, MapDisplay )
{
    //
    // Cue up some maps
    //
    memcpy( &g_inputMap, LinearOneToOne, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, LinearInverse, sizeof( g_outputMap ) );

    //
    // Load in our maps successfully
    //
    g_result = true;
    ASSERT_TRUE( ProcessCommand( "l" ) );

    //
    // Request to display the maps
    //
    g_output.clear();
    ASSERT_TRUE( ProcessCommand( "m" ) );

    //
    // Verify the output matches the two maps we loaded
    //
    ASSERT_EQ( g_output.size(), 18 );
    EXPECT_STREQ( g_output[ 0 ].c_str(), "Input[0] : 0\n" ); // zero is special
    EXPECT_STREQ( g_output[ 1 ].c_str(), "Input[1] : 0x2000\n" );
    EXPECT_STREQ( g_output[ 2 ].c_str(), "Input[2] : 0x4000\n" );
    EXPECT_STREQ( g_output[ 3 ].c_str(), "Input[3] : 0x6000\n" );
    EXPECT_STREQ( g_output[ 4 ].c_str(), "Input[4] : 0x8000\n" );
    EXPECT_STREQ( g_output[ 5 ].c_str(), "Input[5] : 0xa000\n" );
    EXPECT_STREQ( g_output[ 6 ].c_str(), "Input[6] : 0xc000\n" );
    EXPECT_STREQ( g_output[ 7 ].c_str(), "Input[7] : 0xe000\n" );
    EXPECT_STREQ( g_output[ 8 ].c_str(), "Input[8] : 0xffff\n" );

    EXPECT_STREQ( g_output[ 9 ].c_str(), "Output[0] : 0xffff\n" );
    EXPECT_STREQ( g_output[ 10 ].c_str(), "Output[1] : 0xe000\n" );
    EXPECT_STREQ( g_output[ 11 ].c_str(), "Output[2] : 0xc000\n" );
    EXPECT_STREQ( g_output[ 12 ].c_str(), "Output[3] : 0xa000\n" );
    EXPECT_STREQ( g_output[ 13 ].c_str(), "Output[4] : 0x8000\n" );
    EXPECT_STREQ( g_output[ 14 ].c_str(), "Output[5] : 0x6000\n" );
    EXPECT_STREQ( g_output[ 15 ].c_str(), "Output[6] : 0x4000\n" );
    EXPECT_STREQ( g_output[ 16 ].c_str(), "Output[7] : 0x2000\n" );
    EXPECT_STREQ( g_output[ 17 ].c_str(), "Output[8] : 0\n" ); // zero is
                                                               // special
}
