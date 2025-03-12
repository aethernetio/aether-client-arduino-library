/*################################################################################
  ##
  ##   Copyright (C) 2016-2024 Keith O'Hara
  ##
  ##   This file is part of the GCE-Math C++ library.
  ##
  ##   Licensed under the Apache License, Version 2.0 (the "License");
  ##   you may not use this file except in compliance with the License.
  ##   You may obtain a copy of the License at
  ##
  ##       http://www.apache.org/licenses/LICENSE-2.0
  ##
  ##   Unless required by applicable law or agreed to in writing, software
  ##   distributed under the License is distributed on an "AS IS" BASIS,
  ##   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ##   See the License for the specific language governing permissions and
  ##   limitations under the License.
  ##
  ################################################################################*/

#ifndef _gcem_HPP
#define _gcem_HPP

#include "third_party/gcem/include/gcem_incl/gcem_options.hpp"

namespace gcem
{
#include "third_party/gcem/include/gcem_incl/quadrature/gauss_legendre_50.hpp"

#include "third_party/gcem/include/gcem_incl/is_inf.hpp"
#include "third_party/gcem/include/gcem_incl/is_nan.hpp"
#include "third_party/gcem/include/gcem_incl/is_finite.hpp"
    
#include "third_party/gcem/include/gcem_incl/signbit.hpp"
#include "third_party/gcem/include/gcem_incl/copysign.hpp"
#include "third_party/gcem/include/gcem_incl/neg_zero.hpp"
#include "third_party/gcem/include/gcem_incl/sgn.hpp"

#include "third_party/gcem/include/gcem_incl/abs.hpp"
#include "third_party/gcem/include/gcem_incl/fabs.hpp"
#include "third_party/gcem/include/gcem_incl/fabsf.hpp"
#include "third_party/gcem/include/gcem_incl/fabsl.hpp"
#include "third_party/gcem/include/gcem_incl/ceil.hpp"
#include "third_party/gcem/include/gcem_incl/floor.hpp"
#include "third_party/gcem/include/gcem_incl/trunc.hpp"
#include "third_party/gcem/include/gcem_incl/is_odd.hpp"
#include "third_party/gcem/include/gcem_incl/is_even.hpp"
#include "third_party/gcem/include/gcem_incl/max.hpp"
#include "third_party/gcem/include/gcem_incl/min.hpp"
#include "third_party/gcem/include/gcem_incl/sqrt.hpp"
#include "third_party/gcem/include/gcem_incl/inv_sqrt.hpp"
#include "third_party/gcem/include/gcem_incl/hypot.hpp"

#include "third_party/gcem/include/gcem_incl/find_exponent.hpp"
#include "third_party/gcem/include/gcem_incl/find_fraction.hpp"
#include "third_party/gcem/include/gcem_incl/find_whole.hpp"
#include "third_party/gcem/include/gcem_incl/mantissa.hpp"
#include "third_party/gcem/include/gcem_incl/round.hpp"
#include "third_party/gcem/include/gcem_incl/fmod.hpp"

#include "third_party/gcem/include/gcem_incl/pow_integral.hpp"
#include "third_party/gcem/include/gcem_incl/exp.hpp"
#include "third_party/gcem/include/gcem_incl/expm1.hpp"
#include "third_party/gcem/include/gcem_incl/log.hpp"
#include "third_party/gcem/include/gcem_incl/log1p.hpp"
#include "third_party/gcem/include/gcem_incl/log2.hpp"
#include "third_party/gcem/include/gcem_incl/log10.hpp"
#include "third_party/gcem/include/gcem_incl/pow.hpp"

#include "third_party/gcem/include/gcem_incl/gcd.hpp"
#include "third_party/gcem/include/gcem_incl/lcm.hpp"

#include "third_party/gcem/include/gcem_incl/tan.hpp"
#include "third_party/gcem/include/gcem_incl/cos.hpp"
#include "third_party/gcem/include/gcem_incl/sin.hpp"

#include "third_party/gcem/include/gcem_incl/atan.hpp"
#include "third_party/gcem/include/gcem_incl/atan2.hpp"
#include "third_party/gcem/include/gcem_incl/acos.hpp"
#include "third_party/gcem/include/gcem_incl/asin.hpp"

#include "third_party/gcem/include/gcem_incl/tanh.hpp"
#include "third_party/gcem/include/gcem_incl/cosh.hpp"
#include "third_party/gcem/include/gcem_incl/sinh.hpp"

#include "third_party/gcem/include/gcem_incl/atanh.hpp"
#include "third_party/gcem/include/gcem_incl/acosh.hpp"
#include "third_party/gcem/include/gcem_incl/asinh.hpp"

#include "third_party/gcem/include/gcem_incl/binomial_coef.hpp"
#include "third_party/gcem/include/gcem_incl/lgamma.hpp"
#include "third_party/gcem/include/gcem_incl/tgamma.hpp"
#include "third_party/gcem/include/gcem_incl/factorial.hpp"
#include "third_party/gcem/include/gcem_incl/lbeta.hpp"
#include "third_party/gcem/include/gcem_incl/beta.hpp"
#include "third_party/gcem/include/gcem_incl/lmgamma.hpp"
#include "third_party/gcem/include/gcem_incl/log_binomial_coef.hpp"

#include "third_party/gcem/include/gcem_incl/erf.hpp"
#include "third_party/gcem/include/gcem_incl/erf_inv.hpp"
#include "third_party/gcem/include/gcem_incl/incomplete_beta.hpp"
#include "third_party/gcem/include/gcem_incl/incomplete_beta_inv.hpp"
#include "third_party/gcem/include/gcem_incl/incomplete_gamma.hpp"
#include "third_party/gcem/include/gcem_incl/incomplete_gamma_inv.hpp"
}

#endif
