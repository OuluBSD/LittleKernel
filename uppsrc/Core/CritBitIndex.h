// CritBitIndex.h
// Header-only Patricia/Crit-bit index over fixed-width hash keys
#pragma once

#include <Core/Core.h>

// Generic, compressed binary trie (Patricia / crit-bit) keyed by fixed-width unsigned hash
// Uses MSB-first bit order (bit 0 is the highest bit).
// Container stores records T and an associated Hash key per record.
//
// Iteration
// - Value iteration: for (auto& v : idx) iterates T& in dense storage order.
// - Key+value iteration: for (auto kv : ~idx) iterates items with fields { key, value }.
//   kv.key is the hash key (dword/uint64 by value), kv.value is T& (const T& for const idx).
// - Iteration order is the dense storage order of recs_ (not a tree traversal).
//
// Invalidation
// - Any structural modification (insert/remove/Optimize/Merge) may invalidate iterators
//   and change iteration order due to dense storage re-packing (swap-delete).

template <bool B, class T, class F>
struct __CrtbSelect { using Result = T; };
template <class T, class F>
struct __CrtbSelect<false, T, F> { using Result = F; };

template <class T, class GetHash, int Bits = 64>
class CritBitIndex {
public:
    using Hash   = typename __CrtbSelect<Bits == 32, dword, uint64>::Result;
    using Size   = int;
    using Handle = int;

    CritBitIndex() : root_(-1) {}

    void Clear() {
        nodes_.Clear();
        recs_.Clear();
        keys_.Clear();
        leaf_of_rec_.Clear();
        root_ = -1;
    }

    bool IsEmpty() const          { return recs_.IsEmpty(); }
    Size GetCount() const         { return recs_.GetCount(); }

    T* Find(Hash h)               { return const_cast<T*>(static_cast<const CritBitIndex*>(this)->Find(h)); }

    const T* Find(Hash h) const {
        int n = root_;
        while(n >= 0) {
            const Node& nd = nodes_[n];
            if(nd.idx_bit < 0)
                return keys_[nd.rec] == h ? &recs_[nd.rec] : nullptr;
            n = Bit(h, nd.idx_bit) ? nd.right : nd.left;
        }
        return nullptr;
    }

    // Upsert by key: default-construct if not present
    T* Put(Hash h) {
        if(T* p = Find(h))
            return p;
        int rec = recs_.GetCount();
        recs_.Add(T());
        keys_.Add(h);
        leaf_of_rec_.Add(-1);
        if(root_ < 0) {
            int leaf = NewLeaf(rec);
            leaf_of_rec_[rec] = leaf;
            root_ = leaf;
            return &recs_[rec];
        }
        InsertRecord(h, rec);
        return &recs_[rec];
    }

    T* Put(Hash h, const T& value) {
        T* p = Put(h);
        *p = value;
        return p;
    }

    T* Put(Hash h, T&& value) {
        T* p = Put(h);
        *p = pick(value);
        return p;
    }

    bool Remove(Hash h) {
        if(root_ < 0)
            return false;
        bool removed = false;
        int  removed_rec = -1;
        int  removed_leaf = -1;
        root_ = RemoveAt(root_, h, removed, removed_leaf, removed_rec);
        if(!removed)
            return false;

        // Maintain dense recs_/keys_ using swap-delete and fix leaf_of_rec_ mapping
        int last = recs_.GetCount() - 1;
        if(removed_rec != last) {
            // Move last into removed_rec slot
            recs_[removed_rec] = pick(recs_.Top());
            keys_[removed_rec] = keys_.Top();
            int moved_leaf = leaf_of_rec_.Top();
            if(moved_leaf >= 0) {
                nodes_[moved_leaf].rec = removed_rec;
                leaf_of_rec_[removed_rec] = moved_leaf;
            }
        }
        recs_.Drop();
        keys_.Drop();
        leaf_of_rec_.Drop();
        return true;
    }

    void Optimize() {
        if(root_ >= 0)
            root_ = OptimizeAt(root_);
    }

    void Merge(CritBitIndex& other, bool keepThis = true) {
        for(int i = 0; i < other.recs_.GetCount(); i++) {
            T& t = other.recs_[i];
            Hash h = other.keys_[i];
            // Prefer provided accessor if keys are not trusted externally
            // But here we keep 'h' from other to avoid recomputing
            T* dst = Find(h);
            if(!dst)
                Put(h, pick(t));
            else if(!keepThis)
                *dst = pick(t);
        }
        other.Clear();
    }

    void PutBulk(const Vector<Hash>& keys, Vector<T*>& out) {
        out.Clear();
        out.SetCount(keys.GetCount());
        recs_.Reserve(recs_.GetCount() + keys.GetCount());
        nodes_.Reserve(nodes_.GetCount() + keys.GetCount() * 2);
        leaf_of_rec_.Reserve(leaf_of_rec_.GetCount() + keys.GetCount());
        for(int i = 0; i < keys.GetCount(); i++)
            out[i] = Put(keys[i]);
    }

    template <class Fn>
    void ForEach(Fn&& fn) {
        for(int i = 0; i < recs_.GetCount(); i++)
            fn(recs_[i]);
    }

    // --- NEW: Readable tree dump ---
    // Returns an indented, multi-line string representation of the internal
    // compressed binary trie. 'indent' adds leading spaces for each line.
    // Diagnostics-only: subtree sizes are computed on the fly (O(n)).
    String GetTreeString(int indent = 0) const {
        String out;
        auto AppendTabs = [&](int n) {
            if(n <= 0) return;
            for(int i = 0; i < n; i++)
                out.Cat('\t');
        };
        auto HexKey = [&](Hash h) -> String {
            if constexpr(Bits == 32)
                return Format("0x%08X", (int)h);
            else
                return Format("0x%08X%08X", (int)((h >> 32) & 0xFFFFFFFF), (int)(h & 0xFFFFFFFF));
        };
        // subtree size helper
        auto SizeOf = [&](auto&& self, int node) -> int {
            if(node < 0)
                return 0;
            const Node& nd = nodes_[node];
            if(nd.idx_bit < 0)
                return 1;
            return self(self, nd.left) + self(self, nd.right);
        };
        auto Dump = [&](auto&& self, int node, int depth) -> void {
            if (node < 0) {
				out.Cat("∅\n");
				return;
			}
            const Node& nd = nodes_[node];
            if (nd.idx_bit < 0) {
                out.Cat(Format("Leaf(rec=%d, key=%s)\n", nd.rec, ~HexKey(keys_[nd.rec])));
                return;
            }
            out.Cat(Format("Node(bit=%d, size=%d)\n", nd.idx_bit, SizeOf(SizeOf, node)));
            // Left branch
            AppendTabs(indent + depth + 1);
            out.Cat("L: ");
            if (nd.left >= 0 && nodes_[nd.left].idx_bit >= 0)
				self(self, nd.left, depth + 2);
            else
				self(self, nd.left, depth + 1);
            // Right branch
            AppendTabs(indent + depth + 1);
            out.Cat("R: ");
            if (nd.right >= 0 && nodes_[nd.right].idx_bit >= 0)
				self(self, nd.right, depth + 2);
            else
				self(self, nd.right, depth + 1);
        };
        if(root_ < 0) { AppendTabs(indent); out.Cat("∅\n"); return out; }
        AppendTabs(indent);
        Dump(Dump, root_, 0);
        return out;
    }

    // --- NEW: C++-style iteration over stored records (dense order) ---
    // Iteration & invalidation:
    // - Iteration order corresponds to dense storage order in recs_.
    // - Removing elements uses swap-delete; this invalidates indices and iterators.
    class Iterator {
    public:
        using difference_type = int;
        using value_type      = T;
        using reference       = T&;
        using pointer         = T*;

        Iterator(CritBitIndex* owner, int i) : owner(owner), i(i) {}
        reference operator*()  const { return owner->recs_[i]; }
        pointer   operator->() const { return &owner->recs_[i]; }
        Iterator& operator++()       { ++i; return *this; }
        bool operator!=(const Iterator& rhs) const { return i != rhs.i; }
    private:
        CritBitIndex* owner;
        int i;
    };

    class ConstIterator {
    public:
        using difference_type = int;
        using value_type      = const T;
        using reference       = const T&;
        using pointer         = const T*;

        ConstIterator(const CritBitIndex* owner, int i) : owner(owner), i(i) {}
        reference operator*()  const { return owner->recs_[i]; }
        pointer   operator->() const { return &owner->recs_[i]; }
        ConstIterator& operator++()  { ++i; return *this; }
        bool operator!=(const ConstIterator& rhs) const { return i != rhs.i; }
    private:
        const CritBitIndex* owner;
        int i;
    };

    Iterator      begin()       { return Iterator(this, 0); }
    Iterator      end()         { return Iterator(this, recs_.GetCount()); }
    ConstIterator begin() const { return ConstIterator(this, 0); }
    ConstIterator end()   const { return ConstIterator(this, recs_.GetCount()); }
    ConstIterator cbegin()const { return ConstIterator(this, 0); }
    ConstIterator cend()  const { return ConstIterator(this, recs_.GetCount()); }

public:
    // --- NEW: U++-style key+value iteration support for ~idx ---
    struct KeyValueItem {
        Hash key;
        T&   value;
    };

    class KVIterator {
    public:
        using difference_type = int;
        using value_type      = KeyValueItem;
        using reference       = KeyValueItem; // returned by value (holds T&)
        using pointer         = void;         // not used

        KVIterator(CritBitIndex* owner, int i) : owner(owner), i(i) {}
        KeyValueItem operator*() const { return { owner->keys_[i], owner->recs_[i] }; }
        KVIterator&  operator++()      { ++i; return *this; }
        bool operator!=(const KVIterator& rhs) const { return i != rhs.i; }
    private:
        CritBitIndex* owner;
        int i;
    };

    class ConstKVIterator {
    public:
        using difference_type = int;
        struct ConstItem { Hash key; const T& value; };

        ConstKVIterator(const CritBitIndex* owner, int i) : owner(owner), i(i) {}
        ConstItem operator*() const { return { owner->keys_[i], owner->recs_[i] }; }
        ConstKVIterator& operator++() { ++i; return *this; }
        bool operator!=(const ConstKVIterator& rhs) const { return i != rhs.i; }
    private:
        const CritBitIndex* owner;
        int i;
    };

    class KVRange {
    public:
        explicit KVRange(CritBitIndex* owner) : owner(owner) {}
        KVIterator begin() const { return KVIterator(owner, 0); }
        KVIterator end()   const { return KVIterator(owner, owner->recs_.GetCount()); }
    private:
        CritBitIndex* owner;
    };

    class ConstKVRange {
    public:
        explicit ConstKVRange(const CritBitIndex* owner) : owner(owner) {}
        ConstKVIterator begin() const { return ConstKVIterator(owner, 0); }
        ConstKVIterator end()   const { return ConstKVIterator(owner, owner->recs_.GetCount()); }
    private:
        const CritBitIndex* owner;
    };

    KVRange      AsKV()       { return KVRange(this); }
    ConstKVRange AsKV() const { return ConstKVRange(this); }

private:
    struct Node {
        int idx_bit; // -1 for leaf
        int left;
        int right;
        int rec;     // valid for leaf
    };

    Array<Node>  nodes_;
    Array<T>     recs_;
    Vector<Hash> keys_;
    Vector<int>  leaf_of_rec_; // rec_index -> leaf node index

    int          root_;
    GetHash      get_;

    // Bit utilities
    static bool Bit(Hash h, int bit) {
        int shift = (Bits - 1) - bit;
        ASSERT(shift >= 0 && shift <= Bits);
        return ((h >> (Hash)shift) & 1) != 0;
    }

    static Hash Divergence(Hash a, Hash b) {
        Hash x = a ^ b;
        if(x == 0)
            return -1;
        Hash hi;
        if constexpr(Bits == 64)
            hi = SignificantBits64(x) - 1; // index from LSB
        else
            hi = SignificantBits((dword)x) - 1;
        return (Bits - 1) - hi;            // index from MSB
    }

    int NewLeaf(int rec_index) {
        int id = nodes_.GetCount();
        Node& n = nodes_.Add();
        n.idx_bit = -1;
        n.left = -1;
        n.right = -1;
        n.rec = rec_index;
        return id;
    }

    int NewInternal(int bit, int left, int right) {
        int id = nodes_.GetCount();
        Node& n = nodes_.Add();
        n.idx_bit = bit;
        n.left = left;
        n.right = right;
        n.rec = -1;
        return id;
    }

    void InsertRecord(Hash h, int rec_index) {
        // Two-phase insertion with local rotation on return to maintain order
        root_ = InsertAt(root_, h, rec_index);
    }

    int InsertAt(int n, Hash h, int rec_index) {
        if(n < 0) {
            int leaf = NewLeaf(rec_index);
            leaf_of_rec_[rec_index] = leaf;
            return leaf;
        }

        Node& nd = nodes_[n];
        if(nd.idx_bit < 0) {
            // Leaf: either duplicate or create internal at divergence bit
            Hash h0 = keys_[nd.rec];
            int d = Divergence(h, h0);
            if(d < 0) {
                // Duplicate: should not happen as caller checks, but be defensive
                return n;
            }
            int newleaf = NewLeaf(rec_index);
            leaf_of_rec_[rec_index] = newleaf;
            bool bnew = Bit(h, d);
            int left  = bnew ? n : newleaf;
            int right = bnew ? newleaf : n;
            return NewInternal(d, left, right);
        }

        // Internal: descend
        bool b = Bit(h, nd.idx_bit);
        int child = b ? nd.right : nd.left;
        int newchild = InsertAt(child, h, rec_index);

        if(newchild == child)
            return n; // no structural change

        // If new split is above current bit, rotate newchild above this node
        const Node& nc = nodes_[newchild];
        if(nc.idx_bit >= 0 && nc.idx_bit < nd.idx_bit) {
            // newchild is internal with a shallower bit. Attach this node where old child was
            Node& mm = nodes_[newchild];
            if(mm.left == child)
                mm.left = n;
            else if(mm.right == child)
                mm.right = n;
            // Keep original child link in 'nd' unchanged (it still points to 'child')
            return newchild;
        }

        // Normal attach
        if(b) nd.right = newchild; else nd.left = newchild;
        return n;
    }

    int RemoveAt(int n, Hash h, bool& removed, int& removed_leaf_node, int& removed_rec) {
        if(n < 0)
            return -1;
        Node& nd = nodes_[n];
        if(nd.idx_bit < 0) {
            if(keys_[nd.rec] == h) {
                removed = true;
                removed_rec = nd.rec;
                removed_leaf_node = n;
                return -1; // delete this leaf
            }
            return n;
        }
        bool b = Bit(h, nd.idx_bit);
        int child = b ? nd.right : nd.left;
        int newchild = RemoveAt(child, h, removed, removed_leaf_node, removed_rec);
        if(b) nd.right = newchild; else nd.left = newchild;

        // Collapse if possible
        int l = nd.left, r = nd.right;
        if(l < 0 && r < 0) return -1;
        if(l < 0) return r;
        if(r < 0) return l;
        return n;
    }

    int OptimizeAt(int n) {
        if(n < 0)
            return -1;
        Node& nd = nodes_[n];
        if(nd.idx_bit < 0)
            return n;
        nd.left = OptimizeAt(nd.left);
        nd.right = OptimizeAt(nd.right);
        if(nd.left < 0) return nd.right;
        if(nd.right < 0) return nd.left;
        return n;
    }
};

// --- NEW: U++-style bitwise-not adapter for range-for of key+value pairs ---
template <class T, class GetHash, int Bits>
inline auto operator~(CritBitIndex<T, GetHash, Bits>& c) {
    return c.AsKV();
}
template <class T, class GetHash, int Bits>
inline auto operator~(const CritBitIndex<T, GetHash, Bits>& c) {
    return c.AsKV();
}
