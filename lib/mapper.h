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

#ifndef MAPPER_H
#define MAPPER_H

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#endif

#include <stdint.h>

#define MAPSIZE 9 // 3-bits + 1

#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif

uint16_t MapValue( const uint16_t *map, uint16_t value );

#ifdef __cplusplus  // Provide C++ Compatibility
    }
#endif

#endif // MAPPER_H
