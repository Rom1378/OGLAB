#pragma once

namespace PostProcessing {
	void init();
	void renderPass();

	float getExposure();
	void setExposure(float exp);
	void enableHDR(bool enabled);
	bool isHDREnabled();
	
	float getGamma();
	void setGamma(float gamma);

	float getBloomThreshold();
	void setBloomThreshold(float threshold);

	float getBloomIntensity();
	void setBloomIntensity(float intensity);
	int getBloomBlurPasses();
	void setBloomBlurPasses(int passes);

	void rescale(float width, float height);
}