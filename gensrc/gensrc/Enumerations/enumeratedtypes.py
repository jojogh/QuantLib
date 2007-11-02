
"""
 Copyright (C) 2005, 2006, 2007 Eric Ehlers
 Copyright (C) 2005 Plamen Neykov
 Copyright (C) 2005 Aurelien Chanudet

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
"""

"""encapsulate enumerations for a library datatype."""

from gensrc.Serialization import serializable
from gensrc.Utilities import common

class EnumeratedType(serializable.Serializable):
    """encapsulate a string->value mapping for a library enumerated type."""

    #############################################
    # class variables
    #############################################

    groupName_ = 'EnumeratedTypes'

    #############################################
    # public interface
    #############################################

    def setType(self, val):
        self.type_ = val

    def setConstructor(self, val):
        self.constructor_ = val

    def string(self):
        return self.string_

    def value(self):
        return self.value_

    def constructor(self):
        if self.constructor_:
            return self.type_ + '(' + self.value_ + ')'
        else:
            return self.value_

    #############################################
    # serializer interface
    #############################################

    def name(self):
        """return unique identifier for this object."""
        return self.string_

    def serialize(self, serializer):
        """load/unload class state to/from serializer object."""
        serializer.serializeProperty(self, common.STRING)
        serializer.serializeProperty(self, common.VALUE)

class EnumeratedTypeGroup(serializable.Serializable):
    """encapsulate enumerations for a library datatype."""

    #############################################
    # class variables
    #############################################

    groupName_ = 'EnumeratedTypeGroups'

    #############################################
    # public interface
    #############################################

    def enumeratedTypes(self):
        """serve up enumeration definition objects alphabetically by name."""
        for key in self.enumeratedTypeKeys_:
            yield self.enumeratedTypes_[key]

    def type(self):
        return self.type_

    def includeFile(self):
        return self.includeFile_

    #############################################
    # serializer interface
    #############################################

    def name(self):
        """return unique identifier for this object."""
        return self.type_

    def serialize(self, serializer):
        """load/unload class state to/from serializer object."""
        serializer.serializeAttribute(self, common.TYPE)
        serializer.serializeBoolean(self, common.CONSTRUCTOR)
        serializer.serializeObjectDict(self, EnumeratedType)
        serializer.serializeProperty(self, 'includeFile')

    def postSerialize(self):
        """invoke any post serialization behavior that may be required."""
        for enumeratedType in self.enumeratedTypes_.values():
            enumeratedType.setType(self.type_)
            enumeratedType.setConstructor(self.constructor_)
