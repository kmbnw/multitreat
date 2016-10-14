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
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Linq;

namespace kmbnw.Multitreat
{

    /// <summary>
    /// Create a new treatment plan whose category type is K.
    ///
    /// K is used as a dictionary key and thus should follow all the
    /// rules necessary for that to work correctly.
    /// </summary>
    public class CategoryTreatmentPlan<K>
    {
        public CategoryTreatmentPlan()
        {  }

        /// <summary>
        /// Construct a mapping from category to Bayes-adjusted response.
        ///
        /// The output value is described in 
        ///  "A Preprocessing Scheme for High Cardinality Categorical Attributes in 
        ///  Classification and Prediction Problems", Micci-Barreca, Daniele.
        /// </summary>
        /// <param name="catGroups">A dictionary from each distinct category
        /// to all of the response values associated with that category.
        /// </param>
        /// <param name="naValue">The value which represents "missing" or "NA".
        /// Currently the mean of all the response values in <b>catGroups</b>.
        /// </param>
        /// <returns>A dictionary from category to re-encoded response.</returns>
        public Dictionary<K, float> BuildTreatments<T>(
                Dictionary<K, T> catGroups, out float naValue) where T: IEnumerable<float>
        {
            // overall dataframe mean and standard deviation
            float naFill = 1e-6f;

            var means = new Dictionary<K, float>();
            var stdDevs = new Dictionary<K, float>();
            var counts = new Dictionary<K, int>();

            float sampleMean = catGroups.Values.SelectMany(x => x).Average();
            float sampleSd = SampleStdDev(catGroups.Values.SelectMany(x => x));
            ComputeGroupStats(catGroups, means, stdDevs, counts);

            naValue = sampleMean;

            var treatment = new Dictionary<K, float>();
            foreach (var k in means.Keys)
            {
                var groupMean = means[k];
                // using the simple version of lambda from the paper:
                // lambda = n / (m + n)
                // where m = group_sd / sample_sd
                // there is a fill-in for when only one sample exists of 1e-6
                int n = counts[k];

                // TODO I would like to make lambda user-settable
                float lambda = naFill;
                if (n > 1) {
                    float m = stdDevs[k] / sampleSd;
                    lambda = n / (m + n);
                }

                // Bayesian formula from the paper
                treatment[k] = lambda * groupMean + (1 - lambda) * sampleMean;
            }

            return treatment;
        }

        private float SampleStdDev<T>(T xs) where T: IEnumerable<float>
        {
            var mean = xs.Average();

            return (float) Math.Sqrt((xs.Select(x => Math.Pow(x - mean, 2))
                        .Sum() / (xs.Count() - 1)));
        }

        private void ComputeGroupStats<T>(
                Dictionary<K, T> catGroups,
                Dictionary<K, float> means,
                Dictionary<K, float> stdDevs,
                Dictionary<K, int> counts) where T: IEnumerable<float>
        {
            foreach (var item in catGroups)
            {
                means[item.Key] = item.Value.Average();
                // TODO be more efficient
                stdDevs[item.Key] = SampleStdDev(item.Value.ToArray());
                counts[item.Key] = item.Value.Count();
            }
        }
    }

    class CategoryTreatmentPlanTest
    {
        // temporary testbed
        public static int Main(string[] args)
        {
            var target = new[] { 25, 50, 75, 100, 100, 300 }.Select(x => (float)x).ToArray();

            var titles = new[] { "A", "A", "A", "A", "B", "B" };
            var emps = new[] { "Fake Inc.", "Fake Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc." };

            var titleMap = new Dictionary<string, List<float>>();
            var empMap = new Dictionary<string, List<float>>();

            for (int idx = 0; idx < target.Length; idx++)
            {
                List<float> titleTargets;
                if (!titleMap.TryGetValue(titles[idx], out titleTargets))
                {
                    titleMap[titles[idx]] = new List<float>();
                }
                titleMap[titles[idx]].Add(target[idx]);

                List<float> empTargets;
                if (!empMap.TryGetValue(emps[idx], out empTargets))
                {
                    empMap[emps[idx]] = new List<float>();
                }
                empMap[emps[idx]].Add(target[idx]);
            }

            float titleNA;
            float empNA;
            var treatPlan = new CategoryTreatmentPlan<string>();
            var titleTreat = treatPlan.BuildTreatments(titleMap, out titleNA);
            var empTreat = treatPlan.BuildTreatments(empMap, out empNA);

            var titleTreated = titles.Select(x => titleTreat[x]);
            var empTreated = emps.Select(x => empTreat[x]);

            Console.WriteLine("Titles: " + string.Join(", ", titleTreated));
            Console.WriteLine("Employers: " + string.Join(", ", empTreated));
            Console.WriteLine("Titles NA: " + titleNA);
            Console.WriteLine("Employers NA: " + empNA);

            // TODO proper unit test
            /*
{"title": "A", "amount": 25, "employer": "Fake Inc.", "title_catN": 65.97610994, "employer_catN": 43.34262378}
{"title": "A", "amount": 50, "employer": "Fake Inc.", "title_catN": 65.97610994, "employer_catN": 43.34262378}
{"title": "A", "amount": 75, "employer": "Evil Inc.", "title_catN": 65.97610994, "employer_catN": 136.2962514}
{"title": "A", "amount": 100, "employer": "Evil Inc.", "title_catN": 65.97610994, "employer_catN": 136.2962514}
{"title": "B", "amount": 100, "employer": "Evil Inc.", "title_catN": 161.6528632, "employer_catN": 136.2962514}
{"title": "B", "amount": 300, "employer": "Evil Inc.", "title_catN": 161.6528632, "employer_catN": 136.2962514}
            */
            return 0;
        }
    }
}
