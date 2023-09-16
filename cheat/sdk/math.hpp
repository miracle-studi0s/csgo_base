#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <intrin.h>

#include "../other/fast_memcpy.hpp"

#define RAD2DEG(radian) (float)radian * (180.f / (float)M_PI)
#define DEG2RAD(degree) (float)degree * ((float)M_PI / 180.f)

struct quaternion_t {
	float x{}, y{}, z{}, w{};
};

struct rect2_t {
	float w{}, h{};

	rect2_t() : w(0.f), h(0.f) {}
	rect2_t(const rect2_t& other) : w(other.w), h(other.h) {}
	rect2_t(const float& w, const float& h) : w(w), h(h) {}

	INLINE void reset() {
		w = 0.f;
		h = 0.f;
	}

	INLINE bool invalid() {
		return w != 0.f && h != 0.f;
	}
};

struct vec2_t {
	float x{}, y{};

	vec2_t() : x(0.f), y(0.f) {}
	vec2_t(const vec2_t& other) : x(other.x), y(other.y) {}
	vec2_t(const float& x, const float& y) : x(x), y(y) {}

	INLINE bool operator==(const vec2_t& other) {
		return x == other.x && y == other.y;
	}

	INLINE vec2_t operator+(const vec2_t& other) {
		return vec2_t(x + other.x, y + other.y);
	}

	INLINE vec2_t operator-(const vec2_t& other) {
		return vec2_t(x - other.x, y - other.y);
	}

	INLINE vec2_t operator/(const vec2_t& other) {
		return vec2_t(x / other.x, y / other.y);
	}

	INLINE vec2_t operator*(const vec2_t& other) {
		return vec2_t(x * other.x, y * other.y);
	}

	INLINE vec2_t operator+=(const vec2_t& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	INLINE vec2_t operator-=(const vec2_t& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	INLINE vec2_t operator/=(const vec2_t& other) {
		x /= other.x;
		y /= other.y;
		return *this;
	}

	INLINE vec2_t operator*=(const vec2_t& other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}

	INLINE float dot(const vec2_t& other) const {
		return x * other.x + y * other.y;
	}

	INLINE float dist_to(const vec2_t& other) const {
		return std::sqrtf(std::powf(other.x - x, 2) + std::powf(other.y - y, 2));
	}

	INLINE float length_sqr() const {
		return x * x + y * y;
	}

	INLINE float length() const {
		return std::sqrtf(length_sqr());
	}

	INLINE void reset() {
		x = y = 0.f;
	}
};

struct vec3_t {
	float x{}, y{}, z{};

	vec3_t() : x(0.f), y(0.f), z(0.f) {}
	vec3_t(const vec3_t& other) : x(other.x), y(other.y), z(other.z) {}
	vec3_t(const float& x, const float& y, const float& z) : x(x), y(y), z(z) {}

	INLINE float* base() {
		return ((float*)this);
	}

	INLINE float& operator[](int i) {
		return ((float*)this)[i];
	}

	INLINE float operator[](int i) const {
		return ((float*)this)[i];
	}

	INLINE bool operator==(const vec3_t& other) {
		return x == other.x && y == other.y && z == other.z;
	}

	INLINE bool operator!=(const vec3_t& other) {
		return x != other.x && y != other.y && z != other.z;
	}

	INLINE vec3_t operator+(const vec3_t& other) const {
		return vec3_t(x + other.x, y + other.y, z + other.z);
	}

	INLINE vec3_t operator-() const {
		return vec3_t(-x, -y, -z);
	}

	INLINE vec3_t operator-(const vec3_t& other) const {
		return vec3_t(x - other.x, y - other.y, z - other.z);
	}

	INLINE vec3_t operator/(const vec3_t& other) const {
		return vec3_t(x / other.x, y / other.y, z / other.z);
	}

	INLINE vec3_t operator*(const vec3_t& other) const {
		return vec3_t(x * other.x, y * other.y, z * other.z);
	}

	INLINE vec3_t operator*(const float& v) const {
		return vec3_t(x * v, y * v, z * v);
	}

	INLINE vec3_t operator+=(const vec3_t& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	INLINE vec3_t operator-=(const vec3_t& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	INLINE vec3_t operator/=(const vec3_t& other) {
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	INLINE vec3_t operator*=(const vec3_t& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	INLINE vec3_t operator*=(const float& fl) {
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}

	INLINE vec3_t operator/(const float& fl) {
		return vec3_t(x / fl, y / fl, z / fl);
	}

	INLINE vec3_t operator/=(const float& fl) {
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}

	INLINE void reset() {
		x = y = z = 0.f;
	}

	INLINE void reset_invalid() {
		if (std::isnan(x) || std::isnan(y) || std::isnan(z))
			reset();

		if (std::isinf(x) || std::isinf(y) || std::isinf(z))
			reset();
	}

	INLINE bool valid() {
		return length() > 0.f;
	}

	INLINE float dot(float* other) const {
		return x * other[0] + y * other[1] + z * other[2];
	}

	INLINE float dot(const vec3_t& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	INLINE vec3_t cross(const vec3_t& other) const {
		return vec3_t((y * other.z) - (z * other.y), (z * other.x) - (x * other.z), (x * other.y) - (y * other.x));
	}

	INLINE float length_sqr_2d() const {
		return x * x + y * y;
	}

	INLINE float length_sqr() const {
		return x * x + y * y + z * z;
	}

	INLINE float length_2d() const {
		return std::sqrtf(length_sqr_2d());
	}

	INLINE float length() const {
		return std::sqrtf(length_sqr());
	}

	INLINE float dist_to(const vec3_t& other) const {
		return std::sqrtf(std::powf(other.x - x, 2) + std::powf(other.y - y, 2) + std::powf(other.z - z, 2));
	}

	INLINE void multiply_angle(const vec3_t& start, float scale, const vec3_t& dir) {
		x = start.x + dir.x * scale;
		y = start.y + dir.y * scale;
		z = start.z + dir.z * scale;
	}

	INLINE vec3_t normalized() const {
		vec3_t out = *this;
		out.normalized_float();
		return out;
	}

	INLINE float normalized_float() {
		const float vec_length = length();
		const float radius = 1.0f / (vec_length + FLT_EPSILON);

		x *= radius;
		y *= radius;
		z *= radius;

		return vec_length;
	}

	INLINE vec3_t normalized_angle() {
		auto vec = *this;
		while (vec.y < -180.f)
			vec.y += 360.f;
		while (vec.y > 180.f)
			vec.y -= 360.f;

		if (vec.x > 89.f)
			vec.x = 89.f;
		if (vec.x < -89.f)
			vec.x = -89.f;

		return vec;
	}
};

struct vec4_t {
	float x, y, w, h;

	vec4_t() : x(0.f), y(0.f), w(0.f), h(0.f) {}
	vec4_t(const vec4_t& other) : x(other.x), y(other.y), w(other.w), h(other.h) {}
	vec4_t(const float& x, const float& y, const float& w, const float& h) : x(x), y(y), w(w), h(h) {}

	INLINE vec4_t operator*=(const vec4_t& other) {
		x *= other.x;
		y *= other.y;
		w *= other.w;
		h *= other.h;
		return *this;
	}

	INLINE float* base() {
		return reinterpret_cast<float*>(this);
	}
};

struct vertex_t {
	vec2_t position{};
	vec2_t coords{};

	vertex_t() {}
	vertex_t(const vec2_t& position, const vec2_t& coords = vec2_t(0, 0)) : position(position), coords(coords) {}
};

struct matrix3x4_t {
	float mat[3][4]{};

	matrix3x4_t() = default;

	INLINE vec3_t at(int i) const {
		return vec3_t{ mat[0][i], mat[1][i], mat[2][i] };
	}

	INLINE float* operator[](int i) {
		return mat[i];
	}

	INLINE const float* operator[](int i) const {
		return mat[i];
	}

	INLINE float* base() {
		return &mat[0][0];
	}

	INLINE const float* base() const {
		return &mat[0][0];
	}

	INLINE matrix3x4_t(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23) {
		mat[0][0] = m00;
		mat[0][1] = m01;
		mat[0][2] = m02;
		mat[0][3] = m03;
		mat[1][0] = m10;
		mat[1][1] = m11;
		mat[1][2] = m12;
		mat[1][3] = m13;
		mat[2][0] = m20;
		mat[2][1] = m21;
		mat[2][2] = m22;
		mat[2][3] = m23;
	}

	INLINE void set_origin(const vec3_t& p) {
		mat[0][3] = p.x;
		mat[1][3] = p.y;
		mat[2][3] = p.z;
	}

	INLINE void scale(int i, float scale) {
		mat[i][0] *= scale;
		mat[i][1] *= scale;
		mat[i][2] *= scale;
	}

	INLINE vec3_t get_origin() {
		return vec3_t{ mat[0][3], mat[1][3], mat[2][3] };
	}

	INLINE void quaternion_matrix(const quaternion_t& q) {
		mat[0][0] = 1.f - 2.f * q.y * q.y - 2.f * q.z * q.z;
		mat[1][0] = 2.f * q.x * q.y + 2.f * q.w * q.z;
		mat[2][0] = 2.f * q.x * q.z - 2.f * q.w * q.y;

		mat[0][1] = 2.f * q.x * q.y - 2.f * q.w * q.z;
		mat[1][1] = 1.f - 2.f * q.x * q.x - 2.f * q.z * q.z;
		mat[2][1] = 2.f * q.y * q.z + 2.f * q.w * q.x;

		mat[0][2] = 2.f * q.x * q.z + 2.f * q.w * q.y;
		mat[1][2] = 2.f * q.y * q.z - 2.f * q.w * q.x;
		mat[2][2] = 1.f - 2.f * q.x * q.x - 2.f * q.y * q.y;

		mat[0][3] = 0.f;
		mat[1][3] = 0.f;
		mat[2][3] = 0.f;
	}

	INLINE void quaternion_matrix(const quaternion_t& q, const vec3_t& pos) {
		quaternion_matrix(q);

		mat[0][3] = pos.x;
		mat[1][3] = pos.y;
		mat[2][3] = pos.z;
	}

	INLINE matrix3x4_t contact_transforms(matrix3x4_t in) const {
		matrix3x4_t out = { };

		out[0][0] = mat[0][0] * in[0][0] + mat[0][1] * in[1][0] + mat[0][2] * in[2][0];
		out[0][1] = mat[0][0] * in[0][1] + mat[0][1] * in[1][1] + mat[0][2] * in[2][1];
		out[0][2] = mat[0][0] * in[0][2] + mat[0][1] * in[1][2] + mat[0][2] * in[2][2];
		out[0][3] = mat[0][0] * in[0][3] + mat[0][1] * in[1][3] + mat[0][2] * in[2][3] + mat[0][3];
		out[1][0] = mat[1][0] * in[0][0] + mat[1][1] * in[1][0] + mat[1][2] * in[2][0];
		out[1][1] = mat[1][0] * in[0][1] + mat[1][1] * in[1][1] + mat[1][2] * in[2][1];
		out[1][2] = mat[1][0] * in[0][2] + mat[1][1] * in[1][2] + mat[1][2] * in[2][2];
		out[1][3] = mat[1][0] * in[0][3] + mat[1][1] * in[1][3] + mat[1][2] * in[2][3] + mat[1][3];
		out[2][0] = mat[2][0] * in[0][0] + mat[2][1] * in[1][0] + mat[2][2] * in[2][0];
		out[2][1] = mat[2][0] * in[0][1] + mat[2][1] * in[1][1] + mat[2][2] * in[2][1];
		out[2][2] = mat[2][0] * in[0][2] + mat[2][1] * in[1][2] + mat[2][2] * in[2][2];
		out[2][3] = mat[2][0] * in[0][3] + mat[2][1] * in[1][3] + mat[2][2] * in[2][3] + mat[2][3];

		return out;
	}

	INLINE void angle_matrix(const vec3_t& angles) {
		float sr, sp, sy, cr, cp, cy;

		sy = std::sin(DEG2RAD(angles.y));
		cy = std::cos(DEG2RAD(angles.y));

		sp = std::sin(DEG2RAD(angles.x));
		cp = std::cos(DEG2RAD(angles.x));

		sr = std::sin(DEG2RAD(angles.z));
		cr = std::cos(DEG2RAD(angles.z));

		mat[0][0] = cp * cy;
		mat[1][0] = cp * sy;
		mat[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		mat[0][1] = sp * srcy - crsy;
		mat[1][1] = sp * srsy + crcy;
		mat[2][1] = sr * cp;

		mat[0][2] = (sp * crcy + srsy);
		mat[1][2] = (sp * crsy - srcy);
		mat[2][2] = cr * cp;

		mat[0][3] = 0.0f;
		mat[1][3] = 0.0f;
		mat[2][3] = 0.0f;
	}

	INLINE void multiply(const matrix3x4_t& other) {
		mat[0][0] = mat[0][0] * other[0][0] + mat[0][1] * other[1][0] + mat[0][2] * other[2][0];
		mat[0][1] = mat[0][0] * other[0][1] + mat[0][1] * other[1][1] + mat[0][2] * other[2][1];
		mat[0][2] = mat[0][0] * other[0][2] + mat[0][1] * other[1][2] + mat[0][2] * other[2][2];
		mat[0][3] = mat[0][0] * other[0][3] + mat[0][1] * other[1][3] + mat[0][2] * other[2][3] + mat[0][3];
		mat[1][0] = mat[1][0] * other[0][0] + mat[1][1] * other[1][0] + mat[1][2] * other[2][0];
		mat[1][1] = mat[1][0] * other[0][1] + mat[1][1] * other[1][1] + mat[1][2] * other[2][1];
		mat[1][2] = mat[1][0] * other[0][2] + mat[1][1] * other[1][2] + mat[1][2] * other[2][2];
		mat[1][3] = mat[1][0] * other[0][3] + mat[1][1] * other[1][3] + mat[1][2] * other[2][3] + mat[1][3];
		mat[2][0] = mat[2][0] * other[0][0] + mat[2][1] * other[1][0] + mat[2][2] * other[2][0];
		mat[2][1] = mat[2][0] * other[0][1] + mat[2][1] * other[1][1] + mat[2][2] * other[2][1];
		mat[2][2] = mat[2][0] * other[0][2] + mat[2][1] * other[1][2] + mat[2][2] * other[2][2];
		mat[2][3] = mat[2][0] * other[0][3] + mat[2][1] * other[1][3] + mat[2][2] * other[2][3] + mat[2][3];
	}

	INLINE void angle_matrix(const vec3_t& angles, const vec3_t& position) {
		angle_matrix(angles);
		set_origin(position);
	}
};

struct __declspec(align(16)) vec3_aligned_t : vec3_t {
	vec3_aligned_t() {}
	vec3_aligned_t(const vec3_t& other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
	}

	vec3_aligned_t(vec3_t other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
	}

	vec3_aligned_t operator=(vec3_t other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}
};

namespace math {
	template <typename T>
	static INLINE T lerp(const T& from, const T& to, const float percent) {
		return to * percent + from * (1.f - percent);
	}

	template <typename T>
	INLINE T interpolate_inversed(float percent, const T& A, const T& B) {
		return A + (B - A) * percent;
	}

	template <typename T>
	static INLINE T hermit_spline(T p1, T p2, T d1, T d2, float t) {
		const auto t_sqr = t * t;
		const auto t_cube = t * t_sqr;

		const auto b1 = 2.0f * t_cube - 3.0f * t_sqr + 1.0f;
		const auto b2 = 1.0f - b1; // -2 * t_cube + 3 * t_sqr;
		const auto b3 = t_cube - 2 * t_sqr + t;
		const auto b4 = t_cube - t_sqr;

		T output;
		output = p1 * b1;
		output += p2 * b2;
		output += d1 * b3;
		output += d2 * b4;

		return output;
	}

	template <typename T>
	static __forceinline T hermit_spline(T p0, T p1, T p2, float t) {
		return hermit_spline(p1, p2, p1 - p0, p2 - p1, t);
	}

	extern vec3_t vector_rotate(vec3_t& in1, matrix3x4_t& in2);
	extern vec3_t vector_rotate(vec3_t& in1, vec3_t& in2);
	extern void matrix_copy(const matrix3x4_t& source, matrix3x4_t& target);
	extern void matrix_multiply(matrix3x4_t& in1, const matrix3x4_t& in2);
	extern void rotate_matrix(matrix3x4_t* in, matrix3x4_t* out, float delta, vec3_t origin);
	extern void vector_angles(const vec3_t& forward, const vec3_t& pseudoup, vec3_t& angles);
	extern void vector_angles(const vec3_t& forward, vec3_t& angles);
	extern void angle_vectors(const vec3_t& angles, vec3_t& forward);
	extern void angle_vectors(const vec3_t& angles, vec3_t& forward, vec3_t& right, vec3_t& up);
	extern float normalize_yaw(float yaw);
	extern float angle_diff(float dst, float src);
	extern void vector_transform(const vec3_t& in1, const matrix3x4_t& in2, vec3_t& out);
	extern void change_bones_position(matrix3x4_t* bones, size_t msize, const vec3_t& current_position, const vec3_t& new_position);
	extern vec3_t calculate_angle(vec3_t src, vec3_t dst);
	extern float random_float(float a, float b);
	extern int random_int(int a, int b);
	extern float segment_to_segment(const vec3_t s1, const vec3_t s2, const vec3_t k1, const vec3_t k2);
	extern vec3_t clamp_angles(const vec3_t& angle);
	extern float approach(float target, float value, float speed);
	extern vec3_t approach(vec3_t target, vec3_t value, float speed);
	extern float approach_angle(float target, float value, float speed);

	INLINE void memcpy_sse(void* dest, const void* src, std::size_t count) {
		__movsb(static_cast<BYTE*>(dest), static_cast<const BYTE*>(src), count);
	}

	INLINE float smoothstep_bounds(float edge0, float edge1, float x) {
		x = std::clamp<float>((x - edge0) / (edge1 - edge0), 0, 1);
		return x * x * (3 - 2 * x);
	}
	 
	INLINE float clamp_cycle(float cycle_in) {
		cycle_in -= static_cast<int>(cycle_in);

		if (cycle_in < 0) {
			cycle_in += 1;
		}
		else if (cycle_in > 1) {
			cycle_in -= 1;
		}

		return cycle_in;
	}
}