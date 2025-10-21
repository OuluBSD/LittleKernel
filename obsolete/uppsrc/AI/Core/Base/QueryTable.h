#ifndef _AICore2_QueryTable_h_
#define _AICore2_QueryTable_h_


// Theory: http://www.saedsayad.com/decision_tree.htm


template <class T>
class QueryTable {
	
	Array<Vector<T> > predictors;
	Vector<T> target;
	Vector<double> gains;
	VectorMap<T, double> scores;
	
	Vector<String> predictor_names;
	String target_name;
	
public:
	
	QueryTable() {}
	
	Vector<T>& AddPredictor(const String& name) {predictor_names.Add(name); return predictors.Add();}
	
	
	void AddValue(int i, const T& value) {predictors[i].Add(value);}
	void AddTargetValue(const T& value) {target.Add(value);}
	void AddTargetScore(const T& value, double score) {scores.Add(value, score);}
	
	const Vector<double>& GetInfoGains() const {return gains;}
	
	int GetLargestInfoGainPredictor() {
		
		// Prepare function
		gains.Clear();
		ASSERT(predictors.GetCount());
		ASSERT(target.GetCount());
		ASSERT(predictors[0].GetCount() == target.GetCount());
		
		// Find all unique target values, and count them
		VectorMap<T, int> target_count;
		for(int i = 0; i < target.GetCount(); i++) {
			T& t = target[i];
			target_count.GetAdd(t, 0)++;
		}
		
		// Count total (some values could have been skipped as invalid)
		double total = 0;
		for(int i = 0; i < target_count.GetCount(); i++)
			total += target_count[i];
		
		// Calculate entropy
		double entropy = 0;
		for(int i = 0; i < target_count.GetCount(); i++)  {
			double part = target_count[i] * 1.0 / total;
			entropy -= part * log2((double)part);
		}
		
		// Loop over all predictors
		for(int i = 0; i < predictors.GetCount(); i++) {
			Vector<T>& vec = predictors[i];
			
			VectorMap<T, VectorMap<T, int> > pred_counts;
			
			double pred_total = 0;
			for(int j = 0; j < vec.GetCount(); j++) {
				// Get predictor value and target value
				T& c = vec[j];
				T& t = target[j];
				
				// Get map by predictor value
				VectorMap<T, int>& pred_target_count = pred_counts.GetAdd(c);
				
				// Increase target count with this predictor value by one
				pred_target_count.GetAdd(t, 0)++;
				
				pred_total += 1;
			}
			
			// Loop over all unique values in the column
			double pred_entropy = 0;
			for(int j = 0; j < pred_counts.GetCount(); j++) {
				VectorMap<T, int>& pred_target_count = pred_counts[j];
				
				// Sum total value count in the table with this unique predictor category
				int pred_cat_total = 0;
				for(int k = 0; k < pred_target_count.GetCount(); k++)
					pred_cat_total += pred_target_count[k];
				
				// Calculate entropy for predictor category
				double pred_cat_entropy = 0;
				for(int k = 0; k < pred_target_count.GetCount(); k++) {
					double part = pred_target_count[k] * 1.0 / pred_cat_total;
					pred_cat_entropy -= part * log2((double)part);
				}
				pred_entropy += (double)pred_cat_total / pred_total * pred_cat_entropy;
			}
			
			// Calculate information gain value with this predictor in the current dataset
			double gain = entropy - pred_entropy;
			gains.Add(gain);
		}
		
		// Find the maximum information gain predictor
		double max_gain = -DBL_MAX;
		int max_id = -1;
		for(int i = 0; i < gains.GetCount(); i++) {
			if (gains[i] > max_gain) {
				max_gain = gains[i];
				max_id = i;
			}
		}
		
		return max_id;
	}
	
	String GetInfoString() {
		GetLargestInfoGainPredictor();
		const Vector<double>& gains = GetInfoGains();
		VectorMap<int,double> gain_idx;
		for(int i = 0; i < gains.GetCount(); i++)
			gain_idx.Add(i, gains[i]);
		SortByValue(gain_idx, StdGreater<double>());
		
		
		String out;
		for(int i = 0; i < gain_idx.GetCount(); i++) {
			int idx = gain_idx.GetKey(i);
			double gain = gain_idx[i];
			
			VectorMap<T, VectorMap<T,int>> value_counts;
			
			const auto& idx_preds = predictors[idx];
			for(int j = 0; j < idx_preds.GetCount(); j++) {
				value_counts.GetAdd(idx_preds[j]).GetAdd(target[j],0)++;
			}
			
			for (auto it : ~value_counts)
				SortByValue(it.value, StdGreater<int>());
			
			struct Sorter {
				QueryTable* qt = 0;
				bool operator()(const VectorMap<T,int>& a, const VectorMap<T,int>& b) const {
					double sum_a = 0, sum_b = 0;
					int count_a = 0, count_b = 0;
					for (const auto it : ~a) {
						double score = qt->scores.Get(it.key, 1);
						sum_a += it.value * score;
						count_a += it.value;
					}
					for (const auto it : ~b) {
						double score = qt->scores.Get(it.key, 1);
						sum_b += it.value * score;
						count_b += it.value;
					}
					double av_a = sum_a / count_a;
					double av_b = sum_b / count_b;
					return av_a > av_b;
				}
			};
			Sorter s;
			s.qt = this;
			SortByValue(value_counts, s);
			
			out << "Predictor " << idx << ": " << predictor_names[idx] << " (" << Format("%.2f", gain) << "):\n";
			for(int j = 0; j < value_counts.GetCount(); j++) {
				const T& pred_key = value_counts.GetKey(j);
				const auto& pred_val = value_counts[j];
				
				double sum = 0;
				int count = 0;
				for (const auto it : ~pred_val) {
					double score = scores.Get(it.key, 1);
					sum += it.value * score;
					count += it.value;
				}
				double av = sum / count;
				
				out << "\t" << AsString(pred_key) << " (" << Format("%.2f", av) << ")\n";
				if (0) {
					for (const auto& it : ~pred_val) {
						double score = scores.Get(it.key, 1);
						double weighted = score * it.value;
						out << "\t\t" << AsString(it.key) << ": " << weighted << "\n";
					}
				}
			}
			out << "\n";
		}
		return out;
	}
};


#endif
