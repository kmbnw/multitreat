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
#include "multitreat.h"

namespace multitreat {
    //arithmetic mean of a vector
    float mean(std::vector<float> const& v) {
        return (float) std::accumulate(v.begin(), v.end(), 0.0l) / v.size();
    }

    CategoryTreatmentPlan::CategoryTreatmentPlan() {
        _group_targets.clear();
    }

    void CategoryTreatmentPlan::add_pair(std::string category, float target) {
        _group_targets[category].push_back(target);
    }


    void CategoryTreatmentPlan::fill_group_stats(
        std::map<std::string, float> & means,
        std::map<std::string, float> & stdDevs,
        std::map<std::string, uint> & counts) {

        std::map<std::string, std::vector<float> >::iterator it;
        for (it = _group_targets.begin(); it != _group_targets.end(); ++it) {
            std::string key = it->first;
            float group_mean = mean(it->second);

            means[key] = group_mean;
            counts[key] = it->second.size();
            // TODO std dev
        }
    }
}
