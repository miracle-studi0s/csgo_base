#include "../main_definitions.hpp"
#include "../security/security.hpp"
#include "math.hpp"

#include <DirectXMath.h>
#include <random>
#include <xmmintrin.h>

typedef __declspec(align(16)) union
{
	float f[4];
	__m128 v;
} m128;

namespace math {
	inline vec3_t vector_rotate(vec3_t& in1, matrix3x4_t& in2) {
		return { in1.dot(in2[0]), in1.dot(in2[1]), in1.dot(in2[2]) };
	}

	inline vec3_t vector_rotate(vec3_t& in1, vec3_t& in2) {
		matrix3x4_t m;
		m.angle_matrix(in2);
		return vector_rotate(in1, m);
	}

	inline void matrix_copy(const matrix3x4_t& source, matrix3x4_t& target) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 4; j++)
				target[i][j] = source[i][j];
	}

	inline void matrix_multiply(matrix3x4_t& in1, const matrix3x4_t& in2) {
		matrix3x4_t out;
		if (&in1 == &out) {
			matrix3x4_t in1b;
			matrix_copy(in1, in1b);
			matrix_multiply(in1b, in2);
			return;
		}

		if (&in2 == &out) {
			matrix3x4_t in2b;
			matrix_copy(in2, in2b);
			matrix_multiply(in1, in2b);
			return;
		}

		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
			in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
			in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
			in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
			in1[0][2] * in2[2][3] + in1[0][3];
		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
			in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
			in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
			in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
			in1[1][2] * in2[2][3] + in1[1][3];
		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
			in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
			in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
			in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
			in1[2][2] * in2[2][3] + in1[2][3];

		in1 = out;
	}

	inline void rotate_matrix(matrix3x4_t* in, matrix3x4_t* out, float delta, vec3_t origin) {
		auto delta__ = vec3_t{ 0.f, delta, 0.f };
		vec3_t out_pos{};
		for (int i = 0; i < 128; i++) {
			out[i].angle_matrix(delta__);
			matrix_multiply(out[i], in[i]);
			auto bone_position = vec3_t{ in[i][0][3],
				in[i][1][3],
				in[i][2][3] } - origin;
			out_pos = vector_rotate(bone_position, delta__);
			out_pos += origin;
			out[i][0][3] = out_pos.x;
			out[i][1][3] = out_pos.y;
			out[i][2][3] = out_pos.z;
		}
	}

	inline void vector_angles(const vec3_t& forward, const vec3_t& pseudoup, vec3_t& angles) {
		vec3_t left = pseudoup.cross(forward);
		left = left.normalized();

		float forward_dist = forward.length_2d();
		if (forward_dist > 0.001f) {
			angles.x = (float)(atan2f(-forward.z, forward_dist) * 180.0 / M_PI);
			angles.y = (float)(atan2f(forward.y, forward.x) * 180.0 / M_PI);
			angles.z = (float)(atan2f(left.z, (left.y * forward.x) - (left.x * forward.y)) * 180.0 / M_PI);
		}
		else {
			angles.x = (float)(atan2f(-forward.z, forward_dist) * 180 / M_PI);
			angles.y = (float)(atan2f(-left.x, left.y) * 180.0 / M_PI);
			angles.z = 0.f;
		}
	}

	inline void vector_angles(const vec3_t& forward, vec3_t& angles) {
		float tmp, yaw, pitch;

		if (forward.y == 0.f && forward.x == 0.f) {
			yaw = 0.f;
			if (forward.z > 0)
				pitch = 270.f;
			else
				pitch = 90.f;
		}
		else {
			yaw = (float)(atan2(forward.y, forward.x) * 180.0 / M_PI);
			if (yaw < 0.f)
				yaw += 360.f;

			tmp = sqrt(forward.x * forward.x + forward.y * forward.y);
			pitch = (float)(atan2(-forward.z, tmp) * 180.0 / M_PI);
			if (pitch < 0.f)
				pitch += 360.f;
		}

		angles.x = pitch;
		angles.y = yaw;
		angles.z = 0.f;
	}

	inline void angle_vectors(const vec3_t& angles, vec3_t& forward) {
		float sx{}, sy{}, cx{}, cy{};

		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles.y));
		DirectX::XMScalarSinCos(&sx, &cx, DEG2RAD(angles.x));

		forward.x = cx * cy;
		forward.y = cx * sy;
		forward.z = -sx;
	}

	inline void angle_vectors(const vec3_t& angles, vec3_t& forward, vec3_t& right, vec3_t& up) {
		float sr, sp, sy, cr, cp, cy;

		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles.y));
		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles.x));
		DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles.z));

		forward.x = (cp * cy);
		forward.y = (cp * sy);
		forward.z = (-sp);
		right.x = (-1.f * sr * sp * cy + -1.f * cr * -sy);
		right.y = (-1.f * sr * sp * sy + -1.f * cr * cy);
		right.z = (-1.f * sr * cp);
		up.x = (cr * sp * cy + -sr * -sy);
		up.y = (cr * sp * sy + -sr * cy);
		up.z = (cr * cp);
	}

	inline float normalize_yaw(float yaw) {
		yaw = std::fmodf(yaw, 360.0f);
		if (yaw > 180)
			yaw -= 360;
		if (yaw < -180)
			yaw += 360;
		return yaw;
	}

	inline float angle_diff(float dst, float src) {
		float delta = dst - src;

		if (delta < -180)
			delta += 360;
		else if (delta > 180)
			delta -= 360;

		return delta;
	}

	inline void vector_transform(const vec3_t& in1, const matrix3x4_t& in2, vec3_t& out) {
		out = {
			in1.dot({ in2[0][0], in2[0][1], in2[0][2] }) + in2[0][3],
			in1.dot({ in2[1][0], in2[1][1], in2[1][2] }) + in2[1][3],
			in1.dot({ in2[2][0], in2[2][1], in2[2][2] }) + in2[2][3]
		};
	}

	inline void change_bones_position(matrix3x4_t* bones, size_t msize, const vec3_t& current_position, const vec3_t& new_position) {
		const auto delta = new_position - current_position;

		for (size_t i = 0; i < msize; ++i) {
			bones[i][0][3] += delta.x;
			bones[i][1][3] += delta.y;
			bones[i][2][3] += delta.z;
		}
	}

	inline vec3_t calculate_angle(vec3_t src, vec3_t dst) {
		vec3_t angles{};

		vec3_t delta = src - dst;
		float hyp = delta.length_2d();

		angles.y = std::atanf(delta.y / delta.x) * 57.2957795131f;
		angles.x = std::atanf(-delta.z / hyp) * -57.2957795131f;
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}

	inline float segment_to_segment(const vec3_t s1, const vec3_t s2, const vec3_t k1, const vec3_t k2) {
		static auto constexpr epsilon = 0.00000001;

		auto u = s2 - s1;
		auto v = k2 - k1;
		const auto w = s1 - k1;

		const auto a = u.dot(u);
		const auto b = u.dot(v);
		const auto c = v.dot(v);
		const auto d = u.dot(w);
		const auto e = v.dot(w);
		const auto D = a * c - b * b;
		float sn, sd = D;
		float tn, td = D;

		if (D < epsilon) {
			sn = 0.0;
			sd = 1.0;
			tn = e;
			td = c;
		}
		else {
			sn = b * e - c * d;
			tn = a * e - b * d;

			if (sn < 0.0) {
				sn = 0.0;
				tn = e;
				td = c;
			}
			else if (sn > sd) {
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if (tn < 0.0) {
			tn = 0.0;

			if (-d < 0.0)
				sn = 0.0;
			else if (-d > a)
				sn = sd;
			else {
				sn = -d;
				sd = a;
			}
		}
		else if (tn > td) {
			tn = td;

			if (-d + b < 0.0)
				sn = 0;
			else if (-d + b > a)
				sn = sd;
			else {
				sn = -d + b;
				sd = a;
			}
		}

		const float sc = abs(sn) < epsilon ? 0.0f : sn / sd;
		const float tc = abs(tn) < epsilon ? 0.0f : tn / td;

		m128 n{};
		auto dp = w + u * sc - v * tc;
		n.f[0] = dp.dot(dp);
		const auto calc = _mm_sqrt_ps(n.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}

	inline vec3_t clamp_angles(const vec3_t& angle) {
		vec3_t ret = angle;
		ret.x = std::clamp(ret.x, -89.f, 89.f);
		ret.z = std::clamp(ret.z, -50.f, 50.f);

		while (ret.y < -180.f)
			ret.y += 360.f;
		while (ret.y > 180.f)
			ret.y -= 360.f;

		return ret;
	}

	class c_random_generator {
	public:
		template <std::integral T>
		[[nodiscard]] static T random(T min, T max) noexcept {
			std::scoped_lock lock{ mutex };
			return std::uniform_int_distribution{ min, max }(gen);
		}

		template <std::floating_point T>
		[[nodiscard]] static T random(T min, T max) noexcept {
			std::scoped_lock lock{ mutex };
			return std::uniform_real_distribution{ min, max }(gen);
		}

		template <typename T>
		[[nodiscard]] static std::enable_if_t<std::is_enum_v<T>, T> random(T min, T max) noexcept {
			return static_cast<T>(random(static_cast<std::underlying_type_t<T>>(min), static_cast<std::underlying_type_t<T>>(max)));
		}
	private:
		inline static std::mt19937 gen{ std::random_device{}() };
		inline static std::mutex mutex;
	};

	inline float random_float(float a, float b) {
		return c_random_generator::random(a, b);
	}

	inline int random_int(int a, int b) {
		return c_random_generator::random(a, b);
	}

	inline float approach(float target, float value, float speed) {
		float delta = target - value;

		if (delta > speed)
			value += speed;
		else if (delta < -speed)
			value -= speed;
		else
			value = target;

		return value;
	}

	inline vec3_t approach(vec3_t target, vec3_t value, float speed) {
		vec3_t diff = (target - value);
		float delta = diff.length();

		if (delta > speed)
			value += diff.normalized() * speed;
		else if (delta < -speed)
			value -= diff.normalized() * speed;
		else
			value = target;

		return value;
	}

	inline float anglemod(float a) {
		a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
		return a;
	}

	inline float approach_angle(float target, float value, float speed) {
		target = anglemod(target);
		value = anglemod(value);

		float delta = target - value;

		// Speed is assumed to be positive
		if (speed < 0)
			speed = -speed;

		if (delta < -180)
			delta += 360;
		else if (delta > 180)
			delta -= 360;

		if (delta > speed)
			value += speed;
		else if (delta < -speed)
			value -= speed;
		else
			value = target;

		return value;
	}
}