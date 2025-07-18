#pragma once

#include <chrono>

enum class InterpolationType {
	Linear,
	easeInOutElastic
};

inline float easeInOutElastic(float x) {
    const float c5 = (2 * 3.14159) / 4.5f;
    if (x == 0.0f)
        return 0.0f;
    else if (x == 1.0f)
        return 1.0f;
    else if (x < 0.5f)
        return -0.5f * std::pow(2.0f, 20.0f * x - 10.0f) * std::sin((20.0f * x - 11.125f) * c5);
    else
        return 0.5f * std::pow(2.0f, -20.0f * x + 10.0f) * std::sin((20.0f * x - 11.125f) * c5) + 1.0f;
}

template<typename T>
struct Interpolated{
	T start{};
	T end{};

	float startTime{};
	float speed{2.0f};

	InterpolationType InterpolationType{InterpolationType::easeInOutElastic};

	explicit Interpolated(T const& initialValue = {}) : start(initialValue), end(start) {}

	[[nodiscard]]
	static float GetCurrentTime() {
		return std::chrono::duration<float>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	[[nodiscard]]
	float GetElapsedTime() const {
		float t = (GetCurrentTime() - startTime) * speed;
		switch(InterpolationType){
			case InterpolationType::Linear:
				return t;
			case InterpolationType::easeInOutElastic:
				return easeInOutElastic(t);
		}
	}

	void setValue(const T& value) {
		start = end;
		end = value;
		startTime = GetCurrentTime();
	}

	void setDuration(float duration) {
		if (duration <= 0.0f) {
			speed = 1.0f; // Default speed if duration is zero or negative
		} else {
			speed = 1.0f / duration; // Speed is inversely proportional to duration
		}
	}

	[[nodiscard]]
	T getValue() const {
		float elapsed = GetElapsedTime();
		if (elapsed <= 0.0f) return start;
		if (elapsed >= 1.0f) return end;
		return start + (end - start) * elapsed;
	}

	[[nodiscard]]
	operator T() const {
		return getValue();
	}

	void operator=(const T& value) {
		setValue(value);
	}
};