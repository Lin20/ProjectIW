#include "QEFSolver.h"
#include <stdexcept>

namespace MDC
{
	QEFData::QEFData()
	{
		this->clear();
	}

	QEFData::QEFData(const float ata_00, const float ata_01,
		const float ata_02, const float ata_11, const float ata_12,
		const float ata_22, const float atb_x, const float atb_y,
		const float atb_z, const float btb, const float massPoint_x,
		const float massPoint_y, const float massPoint_z,
		const int numPoints)
	{
		this->set(ata_00, ata_01, ata_02, ata_11, ata_12, ata_22, atb_x, atb_y,
			atb_z, btb, massPoint_x, massPoint_y, massPoint_z, numPoints);
	}

	QEFData::QEFData(const QEFData &rhs)
	{
		this->set(rhs);
	}

	QEFData& QEFData::operator=(const QEFData& rhs)
	{
		this->set(rhs);
		return *this;
	}

	void QEFData::add(const QEFData &rhs)
	{
		this->ata_00 += rhs.ata_00;
		this->ata_01 += rhs.ata_01;
		this->ata_02 += rhs.ata_02;
		this->ata_11 += rhs.ata_11;
		this->ata_12 += rhs.ata_12;
		this->ata_22 += rhs.ata_22;
		this->atb_x += rhs.atb_x;
		this->atb_y += rhs.atb_y;
		this->atb_z += rhs.atb_z;
		this->btb += rhs.btb;
		this->massPoint_x += rhs.massPoint_x;
		this->massPoint_y += rhs.massPoint_y;
		this->massPoint_z += rhs.massPoint_z;
		this->numPoints += rhs.numPoints;
	}

	void QEFData::clear()
	{
		this->set(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}

	void QEFData::set(const float ata_00, const float ata_01,
		const float ata_02, const float ata_11, const float ata_12,
		const float ata_22, const float atb_x, const float atb_y,
		const float atb_z, const float btb, const float massPoint_x,
		const float massPoint_y, const float massPoint_z,
		const int numPoints)
	{
		this->ata_00 = ata_00;
		this->ata_01 = ata_01;
		this->ata_02 = ata_02;
		this->ata_11 = ata_11;
		this->ata_12 = ata_12;
		this->ata_22 = ata_22;
		this->atb_x = atb_x;
		this->atb_y = atb_y;
		this->atb_z = atb_z;
		this->btb = btb;
		this->massPoint_x = massPoint_x;
		this->massPoint_y = massPoint_y;
		this->massPoint_z = massPoint_z;
		this->numPoints = numPoints;
	}

	void QEFData::set(const QEFData &rhs)
	{
		this->set(rhs.ata_00, rhs.ata_01, rhs.ata_02, rhs.ata_11, rhs.ata_12,
			rhs.ata_22, rhs.atb_x, rhs.atb_y, rhs.atb_z, rhs.btb,
			rhs.massPoint_x, rhs.massPoint_y, rhs.massPoint_z,
			rhs.numPoints);
	}

	QEFSolver::QEFSolver() : data(), ata(), atb(), massPoint(), x(),
		hasSolution(false) {}

	QEFSolver::QEFSolver(const QEFSolver& q) : data(), ata(), atb(), massPoint(), x(),
		hasSolution(false)
	{
		memcpy(this, &q, sizeof(q));
		
		this->data.set(q.data);
	}

	static void normalize(float &nx, float &ny, float &nz)
	{
		Vec3 tmpv(nx, ny, nz);
		VecUtils::normalize(tmpv);
		nx = tmpv.x;
		ny = tmpv.y;
		nz = tmpv.z;
	}

	void QEFSolver::add(const float px, const float py, const float pz,
		float nx, float ny, float nz)
	{
		this->hasSolution = false;
		normalize(nx, ny, nz);
		this->data.ata_00 += nx * nx;
		this->data.ata_01 += nx * ny;
		this->data.ata_02 += nx * nz;
		this->data.ata_11 += ny * ny;
		this->data.ata_12 += ny * nz;
		this->data.ata_22 += nz * nz;
		const float dot = nx * px + ny * py + nz * pz;
		this->data.atb_x += dot * nx;
		this->data.atb_y += dot * ny;
		this->data.atb_z += dot * nz;
		this->data.btb += dot * dot;
		this->data.massPoint_x += px;
		this->data.massPoint_y += py;
		this->data.massPoint_z += pz;
		++this->data.numPoints;
	}

	void QEFSolver::add(const Vec3 &p, const Vec3 &n)
	{
		this->add(p.x, p.y, p.z, n.x, n.y, n.z);
	}

	void QEFSolver::add(const QEFData &rhs)
	{
		this->hasSolution = false;
		this->data.add(rhs);
	}

	QEFData QEFSolver::getData()
	{
		return data;
	}

	float QEFSolver::getError()
	{
		if (!this->hasSolution) {
			throw std::runtime_error("illegal state");
		}

		return this->getError(this->x);
	}

	float QEFSolver::getError(const Vec3 &pos)
	{
		if (!this->hasSolution) {
			this->setAta();
			this->setAtb();
		}

		Vec3 atax;
		MatUtils::vmul_symmetric(atax, this->ata, pos);
		return VecUtils::dot(pos, atax) - 2 * VecUtils::dot(pos, this->atb)
			+ this->data.btb;
	}

	void QEFSolver::reset()
	{
		this->hasSolution = false;
		this->data.clear();
	}

	void QEFSolver::setAta()
	{
		this->ata.setSymmetric(this->data.ata_00, this->data.ata_01,
			this->data.ata_02, this->data.ata_11, this->data.ata_12,
			this->data.ata_22);
	}

	void QEFSolver::setAtb()
	{
		this->atb.set(this->data.atb_x, this->data.atb_y, this->data.atb_z);
	}

	float QEFSolver::solve(Vec3 &outx, const float svd_tol,
		const int svd_sweeps, const float pinv_tol)
	{
		if (this->data.numPoints == 0) {
			throw std::invalid_argument("...");
		}

		this->massPoint.set(this->data.massPoint_x, this->data.massPoint_y,
			this->data.massPoint_z);
		VecUtils::scale(this->massPoint, 1.0f / this->data.numPoints);
		this->setAta();
		this->setAtb();
		Vec3 tmpv;
		MatUtils::vmul_symmetric(tmpv, this->ata, this->massPoint);
		VecUtils::sub(this->atb, this->atb, tmpv);
		this->x.clear();
		const float result = Svd::solveSymmetric(this->ata, this->atb,
			this->x, svd_tol, svd_sweeps, pinv_tol);
		if (isnan(result))
			this->x.set(this->massPoint);
		else
			VecUtils::addScaled(this->x, 1, this->massPoint);
		this->setAtb();
		outx.set(x);
		this->hasSolution = true;
		return result;
	}
}