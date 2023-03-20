library("readr")
setwd("/Users/alexandernizov/Desktop")

asset <- read_csv("Asset.csv")
asset <- asset[,-c(2, 3, 4, 5, 7)]
colnames(asset) <- c("DATE", "ALROSA")
asset$DATE <- as.character(asset$DATE)
asset$DATE <- as.Date(asset$DATE, "%Y%m%d")
head(asset)

imoex <- read_csv("IMOEX.csv")
colnames(imoex) <- c("DATE", "IMOEX")
imoex$DATE <- as.character(imoex$DATE)
imoex$DATE <- as.Date(imoex$DATE, "%Y%m%d")
imoex <- imoex[,-c(2, 3, 4, 5, 7)]
head(imoex)

library(PerformanceAnalytics)

plot(asset$ALROSA, 
     type = 'b', main = "ALROSA daily",
     pch = 21, col = "green", lwd = 2, xlab = 'data', ylab = "price")

plot(imoex$IMOEX, 
     type = 'b', main = "IMOEX daily",
     pch = 21, col = "green", lwd = 2, xlab = 'data', ylab = "price")

#Переходим к доходностям
Rf <- 0.04
len <- dim(asset)[1]
r1 <- diff(log(asset$ALROSA))
r2 <- diff(log(imoex$IMOEX))
rates <- cbind(r1, r2)
rownames(rates) <- as.character(imoex$DATE[2:252])
colnames(rates) <- c("ALROSA", "IMOEX")
head(rates)

#Коэффициент бета
beta <- CAPM.beta(rates[, "ALROSA"], rates[, "IMOEX"])
beta

#Коэффициент альфа
alpha <- CAPM.alpha(rates[, "ALROSA"], rates[, "IMOEX"], Rf = 0.04)
alpha

#Коэффициент Шарпа
SharpeRatio(rates[, "ALROSA"], Rf = Rf, p = 0.95, FUN = c("StdDev"))

#Нормальный VAR уровня 0.95
(VaR(rates[, "ALROSA"], p=.95, method="gaussian"))

#Исторический VAR уровня 0.95
(VaR(rates[, "ALROSA"], p=.95, method="historical"))

#Нормальный Shortfall для уровня 0.95
ES(rates, p=.95, method="gaussian")

#Исторический Shortfall для уровня 0.95
ES(rates, p=.95, method="historical")

