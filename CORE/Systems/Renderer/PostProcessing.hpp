#pragma once


namespace PostProcessing {
	void init();

	float getGamma();
	void setGamma(float gamma);
	void enableGammaCorrection(bool enabled);
	bool isGammaCorrectionEnabled();

	void renderPass();
}