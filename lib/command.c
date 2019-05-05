///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Command processor for the Fuel Gauge
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//
// Command grammar:
//
//   Command (1 character)
//   Fields (space separated)
//
// Commands:
//
// p                - Program mode
// r                - Run mode
// d                - Display current tank input value and output gauge value
// g <Value>        - Output raw gauge value
// t                - One shot test map the current tank input to the gauge
//                    output
// i <Bin> <Value>  - Set the input bin number to a specific linear tank
//                    value
// o <Bin> <Value>  - Set the output bin number to a specific value
// m                - Display the input and output maps
// s                - Save input and output maps to persistent storage
// l                - Load input and output maps from persistent storage
//

//
//! Flag to indicate whether we are running or programming the gauge
//
static bool g_running = true;

//
//! Input mapping between the tank value and a linear actual value
//
static uint16_t s_inputMap[ MAPSIZE ];

//
//! Output mapping between a linear actual value and the guage output
//
static uint16_t s_outputMap[ MAPSIZE ];

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Check whether the Fuel Gauge is in programming or run mode
//!
///////////////////////////////////////////////////////////////////////////////
bool IsRunning()
{
    return g_running;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Print a short uint16_t value as text to the console
//!
///////////////////////////////////////////////////////////////////////////////
static void PrintValue( uint16_t value )
{
    char buf[ 5 ];

    for ( uint8_t i = 4; i-- > 0; )
    {
        uint8_t nibble = value & 0xF;
        if ( nibble < 0xA )
        {
            buf[ i ] = '0' + nibble;
        }
        else
        {
            buf[ i ] = 'a' + nibble - 0xA;
        }

        value = value >> 4;
    }
    buf[ 4 ] = '\0';

    HAL_PrintText( buf );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Print a bin number as text to the console
//!
///////////////////////////////////////////////////////////////////////////////
static void PrintBin( uint8_t bin )
{
    if ( bin <= MAPSIZE )
    {
        char buf[ 2 ];
        buf[ 0 ] = '0' + bin;
        buf[ 1 ] = '\0';
        HAL_PrintText( buf );
    }
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Display current tank input value and output gauge value
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessDisplayCommand()
{
    HAL_PrintText( "Tank: 0x" );
    PrintValue( HAL_GetTankInput() );
    HAL_PrintText( " Gauge: 0x" );
    PrintValue( HAL_GetGaugeOutput() );
    HAL_PrintNewline();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Set the gauge output to a specific value
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessGaugeOutputCommand( const char* command )
{
    //
    // Fail immediately if we are running
    //
    if ( g_running )
    {
        return false;
    }

    uint16_t output;
    int      ret = sscanf( command, "%hx", &output );

    if ( ret == 1 )
    {
        HAL_SetGaugeOutput( output );
        return true;
    }
    else
    {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Run a one-shot mapping of the current tank input to gauge output
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessMapping( bool logging )
{
    uint16_t input = HAL_GetTankInput();

    uint16_t actual = MapValue( s_inputMap, input );

    uint16_t output = MapValue( s_outputMap, actual );

    HAL_SetGaugeOutput( output );

    if ( logging )
    {
        HAL_PrintText( "Tank: 0x" );
        PrintValue( input );
        HAL_PrintText( " Actual: 0x" );
        PrintValue( actual );
        HAL_PrintText( " Gauge: 0x" );
        PrintValue( output );
        HAL_PrintNewline();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Load our input and output maps
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessLoadCommand()
{
    return HAL_LoadMaps( s_inputMap, s_outputMap );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Save our input and output maps
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessSaveCommand()
{
    return HAL_SaveMaps( s_inputMap, s_outputMap );
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Display the contents of our input and output maps
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessMapDisplayCommand()
{
    for ( int i = 0; i < MAPSIZE; i++ )
    {
        HAL_PrintText( "Input[" );
        PrintBin( i );
        HAL_PrintText( "] : 0x" );
        PrintValue( s_inputMap[ i ] );
        HAL_PrintNewline();
    }

    for ( int i = 0; i < MAPSIZE; i++ )
    {
        HAL_PrintText( "Output[" );
        PrintBin( i );
        HAL_PrintText( "] : 0x" );
        PrintValue( s_outputMap[ i ] );
        HAL_PrintNewline();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Modify a value in a specific bin in a given map
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessModifyMapValueCommand( const char* command, uint16_t* map )
{
    uint8_t  bin;
    uint16_t value;

    int ret = sscanf( command, "%hhd %hx", &bin, &value );

    //
    // bail if we don't have two values read successfully
    //
    if ( ret != 2 )
    {
        return false;
    }

    //
    // Range check the bin value
    //
    if ( bin >= MAPSIZE )
    {
        return false;
    }

    //
    // With valid input we can now modify the map
    //
    map[ bin ] = value;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Process a command
//!
///////////////////////////////////////////////////////////////////////////////
bool ProcessCommand( const char* command )
{
    size_t len = strlen( command );

    //
    // Barf if we have an empty string
    //
    if ( len == 0 )
    {
        return false;
    }

    bool result = false;

    switch ( command[ 0 ] )
    {
    case 'p':
        g_running = false;
        result = true;
        break;

    case 'r':
        g_running = true;
        result = true;
        break;

    case 'd':
        result = ProcessDisplayCommand();
        break;
    case 'g':
        result = ProcessGaugeOutputCommand( &command[ 1 ] );
        break;
    case 't':
        // Read the input and map with logging
        result = ProcessMapping( true );
        break;
    case 'i':
        result = ProcessModifyMapValueCommand( &command[ 1 ], s_inputMap );
        break;
    case 'o':
        result = ProcessModifyMapValueCommand( &command[ 1 ], s_outputMap );
        break;
    case 'm':
        result = ProcessMapDisplayCommand();
        break;
    case 's':
        result = ProcessSaveCommand();
        break;
    case 'l':
        result = ProcessLoadCommand();
        break;

    default:
        break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Run the guage by reading the tank input, mapping and then changing
//!         the output
//!
//! This runs the mapping process once before returning. It is envisaged this
//! is run periodically from a main loop
//!
///////////////////////////////////////////////////////////////////////////////
void RunGauge( void )
{
    //
    // Run the mapping command but with logging turned off
    //
    ProcessMapping( false );
}
