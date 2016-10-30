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
#include <vector>
#include <unordered_map>

#ifndef KMBNW_MULTITREAT_H
#define KMBNW_MULTITREAT_H

namespace multitreat {
    template <class K>
    class CategoryTreatmentPlan {
        public:
            CategoryTreatmentPlan();

            // treatment: output of category to Bayes-adjusted encoded response
            // na_value: key that represents missing/NA values.  Used for overall
            // mean calculation and will exist as an output key in the returned
            // treatment map.
            void build(
                std::unordered_map<K, float> &treatment,
                const K &na_value) const;

            void add(K category, float new_value);

        private:
            double _mean;
            double _m2_stdev;
            unsigned long _count;
            std::unordered_map<K, double> _group_means;
            std::unordered_map<K, unsigned long> _group_counts;
            std::unordered_map<K, double> _group_m2_stdev;
    };
}
#endif //KMBNW_MULTITREAT_H
