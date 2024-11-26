#pragma once
#include "hangman_env.h"
#include <functional>
#include <random>
#include <fstream>

const double LEARNING_RATE_DEFAULT = 0.1;

double recl(double x);
double recl_diff(double x);
double plain(double x);
double plain_diff(double x);
double mse(const matrix& out, const matrix& target);
matrix mse_diff(const matrix& out, const matrix& target);

class perceptron_layer {
public:

	perceptron_layer(int _input_size, int _output_size, double _learning_rate, function<double(double)> _act, function<double(double)> _diff);
	perceptron_layer(const perceptron_layer& other);
	matrix compute_raw(const matrix& input) const;
	matrix compute(const matrix& input) const;
	matrix backward(const matrix& prev, const matrix& cur, const matrix& grad, bool finale);
	int getInputSize();
	int getOutputSize();
	void rerandom();
	void print(ofstream& out);
private:
	int input_size, output_size;
	matrix W;
	mt19937 rng;
	double learning_rate;
	function<double(double)> act, diff;
};

class perceptron {
public:
	perceptron(int _learning_rate);
	perceptron();
	perceptron(const perceptron& other);
	void add_layer(int _input_size, int _output_size, function<double(double)> _act, function<double(double)> _diff);
	void add_layer(int _output_size, function<double(double)> _act, function<double(double)> _diff);
	matrix forward_pass(const matrix& input) const;
	double full_pass(const matrix& input, const matrix& target);
	function<double(const matrix&, const matrix&)> batch_err = mse;
	function<matrix(const matrix&, const matrix&)> err_diff = mse_diff;
	double learning_rate;
	void save(ofstream& out);
	bool DEBUG_ECHO = false;
private:
	vector<perceptron_layer> layers;
};

