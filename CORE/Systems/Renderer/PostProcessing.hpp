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
	
}