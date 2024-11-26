#include "matrix.h"

matrix::matrix(int n, int m) {
	M = vector<vector<double>>(n, vector<double>(m, 0));
}
matrix::matrix(const vector<vector<double>>& _M) {
	M = _M;
}
matrix::matrix(const matrix& _matrix) {
	M = _matrix.M;
}
matrix matrix::operator+(const matrix& o) const {
	assert(h() == o.h() && w() == o.w());
	matrix ans(h(), w());
	for (int i = 0; i < h(); i++) {
		for (int j = 0; j < w(); j++)
			ans[i][j] = M[i][j] + o.M[i][j];
	}
	return ans;
}
matrix matrix::operator-(const matrix& o) const {
	assert(h() == o.h() && w() == o.w());
	matrix ans(h(), w());
	for (int i = 0; i < h(); i++) {
		for (int j = 0; j < w(); j++)
			ans[i][j] = M[i][j] - o.M[i][j];
	}
	return ans;
}
matrix matrix::operator*(const matrix& o) const {
	assert(w() == o.h());
	matrix ans(h(), o.w());
	for (int i = 0; i < h(); i++) {
		for (int j = 0; j < o.w(); j++) {
			for (int k = 0; k < w(); k++) {
				ans[i][j] += M[i][k] * o.M[k][j];
			}
		}
	}
	return ans;
}
int matrix::h() const {
	return M.size();
}
int matrix::w() const {
	return M[0].size();
}
vector<double> matrix::operator[](int i) const {
	return M[i];
}
vector<double>& matrix::operator[](int i) {
	return M[i];
}
matrix matrix::operator*(double k) const {
	matrix ans(M);
	for (int i = 0; i < h(); i++)
		for (int j = 0; j < w(); j++)
			ans[i][j] *= k;
	return ans;
}
matrix matrix::T() const {
	matrix ans(w(), h());
	for (int i = 0; i < h(); i++)
		for (int j = 0; j < w(); j++)
			ans[j][i] = M[i][j];
	return ans;
}
matrix matrix::operator^(const matrix& o) const {
	assert(h() == o.h() && w() == o.w());
	matrix ans(h(), w());
	for (int i = 0; i < h(); i++) {
		for (int j = 0; j < w(); j++)
			ans[i][j] = M[i][j] * o.M[i][j];
	}
	return ans;
}
matrix::matrix(const vector<double>& _m) {
	M.resize(1);
	M[0] = _m;
}
matrix matrix::apply(function<double(double)> f) const {
	matrix ans(h(), w());
	for (int i = 0; i < h(); i++)
		for (int j = 0; j < w(); j++)
			ans[i][j] = f(M[i][j]);
	return ans;
}
matrix matrix::append1() const {
	matrix ans(h(), w());
	for (int i = 0; i < h(); i++)
		ans[i].push_back(1);
	return ans;
}
matrix::matrix() {}
matrix matrix::clip() const {
	matrix ans(M);
	for (int i = 0; i < h(); i++)
		ans[i].pop_back();
	return ans;
}