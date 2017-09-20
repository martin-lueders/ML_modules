#include "ML_modules.hpp"
#include <math.h>
#include "dsp.hpp"


struct ML_modulesPlugin : Plugin {
	ML_modulesPlugin() {
		slug = "ML Modules";
		name = "ML Modules";
		createModel<QuantizerWidget>(this, "Quantizer", "Quantizer");
		createModel<QuantumWidget>(this, "Quantum", "Quantum");
		createModel<TrigBufWidget>(this, "TrigBuf", "TrigBuf");
		// createModel<SEQGWidget>(this, "SEQ-Gate", "SEQ-Gate");
	}
};


Plugin *init() {
	return new ML_modulesPlugin();
}
