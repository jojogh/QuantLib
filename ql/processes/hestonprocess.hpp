/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2007 Klaus Spanderen

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

/*! \file hestonprocess.hpp
    \brief Heston stochastic process
*/

#ifndef quantlib_heston_process_hpp
#define quantlib_heston_process_hpp

#include <ql/stochasticprocess.hpp>
#include <ql/yieldtermstructure.hpp>
#include <ql/quote.hpp>

namespace QuantLib {

    //! Square-root stochastic-volatility Heston process
    /*! This class describes the square root stochastic volatility
        process governed by
        \f[
        \begin{array}{rcl}
        dS(t, S)  &=& \mu S dt + \sqrt{v} S dW_1 \\
        dv(t, S)  &=& \kappa (\theta - v) dt + \sigma \sqrt{v} dW_2 \\
        dW_1 dW_2 &=& \rho dt
        \end{array}
        \f]

        \ingroup processes
    */
    class HestonProcess : public StochasticProcess {
      public:
        enum Discretization { PartialTruncation, FullTruncation, 
                              Reflection, ExactVariance };

        HestonProcess(const Handle<YieldTermStructure>& riskFreeRate,
                      const Handle<YieldTermStructure>& dividendYield,
                      const Handle<Quote>& s0,
                      double v0, double kappa, 
                      double theta, double sigma, double rho,
                      Discretization d = FullTruncation);


        Size size() const;
        Disposable<Array> initialValues() const;
        Disposable<Array> drift(Time t, const Array& x) const;
        Disposable<Matrix> diffusion(Time t, const Array& x) const;
        Disposable<Array> apply(const Array& x0, const Array& dx) const;
        Disposable<Array> evolve(Time t0, const Array& x0,
                                 Time dt, const Array& dw) const;

        const RelinkableHandle<Quote>& v0()    const;
        const RelinkableHandle<Quote>& rho()   const;
        const RelinkableHandle<Quote>& kappa() const;
        const RelinkableHandle<Quote>& theta() const;
        const RelinkableHandle<Quote>& sigma() const;

        const Handle<Quote>& s0()    const;
        const Handle<YieldTermStructure>& dividendYield() const;
        const Handle<YieldTermStructure>& riskFreeRate() const;

        void update();
        Time time(const Date&) const;
      private:
        Handle<YieldTermStructure> riskFreeRate_, dividendYield_;
        Handle<Quote> s0_;
        RelinkableHandle<Quote> v0_, kappa_, theta_, sigma_, rho_;

        const Discretization discretization_;

        Real s0v_, v0v_, kappav_, thetav_, sigmav_, rhov_, sqrhov_;
    };
}


#endif