//
//  Author  : github.com/luncliff (luncliff@gmail.com)
//  License : CC BY 4.0
//
//  Note
//    This is a test code for GCC C++ Coroutines
//    Save current coroutine's frame using co_await operator
//

// https://github.com/iains/gcc-cxx-coroutines/blob/c%2B%2B-coroutines/gcc/testsuite/g%2B%2B.dg/coroutines/coro.h
#include "coro.h"

using namespace std;
using namespace std::experimental;

class promise_return_preserve {
  public:
    auto initial_suspend() noexcept {
        return suspend_never{};
    }
    auto final_suspend() noexcept {
        return suspend_always{};
    }
    void unhandled_exception() noexcept {
    }
};

class preserve_frame final : public coroutine_handle<void> {
  public:
    class promise_type final : public promise_return_preserve {
      public:
        void return_void() noexcept {
            // nothing to do because this is `void` return
        }
        auto get_return_object() noexcept -> preserve_frame {
            return preserve_frame{this};
        }
        static auto get_return_object_on_allocation_failure() noexcept
            -> preserve_frame {
            return preserve_frame{nullptr};
        }
    };

  private:
    explicit preserve_frame(promise_type* p) noexcept
        : coroutine_handle<void>{} {
        coroutine_handle<void>& ref = *this;
        ref = coroutine_handle<promise_type>::from_promise(*p);
    }

  public:
    // gcc-10 requires the type to be default constructible
    preserve_frame() noexcept = default;
};

class save_frame_t final {
  public:
    void await_suspend(coroutine_handle<void> coro) noexcept {
        ref = coro;
    }
    constexpr bool await_ready() noexcept {
        return false;
    }
    constexpr void await_resume() noexcept {
    }

  public:
    explicit save_frame_t(coroutine_handle<void>& target) noexcept
        : ref{target} {
    }

  private:
    coroutine_handle<void>& ref;
};

auto return_void_and_preserve(coroutine_handle<void>& coro) noexcept
    -> preserve_frame {
    co_await save_frame_t{coro};
    co_return;
}

int main(int, char* []) {
    // coroutine_handle<void> coro{};
    // return_void_and_preserve(coro);
    // REQUIRE(coro.done() == false);
    // coro.resume();
    // REQUIRE(coro.done());
    // coro.destroy();
    return 0;
}
