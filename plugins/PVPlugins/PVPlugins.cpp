// PluginPVPlugins.cpp
// Johannes Burström (johannes@ljud.org)

#include "SC_PlugIn.h"
#include "FFT_UGens.h"

#define DefinePVUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(PV_Unit), (UnitCtorFunc)&name##_Ctor, 0, 0);

#define MaybeIncrementPartial(i) \
    if (currentPartial < partials && i > binEnd) { \
        ++currentPartial; \
        CalculateBins; \
    }

#define CalculateBins \
    binStart = (int)sc_ceil(freq * currentPartial * binWidthR) - (bandwidth >> 1) - 1; \
    binEnd = binStart + bandwidth;
    //Print("partial: %if, binStart: %i, binEnd: %i\n", currentPartial, binStart, binEnd);

InterfaceTable* ft;

struct PV_FilterPartials : PV_Unit {
    SndBuf* m_buf;
    float m_binWidth;
};


struct PV_SplitPartials : PV_Unit {
    SndBuf* m_buf;
    float m_binWidth;
};


static void PV_FilterPartials_next(PV_FilterPartials* unit, int inNumSamples);
static void PV_FilterPartials_Ctor(PV_FilterPartials* unit);

static void PV_SplitPartials_next(PV_SplitPartials* unit, int inNumSamples);
static void PV_SplitPartials_Ctor(PV_SplitPartials* unit);

void PV_FilterPartials_Ctor(PV_FilterPartials* unit) {
	SETCALC(PV_FilterPartials_next);
	ZOUT0(0) = ZIN0(0);

}

void PV_FilterPartials_next(PV_FilterPartials* unit, int inNumSamples) {

    float freq = IN0(1);
    int partials = (int)IN0(2);
    int mode = (int)IN0(4); // 0 = reject, 1 = select

    PV_GET_BUF

    float binWidthR = buf->samples / FULLRATE;
    //Convert bandwidth factor of freq to bins
    int bandwidth = (int)(freq * IN0(3) * binWidthR);
    int binStart, binEnd, currentPartial=1;

    Print("bandwidth: %i\n", bandwidth);

    CalculateBins;

    //SCPolarBuf *p = ToPolarApx(buf);
	SCComplexBuf *p = (SCComplexBuf*)buf->data;

    if (mode == 0) {
        for (int i = 0; i < numbins; ++i) {

            MaybeIncrementPartial(i);

            if (i >= binStart && i < binEnd) {
                //p->bin[i].mag =  p->bin[i].mag * 0.f;
                p->bin[i] = 0.f;
            }
            //If we haven't succeeded in incrementing, no need to continue loop
            if ( i > binEnd ) {
                break;
            }
        }
    } else {
        for (int i = 0; i < numbins; ++i) {

            MaybeIncrementPartial(i);

            if (i < binStart) {
                //p->bin[i].mag =  p->bin[i].mag * 0.f;
                p->bin[i] = 0.f;
            }
            //If we haven't succeeded in incrementing, no need to continue loop
            if ( i > binEnd ) {
                break;
            }
        }

        //Zero the rest of the partials
        for (int i = binEnd; i < numbins; ++i) {
            p->bin[i] = 0.f;
        }

        p->nyq = 0.f;
        p->dc = 0.f;
    }

}

void PV_SplitPartials_Ctor(PV_SplitPartials* unit) {
	SETCALC(PV_SplitPartials_next);
	ZOUT0(0) = ZIN0(0);
    ZOUT0(1) = ZIN0(1);
}

void PV_SplitPartials_next(PV_SplitPartials* unit, int inNumSamples) {

    float freq = IN0(2);
    int partials = (int)IN0(3);

    PV_GET_BUF2

    float binWidthR = buf1->samples / FULLRATE;
    //Convert bandwidth factor of freq to bins
    int bandwidth = (int)(freq * IN0(4) * binWidthR);
    int binStart, binEnd, currentPartial=1;

    CalculateBins;

	SCComplexBuf *p = (SCComplexBuf*)buf1->data;
	SCComplexBuf *q = (SCComplexBuf*)buf2->data;

    for (int i = 0; i < numbins; ++i) {

        MaybeIncrementPartial(i);
        if (i >= binStart && i < binEnd) {
            q->bin[i] = 0.f;
        } else {
            q->bin[i] = p->bin[i];
            p->bin[i] = 0.f;
        }
    }
}

PluginLoad(PVPlugins) {
    ft = inTable;

    DefinePVUnit(PV_FilterPartials)
    DefinePVUnit(PV_SplitPartials)
}
