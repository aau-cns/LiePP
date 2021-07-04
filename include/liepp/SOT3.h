/*
    This file is part of LiePP.

    LiePP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LiePP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LiePP.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "SO3.h"
#include "eigen3/Eigen/Dense"

template <typename _Scalar = double> class SOT3 {
  public:
    using Vector3S = Eigen::Matrix<_Scalar, 3, 1>;
    using VectorAlgS = Eigen::Matrix<_Scalar, 4, 1>;
    using Matrix3S = Eigen::Matrix<_Scalar, 3, 3>;
    using MatrixAlgS = Eigen::Matrix<_Scalar, 4, 4>;
    using SO3S = SO3<_Scalar>;

    static MatrixAlgS wedge(const VectorAlgS& u) {
        MatrixAlgS U = MatrixAlgS::Zero();
        U.template block<3,3>(0,0) = SO3S::skew(u.template segment<3>(0));
        U(3,3) = u(3);
        return U;
    }
    static VectorAlgS vee(const MatrixAlgS& U) {
        VectorAlgS u;
        u.template segment<3>(0) = SO3S::vex(U.template block<3, 3>(0,0));
        u(3) = U(3,3);
        return u;
    }
    static SOT3 exp(const VectorAlgS& w) {
        SOT3 result;
        result.R = SO3S::exp(w.template block<3, 1>(0, 0));
        result.a = std::exp(w(3));
        return result;
    }
    static VectorAlgS log(const SOT3& T) {
        VectorAlgS result;
        result.template block<3, 1>(0, 0) = SO3S::log(T.R);
        result(3) = std::log(T.a);
        return result;
    }
    static SOT3 Identity() {
        SOT3 Q;
        Q.setIdentity();
        return Q;
    }
    static SOT3 Random() { return SOT3( SO3S::Random(), exp(_Scalar(rand() / RAND_MAX)) ); }

    SOT3() = default;
    SOT3(const SOT3& other) = default;
    SOT3(const SO3S& R, const _Scalar& a) {
        this->R = R;
        this->a = a;
    }
    SOT3(const MatrixAlgS& mat) {
        R.fromMatrix(mat.template block<3, 3>(0, 0));
        a = mat(3, 3);
    }

    void setIdentity() {
        R.setIdentity();
        a = 1.0;
    }
    Vector3S operator*(const Vector3S& point) const { return a * (R * point); }
    SOT3 operator*(const SOT3& other) const { return SOT3(R * other.R, a * other.a); }
    Vector3S applyInverse(const Vector3S& p) const { return 1.0 / a * R.applyInverse(p); }

    void invert() {
        R.invert();
        a = 1.0 / a;
    }
    SOT3 inverse() const { return SOT3(R.inverse(), 1.0 / a); }

    // Set and get
    MatrixAlgS asMatrix() const {
        MatrixAlgS result = MatrixAlgS::Identity();
        result.template block<3, 3>(0, 0) = R.asMatrix();
        result(3, 3) = a;
        return result;
    }
    Matrix3S asMatrix3() const {
        Matrix3S result = a * R.asMatrix();
        return result;
    }

    SO3S R;
    _Scalar a;
};

typedef SOT3<double> SOT3d;
typedef SOT3<float> SOT3f;
typedef SOT3<Eigen::dcomplex> SOT3cd;
typedef SOT3<Eigen::scomplex> SOT3cf;