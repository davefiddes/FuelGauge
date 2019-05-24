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
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
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
static bool s_running;

//
//! Input mapping between the tank value and a linear actual value
//
static uint16_t s_inputMap[ MAPSIZE ];

//
//! Output mapping between a linear actual value and the gauge output
//
static uint16_t s_outputMap[ MAPSIZE ];

//
//! Low fuel warning level - an actual fuel value below this should turn
//! on the low fuel light
//
static uint16_t s_lowFuelLevel;

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
//! \brief  Parse a uint16_t value from the supplied string in hex
//!
///////////////////////////////////////////////////////////////////////////////
static char* ParseValue( const char* str, uint16_t* value )
{
    char     ch;
    bool     success = false;
    uint16_t tempvalue = 0;

    //
    // Skip over any preceeding whitespace
    //
    while ( isspace( *str ) )
    {
        str++;
    }

    //
    // while there are hex digits load them in - extra digits will be ignored
    //
    while ( isxdigit( ch = *str++ ) )
    {
        // Support both upper and lower case hex
        if ( isupper( ch ) )
        {
            ch = tolower( ch );
        }

        if ( isdigit( ch ) )
        {
            ch = ch - '0';
        }
        else
        {
            ch = ch - 'a' + 0xA;
        }

        tempvalue = tempvalue << 4;
        tempvalue = tempvalue + (unsigned char)ch;
        success = true;
    }

    if ( success )
    {
        *value = tempvalue;
        return (char*)str;
    }
    else
    {
        return NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Parse a mapping bin value
//!
//!
///////////////////////////////////////////////////////////////////////////////
static char* ParseBin( const char* str, uint8_t* bin )
{
    char    ch;
    bool    success = false;
    uint8_t tempbin = 0;

    while ( isspace( *str ) )
    {
        str++;
    }

    while ( isdigit( ch = *str++ ) )
    {
        tempbin = tempbin * 10 + ( ch - '0' );
        success = true;
    }

    if ( success )
    {
        *bin = tempbin;
        return (char*)str;
    }
    else
    {
        return NULL;
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
    HAL_PrintText( " Mode: " );
    HAL_PrintText( IsRunning() ? "Run" : "Program" );
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
    if ( s_running )
    {
        return false;
    }

    uint16_t output;

    if ( ParseValue( command, &output ) )
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

    HAL_SetLowFuelLight( actual <= s_lowFuelLevel );

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
    HAL_LoadMaps( s_inputMap, s_outputMap, &s_lowFuelLevel );
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Save our input and output maps
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessSaveCommand()
{
    HAL_SaveMaps( s_inputMap, s_outputMap, s_lowFuelLevel );
    return true;
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

    HAL_PrintText( "Low Fuel Level : 0x" );
    PrintValue( s_lowFuelLevel );
    HAL_PrintNewline();

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

    char* newpos = ParseBin( command, &bin );
    if ( newpos == NULL )
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

    if ( !ParseValue( newpos, &value ) )
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
//! \brief  Display the usage information for the command processor
//!
///////////////////////////////////////////////////////////////////////////////
static void ProcessUsageDisplay()
{
    HAL_PrintText(
        "Usage:\r\n"
        "p\t\t- Program mode\r\n"
        "r\t\t- Run mode\r\n"
        "d\t\t- Display current tank input value and output gauge value\r\n"
        "g <Value>   \t- Output raw gauge value\r\n"
        "t\t\t- One shot test map the current tank input to the gauge "
        "output\r\n"
        "i <Bin> <Value> - Set the input bin number to a specific linear tank "
        "value\r\n"
        "o <Bin> <Value> - Set the output bin number to a specific value\r\n"
        "m\t\t- Display the input and output maps\r\n"
        "s\t\t- Save input and output maps to persistent storage\r\n"
        "l\t\t- Load input and output maps from persistent storage\r\n"
        "u\t\t- This usage information\r\n"
        "f <Value>   \t- Set the low fuel limit\r\n" );
    HAL_PrintNewline();
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Set the low fuel level warning level value
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessLowFuelLevel( const char* command )
{
    //
    // Fail immediately if we are running
    //
    if ( s_running )
    {
        return false;
    }

    uint16_t lowFuelLevel;

    if ( ParseValue( command, &lowFuelLevel ) )
    {
        s_lowFuelLevel = lowFuelLevel;
        return true;
    }
    else
    {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Initialise the gauge and get it ready to run
//!
///////////////////////////////////////////////////////////////////////////////
void InitialiseGauge()
{
    HAL_LoadMaps( s_inputMap, s_outputMap, &s_lowFuelLevel );
    s_running = true;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Process a command
//!
///////////////////////////////////////////////////////////////////////////////
bool ProcessCommand( const char* command )
{
    //
    // Barf if we have an empty string
    //
    if ( *command == '\0' )
    {
        return false;
    }

    bool result = false;

    switch ( command[ 0 ] )
    {
    case 'p':
        s_running = false;
        result = true;
        break;

    case 'r':
        s_running = true;
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
    case 'u':
        ProcessUsageDisplay();
        result = true;
        break;
    case 'f':
        result = ProcessLowFuelLevel( &command[ 1 ] );
        break;

    default:
        break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Run the gauge by reading the tank input, mapping and then changing
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
    if ( s_running )
    {
        ProcessMapping( false );
    }
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Check whether the Fuel Gauge is in programming or run mode
//!
///////////////////////////////////////////////////////////////////////////////
bool IsRunning()
{
    return s_running;
}
