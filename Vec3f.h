#ifndef VEC3F_H
#define VEC3F_H


struct Vec3f {
	float x;
	float y;
	float z;

	Vec3f() : x(0), y(0), z(0) {}
	Vec3f(float a) : x(a), y(a), z(a) {}
	Vec3f(float x2, float y2, float z2) : x(x2), y(y2), z(z2) {}

	Vec3f operator + (const Vec3f v) {
		Vec3f result;
		result.x = x + v.x;
		result.y = y + v.y;
		result.z = z + v.z;
		return result;
	}

	Vec3f operator - (const Vec3f v) {
		Vec3f result;
		result.x = x - v.x;
		result.y = y - v.y;
		result.z = z - v.z;
		return result;
	}

	Vec3f operator * (const Vec3f v) {
		Vec3f result;
		result.x = x * v.x;
		result.y = y * v.y;
		result.z = z * v.z;
		return result;
	}

	Vec3f operator * (const float c) {
		Vec3f result;
		result.x = x * c;
		result.y = y * c;
		result.z = z * c;
		return result;
	}
	
	Vec3f operator / (const float c) {
		Vec3f result;
		result.x = x / c;
		result.y = y / c;
		result.z = z / c;
		return result;
	}

	float dot(Vec3f v) {
		return (x * v.x) + (y * v.y) + (z * v.z);
	}

	void normalize() {
		float magnitude = sqrt((x*x) + (y*y) + (z*z));
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}

	float calculateMagnitude() {
		float magnitude = sqrt((x*x) + (y*y) + (z*z));
		return magnitude;
	}

	void setAll(float val) {
		x = val;
		y = val;
		z = val;
	}

	void setEach(float xNew, float yNew, float zNew) {
		x = xNew;
		y = yNew;
		z = zNew;
	}

};


#endif
