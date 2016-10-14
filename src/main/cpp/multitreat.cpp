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
    float mean(const std::map<uint, std::vector<float>> &m) {
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
    float stddev(const std::map<uint, std::vector<float>> &m, float v_mean) {
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

    CategoryTreatmentPlan::CategoryTreatmentPlan() {
    }

    void CategoryTreatmentPlan::fill_group_stats(
        const std::map<uint, std::vector<float>> &cat_groups,
        std::map<uint, float> &means,
        std::map<uint, float> &std_devs,
        std::map<uint, uint>  &counts) {

        for (const auto& kv : cat_groups) {
            uint key = kv.first;
            float group_mean = mean(kv.second);

            means[key] = group_mean;
            std_devs[key] = stddev(kv.second, group_mean);
            counts[key] = kv.second.size();
        }
    }

    void CategoryTreatmentPlan::build_treatment(
            const std::map<uint, std::vector<float>> &cat_groups,
            std::map<uint, float> &treatment,
            float &na_value) {
        float na_fill = 1e-6f;
        float sample_mean = 0.0f;
        float sample_sd = 0.0f;

        sample_mean = mean(cat_groups);
        sample_sd = stddev(cat_groups, sample_mean);
        na_value = sample_mean;

        std::map<uint, float> means;
        std::map<uint, float> std_devs;
        std::map<uint, uint> counts;

        fill_group_stats(cat_groups, means, std_devs, counts);

        for (const auto& kv : means) {
            uint key = kv.first;
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

int main() {
    std::vector<float> target = { 25.0, 50.0, 75.0, 100.0, 100.0, 300.0 };
    std::vector<std::string> titles = { "A", "A", "A", "A", "B", "B" };
    std::vector<std::string> emps = { "Fake Inc.", "Fake Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc." };

    std::vector<float> title_a(target.begin(), target.begin() + 4);
    std::vector<float> title_b(target.begin() + 4, target.end());

    std::vector<float> emp_a(target.begin(), target.begin() + 2);
    std::vector<float> emp_b(target.begin() + 2, target.end());

    std::map<uint, std::vector<float>> title_groups;
    std::map<uint, std::vector<float>> emp_groups;

    title_groups[11] = title_a;
    title_groups[20] = title_b;
    emp_groups[303] = emp_a;
    emp_groups[201] = emp_b;

    multitreat::CategoryTreatmentPlan plan;

    std::map<uint, float> title_treated;
    std::map<uint, float> emp_treated;
    float title_na;
    float emp_na;

    plan.build_treatment(title_groups, title_treated, title_na);
    plan.build_treatment(emp_groups, emp_treated, emp_na);

    std::cout << "Titles: " << std::endl;

    for (auto& kv : title_treated) {
        std::cout << kv.first << ": " << kv.second << std::endl;
    }

    std::cout << "Title NA Value: " << title_na << std::endl;

    std::cout << std::endl << "Employers: " << std::endl;

    for (auto& kv : emp_treated) {
        std::cout << kv.first << ": " << kv.second << std::endl;
    }

    std::cout << "Employer NA Value: " << emp_na << std::endl;

    /*
       Expecting this output:
       Titles:
       11: 65.9761
       20: 161.653
       Title NA Value: 108.333

       Employers:
       201: 136.296
       303: 43.3426
       Employer NA Value: 108.333

       From this original input:
{"title": "A", "amount": 25, "employer": "Fake Inc.", "title_catN": 65.97610994, "employer_catN": 43.34262378}
{"title": "A", "amount": 50, "employer": "Fake Inc.", "title_catN": 65.97610994, "employer_catN": 43.34262378}
{"title": "A", "amount": 75, "employer": "Evil Inc.", "title_catN": 65.97610994, "employer_catN": 136.2962514}
{"title": "A", "amount": 100, "employer": "Evil Inc.", "title_catN": 65.97610994, "employer_catN": 136.2962514}
{"title": "B", "amount": 100, "employer": "Evil Inc.", "title_catN": 161.6528632, "employer_catN": 136.2962514}
{"title": "B", "amount": 300, "employer": "Evil Inc.", "title_catN": 161.6528632, "employer_catN": 136.2962514}
 * */

    return 0;
}
