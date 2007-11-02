/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Ferdinando Ametrano
 Copyright (C) 2004 Neil Firth

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

#include <ql/pricingengines/basket/stulzengine.hpp>
#include <ql/pricingengines/blackcalculator.hpp>
#include <ql/pricingengines/blackformula.hpp>
#include <ql/processes/stochasticprocessarray.hpp>
#include <ql/processes/blackscholesprocess.hpp>
#include <ql/math/distributions/bivariatenormaldistribution.hpp>
#include <ql/math/distributions/normaldistribution.hpp>
#include <ql/exercise.hpp>

namespace QuantLib {

    namespace {

        // calculate the value of euro min basket call
        Real euroTwoAssetMinBasketCall(Real forward1, Real forward2,
                                       Real strike,
                                       DiscountFactor riskFreeDiscount,
                                       Real variance1, Real variance2,
                                       Real rho) {

            Real stdDev1 = std::sqrt(variance1);
            Real stdDev2 = std::sqrt(variance2);

            Real variance = variance1 + variance2 - 2*rho*stdDev1*stdDev2;
            Real stdDev = std::sqrt(variance);

            Real modRho1 = (rho * stdDev2 - stdDev1) / stdDev;
            Real modRho2 = (rho * stdDev1 - stdDev2) / stdDev;

            Real D1 = (std::log(forward1/forward2) + 0.5*variance) / stdDev;

            Real alfa, beta, gamma;
            if (strike != 0.0) {
                BivariateCumulativeNormalDistribution bivCNorm =
                    BivariateCumulativeNormalDistribution(rho);
                BivariateCumulativeNormalDistribution bivCNormMod2 =
                    BivariateCumulativeNormalDistribution(modRho2);
                BivariateCumulativeNormalDistribution bivCNormMod1 =
                    BivariateCumulativeNormalDistribution(modRho1);

                Real D1_1 =
                    (std::log(forward1/strike) + 0.5*variance1) / stdDev1;
                Real D1_2 =
                    (std::log(forward2/strike) + 0.5*variance2) / stdDev2;
                alfa = bivCNormMod1(D1_1, -D1);
                beta = bivCNormMod2(D1_2, D1 - stdDev);
                gamma = bivCNorm(D1_1 - stdDev1, D1_2 - stdDev2);
            } else {
                CumulativeNormalDistribution cum;
                alfa = cum(-D1);
                beta = cum(D1 - stdDev);
                gamma = 1.0;
            }

            return riskFreeDiscount *
                (forward1*alfa + forward2*beta - strike*gamma);

        }

        // calculate the value of euro max basket call
        Real euroTwoAssetMaxBasketCall(Real forward1, Real forward2,
                                       Real strike,
                                       DiscountFactor riskFreeDiscount,
                                       Real variance1, Real variance2,
                                       Real rho) {

            boost::shared_ptr<StrikedTypePayoff> payoff(new
                PlainVanillaPayoff(Option::Call, strike));

            Real black1 = blackFormula(payoff->optionType(), payoff->strike(),
                forward1, std::sqrt(variance1)) * riskFreeDiscount;

            Real black2 = blackFormula(payoff->optionType(), payoff->strike(),
                forward2, std::sqrt(variance2)) * riskFreeDiscount;

            return black1 + black2 -
                euroTwoAssetMinBasketCall(forward1, forward2, strike,
                                          riskFreeDiscount,
                                          variance1, variance2, rho);
        }
    }

    void StulzEngine::calculate() const {

        QL_REQUIRE(arguments_.exercise->type() == Exercise::European,
                   "not an European Option");

        QL_REQUIRE(arguments_.stochasticProcess->size() == 2,
                   "not a basket of two stocks");
        boost::shared_ptr<StochasticProcessArray> processes =
            boost::dynamic_pointer_cast<StochasticProcessArray>(
                                           arguments_.stochasticProcess);
        QL_REQUIRE(processes, "Stochastic-process array required");
        boost::shared_ptr<GeneralizedBlackScholesProcess> process1 =
            boost::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                                                       processes->process(0));
        QL_REQUIRE(process1, "Black-Scholes processes required");
        boost::shared_ptr<GeneralizedBlackScholesProcess> process2 =
            boost::dynamic_pointer_cast<GeneralizedBlackScholesProcess>(
                                                       processes->process(1));
        QL_REQUIRE(process2, "Black-Scholes processes required");

        boost::shared_ptr<EuropeanExercise> exercise =
            boost::dynamic_pointer_cast<EuropeanExercise>(arguments_.exercise);
        QL_REQUIRE(exercise, "not an European Option");

        boost::shared_ptr<BasketPayoff> basket_payoff =
            boost::dynamic_pointer_cast<BasketPayoff>(arguments_.payoff);

        boost::shared_ptr<MinBasketPayoff> min_basket =
            boost::dynamic_pointer_cast<MinBasketPayoff>(arguments_.payoff);

        boost::shared_ptr<MaxBasketPayoff> max_basket =
            boost::dynamic_pointer_cast<MaxBasketPayoff>(arguments_.payoff);
        QL_REQUIRE(min_basket || max_basket, "unknown basket type");

        boost::shared_ptr<PlainVanillaPayoff> payoff =
            boost::dynamic_pointer_cast<PlainVanillaPayoff>(basket_payoff->basePayoff());
        QL_REQUIRE(payoff, "non-plain payoff given");

        Real strike = payoff->strike();

        Real variance1 = process1->blackVolatility()->blackVariance(
                                                exercise->lastDate(), strike);
        Real variance2 = process2->blackVolatility()->blackVariance(
                                                exercise->lastDate(), strike);

        Real rho = processes->correlation()[1][0];

        DiscountFactor riskFreeDiscount =
            process1->riskFreeRate()->discount(exercise->lastDate());

        // cannot handle non zero dividends, so don't believe this...
        DiscountFactor dividendDiscount1 =
            process1->dividendYield()->discount(exercise->lastDate());
        DiscountFactor dividendDiscount2 =
            process2->dividendYield()->discount(exercise->lastDate());

        Real forward1 = process1->stateVariable()->value() *
            dividendDiscount1 / riskFreeDiscount;
        Real forward2 = process2->stateVariable()->value() *
            dividendDiscount2 / riskFreeDiscount;

        if (max_basket) {
            switch (payoff->optionType()) {
              // euro call on a two asset max basket
              case Option::Call:
                results_.value =
                    euroTwoAssetMaxBasketCall(forward1, forward2, strike,
                                              riskFreeDiscount,
                                              variance1, variance2,
                                              rho);

                break;
              // euro put on a two asset max basket
              case Option::Put:
                results_.value = strike * riskFreeDiscount -
                    euroTwoAssetMaxBasketCall(forward1, forward2, 0.0,
                                              riskFreeDiscount,
                                              variance1, variance2, rho) +
                    euroTwoAssetMaxBasketCall(forward1, forward2, strike,
                                              riskFreeDiscount,
                                              variance1, variance2, rho);
                break;
              default:
                QL_FAIL("unknown option type");
            }
        } else if (min_basket) {
            switch (payoff->optionType()) {
              // euro call on a two asset min basket
              case Option::Call:
                results_.value =
                    euroTwoAssetMinBasketCall(forward1, forward2, strike,
                                              riskFreeDiscount,
                                              variance1, variance2,
                                              rho);
                break;
              // euro put on a two asset min basket
              case Option::Put:
                results_.value = strike * riskFreeDiscount -
                    euroTwoAssetMinBasketCall(forward1, forward2, 0.0,
                                              riskFreeDiscount,
                                              variance1, variance2, rho) +
                    euroTwoAssetMinBasketCall(forward1, forward2, strike,
                                              riskFreeDiscount,
                                              variance1, variance2, rho);
                break;
              default:
                QL_FAIL("unknown option type");
            }
        } else {
            QL_FAIL("unknown type");
        }
    }

}
