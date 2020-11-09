PV_FilterPartials : PV_ChainUGen {
	*new { arg buffer, freq=440.0, partials=8, bandwidth=0.33, mode=\reject;

		mode = switch(mode,
			\reject, 0,
			\select, 1
		);

		^this.multiNew('control', buffer, freq, partials, bandwidth, mode);
	}
}

PV_SplitPartials : MultiOutUGen {
	*new { arg bufferA, bufferB, freq=440.0, partials=8, bandwidth=0.33;
		^this.multiNew('control', bufferA, bufferB, freq, partials, bandwidth);
	}

	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}

