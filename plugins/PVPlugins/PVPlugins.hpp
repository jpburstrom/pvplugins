// PluginPVPlugins.hpp
// Johannes Burström (johannes@ljud.org)

#pragma once

#include "SC_PlugIn.hpp"

namespace PVPlugins {

class PVPlugins : public SCUnit {
public:
    PVPlugins();

    // Destructor
    // ~PVPlugins();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace PVPlugins
