#ifndef REDIS_ITERATORH
#define REDIS_ITERATORH
namespace RedisDataStructure
{
    template <typename T>
    class RedisBidirectionalIterator : public std::iterator<std::bidirectional_iterator_tag, T, ptrdiff_t, T *, T &>
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;
    public:
        RedisBidirectionalIterator(T *ptr) : m_ptr(ptr) {}
        RedisBidirectionalIterator(const RedisBidirectionalIterator<T> &other) { m_ptr = other.m_ptr; };
        RedisBidirectionalIterator<T> &operator=(const RedisBidirectionalIterator<T> &other) { m_ptr = other.m_ptr; };
        //RedisBidirectionalIterator(RedisBidirectionalIterator<T> &&other) = delete;
        //RedisBidirectionalIterator<T> &operator = (RedisBidirectionalIterator<T> &&other) = delete;
        ~RedisBidirectionalIterator() {}

        T &operator*() { return *m_ptr; }
        const T &operator*() const { return *m_ptr; }
        T *operator->() { return m_ptr; }
        T *getPtr() const { return m_ptr; }
        const T *getConstPtr() const { return m_ptr; }

        RedisBidirectionalIterator<T> &operator++()
        {
            m_ptr = m_ptr->next();
            return (*this);
        }
        RedisBidirectionalIterator<T> &operator--()
        {
            m_ptr = m_ptr->prev();
            return (*this);
        }
        RedisBidirectionalIterator<T> operator++(int)
        {
            auto temp(*this);
            m_ptr = m_ptr->next();
            return temp;
        }
        RedisBidirectionalIterator<T> operator--(int)
        {
            auto temp(*this);
            m_ptr = m_ptr->prev();
            return temp;
        }
        bool operator==(const RedisBidirectionalIterator<T> &rawIterator) const { return (m_ptr == rawIterator.getConstPtr()); }
        bool operator!=(const RedisBidirectionalIterator<T> &rawIterator) const { return (m_ptr != rawIterator.getConstPtr()); }

    protected:
        T *m_ptr;
    };

    template <typename T>
    class RedisBidirectionalReverseIterator : public RedisBidirectionalIterator<T>
    {
    public:
        RedisBidirectionalReverseIterator<T>(T *ptr) : RedisBidirectionalIterator(ptr) {}
        RedisBidirectionalReverseIterator<T> &operator++()
        {
            m_ptr = m_ptr->prev();
            return (*this);
        }
        RedisBidirectionalReverseIterator<T> &operator--()
        {
            m_ptr = m_ptr->next();
            return (*this);
        }
        RedisBidirectionalReverseIterator<T> operator++(int)
        {
            auto temp(*this);
            m_ptr = m_ptr->prev();
            return temp;
        }
        RedisBidirectionalReverseIterator<T> operator--(int)
        {
            auto temp(*this);
            m_ptr = m_ptr->next();
            return temp;
        }
    };
}; // namespace RedisDataStructure
#endif