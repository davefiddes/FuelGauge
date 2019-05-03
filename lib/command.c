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
#include <string.h>

//
// Global variables used by the command processor
//
bool g_running = true;

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
//! Input mapping between the tank value and a linear actual value
//
uint16_t g_inputMap[ MAPSIZE ];

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Load an input map into operation
//!
//! \note This assumes that a map is MAPSIZE and no checking is done
//!
///////////////////////////////////////////////////////////////////////////////
void LoadInputMap( const uint16_t* map )
{
    memcpy( &g_inputMap, map, sizeof( g_inputMap ) );
}

//
//! Output mapping between a linear actual value and the guage output
//
uint16_t g_outputMap[ MAPSIZE ];

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief  Load an output map into operation
//!
//! \note This assumes that a map is MAPSIZE and no checking is done
//!
///////////////////////////////////////////////////////////////////////////////
void LoadOutputMap( const uint16_t* map )
{
    memcpy( &g_outputMap, map, sizeof( g_outputMap ) );
}

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
//! \brief  Display current tank input value and output gauge value
//!
///////////////////////////////////////////////////////////////////////////////
static bool ProcessDisplayCommand()
{
    uint16_t input = HAL_GetTankInput();
    uint16_t output = HAL_GetGaugeOutput();

    HAL_Printf( "Tank: %0#x Gauge: %0#x\n", input, output );

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
static bool ProcessOneShotMapping()
{
    uint16_t input = HAL_GetTankInput();

    uint16_t actual = MapValue( g_inputMap, input);

    uint16_t output = MapValue( g_outputMap, actual);

    HAL_SetGaugeOutput( output );
    HAL_Printf( "Tank: %0#x Actual: %0#x Gauge: %0#x\n", input, actual, output);

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
        result = ProcessOneShotMapping();
        break;
    case 'i':
        /* code */
        break;
    case 'o':
        /* code */
        break;
    case 'm':
        /* code */
        break;
    case 's':
        /* code */
        break;
    case 'l':
        /* code */
        break;

    default:
        break;
    }

    return result;
}