// (C) Copyright Tymofey 2011.
//
// Distributed under the Boost Software License, Version 1.0.
// (See copy at http://www.boost.org/LICENSE_1_0.txt)

/*****************************************************************************************
 * Functions co clamp values to specified ranges                                         *
 *****************************************************************************************
 * clamp(a, x, b)                                                                        *
 *                                                                                       *
 * Effect:                                                                               *
 *    if (x < a) returns a, otherwise if (b < x) returns b, otherwise returns x.         *
 *****************************************************************************************
 * clamp(a, x, b, comp)                                                                  *
 * clamp<comp>(a, x, b)                                                                  *
 *                                                                                       *
 * Effect:                                                                               *
 *    if comp(x, a) returns a, otherwise if comp(b, x) returns b, otherwise returns x.   *
 *****************************************************************************************/

#ifndef BOOST_ALGORITHM_CLAMP_HPP
#define BOOST_ALGORITHM_CLAMP_HPP

namespace boost
{

    template <class T>
    T const& clamp(T const& a, T const& x, T const& b)
    {
        if (x < a) return a;
        if (b < x) return b;
        return x;
    }

    template <class C, class T>
    T const& clamp(T const& a, T const& x, T const& b, C c = C())
    {
        if (c(x, a)) return a;
        if (c(b, x)) return b;
        return x;
    }

}

#endif //BOOST_ALGORITHM_CLAMP_HPP
