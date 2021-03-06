/* CirKit: A circuit toolkit
 * Copyright (C) 2009-2015  University of Bremen
 * Copyright (C) 2015-2016  EPFL
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
 * @file npn_manager.hpp
 *
 * @brief NPN manager
 *
 * @author Mathias Soeken
 * @since  2.3
 */

#ifndef NPN_MANAGER_HPP
#define NPN_MANAGER_HPP

#include <functional>
#include <iostream>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include <classical/functions/npn_canonization.hpp>
#include <classical/utils/truth_table_utils.hpp>

namespace cirkit
{

inline std::function<tt(const tt&, boost::dynamic_bitset<>&, std::vector<unsigned>&)> make_exact_npn_canonization_wrapper()
{
  return std::function<tt(const tt&, boost::dynamic_bitset<>&, std::vector<unsigned>&)>( []( const tt& t, boost::dynamic_bitset<>& phase, std::vector<unsigned>& perm ) {
      return exact_npn_canonization( t, phase, perm );
    } );
}

class npn_manager
{
public:
  using npn_classifier_t = std::function<tt(const tt&, boost::dynamic_bitset<>&, std::vector<unsigned>&)>;

  npn_manager( unsigned hash_table_size = 4096, const npn_classifier_t& npn_func = make_exact_npn_canonization_wrapper() );

  tt compute( const tt& tt, boost::dynamic_bitset<>& phase, std::vector<unsigned>& perm );
  void print_statistics( std::ostream& os = std::cout ) const;

private:
  struct table_entry_t
  {
    table_entry_t() {}
    table_entry_t( const std::string& tt, const std::string& npn, const std::vector<unsigned>& perm, const boost::dynamic_bitset<>& phase )
      : tt( tt ), npn( npn ), perm( perm ), phase( phase ) {}

    std::string             tt;
    std::string             npn;
    std::vector<unsigned>   perm;
    boost::dynamic_bitset<> phase;
  };

  using table_t = std::vector<table_entry_t>;

  table_t          table;

  npn_classifier_t npn_func;

  double           runtime    = 0.0;
  unsigned long    cache_hit  = 0;
  unsigned long    cache_miss = 0;
};

}

#endif

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
