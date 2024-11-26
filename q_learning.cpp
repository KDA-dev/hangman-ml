#include "q_learning.h"

exp_replay::exp_replay() {
	cur_index = 0;
	cur_batch = -1;
	full = false;
	batches = vector<matrix>(EXP_BATCH_CNT, matrix(EXP_BATCH_SIZE, ML_INPUT));
	afters = vector<vector<hangman_env>>(EXP_BATCH_CNT, vector<hangman_env>(EXP_BATCH_SIZE));
	rewards = vector<matrix>(EXP_BATCH_CNT, matrix(EXP_BATCH_SIZE, 1));
}

void exp_replay::add_exp(vector<double> input, int act, double reward, hangman_env after) {
	cur_batch++;
	if (cur_batch == EXP_BATCH_CNT) {
		cur_batch = 0;
		cur_index++;
	}
	if (cur_index == EXP_BATCH_SIZE) {
		cur_index = 0;
		full = true;
	}

	batches[cur_batch][cur_index] = input;
	batches[cur_batch][cur_index][act] = 1;
	rewards[cur_batch][cur_index][0] = reward;
	afters[cur_batch][cur_index] = after;
}
bool exp_replay::is_full() {
	return full;
}
matrix exp_replay::get_batch() {
	return batches[cur_batch];
}
vector<hangman_env> exp_replay::get_afters() {
	return afters[cur_batch];
}
matrix exp_replay::get_rewards() {
	return rewards[cur_batch];
}

learning_env::learning_env() {
	Q.add_layer(ML_INPUT, 64, recl, recl_diff);
	Q.add_layer(16, recl, recl_diff);
	Q.add_layer(1, plain, plain_diff);

	Q_ = Q;
	ifstream dict_s("dictionary.txt");
	string s;
	while (dict_s >> s) {
		dictionary.push_back(s);
	}
	iter_cnt = 0;
	random_device rd;
	rng = mt19937(rd());
	greed = uniform_real_distribution<>(0., 1.);
}
pair<int, double> learning_env::optimise(const vector<double>& input, const vector<int>& legal, bool dash) {
	matrix in;
	for (int i = 0; i < legal.size(); i++) {
		in.M.push_back(input);
		in[i][legal[i]] = 1;
	}
	matrix out = (dash ? Q_ : Q).forward_pass(in);
	int imax = 0;
	double ans = out[0][0];
	for (int i = 1; i < legal.size(); i++) {
		if (out[i][0] > ans) {
			ans = out[i][0];
			imax = i;
		}
	}
	int z = 0;
	return { legal[imax], ans };
}
void learning_env::learn_word(string& s) {
	hangman_env env(s);
	while (!env.finished()) {
		vector<int> legal = env.legal();
		auto input = env.getDataML();
		int act = (greed(rng) > GREED_EPS) ? optimise(input, legal, false).first : legal[move_random(pattern_frequency_w(legal, env.get_pattern()))];
		double r = env.make_turn(act);
		replay.add_exp(input, act, r, env);

		if (replay.is_full()) {
			matrix t = replay.get_rewards();
			auto envs = replay.get_afters();
			for (int i = 0; i < EXP_BATCH_SIZE; i++) {
				if (envs[i].finished())
					continue;

				t[i][0] += optimise(envs[i].getDataML(), envs[i].legal(), true).second * FORGETTANCE;
			}

			Q.full_pass(replay.get_batch(), t);
		}

		++iter_cnt;

		if (iter_cnt % 100 == 0) {
			Q_ = Q;
		}
	}
}

vector<double> learning_env::pattern_frequency_w(const vector<int>& legal, const string& pattern) {
	vector<double> res(legal.size(), 0);
	for (const string& s : dictionary) {
		if (!pattern_match(pattern, s))
			continue;
		bool skip = false;
		for (int i = 0; i < s.size(); i++) {
			if (pattern[i] == '*' && find(legal.begin(), legal.end(), s[i] - 'a') == legal.end())
				skip = true;
		}
		if (skip)
			continue;

		vector<bool> letters_used(26, false);
		for (char c : s)
			letters_used[c - 'a'] = true;

		for (int i = 0; i < res.size(); i++) {
			if (letters_used[legal[i]])
				res[i] += 1;
		}
	}
	return res;
}

int learning_env::move_random(const vector<double>& weights) {
	discrete_distribution<> dist(weights.begin(), weights.end());
	return dist(rng);
}

void learning_env::learn() {
	Q.DEBUG_ECHO = false;
	shuffle(dictionary.begin(), dictionary.end(), rng);
	while (true) {
		for (int i = 0; i < 10; i++)
			learn_word(dictionary[i]);
		cout << dictionary[0] << endl;
		hangman_env test(dictionary[0]);
		test.draw_game();
		while (!test.finished()) {
			int act = optimise(test.getDataML(), test.legal(), false).first;
			test.make_turn(act);
			test.draw_game();
		}
		ofstream out;
		out.open("perceptron.txt");
		Q.save(out);
		out.close();
	}
}