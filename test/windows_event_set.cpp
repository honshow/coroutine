/**
 * @author github.com/luncliff (luncliff@gmail.com)
 */
#include <atomic>
#include <cassert>
#include <iostream>

#include <gsl/gsl>

#include <coroutine/return.h>
#include <coroutine/windows.h>

using namespace std;
using namespace coro;

auto wait_an_event(set_or_cancel& token, atomic_flag& flag) -> frame_t;

int main(int, char*[]) {
    HANDLE e = CreateEventEx(nullptr, nullptr, //
                             CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
    assert(e != NULL);
    auto on_return = gsl::finally([e]() { CloseHandle(e); });

    ResetEvent(e);
    set_or_cancel token{e};
    atomic_flag flag = ATOMIC_FLAG_INIT;

    wait_an_event(token, flag);
    SetEvent(e); // set

    SleepEx(30, true); // give time to windows threads
    assert(flag.test_and_set());
    return EXIT_SUCCESS;
}

auto wait_an_event(set_or_cancel& evt, atomic_flag& flag) -> frame_t {
    // wait for set or cancel
    // `co_await` will forward `GetLastError` if canceled.
    if (DWORD ec = co_await evt) {
        cerr << system_category().message(ec) << endl;
        co_return;
    }
    flag.test_and_set();
}
