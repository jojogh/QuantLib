/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file mcpagoda.hpp
    \brief Roofed multi asset Asian option
*/

#ifndef quantlib_pagoda_pricer_h
#define quantlib_pagoda_pricer_h

#include <ql/legacy/pricers/mcpricer.hpp>
#include <ql/yieldtermstructure.hpp>
#include <ql/voltermstructures/equityfx/blackvoltermstructure.hpp>


namespace QuantLib {
    class Matrix;
    //! roofed Asian option
    /*! Given a certain portfolio of assets at the end of the period
        it is returned the minimum of a given roof and a certain
        fraction of the positive portfolio performance.  If the
        performance of the portfolio is below then the payoff is null.
    */
    class McPagoda : public McPricer<MultiVariate,PseudoRandom> {
      public:
        McPagoda(const std::vector<Real>& underlyings,
                 Real fraction,
                 Real roof,
                 const std::vector<Handle<YieldTermStructure> >&
                                                           dividendYields,
                 const Handle<YieldTermStructure>& riskFreeRate,
                 const std::vector<Handle<BlackVolTermStructure> >&
                                                             volatilities,
                 const Matrix& correlation,
                 const std::vector<Time>& times,
                 BigNatural seed = 0);
    };

}


#endif