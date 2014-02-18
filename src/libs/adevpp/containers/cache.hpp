#ifndef ADEVPP_CACHE_H
#define ADEVPP_CACHE_H

#include <functional>
#include <algorithm>
#include <map>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>


namespace Adevpp{

///
/// Thread Safe Cache container
///
template <class Key, class Value, class CompareT = std::less<Key> >
class Cache {
    typedef std::map<Key, boost::shared_ptr<Value> > Map;
    typedef boost::shared_ptr<Value> shrPtr_type;

public:
    Cache() : cmp(CompareT()), map(cmp){}

    shrPtr_type insert(const Key & key, Value* value){
        return insert(key, shrPtr_type(value));
    }

    ///
    /// \brief insert a new value into the cache, if an already existing one is mapped to this case, the old value is replaced
    /// \param key
    /// \param value
    /// \return
    ///
    shrPtr_type insert(const Key & key, const shrPtr_type & value){
        boost::lock_guard<boost::mutex> l(m);
        std::pair< typename Map::iterator, bool > stored = map.insert(std::pair<Key,shrPtr_type> (key, value) );
        if(stored.second)
            return (*stored.first).second;
        (*stored.first).second = value;
        return value;
    }

    ///
    /// \brief find a cached value, returned a NULL shared_pointer if not found
    /// \param key
    /// \return
    ///
    shrPtr_type find(const Key & key){
        boost::lock_guard<boost::mutex> l(m);
        typename Map::iterator it = map.find( key);
        if(it == map.end())
            return shrPtr_type();
        return it->second;
    }

    ///
    /// \brief getSize return the number of object sotred in the cache
    /// \return
    ///
    size_t getSize() const{
        boost::lock_guard<boost::mutex> l(m);
        return map.size();
    }

    ///
    /// \brief remove an object from this cache and return its reference
    /// \param key
    /// \return
    ///
    shrPtr_type take( const Key & key){
        boost::lock_guard<boost::mutex> l(m);
        typename Map::iterator it = map.find( key);
        if(it == map.end())
            return shrPtr_type();
        map.erase(key);
        return it->second;
    }

    ///
    /// \brief erase an object from the cache
    /// \param key
    /// \return
    ///
    bool erase( const Key & key){
        boost::lock_guard<boost::mutex> l(m);
        return ( map.erase(key) != 0);
    }

    void clear(){
        boost::lock_guard<boost::mutex> l(m);
        map.clear();
    }


protected:
    CompareT cmp;
    Map map;
    mutable boost::mutex m;
};




}

#endif // ADEVPP_H
