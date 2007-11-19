/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2007 StatPro Italia srl

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

/*! \file oneassetstrikedoption.hpp
    \brief Option on a single asset with striked payoff
*/

#ifndef quantlib_oneassetstriked_option_hpp
#define quantlib_oneassetstriked_option_hpp

#include <ql/instruments/oneassetoption.hpp>
#include <ql/instruments/payoffs.hpp>

namespace QuantLib {

    //! Base class for options on a single asset with striked payoff
    class OneAssetStrikedOption : public OneAssetOption {
      public:
        class engine;
        OneAssetStrikedOption(const boost::shared_ptr<StochasticProcess>&,
                              const boost::shared_ptr<StrikedTypePayoff>&,
                              const boost::shared_ptr<Exercise>&,
                              const boost::shared_ptr<PricingEngine>& engine =
                                      boost::shared_ptr<PricingEngine>());
        //! \name greeks
        //@{
        Real strikeSensitivity() const;
        //@}
        void setupArguments(PricingEngine::arguments*) const;
        void fetchResults(const PricingEngine::results*) const;
      protected:
        void setupExpired() const;
        // results
        mutable Real strikeSensitivity_;
    };

    class OneAssetStrikedOption::engine
        : public OneAssetOption::engine {};
}


#endif
