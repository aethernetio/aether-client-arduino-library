///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2016 John Wellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef ETL_UNORDERED_MAP_INCLUDED
#define ETL_UNORDERED_MAP_INCLUDED

#include "third_party/etl/include/etl/platform.h"
#include "third_party/etl/include/etl/algorithm.h"
#include "third_party/etl/include/etl/iterator.h"
#include "third_party/etl/include/etl/functional.h"
#include "third_party/etl/include/etl/utility.h"
#include "third_party/etl/include/etl/pool.h"
#include "third_party/etl/include/etl/array.h"
#include "third_party/etl/include/etl/intrusive_forward_list.h"
#include "third_party/etl/include/etl/hash.h"
#include "third_party/etl/include/etl/type_traits.h"
#include "third_party/etl/include/etl/nth_type.h"
#include "third_party/etl/include/etl/parameter_type.h"
#include "third_party/etl/include/etl/nullptr.h"
#include "third_party/etl/include/etl/vector.h"
#include "third_party/etl/include/etl/error_handler.h"
#include "third_party/etl/include/etl/exception.h"
#include "third_party/etl/include/etl/debug_count.h"
#include "third_party/etl/include/etl/iterator.h"
#include "third_party/etl/include/etl/placement_new.h"
#include "third_party/etl/include/etl/initializer_list.h"

#include "third_party/etl/include/etl/private/comparator_is_transparent.h"

#include <stddef.h>

//*****************************************************************************
///\defgroup unordered_map unordered_map
/// A unordered_map with the capacity defined at compile time.
///\ingroup containers
//*****************************************************************************

namespace etl
{
  //***************************************************************************
  /// Exception for the unordered_map.
  ///\ingroup unordered_map
  //***************************************************************************
  class unordered_map_exception : public etl::exception
  {
  public:

    unordered_map_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : etl::exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Full exception for the unordered_map.
  ///\ingroup unordered_map
  //***************************************************************************
  class unordered_map_full : public etl::unordered_map_exception
  {
  public:

    unordered_map_full(string_type file_name_, numeric_type line_number_)
      : etl::unordered_map_exception(ETL_ERROR_TEXT("unordered_map:full", ETL_UNORDERED_MAP_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Out of range exception for the unordered_map.
  ///\ingroup unordered_map
  //***************************************************************************
  class unordered_map_out_of_range : public etl::unordered_map_exception
  {
  public:

    unordered_map_out_of_range(string_type file_name_, numeric_type line_number_)
      : etl::unordered_map_exception(ETL_ERROR_TEXT("unordered_map:range", ETL_UNORDERED_MAP_FILE_ID"B"), file_name_, line_number_)
    {}
  };

  //***************************************************************************
  /// Iterator exception for the unordered_map.
  ///\ingroup unordered_map
  //***************************************************************************
  class unordered_map_iterator : public etl::unordered_map_exception
  {
  public:

    unordered_map_iterator(string_type file_name_, numeric_type line_number_)
      : etl::unordered_map_exception(ETL_ERROR_TEXT("unordered_map:iterator", ETL_UNORDERED_MAP_FILE_ID"C"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The base class for specifically sized unordered_map.
  /// Can be used as a reference type for all unordered_map containing a specific type.
  ///\ingroup unordered_map
  //***************************************************************************
  template <typename TKey, typename T, typename THash = etl::hash<TKey>, typename TKeyEqual = etl::equal_to<TKey> >
  class iunordered_map
  {
  public:

    typedef ETL_OR_STD::pair<const TKey, T> value_type;

    typedef TKey              key_type;
    typedef T                 mapped_type;
    typedef THash             hasher;
    typedef TKeyEqual         key_equal;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
#if ETL_USING_CPP11
    typedef value_type&&      rvalue_reference;
#endif
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef size_t            size_type;

    /// Defines the parameter types
    typedef const key_type&    const_key_reference;
#if ETL_USING_CPP11
    typedef key_type&&         rvalue_key_reference;
#endif
    typedef mapped_type&       mapped_reference;
    typedef const mapped_type& const_mapped_reference;

    typedef etl::forward_link<0> link_t; // Default link.

    // The nodes that store the elements.
    // The nodes that store the elements.
    struct node_t : public link_t
    {
      node_t(const_reference key_value_pair_)
        : key_value_pair(key_value_pair_)
      {
      }

      value_type key_value_pair;
    };

    friend bool operator ==(const node_t& lhs, const node_t& rhs)
    {
      return (lhs.key_value_pair.first  == rhs.key_value_pair.first) &&
             (lhs.key_value_pair.second == rhs.key_value_pair.second);
    }

    friend bool operator !=(const node_t& lhs, const node_t& rhs)
    {
      return !(lhs == rhs);
    }

  protected:

    typedef etl::intrusive_forward_list<node_t, link_t> bucket_t;
    typedef etl::ipool pool_t;

  public:

    // Local iterators iterate over one bucket.
    typedef typename bucket_t::iterator       local_iterator;
    typedef typename bucket_t::const_iterator const_local_iterator;

    //*********************************************************************
    class iterator : public etl::iterator<ETL_OR_STD::forward_iterator_tag, T>
    {
    public:

      typedef typename etl::iterator<ETL_OR_STD::forward_iterator_tag, T>::value_type value_type;
      typedef typename iunordered_map::key_type        key_type;
      typedef typename iunordered_map::mapped_type     mapped_type;
      typedef typename iunordered_map::hasher          hasher;
      typedef typename iunordered_map::key_equal       key_equal;
      typedef typename iunordered_map::reference       reference;
      typedef typename iunordered_map::const_reference const_reference;
      typedef typename iunordered_map::pointer         pointer;
      typedef typename iunordered_map::const_pointer   const_pointer;
      typedef typename iunordered_map::size_type       size_type;

      friend class iunordered_map;
      friend class const_iterator;

      //*********************************
      iterator()
      {
      }

      //*********************************
      iterator(const iterator& other)
        : pbuckets_end(other.pbuckets_end)
        , pbucket(other.pbucket)
        , inode(other.inode)
      {
      }

      //*********************************
      iterator& operator ++()
      {
        ++inode;

        // The end of this node list?
        if (inode == pbucket->end())
        {
          // Search for the next non-empty bucket.
          ++pbucket;
          while ((pbucket != pbuckets_end) && (pbucket->empty()))
          {
            ++pbucket;
          }

          // If not past the end, get the first node in the bucket.
          if (pbucket != pbuckets_end)
          {
            inode = pbucket->begin();
          }
        }

        return *this;
      }

      //*********************************
      iterator operator ++(int)
      {
        iterator temp(*this);
        operator++();
        return temp;
      }

      //*********************************
      iterator& operator =(const iterator& other)
      {
        pbuckets_end = other.pbuckets_end;
        pbucket = other.pbucket;
        inode = other.inode;
        return *this;
      }

      //*********************************
      reference operator *() const
      {
        return inode->key_value_pair;
      }

      //*********************************
      pointer operator &() const
      {
        return &(inode->key_value_pair);
      }

      //*********************************
      pointer operator ->() const
      {
        return &(inode->key_value_pair);
      }

      //*********************************
      friend bool operator == (const iterator& lhs, const iterator& rhs)
      {
        return lhs.compare(rhs);
      }

      //*********************************
      friend bool operator != (const iterator& lhs, const iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      //*********************************
      iterator(bucket_t* pbuckets_end_, bucket_t* pbucket_, local_iterator inode_)
        : pbuckets_end(pbuckets_end_)
        , pbucket(pbucket_)
        , inode(inode_)
      {
      }

      //*********************************
      bool compare(const iterator& rhs) const
      {
        return rhs.inode == inode;
      }

      //*********************************
      bucket_t& get_bucket()
      {
        return *pbucket;
      }

      //*********************************
      bucket_t* get_bucket_list_iterator()
      {
        return pbucket;
      }

      //*********************************
      local_iterator get_local_iterator()
      {
        return inode;
      }

      bucket_t* pbuckets_end;
      bucket_t* pbucket;
      local_iterator inode;
    };

    //*********************************************************************
    class const_iterator : public etl::iterator<ETL_OR_STD::forward_iterator_tag, const T>
    {
    public:

      typedef typename etl::iterator<ETL_OR_STD::forward_iterator_tag, const T>::value_type value_type;
      typedef typename iunordered_map::key_type        key_type;
      typedef typename iunordered_map::mapped_type     mapped_type;
      typedef typename iunordered_map::hasher          hasher;
      typedef typename iunordered_map::key_equal       key_equal;
      typedef typename iunordered_map::reference       reference;
      typedef typename iunordered_map::const_reference const_reference;
      typedef typename iunordered_map::pointer         pointer;
      typedef typename iunordered_map::const_pointer   const_pointer;
      typedef typename iunordered_map::size_type       size_type;

      friend class iunordered_map;
      friend class iterator;

      //*********************************
      const_iterator()
      {
      }

      //*********************************
      const_iterator(const typename iunordered_map::iterator& other)
        : pbuckets_end(other.pbuckets_end)
        , pbucket(other.pbucket)
        , inode(other.inode)
      {
      }

      //*********************************
      const_iterator(const const_iterator& other)
        : pbuckets_end(other.pbuckets_end)
        , pbucket(other.pbucket)
        , inode(other.inode)
      {
      }

      //*********************************
      const_iterator& operator ++()
      {
        ++inode;

        // The end of this node list?
        if (inode == pbucket->end())
        {
          // Search for the next non-empty bucket.
          ++pbucket;
          while ((pbucket != pbuckets_end) && (pbucket->empty()))
          {
            ++pbucket;
          }

          // If not past the end, get the first node in the bucket.
          if (pbucket != pbuckets_end)
          {
            inode = pbucket->begin();
          }
        }

        return *this;
      }

      //*********************************
      const_iterator operator ++(int)
      {
        const_iterator temp(*this);
        operator++();
        return temp;
      }

      //*********************************
      const_iterator& operator =(const const_iterator& other)
      {
        pbuckets_end = other.pbuckets_end;
        pbucket = other.pbucket;
        inode = other.inode;
        return *this;
      }

      //*********************************
      const_reference operator *() const
      {
        return inode->key_value_pair;
      }

      //*********************************
      const_pointer operator &() const
      {
        return &(inode->key_value_pair);
      }

      //*********************************
      const_pointer operator ->() const
      {
        return &(inode->key_value_pair);
      }

      //*********************************
      friend bool operator == (const const_iterator& lhs, const const_iterator& rhs)
      {
        return lhs.compare(rhs);
      }

      //*********************************
      friend bool operator != (const const_iterator& lhs, const const_iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      //*********************************
      const_iterator(bucket_t* pbuckets_end_, bucket_t* pbucket_, local_iterator inode_)
        : pbuckets_end(pbuckets_end_)
        , pbucket(pbucket_)
        , inode(inode_)
      {
      }

      //*********************************
      bool compare(const const_iterator& rhs) const
      {
        return rhs.inode == inode;
      }

      //*********************************
      bucket_t& get_bucket()
      {
        return *pbucket;
      }

      //*********************************
      bucket_t* get_bucket_list_iterator()
      {
        return pbucket;
      }

      //*********************************
      local_iterator get_local_iterator()
      {
        return inode;
      }

      bucket_t* pbuckets_end;
      bucket_t* pbucket;
      local_iterator inode;
    };

    typedef typename etl::iterator_traits<iterator>::difference_type difference_type;

    //*********************************************************************
    /// Returns an iterator to the beginning of the unordered_map.
    ///\return An iterator to the beginning of the unordered_map.
    //*********************************************************************
    iterator begin()
    {
      return iterator((pbuckets + number_of_buckets), first, first->begin());
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the unordered_map.
    ///\return A const iterator to the beginning of the unordered_map.
    //*********************************************************************
    const_iterator begin() const
    {
      return const_iterator((pbuckets + number_of_buckets), first, first->begin());
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the unordered_map.
    ///\return A const iterator to the beginning of the unordered_map.
    //*********************************************************************
    const_iterator cbegin() const
    {
      return const_iterator((pbuckets + number_of_buckets), first, first->begin());
    }

    //*********************************************************************
    /// Returns an iterator to the beginning of the unordered_map bucket.
    ///\return An iterator to the beginning of the unordered_map bucket.
    //*********************************************************************
    local_iterator begin(size_t i)
    {
      return pbuckets[i].begin();
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the unordered_map bucket.
    ///\return A const iterator to the beginning of the unordered_map bucket.
    //*********************************************************************
    const_local_iterator begin(size_t i) const
    {
      return pbuckets[i].cbegin();
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the unordered_map bucket.
    ///\return A const iterator to the beginning of the unordered_map bucket.
    //*********************************************************************
    const_local_iterator cbegin(size_t i) const
    {
      return pbuckets[i].cbegin();
    }

    //*********************************************************************
    /// Returns an iterator to the end of the unordered_map.
    ///\return An iterator to the end of the unordered_map.
    //*********************************************************************
    iterator end()
    {
      return iterator((pbuckets + number_of_buckets), last, last->end());
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the unordered_map.
    ///\return A const iterator to the end of the unordered_map.
    //*********************************************************************
    const_iterator end() const
    {
      return const_iterator((pbuckets + number_of_buckets), last, last->end());
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the unordered_map.
    ///\return A const iterator to the end of the unordered_map.
    //*********************************************************************
    const_iterator cend() const
    {
      return const_iterator((pbuckets + number_of_buckets), last, last->end());
    }

    //*********************************************************************
    /// Returns an iterator to the end of the unordered_map bucket.
    ///\return An iterator to the end of the unordered_map bucket.
    //*********************************************************************
    local_iterator end(size_t i)
    {
      return pbuckets[i].end();
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the unordered_map bucket.
    ///\return A const iterator to the end of the unordered_map bucket.
    //*********************************************************************
    const_local_iterator end(size_t i) const
    {
      return pbuckets[i].cend();
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the unordered_map bucket.
    ///\return A const iterator to the end of the unordered_map bucket.
    //*********************************************************************
    const_local_iterator cend(size_t i) const
    {
      return pbuckets[i].cend();
    }

    //*********************************************************************
    /// Returns the bucket index for the key.
    ///\return The bucket index for the key.
    //*********************************************************************
    size_type get_bucket_index(const_key_reference key) const
    {
      return key_hash_function(key) % number_of_buckets;
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Returns the bucket index for the key.
    ///\return The bucket index for the key.
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    size_type get_bucket_index(const K& key) const
    {
      return key_hash_function(key) % number_of_buckets;
    }
#endif

    //*********************************************************************
    /// Returns the size of the bucket key.
    ///\return The bucket size of the bucket key.
    //*********************************************************************
    size_type bucket_size(const_key_reference key) const
    {
      size_t index = bucket(key);

      return etl::distance(pbuckets[index].begin(), pbuckets[index].end());
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Returns the size of the bucket key.
    ///\return The bucket size of the bucket key.
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    size_type bucket_size(const K& key) const
    {
      size_t index = bucket(key);

      return etl::distance(pbuckets[index].begin(), pbuckets[index].end());
    }
#endif

    //*********************************************************************
    /// Returns the maximum number of the buckets the container can hold.
    ///\return The maximum number of the buckets the container can hold.
    //*********************************************************************
    size_type max_bucket_count() const
    {
      return number_of_buckets;
    }

    //*********************************************************************
    /// Returns the number of the buckets the container holds.
    ///\return The number of the buckets the container holds.
    //*********************************************************************
    size_type bucket_count() const
    {
      return number_of_buckets;
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Returns a reference to the value at index 'key'
    ///\param key The key.
    ///\return A reference to the value at index 'key'
    //*********************************************************************
    mapped_reference operator [](rvalue_key_reference key)
    {
      // Find the bucket.
      bucket_t* pbucket = pbuckets + get_bucket_index(key);

      // Find the first node in the bucket.
      local_iterator inode = pbucket->begin();

      // Walk the list looking for the right one.
      while (inode != pbucket->end())
      {
        // Equal keys?
        if (key_equal_function(key, inode->key_value_pair.first))
        {
          // Found a match.
          return inode->key_value_pair.second;
        }
        else
        {
          ++inode;
        }
      }

      // Doesn't exist, so add a new one.
      // Get a new node.
      node_t* node = allocate_data_node();
      node->clear();
      ::new ((void*)etl::addressof(node->key_value_pair.first))  key_type(etl::move(key));
      ::new ((void*)etl::addressof(node->key_value_pair.second)) mapped_type();
      ETL_INCREMENT_DEBUG_COUNT;

      pbucket->insert_after(pbucket->before_begin(), *node);

      adjust_first_last_markers_after_insert(pbucket);

      return pbucket->begin()->key_value_pair.second;
    }
#endif

    //*********************************************************************
    /// Returns a reference to the value at index 'key'
    ///\param key The key.
    ///\return A reference to the value at index 'key'
    //*********************************************************************
    mapped_reference operator [](const_key_reference key)
    {
      // Find the bucket.
      bucket_t* pbucket = pbuckets + get_bucket_index(key);

      // Find the first node in the bucket.
      local_iterator inode = pbucket->begin();

      // Walk the list looking for the right one.
      while (inode != pbucket->end())
      {
        // Equal keys?
        if (key_equal_function(key, inode->key_value_pair.first))
        {
          // Found a match.
          return inode->key_value_pair.second;
        }
        else
        {
          ++inode;
        }
      }

      // Doesn't exist, so add a new one.
      // Get a new node.
      node_t* node = allocate_data_node();
      node->clear();
      ::new ((void*)etl::addressof(node->key_value_pair.first))  key_type(key);
      ::new ((void*)etl::addressof(node->key_value_pair.second)) mapped_type();
      ETL_INCREMENT_DEBUG_COUNT;

        pbucket->insert_after(pbucket->before_begin(), *node);

      adjust_first_last_markers_after_insert(pbucket);

      return pbucket->begin()->key_value_pair.second;
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Returns a reference to the value at index 'key'
    ///\param key The key.
    ///\return A reference to the value at index 'key'
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    mapped_reference operator [](const K& key)
    {
      // Find the bucket.
      bucket_t* pbucket = pbuckets + get_bucket_index(key);

      // Find the first node in the bucket.
      local_iterator inode = pbucket->begin();

      // Walk the list looking for the right one.
      while (inode != pbucket->end())
      {
        // Equal keys?
        if (key_equal_function(key, inode->key_value_pair.first))
        {
          // Found a match.
          return inode->key_value_pair.second;
        }
        else
        {
          ++inode;
        }
      }

      // Doesn't exist, so add a new one.
      // Get a new node.
      node_t* node = allocate_data_node();
      node->clear();
      ::new ((void*)etl::addressof(node->key_value_pair.first))  key_type(key);
      ::new ((void*)etl::addressof(node->key_value_pair.second)) mapped_type();
      ETL_INCREMENT_DEBUG_COUNT;

      pbucket->insert_after(pbucket->before_begin(), *node);

      adjust_first_last_markers_after_insert(pbucket);

      return pbucket->begin()->key_value_pair.second;
    }
#endif

    //*********************************************************************
    /// Returns a reference to the value at index 'key'
    /// If asserts or exceptions are enabled, emits an etl::unordered_map_out_of_range if the key is not in the range.
    ///\param key The key.
    ///\return A reference to the value at index 'key'
    //*********************************************************************
    mapped_reference at(const_key_reference key)
    {
      // Find the bucket.
      bucket_t* pbucket = pbuckets + get_bucket_index(key);

      // Find the first node in the bucket.
      local_iterator inode = pbucket->begin();

      // Walk the list looking for the right one.
      while (inode != pbucket->end())
      {
        // Equal keys?
        if (key_equal_function(key, inode->key_value_pair.first))
        {
          // Found a match.
          return inode->key_value_pair.second;
        }
        else
        {
          ++inode;
        }
      }

      // Doesn't exist.
      ETL_ASSERT(false, ETL_ERROR(unordered_map_out_of_range));

      return begin()->second;
    }

    //*********************************************************************
    /// Returns a const reference to the value at index 'key'
    /// If asserts or exceptions are enabled, emits an etl::unordered_map_out_of_range if the key is not in the range.
    ///\param key The key.
    ///\return A const reference to the value at index 'key'
    //*********************************************************************
    const_mapped_reference at(const_key_reference key) const
    {
      // Find the bucket.
      bucket_t* pbucket = pbuckets + get_bucket_index(key);

      // Find the first node in the bucket.
      local_iterator inode = pbucket->begin();

      // Walk the list looking for the right one.
      while (inode != pbucket->end())
      {
        // Equal keys?
        if (key_equal_function(key, inode->key_value_pair.first))
        {
          // Found a match.
          return inode->key_value_pair.second;
        }
        else
        {
          ++inode;
        }
      }

      // Doesn't exist.
      ETL_ASSERT(false, ETL_ERROR(unordered_map_out_of_range));

      return begin()->second;
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Returns a reference to the value at index 'key'
    /// If asserts or exceptions are enabled, emits an etl::unordered_map_out_of_range if the key is not in the range.
    ///\param key The key.
    ///\return A reference to the value at index 'key'
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    mapped_reference at(const K& key)
    {
      // Find the bucket.
      bucket_t* pbucket = pbuckets + get_bucket_index(key);

      // Find the first node in the bucket.
      local_iterator inode = pbucket->begin();

      // Walk the list looking for the right one.
      while (inode != pbucket->end())
      {
        // Equal keys?
        if (key_equal_function(key, inode->key_value_pair.first))
        {
          // Found a match.
          return inode->key_value_pair.second;
        }
        else
        {
          ++inode;
        }
      }

      // Doesn't exist.
      ETL_ASSERT(false, ETL_ERROR(unordered_map_out_of_range));

      return begin()->second;
    }
#endif

#if ETL_USING_CPP11
    //*********************************************************************
    /// Returns a const reference to the value at index 'key'
    /// If asserts or exceptions are enabled, emits an etl::unordered_map_out_of_range if the key is not in the range.
    ///\param key The key.
    ///\return A const reference to the value at index 'key'
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    const_mapped_reference at(const K& key) const
    {
      // Find the bucket.
      bucket_t* pbucket = pbuckets + get_bucket_index(key);

      // Find the first node in the bucket.
      local_iterator inode = pbucket->begin();

      // Walk the list looking for the right one.
      while (inode != pbucket->end())
      {
        // Equal keys?
        if (key_equal_function(key, inode->key_value_pair.first))
        {
          // Found a match.
          return inode->key_value_pair.second;
        }
        else
        {
          ++inode;
        }
      }

      // Doesn't exist.
      ETL_ASSERT(false, ETL_ERROR(unordered_map_out_of_range));

      return begin()->second;
    }
#endif

    //*********************************************************************
    /// Assigns values to the unordered_map.
    /// If asserts or exceptions are enabled, emits unordered_map_full if the unordered_map does not have enough free space.
    /// If asserts or exceptions are enabled, emits unordered_map_iterator if the iterators are reversed.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*********************************************************************
    template <typename TIterator>
    void assign(TIterator first_, TIterator last_)
    {
#if ETL_IS_DEBUG_BUILD
      difference_type d = etl::distance(first_, last_);
      ETL_ASSERT(d >= 0, ETL_ERROR(unordered_map_iterator));
      ETL_ASSERT(size_t(d) <= max_size(), ETL_ERROR(unordered_map_full));
#endif

      clear();

      while (first_ != last_)
      {
        insert(*first_);
        ++first_;
      }
    }

    //*********************************************************************
    /// Inserts a value to the unordered_map.
    /// If asserts or exceptions are enabled, emits unordered_map_full if the unordered_map is already full.
    ///\param value The value to insert.
    //*********************************************************************
    ETL_OR_STD::pair<iterator, bool> insert(const_reference key_value_pair)
    {
      ETL_OR_STD::pair<iterator, bool> result(end(), false);

      ETL_ASSERT(!full(), ETL_ERROR(unordered_map_full));

      const key_type&    key = key_value_pair.first;

      // Get the hash index.
      size_t index = get_bucket_index(key);

      // Get the bucket & bucket iterator.
      bucket_t* pbucket = pbuckets + index;
      bucket_t& bucket = *pbucket;

      // The first one in the bucket?
      if (bucket.empty())
      {
        // Get a new node.
        node_t* node = allocate_data_node();
        node->clear();
        ::new ((void*)etl::addressof(node->key_value_pair)) value_type(key_value_pair);        
        ETL_INCREMENT_DEBUG_COUNT;

        // Just add the pointer to the bucket;
        bucket.insert_after(bucket.before_begin(), *node);

        adjust_first_last_markers_after_insert(pbucket);

        result.first = iterator((pbuckets + number_of_buckets), pbucket, pbucket->begin());
        result.second = true;
      }
      else
      {
        // Step though the bucket looking for a place to insert.
        local_iterator inode_previous = bucket.before_begin();
        local_iterator inode = bucket.begin();

        while (inode != bucket.end())
        {
          // Do we already have this key?
          if (key_equal_function(inode->key_value_pair.first, key))
          {
            break;
          }

          ++inode_previous;
          ++inode;
        }

        // Not already there?
        if (inode == bucket.end())
        {
          // Get a new node.
          node_t* node = allocate_data_node();
          node->clear();
          ::new ((void*)etl::addressof(node->key_value_pair)) value_type(key_value_pair);
          ETL_INCREMENT_DEBUG_COUNT;

          // Add the node to the end of the bucket;
          bucket.insert_after(inode_previous, *node);
          adjust_first_last_markers_after_insert(&bucket);
          ++inode_previous;

          result.first = iterator((pbuckets + number_of_buckets), pbucket, inode_previous);
          result.second = true;
        }
      }

      return result;
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Inserts a value to the unordered_map.
    /// If asserts or exceptions are enabled, emits unordered_map_full if the unordered_map is already full.
    ///\param value The value to insert.
    //*********************************************************************
    ETL_OR_STD::pair<iterator, bool> insert(rvalue_reference key_value_pair)
    {
      ETL_OR_STD::pair<iterator, bool> result(end(), false);

      ETL_ASSERT(!full(), ETL_ERROR(unordered_map_full));

      const key_type&    key = key_value_pair.first;

      // Get the hash index.
      size_t index = get_bucket_index(key);

      // Get the bucket & bucket iterator.
      bucket_t* pbucket = pbuckets + index;
      bucket_t& bucket = *pbucket;

      // The first one in the bucket?
      if (bucket.empty())
      {
        // Get a new node.
        node_t* node = allocate_data_node();
        node->clear();
        ::new ((void*)etl::addressof(node->key_value_pair)) value_type(etl::move(key_value_pair));
        ETL_INCREMENT_DEBUG_COUNT;

        // Just add the pointer to the bucket;
        bucket.insert_after(bucket.before_begin(), *node);

        adjust_first_last_markers_after_insert(pbucket);

        result.first = iterator((pbuckets + number_of_buckets), pbucket, pbucket->begin());
        result.second = true;
      }
      else
      {
        // Step though the bucket looking for a place to insert.
        local_iterator inode_previous = bucket.before_begin();
        local_iterator inode = bucket.begin();

        while (inode != bucket.end())
        {
          // Do we already have this key?
          if (key_equal_function(inode->key_value_pair.first, key))
          {
            break;
          }

          ++inode_previous;
          ++inode;
        }

        // Not already there?
        if (inode == bucket.end())
        {
          // Get a new node.
          node_t* node = allocate_data_node();
          node->clear();
          ::new ((void*)etl::addressof(node->key_value_pair)) value_type(etl::move(key_value_pair));
          ETL_INCREMENT_DEBUG_COUNT;

          // Add the node to the end of the bucket;
          bucket.insert_after(inode_previous, *node);
          adjust_first_last_markers_after_insert(&bucket);
          ++inode_previous;

          result.first = iterator((pbuckets + number_of_buckets), pbucket, inode_previous);
          result.second = true;
        }
      }

      return result;
    }
#endif

    //*********************************************************************
    /// Inserts a value to the unordered_map.
    /// If asserts or exceptions are enabled, emits unordered_map_full if the unordered_map is already full.
    ///\param position The position to insert at.
    ///\param value    The value to insert.
    //*********************************************************************
    iterator insert(const_iterator, const_reference key_value_pair)
    {
      return insert(key_value_pair).first;
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Inserts a value to the unordered_map.
    /// If asserts or exceptions are enabled, emits unordered_map_full if the unordered_map is already full.
    ///\param position The position to insert at.
    ///\param value    The value to insert.
    //*********************************************************************
    iterator insert(const_iterator, rvalue_reference key_value_pair)
    {
      return insert(etl::move(key_value_pair)).first;
    }
#endif

    //*********************************************************************
    /// Inserts a range of values to the unordered_map.
    /// If asserts or exceptions are enabled, emits unordered_map_full if the unordered_map does not have enough free space.
    ///\param position The position to insert at.
    ///\param first    The first element to add.
    ///\param last     The last + 1 element to add.
    //*********************************************************************
    template <class TIterator>
    void insert(TIterator first_, TIterator last_)
    {
      while (first_ != last_)
      {
        insert(*first_);
        ++first_;
      }
    }

    //*********************************************************************
    /// Erases an element.
    ///\param key The key to erase.
    ///\return The number of elements erased. 0 or 1.
    //*********************************************************************
    size_t erase(const_key_reference key)
    {
      size_t n = 0UL;
      size_t index = get_bucket_index(key);

      bucket_t& bucket = pbuckets[index];

      local_iterator iprevious = bucket.before_begin();
      local_iterator icurrent = bucket.begin();

      // Search for the key, if we have it.
      while ((icurrent != bucket.end()) && (!key_equal_function(icurrent->key_value_pair.first, key)))
      {
        ++iprevious;
        ++icurrent;
      }

      // Did we find it?
      if (icurrent != bucket.end())
      {
        delete_data_node(iprevious, icurrent, bucket);
        n = 1;
      }

      return n;
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Erases an element.
    ///\param key The key to erase.
    ///\return The number of elements erased. 0 or 1.
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    size_t erase(const K& key)
    {
      size_t n = 0UL;
      size_t index = get_bucket_index(key);

      bucket_t& bucket = pbuckets[index];

      local_iterator iprevious = bucket.before_begin();
      local_iterator icurrent = bucket.begin();

      // Search for the key, if we have it.
      while ((icurrent != bucket.end()) && (!key_equal_function(icurrent->key_value_pair.first, key)))
      {
        ++iprevious;
        ++icurrent;
      }

      // Did we find it?
      if (icurrent != bucket.end())
      {
        delete_data_node(iprevious, icurrent, bucket);
        n = 1;
      }

      return n;
    }
#endif

    //*********************************************************************
    /// Erases an element.
    ///\param ielement Iterator to the element.
    //*********************************************************************
    iterator erase(const_iterator ielement)
    {
      // Make a note of the next one.
      iterator inext((pbuckets + number_of_buckets), ielement.get_bucket_list_iterator(), ielement.get_local_iterator());
      ++inext;

      bucket_t&      bucket = ielement.get_bucket();
      local_iterator iprevious = bucket.before_begin();
      local_iterator icurrent = ielement.get_local_iterator();

      // Find the node previous to the one we're interested in.
      while (iprevious->etl_next != &*icurrent)
      {
        ++iprevious;
      }

      delete_data_node(iprevious, icurrent, bucket);

      return inext;
    }

    //*********************************************************************
    /// Erases a range of elements.
    /// The range includes all the elements between first and last, including the
    /// element pointed by first, but not the one pointed to by last.
    ///\param first Iterator to the first element.
    ///\param last  Iterator to the last element.
    //*********************************************************************
    iterator erase(const_iterator first_, const_iterator last_)
    {
      // Erasing everything?
      if ((first_ == begin()) && (last_ == end()))
      {
        clear();
        return end();
      }

      // Get the starting point.
      bucket_t*      pbucket     = first_.get_bucket_list_iterator();
      bucket_t*      pend_bucket = last_.get_bucket_list_iterator();
      local_iterator iprevious   = pbucket->before_begin();
      local_iterator icurrent    = first_.get_local_iterator();
      local_iterator iend        = last_.get_local_iterator(); // Note: May not be in the same bucket as icurrent.

      // Find the node previous to the first one.
      while (iprevious->etl_next != &*icurrent)
      {
        ++iprevious;
      }

      // Remember the item before the first erased one.
      iterator ibefore_erased = iterator((pbuckets + number_of_buckets), pbucket, iprevious);

      // Until we reach the end.
      while ((icurrent != iend) || (pbucket != pend_bucket))
      {      
        icurrent = delete_data_node(iprevious, icurrent, *pbucket);

        // Have we not reached the end?
        if ((icurrent != iend) || (pbucket != pend_bucket))
        {
          // At the end of this bucket?
          if ((icurrent == pbucket->end()))
          {
            // Find the next non-empty one.
            do
            {
              ++pbucket;
            } while (pbucket->empty());

            iprevious = pbucket->before_begin();
            icurrent  = pbucket->begin();
          }
        }
      }

      return ++ibefore_erased;
    }

    //*************************************************************************
    /// Clears the unordered_map.
    //*************************************************************************
    void clear()
    {
      initialise();
    }

    //*********************************************************************
    /// Counts an element.
    ///\param key The key to search for.
    ///\return 1 if the key exists, otherwise 0.
    //*********************************************************************
    size_t count(const_key_reference key) const
    {
      return (find(key) == end()) ? 0 : 1;
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Counts an element.
    ///\param key The key to search for.
    ///\return 1 if the key exists, otherwise 0.
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    size_t count(const K& key) const
    {
      return (find(key) == end()) ? 0 : 1;
    }
#endif

    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator to the element if the key exists, otherwise end().
    //*********************************************************************
    iterator find(const_key_reference key)
    {
      size_t index = get_bucket_index(key);

      bucket_t* pbucket = pbuckets + index;
      bucket_t& bucket = *pbucket;

      // Is the bucket not empty?
      if (!bucket.empty())
      {
        // Step though the list until we find the end or an equivalent key.
        local_iterator inode = bucket.begin();
        local_iterator iend = bucket.end();

        while (inode != iend)
        {
          // Do we have this one?
          if (key_equal_function(key, inode->key_value_pair.first))
          {
            return iterator((pbuckets + number_of_buckets), pbucket, inode);
          }

          ++inode;
        }
      }

      return end();
    }

    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator to the element if the key exists, otherwise end().
    //*********************************************************************
    const_iterator find(const_key_reference key) const
    {
      size_t index = get_bucket_index(key);

      bucket_t* pbucket = pbuckets + index;
      bucket_t& bucket = *pbucket;

      // Is the bucket not empty?
      if (!bucket.empty())
      {
        // Step though the list until we find the end or an equivalent key.
        local_iterator inode = bucket.begin();
        local_iterator iend = bucket.end();

        while (inode != iend)
        {
          // Do we have this one?
          if (key_equal_function(key, inode->key_value_pair.first))
          {
            return iterator((pbuckets + number_of_buckets), pbucket, inode);
          }

          ++inode;
        }
      }

      return end();
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator to the element if the key exists, otherwise end().
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    iterator find(const K& key)
    {
      size_t index = get_bucket_index(key);

      bucket_t* pbucket = pbuckets + index;
      bucket_t& bucket = *pbucket;

      // Is the bucket not empty?
      if (!bucket.empty())
      {
        // Step though the list until we find the end or an equivalent key.
        local_iterator inode = bucket.begin();
        local_iterator iend = bucket.end();

        while (inode != iend)
        {
          // Do we have this one?
          if (key_equal_function(key, inode->key_value_pair.first))
          {
            return iterator((pbuckets + number_of_buckets), pbucket, inode);
          }

          ++inode;
        }
      }

      return end();
    }
#endif

#if ETL_USING_CPP11
    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator to the element if the key exists, otherwise end().
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    const_iterator find(const K& key) const
    {
      size_t index = get_bucket_index(key);

      bucket_t* pbucket = pbuckets + index;
      bucket_t& bucket = *pbucket;

      // Is the bucket not empty?
      if (!bucket.empty())
      {
        // Step though the list until we find the end or an equivalent key.
        local_iterator inode = bucket.begin();
        local_iterator iend = bucket.end();

        while (inode != iend)
        {
          // Do we have this one?
          if (key_equal_function(key, inode->key_value_pair.first))
          {
            return iterator((pbuckets + number_of_buckets), pbucket, inode);
          }

          ++inode;
        }
      }

      return end();
    }
#endif

    //*********************************************************************
    /// Returns a range containing all elements with key key in the container.
    /// The range is defined by two iterators, the first pointing to the first
    /// element of the wanted range and the second pointing past the last
    /// element of the range.
    ///\param key The key to search for.
    ///\return An iterator pair to the range of elements if the key exists, otherwise end().
    //*********************************************************************
    ETL_OR_STD::pair<iterator, iterator> equal_range(const_key_reference key)
    {
      iterator f = find(key);
      iterator l = f;

      if (l != end())
      {
        ++l;
      }

      return ETL_OR_STD::pair<iterator, iterator>(f, l);
    }

    //*********************************************************************
    /// Returns a range containing all elements with key key in the container.
    /// The range is defined by two iterators, the first pointing to the first
    /// element of the wanted range and the second pointing past the last
    /// element of the range.
    ///\param key The key to search for.
    ///\return A const iterator pair to the range of elements if the key exists, otherwise end().
    //*********************************************************************
    ETL_OR_STD::pair<const_iterator, const_iterator> equal_range(const_key_reference key) const
    {
      const_iterator f = find(key);
      const_iterator l = f;

      if (l != end())
      {
        ++l;
      }

      return ETL_OR_STD::pair<const_iterator, const_iterator>(f, l);
    }

#if ETL_USING_CPP11
    //*********************************************************************
    /// Returns a range containing all elements with key key in the container.
    /// The range is defined by two iterators, the first pointing to the first
    /// element of the wanted range and the second pointing past the last
    /// element of the range.
    ///\param key The key to search for.
    ///\return An iterator pair to the range of elements if the key exists, otherwise end().
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    ETL_OR_STD::pair<iterator, iterator> equal_range(const K& key)
    {
      iterator f = find(key);
      iterator l = f;

      if (l != end())
      {
        ++l;
      }

      return ETL_OR_STD::pair<iterator, iterator>(f, l);
    }
#endif

#if ETL_USING_CPP11
    //*********************************************************************
    /// Returns a range containing all elements with key key in the container.
    /// The range is defined by two iterators, the first pointing to the first
    /// element of the wanted range and the second pointing past the last
    /// element of the range.
    ///\param key The key to search for.
    ///\return A const iterator pair to the range of elements if the key exists, otherwise end().
    //*********************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    ETL_OR_STD::pair<const_iterator, const_iterator> equal_range(const K& key) const
    {
      const_iterator f = find(key);
      const_iterator l = f;

      if (l != end())
      {
        ++l;
      }

      return ETL_OR_STD::pair<const_iterator, const_iterator>(f, l);
    }
#endif

    //*************************************************************************
    /// Gets the size of the unordered_map.
    //*************************************************************************
    size_type size() const
    {
      return pnodepool->size();
    }

    //*************************************************************************
    /// Gets the maximum possible size of the unordered_map.
    //*************************************************************************
    size_type max_size() const
    {
      return pnodepool->max_size();
    }

    //*************************************************************************
    /// Gets the maximum possible size of the unordered_map.
    //*************************************************************************
    size_type capacity() const
    {
      return pnodepool->max_size();
    }

    //*************************************************************************
    /// Checks to see if the unordered_map is empty.
    //*************************************************************************
    bool empty() const
    {
      return pnodepool->empty();
    }

    //*************************************************************************
    /// Checks to see if the unordered_map is full.
    //*************************************************************************
    bool full() const
    {
      return pnodepool->full();
    }

    //*************************************************************************
    /// Returns the remaining capacity.
    ///\return The remaining capacity.
    //*************************************************************************
    size_t available() const
    {
      return pnodepool->available();
    }

    //*************************************************************************
    /// Returns the load factor = size / bucket_count.
    ///\return The load factor = size / bucket_count.
    //*************************************************************************
    float load_factor() const
    {
      return static_cast<float>(size()) / static_cast<float>(bucket_count());
    }

    //*************************************************************************
    /// Returns the function that hashes the keys.
    ///\return The function that hashes the keys..
    //*************************************************************************
    hasher hash_function() const
    {
      return key_hash_function;
    }

    //*************************************************************************
    /// Returns the function that compares the keys.
    ///\return The function that compares the keys..
    //*************************************************************************
    key_equal key_eq() const
    {
      return key_equal_function;
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    iunordered_map& operator = (const iunordered_map& rhs)
    {
      // Skip if doing self assignment
      if (this != &rhs)
      {
        key_hash_function = rhs.hash_function();
        key_equal_function = rhs.key_eq();
        assign(rhs.cbegin(), rhs.cend());
      }

      return *this;
    }
#if ETL_USING_CPP11
    //*************************************************************************
    /// Move assignment operator.
    //*************************************************************************
    iunordered_map& operator = (iunordered_map&& rhs)
    {
      // Skip if doing self assignment
      if (this != &rhs)
      {
        clear();
        key_hash_function = rhs.hash_function();
        key_equal_function = rhs.key_eq();
        this->move(rhs.begin(), rhs.end());
      }

      return *this;
    }
#endif

    //*************************************************************************
    /// Check if the unordered_map contains the key.
    //*************************************************************************
    bool contains(const_key_reference key) const
    {
      return find(key) != end();
    }

#if ETL_USING_CPP11
    //*************************************************************************
    /// Check if the unordered_map contains the key.
    //*************************************************************************
    template <typename K, typename KE = TKeyEqual, etl::enable_if_t<comparator_is_transparent<KE>::value, int> = 0>
    bool contains(const K& key) const
    {
      return find(key) != end();
    }
#endif

  protected:

    //*********************************************************************
    /// Constructor.
    //*********************************************************************
    iunordered_map(pool_t& node_pool_, bucket_t* pbuckets_, size_t number_of_buckets_, hasher key_hash_function_, key_equal key_equal_function_)
      : pnodepool(&node_pool_)
      , pbuckets(pbuckets_)
      , number_of_buckets(number_of_buckets_)
      , first(pbuckets)
      , last(pbuckets)
      , key_hash_function(key_hash_function_)
      , key_equal_function(key_equal_function_)
    {
    }

    //*********************************************************************
    /// Initialise the unordered_map.
    //*********************************************************************
    void initialise()
    {
      if (!empty())
      {
        // For each bucket...
        for (size_t i = 0UL; i < number_of_buckets; ++i)
        {
          bucket_t& bucket = pbuckets[i];

          if (!bucket.empty())
          {
            // For each item in the bucket...
            local_iterator it = bucket.begin();

            while (it != bucket.end())
            {
              // Destroy the value contents.
              it->key_value_pair.~value_type();
              ETL_DECREMENT_DEBUG_COUNT;

              ++it;
            }

            // Now it's safe to clear the bucket.
            bucket.clear();
          }
        }

        // Now it's safe to clear the entire pool in one go.
        pnodepool->release_all();
      }

      first = pbuckets;
      last  = first;
    }

#if ETL_USING_CPP11
    //*************************************************************************
    /// Move from a range
    //*************************************************************************
    void move(iterator b, iterator e)
    {
      while (b != e)
      {
        iterator temp = b;
        ++temp;
        insert(etl::move(*b));
        b = temp;
      }
    }
#endif

  private:

    //*************************************************************************
    /// Create a node.
    //*************************************************************************
    node_t* allocate_data_node()
    {
      node_t* (etl::ipool::*func)() = &etl::ipool::allocate<node_t>;
      return (pnodepool->*func)();
    }

    //*********************************************************************
    /// Adjust the first and last markers according to the new entry.
    //*********************************************************************
    void adjust_first_last_markers_after_insert(bucket_t* pbucket)
    {
      if (size() == 1)
      {
        first = pbucket;
        last  = pbucket;
      }
      else
      {
        if (pbucket < first)
        {
          first = pbucket;
        }
        else if (pbucket > last)
        {
          last = pbucket;
        }
      }
    }

    //*********************************************************************
    /// Adjust the first and last markers according to the erased entry.
    //*********************************************************************
    void adjust_first_last_markers_after_erase(bucket_t* pbucket)
    {
      if (empty())
      {
        first = pbuckets;
        last  = pbuckets;
      }
      else
      {
        if (pbucket == first)
        {
          // We erased the first so, we need to search again from where we erased.
          while (first->empty())
          {
            ++first;
          }
        }
        else if (pbucket == last)
        {
          // We erased the last, so we need to search again. Start from the first, go no further than the current last.
          pbucket = first;
          bucket_t* pend = last;

          last = first;

          while (pbucket != pend)
          {
            if (!pbucket->empty())
            {
              last = pbucket;
            }

            ++pbucket;
          }
        }
      }
    }

    //*********************************************************************
    /// Delete a data node at the specified location.
    //*********************************************************************
    local_iterator delete_data_node(local_iterator iprevious, local_iterator icurrent, bucket_t& bucket)
    {
      local_iterator inext = bucket.erase_after(iprevious); // Unlink from the bucket.
      icurrent->key_value_pair.~value_type();               // Destroy the value.
      pnodepool->release(&*icurrent);                       // Release it back to the pool.
      adjust_first_last_markers_after_erase(&bucket);
      ETL_DECREMENT_DEBUG_COUNT;

      return inext;
    }

    // Disable copy construction.
    iunordered_map(const iunordered_map&);

    /// The pool of data nodes used in the list.
    pool_t* pnodepool;

    /// The bucket list.
    bucket_t* pbuckets;

    /// The number of buckets.
    const size_t number_of_buckets;

    /// The first and last pointers to buckets with values.
    bucket_t* first;
    bucket_t* last;

    /// The function that creates the hashes.
    hasher key_hash_function;

    /// The function that compares the keys for equality.
    key_equal key_equal_function;

    /// For library debugging purposes only.
    ETL_DECLARE_DEBUG_COUNT;

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
#if defined(ETL_POLYMORPHIC_UNORDERED_MAP) || defined(ETL_POLYMORPHIC_CONTAINERS)
  public:
    virtual ~iunordered_map()
    {
    }
#else
  protected:
    ~iunordered_map()
    {
    }
#endif
  };

  //***************************************************************************
  /// Equal operator.
  ///\param lhs Reference to the first unordered_map.
  ///\param rhs Reference to the second unordered_map.
  ///\return <b>true</b> if the arrays are equal, otherwise <b>false</b>
  ///\ingroup unordered_map
  //***************************************************************************
  template <typename TKey, typename T, typename THash, typename TKeyEqual>
  bool operator ==(const etl::iunordered_map<TKey, T, THash, TKeyEqual>& lhs, 
                   const etl::iunordered_map<TKey, T, THash, TKeyEqual>& rhs)
  {
    const bool sizes_match = (lhs.size() == rhs.size());
    bool elements_match = true;

    typedef typename etl::iunordered_map<TKey, T, THash, TKeyEqual>::const_iterator itr_t;
    
    if (sizes_match)
    {
      itr_t l_begin = lhs.begin();
      itr_t l_end   = lhs.end();

      while ((l_begin != l_end) && elements_match)
      {
        const TKey key     = l_begin->first;
        const T    l_value = l_begin->second;

        // See if the lhs key exists in the rhs.
        ETL_OR_STD::pair<itr_t, itr_t> range = rhs.equal_range(key);

        if (range.first != rhs.end())
        {
          // See if the values match
          const T r_value = range.first->second;

          elements_match = (r_value == l_value);
        }
        else
        {
          elements_match = false;
        }

        ++l_begin;
      }
    }

    return (sizes_match && elements_match);
  }

  //***************************************************************************
  /// Not equal operator.
  ///\param lhs Reference to the first unordered_map.
  ///\param rhs Reference to the second unordered_map.
  ///\return <b>true</b> if the arrays are not equal, otherwise <b>false</b>
  ///\ingroup unordered_map
  //***************************************************************************
  template <typename TKey, typename T, typename THash, typename TKeyEqual>
  bool operator !=(const etl::iunordered_map<TKey, T, THash, TKeyEqual>& lhs, 
                   const etl::iunordered_map<TKey, T, THash, TKeyEqual>& rhs)
  {
    return !(lhs == rhs);
  }

  //*************************************************************************
  /// A templated unordered_map implementation that uses a fixed size buffer.
  //*************************************************************************
  template <typename TKey, typename TValue, const size_t MAX_SIZE_, const size_t MAX_BUCKETS_ = MAX_SIZE_, typename THash = etl::hash<TKey>, typename TKeyEqual = etl::equal_to<TKey> >
  class unordered_map : public etl::iunordered_map<TKey, TValue, THash, TKeyEqual>
  {
  private:

    typedef iunordered_map<TKey, TValue, THash, TKeyEqual> base;

  public:

    static ETL_CONSTANT size_t MAX_SIZE    = MAX_SIZE_;
    static ETL_CONSTANT size_t MAX_BUCKETS = MAX_BUCKETS_;

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    unordered_map(const THash& hash = THash(), const TKeyEqual& equal = TKeyEqual())
      : base(node_pool, buckets, MAX_BUCKETS_, hash, equal)
    {
    }

    //*************************************************************************
    /// Copy constructor.
    //*************************************************************************
    unordered_map(const unordered_map& other)
      : base(node_pool, buckets, MAX_BUCKETS_, other.hash_function(), other.key_eq())
    {
      base::assign(other.cbegin(), other.cend());
    }

#if ETL_USING_CPP11
    //*************************************************************************
    /// Move constructor.
    //*************************************************************************
    unordered_map(unordered_map&& other)
      : base(node_pool, buckets, MAX_BUCKETS_, other.hash_function(), other.key_eq())
    {
      if (this != &other)
      {
        base::move(other.begin(), other.end());
      }
    }
#endif

    //*************************************************************************
    /// Constructor, from an iterator range.
    ///\tparam TIterator The iterator type.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*************************************************************************
    template <typename TIterator>
    unordered_map(TIterator first_, TIterator last_, const THash& hash = THash(), const TKeyEqual& equal = TKeyEqual())
      : base(node_pool, buckets, MAX_BUCKETS_, hash, equal)
    {
      base::assign(first_, last_);
    }

#if ETL_HAS_INITIALIZER_LIST
    //*************************************************************************
    /// Construct from initializer_list.
    //*************************************************************************
    unordered_map(std::initializer_list<ETL_OR_STD::pair<TKey, TValue>> init, const THash& hash = THash(), const TKeyEqual& equal = TKeyEqual())
      : base(node_pool, buckets, MAX_BUCKETS_, hash, equal)
    {
      base::assign(init.begin(), init.end());
    }
#endif

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~unordered_map()
    {
      base::initialise();
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    unordered_map& operator = (const unordered_map& rhs)
    {
      base::operator=(rhs);
      return *this;
    }

#if ETL_USING_CPP11
    //*************************************************************************
    /// Move assignment operator.
    //*************************************************************************
    unordered_map& operator = (unordered_map&& rhs)
    {
      base::operator=(etl::move(rhs));
      return *this;
    }
#endif

  private:

    /// The pool of nodes used for the unordered_map.
    etl::pool<typename base::node_t, MAX_SIZE> node_pool;

    /// The buckets of node lists.
    typename base::bucket_t buckets[MAX_BUCKETS_];
  };

  //*************************************************************************
  /// Template deduction guides.
  //*************************************************************************
#if ETL_USING_CPP17 && ETL_HAS_INITIALIZER_LIST
  template <typename... TPairs>
  unordered_map(TPairs...) -> unordered_map<typename etl::nth_type_t<0, TPairs...>::first_type,
                                            typename etl::nth_type_t<0, TPairs...>::second_type,
                                            sizeof...(TPairs)>;
#endif

  //*************************************************************************
  /// Make
  //*************************************************************************
#if ETL_USING_CPP11 && ETL_HAS_INITIALIZER_LIST
  template <typename TKey, typename T, typename THash = etl::hash<TKey>, typename TKeyEqual = etl::equal_to<TKey>, typename... TPairs>
  constexpr auto make_unordered_map(TPairs&&... pairs) -> etl::unordered_map<TKey, T, sizeof...(TPairs), sizeof...(TPairs), THash, TKeyEqual>
  {
    return { etl::forward<TPairs>(pairs)... };
  }
#endif
}

#endif
