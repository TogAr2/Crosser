#include "renderUtils.hpp"

float interpolate(float start, float end, float alpha) {
	return start + (end - start) * alpha;
}

int interpolate(int start, int end, float alpha) {
	return start + (int) ((float) (end - start) * alpha);
}
