"""
Written by John MacCallum & Ilya Rostovtsev, The Center for New Music and 
Audio Technologies (CNMAT), University of California, Berkeley.  
Copyright (c) 2009-ll, The Regents of the University of California (Regents). 
Permission to use, copy, modify, distribute, and distribute modified versions
of this software and its documentation without fee and without a signed
licensing agreement, is hereby granted, provided that the above copyright
notice, this paragraph and the following two paragraphs appear in all copies,
modifications, and distributions.

IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
"""

import libo as odot

class timetag(object):
    def __init__(self, binary = None):
        if binary:
            self.__tt = binary
        else:
            self.__tt = odot.osc_timetag_now()

    def __repr__(self):
        return str()

    def __str__(self):
        return str(self.getSeconds()) + " " + str(self.getFraction())

    def ___compare(self, other):
        return odot.osc_timetag_compare(self.__tt, other.__tt)

    def __lt__(self, other):
        return self.___compare(other) is -1

    def __le__(self, other):
        return self.__lt__(other) or self.__eq__(other)

    def __eq__(self, other):
        return self.___compare(other) is 0

    def __ne__(self, other):
        return not self.__eq__(other)

    def __gt__(self, other):
        return self.___compare(other) is 1

    def __ge__(self, other):
        return self.__gt__(other) or self.__eq__(other)

    def __add__(self, other):
        if type(other) is not timetag:
            result = None
            try:
                result = odot.osc_timetag_add(self.__tt, odot.osc_timetag_floatToTimetag(other))
            except:
                print("Can't convert into a timetag...")
        else:
            result = odot.osc_timetag_add(self.__tt, other._timetag__tt)
        return timetag(binary = result)

    def __radd__(self, other):
        return self.__add__(other)

    def __sub__(self, other):
        if type(other) is not timetag:
            result = None
            try:
                result = odot.osc_timetag_subtract(self.__tt, odot.osc_timetag_floatToTimetag(other))
            except:
                print("Can't convert into a timetag...")
        else:
            result = odot.osc_timetag_subtract(self.__tt, other._timetag__tt)
        return timetag(binary = result)

    def __rsub__(self, other):
        if type(other) is not timetag:
            result = None
            try:
                result = odot.osc_timetag_subtract(odot.osc_timetag_floatToTimetag(other), self.__tt)
            except:
                print("Can't convert into a timetag...")
        else:
            result = odot.osc_timetag_subtract(other._timetag__tt, self.__tt)
        return timetag(binary = result)

    def __iadd__(self, other):
        if type(other) is not timetag:
            result = None
            try:
                result = odot.osc_timetag_add(self.__tt, odot.osc_timetag_floatToTimetag(other))
            except:
                print("Can't convert into a timetag...")
        else:
            result = odot.osc_timetag_add(self.__tt, other._timetag__tt)
        self.__tt = result
        return self

    def __isub__(self, other):
        if type(other) is not timetag:
            result = None
            try:
                result = odot.osc_timetag_subtract(self.__tt, odot.osc_timetag_floatToTimetag(other))
            except:
                print("Can't convert into a timetag...")
        else:
            result = odot.osc_timetag_subtract(self.__tt, other._timetag__tt)
        self.__tt = result
        return self

    def copy(self):
        return timetag(binary = self.__tt)

    def getFloat(self):
        return odot.osc_timetag_timetagToFloat(self.__tt)

    def getSeconds(self):
        return odot.osc_timetag_ntp_getSeconds(self.__tt)

    def getFraction(self):
        return odot.osc_timetag_ntp_getFraction(self.__tt)

    def getHeader(self, bundle_in):
        return timetag(odot.osc_timetag_decodeFromHeader(bundle_in.getRaw()))

    def getRaw(self):
        return self.__tt

class message(object):
    """
    Immutable dictionary container, serialized OpenSoundControl.

    Attributes
    ----------
    NO PUBLIC ATTRIBUTES

    All interactions are handled through dedicated functions. 
    """
    def __init__(self, address = None, values = None, binary = None):
        if binary is not None:
            self.__message = binary
            self.__dealloc = False
            return
        self.__dealloc = True
        if address is not None:
            if address[0] != '/': raise TypeError
            __umess = odot.osc_message_u_allocWithAddress(address)
        else:
            if values:
                raise TypeError
            else:
                self.__message = None
                return
        if values:
            self.__append__(__umess, values)
        self.__message = odot.osc_message_u_serialize(__umess)
        odot.osc_message_u_free(__umess)
        del(__umess)

    def __del__(self):
        if self.__dealloc:
            odot.osc_message_s_free(self.__message)

    def __repr__(self):
        return odot.osc_message_s_format(self.__message)

    def __str__(self):
        return odot.osc_message_s_format(self.__message)

    def __len__(self):
        return odot.osc_message_s_getArgCount(self.__message)

    def __getitem__(self, key):
        # TypeError for non-int keys
        if type(key) is not int:
            raise TypeError
        if key >= len(self):
            return None # should raise something?
        return self.__getArg(key)

    def __missing__(self, key):
        pass

    def __iter__(self):
        self.__iter = odot.osc_message_iterator_s_getIterator(self.__message)
        return self

    def next(self):
        if odot.osc_message_iterator_s_hasNext(self.__iter) is 1:
            self.__atom = odot.osc_message_iterator_s_next(self.__iter)
            return self.__AtomToPython(self.__atom)
        else:
            odot.osc_message_iterator_s_destroyIterator(self.__iter)
            del(self.__iter)
            raise StopIteration

    def __append__(self, message, value):
        t = type(value)
        atom = None
        if t is float:
            atom = odot.osc_message_u_appendDouble(message, value)
        elif t is int:
            atom = odot.osc_message_u_appendInt32(message, value)
        elif t is bool:
            if t:
                atom = odot.osc_message_u_appendTrue(message)
            else:
                atom = odot.osc_message_u_appendFalse(message)
        elif t is str:
            atom = odot.osc_message_u_appendString(message, value)
        elif t is timetag:
            atom = odot.osc_message_u_appendTimetag(message, value._timetag__tt)
        elif t is bundle:
            atom = odot.osc_message_u_appendBndl_s(message, len(value._bundle__bundle), value._bundle__bundle)
        elif t is list:
            for v in value:
                self.__append__(message, v)

    def __getArg(self, index):
        atom = odot.osc_message_s_getArg(self.__message, index)
        result = self.__AtomToPython(atom)
        odot.osc_atom_s_free(atom)
        return result

    def __AtomToPython(self, atom):
        tt = odot.osc_atom_s_getTypetag(atom)
        result = None
        if tt is 'c': # signed 8-bit int
            result = odot.osc_atom_s_getInt8(atom)
        elif tt is 'C': # unsigned 8-bit int
            result = odot.osc_atom_s_getUInt8(atom)
        elif tt is 'i': # signed 32-bit int
            result = odot.osc_atom_s_getInt32(atom)
        elif tt is 'f': # 32-bit IEEE 754 float
            result = odot.osc_atom_s_getFloat(atom)
        elif tt is 'd': # 64-bit IEEE 754 double
            result = odot.osc_atom_s_getDouble(atom)
        elif tt is 's': # string
            result = str(odot.osc_atom_s_getBlob(atom))[4:]
        elif tt is 'T': # true
            result = True
        elif tt is 'F': # false
            result = False
        elif tt is 'I': # signed 64-bit int
            result = odot.osc_atom_s_getInt64(atom)
        elif tt is 'H': # unsigned 64-bit int
            result = odot.osc_atom_s_getUInt64(atom)
        elif tt is 'N': # null
            result = None
        elif tt is 'b': # blob
            result = odot.osc_atom_s_getBlob(atom)
        elif tt is odot.OSC_TIMETAG_TYPETAG : # timetag
            result = o.timetag(binary = odot.osc_atom_s_getTimetag(atom))
        elif tt is odot.OSC_BUNDLE_TYPETAG : # bundle
            result = o.bundle(binary = odot.osc_atom_s_getBndl(atom))
        return result

    def copy(self):
        return message(binary = self.__message)
        
    def getAddress(self):
        return odot.osc_message_s_getAddress(self.__message)

    def getData(self):
        if len(self) is 1:
            for v in self:
                return v
        else:
            result = []
            for v in self:
                result.append(v)
            return result

    def getRaw(self):
        return self.__message

class bundle(object):
    """
    Immutable dictionary container, serialized OpenSoundControl.

    Attributes
    ----------
    NO PUBLIC ATTRIBUTES

    All interactions are handled through dedicated functions. 
    """
    def __init__(self, text=None, messages=None, binary=None):
        if text is None:
            if messages is None:
                if binary is None:
                    self.___createEmptyBundle()
                else: # using binary -- TODO: validate first!
                    self.__bundle = binary
            else: # messages
                ub = odot.osc_bundle_u_alloc()
                for m in messages:
                    um = odot.osc_message_s_deserialize_r(m.getRaw())
                    odot.osc_bundle_u_addMsg(ub, um)
                self.__bundle = odot.osc_bundle_u_serialize(ub)
                odot.osc_bundle_u_free(ub)
                return
        else: # from string
            self.___fromString(text)

    def ___validate(self, data):
        __err = odot.osc_error_bundleSanityCheck(len(data), data)
        __result = False
        if __err == odot.OSC_ERR_NONE:
            __result = True
        return __result

    def __repr__(self):
        return self.__bundle 

    def __str__(self):
        return odot.osc_bundle_s_format(len(self.__bundle), self.__bundle)

    def __iter__(self):
        self.__iter = odot.osc_bundle_iterator_s_getIterator(len(self.__bundle), self.__bundle)
        return self

    def next(self):
        if odot.osc_bundle_iterator_s_hasNext(self.__iter) is 1:
            self.message = odot.osc_bundle_iterator_s_next(self.__iter)
            return message(binary = self.message)
        else:
            odot.osc_bundle_iterator_s_destroyIterator(self.__iter)
            del(self.__iter)
            del(self.message)
            raise StopIteration

    def __len__(self):
        return len(self.__bundle)

    def __bytes__(self):
        return self.__bundle

    def __getattr__(self, name):
        proper = '/' + name
        if self.checkAddress(proper):
            return self.getMessageWithAddress(proper).getData()
        else: raise AttributeError

    def __getitem__(self, key):
        if self.checkAddress(key):
            m = self.getMessageWithAddress(key)
            return m.getData()
        else: raise AttributeError

    def __setitem__(self, key, value):
        raise TypeError

    def __missing__(self, key):
        pass

    def __contains__(self, key):
        return checkAddress(key)

    def copy(self):
        return bundle(binary = self.__bundle)

    def append(self, message):
        print("append a message")

    def remove(self, address):
        print("removes a message with address" + str(address))

    def getAddresses(self):
        result = []
        for m in self:
            result.append(m.getAddress())
        return result

    def checkAddress(self, address):
        result = False
        for m in self:
            if m.getAddress() == address:
                return True
        return False

    def getMessageCount(self):
        counter = 0
        for m in self:
            counter += 1
        return counter

    def getMessageWithAddress(self, address):
        for m in self:
            if m.getAddress() == address:
                return m

    def getRaw(self):
        """possibly unnecessary due to __repr__ and __len__ overloads"""
        return self.__bundle

    def explode(self):
        print("returns an exploded copy of itself")

    def flatten(self):
        print("returns a flattened copy of itself")

    def toDictionary(self, strip_slashes=False):
        result = {}
        for m in self:
            result[m.getAddress()] = m.getData()
        return result

    def fromDictionary(self, d):
        if type(d) is dict :
            for key in d.keys():
                ___fixed_key = ___fixAddresses(key)
                if hasattr(d[key], '__iter__'):
                    print("will iterate")
                else :
                    print("will not iterate")

    def ___createEmptyBundle(self):
        __ub = odot.osc_bundle_u_alloc()
        self.__bundle = odot.osc_bundle_u_serialize(__ub)
        odot.osc_bundle_u_free(__ub)

    def ___fromString(self, text_bundle):
        if self.___validate(text_bundle):
            self.__bundle = text_bundle
            return
        __ub = odot.osc_parser_parseString_r(len(text_bundle) + 1, text_bundle)
        self.__bundle = odot.osc_bundle_u_serialize(__ub)
        odot.osc_bundle_u_free(__ub)
                                    
    def ___fixAddresses(key):
        if len(key > 0):
            if key[0] is '/':
                return key
            else : return '/' + key

def now():
    return timetag()

def validate(data):
    """
    Returns a bundle object if input data is a valid odot bundle.
    """
    if type(data) is bundle:
        return data.___validate()
    else:
        __err = odot.osc_error_bundleSanityCheck(len(data), data)
        __result = False
        if __err == odot.OSC_ERR_NONE:
            __result = True
        return __result

def difference(lhs, rhs):
    """
    Returns bundle containing the difference between lhs & rhs
    """

def union(lhs, rhs):
    """returns a new bundle containing the union between lhs & rhs")"""

def rcompliment(lhs, rhs):
    print("returns a bundle containing the bindings in rhs whose addresses are not also in lhs")
    pass

def intersection(lhs, rhs):
    print("returns an intersection between lhs & rhs")
    pass

def select(bundle, address):
    print("returns a bundle containing all messages with addresses that matched")
    pass
