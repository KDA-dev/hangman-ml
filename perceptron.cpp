#include "perceptron.h"

double recl(double x) {
	return x > 0 ? x : 0;
}
double recl_diff(double x) {
	return x > 0 ? 1 : 0;
}
double plain(double x) {
	return x;
}
double plain_diff(double x) {
	return 1;
}
double mse(const matrix& out, const matrix& target) {
	double ans = 0;
	matrix se = (target - out).apply([](double cur) {return cur * cur; });
	for (int i = 0; i < out.h(); i++) {
		for (int j = 0; j < out.w(); j++)
			ans += se[i][j];
	}
	return ans / 2;
}
matrix mse_diff(const matrix& out, const matrix& target) {
	return out - target;
}

perceptron_layer::perceptron_layer(int _input_size, int _output_size, double _learning_rate, function<double(double)> _act, function<double(double)> _diff) {
	random_device rd;
	rng = mt19937(rd());
	input_size = _input_size, output_size = _output_size, learning_rate = _learning_rate, act = _act, diff = _diff;
	normal_distribution<> dist(0, 2.0 / input_size);
	W = matrix(input_size, output_size).apply([&](double cur) { return dist(rng); });
}

perceptron_layer::perceptron_layer(const perceptron_layer& other) {
	random_device rd;
	rng = mt19937(rd());
	input_size = other.input_size;
	output_size = other.output_size;
	learning_rate = other.learning_rate;
	W = other.W;
	act = other.act;
	diff = other.diff;
}
int perceptron_layer::getInputSize() { return input_size; };
int perceptron_layer::getOutputSize() { return output_size; };

void perceptron::add_layer(int _input_size, int _output_size, function<double(double)> _act, function<double(double)> _diff) {
	layers.push_back(perceptron_layer(_input_size, _output_size, learning_rate, _act, _diff));
}
void perceptron::add_layer(int _output_size, function<double(double)> _act, function<double(double)> _diff) {
	layers.push_back(perceptron_layer(layers.back().getOutputSize(), _output_size, learning_rate, _act, _diff));
}

matrix perceptron_layer::compute_raw(const matrix& input) const {
	return (input * W);
}
matrix perceptron_layer::compute(const matrix& input) const {
	return (input * W).apply(act);
}
matrix perceptron_layer::backward(const matrix& prev, const matrix& cur_raw, const matrix& grad, bool finale) {
	matrix newgrad = cur_raw.apply(diff) ^ grad;
	matrix ans = newgrad * W.T();
	W = W - (prev.T() * newgrad) * learning_rate;
	return ans;
}
void perceptron_layer::rerandom() {
	random_device rd;
	rng = mt19937(rd());
}


perceptron::perceptron(int _learning_rate) {
	learning_rate = _learning_rate;
}
perceptron::perceptron() {
	learning_rate = LEARNING_RATE_DEFAULT;
}
perceptron::perceptron(const perceptron& other) {
	learning_rate = other.learning_rate;
	batch_err = other.batch_err;
	err_diff = other.err_diff;
	layers = other.layers;
	for (int i = 0; i < layers.size(); i++)
		layers[i].rerandom();
}

matrix perceptron::forward_pass(const matrix& input) const {
	matrix ans = input;
	for (int i = 0; i < layers.size(); i++) {
		ans = layers[i].compute(ans);
	}
	return ans;
}

double perceptron::full_pass(const matrix& input, const matrix& target) {
	vector<matrix> pass_data;
	pass_data.push_back(input);
	for (int i = 0; i < layers.size(); i++) {
		pass_data.push_back(layers[i].compute_raw(pass_data.back()));
		pass_data.push_back(layers[i].compute(pass_data[pass_data.size() - 2]));
	}
	double total_error = batch_err(pass_data.back(), target);

	if (DEBUG_ECHO) {
		cout << "total error " << total_error << endl;
	}

	auto grad = err_diff(pass_data.back(), target);
	for (int i = 0; i < layers.size(); i++) {
		grad = layers[layers.size() - 1 - i].backward(pass_data[pass_data.size() - i * 2 - 3], pass_data[pass_data.size() - i * 2 - 2], grad, i == 0);
	}
	return total_error;
}

void perceptron::save(ofstream& out) {
	out << layers.size() << endl;
	for (int i = 0; i < layers.size(); i++) {
		layers[i].print(out);
	}
}

void perceptron_layer::print(ofstream& out) {
	out << input_size << " " << output_size << endl;
	for (int i = 0; i < input_size; i++) {
		for (int j = 0; j < output_size; j++)
			out << W[i][j] << ' ';
		out << endl;
	}
}