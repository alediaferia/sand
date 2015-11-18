#include "console.h"

#ifdef UNIX
#include <signal.h>
#endif

static const char *SAND_CONSOLE_HEADER =
" \
  Sand Version 0.0.0.1 \
  Press Ctrl+C to Quit \
";

static const char *SAND_PS = "sand> ";

static Console *s_instance = nullptr;
void _int_handler(int sig) {
    if (s_instance != 0) {
        s_instance->_running = false;
    }
}

Console::Console() :_running(false) {
    s_instance = this;
}

void Console::run() {
#ifdef UNIX
    signal(SIGINT, _int_handler);
#endif
    while (_running) {

    }
}
