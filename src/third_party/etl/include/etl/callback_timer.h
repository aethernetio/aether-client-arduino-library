/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2017 John Wellbelove

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

#ifndef ETL_CALLBACK_TIMER_INCLUDED
#define ETL_CALLBACK_TIMER_INCLUDED

#include "third_party/etl/include/etl/platform.h"
#include "third_party/etl/include/etl/algorithm.h"
#include "third_party/etl/include/etl/nullptr.h"
#include "third_party/etl/include/etl/function.h"
#include "third_party/etl/include/etl/static_assert.h"
#include "third_party/etl/include/etl/timer.h"
#include "third_party/etl/include/etl/atomic.h"
#include "third_party/etl/include/etl/error_handler.h"
#include "third_party/etl/include/etl/placement_new.h"
#include "third_party/etl/include/etl/delegate.h"

#include <stdint.h>

#if defined(ETL_IN_UNIT_TEST) && ETL_NOT_USING_STL
  #define ETL_DISABLE_TIMER_UPDATES
  #define ETL_ENABLE_TIMER_UPDATES
  #define ETL_TIMER_UPDATES_ENABLED true

  #undef ETL_CALLBACK_TIMER_USE_ATOMIC_LOCK
  #undef ETL_CALLBACK_TIMER_USE_INTERRUPT_LOCK
#else
  #if !defined(ETL_CALLBACK_TIMER_USE_ATOMIC_LOCK) && !defined(ETL_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
    #error ETL_CALLBACK_TIMER_USE_ATOMIC_LOCK or ETL_CALLBACK_TIMER_USE_INTERRUPT_LOCK not defined
  #endif

  #if defined(ETL_CALLBACK_TIMER_USE_ATOMIC_LOCK) && defined(ETL_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
    #error Only define one of ETL_CALLBACK_TIMER_USE_ATOMIC_LOCK or ETL_CALLBACK_TIMER_USE_INTERRUPT_LOCK
  #endif

  #if defined(ETL_CALLBACK_TIMER_USE_ATOMIC_LOCK)
    #define ETL_DISABLE_TIMER_UPDATES (++process_semaphore)
    #define ETL_ENABLE_TIMER_UPDATES  (--process_semaphore)
    #define ETL_TIMER_UPDATES_ENABLED (process_semaphore.load() == 0)
  #endif
#endif

#if defined(ETL_CALLBACK_TIMER_USE_INTERRUPT_LOCK)
  #if !defined(ETL_CALLBACK_TIMER_DISABLE_INTERRUPTS) || !defined(ETL_CALLBACK_TIMER_ENABLE_INTERRUPTS)
    #error ETL_CALLBACK_TIMER_DISABLE_INTERRUPTS and/or ETL_CALLBACK_TIMER_ENABLE_INTERRUPTS not defined
  #endif

  #define ETL_DISABLE_TIMER_UPDATES ETL_CALLBACK_TIMER_DISABLE_INTERRUPTS
  #define ETL_ENABLE_TIMER_UPDATES  ETL_CALLBACK_TIMER_ENABLE_INTERRUPTS
  #define ETL_TIMER_UPDATES_ENABLED true
#endif

namespace etl
{
  //*************************************************************************
  /// The configuration of a timer.
  struct callback_timer_data
  {
    typedef etl::delegate<void(void)> callback_type;

    enum callback_type_id
    {
      C_CALLBACK,
      IFUNCTION,
      DELEGATE
    };

    //*******************************************
    callback_timer_data()
      : p_callback(ETL_NULLPTR),
        period(0),
        delta(etl::timer::state::Inactive),
        id(etl::timer::id::NO_TIMER),
        previous(etl::timer::id::NO_TIMER),
        next(etl::timer::id::NO_TIMER),
        repeating(true),
        cbk_type(IFUNCTION)
    {
    }

    //*******************************************
    /// C function callback
    //*******************************************
    callback_timer_data(etl::timer::id::type id_,
                        void                 (*p_callback_)(),
                        uint32_t             period_,
                        bool                 repeating_)
      : p_callback(reinterpret_cast<void*>(p_callback_)),
        period(period_),
        delta(etl::timer::state::Inactive),
        id(id_),
        previous(etl::timer::id::NO_TIMER),
        next(etl::timer::id::NO_TIMER),
        repeating(repeating_),
        cbk_type(C_CALLBACK)
    {
    }

    //*******************************************
    /// ETL function callback
    //*******************************************
    callback_timer_data(etl::timer::id::type  id_,
                        etl::ifunction<void>& callback_,
                        uint32_t              period_,
                        bool                  repeating_)
      : p_callback(reinterpret_cast<void*>(&callback_)),
        period(period_),
        delta(etl::timer::state::Inactive),
        id(id_),
        previous(etl::timer::id::NO_TIMER),
        next(etl::timer::id::NO_TIMER),
        repeating(repeating_),
        cbk_type(IFUNCTION)
    {
    }

    //*******************************************
    /// ETL delegate callback
    //*******************************************
    callback_timer_data(etl::timer::id::type id_,
                        callback_type&       callback_,
                        uint32_t             period_,
                        bool                 repeating_)
            : p_callback(reinterpret_cast<void*>(&callback_)),
              period(period_),
              delta(etl::timer::state::Inactive),
              id(id_),
              previous(etl::timer::id::NO_TIMER),
              next(etl::timer::id::NO_TIMER),
              repeating(repeating_),
              cbk_type(DELEGATE)
    {
    }

    //*******************************************
    /// Returns true if the timer is active.
    //*******************************************
    bool is_active() const
    {
      return delta != etl::timer::state::Inactive;
    }

    //*******************************************
    /// Sets the timer to the inactive state.
    //*******************************************
    void set_inactive()
    {
      delta = etl::timer::state::Inactive;
    }

    void*                 p_callback;
    uint32_t              period;
    uint32_t              delta;
    etl::timer::id::type  id;
    uint_least8_t         previous;
    uint_least8_t         next;
    bool                  repeating;
    callback_type_id      cbk_type;

  private:

    // Disabled.
    callback_timer_data(const callback_timer_data& other);
    callback_timer_data& operator =(const callback_timer_data& other);
  };

  namespace private_callback_timer
  {
    //*************************************************************************
    /// A specialised intrusive linked list for timer data.
    //*************************************************************************
    class list
    {
    public:

      //*******************************
      list(etl::callback_timer_data* ptimers_)
        : head(etl::timer::id::NO_TIMER),
          tail(etl::timer::id::NO_TIMER),
          current(etl::timer::id::NO_TIMER),
          ptimers(ptimers_)
      {
      }

      //*******************************
      bool empty() const
      {
        return head == etl::timer::id::NO_TIMER;
      }

      //*******************************
      // Inserts the timer at the correct delta position
      //*******************************
      void insert(etl::timer::id::type id_)
      {
        etl::callback_timer_data& timer = ptimers[id_];

        if (head == etl::timer::id::NO_TIMER)
        {
          // No entries yet.
          head = id_;
          tail = id_;
          timer.previous = etl::timer::id::NO_TIMER;
          timer.next     = etl::timer::id::NO_TIMER;
        }
        else
        {
          // We already have entries.
          etl::timer::id::type test_id = begin();

          while (test_id != etl::timer::id::NO_TIMER)
          {
            etl::callback_timer_data& test = ptimers[test_id];

            // Find the correct place to insert.
            if (timer.delta <= test.delta)
            {
              if (test.id == head)
              {
                head = timer.id;
              }

              // Insert before test.
              timer.previous = test.previous;
              test.previous  = timer.id;
              timer.next     = test.id;

              // Adjust the next delta to compensate.
              test.delta -= timer.delta;

              if (timer.previous != etl::timer::id::NO_TIMER)
              {
                ptimers[timer.previous].next = timer.id;
              }
              break;
            }
            else
            {
              timer.delta -= test.delta;
            }

            test_id = next(test_id);
          }

          // Reached the end?
          if (test_id == etl::timer::id::NO_TIMER)
          {
            // Tag on to the tail.
            ptimers[tail].next = timer.id;
            timer.previous     = tail;
            timer.next         = etl::timer::id::NO_TIMER;
            tail               = timer.id;
          }
        }
      }

      //*******************************
      void remove(etl::timer::id::type id_, bool has_expired)
      {
        etl::callback_timer_data& timer = ptimers[id_];

        if (head == id_)
        {
          head = timer.next;
        }
        else
        {
          ptimers[timer.previous].next = timer.next;
        }

        if (tail == id_)
        {
          tail = timer.previous;
        }
        else
        {
          ptimers[timer.next].previous = timer.previous;
        }

        if (!has_expired)
        {
          // Adjust the next delta.
          if (timer.next != etl::timer::id::NO_TIMER)
          {
            ptimers[timer.next].delta += timer.delta;
          }
        }

        timer.previous = etl::timer::id::NO_TIMER;
        timer.next     = etl::timer::id::NO_TIMER;
        timer.delta    = etl::timer::state::Inactive;
      }

      //*******************************
      etl::callback_timer_data& front()
      {
        return ptimers[head];
      }

      //*******************************
      const etl::callback_timer_data& front() const
      {
        return ptimers[head];
      }

      //*******************************
      etl::timer::id::type begin()
      {
        current = head;
        return current;
      }

      //*******************************
      etl::timer::id::type previous(etl::timer::id::type last)
      {
        current = ptimers[last].previous;
        return current;
      }

      //*******************************
      etl::timer::id::type next(etl::timer::id::type last)
      {
        current = ptimers[last].next;
        return current;
      }

      //*******************************
      void clear()
      {
        etl::timer::id::type id = begin();

        while (id != etl::timer::id::NO_TIMER)
        {
          etl::callback_timer_data& timer = ptimers[id];
          id = next(id);
          timer.next = etl::timer::id::NO_TIMER;
        }

        head    = etl::timer::id::NO_TIMER;
        tail    = etl::timer::id::NO_TIMER;
        current = etl::timer::id::NO_TIMER;
      }

    private:

      etl::timer::id::type head;
      etl::timer::id::type tail;
      etl::timer::id::type current;

      etl::callback_timer_data* const ptimers;
    };
  }

  //***************************************************************************
  /// Interface for callback timer
  //***************************************************************************
  class icallback_timer
  {
  public:

    typedef etl::delegate<void(void)> callback_type;

    //*******************************************
    /// Register a timer.
    //*******************************************
    etl::timer::id::type register_timer(void     (*p_callback_)(),
                                        uint32_t period_,
                                        bool     repeating_)
    {
      etl::timer::id::type id = etl::timer::id::NO_TIMER;

      bool is_space = (registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // Search for the free space.
        for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
        {
          etl::callback_timer_data& timer = timer_array[i];

          if (timer.id == etl::timer::id::NO_TIMER)
          {
            // Create in-place.
            new (&timer) callback_timer_data(i, p_callback_, period_, repeating_);
            ++registered_timers;
            id = i;
            break;
          }
        }
      }

      return id;
    }

    //*******************************************
    /// Register a timer.
    //*******************************************
    etl::timer::id::type register_timer(etl::ifunction<void>& callback_,
                                        uint32_t              period_,
                                        bool                  repeating_)
    {
      etl::timer::id::type id = etl::timer::id::NO_TIMER;

      bool is_space = (registered_timers < MAX_TIMERS);

      if (is_space)
      {
        // Search for the free space.
        for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
        {
          etl::callback_timer_data& timer = timer_array[i];

          if (timer.id == etl::timer::id::NO_TIMER)
          {
            // Create in-place.
            new (&timer) callback_timer_data(i, callback_, period_, repeating_);
            ++registered_timers;
            id = i;
            break;
          }
        }
      }

      return id;
    }

      //*******************************************
      /// Register a timer.
      //*******************************************
#if ETL_USING_CPP11
      etl::timer::id::type register_timer(callback_type& callback_,
                                          uint32_t       period_,
                                          bool           repeating_)
      {
          etl::timer::id::type id = etl::timer::id::NO_TIMER;

          bool is_space = (registered_timers < MAX_TIMERS);

          if (is_space)
          {
              // Search for the free space.
              for (uint_least8_t i = 0U; i < MAX_TIMERS; ++i)
              {
                  etl::callback_timer_data& timer = timer_array[i];

                  if (timer.id == etl::timer::id::NO_TIMER)
                  {
                      // Create in-place.
                      new (&timer) callback_timer_data(i, callback_, period_, repeating_);
                      ++registered_timers;
                      id = i;
                      break;
                  }
              }
          }

          return id;
      }
#endif

    //*******************************************
    /// Unregister a timer.
    //*******************************************
    bool unregister_timer(etl::timer::id::type id_)
    {
      bool result = false;

      if (id_ != etl::timer::id::NO_TIMER)
      {
        etl::callback_timer_data& timer = timer_array[id_];

        if (timer.id != etl::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            ETL_DISABLE_TIMER_UPDATES;
            active_list.remove(timer.id, false);
            ETL_ENABLE_TIMER_UPDATES;
          }

          // Reset in-place.
          new (&timer) callback_timer_data();
          --registered_timers;

          result = true;
        }
      }

      return result;
    }

    //*******************************************
    /// Enable/disable the timer.
    //*******************************************
    void enable(bool state_)
    {
      enabled = state_;
    }

    //*******************************************
    /// Get the enable/disable state.
    //*******************************************
    bool is_running() const
    {
      return enabled;
    }

    //*******************************************
    /// Clears the timer of data.
    //*******************************************
    void clear()
    {
      ETL_DISABLE_TIMER_UPDATES;
      active_list.clear();
      ETL_ENABLE_TIMER_UPDATES;

      for (int i = 0; i < MAX_TIMERS; ++i)
      {
        ::new (&timer_array[i]) callback_timer_data();
      }

      registered_timers = 0;
    }

    //*******************************************
    // Called by the timer service to indicate the
    // amount of time that has elapsed since the last successful call to 'tick'.
    // Returns true if the tick was processed,
    // false if not.
    //*******************************************
    bool tick(uint32_t count)
    {
      if (enabled)
      {
        if (ETL_TIMER_UPDATES_ENABLED)
        {
          // We have something to do?
          bool has_active = !active_list.empty();

          if (has_active)
          {
            while (has_active && (count >= active_list.front().delta))
            {
              etl::callback_timer_data& timer = active_list.front();

              count -= timer.delta;

              active_list.remove(timer.id, true);

              if (timer.repeating)
              {
                // Reinsert the timer.
                timer.delta = timer.period;
                active_list.insert(timer.id);
              }

              if (timer.p_callback != ETL_NULLPTR)
              {
                if (timer.cbk_type == callback_timer_data::C_CALLBACK)
                {
                  // Call the C callback.
                  reinterpret_cast<void(*)()>(timer.p_callback)();
                }
                else if(timer.cbk_type == callback_timer_data::IFUNCTION)
                {
                  // Call the function wrapper callback.
                  (*reinterpret_cast<etl::ifunction<void>*>(timer.p_callback))();
                }
                else if(timer.cbk_type == callback_timer_data::DELEGATE)
                {
                    // Call the delegate callback.
                    (*reinterpret_cast<callback_type*>(timer.p_callback))();
                }
              }

              has_active = !active_list.empty();
            }

            if (has_active)
            {
              // Subtract any remainder from the next due timeout.
              active_list.front().delta -= count;
            }
          }

          return true;
        }
      }

      return false;
    }

    //*******************************************
    /// Starts a timer.
    //*******************************************
    bool start(etl::timer::id::type id_, bool immediate_ = false)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != etl::timer::id::NO_TIMER)
      {
        etl::callback_timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != etl::timer::id::NO_TIMER)
        {
          // Has a valid period.
          if (timer.period != etl::timer::state::Inactive)
          {
            ETL_DISABLE_TIMER_UPDATES;
            if (timer.is_active())
            {
              active_list.remove(timer.id, false);
            }

            timer.delta = immediate_ ? 0 : timer.period;
            active_list.insert(timer.id);
            ETL_ENABLE_TIMER_UPDATES;

            result = true;
          }
        }
      }

      return result;
    }

    //*******************************************
    /// Stops a timer.
    //*******************************************
    bool stop(etl::timer::id::type id_)
    {
      bool result = false;

      // Valid timer id?
      if (id_ != etl::timer::id::NO_TIMER)
      {
        etl::callback_timer_data& timer = timer_array[id_];

        // Registered timer?
        if (timer.id != etl::timer::id::NO_TIMER)
        {
          if (timer.is_active())
          {
            ETL_DISABLE_TIMER_UPDATES;
            active_list.remove(timer.id, false);
            ETL_ENABLE_TIMER_UPDATES;
          }

          result = true;
        }
      }

      return result;
    }

    //*******************************************
    /// Sets a timer's period.
    //*******************************************
    bool set_period(etl::timer::id::type id_, uint32_t period_)
    {
      if (stop(id_))
      {
        timer_array[id_].period = period_;
        return true;
      }

      return false;
    }

    //*******************************************
    /// Sets a timer's mode.
    //*******************************************
    bool set_mode(etl::timer::id::type id_, bool repeating_)
    {
      if (stop(id_))
      {
        timer_array[id_].repeating = repeating_;
        return true;
      }

      return false;
    }

    //*******************************************
    /// Check if there is an active timer.
    //*******************************************
    bool has_active_timer() const
    {
      return !active_list.empty();
    }

    //*******************************************
    /// Get the time to the next timer event.
    /// Returns etl::timer::interval::No_Active_Interval if there is no active timer.
    //*******************************************
    uint32_t time_to_next() const
    {
      uint32_t delta = static_cast<uint32_t>(etl::timer::interval::No_Active_Interval);

      if (has_active_timer())
      {
        delta = active_list.front().delta;
      }

      return delta;
    }

    //*******************************************
    /// Checks if a timer is currently active.
    /// Returns <b>true</b> if the timer is active, otherwise <b>false</b>.
    //*******************************************
    bool is_active(etl::timer::id::type id_) const
    {
      // Valid timer id?
      if (is_valid_timer_id(id_))
      {
        if (has_active_timer())
        {
          const etl::callback_timer_data& timer = timer_array[id_];

          // Registered timer?
          if (timer.id != etl::timer::id::NO_TIMER)
          {
            return timer.is_active();
          }
        }
      }

      return false;
    }

  protected:

    //*******************************************
    /// Constructor.
    //*******************************************
    icallback_timer(callback_timer_data* const timer_array_, const uint_least8_t  MAX_TIMERS_)
      : timer_array(timer_array_),
        active_list(timer_array_),
        enabled(false),
#if defined(ETL_CALLBACK_TIMER_USE_ATOMIC_LOCK)
        process_semaphore(0),
#endif
        registered_timers(0),
        MAX_TIMERS(MAX_TIMERS_)
    {
    }

  private:

    //*******************************************
    /// Check that the timer id is valid.
    //*******************************************
    bool is_valid_timer_id(etl::timer::id::type id_) const
    {
      return (id_ < MAX_TIMERS);
    }

    // The array of timer data structures.
    callback_timer_data* const timer_array;

    // The list of active timers.
    private_callback_timer::list active_list;

    volatile bool enabled;
#if defined(ETL_CALLBACK_TIMER_USE_ATOMIC_LOCK)

#if defined(ETL_TIMER_SEMAPHORE_TYPE)
    typedef ETL_TIMER_SEMAPHORE_TYPE timer_semaphore_t;
#else
  #if ETL_HAS_ATOMIC
    typedef etl::atomic_uint16_t timer_semaphore_t;
  #else
    #error No atomic type available
  #endif
#endif

    mutable etl::timer_semaphore_t process_semaphore;
#endif
    uint_least8_t registered_timers;

  public:

    const uint_least8_t MAX_TIMERS;
  };

  //***************************************************************************
  /// The callback timer
  //***************************************************************************
  template <const uint_least8_t MAX_TIMERS_>
  class callback_timer : public etl::icallback_timer
  {
  public:

    ETL_STATIC_ASSERT(MAX_TIMERS_ <= 254, "No more than 254 timers are allowed");

    //*******************************************
    /// Constructor.
    //*******************************************
    callback_timer()
      : icallback_timer(timer_array, MAX_TIMERS_)
    {
    }

  private:

    callback_timer_data timer_array[MAX_TIMERS_];
  };
}

#undef ETL_DISABLE_TIMER_UPDATES
#undef ETL_ENABLE_TIMER_UPDATES
#undef ETL_TIMER_UPDATES_ENABLED

#endif
