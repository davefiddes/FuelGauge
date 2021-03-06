///////////////////////////////////////////////////////////////////////////////
//!
//! \file
//! \brief  Hardware Abstraction Library for the Fuel Gauge
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

#ifndef HAL_H
#define HAL_H

#include <stdbool.h>
#include <stdint.h>

#if defined( __XC )
#include <xc.h> /* XC8 General Include File */
#endif

//
//! This value indicates an error in the tank input
//
#define TANK_INPUT_ERROR 0xffff

#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif

uint16_t HAL_GetTankInput( void );
uint16_t HAL_GetGaugeOutput( void );
void     HAL_SetGaugeOutput( uint16_t value );
void     HAL_SetLowFuelLight( bool newState );

void HAL_PrintText( const char* text );
void HAL_PrintNewline( void );

void HAL_LoadMaps( uint16_t* input, uint16_t* output, uint16_t* lowFuelLevel );
void HAL_SaveMaps(
    const uint16_t* input,
    const uint16_t* output,
    uint16_t        lowFuelLevel );

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif // HAL_H
