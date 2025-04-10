/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2021 John Wellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef ETL_NTH_TYPE_INCLUDED
#define ETL_NTH_TYPE_INCLUDED

#include "third_party/etl/include/etl/platform.h"
#include "third_party/etl/include/etl/static_assert.h"

namespace etl
{
#if ETL_USING_CPP11

  //***************************************************************************
  /// Finds the nth type in a variadic type parameter.
  //***************************************************************************
  template <size_t N, typename... TTypes>
  struct nth_type;

  //***************************************************************************
  /// Finds the nth type in a variadic type parameter.
  //***************************************************************************
  template <size_t N, typename T1, typename... TRest>
  struct nth_type<N, T1, TRest...>
  {
    ETL_STATIC_ASSERT(N <= sizeof...(TRest), "etl::nth_type out of range for type list");

    using type = typename nth_type<N - 1U, TRest...>::type;
  };

  //***************************************************************************
  /// Finds the 0thth type in a variadic type parameter.
  //***************************************************************************
  template <typename T1, typename... TRest>
  struct nth_type<0U, T1, TRest...>
  {
    using type = T1;
  };

  //***************************************************************************
  /// Finds the 0th type in a variadic type parameter.
  //***************************************************************************
  template <size_t N>
  struct nth_type<N>
  {
  };

  //***************************************************************************
  /// Finds the nth type in a variadic type parameter.
  //***************************************************************************
  template <size_t N, typename... TTypes>
  using nth_type_t = typename nth_type<N, TTypes...>::type;
#endif
}

#endif
