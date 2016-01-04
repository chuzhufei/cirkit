/* CirKit: A circuit toolkit
 * Copyright (C) 2009-2015  University of Bremen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file stores.hpp
 *
 * @brief Meta-data for stores
 *
 * @author Mathias Soeken
 * @since  2.3
 */

#ifndef STORES_HPP
#define STORES_HPP

#include <core/cli/store.hpp>
#include <classical/aig.hpp>
#include <reversible/circuit.hpp>
#include <reversible/rcbdd.hpp>
#include <reversible/truth_table.hpp>

namespace cirkit
{

/******************************************************************************
 * circuit                                                                    *
 ******************************************************************************/

template<>
struct store_info<circuit>
{
  static constexpr const char* key         = "circuits";
  static constexpr const char* option      = "circuits";
  static constexpr const char* mnemonic    = "c";
  static constexpr const char* name        = "circuit";
  static constexpr const char* name_plural = "circuits";
};

template<>
std::string store_entry_to_string<circuit>( const circuit& circ );

template<>
void print_store_entry<circuit>( std::ostream& os, const circuit& circ );

template<>
void print_store_entry_statistics<circuit>( std::ostream& os, const circuit& circ );

template<>
command_log_opt_t log_store_entry_statistics<circuit>( const circuit& circ );

template<>
inline bool store_can_convert<circuit, aig_graph>() { return true; }

template<>
aig_graph store_convert<circuit, aig_graph>( const circuit& circ );

/******************************************************************************
 * binary_truth_table                                                         *
 ******************************************************************************/

template<>
struct store_info<binary_truth_table>
{
  static constexpr const char* key         = "spec";
  static constexpr const char* option      = "specs";
  static constexpr const char* mnemonic    = "s";
  static constexpr const char* name        = "specification";
  static constexpr const char* name_plural = "specifications";
};

template<>
std::string store_entry_to_string<binary_truth_table>( const binary_truth_table& spec );

template<>
void print_store_entry<binary_truth_table>( std::ostream& os, const binary_truth_table& spec );

/******************************************************************************
 * rcbdd                                                                      *
 ******************************************************************************/

template<>
struct store_info<rcbdd>
{
  static constexpr const char* key         = "rcbdds";
  static constexpr const char* option      = "rcbdds";
  static constexpr const char* mnemonic    = "r";
  static constexpr const char* name        = "RCBDD";
  static constexpr const char* name_plural = "RCBDDs";
};

template<>
std::string store_entry_to_string<rcbdd>( const rcbdd& bdd );

template<>
struct show_store_entry<rcbdd>
{
  show_store_entry( program_options& opts );

  bool operator()( rcbdd& bdd, const std::string& dotname, const program_options& opts, const properties::ptr& settings );
};

template<>
void print_store_entry<rcbdd>( std::ostream& os, const rcbdd& bdd );

}

#endif

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End: