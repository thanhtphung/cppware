/*
 * Software by Thanh Phung -- thanhtphung@yahoo.com.
 * No copyrights. No warranties. No restrictions in reuse.
 */
#ifndef SYSKIT_TRIE_HPP
#define SYSKIT_TRIE_HPP

#include <new>
#include <string.h>
#include "syskit/macros.h"

class TrieSuite;

BEGIN_NAMESPACE1(syskit)


//! digit trie
class Trie
    //!
    //! A class representing a digit trie of non-zero opaque items. A digit trie can
    //! serve as a map of digit strings (keys) to non-zero opaque items (values). A
    //! key is represented as a contiguous array of digits prefixed by its length. A
    //! key is valid for a trie if it has at least one digit and if its digits are
    //! within the trie's digit range. The digit range is specified at construction
    //! and cannot be changed. A few Trie::XxxKey classes are provided to help
    //! form normalized keys. A typical trie uses direct indexing to locate children
    //! nodes. This implementation uses direct indexing only when a node has more than
    //! 5 children. A linear search is used when a node has less than 5 children.
    //! Example:
    //!\code
    //! Trie trie;
    //! Trie::U32Key k(123);
    //! trie.add(k, (void*)(0xabc));
    //! void* foundV;
    //! if ((! trie.find(k, foundV)) || (foundV != (void*)(0xabc)))
    //! {
    //!   assert("coding error?" == 0);
    //! }
    //!\endcode
    //!
{

public:
    enum
    {
        DefaultMaxDigit = 15,
        MaxKeyLength = 255
    };

    typedef bool(*cb0_t)(void* arg, const unsigned char* k, void* v);
    typedef void(*cb1_t)(void* arg, const unsigned char* k, void* v);

    class Node;

    // Constructors and destructor.
    Trie(const Trie& trie);
    Trie(unsigned char maxDigit = DefaultMaxDigit);
    ~Trie();

    // Operators.
    bool operator !=(const Trie& trie) const;
    bool operator ==(const Trie& trie) const;
    const Trie& operator =(const Trie& trie);

    // Trie operations.
    bool add(const unsigned char* k, void* v);
    bool add(const unsigned char* k, void* v, void*& foundV);
    bool associate(const unsigned char* k, void* v);
    bool associate(const unsigned char* k, void* v, void*& oldV);
    bool find(const unsigned char* k) const;
    bool find(const unsigned char* k, void*& foundV) const;
    bool rm(const unsigned char* k);
    bool rm(const unsigned char* k, void*& removedV);
    unsigned int rmAll(const unsigned char* subkey);
    void reset();

    // Getters.
    unsigned char maxDigit() const;
    unsigned int numKvPairs() const;

    // Utilities.
    bool isValid(const unsigned char* key) const;
    unsigned int countKvPairs(const unsigned char* subkey) const;
    unsigned int numNodes() const;

    // Iterator support.
    bool applyChildFirst(cb0_t cb, void* arg = 0) const;
    bool applyParentFirst(cb0_t cb, void* arg = 0) const;
    const Node* root() const;
    void applyChildFirst(cb1_t cb, void* arg = 0) const;
    void applyParentFirst(cb1_t cb, void* arg = 0) const;


    //!
    //! ABC for all nodes.
    //!
    class Node
    {
    public:
        void setV(void* v);
        void* v() const;
        virtual ~Node();
        virtual Node* child(unsigned char digit) const = 0;
        virtual Node* clone() const = 0;
        virtual Node* rmChild(unsigned char digit, Node*& removedChild) = 0;
        virtual Node* setChild(unsigned char digit, Node* child, unsigned char maxDigit) = 0;
        virtual bool applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const = 0;
        virtual bool applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const = 0;
        virtual bool isLeaf() const = 0;
        virtual unsigned int countNodes(size_t& cumKvPairs) const = 0;
        virtual unsigned int numChildren() const = 0;
        virtual void applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const = 0;
        virtual void applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const = 0;
        static void operator delete(void* p, size_t size);
        static void* operator new(size_t size);
    protected:
        Node(void* v = 0);
    private:
        void* v_;
        Node(const Node&); //prohibit usage
        const Node& operator =(const Node&); //prohibit usage
    };


    //! null-terminated string key
    class StrKey
        //!
        //! A class representing a null-terminated string key. It helps form normalized
        //! keys used by a digit trie. For example, the normalized form of the
        //! ASCII "abc123" string is the 06-61-62-63-31-32-33 hexadecimal byte
        //! sequence. The normalized form of the hexadecimal "abc123" string is
        //! the 06-0a-0b-0c-01-02-03 hexadecimal byte sequence. The normalized
        //! form of the decimal "abc123" string is unpredictable since it contains
        //! non-decimal characters.
        //!
    {
    public:
        enum type_e
        {
            Ascii = 0,
            Dec,
            HEX,
            Hex
        };
        StrKey(const StrKey& key);
        StrKey(type_e type = Ascii, const char* key = "");
        StrKey(type_e type, const char* key, unsigned char n);
        ~StrKey();
        operator const unsigned char*() const;
        bool operator !=(const StrKey& key) const;
        bool operator ==(const StrKey& key) const;
        const StrKey& operator =(const StrKey& key);
        const unsigned char* asBytes() const;
        void reset(type_e type = Ascii, const char* key = "");
        void reset(type_e type, const char* key, unsigned char n);
        static char* decode(char* k, type_e type, const unsigned char* key);
        static char* decode(type_e type, unsigned char* key);
        static unsigned char* encode(type_e type, char* k);
        static unsigned char* encode(unsigned char* key, type_e type, const char* k);
        static unsigned char* encode(unsigned char* key, type_e type, const char* k, unsigned char n);
    private:
        typedef char* (*decoder_t)(char* k, const unsigned char* key);
        typedef unsigned char* (*encoder_t)(unsigned char* key, const char* k, unsigned char n);
        unsigned char* k_;
        static decoder_t decode_[];
        static encoder_t encode_[];
        static char* decodeAscii(char*, const unsigned char*);
        static char* decodeDec(char*, const unsigned char*);
        static char* decodeHEX(char*, const unsigned char*);
        static char* decodeHex(char*, const unsigned char*);
        static unsigned char* encodeAscii(unsigned char*, const char*, unsigned char);
        static unsigned char* encodeDec(unsigned char*, const char*, unsigned char);
        static unsigned char* encodeHex(unsigned char*, const char*, unsigned char);
    };


    //! unsigned 16-bit key
    class U16Key
        //!
        //! A class representing an unsigned 16-bit key. It helps form normalized keys
        //! used by a digit trie. This implementation breaks a 16-bit number into a
        //! sequence of 4 bytes with each byte holding 4 bits. For example, the
        //! normalized form of 0x1234U is the 04-01-02-03-04 hexadecimal byte sequence.
        //! Construct an instance or use encode() to normalize a native key. Use decode()
        //! to decode a normalized key. Example:
        //!\code
        //! Trie::U16Key key(k);
        //! assert(Trie::U16Key::decode(key) == k);
        //!\endcode
        //!
    {
    public:
        U16Key(unsigned short k = 0);
        operator const unsigned char*() const;
        const U16Key& operator =(unsigned short k);
        const unsigned char* asBytes() const;
        void reset(unsigned short k = 0);
        static unsigned char* encode(unsigned char k[1 + 4], unsigned short key);
        static unsigned short decode(const unsigned char* k);
    private:
        unsigned char k_[1 + 4];
        U16Key(const U16Key&); //prohibit usage
        const U16Key& operator =(const U16Key&); //prohibit usage
    };


    //! unsigned 28-bit key
    class U28Key
        //!
        //! A class representing an unsigned 28-bit key. It helps form normalized keys
        //! used by a digit trie. This implementation breaks a 28-bit number into a
        //! sequence of 7 bytes with each byte holding 4 bits. A 28-bit number resides
        //! in the least significant 28 bits of an unsigned int. For example, the
        //! normalized form of 0x01234567UL is the 07-01-02-03-04-05-06-07 hexadecimal
        //! byte sequence. Construct an instance or use encode() to normalize a native
        //! key. Use decode() to decode a normalized key. Example:
        //!\code
        //! Trie::U28Key key(k);
        //! assert(Trie::U28Key::decode(key) == k);
        //!\endcode
        //!
    {
    public:
        U28Key(unsigned int k = 0);
        operator const unsigned char*() const;
        const U28Key& operator =(unsigned int k);
        const unsigned char* asBytes() const;
        void reset(unsigned int k = 0);
        static unsigned char* encode(unsigned char k[1 + 7], unsigned int key);
        static unsigned int decode(const unsigned char* k);
    private:
        unsigned char k_[1 + 7];
        U28Key(const U28Key&); //prohibit usage
        const U28Key& operator =(const U28Key&); //prohibit usage
    };


    //! unsigned 32-bit key
    class U32Key
        //!
        //! A class representing an unsigned 32-bit key. It helps form normalized keys
        //! used by a digit trie. This implementation breaks a 32-bit number into a
        //! sequence of 8 bytes with each byte holding 4 bits. For example, the
        //! normalized form of 0x12345678UL is the 08-01-02-03-04-05-06-07-08 hexadecimal
        //! byte sequence. Construct an instance or use encode() to normalize a native
        //! key. Use decode() to decode a normalized key. Example:
        //!\code
        //! Trie::U32Key key(k);
        //! assert(Trie::U32Key::decode(key) == k);
        //!\endcode
        //!
    {
    public:
        U32Key(unsigned int k = 0);
        operator const unsigned char*() const;
        const U32Key& operator =(unsigned int k);
        const unsigned char* asBytes() const;
        void reset(unsigned int k = 0);
        static unsigned char* encode(unsigned char k[1 + 8], unsigned int key);
        static unsigned int decode(const unsigned char* k);
    private:
        unsigned char k_[1 + 8];
        U32Key(const U32Key&); //prohibit usage
        const U32Key& operator =(const U32Key&); //prohibit usage
    };


    //! unsigned 56-bit key
    class U56Key
        //!
        //! A class representing an unsigned 56-bit key. It helps form normalized keys
        //! used by a digit trie. This implementation breaks a 56-bit number into a
        //! sequence of 7 bytes with each byte holding 8 bits. A 56-bit number resides
        //! in the least significant 56 bits of an unsigned long long. For example, the
        //! normalized form of 0x00123456789abcdeULL is the 07-12-34-56-78-9a-bc-de
        //! hexadecimal byte sequence. Construct an instance or use encode() to normalize
        //! a native key. Use decode() to decode a normalized key. Example:
        //!\code
        //! Trie::U56Key key(k);
        //! assert(Trie::U56Key::decode(key) == k);
        //!\endcode
        //!
    {
    public:
        U56Key(unsigned long long k = 0);
        operator const unsigned char*() const;
        const U56Key& operator =(unsigned long long k);
        const unsigned char* asBytes() const;
        void reset(unsigned long long k = 0);
        static unsigned char* encode(unsigned char k[1 + 7], unsigned long long key);
        static unsigned long long decode(const unsigned char* k);
    private:
        unsigned char k_[1 + 7];
        U56Key(const U56Key&); //prohibit usage
        const U56Key& operator =(const U56Key&); //prohibit usage
    };


    //! unsigned 64-bit key
    class U64Key
        //!
        //! A class representing an unsigned 64-bit key. It helps form normalized keys
        //! used by a digit trie. This implementation breaks a 32-bit number into a
        //! sequence of 8 bytes with each byte holding 8 bits. For example, the
        //! normalized form of 0x123456789abcdef0ULL is the 08-12-34-56-78-9a-bc-de-f0
        //! hexadecimal byte sequence. Construct an instance or use encode() to normalize
        //! a native key. Use decode() to decode a normalized key. Example:
        //!\code
        //! Trie::U64Key key(k);
        //! assert(Trie::U64Key::decode(key) == k);
        //!\endcode
        //!
    {
    public:
        U64Key(unsigned long long k = 0);
        operator const unsigned char*() const;
        const U64Key& operator =(unsigned long long k);
        const unsigned char* asBytes() const;
        void reset(unsigned long long k = 0);
        static unsigned char* encode(unsigned char k[1 + 8], unsigned long long key);
        static unsigned long long decode(const unsigned char* k);
    private:
        unsigned char k_[1 + 8];
        U64Key(const U64Key&); //prohibit usage
        const U64Key& operator =(const U64Key&); //prohibit usage
    };

private:

    //
    // Leaf node.
    //
    class Node0: public Node
    {
    public:
        Node0(void* v = 0);
        virtual ~Node0();
        virtual Node* child(unsigned char digit) const;
        virtual Node* clone() const;
        virtual Node* rmChild(unsigned char digit, Node*& removedChild);
        virtual Node* setChild(unsigned char digit, Node* child, unsigned char maxDigit);
        virtual bool applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const;
        virtual bool applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const;
        virtual bool isLeaf() const;
        virtual unsigned int countNodes(size_t& cumKvPairs) const;
        virtual unsigned int numChildren() const;
        virtual void applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const;
        virtual void applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const;
    private:
        Node0(const Node0&); //prohibit usage
        const Node0& operator =(const Node0&); //prohibit usage
    };

    //
    // 1-child node.
    //
    class Node1: public Node
    {
    public:
        Node1(void* v, unsigned char digit, Node* child);
        virtual ~Node1();
        virtual Node* child(unsigned char digit) const;
        virtual Node* clone() const;
        virtual Node* rmChild(unsigned char digit, Node*& removedChild);
        virtual Node* setChild(unsigned char digit, Node* child, unsigned char maxDigit);
        virtual bool applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const;
        virtual bool applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const;
        virtual bool isLeaf() const;
        virtual unsigned int countNodes(size_t& cumKvPairs) const;
        virtual unsigned int numChildren() const;
        virtual void applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const;
        virtual void applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const;
    private:
        Node* child_[1];
        unsigned char digit_[1];
        Node1(const Node1&); //prohibit usage
        const Node1& operator =(const Node1&); //prohibit usage
    };

    //
    // Node having 2 to 4 children.
    //
    class Node4: public Node
    {
    public:
        Node4(void* v, unsigned char const digit[4], Node* const child[4]);
        Node4(void* value, unsigned char digit0, Node* child0, unsigned char digit1, Node* child1);
        virtual ~Node4();
        virtual Node* child(unsigned char digit) const;
        virtual Node* clone() const;
        virtual Node* rmChild(unsigned char digit, Node*& removedChild);
        virtual Node* setChild(unsigned char digit, Node* child, unsigned char maxDigit);
        virtual bool applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const;
        virtual bool applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const;
        virtual bool isLeaf() const;
        virtual unsigned int countNodes(size_t& cumKvPairs) const;
        virtual unsigned int numChildren() const;
        virtual void applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const;
        virtual void applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const;
    private:
        Node* child_[4];
        unsigned char digit_[4];
        Node4(const Node4&); //prohibit usage
        const Node4& operator =(const Node4&); //prohibit usage
    };

    //
    // Highest-capacity node.
    //
    class NodeN: public Node
    {
    public:
        NodeN(void* v,
            unsigned char const digit[4],
            Node* const child[4],
            unsigned char digit4,
            Node* child4,
            unsigned char maxDigit);
        virtual ~NodeN();
        virtual Node* child(unsigned char digit) const;
        virtual Node* clone() const;
        virtual Node* rmChild(unsigned char digit, Node*& removedChild);
        virtual Node* setChild(unsigned char digit, Node* child, unsigned char maxDigit);
        virtual bool applyChildFirst(unsigned char* k, cb0_t cb, void* arg) const;
        virtual bool applyParentFirst(unsigned char* k, cb0_t cb, void* arg) const;
        virtual bool isLeaf() const;
        virtual unsigned int countNodes(size_t& cumKvPairs) const;
        virtual unsigned int numChildren() const;
        virtual void applyChildFirst(unsigned char* k, cb1_t cb, void* arg) const;
        virtual void applyParentFirst(unsigned char* k, cb1_t cb, void* arg) const;
    private:
        Node** child_;
        unsigned char maxDigit_;
        unsigned char numChildren_;
        const NodeN& operator =(const NodeN&); //prohibit usage
        NodeN(const NodeN&);
    };

    Node* root_;
    unsigned char maxDigit_;
    unsigned int numKvPairs_;
    unsigned int numNodes_;

    static Node* const NODE_ADDED;

    Node* addNode(const unsigned char*, void*);
    Node* findNode(const unsigned char*) const;
    Node* mkNodes(const unsigned char*, const unsigned char*, void*) const;
    bool addKv(const unsigned char*, void*, void*&);
    bool associateKv(const unsigned char*, void*, void*&);

    static bool isEqual(void*, const unsigned char*, void*);
    static void addNode(void*, const unsigned char*, void*);

    friend class ::TrieSuite;

};

END_NAMESPACE1

#include "syskit/BufPool.hpp"

BEGIN_NAMESPACE1(syskit)

inline void Trie::Node::operator delete(void* p, size_t size)
{
    BufPool::freeBuf(p, size);
}

inline void* Trie::Node::operator new(size_t size)
{
    void* buf = BufPool::allocateBuf(size);
    return buf;
}

inline void Trie::Node::setV(void* v)
{
    v_ = v;
}

inline void* Trie::Node::v() const
{
    return v_;
}

//! Return true if this trie does not equal given trie. That is,
//! if both do not have the same number of key-value pairs or
//! if corresponding key-value pairs are not equal.
inline bool Trie::operator !=(const Trie& trie) const
{
    return !(*this == trie);
}

//! Add given key-value pair. Return true if successful. Return
//! false otherwise (i.e., key already exists or key is invalid
//! or value is zero). A valid key is non-null and has at least
//! one digit and its digits are within the trie's digit range.
inline bool Trie::add(const unsigned char* k, void* v)
{
    void* foundV;
    bool ok = ((k != 0) && (v != 0))? addKv(k, v, foundV): false;
    return ok;
}

//! Add given key-value pair. Return true if successful. Return
//! false otherwise (i.e., key already exists or key is invalid
//! or value is zero). A valid key is non-null and has at least
//! one digit and its digits are within the trie's digit range.
inline bool Trie::add(const unsigned char* k, void* v, void*& foundV)
{
    bool ok = ((k != 0) && (v != 0))? addKv(k, v, foundV): false;
    return ok;
}

//! Associate given key-value pair. If given key does not exist, this
//! is equivalent to the add() method. If given key already exists, its
//! associated value is updated with given value. Return true if successful.
//! Return false otherwise (i.e., key is invalid). A valid key is non-null
//! and has at least one digit and its digits are within the trie's digit
//! range.
inline bool Trie::associate(const unsigned char* k, void* v)
{
    void* oldV;
    bool ok = ((k != 0) && (v != 0))? associateKv(k, v, oldV): false;
    return ok;
}

//! Associate given key-value pair. If given key does not exist, this is
//! equivalent to the add() method, and oldV is set to zero. If given key
//! already exists, its associated value is saved in oldV before being
//! updated with given value. Return true if successful. Return false
//! otherwise (i.e., key is invalid). A valid key is non-null and has at
//! least one digit and its digits are within the trie's digit range.
inline bool Trie::associate(const unsigned char* k, void* v, void*& oldV)
{
    bool ok = ((k != 0) && (v != 0))? associateKv(k, v, oldV): false;
    return ok;
}

//! Locate given key. Return true if found. Return false otherwise.
inline bool Trie::find(const unsigned char* key) const
{
    const Node* p = findNode(key);
    bool found = (p->v() != 0);
    return found;
}

//! Locate given key. If found, remove the key-value pair from the
//! trie and return true. Return false otherwise.
inline bool Trie::rm(const unsigned char* key)
{
    void* removedValue;
    bool found = rm(key, removedValue);
    return found;
}

//! Return the root node.
//! The root node exists even in an empty trie.
inline const Trie::Node* Trie::root() const
{
    return root_;
}

//! Return the node capacity. A node can have at most maxDigit()+1
//! children. The node capacity is specified at construction and
//! cannot be changed.
inline unsigned char Trie::maxDigit() const
{
    return maxDigit_;
}

//! Return the current number of key-value pairs in the trie.
inline unsigned int Trie::numKvPairs() const
{
    return numKvPairs_;
}

//! Return the current number of nodes in the trie. An empty trie has
//! one empty root node. A trie with one "AB" key has three nodes: root
//! node having child A and grandchild B. Adding key "AC" to the trie
//! requires one new node C: a new child of A. This information helps
//! determine how sparse a trie is.
inline unsigned int Trie::numNodes() const
{
    return numNodes_;
}

//! Return the normalized key.
inline Trie::StrKey::operator const unsigned char*() const
{
    return k_;
}

//! Return true if this key does not equal given key.
inline bool Trie::StrKey::operator !=(const StrKey& key) const
{
    bool eq = (k_[0] == key.k_[0]) && (memcmp(k_ + 1, key.k_ + 1, k_[0]) == 0);
    return (!eq);
}

//! Return true if this key equals given key.
inline bool Trie::StrKey::operator ==(const StrKey& key) const
{
    bool eq = (k_[0] == key.k_[0]) && (memcmp(k_ + 1, key.k_ + 1, k_[0]) == 0);
    return eq;
}

//! Decode the normalized key k which was formed using StrKey and given
//! type. Save result in key, a caller-provided buffer big enough for
//! (k[0]+1) characters. Return the native key. For type Hex or HEX,
//! encoding is case-insensitive, but decoding can return a lower or
//! uppercase hexadecimal string.
inline char* Trie::StrKey::decode(char* key, type_e type, const unsigned char* k)
{
    return decode_[type](key, k);
}

//! Decode the normalized key k which was formed using StrKey and given type.
//! Save result in the same buffer. Return the native key. For type Hex or HEX,
//! encoding is case-insensitive, but decoding can return a lower or uppercase
//! hexadecimal string.
inline char* Trie::StrKey::decode(type_e type, unsigned char* k)
{
    return decode_[type](reinterpret_cast<char*>(k), k);
}

//! Return the normalized key.
inline const unsigned char* Trie::StrKey::asBytes() const
{
    return k_;
}

//! Encode given native key key. Save result in the same buffer. Return the
//! normalized key. For type Hex or HEX, encoding is case-insensitive. For
//! example, "abc123" and "ABc123" would result in an identical normalized
//! key.
inline unsigned char* Trie::StrKey::encode(type_e type, char* key)
{
    unsigned char n = static_cast<unsigned char>(strlen(key));
    return encode_[type](reinterpret_cast<unsigned char*>(key), key, n);
}

//! Encode given native key key. Save result in k, a caller-provided buffer big
//! enough for (1+trlen(key)) bytes. Return the normalized key k. For type Hex or
//! HEX, encoding is case-insensitive. For example, "abc123" and "ABc123" would
//! result in an identical normalized key.
inline unsigned char* Trie::StrKey::encode(unsigned char* k, type_e type, const char* key)
{
    unsigned char n = static_cast<unsigned char>(strlen(key));
    return encode_[type](k, key, n);
}

//! Encode given native key (n characters starting at key). Save result in k, a
//! caller-provided buffer big enough for (1+n) bytes. Return the normalized key
//! k. For type Hex or HEX, encoding is case-insensitive. For example, "abc123"
//! and "ABc123" would result in an identical normalized key.
inline unsigned char* Trie::StrKey::encode(unsigned char* k, type_e type, const char* key, unsigned char n)
{
    return encode_[type](k, key, n);
}

//! Construct instance w/ native key key.
inline Trie::U16Key::U16Key(unsigned short key)
{
    encode(k_, key);
}

//! Return the normalized key.
inline Trie::U16Key::operator const unsigned char*() const
{
    return k_;
}

//! Reset instance w/ native key key. Return reference to self.
inline const Trie::U16Key& Trie::U16Key::operator =(unsigned short key)
{
    encode(k_, key);
    return *this;
}

//! Return the normalized key.
inline const unsigned char* Trie::U16Key::asBytes() const
{
    return k_;
}

//! Reset instance w/ native key key.
inline void Trie::U16Key::reset(unsigned short key)
{
    encode(k_, key);
}

//! Construct instance w/ native key key.
inline Trie::U28Key::U28Key(unsigned int key)
{
    encode(k_, key);
}

//! Return the normalized key.
inline Trie::U28Key::operator const unsigned char*() const
{
    return k_;
}

//! Reset instance w/ native key key. Return reference to self.
inline const Trie::U28Key& Trie::U28Key::operator =(unsigned int key)
{
    encode(k_, key);
    return *this;
}

//! Return the normalized key.
inline const unsigned char* Trie::U28Key::asBytes() const
{
    return k_;
}

//! Reset instance w/ native key key.
inline void Trie::U28Key::reset(unsigned int key)
{
    encode(k_, key);
}

//! Construct instance w/ native key key.
inline Trie::U32Key::U32Key(unsigned int key)
{
    encode(k_, key);
}

//! Return the normalized key.
inline Trie::U32Key::operator const unsigned char*() const
{
    return k_;
}

//! Reset instance w/ native key key. Return reference to self.
inline const Trie::U32Key& Trie::U32Key::operator =(unsigned int key)
{
    encode(k_, key);
    return *this;
}

//! Return the normalized key.
inline const unsigned char* Trie::U32Key::asBytes() const
{
    return k_;
}

//! Reset instance w/ native key key.
inline void Trie::U32Key::reset(unsigned int key)
{
    encode(k_, key);
}

//! Construct instance w/ native key key.
inline Trie::U56Key::U56Key(unsigned long long key)
{
    encode(k_, key);
}

//! Return the normalized key.
inline Trie::U56Key::operator const unsigned char*() const
{
    return k_;
}

//! Reset instance w/ native key key. Return reference to self.
inline const Trie::U56Key& Trie::U56Key::operator =(unsigned long long key)
{
    encode(k_, key);
    return *this;
}

//! Return the normalized key.
inline const unsigned char* Trie::U56Key::asBytes() const
{
    return k_;
}

//! Reset instance w/ native key key.
inline void Trie::U56Key::reset(unsigned long long key)
{
    encode(k_, key);
}

//! Construct instance w/ native key key.
inline Trie::U64Key::U64Key(unsigned long long key)
{
    encode(k_, key);
}

//! Return the normalized key.
inline Trie::U64Key::operator const unsigned char*() const
{
    return k_;
}

//! Reset instance w/ native key key. Return reference to self.
inline const Trie::U64Key& Trie::U64Key::operator =(unsigned long long key)
{
    encode(k_, key);
    return *this;
}

//! Return the normalized key.
inline const unsigned char* Trie::U64Key::asBytes() const
{
    return k_;
}

//! Reset instance w/ native key key.
inline void Trie::U64Key::reset(unsigned long long key)
{
    encode(k_, key);
}

END_NAMESPACE1

#endif
