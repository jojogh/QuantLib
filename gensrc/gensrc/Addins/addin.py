
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

"""class to encapsulate data and behavior 
required to generate addin source code."""

from gensrc.Utilities import common
from gensrc.Rules import rule
from gensrc.Serialization import serializable
from gensrc.Serialization import xmlreader
from gensrc.Utilities import buffer
import os

class Addin(serializable.Serializable):
    """class to encapsulate data and behavior 
    required to generate addin source code."""

    #############################################
    # class variables
    #############################################

    objectIdSuffix_ = ''
    # The next two values are required for autogeneration of constructors
    # and may be overridden for certain platforms
    repositoryClass_ = 'Repository'
    overwriteVariable_ = 'Overwrite'

    #############################################
    # public interface
    #############################################

    def objectIdSuffix(self):
        return self.objectIdSuffix_

    def bufferLoop(self):
        return self.bufferLoop_

    def unchanged(self):
        return self.unchanged_

    def updated(self):
        return self.updated_

    def created(self):
        return self.created_

    def incrementUnchanged(self):
        self.unchanged_ += 1

    def incrementUpdated(self):
        self.updated_ += 1

    def incrementCreated(self):
        self.created_ += 1

    def voCall(self):
        return self.voCall_

    def libraryCall(self):
        return self.libraryCall_

    def libraryReturnType(self):
        return self.libraryReturnType_

    def loopInputs(self):
        return self.loopInputs_

    def loopReturnType(self):
        return self.loopReturnType_

    def relativePath(self):
        return self.relativePath_

    def dataDirectory(self):
        return self.dataDirectory_

    def idStrip(self, parameterList):
        return ""

    def repositoryClass(self):
        return self.repositoryClass_

    def overwriteVariable(self):
        return self.overwriteVariable_

    #############################################
    # serializer interface
    #############################################

    def serialize(self, serializer):
        """load/unload class state to/from serializer object."""
        serializer.serializeAttribute(self, common.NAME)
        serializer.serializeProperty(self, common.ROOT_DIRECTORY)
        serializer.serializeProperty(self, 'dataDirectory')
        serializer.serializeObjectPropertyDict(self, buffer.Buffer)
        serializer.serializeProperty(self, common.COPYRIGHT)
        serializer.serializeBoolean(self, 'loadRules', True)
        serializer.serializeProperty(self, 'namespaceAddin')
        serializer.serializeProperty(self, 'relativePath')

    def postSerialize(self):
        """Perform post serialization initialization."""

        if self.loadRules_:
            serializer = xmlreader.XmlReader('metadata/Rules/' + self.name_.lower())
            serializer.serializeObjectPropertyDict(self, rule.RuleGroup)

        if self.rootDirectory_:
            self.rootPath_ = "../%s/" % self.rootDirectory_
            if not os.path.exists(self.rootPath_): 
                os.makedirs(self.rootPath_)

    #############################################
    # private member functions
    #############################################

    def __init__(self):
        """class initialization"""
        # count of files generated by this Addin
        self.unchanged_ = 0
        self.updated_ = 0
        self.created_ = 0
