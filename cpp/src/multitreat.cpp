/*
 * Copyright 2016 Krysta M Bouzek
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <numeric>
#include <vector>
#include <algorithm>
#include <cmath>
#include "multitreat.h"

namespace multitreat {
    void online_update(
            const float& new_value,
            unsigned long& x_count,
            double& x_mean,
            double& x_m2_stdev) {
        x_count += 1;
        double delta = new_value - x_mean;
        x_mean += delta / x_count;
        x_m2_stdev += delta * (new_value - x_mean);
    }

    double std_dev_from_m2(const unsigned long& count, const double& m2_stdev) {
        return count < 2 ? nan("") : m2_stdev / (count - 1);
    }

    template <class K>
    CategoryTreatmentPlan<K>::CategoryTreatmentPlan() {
        _group_means.clear();
        _group_counts.clear();
        _group_m2_stdev.clear();
    }

    // online calculation of mean and variance
    // see https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
    template <class K>
    void CategoryTreatmentPlan<K>::add(K category, float new_value) {
        // update mean and std dev for overall sample
        online_update(new_value, _count, _mean, _m2_stdev);

        // and for the group
        double cat_mean = 0;
        unsigned long cat_count = 0;
        double cat_m2_stdev = 0;

        // have we seen this category before?
        auto cat_mean_iter = _group_means.find(category);
        if (cat_mean_iter != _group_means.end()) {
            cat_mean = cat_mean_iter->second;
            cat_count = _group_counts.find(category)->second;
            cat_m2_stdev = _group_m2_stdev.find(category)->second;
        }

        online_update(new_value, cat_count, cat_mean, cat_m2_stdev);

        // update internal structures
        _group_means[category] = cat_mean;
        _group_counts[category] = cat_count;
        _group_m2_stdev[category] = cat_m2_stdev;
    }

    template <class K>
    void CategoryTreatmentPlan<K>::build(
            std::unordered_map<K, float> &treatment,
            const K &na_value) const {

        if (_count < 1) {
            // nothing to calculate on
            return;
        }

        float na_fill = 1e-6f;
        double stdev = std_dev_from_m2(_count, _m2_stdev);
        treatment[na_value] = (float) _mean;

        for (const auto& kv : _group_means) {
            K category = kv.first;
            if (category == na_value) {
                continue;
            }
            double cat_mean = kv.second;
            unsigned long cat_count = _group_counts.find(category)->second;
            double cat_m2_stdev = _group_m2_stdev.find(category)->second;
            double cat_stdev = std_dev_from_m2(cat_count, cat_m2_stdev);

            // using the simple version of lambda from the paper:
            // lambda = n / (m + n)
            // where m = group_sd / sample_sd
            // there is a fill-in for when only one sample exists of 1e-6
            unsigned long n = cat_count;

            // TODO make lambda user-settable
            double lambda = na_fill;
            if (_count > 1 && n > 1 && stdev > 0) {
                double m = cat_stdev / stdev;
                lambda = n / (m + n);
            }

            // Bayesian formula from the paper
            double treated = lambda * cat_mean + (1 - lambda) * _mean;
            treatment[category] = (float) treated;
        }
    }

    template class CategoryTreatmentPlan<unsigned char>;
    template class CategoryTreatmentPlan<signed char>;
    template class CategoryTreatmentPlan<unsigned int>;
    template class CategoryTreatmentPlan<int>;
    template class CategoryTreatmentPlan<unsigned long>;
    template class CategoryTreatmentPlan<long>;
    template class CategoryTreatmentPlan<unsigned long long>;
    template class CategoryTreatmentPlan<long long>;
    template class CategoryTreatmentPlan<std::string>;
}
