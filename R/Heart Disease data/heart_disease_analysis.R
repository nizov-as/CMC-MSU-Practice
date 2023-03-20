install.packages("readr")
library("readr")
setwd("/Users/alexandernizov/Desktop/Prac")
disease_data <- read_csv("heart_2020_cleaned.csv")
#Ограничим объём данных 2500 респондентами и удалим несколько столбцов
df <- disease_data[-c(2501:319795),-c(5,11,12,14,16,17,18)]

#1) Реализовать аппроксимацию распределений данных с помощью ядерных оценок.
data_BMI <- df$BMI
hist(data_BMI, breaks = 25, freq = FALSE,
     xlab = "Индекс массы тела (BMI)", ylab = "Ядерная плотность",
     main = "Аппроксимация распределений данных BMI")
lines(density(data_BMI), lwd = 3)

#2) Реализовать анализ данных с помощью cdplot, dotchart, boxplot и stripchart.
install.packages("dplyr")
library("dplyr")

#cdplot
df$HeartDisease <- as.factor(df$HeartDisease)
disease_data$HeartDisease <- as.factor(disease_data$HeartDisease)
layout(matrix(1:2, ncol = 2))
cdplot(disease_data$BMI[disease_data$Sex == "Male"],
       disease_data$HeartDisease[disease_data$Sex == "Male"], 
       xlab = "Male BMI",
       ylab ="Male Heart Disease", 
       main ="Male Disease",
       bw=1.1)
cdplot(disease_data$BMI[disease_data$Sex == "Female"],
       disease_data$HeartDisease[disease_data$Sex == "Female"], 
       xlab = "Female BMI",
       ylab ="Female Heart Disease", 
       main ="Female Disease",
       bw=1.1)

#dotchart
y <- factor(disease_data$AgeCategory)
newdate_mental <- aggregate(disease_data$MentalHealth, 
                            by = list(y), mean)
newdate_physical <- aggregate(disease_data$PhysicalHealth, 
                              by = list(y), mean)
colnames(newdate_mental) <- c('AgeCategory', 'MeanMentalHealth')
colnames(newdate_physical) <- c('AgeCategory', 'MeanPhysicalHealth')

layout(matrix(1:2, ncol = 2))
dotchart(newdate_mental$MeanMentalHealth,
         groups = newdate_mental$AgeCategory, gcolor = "blue", pch = 20,
         main="Средний MentalHealth",
         xlab="Дней с плохим ментальным здоровьем")
dotchart(newdate_physical$MeanPhysicalHealth,
         groups = newdate_physical$AgeCategory, gcolor = "red", pch = 20,
         main="Средний PhysicalHealth",
         xlab="Дней с плохим физическим здоровьем")

layout(matrix(1:1, ncol = 1))

#boxplot
boxplot(BMI ~ AgeCategory, xlab = "Возраст", ylab = "BMI",
        main = "Распределение BMI по категориям", data = df)

#stripchart
#Для корректного отображения сузим выборку.
#Посмотрим, например, на значения BMI для первых 100 респондентов
small_df <- df[-c(101:2500),]

#rounded_BMI <- round(smalldf, digits=1)
stripchart(small_df$BMI,
           xlab="Показатель BMI",
           pch=1)

#Критерий Граббса
install.packages("outliers")
library("outliers")
grubbs.test(df$BMI, type = 11)

#Q-тест Диксона
#Применяется на небольших объёмах данных. Рассмотрим 30 наблюдений.
#Чтобы соотнести их с результатами критерия Граббса,
#рассмотрим BMI первых 28 респондентов и добавим к ним выбросы критерия Граббса
small_BMI <- c(df$BMI[1:28], 12.48, 75.82)
dixon.test(small_BMI)
dixon.test(small_BMI,opposite=TRUE)

stripchart(small_BMI)

#Воспользоваться инструментами для заполнения пропусков в данных
#Создадим копию датафрейма и вручную внесём 10 пропусков в столбец BMI:
df_testing <- df
right_vals <- df_testing$BMI[c(1, 11, 21, 31, 41, 51, 61, 71, 81, 91, 101)]
df_testing$BMI[c(1, 11, 21, 31, 41, 51, 61, 71, 81, 91, 101)] = NaN

df_testing_mean <- df_testing
df_testing_mean$BMI[is.na(df_testing_mean$BMI)] <- 
  mean(df_testing_mean$BMI, na.rm = T)
mean_vals <- df_testing_mean$BMI[c(1, 11, 21, 31, 41, 51, 61, 71, 81, 91, 101)]
mean(abs(right_vals-mean_vals))

df_testing_median <- df_testing
df_testing_median$BMI[is.na(df_testing_median$BMI)] <- 
  median(df_testing_median$BMI, na.rm = T)
median_vals <- df_testing_median$BMI[c(1, 11, 21, 31, 41, 51, 61, 71, 81, 91, 101)]
mean(abs(right_vals-median_vals))

install.packages("mice")
library("mice")
method_use <- mice(df_testing)
df_testing_2 <- complete(method_use)
mice_vals <- df_testing_2$BMI[c(1, 11, 21, 31, 41, 51, 61, 71, 81, 91, 101)]
mean(abs(right_vals-mice_vals))

#Работа с произвольными данными из нормального распределения
#Малые выборки по 50 наблюдений с различными параметрами
small_1 <- rnorm(50, 0, 1)
small_2 <- rnorm(50, 5, 2)
small_3 <- rnorm(50, -5, 5)
#Умеренные выборки по 2500 наблюдений с различными параметрами
big_1 <- rnorm(2500, 0, 1)
big_2 <- rnorm(2500, 5, 2)
big_3 <- rnorm(2500, -5, 5)

install.packages("ggplot2")
library(ggplot2)

ecdf_graph <- function(data, pc){ 
  plot(data, pc, type = "l", col = "blue", lwd = 3) 
  plot(ecdf(data), add = TRUE) 
}

layout(matrix(1:3, ncol = 3))
ecdf_graph(sort(small_1), pnorm(sort(small_1), mean = 0, sd = 1))
ecdf_graph(sort(small_2), pnorm(sort(small_2), mean = 5, sd = 2))
ecdf_graph(sort(small_3), pnorm(sort(small_3), mean = -5, sd = 5))

ecdf_graph(sort(big_1), pnorm(sort(big_1), mean = 0, sd = 1))
ecdf_graph(sort(big_2), pnorm(sort(big_2), mean = 5, sd = 2))
ecdf_graph(sort(big_3), pnorm(sort(big_3), mean = -5, sd = 5))

#Графики квантилей
quantile <- function(data){
  qqnorm(data)
  qqline(data)
}
quantile(small_1)
quantile(small_2)
quantile(small_3)
quantile(big_1)
quantile(big_2)
quantile(big_3)

#Метод огибающих
envelmet <- function(data){
  stand_data <- (data-mean(data))/sqrt(var(data))
  data.qq <- qqnorm(stand_data, plot.it = FALSE)
  data.qq <- lapply(data.qq, sort)
  plot(data.qq, 
       ylab = "Z-статистики выборки", 
       xlab = "Квантили нормального распределения")
}
envelmet(small_1)
envelmet(small_2)
envelmet(small_3)
envelmet(big_1)
envelmet(big_2)
envelmet(big_3)

#Критерий Колмогорова-Смирнова:
ks.test(small_1, "pnorm")
ks.test(small_2, "pnorm")
ks.test(small_3, "pnorm")
ks.test(big_1, "pnorm")
ks.test(big_2, "pnorm")
ks.test(big_3, "pnorm")

install.packages("nortest")
library(nortest)

#Критерий Шапиро-Уилка
shapiro.test(small_1)
shapiro.test(small_2)
shapiro.test(small_3)
shapiro.test(big_1)
shapiro.test(big_2)
shapiro.test(big_3)

#Критерий Андерсона-Дарлинга
ad.test(small_1)
ad.test(small_2)
ad.test(small_3)
ad.test(big_1)
ad.test(big_2)
ad.test(big_3)

#Критерий Крамера фон Мизеса
cvm.test(small_1)
cvm.test(small_2)
cvm.test(small_3)
cvm.test(big_1)
cvm.test(big_2)
cvm.test(big_3)

#Критерий Колмогорова-Смирнова в модификации Лиллиефорса
lillie.test(small_1)
lillie.test(small_2)
lillie.test(small_3)
lillie.test(big_1)
lillie.test(big_2)
lillie.test(big_3)

#Критерий Колмогорова-Смирнова в модификации Шапиро-Франсия
sf.test(small_1)
sf.test(small_2)
sf.test(small_3)
sf.test(big_1)
sf.test(big_2)
sf.test(big_3)

#Пример анализа данных с помощью графиков квантилей, метода огибающих, 
#а также стандартных процедур проверки гипотез о нормальности

#Выборка малого объёма — курящие и пьющие мужчины (32 наблюдения)
data_BMI_small <- df$BMI[df$Sex == "Male" & df$Smoking == "Yes" 
                         & df$AlcoholDrinking == "Yes" ]

#Выборка умеренного объёма — столбец с данными BMI (2500 наблюдений)
data_BMI <- df$BMI

#Графики квантилей
layout(matrix(1:1, ncol = 1))
quantile(data_BMI_small)
quantile(data_BMI)

#Метод огибающих
envelmet(data_BMI_small)
envelmet(data_BMI)

#Применим ранее введённые критерии для проверки гипотез о нормальности
ks.test(unique(data_BMI_small), "pnorm")
ks.test(unique(data_BMI), "pnorm")

shapiro.test(data_BMI_small)
shapiro.test(data_BMI)

ad.test(data_BMI_small)
ad.test(data_BMI)

cvm.test(data_BMI_small)
cvm.test(data_BMI)

lillie.test(data_BMI_small)
lillie.test(data_BMI)

sf.test(data_BMI_small)
sf.test(data_BMI)

#№7 (критерии Стьюдента, Уилкоксона-Манна-Уитни, 
#             Фишера, Левене, Бартлетта, Флигнера-Килина)

#Критерий Стьюдента
#Для применения критерия необходимо, 
#чтобы исходные данные имели нормальное распределение.
#Сгенерируем две нормально распределённые выборки:
norm_1 <- rnorm(50, 5, 4)
norm_2 <- rnorm(50, 2, 4)

#Одновыборочный тест
t.test(norm_1, mu = 6, conf.level = 0.9, alternative = 'greater')
t.test(norm_1, mu = 6, conf.level = 0.95, alternative = 'greater')
t.test(norm_1, mu = 6, conf.level = 0.99, alternative = 'greater')

#Гипотеза "Средняя первой выборки равна средней второй выборки"
t.test(norm_1, norm_2, conf.level = 0.9, alternative = 'two.sided')
t.test(norm_1, norm_2, conf.level = 0.95, alternative = 'two.sided')
t.test(norm_1, norm_2, conf.level = 0.99, alternative = 'two.sided')

#Гипотеза "Средняя второй выборки больше средней первой выборки"
t.test(norm_1, norm_2, conf.level = 0.9, alternative = 'greater')
t.test(norm_1, norm_2, conf.level = 0.95, alternative = 'greater')
t.test(norm_1, norm_2, conf.level = 0.99, alternative = 'greater')

#Оценка мощности критериев при заданном объёме выборки
power.t.test(n = 50, delta = 3, sd = 4, sig.level = 0.1)
power.t.test(n = 50, delta = 3, sd = 4, sig.level = 0.05)
power.t.test(n = 50, delta = 3, sd = 4, sig.level = 0.01)

#Определение объёма выборки для достижения заданной мощности
power.t.test(delta = 3, sd = 4, sig.level = 0.1, power = 0.98)
power.t.test(delta = 3, sd = 4, sig.level = 0.05, power = 0.96)
power.t.test(delta = 3, sd = 4, sig.level = 0.01, power = 0.87)

#Критерий Уилкоксона-Манна-Уитни
gamma_1 <- rgamma(40, 1, 1)
gamma_2 <- rgamma(40, 2, 3)

wilcox.test(gamma_1, gamma_2, conf.level = 0.9)
wilcox.test(gamma_1, gamma_2, conf.level = 0.95)
wilcox.test(gamma_1, gamma_2, conf.level = 0.99)

#Критерий Фишера
norm_1 <- rnorm(50, 5, 4)
norm_2 <- rnorm(50, 2, 4)
norm_3 <- rnorm(50, 2, 6)

var.test(norm_1, norm_2)
var.test(norm_1, norm_3)

#Критерии Левене, Бартлетта, Флигнера-Килина о равенстве дисперсий
install.packages("car")
library(car)
install.packages("stats")
library(stats)

leveneTest(df$BMI, as.factor(df$Sex))
bartlett.test(df$BMI, as.factor(df$Sex))
fligner.test(df$BMI, as.factor(df$Sex))

#Корреляционные взаимосвязи в данных
#Коэффициент корреляции Пирсона
cor.test(df$PhysicalHealth, df$MentalHealth)

#Коэффициент корреляции Спирмена
cor.test(df$PhysicalHealth, df$MentalHealth, method = "spearman")

#Коэффициент корреляции Кендалла
cor.test(df$PhysicalHealth, df$MentalHealth, method = "kendall")

#№9
men_smoking <- nrow(df[df$Sex == "Male" & df$Smoking == "Yes",])
men_drinking <- nrow(df[df$Sex == "Male" & df$AlcoholDrinking == "Yes",])
women_smoking <- nrow(df[df$Sex == "Female" & df$Smoking == "Yes",])
women_drinking <- nrow(df[df$Sex == "Female" & df$AlcoholDrinking == "Yes",])

bad_habit_matrix <- 
  matrix(c(men_smoking, men_drinking, women_smoking, women_drinking), 
         nrow = 2, byrow = TRUE,
         dimnames = list(c("Мужчины", "Женщины"), c("Курящие", "Пьющие")))

#Метод хи-квадрат
chisq.test(bad_habit_matrix) 

#Точный тест Фишера
fisher.test(bad_habit_matrix)

mcnemar_data <- df
#Составим таблицу сопряжённости по параметрам "Smoking" и "AlcoholDrinking":
ct <- table(mcnemar_data[,c("Smoking","AlcoholDrinking")])
ct

#Тест МакНемара
mcnemar.test(ct)

#Тест Кохрана-Мантеля-Хензеля
#Составим таблицы из курящих/пьющих мужчин/женщин по трём возрастным группам
men_smoking_18 <- nrow(df[df$Sex == "Male" & df$Smoking == "Yes" & df$AgeCategory == "18-24",])
men_drinking_18 <- nrow(df[df$Sex == "Male" & df$AlcoholDrinking == "Yes" & df$AgeCategory == "18-24",])
women_smoking_18 <- nrow(df[df$Sex == "Female" & df$Smoking == "Yes" & df$AgeCategory == "18-24",])
women_drinking_18 <- nrow(df[df$Sex == "Female" & df$AlcoholDrinking == "Yes" & df$AgeCategory == "18-24",])

men_smoking_40 <- nrow(df[df$Sex == "Male" & df$Smoking == "Yes" & df$AgeCategory == "40-44",])
men_drinking_40 <- nrow(df[df$Sex == "Male" & df$AlcoholDrinking == "Yes" & df$AgeCategory == "40-44",])
women_smoking_40 <- nrow(df[df$Sex == "Female" & df$Smoking == "Yes" & df$AgeCategory == "40-44",])
women_drinking_40 <- nrow(df[df$Sex == "Female" & df$AlcoholDrinking == "Yes" & df$AgeCategory == "40-44",])

men_smoking_60 <- nrow(df[df$Sex == "Male" & df$Smoking == "Yes" & df$AgeCategory == "60-64",])
men_drinking_60 <- nrow(df[df$Sex == "Male" & df$AlcoholDrinking == "Yes" & df$AgeCategory == "60-64",])
women_smoking_60 <- nrow(df[df$Sex == "Female" & df$Smoking == "Yes" & df$AgeCategory == "60-64",])
women_drinking_60 <- nrow(df[df$Sex == "Female" & df$AlcoholDrinking == "Yes" & df$AgeCategory == "60-64",])


three_dim_table <-
  array(c(men_smoking_18, men_drinking_18, women_smoking_18, women_drinking_18,
          men_smoking_40, men_drinking_40, women_smoking_40, women_drinking_40,
          men_smoking_60, men_drinking_60, women_smoking_60, women_drinking_60),
        dim = c(2, 2, 3),
        dimnames = list(
          Sex = c("Мужчины", "Женщины"),
          Habit = c("Курящие", "Пьющие"),
          AgeCatregory = c("18-24", "40-44", "60-64")))

mantelhaen.test(three_dim_table)

#№10
#Корреляционная матрица
install.packages("corrplot")
library(corrplot)

M <- cor(df[,c(2, 5, 6, 12)])
corrplot(M, method = 'number')

#Фактор инфляции дисперсии
vif(lm(BMI ~ PhysicalHealth + MentalHealth + SleepTime, data = df))


#№11 Дисперсионный анализ
tooth_data <- ToothGrowth
str(tooth_data)

shapiro.test(tooth_data$len)

#Однофакторный дисперсионный анализ
one_factor <- aov(len ~ as.factor(dose), data=ToothGrowth)
summary(one_factor)

TukeyHSD(one_factor)

#Двухфакторный дисперсионный анализ
two_factor <- aov(len ~ as.factor(dose) + supp + as.factor(dose):supp, 
                  data=ToothGrowth)
summary(two_factor)

TukeyHSD(two_factor)

#№12. Регрессионные модели

#Логистическая регрессия

log_regr <- disease_data[,-c(5,11,12,14,16,17,18)]
log_regr[log_regr == "Yes"] <- "1"
log_regr[log_regr == "No"] <- "0"
log_regr$HeartDisease <- as.numeric(log_regr$HeartDisease)
log_regr$Smoking <- as.numeric(log_regr$Smoking)
log_regr$AlcoholDrinking <- as.numeric(log_regr$AlcoholDrinking)
log_regr$DiffWalking <- as.numeric(log_regr$DiffWalking)
log_regr$PhysicalActivity <- as.numeric(log_regr$PhysicalActivity)

input <- log_regr[,-c(8, 9)]
heart.data <- glm(formula = HeartDisease ~ 
                    BMI + PhysicalHealth + MentalHealth + SleepTime +
                    Smoking + AlcoholDrinking + DiffWalking + PhysicalActivity, 
                  data = input, family = binomial)
summary(heart.data)

log_regr$prob <- predict(object=heart.data, type="response")
head(log_regr$prob)

#ROC-кривая

install.packages("ROCR")
library(ROCR)

pred_fit <- prediction(log_regr$prob, log_regr$HeartDisease)
perf_fit <- performance(pred_fit,"tpr","fpr")
plot(perf_fit, colorize=T , print.cutoffs.at = seq(0,1,by=0.1))
auc  <- performance(pred_fit, measure = "auc")
str(auc)







