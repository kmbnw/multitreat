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
    // arithmetic mean of a vector
    float mean(const std::vector<float> &v) {
        return (float) std::accumulate(v.begin(), v.end(), 0.0l) / v.size();
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

    CategoryTreatmentPlan::CategoryTreatmentPlan() {
        _group_targets.clear();
    }

    void CategoryTreatmentPlan::add_pair(std::string category, float target) {
        _group_targets[category].push_back(target);
    }

    void CategoryTreatmentPlan::fill_group_stats(
        std::map<std::string, float> &means,
        std::map<std::string, float> &std_devs,
        std::map<std::string, uint>  &counts) {

        for (const auto& kv : _group_targets) {
            std::string key = kv.first;
            float group_mean = mean(kv.second);

            means[key] = group_mean;
            std_devs[key] = stddev(kv.second, group_mean);
            counts[key] = kv.second.size();
        }
    }

    void CategoryTreatmentPlan::build_treatment(std::map<std::string, float> &treatment) {
        float na_fill = 1e-6f;
        float sample_mean = 0.0f;
        float sample_sd = 0.0f;

        /*
        sampleMean = m_groups.Values.SelectMany(x => x).Average();
        sampleSd = SampleStdDev(m_groups.Values.SelectMany(x => x));
        */

        std::map<std::string, float> means;
        std::map<std::string, float> std_devs;
        std::map<std::string, uint> counts;

        fill_group_stats(means, std_devs, counts);

        for (const auto& kv : means) {
            std::string key = kv.first;
            float group_mean = kv.second;
            // using the simple version of lambda from the paper:
            // lambda = n / (m + n)
            // where m = group_sd / sample_sd
            // there is a fill-in for when only one sample exists of 1e-6
            uint n = counts[key];

            // TODO make lambda user-settable
            float lambda = na_fill;
            if (n > 1) {
                float m = std_devs[key] / sample_sd;
                lambda = n / (m + n);
            }

            // Bayesian formula from the paper
            treatment[key] = lambda * group_mean + (1 - lambda) * sample_mean;
        }
    }
}
