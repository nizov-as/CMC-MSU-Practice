install.packages("readr")
library("readr")

#Данные о ценах закрытия акций Boeing Industrials (тикер BA) с 3.01.2022 до 24.02.2023 
boeing_data <- read_csv("BAdata.csv")

prices <- boeing_data$BA

ndates <- as.Date(boeing_data$Date, "%m/%d/%y") 
matplot(ndates, boeing_data$BA, type = "b", pch = 20, col = 'blue', main = "BA/USD")



#Составим выборку rates из показателей доходности акций Boeing Industrials (BA),
#где i-ый элемент выборки — доходность акций BA на i-ый день в процентах от (i-1)-ого
nrates <- length(prices)
rates <- boeing_data$BA[2:nrates]/boeing_data$BA[1:(nrates-1)]-1
matplot(ndates[2:nrates], rates, type = "b", pch = 20, col = 'blue', main = "BA/USD rates")

#Гистограмма
library(fBasics)
h <- hist(rates, col = "blue", nclass = 20)
histPlot(as.timeSeries(rates))

#Гипотеза случайности
#Предполагаем, что компоненты выборки независимы и одинаково распределены
library(randtests)

rank.test(rates) #Mann-Kendall Rank Test
bartels.rank.test(rates) #Bartels Ratio Test
cox.stuart.test(rates) #Cox Stuart test
runs.test(rates) #Runs test
turning.point.test(rates) #Turning Point test
#Для каждого из пяти тестов p-value больше уровня значимости 0.05,
#следовательно, гипотеза случайности не отвергается.


#Гипотеза однородности
#В качестве основной гипотезы выдвинем предположение
#об идентичности функций распределения двух выборов доходностей:
rates1 <- rates[1:124] #доходности акций до 2022-07-01 (первые 124 дня года)
rates2 <- rates[125:248] #доходности акций после 2022-07-01 (вторые 124 дня года)

wilcox.test(rates1, rates2, paired = T) #Wilcoxon signed rank test
#В тесте Уилкокса p-value больше нашего заданного уровня значимости 0.05.
#Вывод: гипотеза об однородности не отвергается.

#Предложить наиболее адекватный тип распределения для доходностей.
install.packages("rriskDistributions")
library(rriskDistributions)
fit.cont(data2fit = rates)
#Согласно визуальному сравнению гистограмм и графиков квантилей
#для предложенных теоретических распределений, делаем вывод,
#что суточные показатели доходности акций BA имеют нормальное распределение

#Разделить случайно выборку на две части тренировочную и тестовую.
d = sort(sample(length(rates), length(rates)*.5))
train <- rates[d]
test <- rates[-d]
length(test)

library(MASS)
histPlot(as.timeSeries(rates))

#Оценим методом максимального правдоподобия 
#параметры предполагаемого распределения по тренировочной части выборки:
library(fitdistrplus)
fitting_normal <- fitdistr(train, "normal")
fitting_normal

#Проверить гипотезу согласия с предложенным распределением всей выборки
#критериями Колмогорова-Смирнова и Хи-квадрат Пирсона

library(ggplot2)
ks.test(test, 'pnorm', fitting_normal$estimate[1], fitting_normal$estimate[2])
#p-value больше 0.05, следовательно гипотезу согласия
# с предложенным распределением всей выборки принимается

h2 <- hist(test, plot = F)
E <- h2$breaks

ntest <- length(test)
dens <- h2$density*ntest

library(zoo)
breaks_cdf <- pnorm(E, fitting_normal$estimate[1], fitting_normal$estimate[2])
breaks_cdf[2]-breaks_cdf[1]

null.probs <- rollapply(breaks_cdf, 2, function(x) x[2]-x[1])
null.probs
dens

sum(null.probs)
xsq <- chisq.test(x = dens, p = null.probs, rescale.p = T)
xsq$p.value









