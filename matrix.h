#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<set>
#include<cassert>
#include<functional>
using namespace std;

class matrix {
public:
	vector<double>& operator[](int i);
	vector<double> operator[](int i) const;
	matrix operator+(const matrix& o) const;
	matrix operator-(const matrix& o) const;
	matrix operator*(const matrix& o) const;
	matrix operator*(const double k) const;
	matrix operator^(const matrix& o) const; // per element multiplication
	matrix();
	matrix T() const;
	matrix apply(function<double(double)> f) const;
	matrix(const vector<vector<double>>& _M);
	matrix(const vector<double>& _m);
	matrix(int n, int m);
	matrix(const matrix& _matrix);
	int h() const;
	int w() const;
	vector<vector<double>> M;
	matrix append1() const;
	matrix clip() const;
};