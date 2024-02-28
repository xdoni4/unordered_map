#include <iostream>
#include <vector>
#include <functional>

template<typename T, typename Allocator=std::allocator<T>>
class List {
public:
    struct BaseNode {
        BaseNode *next;
        BaseNode *prev;
        BaseNode(): next(this), prev(this) {}
        BaseNode(BaseNode *next, BaseNode *prev): next(next), prev(prev) {}
    };

    struct Node : public BaseNode {
        T key;

        Node(): BaseNode() {}
        Node(T val): BaseNode(), key(val) {}
        Node(BaseNode *next, BaseNode *prev): BaseNode(next, prev), key(T()) {}
        Node(BaseNode *next, BaseNode *prev, const T &val): BaseNode(next, prev), key(val) {}
        Node(BaseNode *next, BaseNode *prev, T &&val) : BaseNode(next, prev), key(std::move(val)) {}
    };

    BaseNode *fakeNode;
    BaseNode *head;
    size_t sz;

public:
    using AllocRebind =  typename Allocator::template rebind<Node>::other;
    using AllocTraits = std::allocator_traits<AllocRebind>;
    using AllocRebindBase =  typename Allocator::template rebind<BaseNode>::other;
    using AllocTraitsBase = std::allocator_traits<AllocRebindBase>;
    
    Allocator allocator;
    AllocRebind alloc = allocator;
    AllocRebindBase allocBase = allocator;

    int szof() {
        return sizeof(Node);
    }

    List() {
        fakeNode = AllocTraitsBase::allocate(allocBase, 1);
        AllocTraitsBase::construct(allocBase, fakeNode);
        head = fakeNode;
        sz = 0;
    }

    List(int count) {
        fakeNode = AllocTraitsBase::allocate(allocBase, 1);
        AllocTraitsBase::construct(allocBase, fakeNode); 
        head = fakeNode;
        Node *tmp;
        try {
            for (int i = 0; i < count; i++) {
                tmp = AllocTraits::allocate(alloc, 1);
                AllocTraits::construct(alloc, tmp, fakeNode, head);
                head->next = tmp;
                head = tmp;
            }
            fakeNode->prev = head;
            sz = count;
        } catch(...) {
            AllocTraits::deallocate(alloc, tmp, 1);
        }
    }

    List(int count, const T &val) {
        fakeNode = AllocTraitsBase::allocate(allocBase, 1);
        AllocTraitsBase::construct(allocBase, fakeNode); 
        head = fakeNode;
        for (int i = 0; i < count; i++) {
            Node *tmp = AllocTraits::allocate(alloc, 1);
            AllocTraits::construct(alloc, tmp, fakeNode, head, val);
            head->next = tmp;
            head = tmp;
        }
        fakeNode->prev = head;
        sz = count;
    }

    List(Allocator a) {
        allocator = a;
        alloc = allocator;
        allocBase = allocator;
        fakeNode = AllocTraitsBase::allocate(allocBase, 1);
        AllocTraitsBase::construct(allocBase, fakeNode);
        head = fakeNode;
        sz = 0;
    }

    List(int count, Allocator a) {
        allocator = a;
        alloc = allocator;
        allocBase = allocator;
        fakeNode = AllocTraitsBase::allocate(allocBase, 1);
        AllocTraitsBase::construct(allocBase, fakeNode); 
        head = fakeNode;
        for (int i = 0; i < count; i++) {
            Node *tmp = AllocTraits::allocate(alloc, 1);
            AllocTraits::construct(alloc, tmp, fakeNode, head);
            head->next = tmp;
            head = tmp;
        }
        fakeNode->prev = head;
        sz = count;
    }

    List(int count, const T &val, Allocator a) {
        allocator = a;
        alloc = allocator;
        allocBase = allocator;
        fakeNode = AllocTraitsBase::allocate(allocBase, 1);
        AllocTraitsBase::construct(allocBase, fakeNode); 
        head = fakeNode;
        for (int i = 0; i < count; i++) {
            Node *tmp = AllocTraits::allocate(alloc, 1);
            AllocTraits::construct(alloc, tmp, fakeNode, head, val);
            head->next = tmp;
            head = tmp;
        }
        fakeNode->prev = head;
        sz = count;
    }

    List(const List &other) {
        allocator = std::allocator_traits<Allocator>::select_on_container_copy_construction(other.allocator);
        alloc = allocator;
        allocBase = allocator;
        fakeNode = AllocTraitsBase::allocate(allocBase, 1);
        AllocTraitsBase::construct(allocBase, fakeNode);        
        head = fakeNode;
        Node *tmp;
        try {
            for (BaseNode* p = other.fakeNode->next; p != other.fakeNode; p = p->next) {
                tmp = AllocTraits::allocate(alloc, 1);
                AllocTraits::construct(alloc, tmp, fakeNode, head, reinterpret_cast<Node*>(p)->key);
                head->next = tmp;
                head = tmp;
            }
            fakeNode->prev = head;
            sz = other.sz;
        } catch(...) {
            AllocTraits::deallocate(alloc, tmp, 1);
        }
    }

    List(List &&other) {
        allocator = std::move(other.allocator);
        alloc = allocator;
        allocBase = allocator;
        fakeNode = other.fakeNode;
        head = other.head;
        sz = other.sz;
        AllocRebindBase alloc(allocator);
        other.fakeNode = AllocTraitsBase::allocate(alloc, 1);
        AllocTraitsBase::construct(alloc, other.fakeNode, BaseNode());
        other.head = other.fakeNode;
        other.sz = 0;
    }

    List& operator=(const List &other) {
        BaseNode *cur = fakeNode->next;
        AllocTraitsBase::destroy(allocBase, fakeNode);
        AllocTraitsBase::deallocate(allocBase, fakeNode, 1);
        for (size_t i = sz; i >= 1; i--) {
            BaseNode *tmp = cur->next;
            AllocTraits::destroy(alloc, reinterpret_cast<Node*>(cur));
            AllocTraits::deallocate(alloc, reinterpret_cast<Node*>(cur), 1);
            cur = tmp;
        }
        if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
            allocator = other.allocator;
            alloc = allocator;
            allocBase = allocator;
        }
        fakeNode = AllocTraitsBase::allocate(allocBase, 1);
        AllocTraitsBase::construct(allocBase, fakeNode);        
        head = fakeNode;
        Node *tmp;
        try {
            for (BaseNode* p = other.fakeNode->next; p != other.fakeNode; p = p->next) {
                tmp = AllocTraits::allocate(alloc, 1);
                AllocTraits::construct(alloc, tmp, fakeNode, head, reinterpret_cast<Node*>(p)->key);
                head->next = tmp;
                head = tmp;
            }
            fakeNode->prev = head; 
            sz = other.sz; 
        } catch(...) {
            AllocTraits::deallocate(alloc, tmp, 1);
        }
        return *this;
    }

    List& operator=(List &&other) {
        BaseNode *cur = fakeNode->next;
        for (size_t i = sz; i >= 1; i--) {
            BaseNode *tmp = cur->next;
            AllocTraits::destroy(alloc, reinterpret_cast<Node*>(cur));
            AllocTraits::deallocate(alloc, reinterpret_cast<Node*>(cur), 1);
            cur = tmp;
        }
        if (std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
            allocator = std::move(other.allocator);
            alloc = allocator;
            allocBase = allocator;
        }
        std::swap(fakeNode, other.fakeNode);
        head = other.head;
        sz = other.sz;
        other.fakeNode->next = other.fakeNode->prev = other.fakeNode;
        other.head = other.fakeNode;
        other.sz = 0;
        return *this;
    }

    ~List() {
        for (; head != fakeNode; ) {
            BaseNode *tmp = head->prev;
            AllocTraits::destroy(alloc, reinterpret_cast<Node*>(head));
            AllocTraits::deallocate(alloc, reinterpret_cast<Node*>(head), 1);
            head = tmp;
        }
        AllocTraitsBase::destroy(allocBase, fakeNode);
        AllocTraitsBase::deallocate(allocBase, fakeNode, 1);
    }

    Allocator get_allocator() const {
        return allocator; 
    }

    size_t size() const {
        return sz;
    }

    void push_back(const T &val) {
        Node *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, tmp, fakeNode, head, val);
        head->next = tmp; 
        head = tmp;
        fakeNode->prev = head;
        sz++;
    }

    void push_back(T &&val) {
        Node *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, tmp, fakeNode, head, std::move(val));
        head->next = tmp; 
        head = tmp;
        fakeNode->prev = head;
        sz++;
    }

    void push_front(const T& val) {
        bool f = (head == fakeNode ? 1 : 0);
        Node *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, tmp, fakeNode->next, fakeNode, val);
        tmp->next->prev = tmp;
        fakeNode->next = tmp;
        if (f) head = tmp;
        sz++;
    }

    void push_front(T&& val) {
        bool f = (head == fakeNode ? 1 : 0);
        Node *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, tmp, fakeNode->next, fakeNode, std::move(val));
        tmp->next->prev = tmp;
        fakeNode->next = tmp;
        if (f) head = tmp;
        sz++;
    }

    void pop_back() {
        BaseNode *tmp = head->prev;
        AllocTraits::destroy(alloc, reinterpret_cast<Node*>(head));
        AllocTraits::deallocate(alloc, reinterpret_cast<Node*>(head), 1);
        head = tmp;
        head->next = fakeNode;
        fakeNode->prev = head;
        sz--;
    }

    void pop_front() {
        bool f = (head->prev == fakeNode ? 1 : 0);
        BaseNode *tmp = fakeNode->next->next;
        AllocTraits::destroy(alloc, reinterpret_cast<Node*>(fakeNode->next));
        AllocTraits::deallocate(alloc, reinterpret_cast<Node*>(fakeNode->next), 1);
        fakeNode->next = tmp;
        fakeNode->next->prev = fakeNode;
        if (f) head = fakeNode;
        sz--;
    }

    template <typename PtrType, typename RefType>
    struct List_iterator {
        using Self = List_iterator; 
        using value_type = T;
        using pointer = PtrType;
        using reference = RefType;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using iterator = List_iterator<T*, T&>;
        using const_iterator = List_iterator<const T*, const T&>;

        BaseNode *ptr;
        
        List_iterator(BaseNode *p): ptr(p) {}
        List_iterator(Node *p) : ptr(reinterpret_cast<BaseNode*>(p)) {}
        List_iterator(const iterator& it): ptr(it.ptr) {}
        List_iterator(const const_iterator& it): ptr(it.ptr) {}


        reference operator*() const {
            return reinterpret_cast<Node*>(ptr)->key;
        }

        pointer operator->() const {
            return &reinterpret_cast<Node*>(ptr)->key;
        }

        Self& operator++() {
            ptr = ptr->next;
            return *this;
        }

        Self operator--() {
            ptr = ptr->prev;
            return *this;
        }

        Self operator++(int) {
            Self tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        Self operator--(int) {
            Self tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        bool operator==(const Self& other) {
            return ptr == other.ptr;
        }

        bool operator!=(const Self& other) {
            return !(*this == other);
        }
    };

    using iterator = List_iterator<T*, T&>;
    using const_iterator = List_iterator<const T*, const T&>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(fakeNode->next);
    }

    iterator end() {
        return iterator(fakeNode);
    }

    const_iterator begin() const {
        return const_iterator(fakeNode->next);
    }

    const_iterator end() const {
        return const_iterator(fakeNode);
    }

    const_iterator cbegin() const {
        return const_iterator(fakeNode->next);
    }

    const_iterator cend() const {
        return const_iterator(fakeNode);
    }

    reverse_iterator rbegin() {
        return std::reverse_iterator(this->end());
    }

    reverse_iterator rend() {
        return std::reverse_iterator(this->begin());
    }

    const_reverse_iterator rbegin() const {
        return std::reverse_iterator(this->end());
    }

    const_reverse_iterator rend() const {
        return std::reverse_iterator(this->begin());
    }

    const_reverse_iterator crbegin() const {
        return std::reverse_iterator(this->cend());
    }

    const_reverse_iterator crend() const {
        return std::reverse_iterator(this->cbegin());
    }

    void insert(iterator it, const T &val) {
        bool f = (it == this->end() ? 1 : 0);
        BaseNode *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, reinterpret_cast<Node*>(tmp), it.ptr, it.ptr->prev, val);
        it.ptr->prev->next = tmp;
        it.ptr->prev = tmp;
        if (f) head = tmp;
        sz++;
    }

    void insert(const_iterator it, const T &val) {
        bool f = (it == this->cend() ? 1 : 0);
        BaseNode *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, reinterpret_cast<Node*>(tmp), it.ptr, it.ptr->prev, val);
        it.ptr->prev->next = tmp;
        it.ptr->prev = tmp;
        if (f) head = tmp;
        sz++;
    }

    void insert(iterator it, T &&val) {
        bool f = (it == this->end() ? 1 : 0);
        BaseNode *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, reinterpret_cast<Node*>(tmp), it.ptr, it.ptr->prev, std::move(val));
        it.ptr->prev->next = tmp;
        it.ptr->prev = tmp;
        if (f) head = tmp;
        sz++;
    }

    void insert(const_iterator it, T &&val) {
        bool f = (it == this->end() ? 1 : 0);
        BaseNode *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, reinterpret_cast<Node*>(tmp), it.ptr, it.ptr->prev, std::move(val));
        it.ptr->prev->next = tmp;
        it.ptr->prev = tmp;
        if (f) head = tmp;
        sz++;
    }

    template<class... Args>
    void emplace(iterator it, Args&&... args) {
        bool f = (it == this->end() ? 1 : 0);
        BaseNode *tmp = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, reinterpret_cast<Node*>(tmp), it.ptr, it.ptr->prev, std::forward<Args>(args)...);
        it.ptr->prev->next = tmp;
        it.ptr->prev = tmp;
        if (f) head = tmp;
        sz++;
    }

    void erase(iterator it) {
        bool f = (it.ptr == (this->end().ptr->prev) ? 1 : 0);
        BaseNode *prev = it.ptr->prev, *next = it.ptr->next;
        AllocTraits::destroy(alloc, reinterpret_cast<Node*>(it.ptr)); 
        AllocTraits::deallocate(alloc, reinterpret_cast<Node*>(it.ptr), 1);
        prev->next = next;
        next->prev = prev;
        if (f) head = prev;
        sz--;
    }

    void erase(const_iterator it) {
        bool f = (it.ptr == (this->cend().ptr->prev) ? 1 : 0);
        BaseNode *prev = it.ptr->prev, *next = it.ptr->next;
        AllocTraits::destroy(alloc, reinterpret_cast<Node*>(it.ptr)); 
        AllocTraits::deallocate(alloc, reinterpret_cast<Node*>(it.ptr), 1);
        prev->next = next;
        next->prev = prev;
        if (f) head = prev;
        sz--;
    }
};


template <typename Key, typename Value, typename Hash=std::hash<Key>, typename Equal=std::equal_to<Key>, typename Alloc=std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {

public:
    using NodeType = std::pair<const Key, Value>; 
    struct Type {
        NodeType kv;
        size_t cached_hash; 

        Type(const NodeType &kv, size_t ch) : kv(kv), cached_hash(ch) {}
        Type(Key &&key, Value &&value, size_t ch) : kv(std::move(key), std::move(value)), cached_hash(ch) {}
        Type(const Type &t) = default;
        Type(Type &&t) : kv(std::move(const_cast<Key&&>(t.kv.first)), std::move(t.kv.second)), cached_hash(t.cached_hash) {}
        Type(NodeType &&kv, size_t ch) : kv(std::move(kv)), cached_hash(ch) {}
    }; 
    using TypeAlloc = typename Alloc::template rebind<Type>::other;
    using ListNodePtr = typename List<Type, TypeAlloc>::Node*;
    using ListNodePtrAlloc = typename Alloc::template rebind<ListNodePtr>::other;
    
    Alloc alloc;
    TypeAlloc type_alloc = alloc;
    ListNodePtrAlloc lnp_alloc = alloc;

    friend class List<Type, TypeAlloc>;

    UnorderedMap() : type_alloc(TypeAlloc()), lnp_alloc(ListNodePtrAlloc()), buckets(std::vector<ListNodePtr, ListNodePtrAlloc>(5)), nodes_list(List<Type, TypeAlloc>()), sz(0), m_load_factor(1.0) {}

    UnorderedMap(size_t bucket_count, const Alloc &a) {
        type_alloc = TypeAlloc(a);
        lnp_alloc = ListNodePtrAlloc(a);
        buckets = std::vector<ListNodePtr, ListNodePtrAlloc>(bucket_count, lnp_alloc);
        nodes_list = List<Type, TypeAlloc>(type_alloc);
        sz = 0;
        m_load_factor = 1.0;
    } 

    UnorderedMap(const Alloc& a)  {
        type_alloc = TypeAlloc(a);
        lnp_alloc = ListNodePtrAlloc(a);
        buckets = std::vector<ListNodePtr, ListNodePtrAlloc>(5, lnp_alloc);
        nodes_list = List<Type, TypeAlloc>(type_alloc);
        sz = 0;
        m_load_factor = 1;
    }

    UnorderedMap(const UnorderedMap &other) {
        sz = other.sz;
        m_load_factor = other.m_load_factor;
        type_alloc = std::allocator_traits<TypeAlloc>::select_on_container_copy_construction(other.type_alloc);
        lnp_alloc = std::allocator_traits<ListNodePtrAlloc>::select_on_container_copy_construction(other.lnp_alloc);
        nodes_list = other.nodes_list;
        buckets = std::vector<ListNodePtr, ListNodePtrAlloc>(other.buckets.size(), lnp_alloc);
        size_t prev = nodes_list.begin()->cached_hash+1;
        for (auto it = nodes_list.begin(); it != nodes_list.end(); ++it) {
            size_t cur = it->cached_hash;
            if (cur != prev) {
                buckets[cur % buckets.size()] = reinterpret_cast<ListNodePtr>(it.ptr); 
            }
            prev = cur;
        }
    }

    UnorderedMap(const UnorderedMap &other, const Alloc &a) { 
        sz = other.sz;
        m_load_factor = other.m_load_factor;
        type_alloc = TypeAlloc(a);
        lnp_alloc = ListNodePtrAlloc(a);
        nodes_list = other.nodes_list;
        buckets = std::vector<ListNodePtr, ListNodePtrAlloc>(other.buckets.size(), lnp_alloc);
        size_t prev = *(nodes_list.begin()).cached_hash+1;
        for (auto it = nodes_list.begin(); it != nodes_list.end(); ++it) {
            size_t cur = *(it).cached_hash;
            if (cur != prev) {
                buckets[cur % buckets.size()] = &(*it); 
            }
            prev = cur;
        }
    }

    UnorderedMap(UnorderedMap &&other) {
        sz = std::exchange(other.sz, 0);
        m_load_factor = std::exchange(other.m_load_factor, 1.0);
        type_alloc = std::move(other.type_alloc);
        lnp_alloc = std::move(other.lnp_alloc);
        buckets = std::move(other.buckets);
        nodes_list = std::move(other.nodes_list);
    }

    UnorderedMap(UnorderedMap &&other, const Alloc &a) {
        sz = std::exchange(other.sz, 0);
        m_load_factor = std::exchange(other.m_load_factor, 1.0);
        type_alloc = TypeAlloc(a);
        lnp_alloc = ListNodePtrAlloc(a);
        buckets = std::move(other.buckets);
        nodes_list = std::move(other.nodes_list);
    }

    UnorderedMap& operator=(const UnorderedMap &other) {
        sz = other.sz;
        m_load_factor = other.m_load_factor;
        if (std::allocator_traits<TypeAlloc>::propagate_on_container_copy_assignment::value) {
            type_alloc = other.type_alloc;
        }
        if (std::allocator_traits<ListNodePtrAlloc>::propagate_on_container_copy_assignment::value) {
            lnp_alloc = other.lnp_alloc;
        }
        nodes_list = other.nodes_list;
        buckets = std::vector<ListNodePtr, ListNodePtrAlloc>(other.buckets.size(), lnp_alloc);
        size_t prev = *(nodes_list.begin()).cached_hash+1;
        for (auto it = nodes_list.begin(); it != nodes_list.end(); ++it) {
            size_t cur = *(it).cached_hash;
            if (cur != prev) {
                buckets[cur % buckets.size()] = &(*it); 
            }
            prev = cur;
        } 
        return *this;
    }

    UnorderedMap& operator=(UnorderedMap &&other) {
        sz = std::exchange(other.sz, 0);
        m_load_factor = std::exchange(other.m_load_factor, 1.0);
        if (std::allocator_traits<TypeAlloc>::propagate_on_container_move_assignment::value) type_alloc = std::move(other.type_alloc);
        if (std::allocator_traits<ListNodePtrAlloc>::propagate_on_container_move_assignment::value) lnp_alloc = std::move(other.lnp_alloc);
        buckets = std::move(other.buckets);
        nodes_list = std::move(other.nodes_list);
        return *this;
    }

    ~UnorderedMap() = default;

    Value& operator[](const Key &key) {
        size_t h = Hash()(key);
        ListNodePtr b = buckets[h%buckets.size()];
        if (b == nullptr) {
            nodes_list.push_front(Type(NodeType(key, Value()), h));
            buckets[h%buckets.size()] = reinterpret_cast<ListNodePtr>(nodes_list.begin().ptr);
            sz++;
            return nodes_list.begin()->kv.second;
        }
        for (auto it = typename List<Type, TypeAlloc>::iterator(b); it != nodes_list.end(); ++it) {
            if (Equal()(it->kv.first, key)) {
                return it->kv.second;
            }
            if (h != it->cached_hash) {
                nodes_list.insert(it, Type(NodeType(key, Value()), h));
                sz++;
                return (--it)->kv.second;
            }
        }
        nodes_list.insert(nodes_list.end(), Type(std::move(key), Value()), h);
        sz++;
        return (--nodes_list.end())->kv.second;
    }

    Value& operator[](Key &&key) {
        size_t h = Hash()(key);
        ListNodePtr b = buckets[h%buckets.size()];
        if (b == nullptr) {
            nodes_list.push_front(Type(std::move(key), Value(), h));
            buckets[h%buckets.size()] = reinterpret_cast<ListNodePtr>(nodes_list.begin().ptr);
            sz++;
            return nodes_list.begin()->kv.second;
        }
        for (auto it = typename List<Type, TypeAlloc>::iterator(b); it != nodes_list.end(); ++it) {
            if (Equal()(it->kv.first, key)) {
                return it->kv.second;
            }
            if (h != it->cached_hash) {
                nodes_list.insert(it, Type(std::move(key), Value(), h));
                sz++;
                return (--it)->kv.second;
            }
        }
        nodes_list.insert(nodes_list.end(), Type(std::move(key), Value(), h));
        sz++;
        return (--nodes_list.end())->kv.second;
    }

    Value& at(const Key &key) {
        size_t h = Hash()(key);
        ListNodePtr b = buckets[h%buckets.size()];
        if (b == nullptr) {
            throw std::runtime_error("no key?"); 
        }
        for (auto it = typename List<Type, TypeAlloc>::iterator(b); it != nodes_list.end(); ++it) {
            if (Equal()(it->kv.first, key)) {
                return it->kv.second;
            }
            if (h % buckets.size() != it->cached_hash % buckets.size()) {
                throw std::runtime_error("no key?"); 
            }
        } 
        throw std::runtime_error("no key?");
    }

    const Value& at(const Key &key) const {
        size_t h = Hash()(key);
        ListNodePtr b = buckets[h%buckets.size()];
        if (b == nullptr) {
            throw std::runtime_error("no key?"); 
        }
        for (auto it = List<Type, TypeAlloc>::iterator(b); it != nodes_list.end(); ++it) {
            if (Equal()(it->kv.first, key)) {
                return it->kv.second;
            }
            if (h != it->cached_hash) {
                throw std::runtime_error("no key?"); 
            }
        } 
    }

    size_t size() const {
        return sz;
    }

    template <typename PtrType, typename RefType>
    struct UnorderedMap_iterator {
        using Self = UnorderedMap_iterator; 
        using value_type = NodeType;
        using pointer = PtrType;
        using reference = RefType;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using iterator = UnorderedMap_iterator<NodeType*, NodeType&>;
        using const_iterator = UnorderedMap_iterator<const NodeType*, const NodeType&>;

        typename List<Type, TypeAlloc>::iterator it;

        UnorderedMap_iterator(typename List<Type, TypeAlloc>::iterator iter): it(iter) {}
        UnorderedMap_iterator(const iterator& iter): it(iter.it) {}

        reference operator*() const {
            return it->kv;
        }

        pointer operator->() const {
            return &(it->kv);
        }

        Self& operator++() {
            ++it;
            return *this;
        }

        Self& operator--() {
            --it;
            return *this;
        }

        Self operator++(int) {
            Self tmp = *this;
            it++;
            return tmp;
        }

        Self operator--(int) {
            Self tmp = *this;
            it--;
            return tmp;
        }

        bool operator==(const Self& other) {
            return it == other.it;
        }

        bool operator!=(const Self& other) {
            return !(*this == other);
        }
    };

    using iterator = UnorderedMap_iterator<NodeType*, NodeType&>;
    using const_iterator = UnorderedMap_iterator<const NodeType*, const NodeType&>;

    iterator begin() {
        return iterator(nodes_list.begin());
    }

    iterator end() {
        return iterator(nodes_list.end());
    }

    const_iterator begin() const {
        return const_iterator(nodes_list.begin());
    }

    const_iterator end() const {
        return const_iterator(nodes_list.end());
    }

    const_iterator cbegin() const {
        return const_iterator(nodes_list.begin());
    }

    const_iterator cend() const {
        return const_iterator(nodes_list.end());
    }
    
    void rehash(size_t count) {
        std::vector<ListNodePtr, ListNodePtrAlloc> v_tmp(count, lnp_alloc);
        List<Type, TypeAlloc> l_tmp(type_alloc);
        
        for (auto it = nodes_list.begin(); it != nodes_list.end(); ++it) {
            size_t new_bucket = it->cached_hash % count;
            if (v_tmp[new_bucket] == nullptr) {
                l_tmp.emplace(l_tmp.begin(), Type(std::move(const_cast<Key&&>(it->kv.first)), std::move(it->kv.second), it->cached_hash));
                v_tmp[new_bucket] = reinterpret_cast<ListNodePtr>(l_tmp.begin().ptr);
            }
            else {
                auto iter = typename List<Type, TypeAlloc>::iterator(v_tmp[new_bucket]); 
                l_tmp.emplace(++iter, Type(std::move(const_cast<Key&&>(it->kv.first)), std::move(it->kv.second), it->cached_hash)); 
            }
        }

        nodes_list = std::move(l_tmp);
        buckets = std::move(v_tmp);
    }

    std::pair<iterator, bool> insert(const NodeType &kv) {
        if ((double)(sz+1)/buckets.size() >= m_load_factor) {
            this->rehash(2*buckets.size()); //TODO make gud
        }
        size_t h = Hash()(kv.first);
        if (buckets[h % buckets.size()] == nullptr) {
            nodes_list.emplace(nodes_list.begin(), Type(kv, h));
            buckets[h % buckets.size()] = reinterpret_cast<ListNodePtr>(nodes_list.begin().ptr);
            sz++;
            return std::make_pair(nodes_list.begin(), true);
        }
        for (auto it = typename List<Type, TypeAlloc>::iterator(buckets[h%buckets.size()]); it != nodes_list.end(); it++) {
            if (Equal()(it->kv.first, kv.first)) {
                return std::make_pair(it, false);
            }
            if (it->cached_hash % buckets.size() != h % buckets.size()) {
                nodes_list.emplace(it, Type(kv, h));
                sz++;
                return std::make_pair(--it, true);
            }
        }
        nodes_list.emplace(nodes_list.end(), Type(kv, h));
        sz++;
        return std::make_pair(--nodes_list.end(), true);
    }

    std::pair<iterator, bool> insert(NodeType &&kv) {
        if ((double)(sz+1)/buckets.size() >= m_load_factor) {
            this->rehash(2*buckets.size()); //TODO make gud
        }
        size_t h = Hash()(kv.first); 
        if (buckets[h % buckets.size()] == nullptr) {
            nodes_list.emplace(nodes_list.begin(), Type(std::move(const_cast<Key&&>(kv.first)), std::move(kv.second), h));
            buckets[h % buckets.size()] = reinterpret_cast<ListNodePtr>(nodes_list.begin().ptr);
            sz++;
            return std::make_pair(nodes_list.begin(), true);
        }
        for (auto it = typename List<Type, TypeAlloc>::iterator(buckets[h%buckets.size()]); it != nodes_list.end(); it++) {
            if (Equal()(it->kv.first, kv.first)) { 
                return std::make_pair(it, false);
            }
            if (it->cached_hash % buckets.size() != h % buckets.size()) {
                nodes_list.emplace(it, Type(std::move(const_cast<Key&&>(kv.first)), std::move(kv.second), h));
                sz++;
                return std::make_pair(--it, true);
            }
        }
        nodes_list.emplace(nodes_list.end(), Type(std::move(const_cast<Key&&>(kv.first)), std::move(kv.second), h));
        sz++;
        return std::make_pair(--nodes_list.end(), true);
    }

    template<class InputIt>
    void insert(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            this->insert(*it);
        }
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        NodeType *tmp = std::allocator_traits<Alloc>::allocate(alloc, 1);
        std::allocator_traits<Alloc>::construct(alloc, tmp, std::forward<Args>(args)...);
        return this->insert(std::move(*tmp));
    }

    void erase(iterator it) {
        nodes_list.erase(it.it);
        sz--;
    }

    void erase(const_iterator it) {
        nodes_list.erase(it.it);
        sz--;
    }

    void erase(iterator first, iterator last) {
        for (auto it = first; it != last;) {
            nodes_list.erase((it++).it);
            sz--;
        }
    }

    void erase(const_iterator first, const_iterator last) {
        for (auto it = first; it != last;) {
            nodes_list.erase((it++).it);
            sz--;
        }
    }

    iterator find(const Key &key) {
        size_t h = Hash()(key); 
        auto b = buckets[h%buckets.size()];
        if (b == nullptr) {
            return this->end();
        }
        for (auto it = typename List<Type, TypeAlloc>::iterator(b); it != nodes_list.end(); ++it) {
            if (Equal()(key, it->kv.first)) {
                return iterator(it);
            }
            if (h != it->cached_hash) {
                return this->end();
            }
        }
        return this->end();
    }

    void reserve(size_t count) {
        buckets.resize((size_t)(count / m_load_factor)+1);
    }

    size_t max_size() const {
        return size_t(-1);
    }

    double load_factor() const {
        return (double)(sz / buckets.size());
    }

    double max_load_factor() const {
        return m_load_factor;
    }

    void max_load_factor(double d) {
        m_load_factor = d;
    }

private: 
    std::vector<ListNodePtr, ListNodePtrAlloc> buckets;
    List<Type, TypeAlloc> nodes_list;
    size_t sz;
    double m_load_factor;
};
