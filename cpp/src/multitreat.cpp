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
#include <map>
#include <numeric>
#include <vector>
#include <algorithm>
#include <cmath>
#include "multitreat.h"

namespace multitreat {
    double sum(const std::vector<float> &v) {
        return std::accumulate(v.begin(), v.end(), 0.0l);
    }

    // arithmetic mean of a vector
    float mean(const std::vector<float> &v) {
        return (float) sum(v) / v.size();
    }

    // arithmetic mean of all vectors in the map
    // TODO I should be able to do this with weighted averages
    // which would avoid iterating over all the values again
    template <typename K>
    float mean(const std::map<K, std::vector<float>> &m) {
        double total = 0.0;
        ulong count = 0;
        for (const auto& kv: m) {
            total += sum(kv.second);
            count += kv.second.size();
        }
        return total / count;
    }

    // sample standard deviation of vector
    float stddev(const std::vector<float> &v, float v_mean) {
        // the combo of std::for_each, std::accumulate, and a lambda function
        // is just a little too opaque for my taste
        double sd = 0.0;
        for (const auto& x: v) {
            sd += pow(x - v_mean, 2.0);
        }
        return (float) sqrt(sd / (v.size() - 1));
    }

    // sample standard deviation of map
    template <typename K>
    float stddev(const std::map<K, std::vector<float>> &m, float v_mean) {
        double sd = 0.0;
        ulong count = 0;
        for (const auto& kv: m) {
            for (const auto& x: kv.second) {
                sd += pow(x - v_mean, 2.0);
            }
            count += kv.second.size();
        }
        return sqrt(sd / (count - 1));
    }

    template <class K>
    CategoryTreatmentPlan<K>::CategoryTreatmentPlan() {
    }

    template <class K>
    void CategoryTreatmentPlan<K>::fill_group_stats(
        const std::map<K, std::vector<float>> &cat_groups,
        std::map<K, float> &means,
        std::map<K, float> &std_devs,
        std::map<K, unsigned int>  &counts) const {

        for (const auto& kv : cat_groups) {
            K key = kv.first;
            float group_mean = mean(kv.second);

            means[key] = group_mean;
            std_devs[key] = stddev(kv.second, group_mean);
            counts[key] = kv.second.size();
        }
    }

    template <class K>
    void CategoryTreatmentPlan<K>::build_treatment(
            const std::map<K, std::vector<float>> &cat_groups,
            std::map<K, float> &treatment,
            const K &na_value) const {
        float na_fill = 1e-6f;
        float sample_mean = 0.0f;
        float sample_sd = 0.0f;

        sample_mean = mean(cat_groups);
        sample_sd = stddev(cat_groups, sample_mean);

        treatment[na_value] = sample_mean;

        std::map<K, float> means;
        std::map<K, float> std_devs;
        std::map<K, unsigned int> counts;

        fill_group_stats(cat_groups, means, std_devs, counts);

        for (const auto& kv : means) {
            K key = kv.first;
            if (key == na_value) {
                continue;
            }
            float group_mean = kv.second;
            // using the simple version of lambda from the paper:
            // lambda = n / (m + n)
            // where m = group_sd / sample_sd
            // there is a fill-in for when only one sample exists of 1e-6
            unsigned int n = counts[key];

            // TODO make lambda user-settable
            float lambda = na_fill;
            if (n > 1 && sample_sd > 0) {
                float m = std_devs[key] / sample_sd;
                lambda = n / (m + n);
            }

            // Bayesian formula from the paper
            treatment[key] = lambda * group_mean + (1 - lambda) * sample_mean;
        }
    }

    template class CategoryTreatmentPlan<unsigned int>;
    template class CategoryTreatmentPlan<int>;
    template class CategoryTreatmentPlan<std::string>;
}
