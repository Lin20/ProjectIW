#pragma once

#include "SimpleMath.h"
#include "SVD.h"

using namespace DirectX::SimpleMath;

namespace MDC
{
	struct QEFData
	{
		float ata_00, ata_01, ata_02, ata_11, ata_12, ata_22;
		float atb_x, atb_y, atb_z;
		float btb;
		float massPoint_x, massPoint_y, massPoint_z;
		int numPoints;

		QEFData();

		QEFData(const float ata_00, const float ata_01,
			const float ata_02, const float ata_11, const float ata_12,
			const float ata_22, const float atb_x, const float atb_y,
			const float atb_z, const float btb, const float massPoint_x,
			const float massPoint_y, const float massPoint_z,
			const int numPoints);

		void add(const QEFData& rhs);

		void clear();

		void set(const float ata_00, const float ata_01,
			const float ata_02, const float ata_11, const float ata_12,
			const float ata_22, const float atb_x, const float atb_y,
			const float atb_z, const float btb, const float massPoint_x,
			const float massPoint_y, const float massPoint_z,
			const int numPoints);

		void set(const QEFData& rhs);

		QEFData(const QEFData& rhs);
		QEFData &operator= (const QEFData& rhs);
	};

	class QEFSolver
	{
	public:
		QEFSolver();
		QEFSolver(const QEFSolver&);

		const Vec3& getMassPoint() const { return massPoint; }

		void add(const float px, const float py, const float pz,
			float nx, float ny, float nz);
		void add(const Vec3 &p, const Vec3 &n);
		void add(const QEFData &rhs);
		QEFData getData();
		float getError();
		float getError(const Vec3 &pos);
		void reset();
		float solve(Vec3 &outx, const float svd_tol,
			const int svd_sweeps, const float pinv_tol);

	private:

		QEFData data;
		SMat3 ata;
		Vec3 atb, massPoint, x;
		bool hasSolution;

		void setAta();
		void setAtb();
	};
}
