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
package net.kmbnw.multitreat

import org.apache.spark.sql.functions._
import org.apache.spark.sql.DataFrame

class Multitreat(
		val targetCol: String,
		val treatmentCols: Seq[String]) {

	def designNumeric(df: DataFrame): Map[String, DataFrame] = {
		return treatmentCols.zip(
			treatmentCols.map(x => designNumericOnCol(df, x))
		).toMap
	}

	// e.g. designNumeric("funded_amnt", "emp_title", df).show()
	private def designNumericOnCol(df: DataFrame, groupCol: String): DataFrame = {

		// overall dataframe mean and standard deviation
		val dfNoNA = df.select(targetCol, groupCol).na.drop()
		val naFill = 1e-6
		val sampleMean = dfNoNA.select(avg(targetCol)).first().getDouble(0)
		val sampleSd = dfNoNA.select(stddev(targetCol)).first().getDouble(0)

		// using the simple version of lambda from the paper: lambda = n / (m + n)
		// where m = group_sd / sample_sd
		// there is a fill-in for when only one sample exists of 1e-6
		// TODO I would like to make lambda user-settable
		val dfMeans = dfNoNA.groupBy(groupCol).agg(
			avg(targetCol).as("mean"),
			count(targetCol).divide(
				stddev(targetCol).divide(sampleSd)
			    .plus(count(targetCol))).as("lambda")
		).na.fill(naFill, Array("lambda"))

		// this is the Bayesian formula:
		// lambda * group_mean + (1 - lambda) * sample_mean
		val lambda = dfMeans.col("lambda")
		val groupMean = dfMeans.col("mean")
		val ret = dfMeans.withColumn(
			"S",
			lambda.multiply(groupMean).plus(
					lambda.minus(1).multiply(-1 * sampleMean))
		)

		return ret
	}
}
