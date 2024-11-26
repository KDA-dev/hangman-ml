#pragma once
#include "perceptron.h"
#include "hangman_env.h"

const double GREED_EPS = 0.3, FORGETTANCE = 0.99;
const int EXP_BATCH_SIZE = 20, EXP_BATCH_CNT = 5, ML_INPUT = ALPHABET_LEN * 2 + ALPHABET_LEN * MAX_WORD_LEN + 2;
class exp_replay {
public:
	bool is_full();
	void add_exp(vector<double> input, int act, double reward, hangman_env after);
	exp_replay();
	matrix get_batch();
	vector<hangman_env> get_afters();
	matrix get_rewards();
private:
	int cur_index, cur_batch;
	vector<matrix> batches;
	vector<vector<hangman_env>> afters;
	bool full;
	vector<matrix> rewards;
};

class learning_env {
public:
	learning_env();
	void learn_word(string& word);
	void learn();
	int move_random(const vector<double>& weights);
	vector<double> pattern_frequency_w(const vector<int>& legal, const string& pattern);
	pair<int, double> optimise(const vector<double>& input, const vector<int>& legal, bool dash);
	perceptron Q, Q_;
private:
	exp_replay replay;
	vector<string> dictionary;
	long long iter_cnt;
	mt19937 rng;
	uniform_real_distribution<> greed;
};