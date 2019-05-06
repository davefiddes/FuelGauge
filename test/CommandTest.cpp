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
std::string                g_currentLine;

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Print to our current line
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_PrintText( const char* text )
{
    g_currentLine.append( text );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Print to our current line
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_PrintNewline()
{
    g_output.push_back( g_currentLine );
    g_currentLine.clear();
}

//! A test tank input to linear actual tank value map
uint16_t g_inputMap[ MAPSIZE ];

//! A test linear actual tank value to gauge output value map
uint16_t g_outputMap[ MAPSIZE ];

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Load our test maps into the fuel gauge processor
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_LoadMaps( uint16_t* input, uint16_t* output )
{
    memcpy( input, &g_inputMap, sizeof( g_inputMap ) );
    memcpy( output, &g_outputMap, sizeof( g_outputMap ) );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Save the supplied maps into our test maps for checking
//!
///////////////////////////////////////////////////////////////////////////////
void HAL_SaveMaps( const uint16_t* input, const uint16_t* output )
{
    memcpy( &g_inputMap, input, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, output, sizeof( g_outputMap ) );
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test switching between Run and Program modes
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, RunProgramSwitching )
{
    InitialiseGauge();

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
    EXPECT_STREQ(
        g_output[ 0 ].c_str(), "Tank: 0x1234 Gauge: 0x5678 Mode: Run" );

    // Attempt to set the gauge output (this will fail in Run mode)
    ASSERT_FALSE( ProcessCommand( "g 1234" ) );

    // Change to programming mode to test the gauge output
    ASSERT_TRUE( ProcessCommand( "p" ) );
    ASSERT_FALSE( IsRunning() );

    //
    // Check the display is updated after switching modes
    //
    g_output.clear();
    ASSERT_TRUE( ProcessCommand( "d" ) );
    ASSERT_EQ( g_output.size(), 1 );
    EXPECT_STREQ(
        g_output[ 0 ].c_str(), "Tank: 0x1234 Gauge: 0x5678 Mode: Program" );

    // Check that invalid gauge output commands fail
    ASSERT_FALSE( ProcessCommand( "g" ) );
    ASSERT_FALSE( ProcessCommand( "g " ) );
    ASSERT_FALSE( ProcessCommand( "g qwio" ) );

    // Check that gauge output works and additional input is ignored
    ASSERT_TRUE( ProcessCommand( "g fedc" ) );
    ASSERT_EQ( g_gauge, 0xfedc );
    ASSERT_TRUE( ProcessCommand( "g 123456789" ) );
    ASSERT_EQ( g_gauge, 0x6789 );
    ASSERT_TRUE( ProcessCommand( "g1234" ) );
    ASSERT_EQ( g_gauge, 0x1234 );
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
        g_output[ 0 ].c_str(), "Tank: 0x1234 Actual: 0x1234 Gauge: 0xedcb" );
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
        g_output[ 0 ].c_str(), "Tank: 0x1234 Actual: 0xedcb Gauge: 0xedca" );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test normal running of the gauge
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, RunGauge )
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
    InitialiseGauge();

    //
    // Run the gauge and verify the output changes to match the input based on
    // the map
    //
    g_tank = 0x1234;
    RunGauge();
    ASSERT_EQ( g_gauge, 0xedca );
    ASSERT_TRUE( g_output.empty() );

    g_tank = 0x3000;
    RunGauge();
    ASSERT_EQ( g_gauge, 0xd000 );
    ASSERT_TRUE( g_output.empty() );

    g_tank = 0xC100;
    RunGauge();
    ASSERT_EQ( g_gauge, 0x3f00 );
    ASSERT_TRUE( g_output.empty() );

    g_tank = 0x1234;
    RunGauge();
    ASSERT_EQ( g_gauge, 0xedca );
    ASSERT_TRUE( g_output.empty() );

    //
    // Change to Program mode and check that runing the gauge does nothing
    //
    ASSERT_TRUE( ProcessCommand( "p" ) );
    g_tank = 0x3000;
    RunGauge();
    ASSERT_EQ( g_gauge, 0xedca );
    ASSERT_FALSE( IsRunning() );
    ASSERT_TRUE( g_output.empty() );
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
    // Load in our maps
    //
    ASSERT_TRUE( ProcessCommand( "l" ) );

    //
    // Zero out the save destination before saving
    //
    memcpy( &g_inputMap, ZeroMap, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, ZeroMap, sizeof( g_outputMap ) );

    //
    // Request the maps to be saved
    //
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
    // Load in our maps
    //
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
    EXPECT_STREQ( g_output[ 0 ].c_str(), "Input[0] : 0x0000" );
    EXPECT_STREQ( g_output[ 1 ].c_str(), "Input[1] : 0x2000" );
    EXPECT_STREQ( g_output[ 2 ].c_str(), "Input[2] : 0x4000" );
    EXPECT_STREQ( g_output[ 3 ].c_str(), "Input[3] : 0x6000" );
    EXPECT_STREQ( g_output[ 4 ].c_str(), "Input[4] : 0x8000" );
    EXPECT_STREQ( g_output[ 5 ].c_str(), "Input[5] : 0xa000" );
    EXPECT_STREQ( g_output[ 6 ].c_str(), "Input[6] : 0xc000" );
    EXPECT_STREQ( g_output[ 7 ].c_str(), "Input[7] : 0xe000" );
    EXPECT_STREQ( g_output[ 8 ].c_str(), "Input[8] : 0xffff" );

    EXPECT_STREQ( g_output[ 9 ].c_str(), "Output[0] : 0xffff" );
    EXPECT_STREQ( g_output[ 10 ].c_str(), "Output[1] : 0xe000" );
    EXPECT_STREQ( g_output[ 11 ].c_str(), "Output[2] : 0xc000" );
    EXPECT_STREQ( g_output[ 12 ].c_str(), "Output[3] : 0xa000" );
    EXPECT_STREQ( g_output[ 13 ].c_str(), "Output[4] : 0x8000" );
    EXPECT_STREQ( g_output[ 14 ].c_str(), "Output[5] : 0x6000" );
    EXPECT_STREQ( g_output[ 15 ].c_str(), "Output[6] : 0x4000" );
    EXPECT_STREQ( g_output[ 16 ].c_str(), "Output[7] : 0x2000" );
    EXPECT_STREQ( g_output[ 17 ].c_str(), "Output[8] : 0x0000" );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test the modification of the input map
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, ModifyInputMap )
{
    //
    // Cue up some maps and load them in
    //
    memcpy( &g_inputMap, LinearOneToOne, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, LinearInverse, sizeof( g_outputMap ) );
    ASSERT_TRUE( ProcessCommand( "l" ) );

    //
    // Try some failed input modification commands
    //
    ASSERT_FALSE( ProcessCommand( "i" ) );         // no params
    ASSERT_FALSE( ProcessCommand( "i 1" ) );       // bin but no value
    ASSERT_FALSE( ProcessCommand( "i -1 1234" ) ); // invalid bin negative
    ASSERT_FALSE( ProcessCommand( "i 10 1234" ) ); // invalid bin too large
    ASSERT_FALSE( ProcessCommand( "i 3 nmkl" ) );  // invalid value

    //
    // Modify a few bins
    //
    ASSERT_TRUE( ProcessCommand( "i 0 1234" ) );
    ASSERT_TRUE( ProcessCommand( "i 1 5678" ) );
    ASSERT_TRUE( ProcessCommand( "i 8 cdef" ) );

    //
    // Request the maps to be saved so we can see the contents
    //
    ASSERT_TRUE( ProcessCommand( "s" ) );

    //
    // Verify the maps have changed as expected
    //
    ASSERT_EQ( g_inputMap[ 0 ], 0x1234 );
    ASSERT_EQ( g_inputMap[ 1 ], 0x5678 );
    ASSERT_EQ( g_inputMap[ 2 ], 0x4000 ); // unmodified
    ASSERT_EQ( g_inputMap[ 7 ], 0xe000 ); // unmodified
    ASSERT_EQ( g_inputMap[ 8 ], 0xcdef );

    //
    // Sanity check that the output map has not changed at all
    //
    ASSERT_TRUE(
        memcmp( g_outputMap, LinearInverse, sizeof( LinearInverse ) ) == 0 );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test the modification of the output map
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, ModifyOutputMap )
{
    //
    // Cue up some maps and load them in
    //
    memcpy( &g_inputMap, LinearOneToOne, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, LinearInverse, sizeof( g_outputMap ) );
    ASSERT_TRUE( ProcessCommand( "l" ) );

    //
    // Try some failed input modification commands
    //
    ASSERT_FALSE( ProcessCommand( "o" ) );         // no params
    ASSERT_FALSE( ProcessCommand( "o 6" ) );       // bin but no value
    ASSERT_FALSE( ProcessCommand( "o -2 1234" ) ); // invalid bin negative
    ASSERT_FALSE( ProcessCommand( "o 10 1234" ) ); // invalid bin too large
    ASSERT_FALSE( ProcessCommand( "o 5 poiu" ) );  // invalid value

    //
    // Modify a few bins
    //
    ASSERT_TRUE( ProcessCommand( "o 0 1234" ) );
    ASSERT_TRUE( ProcessCommand( "o 2 5678" ) );
    ASSERT_TRUE( ProcessCommand( "o 7 cdef" ) );

    //
    // Request the maps to be saved so we can see the contents
    //
    ASSERT_TRUE( ProcessCommand( "s" ) );

    //
    // Verify the maps have changed as expected
    //
    ASSERT_EQ( g_outputMap[ 0 ], 0x1234 );
    ASSERT_EQ( g_outputMap[ 1 ], 0xe000 ); // unmodified
    ASSERT_EQ( g_outputMap[ 2 ], 0x5678 );
    ASSERT_EQ( g_outputMap[ 6 ], 0x4000 ); // unmodified
    ASSERT_EQ( g_outputMap[ 7 ], 0xcdef );
    ASSERT_EQ( g_outputMap[ 8 ], 0x0000 ); // unmodified

    //
    // Sanity check that the input map has not changed at all
    //
    ASSERT_TRUE(
        memcmp( g_inputMap, LinearOneToOne, sizeof( LinearOneToOne ) ) == 0 );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Test the start up of the gauge
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, Initialisation )
{
    //
    // Cue up some maps
    //
    memcpy( &g_inputMap, LinearOneToOne, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, LinearInverse, sizeof( g_outputMap ) );

    //
    // Load in our maps successfully on initialisation
    //
    InitialiseGauge();
    ASSERT_TRUE( IsRunning() );

    //
    // Check that the maps loaded during initialisation are correct
    //
    memcpy( &g_inputMap, ZeroMap, sizeof( g_inputMap ) );
    memcpy( &g_outputMap, ZeroMap, sizeof( g_outputMap ) );
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
//! \brief  Test usage information
//!
///////////////////////////////////////////////////////////////////////////////
TEST( Command, DisplayUsage )
{
    //
    // Very noddy check of the usage information display
    //
    g_output.clear();
    ASSERT_TRUE( ProcessCommand( "u" ) );
    ASSERT_EQ( g_output.size(), 1 );
}
