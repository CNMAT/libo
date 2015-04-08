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

    ### no need for __del__ as timetags are initialized on the stack

    def __repr__(self):
        return odot.osc_timetag_format(self.__tt)

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
            return odot.osc_timetag_timetagToFloat(result)
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
            return odot.osc_timetag_timetagToFloat(result)
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
    An immutable binding of data to an address.

    Attributes
    ----------
    NO PUBLIC ATTRIBUTES

    All interactions are handled through dedicated functions. 
    """
    def __init__(self, address = None, values = None, binary = None):
        ### TODO: binary keyword requires cloning semantics
        ### self.__dealloc is a bug
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
            result = o.bundle(binary = odot.osc_atom_s_getBndlCopy(atom))
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
    Immutable, iterable dictionary container.

    Attributes
    ----------
    NO PUBLIC MEMBERS

    All interactions are handled through dedicated functions.

    Functions
    ---------
    len(bundle)                     - returns the number of bindings in a bundle
    str(bundle)                     - pretty-prints the bundle according to CNMAT odot conventions
    .append(message)                - returns a copy of the bundle with a new message appended at the end
    .checkAddress(string)           - returns True if the address is in the bundle, False otherwise
    .copy()                         - returns a copy of the bundle's contents (with an updated header timetag)
    .explode()                      - returns a copy of the bundle with subbundles representing nested addresses
    .flatten()                      - returns a copy of the bundle with subbundles flattened to a singular namespace
    .fromDictionary()               - TODO: move this and toDictionary() out of the bundle class
    .getAddresses()                 - returns a list of addresses in the bundle
    .getBytes()                     - returns a network-friendly binary encoding of the data (Python 3.x use bytes())
    .getMessageWithAddress(string)  - returns a message with matched address
    .getRaw()                       - returns the SWIG object representing a bundle, not recommended outside of internal use
    .getTimetag()                   - returns a timetag stored in the bundle's header (guaranteed to be set during construction)
    .next()                         - used by __iter__ - will throw an exception if used outside of iteration
    .remove(string)                 - returns a copy of the bundle with the indicated address removed
    .toDictionary()                 - TODO: move this and fromDictionary() out of the bundle class
    """
    def __init__(self, text=None, messages=None, binary=None):
        """
        Bundle Constructor Arguments
        ----------------------------
            * text = < textual bundle representation >
              example: o.bundle('/foo : 1, /bar : [2, 3, 4]')
            * messages = < list of odot messages >
              example: o.bundle(messages=[o.message('/foo', 1), o.message('/bar', [2, 3, 4])])
            * binary = < an array of bytes, e.g. from a UDP packet > (Python String in Python 2.x, bytes() in Python 3.x)
              example: o.bundle(bytes = from_udp)
        """
        if text is None:
            if messages is None:
                if binary is None:
                    self.___createEmptyBundle()
                else: # using binary -- TODO: validate first!
                    ### note that setting a header timetag returns a copy of the bundle
                    self.__bundle = odot.osc_bundle_s_setTimetag_p(self.__bundle, now().getRaw())
            else: # messages
                ub = odot.osc_bundle_u_alloc()
                for m in messages:
                    um = odot.osc_message_s_deserialize_r(m.getRaw())
                    odot.osc_bundle_u_addMsg(ub, um)
                temp = odot.osc_bundle_u_serialize(ub)
                self.__bundle = osc_bundle_s_setTimetag_p(self.__temp, now().getRaw())
                odot.osc_bundle_s_deepFree(temp)
                del(temp)
                odot.osc_bundle_u_free(ub)
                return
        else: # from string
            self.___fromString(text)

    def __del__(self):
        odot.osc_bundle_s_deepFree(self.__bundle)
        try: self.__iter
        except NameError: pass
        except AttributeError: pass
        else:
            odot.osc_bundle_iterator_s_destroyIterator(self.__iter)

    def ___validate(self, data):
        __err = odot.osc_error_bundleSanityCheck(len(data), data)
        __result = False
        if __err == odot.OSC_ERR_NONE:
            __result = True
        return __result

    def __repr__(self):
        return odot.osc_bundle_s_format_p(self.__bundle) 

    def __str__(self):
        return odot.osc_bundle_s_format_p(self.__bundle)

    def __iter__(self):
        ### clean up previous iterator, if one exists...
        try: self.__iter
        except NameError: pass
        except AttributeError: pass
        else:
            odot.osc_bundle_iterator_s_destroyIterator(self.__iter)
        ### create a new iterator:
        self.__iter = odot.osc_bundle_iterator_s_getIterator_p(self.__bundle)
        return self

    def next(self):
        if odot.osc_bundle_iterator_s_hasNext(self.__iter) is 1:
            return message(binary = odot.osc_bundle_iterator_s_next(self.__iter))
        else:
            odot.osc_bundle_iterator_s_destroyIterator(self.__iter)
            del(self.__iter)
            raise StopIteration

    def __len__(self):
        return odot.osc_bundle_s_getMsgCount_p(self.__bundle)

    def __bytes__(self):
        return odot.osc_bundle_s_swugged(self.__bundle)

    def getBytes(self):
        return odot.osc_bundle_s_swugged(self.__bundle)

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
        return bundle(binary = odot.osc_bundle_s_setTimetag_p(self.__bundle, now().getRaw()))

    def append(self, message):
        pass

    def remove(self, address):
        pass

    def getTimetag(self):
        return timetag(odot.osc_bundle_s_getTimetag_p(self.__bundle))

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

    def getMessageWithAddress(self, address):
        for m in self:
            if m.getAddress() == address:
                return m

    def getRaw(self):
        return self.__bundle

    def explode(self):
        pass

    def flatten(self):
        pass

    def toDictionary(self, strip_slashes=False):
        result = {}
        for m in self:
            if type(m.getData()) is 'bundle':
                result[m.getAddress()] = toDictionary(m.getData())
            else:
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
        self.__temp = odot.osc_bundle_u_serialize(__ub)
        self.__bundle = odot.osc_bundle_s_setTimetag_p(self.__temp, now().getRaw())
        odot.osc_bundle_s_deepFree(self.__temp)
        del(self.__temp)
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
    pass

def union(lhs, rhs):
    """returns a new bundle containing the union between lhs & rhs")"""
    pass

def rcompliment(lhs, rhs):
    """returns a bundle containing the bindings in rhs whose addresses are not also in lhs"""
    pass

def intersection(lhs, rhs):
    """returns an intersection between lhs & rhs"""
    pass

def select(bundle, address):
    """returns a bundle containing all messages with addresses that matched"""
    pass
