// Fuzz harness for the Verilated runtime's message formatters (include/verilated.cpp).
//
// The fuzzer bytes are the *Verilog* format string handed to VL_SFORMATF_N_NX() ->
// _vl_vsformat(), the $display/$sformatf formatter, together with a fixed, well-formed argument
// list that covers every VL_VFORMATATTR kind (timeunit, scope, narrow, quad, wide, real, string).
// The same bytes also go through VL_DBG_MSGF() as an *argument* ("%s"), never as its printf
// format: VL_DBG_MSGF forwards its format to libc vsnprintf, and a format with no matching
// arguments is harness misuse, not a Verilator bug.
//
// Uses no libFuzzer-only headers so the same source links against both the fuzzing
// engine and the standalone (run-once) reproducer driver.
#include <cstddef>
#include <cstdint>
#include <string>

#include "verilated.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // A model's _Vconfigure() sets the timescale before any $display runs; the bare context's
    // constructor default leaves timeprecision() with the wrong sign (index OOB in %t).
    static const bool configured = [] {
        VerilatedContext* const contextp = Verilated::threadContextp();
        contextp->timeunit(-9);
        contextp->timeprecision(-12);
        return true;
    }();
    static_cast<void>(configured);

    const std::string fmt(reinterpret_cast<const char*>(data), size);

    VL_DBG_MSGF("%s\n", fmt.c_str());

    EData wide[4] = {0xdeadbeefU, 0x01234567U, 0x89abcdefU, 0x0000000fU};  // 100 bits
    std::string text("fuzz-string");
    const std::string out = VL_SFORMATF_N_NX(
        fmt, 7,  //
        VL_VFORMATATTR_TIMEUNIT, -9,  //
        VL_VFORMATATTR_SCOPE, "top", "inst",  //
        VL_VFORMATATTR_UNSIGNED, 32, static_cast<IData>(0xa5a5a5a5U),  //
        VL_VFORMATATTR_SIGNED, 64, static_cast<QData>(0xfedcba9876543210ULL),  //
        VL_VFORMATATTR_UNSIGNED, 100, wide,  //
        VL_VFORMATATTR_DOUBLE, 3.14159,  //
        VL_VFORMATATTR_STRING, &text);
    // Keep the result observable so the formatter is not optimised away.
    return out.size() == static_cast<size_t>(-1) ? 1 : 0;
}
