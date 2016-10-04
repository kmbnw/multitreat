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
using System.Collections.Generic;
using System.Linq;

namespace kmbnw.Multitreat
{
    public sealed class TreatmentPlan
    {
        public TreatmentPlan()
        {  }

        public Dictionary<string, float> DesignNumeric(float[] target, string[] categorical) {
            // overall dataframe mean and standard deviation
            float naFill = 1e-6f;
            float sampleMean = target.Average();
            float sampleSd = SampleStdDev(target);

            var means = new Dictionary<string, float>();
            var stdDevs = new Dictionary<string, float>();
            var counts = new Dictionary<string, int>();

            ComputeGroupStats(target, categorical, means, stdDevs, counts);

            var treatment = new Dictionary<string, float>();
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

        public float[] ApplyTreatment(string[] xs, Dictionary<string, float> treatment)
        {
            // now map the group-encoded values back onto the original array
            return xs.Select(x => treatment[x]).ToArray();
        }

        private float SampleStdDev(float[] xs) 
        {
            var mean = xs.Average();

            return (float) Math.Sqrt((xs.Select(x => Math.Pow(x - mean, 2))
                        .Sum() / (xs.Length - 1)));
        }

        private void ComputeGroupStats(
                float[] target,
                string[] categorical,
                Dictionary<string, float> means,
                Dictionary<string, float> stdDevs,
                Dictionary<string, int> counts)
        {
            var groups = new Dictionary<string, List<float>>();
            for (int i = 0; i < categorical.Length; i++)
            {
                string category = categorical[i];
                if (!groups.ContainsKey(category))
                {
                    groups[category] = new List<float>();
                }
                groups[category].Add(target[i]);
            }

            foreach (var item in groups)
            {
                means[item.Key] = item.Value.Average();
                // TODO be more efficient
                stdDevs[item.Key] = SampleStdDev(item.Value.ToArray());
                counts[item.Key] = item.Value.Count;
            }
        }

        // temporary testbed
        public static int Main(string[] args)
        {
            var target = new[] { 25, 50, 75, 100, 100, 300 }.Select(x => (float)x).ToArray();
            var titles = new[] { "A", "A", "A", "A", "B", "B" };
            var emps = new[] { "Fake Inc.", "Fake Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc." };

            var treatmentPlan = new TreatmentPlan();
            var titleTreat = treatmentPlan.DesignNumeric(target, titles);
            var empTreat = treatmentPlan.DesignNumeric(target, emps);

            var titleTreated = treatmentPlan.ApplyTreatment(titles, titleTreat);
            var empTreated = treatmentPlan.ApplyTreatment(emps, empTreat);

            Console.WriteLine("Titles: " + string.Join(", ", titleTreated));
            Console.WriteLine("Employers: " + string.Join(", ", empTreated));

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
