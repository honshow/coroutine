/**
 * @author github.com/luncliff (luncliff@gmail.com)
 */
#undef NDEBUG
#include <cassert>

#include "internal/channel.hpp"
#include <coroutine/return.h> // includes `coroutine_traits<void, ...>`

using namespace std;
using namespace coro;

constexpr int bye = 0;

auto consumer(channel<int>& ch) -> void {
    // the type doesn't support for-co_await for now
    for (auto [msg, ok] = co_await ch.read(); ok;
         tie(msg, ok) = co_await ch.read()) {
        // ok == true: we received a value
        if (msg == bye) {
            puts("consumer loop exit");
            break;
        }
    }

    // we know that we received the `bye`,
    // but will read again to check `ok` becomes false
    auto [_, ok] = co_await ch.read();
    // ok == false: channel is going to be destroyed (no effect for read/write)
    assert(ok == false);
    puts("channel destruction detected");
}

auto producer_owner() -> void {
    channel<int> ch{};
    consumer(ch); // start a consumer routine

    for (int msg : {1, 2, 3, bye}) {
        auto ok = co_await ch.write(msg);
        // ok == true: we sent a value
        assert(ok == true);
    }
    puts("producer loop exit");
    puts("producer_owner is going to return");
}

int main(int, char*[]) {
    producer_owner();
    return EXIT_SUCCESS;
}
