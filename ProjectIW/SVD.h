#pragma once

namespace MDC
{
	class SMat3
	{
	public:
		float m00, m01, m02, m11, m12, m22;
	public:
		SMat3();
		SMat3(const float m00, const float m01, const float m02,
			const float m11, const float m12, const float m22);
		void clear();
		void setSymmetric(const float m00, const float m01, const float m02,
			const float m11,
			const float m12, const float m22);
		void setSymmetric(const SMat3 &rhs);
	private:
		SMat3(const SMat3 &rhs);
		SMat3 &operator=(const SMat3 &rhs);
	};
	class Mat3
	{
	public:
		float m00, m01, m02, m10, m11, m12, m20, m21, m22;
	public:
		Mat3();
		Mat3(const float m00, const float m01, const float m02,
			const float m10, const float m11, const float m12,
			const float m20, const float m21, const float m22);
		void clear();
		void set(const float m00, const float m01, const float m02,
			const float m10, const float m11, const float m12,
			const float m20, const float m21, const float m22);
		void set(const Mat3 &rhs);
		void setSymmetric(const float a00, const float a01, const float a02,
			const float a11, const float a12, const float a22);
		void setSymmetric(const SMat3 &rhs);
	private:
		Mat3(const Mat3 &rhs);
		Mat3 &operator=(const Mat3 &rhs);
	};
	class Vec3
	{
	public:
		float x, y, z;
	public:
		Vec3();
		Vec3(const float x, const float y, const float z);
		void clear();
		void set(const float x, const float y, const float z);
		void set(const Vec3 &rhs);
	private:
		Vec3(const Vec3 &rhs);
		Vec3 &operator=(const Vec3 &rhs);
	};

	class MatUtils
	{
	public:
		static float fnorm(const Mat3 &a);
		static float fnorm(const SMat3 &a);
		static float off(const Mat3 &a);
		static float off(const SMat3 &a);

	public:
		static void mmul(Mat3 &out, const Mat3 &a, const Mat3 &b);
		static void mmul_ata(SMat3 &out, const Mat3 &a);
		static void transpose(Mat3 &out, const Mat3 &a);
		static void vmul(Vec3 &out, const Mat3 &a, const Vec3 &v);
		static void vmul_symmetric(Vec3 &out, const SMat3 &a, const Vec3 &v);
	};
	class VecUtils
	{
	public:
		static void addScaled(Vec3 &v, const float s, const Vec3 &rhs);
		static float dot(const Vec3 &a, const Vec3 &b);
		static void normalize(Vec3 &v);
		static void scale(Vec3 &v, const float s);
		static void sub(Vec3 &c, const Vec3 &a, const Vec3 &b);
	};
	class Givens
	{
	public:
		static void rot01_post(Mat3 &m, const float c, const float s);
		static void rot02_post(Mat3 &m, const float c, const float s);
		static void rot12_post(Mat3 &m, const float c, const float s);
	};
	class Schur2
	{
	public:
		static void rot01(SMat3 &out, float &c, float &s);
		static void rot02(SMat3 &out, float &c, float &s);
		static void rot12(SMat3 &out, float &c, float &s);
	};
	class Svd
	{
	public:
		static void getSymmetricSvd(const SMat3 &a, SMat3 &vtav, Mat3 &v,
			const float tol, const int max_sweeps);
		static void pseudoinverse(Mat3 &out, const SMat3 &d, const Mat3 &v,
			const float tol);
		static float solveSymmetric(const SMat3 &A, const Vec3 &b, Vec3 &x,
			const float svd_tol, const int svd_sweeps, const float pinv_tol);
	};
	class LeastSquares
	{
	public:
		static float solveLeastSquares(const Mat3 &a, const Vec3 &b, Vec3 &x,
			const float svd_tol, const int svd_sweeps, const float pinv_tol);
	};
}
