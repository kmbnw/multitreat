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

def designNumeric(
		targetCol: String, groupCol: String, df: DataFrame) : DataFrame = {
	val sampleMean = df.select(avg(targetCol)).first().getDouble(0)
	val sampleSd = df.select(stddev(targetCol)).first().getDouble(0)

  val targetMeanCol = targetCol + "_mean"

	val dfMeans = df.groupBy(groupCol).agg(
		stddev_pop(targetCol).as(targetCol + "_sd"),
		avg(targetCol).as(targetMeanCol),
		count(targetCol).divide(
			stddev(targetCol).divide(sampleSd)
		    .plus(count(targetCol))).as("lambda")
	)

	val ret = dfMeans.withColumn("S", dfMeans.col("lambda").multiply(
		dfMeans.col(targetMeanCol)).plus(
			dfMeans.col("lambda").minus(1).multiply(-1 * sampleMean))
	)
	return ret
}
