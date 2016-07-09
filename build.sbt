name         := "multitreat"
version      := "0.1a"
organization := "net.kmbnw"

scalaVersion := "2.10.5"

libraryDependencies += "org.apache.spark" %% "spark-core" % "1.6.2"
libraryDependencies += "org.apache.spark" %% "spark-sql" % "1.6.2"
libraryDependencies += "org.apache.spark" %% "spark-hive" % "1.6.2"

resolvers += Resolver.mavenLocal
