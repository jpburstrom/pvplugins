// PluginPVPlugins.cpp
// Johannes Burström (johannes@ljud.org)

#include "SC_PlugIn.hpp"
#include "PVPlugins.hpp"

static InterfaceTable* ft;

namespace PVPlugins {

PVPlugins::PVPlugins() {
    mCalcFunc = make_calc_function<PVPlugins, &PVPlugins::next>();
    next(1);
}

void PVPlugins::next(int nSamples) {
    const float* input = in(0);
    const float* gain = in(0);
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain[i];
    }
}

} // namespace PVPlugins

PluginLoad(PVPluginsUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<PVPlugins::PVPlugins>(ft, "PVPlugins");
}
