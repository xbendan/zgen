module;

export module sdk:list;

import :traits;
import :types;
import :utility;
import :opt;

export namespace Meta {

template <typename T>
struct LinkedTrait {
    T* _next;
    T* _prev;

    [[gnu::always_inline]] constexpr bool operator==(
        LinkedTrait const& other) const {
        return this == &other;
    }
};

template <typename T>
concept ILinked = Extends<T, LinkedTrait<T>>;

namespace _ {
template <typename T>
struct Node {
    T        _value;
    Node<T>* _next;
    Node<T>* _prev;
};
} // namespace _

template <typename T>
class List {
    struct Node {
        Node *_next, *_prev;
        T     _value;
    };
    using E = Cond<ILinked<T>, T, Node>;

    E*    _head;
    E*    _tail;
    usize _count {};

    static constexpr E* wrap(T const& value) {
        if constexpr (ILinked<T>) {
            return &value;
        } else {
            return new E { value, nullptr, nullptr };
        }
    }

    static constexpr T const& unwrap(E const* elem) {
        if constexpr (ILinked<T>) {
            return *elem;
        } else {
            return elem->_value;
        }
    }

public:
    constexpr List() noexcept : _head(nullptr), _tail(nullptr), _count(0) { }

    template <CopyConstructible<T> U>
    List(List<U> const& other) : _head(nullptr),
                                 _tail(nullptr),
                                 _count(0) {
        for (E* curr = other.head(); curr != NONE; curr = curr->_next) {
            append(unwrap(curr));
        }
    }

    template <MoveConstructible<T> U>
    List(List<U>&& other) noexcept {
        clear();
        ::swap(_head, other._head);
        ::swap(_tail, other._tail);
        ::swap(_count, other._count);
    }

    ~List() { clear(); }

    E* head() const { return _head; }

    E* tail() const { return _tail; }

    // MARK: - Iterators

    // MARK: - Capacity

    [[gnu::always_inline]] bool isEmpty() const { return _count == 0; }

    [[gnu::always_inline]] usize size() const { return _count; }

    // MARK: - Modifiers

    [[gnu::always_inline]] void clear() {
        while (_head != nullptr) {
            E* n = _head->_next;
            delete _head;
            _head = n;
        }

        _tail  = nullptr;
        _count = 0;
    }

    [[gnu::always_inline]] void insert(usize index, T const& value) { }

    template <typename... Args>
    [[gnu::always_inline]] void emplace(Args&&... args) {
        E* n = new E { T(::forward<Args>(args)...) };

        if (_head == NONE)
            _head = _tail = n;
        else {
            n->_prev     = _tail;
            _tail->_next = n;
            _tail        = n;
        }
        n->_next = nullptr;

        _count++;
    }

    template <typename... Args>
    [[gnu::always_inline]] void emplaceAt(usize index, Args&&... args) { }

    [[gnu::always_inline]] void append(T const& value) {
        auto* n = wrap(value);

        if (_head == NONE)
            _head = _tail = n;
        else {
            n->_prev     = _tail;
            _tail->_next = n;
            _tail        = n;
        }
        n->_next = nullptr;

        _count++;
    }

    [[gnu::always_inline]] void remove(usize index) {
        if (index >= size()) {
            // Handle out-of-bounds index
        }

        E* curr = head();
        for (usize i = 0; i < index; ++i) {
            curr = next(curr);
        }

        if (curr->_prev != NONE)
            curr->_prev->_next = curr->_next;
        else
            _head = curr->_next;

        if (curr->_next != NONE)
            curr->_next->_prev = curr->_prev;
        else
            _tail = curr->_prev;

        delete curr;
        _count--;
    }

    [[gnu::always_inline]] void remove(T const& value) {
        auto* curr = head();

        while (curr != NONE) {
            if (unwrap(curr) != value) {
                curr = curr->_next;
                continue;
            }

            if (curr->_prev != NONE)
                curr->_prev->_next = curr->_next;
            else
                _head = curr->_next;

            if (curr->_next != NONE)
                curr->_next->_prev = curr->_prev;
            else
                _tail = curr->_prev;

            delete curr;
            _count--;
            return;
        }
    }

    [[gnu::always_inline]] Opt<T&> at(usize index) {
        if (index >= size()) {
            return NONE;
        }

        E* curr = head();
        for (usize i = 0; i < index; ++i) {
            curr = next(curr);
        }
        return unwrap(curr);
    }

    [[gnu::always_inline]] void operator+=(T const& value) { }

    [[gnu::always_inline]] void operator-=(T const& value) { }

    [[gnu::always_inline]] T& operator[](usize index) {
        if (index >= size()) {
            panic("List::operator[]: index out of bounds.");
        }

        E* curr = head();
        for (usize i = 0; i < index; ++i) {
            curr = curr->_next;
        }
        return unwrap(curr);
    }

    [[gnu::always_inline]] T const& operator[](usize index) const {
        if (index >= size()) {
            panic("List::operator[]: index out of bounds.");
        }

        E* curr = head();
        for (usize i = 0; i < index; ++i) {
            curr = curr->_next;
        }
        return unwrap(curr);
    }

    struct It {
        List const& list;
        E*          curr;

        constexpr It(List const& lst) : list(lst), curr(lst.head()) { }
    };
};

} // namespace Meta
