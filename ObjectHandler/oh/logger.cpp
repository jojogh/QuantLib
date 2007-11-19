
/*
 Copyright (C) 2005, 2006, 2007 Eric Ehlers

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

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <oh/config.hpp>
#endif
#include <oh/logger.hpp>
#include <oh/ohdefines.hpp>
#include <oh/exception.hpp>
#include <log4cxx/helpers/exception.h>
/* Use BOOST_MSVC instead of _MSC_VER since some other vendors (Metrowerks,
   for example) also #define _MSC_VER
*/
#ifdef BOOST_MSVC
#  define BOOST_LIB_DIAGNOSTIC
#  include <log4cxx/auto_link.hpp>
#  undef BOOST_LIB_DIAGNOSTIC
#endif
#include <ostream>
#include <boost/filesystem.hpp>

using namespace log4cxx;

namespace ObjectHandler {

    Logger::Logger() {
        try {
     		_logger = log4cxx::Logger::getRootLogger();
            _logger->setLevel(Level::OFF);
            _layout = LayoutPtr(new SimpleLayout());
        } catch (helpers::Exception &e) {
            OH_FAIL("Logger::Logger: error initializing: " + e.getMessage());
        }
    }

    void Logger::setLogFile(
            const std::string &logFileName,
            const int &logLevel) {

        // Create a boost path object from the std::string.
        boost::filesystem::path path(logFileName);

        // If a parent directory has been specified then ensure it exists.
        if (path.has_branch_path()) {
            OH_REQUIRE(boost::filesystem::exists(path.branch_path()),
                "Invalid path : " << logFileName);
        }

        try {
            _logger->removeAppender(_fileAppender);
            _fileAppender = AppenderPtr(new FileAppender(_layout, logFileName));
            _logger->addAppender(_fileAppender);
            setLogLevel(logLevel);
        } catch (helpers::Exception &e) {
            OH_FAIL("Logger::setLogFile: unable to set logfile: " + e.getMessage());
        }
    }

    void Logger::setConsole(
            const int &console,
            const int &logLevel) {
        try {
            _logger->removeAppender(_consoleAppender);
            if (console) {
                _consoleAppender = AppenderPtr(new ConsoleAppender(_layout));
                _logger->addAppender(_consoleAppender);
            }
            setLogLevel(logLevel);
        } catch (helpers::Exception &e) {
            OH_FAIL("Logger::setLogFile: unable to set logfile: " + e.getMessage());
        }
    }

    void Logger::setLogLevel(const int &logLevel) {
        try {
            switch (logLevel) {
                case 0:
                    _logger->setLevel(Level::OFF);
                    break;
                case 1:
                    _logger->setLevel(Level::FATAL);
                    break;
                case 2:
                    _logger->setLevel(Level::ERROR);
                    break;
                case 3:
                    _logger->setLevel(Level::WARN);
                    break;
                case 4:
                    _logger->setLevel(Level::INFO);
                    break;
                case 5:
                    _logger->setLevel(Level::DEBUG);
                    break;
                default:
                    OH_FAIL("Logger::setLogLevel: invalid logLevel: " << logLevel);
            }
        } catch (helpers::Exception &e) {
            OH_FAIL("Logger::Logger: error initializing: " + e.getMessage());
        }
    }

    void Logger::logMessage(
            const std::string &message,
            const int &level) {
        // client applications call this function from within their
        // catch() clauses so this function must not throw.
        try {
            switch (level) {
                case 1:
                    LOG4CXX_FATAL(_logger, message);
                    break;
                case 2:
                    LOG4CXX_ERROR(_logger, message);
                    break;
                case 3:
                    LOG4CXX_WARN(_logger, message);
                    break;
                case 4:
                    LOG4CXX_INFO(_logger, message);
                    break;
                case 5:
                    LOG4CXX_DEBUG(_logger, message);
                    break;
            }
        } catch (...) {}
    }

}
